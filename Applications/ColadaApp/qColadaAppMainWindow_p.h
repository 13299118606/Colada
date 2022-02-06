/*==============================================================================

  Copyright (c) Kitware, Inc.

  See http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Julien Finet, Kitware, Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qColadaAppMainWindow_p_h
#define __qColadaAppMainWindow_p_h

// Colada includes
#include "qColadaAppMainWindow.h"

// Slicer includes
#include "qSlicerMainWindow_p.h"

class QLabel;
class QDockWidget;
class qColadaH5MapTreeView;
class qColadaH5SeisTreeView;
class qColadaH5WellTreeView;
template<class T>
class vtkSmartPointer;
class vtkMatrix3x3;

//-----------------------------------------------------------------------------
class Q_COLADA_APP_EXPORT qColadaAppMainWindowPrivate
  : public qSlicerMainWindowPrivate
{
  Q_DECLARE_PUBLIC(qColadaAppMainWindow);

public:
  typedef qSlicerMainWindowPrivate Superclass;
  qColadaAppMainWindowPrivate(qColadaAppMainWindow& object);
  virtual ~qColadaAppMainWindowPrivate();

  virtual void init();
  virtual void setupPythonModules();
  /// Reimplemented for custom behavior
  virtual void setupUi(QMainWindow * mainWindow);

protected:
  virtual void setupStatusBar(QMainWindow* mainWindow);
  virtual void setupColadaStyles(QMainWindow * mainWindow);

public:
  QLabel *currentProjectLabel, *currentCRSLabel;
};

#endif
