#ifndef __qColadaH5FileDialog_h
#define __qColadaH5FileDialog_h

// Qt includes
#include <QDialog>

// Colada includes
#include "qColadaAppExport.h"

class qColadaH5FileDialogPrivate;
class QAbstractButton;
class ctkPathLineEdit;
class QLineEdit;
class QComboBox;

class Q_COLADA_APP_EXPORT qColadaH5FileDialog : public QDialog {
  Q_OBJECT

public:
  explicit qColadaH5FileDialog(QWidget *parent = nullptr);
  virtual ~qColadaH5FileDialog() override;

  ///
  /// \brief getOpenH5FileDialog convenience to open dialog
  /// and get chosen parameters.
  /// All arguments passed by reference are changed only if
  /// QDialog::Accepted is returned.
  /// \param parent
  /// \param h5File
  /// \param h5FileCreateType is a casted h5geo::CreationType
  /// \param h5Obj
  /// \param h5ObjCreateType is a casted h5geo::CreationType
  /// \return QDialog::Accepted or QDialog::Rejected
  static int getOpenH5FileDialog(
      QWidget *parent,
      QString& h5File,
      unsigned& h5FileCreateType,
      QString& h5Obj,
      unsigned& h5ObjCreateType);

public slots:
  ctkPathLineEdit* getH5FilePathLineEdit();
  QLineEdit* getH5ObjectLineEdit();
  QComboBox* getH5FileCreateTypeComboBox();
  QComboBox* getH5ObjectCreateTypeComboBox();
  virtual void onButtonBoxClicked(QAbstractButton *button);

protected:
  QScopedPointer<qColadaH5FileDialogPrivate> d_ptr;

  explicit qColadaH5FileDialog(qColadaH5FileDialogPrivate *pimpl,
                               QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qColadaH5FileDialog);
  Q_DISABLE_COPY(qColadaH5FileDialog);
};

#endif // __qColadaH5FileDialog_h
