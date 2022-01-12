// Stack3D Widgets includes
#include "qSlicerCaptionsWidget.h"
#include "ui_qSlicerCaptionsWidget.h"
#include "vtkSlicerAxesLogic.h"

// Qt includes
#include <QToolButton>
#include <QLineEdit>

// Slicer includes
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"

// CTK includes
#include "ctkPopupWidget.h"
#include "ctkDoubleSlider.h"
#include "ctkSliderWidget.h"

// VTK includes
#include "vtkCubeAxesActor.h"
#include "vtkRenderer.h"
#include "vtkProperty.h"
#include "vtkTextProperty.h"

// MRML includes
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLCaptionsDisplayableManager.h"
#include "vtkMRMLDisplayableNode.h"

// MRMLQt includes
#include "qMRMLSliceWidget.h"
#include "qMRMLSliceView.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLThreeDView.h"
#include "qMRMLThreeDViewControllerWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Captions
class qSlicerCaptionsWidgetPrivate
  : public Ui_qSlicerCaptionsWidget
{
  Q_DECLARE_PUBLIC(qSlicerCaptionsWidget);
protected:
  qSlicerCaptionsWidget* const q_ptr;

public:
  qSlicerCaptionsWidgetPrivate(
    qSlicerCaptionsWidget& object);
  virtual void setupUi(qSlicerCaptionsWidget*);

  vtkMRMLCaptionsDisplayableManager*
  getCurrentCaptionsDisplayableManager();

  qSlicerApplication* app = nullptr;
};

// --------------------------------------------------------------------------
qSlicerCaptionsWidgetPrivate
::qSlicerCaptionsWidgetPrivate(
  qSlicerCaptionsWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerCaptionsWidgetPrivate
::setupUi(qSlicerCaptionsWidget* widget)
{
  Q_Q(qSlicerCaptionsWidget);

  this->Ui_qSlicerCaptionsWidget::setupUi(widget);

  app = qSlicerApplication::application();
}

vtkMRMLCaptionsDisplayableManager*
qSlicerCaptionsWidgetPrivate::getCurrentCaptionsDisplayableManager()
{
  if (!viewNodeSelector->currentNode())
    return nullptr;

  char* name = viewNodeSelector->currentNode()->GetName();
  qMRMLThreeDWidget* threeDWidget = app->layoutManager()->threeDWidget(
        QString::fromUtf8(name));

  if (!threeDWidget)
    return nullptr;

  return vtkMRMLCaptionsDisplayableManager::SafeDownCast(
        threeDWidget->threeDView()->
        displayableManagerByClassName(
          "vtkMRMLCaptionsDisplayableManager"));
}

//-----------------------------------------------------------------------------
// qSlicerCaptionsWidget methods

//-----------------------------------------------------------------------------
qSlicerCaptionsWidget
::qSlicerCaptionsWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerCaptionsWidgetPrivate(*this) )
{
  Q_D(qSlicerCaptionsWidget);
  d->setupUi(this);

  connect(d->showCaptionsBtn, &QAbstractButton::toggled,
          this, &qSlicerCaptionsWidget::onCaptionsBtnToggled);

  QList<ctkCollapsibleGroupBox*> groupBoxList(
        {
          d->captionsSettingsCollapsibleGroupBox
        });

  for (ctkCollapsibleGroupBox* groupBox : groupBoxList){
    for (const auto& child : groupBox->children()){
      QCheckBox* checkBox = qobject_cast<QCheckBox*>(child);
      if (checkBox){
        connect(checkBox, &QCheckBox::stateChanged,
                this, &qSlicerCaptionsWidget::onCheckBoxStateChanged);
        continue;
      }
      ctkColorPickerButton* colorBtn = qobject_cast<ctkColorPickerButton*>(child);
      if (colorBtn){
        connect(colorBtn, &ctkColorPickerButton::colorChanged,
                this, &qSlicerCaptionsWidget::onColorChanged);
        continue;
      }
      QLineEdit* lineEdit = qobject_cast<QLineEdit*>(child);
      if (lineEdit){
        connect(lineEdit, &QLineEdit::textChanged,
                this, &qSlicerCaptionsWidget::onTextChanged);
        continue;
      }
      QSpinBox* spinBox = qobject_cast<QSpinBox*>(child);
      if (spinBox){
        connect(spinBox, qOverload<int>(&QSpinBox::valueChanged),
                this, &qSlicerCaptionsWidget::onSpinBoxValueChanged);
        continue;
      }
      QDoubleSpinBox* dSpinBox = qobject_cast<QDoubleSpinBox*>(child);
      if (dSpinBox){
        connect(dSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged),
                this, &qSlicerCaptionsWidget::onDoubleSpinBoxValueChanged);
        continue;
      }
      QComboBox* combo = qobject_cast<QComboBox*>(child);
      if (combo){
        connect(combo, qOverload<int>(&QComboBox::currentIndexChanged),
                this, &qSlicerCaptionsWidget::onComboBoxIndexChanged);
        continue;
      }
      ctkSliderWidget* sliderWidget = qobject_cast<ctkSliderWidget*>(child);
      if (sliderWidget){
        connect(sliderWidget, &ctkSliderWidget::valueChanged,
                this, &qSlicerCaptionsWidget::onDoubleSpinBoxValueChanged);
        continue;
      }
    }
  }
}

//-----------------------------------------------------------------------------
qSlicerCaptionsWidget
::~qSlicerCaptionsWidget()
{
}

void qSlicerCaptionsWidget::setCaptionsBtnToggled(bool val)
{
  Q_D(qSlicerCaptionsWidget);
  d->showCaptionsBtn->setChecked(val);
}

void qSlicerCaptionsWidget::onCheckBoxStateChanged(int state)
{
  setupCaptionsFromWidget();
}

void qSlicerCaptionsWidget::onColorChanged(QColor c)
{
  setupCaptionsFromWidget();
}

void qSlicerCaptionsWidget::onTextChanged(const QString& text)
{
  setupCaptionsFromWidget();
}

void qSlicerCaptionsWidget::onSpinBoxValueChanged(int val)
{
  setupCaptionsFromWidget();
}

void qSlicerCaptionsWidget::onDoubleSpinBoxValueChanged(double val)
{
  setupCaptionsFromWidget();
}

void qSlicerCaptionsWidget::onComboBoxIndexChanged(int val)
{
  setupCaptionsFromWidget();
}

void qSlicerCaptionsWidget::onCaptionsBtnToggled(bool val)
{
  Q_D(qSlicerCaptionsWidget);
  vtkMRMLCaptionsDisplayableManager* captionsDM = d->getCurrentCaptionsDisplayableManager();

  if (!captionsDM)
      return;

  captionsDM->SetCaptionsVisibility(val);
  captionsDM->RequestRender();

  emit captionsVisibilityToggled(val, d->viewNodeSelector->currentNode());
}

void qSlicerCaptionsWidget::setupCaptionsFromWidget()
{
  Q_D(qSlicerCaptionsWidget);
  vtkMRMLCaptionsDisplayableManager* captionsDM = d->getCurrentCaptionsDisplayableManager();

  if (!captionsDM)
      return;
  this->qvtkBlockAll();

  /*-------- START OF THE SETTINGS --------*/

  captionsDM->SetOrientation(d->orientatoinSliderWidget->value());
  captionsDM->SetOpacity(d->opacitySliderWidget->value());
  captionsDM->SetFontSize(d->fontSizeSpinBox->value());
  captionsDM->SetBold(d->boldFontCheckBox->isChecked());
  captionsDM->SetItalic(d->italicFontCheckBox->isChecked());
  captionsDM->SetShadow(d->shadowFontCheckBox->isChecked());
  captionsDM->SetFrame(d->frameCheckBox->isChecked());
  captionsDM->SetFrameWidth(d->frameWidthSpinBox->value());
  captionsDM->SetRelativePosition(
        d->xPositionSliderWidget->value(),
        d->yPositionSliderWidget->value(),
        d->zPositionSliderWidget->value());

  QColor c;
  c = d->fontColorBtn->color();
  captionsDM->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->frameColorBtn->color();
  captionsDM->SetFrameColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);

  /*-------- END OF THE SETTINGS --------*/
  this->qvtkUnblockAll();
  captionsDM->RequestRender();
}
