from PythonQt import *
from qColadaAppPythonQt import *
from h5gtpy import h5gt
from h5geopy import h5geo
from welly import Well
import os, slicer, ctk
import numpy as np

class ReadWriteWellParam(h5geo.WellParam):
    """Contains data needed to create new `h5geo.H5Well` object. Inherits from `h5geo.WellParam`"""
    readFile = ''
    saveFile = '' 
    crs = '' 
    wellName = ''
    wellCreateType = ''
    xNorth = False

class ReadWriteLogParam(h5geo.LogCurveParam):
    """Contains data needed to create new `h5geo.H5LogCurve` object. Inherits from `h5geo.LogCurveParam`"""
    readFile = ''
    saveFile = '' 
    crs = '' 
    wellName = ''
    wellCreateType = ''
    logType = ''
    logName = ''
    logCreateType = ''

class qColadaLasReader(qColadaReader):
    """Class designated to read LAS files (log curves)

    Args:
        qColadaReader (PythonQt.QtGui.QObject): parent object

    Returns:
        qColadaReader: object instance
    """
    wellTableView = None
    wellHrzHeader = None
    wellVertHeader = None
    wellProxy = None
    wellModel = None

    logTableView = None
    logHrzHeader = None
    logVertHeader = None
    logProxy = None
    logModel = None

    buttonBox = None
    addToolBtn = None
    removeToolBtn = None
    autoDefineBtn = None
    vSplitter = None
    mainSplitter = None

    # delegates must exist during object's lifetime or application will be break down!
    well_save_to_pathEditDelegate = qPathEditDelegate() 
    well_create_comboDelegate = qComboBoxDelegate()
    well_spatial_units_comboDelegate = qComboBoxDelegate()
    well_headX_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    well_headY_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    well_KB_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    well_name_lineEditDelegate = qLineEditDelegate()
    well_UWI_lineEditDelegate = qLineEditDelegate()

    log_create_comboDelegate = qComboBoxDelegate()
    log_chunkSize_spinBoxDelegate = qSpinBoxDelegate()
    log_type_lineEditDelegate = qLineEditDelegate()
    log_name_lineEditDelegate = qLineEditDelegate()

    validator = qREValidator()
    validator_noSpaces = qREValidator()

    wellTableHdrNames = [
        "read file", "save to", "CRS",
        "well name", "UWI", "well create",
        "spatial units",
        "head x", "head y", "KB",
        "XNorth"]

    wellTableHdrTips = [
        "Read file", "Container where to save data", "CRS authority name and code (example: EPSG:2000). Must be set if new well is going to be created",
        "Well name", "Unique Well Identifier", "Creation type for well", 
        "Spatial units",
        "Header X coordinate. Must be set if new well is going to be created", "Header Y coordinate. Must be set if new well is going to be created", "Kelly Bushing",
        "`X` axis points to the North? checked - True, unchecked - False"]

    logTableHdrNames = [
        "plot", "read file", "log type", "log name", 
        "log create", "data units", "chunk size"]

    logTableHdrTips = [
        "Plot data", "Read file", "Log type (name for group of logs)", "Log name (must be unique inside well and log type)", 
        "Creation type for log", "Data units", "Chunk size (hdf5 feature aimed to increase perfomance of IO operations)"]
    
    def __init__(self, parent=None):
        super(qColadaReader, self).__init__(parent)
        self.initGUIVars()
        self.initWellTable()
        self.initLogTable()
        self.addToolBtn.clicked.connect(self.onAddBtnClicked)
        self.removeToolBtn.clicked.connect(self.onRemoveToolBtnClicked)
        self.autoDefineBtn.clicked.connect(self.onAutoDefineToolBtnClicked)
        self.buttonBox.clicked.connect(self.onButtonBoxClicked)

    def initGUIVars(self):
        self.wellTableView = slicer.util.findChild(self, 'TableView')
        self.wellHrzHeader = slicer.util.findChild(self.wellTableView, 'HrzHeader')
        self.wellVertHeader = slicer.util.findChild(self.wellTableView, 'VertHeader')
        self.wellProxy = self.wellTableView.model()
        self.wellModel = self.wellProxy.sourceModel
        self.buttonBox = slicer.util.findChild(self, 'ButtonBox')
        self.addToolBtn = slicer.util.findChild(self, 'AddToolBtn')
        self.removeToolBtn = slicer.util.findChild(self, 'RemoveToolBtn')
        self.autoDefineBtn = slicer.util.findChild(self, 'AutoDefineBtn')
        self.vSplitter = slicer.util.findChild(self, 'VSplitter')
        self.mainSplitter = slicer.util.findChild(self, 'MainSplitter')
        self.logTableView = qCopyPasteTableView()
        self.logTableView.setObjectName("LogTableView")
        self.logTableView.setVerticalScrollMode(QtGui.QAbstractItemView.ScrollPerPixel)
        self.logTableView.setHorizontalScrollMode(QtGui.QAbstractItemView.ScrollPerPixel)
        self.mainSplitter.addWidget(self.logTableView)
        self.logHrzHeader = qCheckableHHeaderView(self.logTableView)
        self.logHrzHeader.setObjectName("LogHrzHeader")
        self.logHrzHeader.setSectionsClickable(True)
        self.logHrzHeader.setSectionsMovable(False)
        self.logHrzHeader.setHighlightSections(True)
        self.logTableView.setHorizontalHeader(self.logHrzHeader)
        self.logVertHeader = qCheckableVHeaderView(self.logTableView)
        self.logVertHeader.setObjectName("VertHeader")
        self.logVertHeader.setSectionsClickable(True)
        self.logVertHeader.setSectionsMovable(False)
        self.logVertHeader.setHighlightSections(True)
        self.logVertHeader.setToolTip("Logs to be: read - checked, skipped - unchecked")
        self.logVertHeader.setToolTipDuration(3000)
        # set row height to match content
        self.logVertHeader.setDefaultSectionSize(self.logVertHeader.minimumSectionSize)
        self.logTableView.setVerticalHeader(self.logVertHeader)
        self.logTableView.setSortingEnabled(True)
        self.logProxy = QtGui.QSortFilterProxyModel()
        self.logModel = QtGui.QStandardItemModel()
        self.logProxy.setSourceModel(self.logModel)
        self.logTableView.setModel(self.logProxy)

    def initWellTable(self):
        self.wellModel.setColumnCount(len(self.wellTableHdrNames))
        self.wellModel.setHorizontalHeaderLabels(self.wellTableHdrNames)
        for i in range(0, self.wellModel.columnCount()):
            self.wellModel.horizontalHeaderItem(i).setToolTip(self.wellTableHdrTips[i])

        self.well_save_to_pathEditDelegate.setParent(self.wellTableView)
        self.well_create_comboDelegate.setParent(self.wellTableView)
        self.well_spatial_units_comboDelegate.setParent(self.wellTableView)
        self.well_headX_scienceSpinBoxDelegate.setParent(self.wellTableView)
        self.well_headY_scienceSpinBoxDelegate.setParent(self.wellTableView)
        self.well_KB_scienceSpinBoxDelegate.setParent(self.wellTableView)
        self.well_name_lineEditDelegate.setParent(self.wellTableView)
        self.well_UWI_lineEditDelegate.setParent(self.wellTableView)

        self.validator.setParent(self.wellTableView)
        self.validator.setRegularExpression(Util.fileNameRegExp())
        self.validator.setToolTipText(Util.fileNameToolTipText())
        self.validator.setToolTipDuration(3000)

        self.well_name_lineEditDelegate.setValidator(self.validator)
        self.well_UWI_lineEditDelegate.setValidator(self.validator)

        self.wellTableView.setItemDelegateForColumn(
            self.wellTableHdrNames.index("save to"), self.well_save_to_pathEditDelegate)

        self.well_create_comboDelegate.setTexts(list(h5geo.CreationType.__members__.keys()))
        self.well_spatial_units_comboDelegate.setTexts(['meter', 'foot'])
        self.wellTableView.setItemDelegateForColumn(
            self.wellTableHdrNames.index("well create"), self.well_create_comboDelegate)
        self.wellTableView.setItemDelegateForColumn(
            self.wellTableHdrNames.index("spatial units"), self.well_spatial_units_comboDelegate)
        self.wellTableView.setItemDelegateForColumn(
            self.wellTableHdrNames.index("head x"), self.well_headX_scienceSpinBoxDelegate)
        self.wellTableView.setItemDelegateForColumn(
            self.wellTableHdrNames.index("head y"), self.well_headY_scienceSpinBoxDelegate)
        self.wellTableView.setItemDelegateForColumn(
            self.wellTableHdrNames.index("KB"), self.well_KB_scienceSpinBoxDelegate)
        self.wellTableView.setItemDelegateForColumn(
            self.wellTableHdrNames.index("well name"), self.well_name_lineEditDelegate)
        self.wellTableView.setItemDelegateForColumn(
            self.wellTableHdrNames.index("UWI"), self.well_UWI_lineEditDelegate)

    def initLogTable(self):
        self.logModel.setHorizontalHeaderLabels(self.logTableHdrNames)
        self.logModel.setColumnCount(len(self.logTableHdrNames))
        for i in range(0, self.logModel.columnCount()):
            self.logModel.horizontalHeaderItem(i).setToolTip(self.logTableHdrTips[i])

        self.log_create_comboDelegate.setParent(self.logTableView)
        self.log_chunkSize_spinBoxDelegate.setParent(self.logTableView)
        self.log_chunkSize_spinBoxDelegate.setMinValue(1)
        self.log_type_lineEditDelegate.setParent(self.logTableView)
        self.log_name_lineEditDelegate.setParent(self.logTableView)

        self.validator_noSpaces.setParent(self.wellTableView)
        self.validator_noSpaces.setRegularExpression(Util.fileNameNoSpaceRegExp())
        self.validator_noSpaces.setToolTipText(Util.fileNameNoSpaceToolTipText())
        self.validator_noSpaces.setToolTipDuration(3000)

        self.log_type_lineEditDelegate.setValidator(self.validator_noSpaces)
        self.log_name_lineEditDelegate.setValidator(self.validator)

        self.log_create_comboDelegate.setTexts(list(h5geo.CreationType.__members__.keys()))
        self.logTableView.setItemDelegateForColumn(
            self.logTableHdrNames.index("log create"), self.log_create_comboDelegate)
        self.logTableView.setItemDelegateForColumn(
            self.logTableHdrNames.index("chunk size"), self.log_chunkSize_spinBoxDelegate)
        self.logTableView.setItemDelegateForColumn(
            self.logTableHdrNames.index("log type"), self.log_type_lineEditDelegate)
        self.logTableView.setItemDelegateForColumn(
            self.logTableHdrNames.index("log name"), self.log_name_lineEditDelegate)

    def getReadWriteWellParamFromTable(self, w_proxy_row: int) -> ReadWriteWellParam:
        """Read data needed to open/create `h5geo.H5Well` object. 

        Args:
            w_proxy_row (int): well table's proxy model row number

        Returns:
            ReadWriteWellParam: filled with values instance
        """
        p = ReadWriteWellParam()

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("read file")))
        p.readFile = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("save to")))
        p.saveFile = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("CRS")))
        p.crs = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("well name")))
        p.wellName = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("UWI")))
        p.uwi = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("well create")))
        p.wellCreateType = h5geo.CreationType.__members__[tmp] if tmp in h5geo.CreationType.__members__ else h5geo.CreationType(0)

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("spatial units")))
        p.spatialUnits = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("head x")))
        p.headX = float(tmp) if tmp else np.nan

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("head y")))
        p.headY = float(tmp) if tmp else np.nan

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("KB")))
        p.kb = float(tmp) if tmp else np.nan
        
        tmp = self.wellTableView.indexWidget(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("XNorth"))).checkState()
        p.xNorth = True if tmp == Qt.Qt.Checked else False
        
        if p.xNorth:
            p.headX, p.headY = p.headY, p.headX

        return p

    def getReadWriteLogParamFromTable(self, l_proxy_row: int) -> ReadWriteLogParam:
        """Read data needed to open/create `h5geo.H5LogCurve` object. 

        Args:
            l_proxy_row (int): log table's proxy model row number

        Returns:
            ReadWriteLogParam: filled with values instance
        """
        p = ReadWriteLogParam()

        tmp = self.logProxy.data(
            self.logProxy.index(l_proxy_row, self.logTableHdrNames.index("read file")))
        p.readFile = tmp if tmp else ''

        tmp = self.logProxy.data(
            self.logProxy.index(l_proxy_row, self.logTableHdrNames.index("log type")))
        p.logType = tmp if tmp else ''

        tmp = self.logProxy.data(
            self.logProxy.index(l_proxy_row, self.logTableHdrNames.index("log name")))
        p.logName = tmp if tmp else ''

        tmp = self.logProxy.data(
            self.logProxy.index(l_proxy_row, self.logTableHdrNames.index("log create")))
        p.logCreateType = h5geo.CreationType.__members__[tmp] if tmp in h5geo.CreationType.__members__ else h5geo.CreationType(0)

        tmp = self.logProxy.data(
            self.logProxy.index(l_proxy_row, self.logTableHdrNames.index("data units")))
        p.dataUnits = tmp if tmp else ''

        tmp = self.logProxy.data(
            self.logProxy.index(l_proxy_row, self.logTableHdrNames.index("chunk size")))
        p.chunkSize = int(tmp) if tmp else 1000


        itemList = self.wellModel.findItems(p.readFile, Qt.Qt.MatchFixedString, self.wellTableHdrNames.index("read file"))
        w_proxy_row = self.wellProxy.mapFromSource(itemList[0].index()).row()

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("save to")))
        p.saveFile = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("CRS")))
        p.crs = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("well name")))
        p.wellName = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("UWI")))
        p.uwi = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("well create")))
        p.wellCreateType = h5geo.CreationType.__members__[tmp] if tmp in h5geo.CreationType.__members__ else h5geo.CreationType(0)
        
        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("spatial units")))
        p.spatialUnits = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("head x")))
        p.headX = float(tmp) if tmp else np.nan

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("head y")))
        p.headY = float(tmp) if tmp else np.nan

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("KB")))
        p.kb = float(tmp) if tmp else np.nan

        return p

    def updateWellTableRow(self, w_proxy_row: int):
        """Resets well table's row and then updates log table. 
        Fills well table as much as it can.

        Args:
            w_proxy_row (int): well table's proxy model row number
        """
        self.resetWellTableRow(w_proxy_row)
        
        p_w = self.getReadWriteWellParamFromTable(w_proxy_row)

        readFile = self.wellProxy.data(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("read file")))

        w = Well.from_las(readFile, index=p_w.spatialUnits)

        well_name = w.name if w.name else os.path.splitext(os.path.basename(readFile))[0]
        self.wellProxy.setData(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("well name")), well_name)
        self.wellProxy.setData(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("UWI")), w.uwi)
        
        self.updateLogTable(w)

    def updateLogTable(self, w: Well):
        """Resets log table for a given `readFile` and inserts and new rows and fills with log info.

        Args:
            readFile (str): file path to read (.LAS file)
        """

        self.resetLogTable(w.fname)

        log_names = w._get_curve_mnemonics()
        for log_name in log_names:
            log = w.get_curve(log_name)
            self.logModel.insertRow(self.logModel.rowCount())
            row = self.logModel.rowCount()-1
            proxy_row = self.logProxy.mapFromSource(self.logModel.index(row, self.logTableHdrNames.index("plot"))).row()

            self.logTableView.setIndexWidget(self.logProxy.index(proxy_row, self.logTableHdrNames.index("plot")), QtGui.QRadioButton())

            readFileItem = QtGui.QStandardItem(w.fname)
            readFileItem.setFlags(readFileItem.flags() & ~Qt.Qt.ItemIsEditable)
            self.logModel.setItem(row, self.logTableHdrNames.index("read file"), readFileItem)
            self.logModel.setData(self.logModel.index(row, self.logTableHdrNames.index("log create")), 
                str(h5geo.CreationType.OPEN_OR_CREATE).rsplit('.', 1)[-1])
            self.logModel.setData(self.logModel.index(row, self.logTableHdrNames.index("log type")), log_name)
            self.logModel.setData(self.logModel.index(row, self.logTableHdrNames.index("log name")), log_name)
            self.logModel.setData(self.logModel.index(row, self.logTableHdrNames.index("data units")), log.units)
            self.logModel.setData(self.logModel.index(row, self.logTableHdrNames.index("chunk size")), 1000)  # don't use `p.chunkSize` as it will be empty

            self.logModel.verticalHeaderItem(row).setCheckState(Qt.Qt.Checked)
            for row in range(self.logModel.rowCount()):
                self.logModel.verticalHeaderItem(row).setText(str(row + 1))

    def resetWellTableRow(self, w_proxy_row: int):
        """Set some default values to well table.

        Args:
            w_proxy_row (int): well table's proxy model row number
        """
        readFile = self.wellProxy.data(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("read file")))
        fi = QtCore.QFileInfo(readFile);
        if not fi.exists():
            return

        self.wellProxy.setData(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("save to")),
                    Util.defaultWellDir() + "/" + fi.baseName() + ".h5")
        self.wellProxy.setData(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("CRS")),
                    Util.CRSAuthName() + ":" + str(Util.CRSCode()))
        self.wellProxy.setData(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("well create")), 
            str(h5geo.CreationType.OPEN_OR_CREATE).rsplit('.', 1)[-1])
        self.wellProxy.setData(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("spatial units")), 'meter')
        
        self.wellTableView.setIndexWidget(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("XNorth")), QtGui.QCheckBox())        

    def resetLogTable(self):
        """Deletes all rows from log model (thus log table becomes clean)."""
        self.logModel.removeRows(0, self.logModel.rowCount())

    def resetLogTable(self, readFile: str):
        """Removes all the rows from log table that corresponds to the given `readFile`.

        Args:
            readFile (str): file name to read
        """
        itemList = self.logModel.findItems(readFile, Qt.Qt.MatchFixedString, self.logTableHdrNames.index("read file"))
        rowList = []
        for item in itemList:
            rowList.append(item.row())

        # to remove rows correctly we should remove them in descending order
        rowList.sort(reverse = True)

        for row in rowList:
            self.logModel.removeRow(row)

        for row in range(self.logModel.rowCount()):
            self.logModel.verticalHeaderItem(row).setText(str(row + 1))

        col = self.logTableHdrNames.index("read file")
        for row in range(self.logModel.rowCount()):
            self.logModel.data(self.logModel.index(row, col))

    def onAddBtnClicked(self):
        fileNames = ctk.ctkFileDialog.getOpenFileNames(None, 'Select one or more LAS files to open', '', 'LAS (*.las *.dat *.txt);; all (*.*)')

        for name in fileNames:
            if self.wellModel.findItems(name, Qt.Qt.MatchFixedString, self.wellTableHdrNames.index("read file")):
                QtGui.QMessageBox.warning(self, "Warning", name+": is already in table!");
                continue

            self.wellModel.insertRow(self.wellModel.rowCount())
            row = self.wellModel.rowCount()-1

            readFileItem = QtGui.QStandardItem(name)
            readFileItem.setFlags(readFileItem.flags() & ~Qt.Qt.ItemIsEditable)
            self.wellModel.setItem(row, self.wellTableHdrNames.index("read file"), readFileItem)

            for row in range(self.wellModel.rowCount()):
                self.wellModel.verticalHeaderItem(row).setText(str(row + 1))

            self.updateWellTableRow(row);

    def onRemoveToolBtnClicked(self):
        """Removes selected rows from well model."""
        indexList = self.wellTableView.selectionModel().selectedRows()

        rowList = []
        for index in indexList:
            rowList.append(index.row())

        # to remove rows correctly we should remove them in descending order
        rowList.sort(reverse = True)

        for row in rowList:
            readFile = self.wellProxy.data(self.wellProxy.index(row, self.wellTableHdrNames.index("read file")))
            self.wellProxy.removeRow(row)
            self.resetLogTable(readFile)

        for row in range(self.wellModel.rowCount()):
            self.wellModel.verticalHeaderItem(row).setText(str(row + 1))
            
    def onAutoDefineToolBtnClicked(self):
        """Calls `updateWellTableRow` for selected well table's rows."""
        indexList = self.wellTableView.selectionModel().selectedRows()
        for index in indexList:
            self.updateWellTableRow(index.row())

    def onButtonBoxClicked(self, button):
        if button == self.buttonBox.button(QtGui.QDialogButtonBox.Ok):
            currentProjectUnits = Util.LengthUnits()
            progressDialog = slicer.util.createProgressDialog(
                parent=self, maximum=self.wellModel.rowCount())
            for row in range(self.wellModel.rowCount()):
                p_w = self.getReadWriteWellParamFromTable(row)
                progressDialog.setLabelText("Reading: " + p_w.readFile)
                progressDialog.setValue(row)
                try:
                    h5wellCnt = h5geo.createWellContainerByName(p_w.saveFile, h5geo.CreationType.OPEN_OR_CREATE)

                    if not h5wellCnt:
                        errMsg = 'Can`t open or create: ' + p_w.saveFile + ''' 
                        Possible reasons:
                        - `save to` is incorrect;
                        - `save to` points to an existing NON WELL CONTAINER file;
                        - you don't have permissions to create files in destination folder;
                        '''
                        QtGui.QMessageBox.critical(self, "Error", errMsg)
                        continue
                    
                    if p_w.xNorth:
                        p_w.headX, p_w.headY, val = Util.convCoord2CurrentProjection(p_w.headY, p_w.headX, p_w.crs, p_w.spatialUnits)
                    else:
                        p_w.headX, p_w.headY, val = Util.convCoord2CurrentProjection(p_w.headX, p_w.headY, p_w.crs, p_w.spatialUnits)
                        
                    # if new well will be created then the units will be `p_w.spatialUnits`
                    coef_w = Util.convertUnits(
                        currentProjectUnits,
                        p_w.spatialUnits)
                    
                    p_w.headX *= coef_w
                    p_w.headY *= coef_w
                        
                    if not val:
                        errMsg = 'Can`t transform coordinates from: ' + p_w.crs + ' to: ' + Util.CRSAuthName() + ":" + str(Util.CRSCode()) + ''' 
                        Possible reasons:
                        - project is not set or contains incorrect CRS;
                        - `CRS` is incorrect;
                        '''
                        QtGui.QMessageBox.critical(self, "Error", errMsg)
                        continue
                    
                    h5well = h5wellCnt.createWell(p_w.wellName, p_w, p_w.wellCreateType)
                    if not h5well:
                        errMsg = 'Can`t open, create or overwrite well: ' + p_w.wellName + ' from container: ' + p_w.saveFile + ''' 
                        Possible reasons:
                        - `well name` is incorrect;
                        - `well create` is incorrect;
                        - well container contains a group with the same name `well name` but this group can`t be treated as well and `well create` flag is `OPEN/CREATE/OPEN_OR_CREATE`;
                        - you don't have permissions to create objects inside specified container (maybe 3rd party app is blocking this container);
                        '''
                        QtGui.QMessageBox.critical(self, "Error", errMsg)
                        continue

                    itemList = self.logModel.findItems(p_w.readFile, Qt.Qt.MatchFixedString, self.logTableHdrNames.index("read file"))
                    for item in itemList:
                        if self.logModel.verticalHeaderItem(item.row()).checkState() != Qt.Qt.Checked:
                            continue

                        l_proxy_row = self.logProxy.mapFromSource(item.index()).row()
                        p_l = self.getReadWriteLogParamFromTable(l_proxy_row)

                        print(p_l.readFile, ': ', p_l.logType, p_l.logName, p_l.logCreateType)

                        h5logCurve = h5well.createLogCurve(p_l.logType, p_l.logName, p_l, p_l.logCreateType)
                        if not h5logCurve:
                            errMsg = 'Can`t open, create or overwrite log type: ' + p_l.logType + ' with name: ' + p_l.logName + ' from well: ' + p_w.wellName + ' from container: ' + p_w.saveFile + ''' 
                            Possible reasons:
                            - `log type` or `log name` is incorrect;
                            - `log create` is incorrect;
                            - `log name` is not unique inside `log type` of specified well and `log create` is `OPEN/CREATE/OPEN_OR_CREATE`;
                            - you don't have permissions to create objects inside specified container (maybe 3rd party app is blocking this container);
                            '''
                            QtGui.QMessageBox.critical(self, "Error", errMsg)
                            continue

                        w = Well.from_las(p_l.readFile, index=p_l.spatialUnits)
                        if not h5logCurve.writeCurve(h5geo.LogDataType.MD, w.survey_basis(), p_l.spatialUnits):
                            errMsg = 'Can`t write depth curve (MD) to log name: ' + p_l.logName + 'from log type: ' + p_l.logType + 'from well: ' + p_w.wellName + ' from container: ' + p_w.saveFile + ''' 
                            Possible reasons:
                            - `log_data` dataset is broken (some attributes are missing or dataset is not resizable);
                            - incorrect `spatial units`;
                            - you don't have write permissions inside specified container (maybe 3rd party app is blocking this container);
                            '''
                            QtGui.QMessageBox.critical(self, "Error", errMsg)
                            continue

                        if p_l.logType not in w.data:
                            errMsg = 'Given `read file`: ' + p_l.readFile + ' doesn`t contain `log type`: ' + p_l.logType + ''' 
                            Please check mentionned `log type` of the given well
                            '''
                            QtGui.QMessageBox.critical(self, "Error", errMsg)
                            continue

                        if not h5logCurve.writeCurve(h5geo.LogDataType.VAL, w.data[p_l.logType].values, p_l.dataUnits):
                            errMsg = 'Can`t write data curve (VAL) to log name: ' + p_l.logName + 'from log type: ' + p_l.logType + 'from well: ' + p_w.wellName + ' from container: ' + p_w.saveFile + ''' 
                            Possible reasons:
                            - `log_data` dataset is broken (some attributes are missing or dataset is not resizable);
                            - incorrect `data units` (if the curve is not overwriten and previous `data units` belongs to another dimensionality then you may need to overwrite it as it is impossible to convert units in this way);
                            - you don't have write permissions inside specified container (maybe 3rd party app is blocking this container);
                            '''
                            QtGui.QMessageBox.critical(self, "Error", errMsg)
                            continue

                        if progressDialog.wasCanceled:
                            break
                        
                    h5wellCnt.getH5File().flush()

                except h5gt.Exception as ex:
                    QtGui.QMessageBox.critical(self, "Error", ex)
                    # print(ex)
                    continue

            progressDialog.setValue(self.wellModel.rowCount())

        elif button == self.buttonBox.button(QtGui.QDialogButtonBox.Cancel):
            self.close()
