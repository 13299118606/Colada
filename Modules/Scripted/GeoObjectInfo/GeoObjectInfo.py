import os
import vtk, qt, ctk, slicer
from slicer.ScriptedLoadableModule import *
import logging
from slicer.util import VTKObservationMixin
from h5gtpy import h5gt
from h5geopy import h5geo
import colada
# from Resources import GeoObjectInfoResourcesResources

class GeoObjectInfo(ScriptedLoadableModule):
  """Uses ScriptedLoadableModule base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "Geo Object Info" # TODO make this more human readable by adding spaces
    self.parent.categories = ["Colada"]
    self.parent.dependencies = []
    self.parent.contributors = ["kerim khemraev (Homeless Inc.)"]
    self.parent.helpText = """This is the GeoObjectInfo module for the custom application"""
    self.parent.helpText += self.getDefaultModuleDocumentationLink()
    self.parent.acknowledgementText = """...""" # replace with organization, grant and thanks.


class GeoObjectInfoWidget(ScriptedLoadableModuleWidget, VTKObservationMixin):
  """Uses ScriptedLoadableModuleWidget base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def __init__(self, parent):
    ScriptedLoadableModuleWidget.__init__(self, parent)
    VTKObservationMixin.__init__(self)
  
  def setup(self):
    ScriptedLoadableModuleWidget.setup(self)

    # Load widget from .ui file (created by Qt Designer)
    self.uiWidget = slicer.util.loadUI(self.resourcePath('UI/GeoObjectInfo.ui'))
    self.layout.addWidget(self.uiWidget)
    self.ui = slicer.util.childWidgetVariables(self.uiWidget)
    self.ui.inputVLayout = qt.QVBoxLayout(self.ui.inputsCollapsibleButton)
    self.ui.h5ItemDropLineEdit = colada.qH5ItemDropLineEdit()
    self.ui.h5ItemDropLineEdit.setOrientation(qt.Qt.Vertical)
    self.ui.inputVLayout.addWidget(self.ui.h5ItemDropLineEdit)

    re = qt.QRegExp("^[a-zA-Z_$][a-zA-Z_$0-9]*$")
    cntVarNameLineEditValidator = qt.QRegExpValidator(self.ui.cntVarNameLineEdit)
    objVarNameLineEditValidator = qt.QRegExpValidator(self.ui.objVarNameLineEdit)
    cntVarNameLineEditValidator.setRegExp(re)
    objVarNameLineEditValidator.setRegExp(re)
    self.ui.cntVarNameLineEdit.setValidator(cntVarNameLineEditValidator)
    self.ui.objVarNameLineEdit.setValidator(objVarNameLineEditValidator)

    #Create logic class
    self.logic = GeoObjectInfoLogic()

    # Connections
    self.ui.h5ItemDropLineEdit.connect("h5ItemChanged(const QString&, const QString&)", self.onH5ItemChanged)
    self.ui.createCntVarBtn.connect("clicked()", self.onCreateCntVarBtnClicked)
    self.ui.createObjVarBtn.connect("clicked()", self.onCreateObjVarBtnClicked)

  def onH5ItemChanged(self, cntName, objName):
    file = None
    try:
      file = h5gt.File(cntName, h5gt.OpenFlag.ReadOnly)
      self.ui.cntTypeLabel.setText(h5geo.openContainer(file).__class__.__name__)
    except h5gt.Exception as exc:
      self.ui.cntTypeLabel.setText("")
      self.ui.objTypeLabel.setText("")
      return

    group = None
    try:
      group = file.getGroup(objName)
      self.ui.objTypeLabel.setText(h5geo.openObject(group).__class__.__name__)
    except h5gt.Exception as exc:
      self.ui.objTypeLabel.setText("")
      return

    cntVarName: str = self.ui.cntVarNameLineEdit.text
    if not cntVarName:
      cntVarName = 'cnt'

    objVarName: str = self.ui.objVarNameLineEdit.text
    if not objVarName:
      objVarName = 'obj'

    self.ui.cntVarNameLineEdit.setText(cntVarName)
    self.ui.objVarNameLineEdit.setText(objVarName)

  def onCreateCntVarBtnClicked(self):
    pm=slicer.app.pythonManager()
    varName = self.ui.cntVarNameLineEdit.text
    cntName = self.ui.h5ItemDropLineEdit.h5Container()
    pm.executeString("from h5geopy import h5geo")
    pm.executeString(varName+"=h5geo.openContainerByName('"+cntName+"')")
    pm.executeString("print('Created variable: " + varName + " (" + cntName
      + ") -> " + varName + "')")
    # Switch focus to the Python console so that the user can start typing immediately
    mw=slicer.util.mainWindow()
    mw.onPythonConsoleToggled(True)

  def onCreateObjVarBtnClicked(self):
    pm=slicer.app.pythonManager()
    varName = self.ui.objVarNameLineEdit.text
    cntName = self.ui.h5ItemDropLineEdit.h5Container()
    objName = self.ui.h5ItemDropLineEdit.h5Object()
    pm.executeString("from h5gtpy import h5gt")
    pm.executeString("from h5geopy import h5geo")
    pm.executeString(varName+"=h5geo.openObject(h5geo.openContainerByName('"+cntName+"').getH5File().getGroup('"+objName+"'))")
    pm.executeString("print('Created variable: " + varName + " (" + objName
      + ") -> " + varName + "')")
    # Switch focus to the Python console so that the user can start typing immediately
    mw=slicer.util.mainWindow()
    mw.onPythonConsoleToggled(True)

  def onClose(self, unusedOne, unusedTwo):
    pass

  def cleanup(self):
    pass

  
  


class GeoObjectInfoLogic(ScriptedLoadableModuleLogic):
  """This class should implement all the actual
  computation done by your module.  The interface
  should be such that other python code can import
  this class and make use of the functionality without
  requiring an instance of the Widget.
  Uses ScriptedLoadableModuleLogic base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def run(self, inputVolume, outputVolume, imageThreshold, enableScreenshots=0):
    """
    Run the actual algorithm
    """

    pass
  



class GeoObjectInfoTest(ScriptedLoadableModuleTest):
  """
  This is the test case for your scripted module.
  Uses ScriptedLoadableModuleTest base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def setUp(self):
    """ Do whatever is needed to reset the state - typically a scene clear will be enough.
    """
    slicer.mrmlScene.Clear(0)

  def runTest(self):
    """Run as few or as many tests as needed here.
    """
    self.setUp()
    self.test_GeoObjectInfo1()

  def test_GeoObjectInfo1(self):
    """ Ideally you should have several levels of tests.  At the lowest level
    tests should exercise the functionality of the logic with different inputs
    (both valid and invalid).  At higher levels your tests should emulate the
    way the user would interact with your code and confirm that it still works
    the way you intended.
    One of the most important features of the tests is that it should alert other
    developers when their changes will have an impact on the behavior of your
    module.  For example, if a developer removes a feature that you depend on,
    your test should break so they know that the feature is needed.
    """

    self.delayDisplay("Starting the test")
    #
    # first, get some data
    #

    logic = GeoObjectInfoLogic()
    self.delayDisplay('Test passed!')


#
# Class for avoiding python error that is caused by the method SegmentEditor::setup
# http://issues.slicer.org/view.php?id=3871
#
class GeoObjectInfoFileWriter(object):
  def __init__(self, parent):
    pass
