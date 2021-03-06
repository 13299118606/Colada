#ifndef __qH5ItemDropLineEdit_h
#define __qH5ItemDropLineEdit_h

// Qt includes
#include <QWidget>

// Colada includes
#include "qColadaAppExport.h"

class qH5ItemDropLineEditPrivate;
class ctkPathLineEdit;
class QLineEdit;
class H5BaseObject;
class H5BaseContainer;

class Q_COLADA_APP_EXPORT qH5ItemDropLineEdit : public QWidget {
  Q_OBJECT

public:
  explicit qH5ItemDropLineEdit(QWidget *parent = nullptr);
  virtual ~qH5ItemDropLineEdit() override;

public slots:
  /// Open container. The user is responsible for deleting it
  H5BaseContainer* openGeoContainer();

  /// Open object. The user is responsible for deleting it
  H5BaseObject* openGeoObject();

  /// \brief setOrientation Default orientation is vertical
  /// \param orientation
  void setOrientation(Qt::Orientation orientation);

  void setH5Container(const QString& name);
  void setH5Object(const QString& name);
  /// emits only one signal `h5ItemChanged` instead of two
  void setH5Data(const QString& container, const QString& object);

  QString h5Container();
  QString h5Object();

  ctkPathLineEdit* getH5ContainerPathLineEdit();
  QLineEdit* getH5ContainerLineEdit();
  QLineEdit* getH5ObjectLineEdit();

signals:
  void h5ItemChanged(const QString& containerName, const QString& objectName);

protected:
  QScopedPointer<qH5ItemDropLineEditPrivate> d_ptr;

  explicit qH5ItemDropLineEdit(qH5ItemDropLineEditPrivate *pimpl,
    QWidget *parent);

  virtual void dragEnterEvent(QDragEnterEvent *event) override;
  virtual void dropEvent(QDropEvent *event) override;

protected slots:
  void onH5ContainerLineEditChanged(const QString& name);
  void onH5ObjectLineEditChanged(const QString& name);

private:
  Q_DECLARE_PRIVATE(qH5ItemDropLineEdit);
  Q_DISABLE_COPY(qH5ItemDropLineEdit);
};

#endif // qH5ItemDropLineEdit_h
