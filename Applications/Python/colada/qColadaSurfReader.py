from PythonQt import *
from qColadaAppPythonQt import *
from h5gtpy import h5gt
from h5geopy import h5geo
import os, slicer, ctk
import numpy as np
from osgeo import gdal


class ReadWriteSurfParam(h5geo.SurfParam):
    """Contains data needed to create new `h5geo.H5Surf` object. Inherits from `h5geo.SurfParam`"""
    readFile = ''
    saveFile = '' 
    crs = '' 
    surfName = ''
    surfCreateType = ''
    depthMult = -1
    xNorth = False


class qColadaSurfReader(qColadaReader):
    """Class designated to read surface files (via GDAL)

    Args:
        qColadaReader (PythonQt.QtGui.QObject): parent object

    Returns:
        qColadaReader: object instance
    """
    surfTableView = None
    surfHrzHeader = None
    surfVertHeader = None
    surfProxy = None
    surfModel = None

    buttonBox = None
    addToolBtn = None
    removeToolBtn = None
    autoDefineBtn = None
    vSplitter = None
    mainSplitter = None

    # delegates must exist during object's lifetime or application will be break down!
    surf_save_to_pathEditDelegate = qPathEditDelegate()
    surf_create_comboDelegate = qComboBoxDelegate()
    surf_x0_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    surf_y0_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    surf_dx_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    surf_dy_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    surf_nx_spinBoxDelegate = qSpinBoxDelegate()
    surf_ny_spinBoxDelegate = qSpinBoxDelegate()
    surf_name_lineEditDelegate = qLineEditDelegate()
    surf_domain_comboDelegate = qComboBoxDelegate()
    surf_depthMult_spinBoxDelegate = qSpinBoxDelegate()

    validator = qREValidator()
    validator_noSpaces = qREValidator()

    surfTableHdrNames = [
        "read file", "save to", "CRS",
        "surf name", "surf create", 
        "domain", "spatial units", "data units",
        "x0", "y0", "dx", "dy", "nx", "ny", 
        "depth mult",
        "XNorth"]

    surfTableHdrTips = [
        "Read file", "Container where to save data", "CRS authority name and code (example: EPSG:2000). . Must be set if new surf is going to be created",
        "Surf name", "Creation type for surf", 
        "Domain", "Spatial units", "Data units",
        "Starting point - x0", "Starting point - y0", "Spacing dx", "Spacing dy", "Number of x points", "Number of y points",
        "Depth multiplier: downwards is negative (usually -1)",
        "`X` axis points to the North? checked - True, unchecked - False"]

    def __init__(self, parent=None):
        super(qColadaReader, self).__init__(parent)
        self.initGUIVars()
        self.initSurfTable()
        self.addToolBtn.clicked.connect(self.onAddBtnClicked)
        self.removeToolBtn.clicked.connect(self.onRemoveToolBtnClicked)
        self.autoDefineBtn.clicked.connect(self.onAutoDefineToolBtnClicked)
        self.buttonBox.clicked.connect(self.onButtonBoxClicked)

    def initGUIVars(self):
        self.surfTableView = slicer.util.findChild(self, 'TableView')
        self.surfHrzHeader = slicer.util.findChild(self.surfTableView, 'HrzHeader')
        self.surfVertHeader = slicer.util.findChild(self.surfTableView, 'VertHeader')
        self.surfProxy = self.surfTableView.model()
        self.surfModel = self.surfProxy.sourceModel
        self.buttonBox = slicer.util.findChild(self, 'ButtonBox')
        self.addToolBtn = slicer.util.findChild(self, 'AddToolBtn')
        self.removeToolBtn = slicer.util.findChild(self, 'RemoveToolBtn')
        self.autoDefineBtn = slicer.util.findChild(self, 'AutoDefineBtn')
        self.vSplitter = slicer.util.findChild(self, 'VSplitter')
        self.mainSplitter = slicer.util.findChild(self, 'MainSplitter')

    def initSurfTable(self):
        self.surfModel.setColumnCount(len(self.surfTableHdrNames))
        self.surfModel.setHorizontalHeaderLabels(self.surfTableHdrNames)
        for i in range(0, self.surfModel.columnCount()):
            self.surfModel.horizontalHeaderItem(i).setToolTip(self.surfTableHdrTips[i])

        self.surf_save_to_pathEditDelegate.setParent(self.surfTableView)
        self.surf_create_comboDelegate.setParent(self.surfTableView)
        self.surf_domain_comboDelegate.setParent(self.surfTableView)
        
        self.surf_x0_scienceSpinBoxDelegate.setParent(self.surfTableView)
        self.surf_y0_scienceSpinBoxDelegate.setParent(self.surfTableView)
        self.surf_dx_scienceSpinBoxDelegate.setParent(self.surfTableView)
        self.surf_dy_scienceSpinBoxDelegate.setParent(self.surfTableView)
        self.surf_nx_spinBoxDelegate.setParent(self.surfTableView)
        self.surf_ny_spinBoxDelegate.setParent(self.surfTableView)
        
        self.surf_depthMult_spinBoxDelegate.setParent(self.surfTableView)
        self.surf_depthMult_spinBoxDelegate.setStep(2)
        self.surf_depthMult_spinBoxDelegate.setMinValue(-1)
        self.surf_depthMult_spinBoxDelegate.setMaxValue(1)
        
        self.surf_name_lineEditDelegate.setParent(self.surfTableView)

        self.validator.setParent(self.surfTableView)
        self.validator.setRegularExpression(ColadaUtil_py().fileNameRegExp())
        self.validator.setToolTipText(ColadaUtil_py().fileNameToolTipText())
        self.validator.setToolTipDuration(3000)

        self.surf_name_lineEditDelegate.setValidator(self.validator)

        self.surfTableView.setItemDelegateForColumn(
            self.surfTableHdrNames.index("save to"), self.surf_save_to_pathEditDelegate)

        self.surf_create_comboDelegate.setTexts(list(h5geo.CreationType.__members__.keys()))
        self.surfTableView.setItemDelegateForColumn(
            self.surfTableHdrNames.index("surf create"), self.surf_create_comboDelegate)
        self.surf_domain_comboDelegate.setTexts(list(h5geo.Domain.__members__.keys()))
        self.surfTableView.setItemDelegateForColumn(
            self.surfTableHdrNames.index("domain"), self.surf_domain_comboDelegate)
        
        self.surfTableView.setItemDelegateForColumn(
            self.surfTableHdrNames.index("depth mult"), self.surf_depthMult_spinBoxDelegate)
        
        self.surfTableView.setItemDelegateForColumn(
            self.surfTableHdrNames.index("surf name"), self.surf_name_lineEditDelegate)

    def getReadWriteSurfParamFromTable(self, s_proxy_row: int) -> ReadWriteSurfParam:
        """Read data needed to open/create `h5geo.H5Surf` object. 

        Args:
            s_proxy_row (int): surf table's proxy model row number

        Returns:
            ReadWriteSurfParam: filled with values instance
        """
        p = ReadWriteSurfParam()

        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("read file")))
        p.readFile = tmp if tmp else ''

        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("save to")))
        p.saveFile = tmp if tmp else ''

        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("CRS")))
        p.crs = tmp if tmp else ''

        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("surf name")))
        p.surfName = tmp if tmp else ''

        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("surf create")))
        p.surfCreateType = h5geo.CreationType.__members__[tmp] if tmp in h5geo.CreationType.__members__ else h5geo.CreationType(0)
        
        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("domain")))
        p.domain = h5geo.Domain.__members__[tmp] if tmp in h5geo.Domain.__members__ else h5geo.Domain(0)
        
        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("spatial units")))
        p.spatialUnits = tmp if tmp else ''

        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("data units")))
        p.dataUnits = tmp if tmp else ''

        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("x0")))
        p.X0 = float(tmp) if tmp else np.nan

        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("y0")))
        p.Y0 = float(tmp) if tmp else np.nan
        
        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("dx")))
        p.dX = float(tmp) if tmp else np.nan

        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("dy")))
        p.dY = float(tmp) if tmp else np.nan
        
        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("nx")))
        p.nX = int(tmp) if tmp else 0

        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("ny")))
        p.nY = int(tmp) if tmp else 0
        
        tmp = self.surfProxy.data(
            self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("depth mult")))
        p.depthMult = int(tmp) if tmp else 1
        
        tmp = self.surfTableView.indexWidget(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("XNorth"))).checkState()
        p.xNorth = True if tmp == Qt.Qt.Checked else False

        return p

    def updateSurfTableRow(self, s_proxy_row: int):
        """Resets surf table's row and then updates surf table. 
        Fills surf table as much as it can.

        Args:
            s_proxy_row (int): surf table's proxy model row number
        """
        s_model_row = self.surfProxy.mapToSource(self.surfProxy.index(s_proxy_row, 0)).row()
        
        self.resetSurfTableRow(s_proxy_row)

        readFile = self.surfProxy.data(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("read file")))

        surf_name = os.path.splitext(os.path.basename(readFile))[0]
        self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("surf name")), surf_name)
        
        ds = gdal.Open(readFile, gdal.GA_ReadOnly)
        if not ds:
            return
            
        # from here:  
        # https://gis.stackexchange.com/questions/57834/how-to-get-raster-corner-coordinates-using-python-gdal-bindings#:~:text=This%20can%20be%20done%20in%20far%20fewer%20lines%20of%20code
        ulx, xres, xskew, uly, yskew, yres  = ds.GetGeoTransform()
        lrx = ulx + (ds.RasterXSize * xres)
        lry = uly + (ds.RasterYSize * yres)
        self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("x0")), ulx)
        self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("y0")), uly)
        self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("dx")), xres)
        self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("dy")), yres)
        # self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("nx")), ds.RasterXSize)
        # self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("ny")), ds.RasterYSize)
        
        nx_item = QtGui.QStandardItem(str(ds.RasterXSize))
        nx_item.setFlags(nx_item.flags() & ~Qt.Qt.ItemIsEditable)
        self.surfModel.setItem(s_model_row, self.surfTableHdrNames.index("nx"), nx_item)
        
        ny_item = QtGui.QStandardItem(str(ds.RasterYSize))
        ny_item.setFlags(ny_item.flags() & ~Qt.Qt.ItemIsEditable)
        self.surfModel.setItem(s_model_row, self.surfTableHdrNames.index("ny"), ny_item)

    def resetSurfTableRow(self, s_proxy_row: int):
        """Set some default values to surf table.

        Args:
            s_proxy_row (int): surf table's proxy model row number
        """
        readFile = self.surfProxy.data(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("read file")))
        fi = QtCore.QFileInfo(readFile);
        if not fi.exists():
            return

        self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("save to")),
                    ColadaDBCore_py().getSurfDir() + "/" + fi.baseName() + ".h5")
        self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("CRS")),
                    ColadaDBCore_py().getCurrentProjectionNameCode())
        self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("surf create")), 
            str(h5geo.CreationType.OPEN_OR_CREATE).rsplit('.', 1)[-1])
        self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("domain")), 
            str(h5geo.Domain.TVDSS).rsplit('.', 1)[-1])
        self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("spatial units")), 'meter')
        self.surfProxy.setData(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("depth mult")), str(-1))
        self.surfTableView.setIndexWidget(self.surfProxy.index(s_proxy_row, self.surfTableHdrNames.index("XNorth")), QtGui.QCheckBox())

    def onAddBtnClicked(self):
        long_name_list = []
        extension_list = []
        for i in range(gdal.GetDriverCount()):
            drv = gdal.GetDriver(i)
            if drv.GetMetadataItem(gdal.DCAP_RASTER):
                long_name_list.append(drv.GetMetadataItem(gdal.DMD_LONGNAME))
                extension_list.append(drv.GetMetadataItem(gdal.DMD_EXTENSIONS))
        
        qt_styled_extension_filter = ''
        for i in range(len(long_name_list)):
            qt_styled_extension_filter += long_name_list[i] + ' ('
            if extension_list[i]:
                for ext in extension_list[i].split():
                    qt_styled_extension_filter += ' *.' + ext
            else:
                qt_styled_extension_filter += '*.*'
            qt_styled_extension_filter += ');; '
            
        qt_styled_extension_filter += 'all (*.*)'
        
        fileNames = ctk.ctkFileDialog.getOpenFileNames(None, 'Select one or more surface files to open', '', qt_styled_extension_filter)

        for name in fileNames:
            if self.surfModel.findItems(name, Qt.Qt.MatchFixedString, self.surfTableHdrNames.index("read file")):
                QtGui.QMessageBox.warning(self, "Warning", name+": is already in table!");
                continue

            self.surfModel.insertRow(self.surfModel.rowCount())
            row = self.surfModel.rowCount()-1

            readFileItem = QtGui.QStandardItem(name)
            readFileItem.setFlags(readFileItem.flags() & ~Qt.Qt.ItemIsEditable)
            self.surfModel.setItem(row, self.surfTableHdrNames.index("read file"), readFileItem)

            for row in range(self.surfModel.rowCount()):
                self.surfModel.verticalHeaderItem(row).setText(str(row + 1))

            self.updateSurfTableRow(row);

    def onRemoveToolBtnClicked(self):
        """Removes selected rows from surf model."""
        indexList = self.surfTableView.selectionModel().selectedRows()

        rowList = []
        for index in indexList:
            rowList.append(index.row())

        # to remove rows correctly we should remove them in descending order
        rowList.sort(reverse = True)

        for row in rowList:
            self.surfProxy.removeRow(row)

        for row in range(self.surfModel.rowCount()):
            self.surfModel.verticalHeaderItem(row).setText(str(row + 1))
            
    def onAutoDefineToolBtnClicked(self):
        """Calls `updateSurfTableRow` for selected surf table's rows."""
        indexList = self.surfTableView.selectionModel().selectedRows()
        for index in indexList:
            self.updateSurfTableRow(index.row())

    def onButtonBoxClicked(self, button):
        if button == self.buttonBox.button(QtGui.QDialogButtonBox.Ok):
            # to accelerate this code I reserve a var (`getCurrentProjectUnits()` invokes SQLITE wich is slow)
            currentProjectUnits = ColadaDBCore_py().getCurrentProjectUnits()
            progressDialog = slicer.util.createProgressDialog(
                parent=self, maximum=self.surfModel.rowCount())
            for row in range(self.surfModel.rowCount()):
                p_s = self.getReadWriteSurfParamFromTable(row)
                progressDialog.setLabelText("Reading: " + p_s.readFile)
                progressDialog.setValue(row)
                try:
                    h5surfCnt = h5geo.createSurfContainerByName(p_s.saveFile, h5geo.CreationType.OPEN_OR_CREATE)

                    if not h5surfCnt:
                        errMsg = 'Can`t open or create: ' + p_s.saveFile + ''' 
                        Possible reasons:
                        - `save to` is incorrect;
                        - `save to` points to an existing NON SURF CONTAINER file;
                        - you don't have permissions to create files in destination folder;
                        '''
                        QtGui.QMessageBox.critical(self, "Error", errMsg)
                        continue
                    
                    if p_s.xNorth:
                        p_s.X0, p_s.Y0 = p_s.Y0, p_s.X0
                        p_s.dX, p_s.dY = p_s.dY, p_s.dX
                        p_s.nX, p_s.nY = p_s.nY, p_s.nX
                        
                    p_s.X0, p_s.Y0, val = ColadaDBCore_py().convCoord2CurrentProjection(p_s.X0, p_s.Y0, p_s.crs, p_s.spatialUnits)
                    
                    # if new surf will be created then the units will be `p_s.spatialUnits`
                    coef = ColadaUtil_py().convertUnits(
                        currentProjectUnits,
                        p_s.spatialUnits)
                    
                    p_s.X0 *= coef
                    p_s.Y0 *= coef
                    p_s.dX *= coef
                    p_s.dY *= coef
                    
                    if not val:
                        errMsg = 'Can`t transform coordinates from: ' + p_s.crs + ' to: ' + ColadaDBCore_py().getCurrentProjectionNameCode() + ''' 
                        Possible reasons:
                        - project is not set or contains incorrect CRS;
                        - `CRS` is incorrect;
                        '''
                        QtGui.QMessageBox.critical(self, "Error", errMsg)
                        continue
                    
                    h5surf = h5surfCnt.createSurf(p_s.surfName, p_s, p_s.surfCreateType)
                    if not h5surf:
                        errMsg = 'Can`t open, create or overwrite surf: ' + p_s.surfName + ' from container: ' + p_s.saveFile + ''' 
                        Possible reasons:
                        - `surf name` is incorrect;
                        - `surf create` is incorrect;
                        - surf container contains a group with the same name `surf name` but this group can`t be treated as surf and `surf create` flag is `OPEN/CREATE/OPEN_OR_CREATE`;
                        - you don't have permissions to create objects inside specified container (maybe 3rd party app is blocking this container);
                        '''
                        QtGui.QMessageBox.critical(self, "Error", errMsg)
                        continue

                    ds = gdal.Open(p_s.readFile, gdal.GA_ReadOnly)
                    if not ds:
                        errMsg = 'GDAL library can`t open `read file`: ' + p_s.readFile + ''' 
                        Possible reasons:
                        - file format is not supported by current GDAL version;
                        - file is broken;
                        '''
                        QtGui.QMessageBox.critical(self, "Error", errMsg)
                        continue
                    
                    arr = np.asfortranarray(ds.ReadAsArray(), dtype=float)
                    if p_s.xNorth:
                        val = h5surf.writeData(np.transpose(arr) * p_s.depthMult, p_s.dataUnits)
                    else:
                        val = h5surf.writeData(arr * p_s.depthMult, p_s.dataUnits)
                        
                    if not val:
                        errMsg = 'Can`t write data to surf: ' + p_s.surfName + ' from container: ' + p_s.saveFile + ''' 
                        Possible reasons:
                        - `surf_data` dataset is broken (some attributes are missing);
                        - you don't have write permissions inside specified container (maybe 3rd party app is blocking this container);
                        '''
                        QtGui.QMessageBox.critical(self, "Error", errMsg)
                        continue

                    if progressDialog.wasCanceled:
                        break
                        
                    h5surfCnt.getH5File().flush()

                except h5gt.Exception as ex:
                    QtGui.QMessageBox.critical(self, "Error", ex)
                    # print(ex)
                    continue

            progressDialog.setValue(self.surfModel.rowCount())

        elif button == self.buttonBox.button(QtGui.QDialogButtonBox.Cancel):
            self.close()
