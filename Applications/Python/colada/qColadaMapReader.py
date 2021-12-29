from PythonQt import *
from qColadaAppPythonQt import *
from h5gtpy import h5gt
from h5geopy import h5geo
import os, slicer, ctk
import numpy as np
from osgeo import gdal


class ReadWriteMapParam(h5geo.MapParam):
    """Contains data needed to create new `h5geo.H5Map` object. Inherits from `h5geo.MapParam`"""
    readFile = ''
    saveFile = ''
    mapName = ''
    mapCreateType = ''
    depthMult = -1
    xNorth = False


class qColadaMapReader(qColadaReader):
    """Class designated to read map files (via GDAL)

    Args:
        qColadaReader (PythonQt.QtGui.QObject): parent object

    Returns:
        qColadaReader: object instance
    """
    mapTableView = None
    mapHrzHeader = None
    mapVertHeader = None
    mapProxy = None
    mapModel = None

    buttonBox = None
    addToolBtn = None
    removeToolBtn = None
    autoDefineBtn = None
    vSplitter = None
    mainSplitter = None

    # delegates must exist during object's lifetime or application will be break down!
    map_save_to_pathEditDelegate = qPathEditDelegate()
    map_create_comboDelegate = qComboBoxDelegate()
    map_x0_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    map_y0_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    map_x1_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    map_y1_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    map_x2_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    map_y2_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    map_nx_spinBoxDelegate = qSpinBoxDelegate()
    map_ny_spinBoxDelegate = qSpinBoxDelegate()
    map_name_lineEditDelegate = qLineEditDelegate()
    map_domain_comboDelegate = qComboBoxDelegate()
    map_depthMult_spinBoxDelegate = qSpinBoxDelegate()

    validator = qREValidator()
    validator_noSpaces = qREValidator()

    mapTableHdrNames = [
        "read file", "save to", "CRS",
        "map name", "map create",
        "domain", "length units", "data units",
        "x0", "y0", "x1", "y1", "x2", "y2", "nx", "ny",
        "depth mult",
        "XNorth"]

    mapTableHdrTips = [
        "Read file", "Container where to save data", "Coordinate reference system",
        "Map name", "Creation type for map",
        "Domain", "Length units", "Data units",
        "Top left point: x", "Top left point: y",
        "Top right point: x", "Top right point: y",
        "Bottom left point: x", "Bottom left point: y",
        "Number of x points",
        "Number of y points",
        "Depth multiplier: downwards is negative (usually -1)",
        "`X` axis points to the North? checked - True, unchecked - False"]

    def __init__(self, parent=None):
        super(qColadaReader, self).__init__(parent)
        self.initGUIVars()
        self.initMapTable()
        self.addToolBtn.clicked.connect(self.onAddBtnClicked)
        self.removeToolBtn.clicked.connect(self.onRemoveToolBtnClicked)
        self.autoDefineBtn.clicked.connect(self.onAutoDefineToolBtnClicked)
        self.buttonBox.clicked.connect(self.onButtonBoxClicked)

    def initGUIVars(self):
        self.mapTableView = slicer.util.findChild(self, 'TableView')
        self.mapHrzHeader = slicer.util.findChild(self.mapTableView, 'HrzHeader')
        self.mapVertHeader = slicer.util.findChild(self.mapTableView, 'VertHeader')
        self.mapProxy = self.mapTableView.model()
        self.mapModel = self.mapProxy.sourceModel
        self.buttonBox = slicer.util.findChild(self, 'ButtonBox')
        self.addToolBtn = slicer.util.findChild(self, 'AddToolBtn')
        self.removeToolBtn = slicer.util.findChild(self, 'RemoveToolBtn')
        self.autoDefineBtn = slicer.util.findChild(self, 'AutoDefineBtn')
        self.vSplitter = slicer.util.findChild(self, 'VSplitter')
        self.mainSplitter = slicer.util.findChild(self, 'MainSplitter')

    def initMapTable(self):
        self.mapModel.setColumnCount(len(self.mapTableHdrNames))
        self.mapModel.setHorizontalHeaderLabels(self.mapTableHdrNames)
        for i in range(0, self.mapModel.columnCount()):
            self.mapModel.horizontalHeaderItem(i).setToolTip(self.mapTableHdrTips[i])

        self.map_save_to_pathEditDelegate.setParent(self.mapTableView)
        self.map_create_comboDelegate.setParent(self.mapTableView)
        self.map_domain_comboDelegate.setParent(self.mapTableView)

        self.map_x0_scienceSpinBoxDelegate.setParent(self.mapTableView)
        self.map_y0_scienceSpinBoxDelegate.setParent(self.mapTableView)
        self.map_x1_scienceSpinBoxDelegate.setParent(self.mapTableView)
        self.map_y1_scienceSpinBoxDelegate.setParent(self.mapTableView)
        self.map_x2_scienceSpinBoxDelegate.setParent(self.mapTableView)
        self.map_y2_scienceSpinBoxDelegate.setParent(self.mapTableView)
        self.map_nx_spinBoxDelegate.setParent(self.mapTableView)
        self.map_ny_spinBoxDelegate.setParent(self.mapTableView)

        self.map_depthMult_spinBoxDelegate.setParent(self.mapTableView)
        self.map_depthMult_spinBoxDelegate.setStep(2)
        self.map_depthMult_spinBoxDelegate.setMinValue(-1)
        self.map_depthMult_spinBoxDelegate.setMaxValue(1)

        self.map_name_lineEditDelegate.setParent(self.mapTableView)

        self.validator.setParent(self.mapTableView)
        self.validator.setRegularExpression(Util.fileNameRegExp())
        self.validator.setToolTipText(Util.fileNameToolTipText())
        self.validator.setToolTipDuration(3000)

        self.map_name_lineEditDelegate.setValidator(self.validator)

        self.mapTableView.setItemDelegateForColumn(
            self.mapTableHdrNames.index("save to"), self.map_save_to_pathEditDelegate)

        self.map_create_comboDelegate.setTexts(list(h5geo.CreationType.__members__.keys()))
        self.mapTableView.setItemDelegateForColumn(
            self.mapTableHdrNames.index("map create"), self.map_create_comboDelegate)
        self.map_domain_comboDelegate.setTexts(list(h5geo.Domain.__members__.keys()))
        self.mapTableView.setItemDelegateForColumn(
            self.mapTableHdrNames.index("domain"), self.map_domain_comboDelegate)

        self.mapTableView.setItemDelegateForColumn(
            self.mapTableHdrNames.index("x0"), self.map_x0_scienceSpinBoxDelegate)
        self.mapTableView.setItemDelegateForColumn(
            self.mapTableHdrNames.index("y0"), self.map_y0_scienceSpinBoxDelegate)
        self.mapTableView.setItemDelegateForColumn(
            self.mapTableHdrNames.index("x1"), self.map_x1_scienceSpinBoxDelegate)
        self.mapTableView.setItemDelegateForColumn(
            self.mapTableHdrNames.index("y1"), self.map_y1_scienceSpinBoxDelegate)
        self.mapTableView.setItemDelegateForColumn(
            self.mapTableHdrNames.index("x2"), self.map_x2_scienceSpinBoxDelegate)
        self.mapTableView.setItemDelegateForColumn(
            self.mapTableHdrNames.index("y2"), self.map_y2_scienceSpinBoxDelegate)

        self.mapTableView.setItemDelegateForColumn(
            self.mapTableHdrNames.index("depth mult"), self.map_depthMult_spinBoxDelegate)

        self.mapTableView.setItemDelegateForColumn(
            self.mapTableHdrNames.index("map name"), self.map_name_lineEditDelegate)

        # connect to update checkstate from selected rows
        self.mapModel.dataChanged.connect(self.onMapModeDataChanged)

    def onMapModeDataChanged(self, topLeft, bottomRight, roles):
        if Qt.Qt.CheckStateRole not in roles:
            return
        selectionModel = self.mapTableView.selectionModel()
        if not selectionModel:
            return
        item = self.mapModel.itemFromIndex(topLeft)
        if not item:
            return
        checkState = item.checkState()
        self.mapModel.dataChanged.disconnect(self.onMapModeDataChanged)
        for index in selectionModel.selectedIndexes:
            item = self.mapModel.item(index.row(), index.column())
            if not item:
                continue
            item.setCheckState(checkState)
        self.mapModel.dataChanged.connect(self.onMapModeDataChanged)

    def getReadWriteMapParamFromTable(self, m_proxy_row: int) -> ReadWriteMapParam:
        """Read data needed to open/create `h5geo.H5Map` object.

        Args:
            m_proxy_row (int): map table's proxy model row number

        Returns:
            ReadWriteMapParam: filled with values instance
        """
        m_row = self.mapProxy.mapToSource(self.mapProxy.index(m_proxy_row, 0)).row()
        p = ReadWriteMapParam()

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("read file")))
        p.readFile = tmp if tmp else ''

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("save to")))
        p.saveFile = tmp if tmp else ''

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("CRS")))
        p.spatialReference = tmp if tmp else ''

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("map name")))
        p.mapName = tmp if tmp else ''

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("map create")))
        p.mapCreateType = h5geo.CreationType.__members__[tmp] if tmp in h5geo.CreationType.__members__ else h5geo.CreationType(0)

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("domain")))
        p.domain = h5geo.Domain.__members__[tmp] if tmp in h5geo.Domain.__members__ else h5geo.Domain(0)

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("length units")))
        p.lengthUnits = tmp if tmp else ''

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("data units")))
        p.dataUnits = tmp if tmp else ''

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("x0")))
        p.X0 = float(tmp) if tmp else np.nan

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("y0")))
        p.Y0 = float(tmp) if tmp else np.nan

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("x1")))
        p.X1 = float(tmp) if tmp else np.nan

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("y1")))
        p.Y1 = float(tmp) if tmp else np.nan

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("x2")))
        p.X2 = float(tmp) if tmp else np.nan

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("y2")))
        p.Y2 = float(tmp) if tmp else np.nan

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("nx")))
        p.nX = int(tmp) if tmp else 0

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("ny")))
        p.nY = int(tmp) if tmp else 0

        tmp = self.mapProxy.data(
            self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("depth mult")))
        p.depthMult = int(tmp) if tmp else 1

        tmp = self.mapTableView.item(m_row, self.mapTableHdrNames.index("XNorth")).checkState()
        p.xNorth = True if tmp == Qt.Qt.Checked else False

        return p

    def updateMapTableRow(self, m_proxy_row: int):
        """Resets map table's row and then updates map table.
        Fills map table as much as it can.

        Args:
            m_proxy_row (int): map table's proxy model row number
        """
        s_model_row = self.mapProxy.mapToSource(self.mapProxy.index(m_proxy_row, 0)).row()

        self.resetMapTableRow(m_proxy_row)

        readFile = self.mapProxy.data(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("read file")))

        map_name = os.path.splitext(os.path.basename(readFile))[0]
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("map name")), map_name)

        ds = gdal.Open(readFile, gdal.GA_ReadOnly)
        if not ds:
            return

        # from here:
        # https://gis.stackexchange.com/questions/57834/how-to-get-raster-corner-coordinates-using-python-gdal-bindings#:~:text=This%20can%20be%20done%20in%20far%20fewer%20lines%20of%20code
#        ulx, xres, xskew, uly, yskew, yres = ds.GetGeoTransform()
#        lrx = ulx + (ds.RasterXSize * xres)
#        lry = uly + (ds.RasterYSize * yres)

        # from here:
        # https://gdal.org/tutorials/geotransforms_tut.html
        GT = ds.GetGeoTransform()
        x0 = GT[0]
        y0 = GT[3]
        x1 = GT[0] + 0*GT[1] + ds.RasterYSize*GT[2]
        y1 = GT[3] + 0*GT[4] + ds.RasterYSize*GT[5]
        x2 = GT[0] + ds.RasterXSize*GT[1] + 0*GT[2]
        y2 = GT[3] + ds.RasterXSize*GT[4] + 0*GT[5]

        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("x0")), x0)
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("y0")), y0)
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("x1")), x1)
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("y1")), y1)
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("x2")), x2)
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("y2")), y2)
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("nx")), ds.RasterXSize)
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("ny")), ds.RasterYSize)

        nx_item = QtGui.QStandardItem(str(ds.RasterXSize))
        nx_item.setFlags(nx_item.flags() & ~Qt.Qt.ItemIsEditable)
        self.mapModel.setItem(s_model_row, self.mapTableHdrNames.index("nx"), nx_item)

        ny_item = QtGui.QStandardItem(str(ds.RasterYSize))
        ny_item.setFlags(ny_item.flags() & ~Qt.Qt.ItemIsEditable)
        self.mapModel.setItem(s_model_row, self.mapTableHdrNames.index("ny"), ny_item)

    def resetMapTableRow(self, m_proxy_row: int):
        """Set some default values to map table.

        Args:
            m_proxy_row (int): map table's proxy model row number
        """
        readFile = self.mapProxy.data(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("read file")))
        fi = QtCore.QFileInfo(readFile);
        if not fi.exists():
            return

        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("save to")),
                    Util.defaultMapDir() + "/" + fi.baseName() + ".h5")
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("CRS")),
                    Util.CRSAuthName() + ":" + str(Util.CRSCode()))
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("map create")),
            str(h5geo.CreationType.OPEN_OR_CREATE).rsplit('.', 1)[-1])
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("domain")),
            str(h5geo.Domain.TVDSS).rsplit('.', 1)[-1])
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("length units")), 'meter')
        self.mapProxy.setData(self.mapProxy.index(m_proxy_row, self.mapTableHdrNames.index("depth mult")), str(-1))

        m_row = self.mapProxy.mapToSource(self.mapProxy.index(m_proxy_row, 0)).row()

        xNorthItem = QtGui.QStandardItem()
        xNorthItem.setCheckable(True)
        xNorthItem.setEditable(False)
        self.mapModel.setItem(m_row, self.mapTableHdrNames.index("XNorth"), xNorthItem)

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

        fileNames = ctk.ctkFileDialog.getOpenFileNames(None, 'Select one or more map files to open', '', qt_styled_extension_filter)

        QtGui.QApplication.setOverrideCursor(QtCore.Qt.BusyCursor)
        for name in fileNames:
            if self.mapModel.findItems(name, Qt.Qt.MatchFixedString, self.mapTableHdrNames.index("read file")):
                QtGui.QMessageBox.warning(self, "Warning", name+": is already in table!");
                continue

            self.mapModel.insertRow(self.mapModel.rowCount())
            row = self.mapModel.rowCount()-1

            readFileItem = QtGui.QStandardItem(name)
            readFileItem.setFlags(readFileItem.flags() & ~Qt.Qt.ItemIsEditable)
            self.mapModel.setItem(row, self.mapTableHdrNames.index("read file"), readFileItem)

            for row in range(self.mapModel.rowCount()):
                self.mapModel.verticalHeaderItem(row).setText(str(row + 1))

            self.updateMapTableRow(row)
        QtGui.QApplication.restoreOverrideCursor()

    def onRemoveToolBtnClicked(self):
        """Removes selected rows from map model."""
        indexList = self.mapTableView.selectionModel().selectedRows()

        rowList = []
        for index in indexList:
            rowList.append(index.row())

        # to remove rows correctly we should remove them in descending order
        rowList.sort(reverse = True)

        for row in rowList:
            self.mapProxy.removeRow(row)

        for row in range(self.mapModel.rowCount()):
            self.mapModel.verticalHeaderItem(row).setText(str(row + 1))

    def onAutoDefineToolBtnClicked(self):
        """Calls `updateMapTableRow` for selected map table's rows."""
        indexList = self.mapTableView.selectionModel().selectedRows()
        QtGui.QApplication.setOverrideCursor(QtCore.Qt.BusyCursor)
        for index in indexList:
            self.updateMapTableRow(index.row())
        QtGui.QApplication.restoreOverrideCursor()

    def onButtonBoxClicked(self, button):
        if button == self.buttonBox.button(QtGui.QDialogButtonBox.Ok):
            QtGui.QApplication.setOverrideCursor(QtCore.Qt.BusyCursor)
            progressDialog = slicer.util.createProgressDialog(
                parent=self, maximum=self.mapModel.rowCount())
            for row in range(self.mapModel.rowCount()):
                p_s = self.getReadWriteMapParamFromTable(row)
                progressDialog.setLabelText("Reading: " + p_s.readFile)
                progressDialog.setValue(row)
                try:
                    h5mapCnt = h5geo.createMapContainerByName(p_s.saveFile, h5geo.CreationType.OPEN_OR_CREATE)

                    if not h5mapCnt:
                        errMsg = 'Can`t open or create: ' + p_s.saveFile + '''
                        Possible reasons:
                        - `save to` is incorrect;
                        - `save to` points to an existing NON MAP CONTAINER file;
                        - you don't have permissions to create files in destination folder;
                        '''
                        QtGui.QMessageBox.critical(self, "Error", errMsg)
                        continue

                    if p_s.xNorth:
                        p_s.X0, p_s.Y0 = p_s.Y0, p_s.X0
                        p_s.X1, p_s.Y1 = p_s.Y1, p_s.X1
                        p_s.X2, p_s.Y2 = p_s.Y2, p_s.X2
                        p_s.nX, p_s.nY = p_s.nY, p_s.nX

                    h5map = h5mapCnt.createMap(p_s.mapName, p_s, p_s.mapCreateType)
                    if not h5map:
                        errMsg = 'Can`t open, create or overwrite map: ' + p_s.mapName + ' from container: ' + p_s.saveFile + '''
                        Possible reasons:
                        - `map name` is incorrect;
                        - `map create` is incorrect;
                        - map container contains a group with the same name `map name` but this group can`t be treated as map and `map create` flag is `OPEN/CREATE/OPEN_OR_CREATE`;
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
                    if arr.ndim > 2:
                        errMsg = 'Multidimensional data: ' + p_s.readFile + '''
                        Currently ndims > 2 is not supported
                        '''
                        QtGui.QMessageBox.critical(self, "Error", errMsg)
                        continue

                    if p_s.xNorth:
                        val = h5map.writeData(np.transpose(arr) * p_s.depthMult, p_s.dataUnits)
                    else:
                        val = h5map.writeData(arr * p_s.depthMult, p_s.dataUnits)

                    if not val:
                        errMsg = 'Can`t write data to map: ' + p_s.mapName + ' from container: ' + p_s.saveFile + '''
                        Possible reasons:
                        - `map_data` dataset is broken (some attributes are missing);
                        - you don't have write permissions inside specified container (maybe 3rd party app is blocking this container);
                        '''
                        QtGui.QMessageBox.critical(self, "Error", errMsg)
                        continue

                    if progressDialog.wasCanceled:
                        break

                    h5mapCnt.getH5File().flush()

                except h5gt.Exception as ex:
                    QtGui.QMessageBox.critical(self, "Error", ex)
                    # print(ex)
                    continue

            progressDialog.setValue(self.mapModel.rowCount())
            QtGui.QApplication.restoreOverrideCursor()

        elif button == self.buttonBox.button(QtGui.QDialogButtonBox.Cancel):
            self.close()
