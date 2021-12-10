from PythonQt import *
from qColadaAppPythonQt import *
from h5gtpy import h5gt
from h5geopy import h5geo
import os, slicer, ctk
import numpy as np


class DevRead():
    """Class used by `qColadaDevReader` and designated to work with `.DEV` (deviation) files

    Returns:
        DevRead: object instance
    """
    readFile = ''
    delimiter = ' '
    wellNameSep = "WELL NAME:"
    uwiSep = "UWI:"
    wellTypeSep = "WELL TYPE:"
    kbSep = "WELL KB:"
    headXSep = "WELL HEAD X-COORDINATE:"
    headYSep = "WELL HEAD Y-COORDINATE:"
    defaultColNames = [
        "X",
        "Y",
        "Z",
        "MD",
        "TVD",
        "TVDSS",
        "DX",
        "DY",
        "AZIM",
        "INCL"]

    _val = True # read/write
    __foo = True # private

    def __init__(self, readFile: str, delimiter = ' '):
        """[summary]

        Args:
            readFile (str): [description]
            delimiter (str, optional): [description]. Defaults to ' '.
        """
        self.setReadFile(readFile)
        self.setDelimiter(delimiter)

    def __getWellInfo(self, separator: str):
        for line in self.getHeaderLines():
            strList = line.split(separator)
            if len(strList) > 1:
                return strList[1].replace(self.delimiter, '')
        return str()

    def setReadFile(self, readFile: str):
        self.readFile = readFile

    def setDelimiter(self, delimiter: str):
        self.delimiter = delimiter

    def getDelimiter(self) -> str:
        return self.delimiter

    def getHeaderLines(self) -> list:
        """[summary]

        Returns:
            list: [description]
        """
        qFile = QtCore.QFile(self.readFile)
        if not qFile.open(QtCore.QIODevice.ReadOnly | QtCore.QIODevice.Text):
            return

        floatRE = Util.floatRegExp()

        headerLines = []
        txtStream = QtCore.QTextStream(qFile)
        while not txtStream.atEnd():
            line = txtStream.readLine()
            lineList = list(filter(None, line.split(self.delimiter)))  # I need to use `filter` func to skip empty parts after `str.split()``
            counter = 0
            for str in lineList:
                counter += floatRE.exactMatch(str)

            if counter == len(lineList):
                break

            headerLines.append(line)

        qFile.close()
        return headerLines

    def getSkipLines(self):
        return len(self.getHeaderLines())

    def getColNames(self, commentSymbol='#') -> list:
        """Return automatically defined column names"""
        hdrLines = self.getHeaderLines()
        for hdr in reversed(hdrLines):
            strList = hdr.split(commentSymbol, 1)
            if strList[0] and not strList[0].isspace():  # check if str before first comment symbol contains only spaces or not
                return list(filter(None, hdr.split(self.delimiter)))

        return list()

    def getWellName(self) -> str:
        return self.__getWellInfo(self.wellNameSep)

    def getUWI(self) -> str:
        return self.__getWellInfo(self.uwiSep)

    def getWellType(self) -> str:
        return self.__getWellInfo(self.wellTypeSep)

    def getKB(self) -> float:
        return float(self.__getWellInfo(self.kbSep))

    def getHeadXCoord(self) -> float:
        return float(self.__getWellInfo(self.headXSep))

    def getHeadYCoord(self) -> float:
        return float(self.__getWellInfo(self.headYSep))

    def getDATA(self, colNames: list, useCols: list, nHdr: int, nFooter: int = 0) -> np.ndarray:
        return np.genfromtxt(self.readFile, skip_header = nHdr, names = colNames, usecols = useCols)


class ReadWriteWellParam(h5geo.WellParam):
    """Contains data needed to create new `h5geo.H5Well` object. Inherits from `h5geo.WellParam`"""
    readFile = ''
    saveFile = ''
    wellName = ''
    wellCreateType = ''
    xNorth = False


class ReadWriteDevParam(h5geo.DevCurveParam):
    """Contains data needed to create new `h5geo.H5DevCurve` object. Inherits from `h5geo.DevCurveParam`"""
    readFile = ''
    saveFile = ''
    wellName = ''
    wellCreateType = ''
    devName = ''
    devCreateType = ''
    skipLines = 0
    delimiter = ' '
    trajFormat = ''
    coord_1_col = 0
    coord_2_col = 0
    coord_3_col = 0
    depthMult = -1


class qColadaDevReader(qColadaReader):
    """Class designated to read DEV files (dev curves)

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

    devTableView = None
    devHrzHeader = None
    devVertHeader = None
    devProxy = None
    devModel = None

    buttonBox = None
    addToolBtn = None
    removeToolBtn = None
    autoDefineBtn = None
    vSplitter = None
    mainSplitter = None

    # delegates must exist during object's lifetime or application will be break down!
    well_save_to_pathEditDelegate = qPathEditDelegate()
    well_create_comboDelegate = qComboBoxDelegate()
    well_headX_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    well_headY_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    well_KB_scienceSpinBoxDelegate = qScienceSpinBoxDelegate()
    well_name_lineEditDelegate = qLineEditDelegate()
    well_UWI_lineEditDelegate = qLineEditDelegate()

    dev_create_comboDelegate = qComboBoxDelegate()
    dev_trajFormat_comboDelegate = qComboBoxDelegate()
    dev_coord_1_spinBoxDelegate = qSpinBoxDelegate()
    dev_coord_2_spinBoxDelegate = qSpinBoxDelegate()
    dev_coord_3_spinBoxDelegate = qSpinBoxDelegate()
    dev_depthMult_spinBoxDelegate = qSpinBoxDelegate()
    dev_chunkSize_spinBoxDelegate = qSpinBoxDelegate()
    dev_type_lineEditDelegate = qLineEditDelegate()
    dev_name_lineEditDelegate = qLineEditDelegate()

    validator = qREValidator()
    validator_noSpaces = qREValidator()

    wellTableHdrNames = [
        "read file", "save to", "CRS",
        "well name", "UWI", "well create",
        "length units",
        "head x", "head y", "KB",
        "XNorth"]

    wellTableHdrTips = [
        "Read file", "Container where to save data", "Coordinate reference system",
        "Well name", "Unique Well Identifier", "Creation type for well",
        "Length units",
        "Header X coordinate. Must be set if new well is going to be created", "Header Y coordinate. Must be set if new well is going to be created", "Kelly Bushing",
        "`X` axis points to the North? checked - True, unchecked - False"]

    devTableHdrNames = [
        "plot", "read file", "dev name", "dev create",
        "skip lines", "delimiter",
        "temporal units", "angle units",
        "trajectory format",
        "coord_1 col", "coord_2 col", "coord_3 col",
        "depth mult",
        "chunk size"]

    devTableHdrTips = [
        "Plot data", "Read file", "Dev name", "Creation type for dev",
        "Number of lines to skip", "Delimiter",
        "Temporal units", "Angle units",
        "Trajectory format",
        "First coordinate column", "Second coordinate column", "Third coordinate column",
        "Depth multiplier: downwards is negative (usually -1)",
        "Chunk size (hdf5 feature aimed to increase perfomance of IO operations)"]

    devDict = dict()

    def __init__(self, parent=None):
        super(qColadaReader, self).__init__(parent)
        self.initGUIVars()
        self.initWellTable()
        self.initDevTable()
        self.devProxy.dataChanged.connect(self.onDevProxyDataChanged)
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
        self.devTableView = qCopyPasteTableView()
        self.devTableView.setObjectName("DevTableView")
        self.devTableView.setVerticalScrollMode(QtGui.QAbstractItemView.ScrollPerPixel)
        self.mainSplitter.addWidget(self.devTableView)
        self.devHrzHeader = qCheckableHHeaderView(self.devTableView)
        self.devHrzHeader.setObjectName("DevHrzHeader")
        self.devHrzHeader.setSectionsClickable(True)
        self.devHrzHeader.setSectionsMovable(False)
        self.devHrzHeader.setHighlightSections(True)
        self.devTableView.setHorizontalHeader(self.devHrzHeader)
        self.devVertHeader = qCheckableVHeaderView(self.devTableView)
        self.devVertHeader.setObjectName("VertHeader")
        self.devVertHeader.setSectionsClickable(True)
        self.devVertHeader.setSectionsMovable(False)
        self.devVertHeader.setHighlightSections(True)
        self.devVertHeader.setToolTip("DEV to be: read - checked, skipped - unchecked")
        self.devVertHeader.setToolTipDuration(3000)
        # set row height to match content
        self.devVertHeader.setDefaultSectionSize(self.devVertHeader.minimumSectionSize)
        self.devTableView.setVerticalHeader(self.devVertHeader)
        self.devTableView.setSortingEnabled(True)
        self.devProxy = QtGui.QSortFilterProxyModel()
        self.devModel = QtGui.QStandardItemModel()
        self.devProxy.setSourceModel(self.devModel)
        self.devTableView.setModel(self.devProxy)

    def initWellTable(self):
        self.wellModel.setColumnCount(len(self.wellTableHdrNames))
        self.wellModel.setHorizontalHeaderLabels(self.wellTableHdrNames)
        for i in range(0, self.wellModel.columnCount()):
            self.wellModel.horizontalHeaderItem(i).setToolTip(self.wellTableHdrTips[i])

        self.well_save_to_pathEditDelegate.setParent(self.wellTableView)
        self.well_create_comboDelegate.setParent(self.wellTableView)
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
        self.wellTableView.setItemDelegateForColumn(
            self.wellTableHdrNames.index("well create"), self.well_create_comboDelegate)
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

    def initDevTable(self):
        self.devModel.setHorizontalHeaderLabels(self.devTableHdrNames)
        self.devModel.setColumnCount(len(self.devTableHdrNames))
        for i in range(0, self.devModel.columnCount()):
            self.devModel.horizontalHeaderItem(i).setToolTip(self.devTableHdrTips[i])

        self.dev_create_comboDelegate.setParent(self.devTableView)
        self.dev_trajFormat_comboDelegate.setParent(self.devTableView)
        self.dev_coord_1_spinBoxDelegate.setParent(self.devTableView)
        self.dev_coord_1_spinBoxDelegate.setMinValue(1)
        self.dev_coord_2_spinBoxDelegate.setParent(self.devTableView)
        self.dev_coord_2_spinBoxDelegate.setMinValue(1)
        self.dev_coord_3_spinBoxDelegate.setParent(self.devTableView)
        self.dev_coord_3_spinBoxDelegate.setMinValue(1)
        self.dev_depthMult_spinBoxDelegate.setParent(self.devTableView)
        self.dev_depthMult_spinBoxDelegate.setStep(2)
        self.dev_depthMult_spinBoxDelegate.setMinValue(-1)
        self.dev_depthMult_spinBoxDelegate.setMaxValue(1)
        self.dev_chunkSize_spinBoxDelegate.setParent(self.devTableView)
        self.dev_chunkSize_spinBoxDelegate.setMinValue(1)
        self.dev_type_lineEditDelegate.setParent(self.devTableView)
        self.dev_name_lineEditDelegate.setParent(self.devTableView)

        self.validator_noSpaces.setParent(self.wellTableView)
        self.validator_noSpaces.setRegularExpression(Util.fileNameNoSpaceRegExp())
        self.validator_noSpaces.setToolTipText(Util.fileNameNoSpaceToolTipText())
        self.validator_noSpaces.setToolTipDuration(3000)

        self.dev_type_lineEditDelegate.setValidator(self.validator_noSpaces)
        self.dev_name_lineEditDelegate.setValidator(self.validator)

        self.dev_create_comboDelegate.setTexts(list(h5geo.CreationType.__members__.keys()))
        self.dev_trajFormat_comboDelegate.setTexts(list(h5geo.TrajectoryFormat.__members__.keys()))

        self.devTableView.setItemDelegateForColumn(
            self.devTableHdrNames.index("dev create"), self.dev_create_comboDelegate)
        self.devTableView.setItemDelegateForColumn(
            self.devTableHdrNames.index("trajectory format"), self.dev_trajFormat_comboDelegate)
        self.devTableView.setItemDelegateForColumn(
            self.devTableHdrNames.index("coord_1 col"), self.dev_coord_1_spinBoxDelegate)
        self.devTableView.setItemDelegateForColumn(
            self.devTableHdrNames.index("coord_2 col"), self.dev_coord_2_spinBoxDelegate)
        self.devTableView.setItemDelegateForColumn(
            self.devTableHdrNames.index("coord_3 col"), self.dev_coord_3_spinBoxDelegate)
        self.devTableView.setItemDelegateForColumn(
            self.devTableHdrNames.index("depth mult"), self.dev_depthMult_spinBoxDelegate)
        self.devTableView.setItemDelegateForColumn(
            self.devTableHdrNames.index("chunk size"), self.dev_chunkSize_spinBoxDelegate)
        self.devTableView.setItemDelegateForColumn(
            self.devTableHdrNames.index("dev name"), self.dev_name_lineEditDelegate)

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
        p.spatialReference = tmp if tmp else ''

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
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("length units")))
        p.lengthUnits = tmp if tmp else ''

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

    def getReadWriteDevParamFromTable(self, d_proxy_row: int) -> ReadWriteDevParam:
        """Read data needed to open/create `h5geo.H5DevCurve` object.

        Args:
            d_proxy_row (int): dev table's proxy model row number

        Returns:
            ReadWriteDevParam: filled with values instance
        """
        p = ReadWriteDevParam()

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("read file")))
        p.readFile = tmp if tmp else ''

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("dev name")))
        p.devName = tmp if tmp else ''

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("dev create")))
        p.devCreateType = h5geo.CreationType.__members__[tmp] if tmp in h5geo.CreationType.__members__ else h5geo.CreationType(0)

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("skip lines")))
        p.skipLines = int(tmp) if tmp else 0

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("delimiter")))
        p.delimiter = tmp if tmp else ' '

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("temporal units")))
        p.temporalUnits = tmp if tmp else ''

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("angle units")))
        p.angleUnits = tmp if tmp else ''

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("trajectory format")))
        p.trajFormat = tmp if tmp else ''

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("coord_1 col")))
        p.coord_1_col = int(tmp)-1 if tmp else 0

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("coord_2 col")))
        p.coord_2_col = int(tmp)-1 if tmp else 0

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("coord_3 col")))
        p.coord_3_col = int(tmp)-1 if tmp else 0

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("depth mult")))
        p.depthMult = int(tmp) if tmp else 1

        tmp = self.devProxy.data(
            self.devProxy.index(d_proxy_row, self.devTableHdrNames.index("chunk size")))
        p.chunkSize = int(tmp) if tmp else 1000


        itemList = self.wellModel.findItems(p.readFile, Qt.Qt.MatchFixedString, self.wellTableHdrNames.index("read file"))
        w_proxy_row = self.wellProxy.mapFromSource(itemList[0].index()).row()

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("save to")))
        p.saveFile = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("CRS")))
        p.spatialReference = tmp if tmp else ''

        tmp = self.wellProxy.data(
            self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("length units")))
        p.lengthUnits = tmp if tmp else ''

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
        """Resets well table's row and then adds `DevRead` obj to `devDict` attr and then updates dev table.
        Fills well table as much as it can.

        Args:
            w_proxy_row (int): well table's proxy model row number
        """
        self.resetWellTableRow(w_proxy_row)

        readFile = self.wellProxy.data(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("read file")))

        d = DevRead(readFile)
        self.devDict[readFile] = d

        well_name = d.getWellName() if d.getWellName() else os.path.splitext(os.path.basename(readFile))[0]
        self.wellProxy.setData(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("well name")), well_name)
        self.wellProxy.setData(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("length units")), 'meter')
        self.wellProxy.setData(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("head x")), d.getHeadXCoord())
        self.wellProxy.setData(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("head y")), d.getHeadYCoord())
        self.wellProxy.setData(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("KB")), d.getKB())

        p = self.getReadWriteWellParamFromTable(w_proxy_row)
        self.updateDevTable(p.readFile)

    def updateDevTable(self, readFile: str):
        """Resets dev table for a given `readFile` and inserts new rows and fills with dev info.

        Args:
            readFile (str): file path to read (.DEV file)
        """
        if readFile not in self.devDict:
            QtGui.QMessageBox.critical(self, "Error", "Dev dict doesn't contain: " + readFile)
            return

        self.resetDevTable(readFile)

        d = DevRead(readFile)

        self.devModel.insertRow(self.devModel.rowCount())
        row = self.devModel.rowCount()-1
        proxy_row = self.devProxy.mapFromSource(self.devModel.index(row, self.devTableHdrNames.index("plot"))).row()

        self.devTableView.setIndexWidget(self.devProxy.index(proxy_row, self.devTableHdrNames.index("plot")), QtGui.QRadioButton())

        readFileItem = QtGui.QStandardItem(readFile)
        readFileItem.setFlags(readFileItem.flags() & ~Qt.Qt.ItemIsEditable)
        self.devModel.setItem(row, self.devTableHdrNames.index("read file"), readFileItem)
        self.devModel.setData(self.devModel.index(row, self.devTableHdrNames.index("dev create")),
            str(h5geo.CreationType.OPEN_OR_CREATE).rsplit('.', 1)[-1])
        self.devModel.setData(self.devModel.index(row, self.devTableHdrNames.index("skip lines")), d.getSkipLines())
        self.devModel.setData(self.devModel.index(row, self.devTableHdrNames.index("delimiter")), d.getDelimiter())
        self.devModel.setData(self.devModel.index(row, self.devTableHdrNames.index("temporal units")), 'millisecond')
        self.devModel.setData(self.devModel.index(row, self.devTableHdrNames.index("angle units")), 'degree')
        self.devModel.setData(self.devModel.index(row, self.devTableHdrNames.index("trajectory format")),
            str(h5geo.TrajectoryFormat.MD_AZIM_INCL).rsplit('.', 1)[-1])
        self.devModel.setData(self.devModel.index(row, self.devTableHdrNames.index("depth mult")), str(-1))
        self.devModel.setData(self.devModel.index(row, self.devTableHdrNames.index("chunk size")), 1000)  # don't use `p.chunkSize` as it will be empty

        self.devModel.verticalHeaderItem(row).setCheckState(Qt.Qt.Checked)
        for row in range(self.devModel.rowCount()):
            self.devModel.verticalHeaderItem(row).setText(str(row + 1))

        self.devDict[readFile] = d

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

        self.wellTableView.setIndexWidget(self.wellProxy.index(w_proxy_row, self.wellTableHdrNames.index("XNorth")), QtGui.QCheckBox())

    def resetDevTable(self):
        """Deletes all rows from dev model (thus dev table becomes clean)."""
        self.devModel.removeRows(0, self.devModel.rowCount())

    def resetDevTable(self, readFile: str):
        """Removes all the rows from dev table that corresponds to the given `readFile`.

        Args:
            readFile (str): file name to read
        """
        itemList = self.devModel.findItems(readFile, Qt.Qt.MatchFixedString, self.devTableHdrNames.index("read file"))
        rowList = []
        for item in itemList:
            rowList.append(item.row())

        # to remove rows correctly we should remove them in descending order
        rowList.sort(reverse = True)

        for row in rowList:
            self.devModel.removeRow(row)

        for row in range(self.devModel.rowCount()):
            self.devModel.verticalHeaderItem(row).setText(str(row + 1))

        col = self.devTableHdrNames.index("read file")
        for row in range(self.devModel.rowCount()):
            self.devModel.data(self.devModel.index(row, col))

    def onDevProxyDataChanged(self, topLeft: QtCore.QModelIndex, bottomRight: QtCore.QModelIndex, roles: list):
        trajFormat_col = self.devTableHdrNames.index("trajectory format")
        if trajFormat_col >= topLeft.column() and trajFormat_col <= bottomRight.column() and Qt.Qt.DisplayRole in roles:
            for row in range(topLeft.row(), bottomRight.row()+1):
                p_d = self.getReadWriteDevParamFromTable(row)
                d = self.devDict[p_d.readFile]
                devFileColNames = d.getColNames()
                if not devFileColNames:
                    continue

                trajFormatSplit = p_d.trajFormat.split('_')
                if len(trajFormatSplit) != 3:
                    continue

                tmp = devFileColNames.index(trajFormatSplit[0])+1 if trajFormatSplit[0] in devFileColNames else ''
                self.devProxy.setData(self.devProxy.index(
                    row, self.devTableHdrNames.index("coord_1 col")), tmp)

                tmp = devFileColNames.index(trajFormatSplit[1])+1 if trajFormatSplit[1] in devFileColNames else ''
                self.devProxy.setData(self.devProxy.index(
                    row, self.devTableHdrNames.index("coord_2 col")), tmp)

                tmp = devFileColNames.index(trajFormatSplit[2])+1 if trajFormatSplit[2] in devFileColNames else ''
                self.devProxy.setData(self.devProxy.index(
                    row, self.devTableHdrNames.index("coord_3 col")), tmp)

    def onAddBtnClicked(self):
        fileNames = ctk.ctkFileDialog.getOpenFileNames(None, 'Select one or more DEV files to open', '', 'Deviation (*.dev *.dat *.txt);; all (*.*)')

        QtGui.QApplication.setOverrideCursor(QtCore.Qt.BusyCursor)
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

            self.updateWellTableRow(row)
        QtGui.QApplication.restoreOverrideCursor()

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
            self.devDict.pop(readFile, None)
            self.wellProxy.removeRow(row)
            self.resetDevTable(readFile)

        for row in range(self.wellModel.rowCount()):
            self.wellModel.verticalHeaderItem(row).setText(str(row + 1))

    def onAutoDefineToolBtnClicked(self):
        """Calls `updateWellTableRow` for selected well table's rows."""
        indexList = self.wellTableView.selectionModel().selectedRows()
        QtGui.QApplication.setOverrideCursor(QtCore.Qt.BusyCursor)
        for index in indexList:
            self.updateWellTableRow(index.row())
        QtGui.QApplication.restoreOverrideCursor()

    def onButtonBoxClicked(self, button):
        if button == self.buttonBox.button(QtGui.QDialogButtonBox.Ok):
            QtGui.QApplication.setOverrideCursor(QtCore.Qt.BusyCursor)
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
                        p_w.headX, p_w.headY = p_w.headY, p_w.headX

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

                    itemList = self.devModel.findItems(p_w.readFile, Qt.Qt.MatchFixedString, self.devTableHdrNames.index("read file"))
                    for item in itemList:
                        if self.devModel.verticalHeaderItem(item.row()).checkState() != Qt.Qt.Checked:
                            continue

                        d_proxy_row = self.devProxy.mapFromSource(item.index()).row()
                        p_d = self.getReadWriteDevParamFromTable(d_proxy_row)

                        print(p_d.readFile, ': ', p_d.devName, p_d.devCreateType)

                        h5devCurve = h5well.createDevCurve(p_d.devName, p_d, p_d.devCreateType)
                        if not h5devCurve:
                            errMsg = 'Can`t open, create or overwrite dev curve: ' + p_d.devName + ' from well: ' + p_w.wellName + ' from container: ' + p_w.saveFile + '''
                            Possible reasons:
                            - `dev name` is incorrect;
                            - `dev create` is incorrect;
                            - `dev name` is not unique for a specified well and `dev create` is `CREATE`;
                            - you don't have permissions to create objects inside specified container (maybe 3rd party app is blocking this container);
                            '''
                            QtGui.QMessageBox.critical(self, "Error", errMsg)
                            continue

                        if p_d.readFile not in self.devDict:
                            QtGui.QMessageBox.critical(self, "Error", "Well dict doesn't contain: " + p_d.readFile)
                            continue

                        A = np.genfromtxt(p_d.readFile, skip_header = p_d.skipLines)

                        if len(A.shape) < 2 or p_d.coord_1_col >= A.shape[1] or p_d.coord_2_col >= A.shape[1] or p_d.coord_3_col >= A.shape[1]:
                            errMsg = 'In file: ' + p_d.readFile + '\nnumber of columns is less then some of the `coord_n col`'
                            QtGui.QMessageBox.critical(self, "Error", errMsg)
                            continue

                        val = True
                        if p_d.trajFormat == h5geo.TrajectoryFormat.MD_AZIM_INCL.name:
                            val &= h5devCurve.writeMD(A[:, p_d.coord_1_col])
                            val &= h5devCurve.writeAZIM(A[:, p_d.coord_2_col])
                            val &= h5devCurve.writeINCL(A[:, p_d.coord_3_col])
                        else:
                            # `traj2ALL` claims that all the length vars are in the same units
                            headXY = h5well.getHeadCoord()
                            kb = h5well.getKB()
                            A_ALL = h5geo.traj2ALL(A[:, [p_d.coord_1_col, p_d.coord_2_col, p_d.coord_3_col]], headXY[0], headXY[1], kb, p_d.angleUnits, h5geo.TrajectoryFormat.__members__[p_d.trajFormat], p_w.xNorth)
                            val &= h5devCurve.writeMD(A_ALL[:, 0])
                            val &= h5devCurve.writeAZIM(A_ALL[:, 7])
                            val &= h5devCurve.writeINCL(A_ALL[:, 8])

                        if not val:
                            errMsg = 'Can`t write data to dev curve: ' + p_d.devName + 'from well: ' + p_w.wellName + ' from container: ' + p_w.saveFile + '''
                            Possible reasons:
                            - `dev_data` dataset is broken (some attributes are missing or dataset is not resizable);
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
            QtGui.QApplication.restoreOverrideCursor()

        elif button == self.buttonBox.button(QtGui.QDialogButtonBox.Cancel):
            self.close()

