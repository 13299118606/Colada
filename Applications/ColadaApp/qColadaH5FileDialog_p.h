#ifndef __qColadaH5FileDialog_p_h
#define __qColadaH5FileDialog_p_h

// Colada includes
#include "qColadaH5FileDialog.h"

class QFormLayout;
class QComboBox;
class QLineEdit;
class ctkPathLineEdit;
class QDialogButtonBox;

class Q_COLADA_APP_EXPORT qColadaH5FileDialogPrivate
{
    Q_DECLARE_PUBLIC(qColadaH5FileDialog);

protected:
    qColadaH5FileDialog* const q_ptr;

public:
    typedef qColadaH5FileDialogPrivate Superclass;
    qColadaH5FileDialogPrivate(qColadaH5FileDialog& q);
    virtual ~qColadaH5FileDialogPrivate();

    virtual void init();

    QFormLayout* formLayout;
    ctkPathLineEdit* filePathLineEdit;
    QLineEdit* objNameLineEdit;
    QComboBox* fileCreateTypeCombo, *objCreateTypeCombo;
    QDialogButtonBox* buttonBox;
};

#endif // __qColadaH5FileDialog_p_h
