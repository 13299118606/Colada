/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Andras Lasso, PerkLab, Queen's University.

==============================================================================*/

// MRMLDisplayableManager includes
#include "vtkMRMLAxesDisplayableManager2D.h"

// MRML includes
#include <vtkMRMLAbstractViewNode.h>
#include <vtkMRMLLogic.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLViewNode.h>

// VTK includes
#include <vtkActor2D.h>
#include <vtkAxisActor2D.h>
#include <vtkCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtksys/SystemTools.hxx>

// STD includes
#include <sstream>

// Constants
static const int RENDERER_LAYER = 1; // layer ID where the orientation marker will be displayed
static const double RULER_LENGTH = 0.7; // normalized ruler length
static const double RULER_LINE_MARGIN_PYXEL = 2; // distance from slice border to ruler
static const double RULER_TICK_BASE_LENGTH_PYXEL = 7; // thin: major tick size = base, minor tick size = base/2; thick: length is 2x (as ratio of window height)

//---------------------------------------------------------------------------
class NiceScale
{
public:

  double minPoint;
  double maxPoint;
  double maxTicks = 10;
  double tickSpacing;
  double range;
  double niceMin;
  double niceMax;

public:
  /**
    * Instantiates a new instance of the NiceScale class.
    *
    * @param min the minimum data point on the axis
    * @param max the maximum data point on the axis
    */
  NiceScale(double min, double max){
    minPoint = min;
    maxPoint = max;
    calculate();
  }

  /**
    * Calculate and update values for tick spacing and nice
    * minimum and maximum data points on the axis.
    */
  void calculate(){
    range = niceNum(maxPoint - minPoint, false);
    tickSpacing = niceNum(range / (maxTicks - 1), true);
    niceMin = floor(minPoint / tickSpacing) * tickSpacing;
    niceMax = ceil(maxPoint / tickSpacing) * tickSpacing;
  }

  /**
    * Returns a "nice" number approximately equal to range Rounds
    * the number if round = true Takes the ceiling if round = false.
    *
    * @param range the data range
    * @param round whether to round the result
    * @return a "nice" number to be used for the data range
    */
  double niceNum(double range, bool round){
    double exponent; /** exponent of range */
    double fraction; /** fractional part of range */
    double niceFraction; /** nice, rounded fraction */

    exponent = floor(log10(range));
    fraction = range / pow(10.f, exponent);

    if (round)
    {   if (fraction < 1.5)
        niceFraction = 1;
      else if (fraction < 3)
        niceFraction = 2;
      else if (fraction < 7)
        niceFraction = 5;
      else
        niceFraction = 10;
    }
    else
    {   if (fraction <= 1)
        niceFraction = 1;
      else if (fraction <= 2)
        niceFraction = 2;
      else if (fraction <= 5)
        niceFraction = 5;
      else
        niceFraction = 10;
    }

    return niceFraction * pow(10, exponent);
  }

  /**
    * Sets the minimum and maximum data points for the axis.
    *
    * @param minPoint the minimum data point on the axis
    * @param maxPoint the maximum data point on the axis
    */
  void setMinMaxPoints(double minPoint, double maxPoint){
    this->minPoint = minPoint;
    this->maxPoint = maxPoint;
    calculate();
  }

  /**
    * Sets maximum number of tick marks we're comfortable with
    *
    * @param maxTicks the maximum number of tick marks for the axis
    */
  void setMaxTicks(double maxTicks){
    this->maxTicks = maxTicks;
    calculate();
  }

  /** minimum number of decimals in tick labels
    * use in sprintf statement:
    * sprintf(buf, "%.*f", decimals(), tickValue);
    */
  int decimals(void){
    double logTickX = log10(tickSpacing);
    if(logTickX >= 0)
      return 0;
    return (int)(abs(floor(logTickX)));
  }
};

class vtkRulerRendererUpdateObserver : public vtkCommand
{
public:
  static vtkRulerRendererUpdateObserver *New()
  {
    return new vtkRulerRendererUpdateObserver;
  }
  vtkRulerRendererUpdateObserver()
  {
    this->DisplayableManager = nullptr;
  }
  void Execute(vtkObject* vtkNotUsed(wdg), unsigned long vtkNotUsed(event), void* vtkNotUsed(calldata)) override
  {
    if (this->DisplayableManager)
    {
      this->DisplayableManager->UpdateFromRenderer();
    }
  }
  vtkWeakPointer<vtkMRMLAxesDisplayableManager2D> DisplayableManager;
};

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLAxesDisplayableManager2D );

//---------------------------------------------------------------------------
class vtkMRMLAxesDisplayableManager2D::vtkInternal
{
public:
  vtkInternal(vtkMRMLAxesDisplayableManager2D * external);
  ~vtkInternal();

  void SetupMarkerRenderer();
  void AddRendererUpdateObserver(vtkRenderer* renderer);
  void RemoveRendererUpdateObserver();

  enum RulerPos
  {
    RulerPosTop=0,
    RulerPosBottom,
    RulerPosLeft,
    RulerPosRight
  };

  void SetupRuler();

  void UpdateRulers();
  void UpdateRuler(
      vtkMRMLSliceNode* sliceNode,
      vtkAxisActor2D* ruler,
      RulerPos rulerPos);
  void ShowRuler(vtkAxisActor2D* ruler, bool show);
  bool IsRulerAddedToRenderer(vtkAxisActor2D* ruler);
  int GetRulerNum(vtkAxisActor2D* ruler);

  vtkSmartPointer<vtkRenderer> MarkerRenderer;
  vtkSmartPointer<vtkAxisActor2D>
  RulerLineActor_0, RulerLineActor_1,
  RulerLineActor_2, RulerLineActor_3;

  vtkSmartPointer<vtkRulerRendererUpdateObserver> RendererUpdateObserver;
  int RendererUpdateObservationId;
  vtkWeakPointer<vtkRenderer> ObservedRenderer;

  bool RulersAddedToRenderer[4];

  vtkMRMLAxesDisplayableManager2D* External;

  int RulerType, RulerColor;
};

//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMRMLAxesDisplayableManager2D::vtkInternal::vtkInternal(vtkMRMLAxesDisplayableManager2D * external)
{
  this->External = external;
  this->RendererUpdateObserver = vtkSmartPointer<vtkRulerRendererUpdateObserver>::New();
  this->RendererUpdateObserver->DisplayableManager = this->External;
  this->RendererUpdateObservationId = 0;
  this->RulersAddedToRenderer[0] = false;
  this->RulersAddedToRenderer[1] = false;
  this->RulersAddedToRenderer[2] = false;
  this->RulersAddedToRenderer[3] = false;
  this->MarkerRenderer = vtkSmartPointer<vtkRenderer>::New();
  // Prevent erasing Z-buffer (important for quick picking and markup label visibility assessment)
  this->MarkerRenderer->EraseOff();
  this->RulerLineActor_0 = vtkSmartPointer<vtkAxisActor2D>::New();
  this->RulerLineActor_1 = vtkSmartPointer<vtkAxisActor2D>::New();
  this->RulerLineActor_2 = vtkSmartPointer<vtkAxisActor2D>::New();
  this->RulerLineActor_3 = vtkSmartPointer<vtkAxisActor2D>::New();
  this->RulerType = vtkMRMLAbstractViewNode::RulerTypeThin;
  this->RulerColor = vtkMRMLAbstractViewNode::RulerColorWhite;
}

//---------------------------------------------------------------------------
vtkMRMLAxesDisplayableManager2D::vtkInternal::~vtkInternal()
{
  RemoveRendererUpdateObserver();
}

//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::vtkInternal::ShowRuler(
    vtkAxisActor2D* ruler, bool show)
{
  if (IsRulerAddedToRenderer(ruler) == show)
  {
    // no change
    return;
  }
  if (show)
  {
    this->MarkerRenderer->AddViewProp(ruler);
  }
  else
  {
    this->MarkerRenderer->RemoveViewProp(ruler);
  }
  int rulerNum = GetRulerNum(ruler);
  if (rulerNum >= 0 && rulerNum <= 3)
    RulersAddedToRenderer[rulerNum] = show;
}

bool vtkMRMLAxesDisplayableManager2D::vtkInternal::IsRulerAddedToRenderer(
    vtkAxisActor2D* ruler)
{
  if (ruler == this->RulerLineActor_0)
    return RulersAddedToRenderer[0];
  else if (ruler == this->RulerLineActor_1)
    return RulersAddedToRenderer[1];
  else if (ruler == this->RulerLineActor_2)
    return RulersAddedToRenderer[2];
  else if (ruler == this->RulerLineActor_3)
    return RulersAddedToRenderer[3];
  return false;
}

int vtkMRMLAxesDisplayableManager2D::vtkInternal::GetRulerNum(
    vtkAxisActor2D* ruler)
{
  if (ruler == this->RulerLineActor_0)
    return 0;
  else if (ruler == this->RulerLineActor_1)
    return 1;
  else if (ruler == this->RulerLineActor_2)
    return 2;
  else if (ruler == this->RulerLineActor_3)
    return 3;
  return -1;
}

//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::vtkInternal::AddRendererUpdateObserver(vtkRenderer* renderer)
{
  RemoveRendererUpdateObserver();
  if (renderer)
  {
    this->ObservedRenderer = renderer;
    this->RendererUpdateObservationId = renderer->AddObserver(vtkCommand::StartEvent, this->RendererUpdateObserver);
  }
}

//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::vtkInternal::RemoveRendererUpdateObserver()
{
  if (this->ObservedRenderer)
  {
    this->ObservedRenderer->RemoveObserver(this->RendererUpdateObservationId);
    this->RendererUpdateObservationId = 0;
    this->ObservedRenderer = nullptr;
  }
}

//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::vtkInternal::SetupMarkerRenderer()
{
  vtkRenderer* renderer = this->External->GetRenderer();
  if (renderer==nullptr)
  {
    vtkErrorWithObjectMacro(this->External, "vtkMRMLAxesDisplayableManager2D::vtkInternal::SetupMarkerRenderer() failed: renderer is invalid");
    return;
  }

  this->MarkerRenderer->InteractiveOff();

  vtkRenderWindow* renderWindow = renderer->GetRenderWindow();
  if (renderWindow->GetNumberOfLayers() < RENDERER_LAYER+1)
  {
    renderWindow->SetNumberOfLayers( RENDERER_LAYER+1 );
  }
  this->MarkerRenderer->SetLayer(RENDERER_LAYER);
  renderWindow->AddRenderer(this->MarkerRenderer);
  // Parallel projection is needed to prevent actors from warping/tilting
  // when they are near the edge of the window.
  vtkCamera* camera = this->MarkerRenderer->GetActiveCamera();
  if (camera)
  {
    camera->ParallelProjectionOn();
  }

  // In 3D viewers we need to follow the renderer and update the orientation marker accordingly
  vtkMRMLViewNode* threeDViewNode = vtkMRMLViewNode::SafeDownCast(this->External->GetMRMLDisplayableNode());
  if (threeDViewNode)
  {
    this->AddRendererUpdateObserver(renderer);
  }

}

//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::vtkInternal::SetupRuler()
{
  this->RulerLineActor_0->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedViewport();
  this->RulerLineActor_0->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedViewport();
  this->RulerLineActor_0->RulerModeOff(); // to allow specifying the number of labels (instead of distance between labels)
  this->RulerLineActor_0->AdjustLabelsOff(); // to allow specifying exact number of labels
  this->RulerLineActor_0->PickableOff();
  this->RulerLineActor_0->DragableOff();

  this->RulerLineActor_1->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedViewport();
  this->RulerLineActor_1->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedViewport();
  this->RulerLineActor_1->RulerModeOff(); // to allow specifying the number of labels (instead of distance between labels)
  this->RulerLineActor_1->AdjustLabelsOff(); // to allow specifying exact number of labels
  this->RulerLineActor_1->PickableOff();
  this->RulerLineActor_1->DragableOff();

  this->RulerLineActor_2->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedViewport();
  this->RulerLineActor_2->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedViewport();
  this->RulerLineActor_2->RulerModeOff(); // to allow specifying the number of labels (instead of distance between labels)
  this->RulerLineActor_2->AdjustLabelsOff(); // to allow specifying exact number of labels
  this->RulerLineActor_2->PickableOff();
  this->RulerLineActor_2->DragableOff();

  this->RulerLineActor_3->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedViewport();
  this->RulerLineActor_3->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedViewport();
  this->RulerLineActor_3->RulerModeOff(); // to allow specifying the number of labels (instead of distance between labels)
  this->RulerLineActor_3->AdjustLabelsOff(); // to allow specifying exact number of labels
  this->RulerLineActor_3->PickableOff();
  this->RulerLineActor_3->DragableOff();
}

//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::vtkInternal::UpdateRulers()
{
  vtkMRMLAbstractViewNode* viewNode = vtkMRMLAbstractViewNode::SafeDownCast(this->External->GetMRMLDisplayableNode());
  if (!viewNode)
  {
    vtkErrorWithObjectMacro(this->External, "vtkMRMLAxesDisplayableManager2D::UpdateMarkerOrientation() failed: view node is invalid");
    this->ShowRuler(this->RulerLineActor_0, false);
    this->ShowRuler(this->RulerLineActor_1, false);
    this->ShowRuler(this->RulerLineActor_2, false);
    this->ShowRuler(this->RulerLineActor_3, false);
    return;
  }

  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(viewNode);
  if (!sliceNode)
  {
    vtkErrorWithObjectMacro(this->External, "vtkMRMLAxesDisplayableManager2D::UpdateMarkerOrientation() failed: displayable node is invalid");
    this->ShowRuler(this->RulerLineActor_0, false);
    this->ShowRuler(this->RulerLineActor_1, false);
    this->ShowRuler(this->RulerLineActor_2, false);
    this->ShowRuler(this->RulerLineActor_3, false);
    return;
  }

  UpdateRuler(sliceNode, this->RulerLineActor_0, RulerPosTop);
  UpdateRuler(sliceNode, this->RulerLineActor_1, RulerPosBottom);
  UpdateRuler(sliceNode, this->RulerLineActor_2, RulerPosLeft);
  UpdateRuler(sliceNode, this->RulerLineActor_3, RulerPosRight);
}

void vtkMRMLAxesDisplayableManager2D::vtkInternal::UpdateRuler(
    vtkMRMLSliceNode* sliceNode,
    vtkAxisActor2D* ruler,
    RulerPos rulerPos)
{
  double sliceMin, sliceMax;
  if (sliceNode->GetOrientation().compare("XY") == 0 ||
      sliceNode->GetOrientation().compare("YX") == 0){
    if (rulerPos == RulerPosTop ||
        rulerPos == RulerPosBottom){
      sliceMin = sliceNode->GetXYToRAS()->GetElement(0,3);
      sliceMax = sliceMin + sliceNode->GetFieldOfView()[0];
    } else {
      sliceMin = sliceNode->GetXYToRAS()->GetElement(1,3);
      sliceMax = sliceMin + sliceNode->GetFieldOfView()[1];
    }
  } else if (sliceNode->GetOrientation().compare("XZ") == 0 ||
             sliceNode->GetOrientation().compare("ZX") == 0){
    if (rulerPos == RulerPosTop ||
        rulerPos == RulerPosBottom){
      sliceMin = sliceNode->GetXYToRAS()->GetElement(0,3);
      sliceMax = sliceMin + sliceNode->GetFieldOfView()[0];
    } else {
      sliceMin = sliceNode->GetXYToRAS()->GetElement(2,3);
      sliceMax = sliceMin + sliceNode->GetFieldOfView()[1];
    }
  } else if (sliceNode->GetOrientation().compare("YZ") == 0 ||
             sliceNode->GetOrientation().compare("ZY") == 0){
    if (rulerPos == RulerPosTop ||
        rulerPos == RulerPosBottom){
      sliceMin = sliceNode->GetXYToRAS()->GetElement(1,3);
      sliceMax = sliceMin + sliceNode->GetFieldOfView()[0];
    } else {
      sliceMin = sliceNode->GetXYToRAS()->GetElement(2,3);
      sliceMax = sliceMin + sliceNode->GetFieldOfView()[1];
    }
  } else {
    this->ShowRuler(ruler, false);
    return;
  }

  double sliceRange = sliceMax - sliceMin;
  double axisMin = sliceMin + sliceRange*(0.5-RULER_LENGTH/2);
  double axisMax = sliceMin + sliceRange*(0.5+RULER_LENGTH/2);

  int viewWidthPixel = sliceNode->GetDimensions()[0];
  int viewHeightPixel = sliceNode->GetDimensions()[1];

  NiceScale niceScale = NiceScale(axisMin, axisMax);

  // RULER_LINE_MARGIN_PYXEL is given in pixel and we need normalize them
  double p2[2], p1[2];
  if (rulerPos == RulerPosTop){
    p2[0] = (niceScale.niceMin-sliceMin) / sliceRange;
    p2[1] = 1 - (RULER_LINE_MARGIN_PYXEL/viewHeightPixel);
    p1[0] = (niceScale.niceMax-sliceMin) / sliceRange;
    p1[1] = 1 - (RULER_LINE_MARGIN_PYXEL/viewHeightPixel);
  } else if (rulerPos == RulerPosBottom){
    p2[0] = (niceScale.niceMin-sliceMin) / sliceRange;
    p2[1] = RULER_LINE_MARGIN_PYXEL/viewHeightPixel;
    p1[0] = (niceScale.niceMax-sliceMin) / sliceRange;
    p1[1] = RULER_LINE_MARGIN_PYXEL/viewHeightPixel;
  } else if (rulerPos == RulerPosLeft){
    p2[0] = RULER_LINE_MARGIN_PYXEL/viewWidthPixel;
    p2[1] = (niceScale.niceMin-sliceMin) / sliceRange;
    p1[0] = RULER_LINE_MARGIN_PYXEL/viewWidthPixel;
    p1[1] = (niceScale.niceMax-sliceMin) / sliceRange;
  } else if (rulerPos == RulerPosRight){
    p2[0] = 1 - (RULER_LINE_MARGIN_PYXEL/viewWidthPixel);
    p2[1] = (niceScale.niceMin-sliceMin) / sliceRange;
    p1[0] = 1 - (RULER_LINE_MARGIN_PYXEL/viewWidthPixel);
    p1[1] = (niceScale.niceMax-sliceMin) / sliceRange;
  }

  if (rulerPos == RulerPosBottom ||
      rulerPos == RulerPosRight){
    ruler->SetPoint2(p2);
    ruler->SetPoint1(p1);

    ruler->SetRange(
          niceScale.niceMax,
          niceScale.niceMin);
  } else {
    ruler->SetPoint2(p1);
    ruler->SetPoint1(p2);

    ruler->SetRange(
          niceScale.niceMin,
          niceScale.niceMax);
  }

//  ruler->GetLabelTextProperty()->SetFontSize(1000);
//  ruler->SizeFontRelativeToAxisOn();
//  ruler->UseFontSizeFromPropertyOn();
//  ruler->SetLabelFactor(250.0/double(viewHeightPixel));

//  ruler->SetFontFactor(250.0/double(viewHeightPixel));

  ruler->SetNumberOfLabels(
        (niceScale.niceMax-niceScale.niceMin)/niceScale.tickSpacing + 1);
  ruler->SetNumberOfMinorTicks(4);

  std::string labelFormat =
      "%." + std::to_string(niceScale.decimals()) + "f";
  ruler->SetLabelFormat(labelFormat.c_str());

  vtkProperty2D* lineProperty = ruler->GetProperty();
  switch (this->RulerType)
  {
  case vtkMRMLAbstractViewNode::RulerTypeThin:
    ruler->SetTickLength(RULER_TICK_BASE_LENGTH_PYXEL);
    ruler->SetMinorTickLength(RULER_TICK_BASE_LENGTH_PYXEL/2.0);
    lineProperty->SetLineWidth(1);
    break;
  case vtkMRMLAbstractViewNode::RulerTypeThick:
    ruler->SetTickLength(RULER_TICK_BASE_LENGTH_PYXEL*2);
    ruler->SetMinorTickLength(RULER_TICK_BASE_LENGTH_PYXEL);
    lineProperty->SetLineWidth(3);
    break;
  default:
    break;
  }

  switch (this->RulerColor)
  {
  case vtkMRMLAbstractViewNode::RulerColorWhite:
    lineProperty->SetColor(1,1,1);
    break;
  case vtkMRMLAbstractViewNode::RulerColorBlack:
    lineProperty->SetColor(0,0,0);
    break;
  case vtkMRMLAbstractViewNode::RulerColorYellow:
    lineProperty->SetColor(1,1,0);
    break;
  default:
    break;
  }

  this->ShowRuler(ruler, true);
}

//---------------------------------------------------------------------------
// vtkMRMLAxesDisplayableManager2D methods

//---------------------------------------------------------------------------
vtkMRMLAxesDisplayableManager2D::vtkMRMLAxesDisplayableManager2D()
{
  this->Internal = new vtkInternal(this);

  // Presets can be modified using AddRulerScalePreset(), so any application
  // that requires a different behavior can customize it by accessing the
  // displayable manager. It might be better to store this preset information
  // in the scene (add members to the view node or unit node, or add a new
  // ruler display node).

  //                        length, major, minor, unit, scale
  this->AddRulerScalePreset(  1e-2,   2,     5,    "um", 1e3 );
  this->AddRulerScalePreset(2.5e-2,   5,     5,    "um", 1e3 );
  this->AddRulerScalePreset(  5e-2,   5,     0,    "um", 1e3 );
  this->AddRulerScalePreset(  1e-1,   2,     5,    "um", 1e3 );
  this->AddRulerScalePreset(2.5e-1,   5,     5,    "um", 1e3 );
  this->AddRulerScalePreset(  5e-1,   5,     0,    "mm", 1e0 );
  this->AddRulerScalePreset(  1e0,    2,     5,    "mm", 1e0 );
  this->AddRulerScalePreset(2.5e0,    5,     5,    "mm", 1e0 );
  this->AddRulerScalePreset(  5e0,    5,     0,    "mm", 1e0 );
  this->AddRulerScalePreset(  1e1,    2,     5,    "mm", 1e0 );
  this->AddRulerScalePreset(2.5e1,    5,     5,    "mm", 1e0 );
  this->AddRulerScalePreset(  5e1,    5,     0,    "cm", 1e-1 );
  this->AddRulerScalePreset(  1e2,    2,     5,    "cm", 1e-1 );
  this->AddRulerScalePreset(2.5e2,    5,     5,    "cm", 1e-1 );
  this->AddRulerScalePreset(  5e2,    5,     0,    "cm", 1e-1 );
  this->AddRulerScalePreset(  1e3,    2,     5,     "m", 1e-3 );
  this->AddRulerScalePreset(2.5e3,    5,     5,     "m", 1e-3 );
  this->AddRulerScalePreset(  5e3,    5,     0,     "m", 1e-3 );
  this->AddRulerScalePreset(  1e4,    2,     5,     "m", 1e-3 );
  this->AddRulerScalePreset(2.5e4,    5,     5,     "m", 1e-3 );
  this->AddRulerScalePreset(  5e4,    5,     0,     "m", 1e-3 );
}

//---------------------------------------------------------------------------
vtkMRMLAxesDisplayableManager2D::~vtkMRMLAxesDisplayableManager2D()
{
  delete this->Internal;
}

//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::Create()
{
  this->Internal->SetupMarkerRenderer();
  this->Internal->SetupRuler();
  this->Superclass::Create();
}

//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::UpdateFromViewNode()
{
  // View node is changed, which may mean that either the marker type (visibility), size, or orientation is changed
  this->Internal->UpdateRulers();
}

//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::OnMRMLDisplayableNodeModifiedEvent(vtkObject* vtkNotUsed(caller))
{
  // view node is changed
  this->UpdateFromViewNode();
}

//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::UpdateFromRenderer()
{
  // Rendering is performed, so let's re-render the marker with up-to-date orientation
  this->Internal->UpdateRulers();
}

//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::AddRulerScalePreset(double length, int numberOfMajorDivisions, int numberOfMinorDivisions,
                                                          const std::string& displayedUnitName, double displayedScale)
{
  // insert into this->RulerScalePresets list, ordered by Length
  std::vector<RulerScalePreset>::iterator it = this->RulerScalePresets.begin();
  for (; it != this->RulerScalePresets.end(); ++it)
  {
    if (it->Length == length)
    {
      // found an exact match, update it
      it->NumberOfMajorDivisions = numberOfMajorDivisions;
      it->NumberOfMinorDivisions = numberOfMinorDivisions;
      it->DisplayedUnitName = displayedUnitName;
      it->DisplayedScale = displayedScale;
      return;
    }
    if (it->Length>length)
    {
      // this element's Length is larger, insert the new element before this
      break;
    }
  }
  RulerScalePreset preset;
  preset.Length = length;
  preset.NumberOfMajorDivisions = numberOfMajorDivisions;
  preset.NumberOfMinorDivisions = numberOfMinorDivisions;
  preset.DisplayedUnitName = displayedUnitName;
  preset.DisplayedScale = displayedScale;
  this->RulerScalePresets.insert(it, preset);
}


//---------------------------------------------------------------------------
void vtkMRMLAxesDisplayableManager2D::RemoveAllRulerScalePresets()
{
  this->RulerScalePresets.clear();
}
