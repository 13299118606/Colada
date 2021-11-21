#ifndef __qH5ItemDropLineEdit_h
#define __qH5ItemDropLineEdit_h

// Qt includes
#include <QWidget>

// Colada includes
#include "qColadaAppExport.h"

class qH5ItemDropLineEditPrivate;
class QLineEdit;

class Q_COLADA_APP_EXPORT qH5ItemDropLineEdit : public QWidget {
  Q_OBJECT

public:
  explicit qH5ItemDropLineEdit(QWidget *parent = nullptr);
  virtual ~qH5ItemDropLineEdit() override;

public slots:
  /// \brief setOrientation Default orientation is horizontal
  /// (i.e. both line edits are set in the first row of a grid layout).
  /// \param orientation
  void setOrientation(Qt::Orientation orientation);

  void setH5Container(QString name);
  void setH5Object(QString name);
  /// emits only one signal `h5ItemChanged` instead of two
  void setH5Data(QString container, QString object);

  QString h5Container();
  QString h5Object();

  QLineEdit* getH5ContainerLineEdit();
  QLineEdit* getH5ObjectLineEdit();

signals:
  void h5ItemChanged(QString containerName, QString objectName);

protected:
  QScopedPointer<qH5ItemDropLineEditPrivate> d_ptr;

  explicit qH5ItemDropLineEdit(qH5ItemDropLineEditPrivate *pimpl,
    QWidget *parent);

  virtual void dragEnterEvent(QDragEnterEvent *event) override;
  virtual void dropEvent(QDropEvent *event) override;

protected slots:
  void onH5ContainerLineEditChanged(QString name);
  void onH5ObjectLineEditChanged(QString name);

private:
  Q_DECLARE_PRIVATE(qH5ItemDropLineEdit);
  Q_DISABLE_COPY(qH5ItemDropLineEdit);
};

#endif // qH5ItemDropLineEdit_h
