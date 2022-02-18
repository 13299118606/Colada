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

// Slicer includes
#include "qSlicerMainWindow.h"

class qColadaAppMainWindowPrivate;
class QDockWidget;
class qColadaH5MapTreeView;
class qColadaH5SeisTreeView;
class qColadaH5WellTreeView;

class Q_COLADA_APP_EXPORT qColadaAppMainWindow : public qSlicerMainWindow
{
  Q_OBJECT

public:
  typedef qSlicerMainWindow Superclass;

  qColadaAppMainWindow(QWidget *parent=0);
  virtual ~qColadaAppMainWindow() override;

public slots:
  void on_HelpAboutColadaAppAction_triggered();
  void onCurrentStyleChanged(const QString& name);

  void emitH5FileToBeAdded(const QString& fileName);
  void emitH5FileToBeRemoved(const QString& fileName);
  void emitH5ObjectToBeAdded(const QString& fileName, const QString& groupName);
  void emitH5ObjectToBeRemoved(const QString& fileName, const QString& groupName);

signals:
  void h5FileToBeRemoved(const QString& fileName);
  void h5FileToBeAdded(const QString& fileName);
  void h5ObjectToBeRemoved(const QString& fileName, const QString& groupName);
  void h5ObjectToBeAdded(const QString& fileName, const QString& groupName);

protected:
  void showEvent(QShowEvent *event) override;

protected:
  qColadaAppMainWindow(qColadaAppMainWindowPrivate* pimpl, QWidget* parent);

private:
  Q_DECLARE_PRIVATE(qColadaAppMainWindow);
  Q_DISABLE_COPY(qColadaAppMainWindow);
};

#endif
