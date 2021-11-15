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

// Qt includes
#include <QDebug>
#include <QLinearGradient>
#include <QMenuBar>
#include <QPainter>
#include <QPalette>
#include <QPushButton>
#include <QStyleFactory>
#include <QStyleOption>
#include <QToolBar>

// CTK includes
#include <ctkCollapsibleButton.h>

// Colada includes
#include "qColadaStyle.h"

// --------------------------------------------------------------------------
// qColadaStyle methods

// --------------------------------------------------------------------------
qColadaStyle::qColadaStyle()
{
  // Slicer uses a QCleanlooksStyle as base style.
  this->setBaseStyle(new QProxyStyle(QStyleFactory::create("fusion")));
}

// --------------------------------------------------------------------------
qColadaStyle::~qColadaStyle()
{
}

//------------------------------------------------------------------------------
QPalette qColadaStyle::standardPalette()const
{
  QPalette palette = standardLightPalette();

  #ifdef Q_OS_WIN
    // Qt on macOS already dynamically picks light/dark theme based on the OS setting
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    if (settings.value("AppsUseLightTheme") == 0)
      {
      palette = standardDarkPalette();
      }
  #endif
  return palette;
}

QPalette qColadaStyle::standardLightPalette()const
{
  QPalette palette = this->Superclass::standardPalette();
  // Customizations from the standard style ("fusion")
  // See https://doc.qt.io/qt-5/qpalette.html#ColorRole-enum
  // Central color roles
  palette.setColor(QPalette::Window, Qt::white);
  palette.setColor(QPalette::WindowText, Qt::black);
  palette.setColor(QPalette::Disabled, QPalette::WindowText, "#bebebe");
  palette.setColor(QPalette::Base, Qt::white);
  palette.setColor(QPalette::Disabled, QPalette::Base, Qt::white);
  palette.setColor(QPalette::AlternateBase, "#e4e4fe");
  palette.setColor(QPalette::ToolTipBase, "#ffffdc");
  palette.setColor(QPalette::ToolTipText, Qt::black);
  palette.setColor(QPalette::Text, Qt::black);
  palette.setColor(QPalette::Disabled, QPalette::Text, "#bebebe");
  palette.setColor(QPalette::Button, "#fcfcfc");
  palette.setColor(QPalette::ButtonText, Qt::black);
  palette.setColor(QPalette::Disabled, QPalette::ButtonText, "#bebebe");
  palette.setColor(QPalette::BrightText, Qt::red);
  // Color roles used mostly for 3D bevel and shadow effects.
  palette.setColor(QPalette::Light, "#c8c8c8");  // Lighter than Button color.
  palette.setColor(QPalette::Midlight, "#e6e6e6");  // Between Button and Light.
  palette.setColor(QPalette::Dark, "#aaaaaa");  // Darker than Button.
  palette.setColor(QPalette::Mid, "#c8c8c8");  // Between Button and Dark.
  palette.setColor(QPalette::Shadow, "#5a5a5a");  // A very dark color.
  // Color roles relate to selected (marked) items
  palette.setColor(QPalette::Highlight, "#1e83fe");
  palette.setColor(QPalette::Disabled, QPalette::Highlight, "#919191");
  palette.setColor(QPalette::HighlightedText, Qt::white);
  palette.setColor(QPalette::Disabled, QPalette::HighlightedText, Qt::white);
  // Color roles related to hyperlinks
  palette.setColor(QPalette::Link, "#0000ff");
  return palette;
}

QPalette qColadaStyle::standardDarkPalette()const
{
  QPalette palette = this->Superclass::standardPalette();
  // See https://doc.qt.io/qt-5/qpalette.html#ColorRole-enum
  // Central color roles
  palette.setColor(QPalette::Window, "#323232");
  palette.setColor(QPalette::WindowText, Qt::white);
  palette.setColor(QPalette::Disabled, QPalette::WindowText, "#6d6d6d");
  palette.setColor(QPalette::Base, "#1e1e1e");
  palette.setColor(QPalette::Disabled, QPalette::Base, "#1e1e1e");
  palette.setColor(QPalette::AlternateBase, "#323232");
  palette.setColor(QPalette::ToolTipBase, "#ffa02f");
  palette.setColor(QPalette::ToolTipText, "#323232");
  palette.setColor(QPalette::Text, Qt::white);
  palette.setColor(QPalette::Disabled, QPalette::Text, "#6d6d6d");
  palette.setColor(QPalette::Button, "#323232");
  palette.setColor(QPalette::ButtonText, Qt::white);
  palette.setColor(QPalette::Disabled, QPalette::ButtonText, "#b4b4b4");
  palette.setColor(QPalette::BrightText, "#ff4444"); // Lighter than Qt::red
  // Color roles used mostly for 3D bevel and shadow effects.
  palette.setColor(QPalette::Light, "#828284");  // Lighter than Button color.
  palette.setColor(QPalette::Midlight, "#5a5a5b");  // Between Button and Light.
  palette.setColor(QPalette::Dark, "#232323");  // Darker than Button.
  palette.setColor(QPalette::Mid, "#2b2b2b");  // Between Button and Dark.
  palette.setColor(QPalette::Shadow, "#141414");  // A very dark color.
  // Color roles relate to selected (marked) items
  palette.setColor(QPalette::Highlight, "#e48a21");
  palette.setColor(QPalette::Disabled, QPalette::Highlight, "#505050");
  palette.setColor(QPalette::HighlightedText, Qt::white);
  palette.setColor(QPalette::Disabled, QPalette::HighlightedText, "#6d6d6d");
  // Color roles related to hyperlinks
  palette.setColor(QPalette::Link, "#ffa02f");
  return palette;
}

//------------------------------------------------------------------------------
void qColadaStyle::drawComplexControl(ComplexControl control,
                                   const QStyleOptionComplex* option,
                                   QPainter* painter,
                                   const QWidget* widget )const
{
  const_cast<QStyleOptionComplex*>(option)->palette =
    this->tweakWidgetPalette(option->palette, widget);
  this->Superclass::drawComplexControl(control, option, painter, widget);
}

//------------------------------------------------------------------------------
void qColadaStyle::drawControl(ControlElement element,
                            const QStyleOption* option,
                            QPainter* painter,
                            const QWidget* widget )const
{
  const_cast<QStyleOption*>(option)->palette =
    this->tweakWidgetPalette(option->palette, widget);

  // For some reason the toolbar paint routine is not respecting the palette.
  // here we make sure the background is correctly drawn.
  if (element == QStyle::CE_ToolBar &&
      qobject_cast<const QToolBar*>(widget))
    {
    painter->fillRect(option->rect, option->palette.brush(QPalette::Window));
    }
  this->Superclass::drawControl(element, option, painter, widget);
}

//------------------------------------------------------------------------------
void qColadaStyle::drawPrimitive(PrimitiveElement element,
                              const QStyleOption* option,
                              QPainter* painter,
                              const QWidget* widget )const
{
  const_cast<QStyleOption*>(option)->palette =
    this->tweakWidgetPalette(option->palette, widget);
  this->Superclass::drawPrimitive(element, option, painter, widget);
}

//------------------------------------------------------------------------------
QPalette qColadaStyle::tweakWidgetPalette(QPalette widgetPalette,
                                       const QWidget* widget)const
{
  if (!widget)
    {
    return widgetPalette;
    }
  const QPushButton* pushButton =
    qobject_cast<const QPushButton*>(widget);
  if (pushButton &&
      !pushButton->text().isEmpty())
    {
    QColor buttonColor = this->standardPalette().color(QPalette::Dark);
    widgetPalette.setColor(QPalette::Active, QPalette::Button, buttonColor);
    widgetPalette.setColor(QPalette::Inactive, QPalette::Button, buttonColor);
    QColor disabledButtonColor = buttonColor.toHsv();
    disabledButtonColor.setHsvF(disabledButtonColor.hueF(),
                                disabledButtonColor.saturationF() * 0.8,
                                disabledButtonColor.valueF() * 0.9);
    widgetPalette.setColor(QPalette::Disabled, QPalette::Button, disabledButtonColor);
    QColor buttonTextColor =
      this->standardPalette().color(QPalette::Light);
    widgetPalette.setColor(QPalette::Active, QPalette::ButtonText, buttonTextColor);
    widgetPalette.setColor(QPalette::Inactive, QPalette::ButtonText, buttonTextColor);
    QColor disabledButtonTextColor = buttonTextColor.toHsv();
    disabledButtonTextColor.setHsvF(buttonColor.hueF(),
                                    disabledButtonTextColor.saturationF() * 0.3,
                                    disabledButtonTextColor.valueF() * 0.8);
    widgetPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledButtonColor);
    }
  if (qobject_cast<const QMenuBar*>(widget))
    {
    QColor highlightColor = this->standardPalette().color(QPalette::Dark);
    //QBrush highlightBrush = this->standardPalette().brush(QPalette::Dark);
    QColor highlightTextColor =
      this->standardPalette().color(QPalette::Light);
    QBrush highlightTextBrush =
      this->standardPalette().brush(QPalette::Light);
    QColor darkColor = this->standardPalette().color(QPalette::Highlight);
    QColor lightColor =
      this->standardPalette().color(QPalette::HighlightedText);

    QLinearGradient hilightGradient(0., 0., 0., 1.);
    hilightGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    hilightGradient.setColorAt(0., highlightColor);
    hilightGradient.setColorAt(1., highlightColor.darker(120));
    QBrush highlightBrush(hilightGradient);

    widgetPalette.setColor(QPalette::Highlight, darkColor);
    widgetPalette.setColor(QPalette::HighlightedText, lightColor);

    widgetPalette.setColor(QPalette::Window, highlightColor);
    widgetPalette.setColor(QPalette::WindowText, highlightTextColor);
    widgetPalette.setColor(QPalette::Base, highlightColor);
    widgetPalette.setColor(QPalette::Text, highlightTextColor);
    widgetPalette.setColor(QPalette::Button, highlightColor);
    widgetPalette.setColor(QPalette::ButtonText, highlightTextColor);

    widgetPalette.setBrush(QPalette::Window, highlightBrush);
    widgetPalette.setBrush(QPalette::WindowText, highlightTextBrush);
    widgetPalette.setBrush(QPalette::Base, highlightBrush);
    widgetPalette.setBrush(QPalette::Text, highlightTextBrush);
    widgetPalette.setBrush(QPalette::Button, highlightBrush);
    widgetPalette.setBrush(QPalette::ButtonText, highlightTextBrush);
    }
/*
  QWidget* parentWidget = widget->parentWidget();
  QWidget* grandParentWidget = parentWidget? parentWidget->parentWidget() : 0;
  if (qobject_cast<const QToolBar*>(widget) ||
      qobject_cast<QToolBar*>(parentWidget) ||
      qobject_cast<QToolBar*>(grandParentWidget))
    {
    QColor windowColor = this->standardPalette().color(QPalette::Window);

    //QColor highlightColor = this->standardPalette().color(QPalette::Highlight);
    //QColor highlightTextColor =
    //  this->standardPalette().color(QPalette::HighlightedText);
    //QColor darkColor = this->standardPalette().color(QPalette::Dark);
    //QColor lightColor =
    //  this->standardPalette().color(QPalette::Light);
    QColor highlightColor = this->standardPalette().color(QPalette::Dark);
    //QBrush highlightBrush = this->standardPalette().brush(QPalette::Dark);
    QBrush highlightTextBrush =
      this->standardPalette().brush(QPalette::Light);
    QColor darkColor = this->standardPalette().color(QPalette::Highlight);
    QColor lightColor =
      this->standardPalette().color(QPalette::HighlightedText);

    QLinearGradient hilightGradient(0., 0., 0., 1.);
    hilightGradient.setCoordinateMode(QGradient::ObjectBoundingMode);

    hilightGradient.setColorAt(0., highlightColor);
    hilightGradient.setColorAt(1., highlightColor.darker(140));
    QBrush highlightBrush(hilightGradient);

    widgetPalette.setColor(QPalette::Highlight, darkColor);
    widgetPalette.setColor(QPalette::HighlightedText, lightColor);
    widgetPalette.setBrush(QPalette::Window, highlightBrush);
    widgetPalette.setBrush(QPalette::WindowText, highlightTextBrush);
    widgetPalette.setBrush(QPalette::Base, highlightBrush);
    widgetPalette.setBrush(QPalette::Text, highlightTextBrush);
    widgetPalette.setBrush(QPalette::Button, highlightBrush);
    widgetPalette.setBrush(QPalette::ButtonText, highlightTextBrush);
    }
*/
  return widgetPalette;
}

//------------------------------------------------------------------------------
void qColadaStyle::polish(QWidget* widget)
{
  this->Superclass::polish(widget);
  ctkCollapsibleButton* collapsibleButton =
    qobject_cast<ctkCollapsibleButton*>(widget);
  if (collapsibleButton)
    {
    collapsibleButton->setFlat(true);
    collapsibleButton->setContentsFrameShadow(QFrame::Sunken);
    }
}
