// MRMLDisplayableManager includes
#include "vtkSlicerStackLoadDisplayableManager.h"

// Qt includes
#include <QColor>

// MRML includes
#include <vtkMRMLAbstractViewNode.h>
//#include <vtkMRMLAstroVolumeDisplayNode.h>
//#include <vtkMRMLAstroVolumeNode.h>
//#include <vtkMRMLAstroLabelMapVolumeDisplayNode.h>
//#include <vtkMRMLAstroLabelMapVolumeNode.h>
#include <vtkMRMLLogic.h>
#include <vtkMRMLSliceLayerLogic.h>
#include <vtkMRMLSliceLogic.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLViewNode.h>

// VTK includes
#include <vtkActor2D.h>
#include <vtkActor2DCollection.h>
#include <vtkAxisActor2D.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkGeneralTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkLine.h>
#include <vtksys/SystemTools.hxx>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkDoubleArray.h>

// STD includes
#include <sstream>

// SlicerQt includes
#include <qSlicerApplication.h>

// vtkSlicer includes
#include <vtkSlicerApplicationLogic.h>

// Constants
static const int RENDERER_LAYER = 1; // layer ID where the orientation marker will be displayed
//const double PI_2  = PI * 0.5;
const double PI_2  = 3.14 * 0.5;

namespace
{
//----------------------------------------------------------------------------
template <typename T> T StringToNumber(const char* num)
{
  std::stringstream ss;
  ss << num;
  T result;
  return ss >> result ? result : 0;
}

//----------------------------------------------------------------------------
int StringToInt(const char* str)
{
  return StringToNumber<int>(str);
}

//----------------------------------------------------------------------------
double StringToDouble(const char* str)
{
  return StringToNumber<double>(str);
}

}// end namespace

//---------------------------------------------------------------------------
class vtkStackLoadRendererUpdateObserver : public vtkCommand
{
public:
  static vtkStackLoadRendererUpdateObserver *New()
    {
    return new vtkStackLoadRendererUpdateObserver;
    }
  vtkStackLoadRendererUpdateObserver()
    {
    this->DisplayableManager = nullptr;
    }
  virtual void Execute(vtkObject* vtkNotUsed(wdg), unsigned long vtkNotUsed(event), void* vtkNotUsed(calldata))
    {
    if (this->DisplayableManager)
      {
      this->DisplayableManager->UpdateFromRenderer();
      }
  }
  vtkWeakPointer<vtkSlicerStackLoadDisplayableManager> DisplayableManager;
};

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerStackLoadDisplayableManager);

//---------------------------------------------------------------------------
class vtkSlicerStackLoadDisplayableManager::vtkInternal
{
public:
  vtkInternal(vtkSlicerStackLoadDisplayableManager * external);
  ~vtkInternal();

  void SetupMarkerRenderer();
  void AddRendererUpdateObserver(vtkRenderer* renderer);
  void RemoveRendererUpdateObserver();

  void SetupAxes();
  void UpdateAxes();
  void ShowActors(bool show);

  vtkSmartPointer<vtkRenderer> MarkerRenderer;
  vtkSmartPointer<vtkStackLoadRendererUpdateObserver> RendererUpdateObserver;
  vtkSmartPointer<vtkPoints> twoDAxesPoints;
  vtkSmartPointer<vtkCellArray> twoDAxesCellArray;
  vtkSmartPointer<vtkPolyData> twoDAxesPolyData;
  vtkSmartPointer<vtkActor2D> twoDAxesActor;
  vtkSmartPointer<vtkPolyDataMapper2D> twoDAxesMapper;
  vtkSmartPointer<vtkCollection> col;

  int RendererUpdateObservationId;
  vtkWeakPointer<vtkRenderer> ObservedRenderer;

  bool ActorsAddedToRenderer;

  vtkSmartPointer<vtkDoubleArray> Color;
  static const double COLOR_INVALID[3];

  int fontSize;
  std::string fontStyle;

  vtkSlicerStackLoadDisplayableManager* External;

  qSlicerApplication* app;
};

//----------------------------------------------------------------------------
const double vtkSlicerStackLoadDisplayableManager::vtkInternal::COLOR_INVALID[3] = {1., 0.731, 0.078};

//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkSlicerStackLoadDisplayableManager::vtkInternal::vtkInternal(vtkSlicerStackLoadDisplayableManager * external)
{
  this->External = external;
  this->RendererUpdateObserver = vtkSmartPointer<vtkStackLoadRendererUpdateObserver>::New();
  this->RendererUpdateObserver->DisplayableManager = this->External;
  this->RendererUpdateObservationId = 0;
  this->ActorsAddedToRenderer = false;
  this->MarkerRenderer = vtkSmartPointer<vtkRenderer>::New();
  this->twoDAxesPoints = vtkSmartPointer<vtkPoints>::New();
  this->twoDAxesCellArray = vtkSmartPointer<vtkCellArray>::New();
  this->twoDAxesPolyData = vtkSmartPointer<vtkPolyData>::New();
  this->twoDAxesActor = vtkSmartPointer<vtkActor2D>::New();
  this->twoDAxesMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->col = vtkSmartPointer<vtkCollection>::New();
  this->app = nullptr;
  this->Color = vtkSmartPointer<vtkDoubleArray>::New();
  this->Color->SetNumberOfValues(3);
  this->Color->SetValue(0, COLOR_INVALID[0]);
  this->Color->SetValue(1, COLOR_INVALID[1]);
  this->Color->SetValue(2, COLOR_INVALID[2]);
  this->fontSize = 12;
  this->fontStyle = "Arial";
}

//---------------------------------------------------------------------------
vtkSlicerStackLoadDisplayableManager::vtkInternal::~vtkInternal()
{
  RemoveRendererUpdateObserver();
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::vtkInternal::ShowActors(bool show)
{
  if (this->ActorsAddedToRenderer == show)
    {
    // no change
    return;
    }
  if (show)
    {
    this->MarkerRenderer->AddViewProp(this->twoDAxesActor);
    }
  else
    {
    this->MarkerRenderer->RemoveViewProp(this->twoDAxesActor);
    }
  this->ActorsAddedToRenderer = show;
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::vtkInternal::AddRendererUpdateObserver(vtkRenderer* renderer)
{
  RemoveRendererUpdateObserver();
  if (renderer)
    {
    this->ObservedRenderer = renderer;
    this->RendererUpdateObservationId = renderer->AddObserver(vtkCommand::StartEvent, this->RendererUpdateObserver);
    }
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::vtkInternal::
    RemoveRendererUpdateObserver() {
  if (this->ObservedRenderer)
    {
    this->ObservedRenderer->RemoveObserver(this->RendererUpdateObservationId);
    this->RendererUpdateObservationId = 0;
    this->ObservedRenderer = nullptr;
    }
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::vtkInternal::SetupMarkerRenderer()
{
  vtkRenderer* renderer = this->External->GetRenderer();
  if (renderer==nullptr)
    {
    vtkErrorWithObjectMacro(this->External, "vtkSlicerStackLoadDisplayableManager"
                                            "::vtkInternal::SetupMarkerRenderer() failed: renderer is invalid");
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
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::vtkInternal::SetupAxes()
{
  this->twoDAxesActor->PickableOff();
  this->twoDAxesActor->DragableOff();
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::vtkInternal::UpdateAxes()
{

}

//---------------------------------------------------------------------------
// vtkMRMLRulerDisplayableManager methods

//---------------------------------------------------------------------------
vtkSlicerStackLoadDisplayableManager::vtkSlicerStackLoadDisplayableManager()
{
  this->Internal = new vtkInternal(this);
}

//---------------------------------------------------------------------------
vtkSlicerStackLoadDisplayableManager::~vtkSlicerStackLoadDisplayableManager()
{
  delete this->Internal;
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
vtkRenderer *vtkSlicerStackLoadDisplayableManager::vtkMarkerRenderer()
{
  return this->Internal->MarkerRenderer;
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::SetAnnotationsColor(double red,
                                                                 double green,
                                                                 double blue)
{
  this->Internal->Color->SetValue(0, red);
  this->Internal->Color->SetValue(1, green);
  this->Internal->Color->SetValue(2, blue);
  this->Internal->UpdateAxes();
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::SetAnnotationsFontStyle(const char *font)
{
  if (!font)
    {
    return;
    }

  this->Internal->fontStyle = font;
  this->Internal->UpdateAxes();
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::SetAnnotationsFontSize(int size)
{
  this->Internal->fontSize = size;
  this->Internal->UpdateAxes();
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::Create()
{
  this->Internal->SetupMarkerRenderer();
  this->Internal->SetupAxes();
  this->Superclass::Create();
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::UpdateFromViewNode()
{
  // View node is changed, which may mean that either the marker type (visibility), size, or orientation is changed
  this->Internal->UpdateAxes();
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::OnMRMLDisplayableNodeModifiedEvent(vtkObject* vtkNotUsed(caller))
{
  // view node is changed
  this->UpdateFromViewNode();
}

//---------------------------------------------------------------------------
void vtkSlicerStackLoadDisplayableManager::UpdateFromRenderer()
{
  // Rendering is performed, so let's re-render the marker with up-to-date orientation
  this->Internal->UpdateAxes();
}
