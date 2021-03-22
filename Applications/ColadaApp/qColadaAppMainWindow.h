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

#ifndef __qColadaAppMainWindow_h
#define __qColadaAppMainWindow_h

// Colada includes
#include "qColadaAppExport.h"
class qColadaAppMainWindowPrivate;

// Slicer includes
#include "qSlicerMainWindow.h"

class Q_COLADA_APP_EXPORT qColadaAppMainWindow : public qSlicerMainWindow
{
  Q_OBJECT
public:
  typedef qSlicerMainWindow Superclass;

  qColadaAppMainWindow(QWidget *parent=0);
  virtual ~qColadaAppMainWindow();

public slots:
  void on_HelpAboutColadaAppAction_triggered();

protected:
  qColadaAppMainWindow(qColadaAppMainWindowPrivate* pimpl, QWidget* parent);

private:
  Q_DECLARE_PRIVATE(qColadaAppMainWindow);
  Q_DISABLE_COPY(qColadaAppMainWindow);
};

#endif
