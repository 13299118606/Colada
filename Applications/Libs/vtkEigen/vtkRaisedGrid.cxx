// Hide VTK_DEPRECATED_IN_9_0_0() warnings for this class.
#define VTK_DEPRECATION_LEVEL 0

#include "vtkRaisedGrid.h"

#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkGenericCell.h"
#include "vtkInformation.h"
#include "vtkInformationIntegerKey.h"
#include "vtkInformationVector.h"
#include "vtkLargeInteger.h"
#include "vtkLine.h"
#include "vtkMath.h"
#include "vtkMatrix3x3.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPixel.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkVertex.h"
#include "vtkVoxel.h"

// My includes
#include "vtkStructuredData.h"
#include "vtkDelaunay2D.h"
#include "vtkWedge.h"
#include "vtkPointInterpolator.h"

#include <cmath>

vtkStandardNewMacro(vtkRaisedGrid);
vtkStandardExtendedNewMacro(vtkRaisedGrid);

//------------------------------------------------------------------------------
vtkRaisedGrid::vtkRaisedGrid()
{
  this->Wedge = nullptr;

  this->DataDescription = VTK_EMPTY;

  this->SurfaceDelaunay2D = vtkDelaunay2D::New();

  this->SetDataDescription(VTK_XYZ_GRID);
}

//------------------------------------------------------------------------------
vtkRaisedGrid::~vtkRaisedGrid()
{
  this->SurfaceDelaunay2D->Delete();
  this->Wedge->Delete();
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
// Copy the geometric and topological structure of an input structured points
// object.
void vtkRaisedGrid::CopyStructure(vtkDataSet* ds)
{
  vtkRaisedGrid* sPts = static_cast<vtkRaisedGrid*>(ds);
  this->Initialize();

  vtkNew<vtkPoints> points;
  points->DeepCopy(sPts->GetSurfacePoints());
  this->SetSurfacePoints(points);

  if (!sPts->ZSpacings.empty())
    SetZSpacings(sPts->ZSpacings.data(), sPts->ZSpacings.size());
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::Initialize()
{
  this->Superclass::Initialize();
}

//----------------------------------------------------------------------------
vtkIdType vtkRaisedGrid::GetNumberOfPoints()
{
  return this->SurfaceDelaunay2D->GetOutput()->GetNumberOfPoints()*(ZSpacings.size()+1);
}

//------------------------------------------------------------------------------
vtkIdType vtkRaisedGrid::GetNumberOfCells()
{
  return this->SurfaceDelaunay2D->GetOutput()->GetNumberOfCells() *
      this->ZSpacings.size();
}

//----------------------------------------------------------------------------
double* vtkRaisedGrid::GetPoint(vtkIdType ptId)
{
  vtkIdType surfPointId, zPointId;
  this->GetSurfPointIdAndZPointIdFromPointId(ptId, surfPointId, zPointId);

  this->SurfaceDelaunay2D->GetOutput()->GetPoint(surfPointId, this->Point);

  double z = 0;
  for (vtkIdType i = 0; i < zPointId; i++)
    z += this->ZSpacings[i];

  this->Point[2] += z;

  return this->Point;
}

//------------------------------------------------------------------------------
vtkCell* vtkRaisedGrid::GetCell(vtkIdType cellId)
{
  vtkCell* cell = this->GetCellTemplateForDataDescription();
  if (cell == nullptr)
  {
    return nullptr;
  }

  AddPointsToCellTemplate(cell, cellId);

  return cell;
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::GetCell(vtkIdType cellId, vtkGenericCell* cell)
{
  if (!this->GetCellTemplateForDataDescription(cell))
  {
    cell->SetCellTypeToEmptyCell();
    return;
  }

  AddPointsToCellTemplate(cell, cellId);
}

//------------------------------------------------------------------------------
int vtkRaisedGrid::GetCellType(vtkIdType vtkNotUsed(cellId))
{
  switch (this->DataDescription)
  {
  case VTK_XYZ_GRID:
    return VTK_WEDGE;

  default:
    vtkErrorMacro(<< "Bad data description!");
    return VTK_EMPTY_CELL;
  }
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::GetCellPoints(vtkIdType cellId, vtkIdList* ptIds)
{
  vtkCell* cell = this->GetCell(cellId);
  if (!cell)
    return;

  ptIds = cell->GetPointIds();
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::GetPointCells(vtkIdType ptId, vtkIdList* cellIds)
{
  vtkIdType nSurfCells = this->SurfaceDelaunay2D->GetOutput()->GetNumberOfCells();
  vtkIdType nZCells = ZSpacings.size();

  vtkIdType surfPointId, zPointId;
  this->GetSurfPointIdAndZPointIdFromPointId(ptId, surfPointId, zPointId);

  vtkNew<vtkIdList> surfCellIds;
  this->SurfaceDelaunay2D->GetOutput()->GetPointCells(surfPointId, surfCellIds);

  if (VerticalEnumerationOrder){
    if (zPointId == 0){
      cellIds->SetNumberOfIds(surfCellIds->GetNumberOfIds());
      for (int i = 0; i < surfCellIds->GetNumberOfIds(); i++){
        cellIds->SetId(i, surfCellIds->GetId(i)*nZCells);
      }
    } else if (zPointId > 0 && zPointId < nZCells){
      cellIds->SetNumberOfIds(surfCellIds->GetNumberOfIds() * 2);
      for (int i = 0; i < surfCellIds->GetNumberOfIds(); i++){
        cellIds->SetId(i, surfCellIds->GetId(i)*nZCells + zPointId - 1);
        cellIds->SetId(surfCellIds->GetNumberOfIds() + i, surfCellIds->GetId(i)*nZCells + zPointId);
      }
    } else if (zPointId == nZCells){
      cellIds->SetNumberOfIds(surfCellIds->GetNumberOfIds());
      for (int i = 0; i < surfCellIds->GetNumberOfIds(); i++){
        cellIds->SetId(i, surfCellIds->GetId(i)*nZCells + zPointId - 1);
      }
    }
  } else {
    if (zPointId == 0){
      cellIds->SetNumberOfIds(surfCellIds->GetNumberOfIds());
      for (int i = 0; i < surfCellIds->GetNumberOfIds(); i++){
        cellIds->SetId(i, surfCellIds->GetId(i));
      }
    } else if (zPointId > 0 && zPointId < nZCells){
      cellIds->SetNumberOfIds(surfCellIds->GetNumberOfIds() * 2);
      for (int i = 0; i < surfCellIds->GetNumberOfIds(); i++){
        cellIds->SetId(i, surfCellIds->GetId(i) + (zPointId-1)*nSurfCells);
        cellIds->SetId(surfCellIds->GetNumberOfIds() + i, surfCellIds->GetId(i) + zPointId*nSurfCells);
      }
    } else if (zPointId == nZCells){
      cellIds->SetNumberOfIds(surfCellIds->GetNumberOfIds());
      for (int i = 0; i < surfCellIds->GetNumberOfIds(); i++){
        cellIds->SetId(i, surfCellIds->GetId(i) + (zPointId-1)*nSurfCells);
      }
    }
  }
}

//------------------------------------------------------------------------------
vtkIdType vtkRaisedGrid::FindPoint(double x[3])
{
  // find nearest point in XY plane and then look only for nearest Z point
  vtkIdType surfPointId = this->SurfaceDelaunay2D->GetOutput()->FindPoint(x);
  vtkIdType zPointId = 0;
  vtkIdType pointId = this->GetPointIdFromSurfPointIdAndZPointId(surfPointId, zPointId);
  double p[3];
  this->GetPoint(pointId, p);
  double z = p[2];
  double dist = std::abs(x[2]-z);
  double minDist = dist;

  for (vtkIdType i = 0; i < this->ZSpacings.size(); i++){
    z += this->ZSpacings[i];
    if (std::abs(x[2]-z) > dist)
      break;

    dist = std::abs(x[2]-z);
    if (dist < minDist){
      minDist = dist;
      zPointId = i+1;
    }
  }

  return this->GetPointIdFromSurfPointIdAndZPointId(surfPointId, zPointId);
}

//------------------------------------------------------------------------------
vtkIdType vtkRaisedGrid::FindCell(double x[3], vtkCell* vtkNotUsed(cell),
vtkGenericCell* vtkNotUsed(gencell), vtkIdType vtkNotUsed(cellId), double tol2, int& subId,
double pcoords[3], double* weights)
{
  return this->FindCell(x, nullptr, 0, tol2, subId, pcoords, weights);
}

//------------------------------------------------------------------------------
vtkIdType vtkRaisedGrid::FindCell(double x[3], vtkCell* vtkNotUsed(cell),
vtkIdType vtkNotUsed(cellId), double tol2, int& subId, double pcoords[3], double* weights)
{
  // find nearest cell in XY plane and then look only for nearest Z point
  vtkIdType surfCellId = this->SurfaceDelaunay2D->GetOutput()->FindCell(
        x, nullptr, 0, std::numeric_limits<double>::max(), subId, pcoords, weights);
  vtkIdType zCellId = 0;
  vtkIdType cellId = this->GetCellIdFromSurfCellIdAndZCellId(surfCellId, zCellId);
  double c[3];
  vtkCell* cell = this->GetCell(cellId);
  cell->GetParametricCenter(c);
  double z = c[2];
  // `dist` is squared as `tol2` is squared
  // suffix `2` means `squared`
  double dist2 = (x[0]-c[0])*(x[0]-c[0]) + (x[1]-c[1])*(x[1]-c[1]) + (x[2]-z)*(x[2]-z);
  double minDist2 = dist2;
  double distNew2 = dist2;

  for (vtkIdType i = 0; i < this->ZSpacings.size(); i++){
    z += this->ZSpacings[i];
    distNew2 = (x[0]-c[0])*(x[0]-c[0]) + (x[1]-c[1])*(x[1]-c[1]) + (x[2]-z)*(x[2]-z);
    if (distNew2 > dist2)
      break;

    dist2 = distNew2;
    if (dist2 < minDist2){
      minDist2 = dist2;
      zCellId = i+1;
    }
  }

  if (minDist2 > tol2)
    return -1;

  //---------DONT KNOW HOW WHAT IS `pcoords` (parametric coords) AND HOW TO CALCULATE `weights`-------
  if (weights)
  {
    // Shift parametric coordinates for XZ/YZ planes
    if (this->DataDescription == VTK_XZ_PLANE)
    {
      pcoords[1] = pcoords[2];
      pcoords[2] = 0.0;
    }
    else if (this->DataDescription == VTK_YZ_PLANE)
    {
      pcoords[0] = pcoords[1];
      pcoords[1] = pcoords[2];
      pcoords[2] = 0.0;
    }
    else if (this->DataDescription == VTK_XY_PLANE)
    {
      pcoords[2] = 0.0;
    }
    vtkWedge::InterpolationFunctions(pcoords, weights);
  }
  //-----------END OF UNCERTAIN BLOCK-----------

  //
  //  From this location get the cell id
  //
  subId = 0;
  return this->GetCellIdFromSurfCellIdAndZCellId(surfCellId, cellId);
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::ComputeBounds()
{
  if (this->GetMTime() <= this->ComputeTime)
  {
    return;
  }

  double z = 0;
  for (int i = 0; i < this->ZSpacings.size(); i++)
    z += this->ZSpacings[i];
  double* bounds = this->SurfaceDelaunay2D->GetOutput()->GetBounds();
  if (z > 0){
    bounds[5] += z;
  } else {
    bounds[2] += z;
  }
  memcpy(this->Bounds, bounds, sizeof(this->Bounds));
  this->ComputeTime.Modified();
}

//------------------------------------------------------------------------------
vtkRaisedGrid* vtkRaisedGrid::GetData(vtkInformation* info)
{
  return info ? vtkRaisedGrid::SafeDownCast(info->Get(DATA_OBJECT())) : nullptr;
}

//------------------------------------------------------------------------------
vtkRaisedGrid* vtkRaisedGrid::GetData(vtkInformationVector* v, int i)
{
  return vtkRaisedGrid::GetData(v->GetInformationObject(i));
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::GetPointsNearVerticalSlice(
    double xmin, double xmax,
    double ymin, double ymax,
    vtkIdList* ptIds)
{
  ptIds->Reset();
  vtkNew<vtkIdList> surfIdFound;
  double p[3];
  size_t nZId = this->ZSpacings.size()+1;
  vtkIdType nSurfId = this->SurfaceDelaunay2D->GetOutput()->GetNumberOfPoints();
  for (vtkIdType i = 0; i < nSurfId; i++){
    this->SurfaceDelaunay2D->GetOutput()->GetPoint(i, p);
    if (p[0] >= xmin &&
        p[0] <= xmax &&
        p[1] >= ymin &&
        p[1] <= ymax){
      surfIdFound->InsertNextId(i);
    }
  }

  vtkIdType nSurfIdFound = surfIdFound->GetNumberOfIds();
  ptIds->SetNumberOfIds(nSurfIdFound * nZId);
  for (vtkIdType j = 0; j < nZId; j++){
    for (vtkIdType i = 0; i < nSurfIdFound; i++){
      ptIds->SetId(nSurfIdFound*j + i, nSurfId*j + surfIdFound->GetId(i));
    }
  }
}

//------------------------------------------------------------------------------
void InterpolateScalarsOnImageData(
    double origin[3],
    double spacings[3],
    vtkDataArray* dataArray)
{

}

//------------------------------------------------------------------------------
void vtkRaisedGrid::SetSurfacePoints(vtkPoints* points){
  vtkNew<vtkPolyData> polyData;
  polyData->SetPoints(points);

  this->SetSurfacePolydata(polyData);
}

//------------------------------------------------------------------------------
vtkPoints* vtkRaisedGrid::GetSurfacePoints(){
  return this->SurfaceDelaunay2D->GetOutput()->GetPoints();
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::SetSurfacePolydata(vtkPolyData* polyData){
  // Triangulate the grid points
  this->SurfaceDelaunay2D->SetInputData(polyData);
  this->SurfaceDelaunay2D->Update();
}

//------------------------------------------------------------------------------
vtkPolyData* vtkRaisedGrid::GetSurfacePolydata(){
  return this->SurfaceDelaunay2D->GetOutput();
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::SetZSpacings(double* spacings, vtkIdType spacingsSize){
  this->ZSpacings.insert(this->ZSpacings.end(), &spacings[0], &spacings[spacingsSize]);
}

//------------------------------------------------------------------------------
vtkIdType vtkRaisedGrid::GetZSpacingsSize(){
  return this->ZSpacings.size();
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::GetZSpacings(double* spacings){
  std::copy(this->ZSpacings.begin(), this->ZSpacings.end(), spacings);
}

//------------------------------------------------------------------------------
vtkCell* vtkRaisedGrid::GetCellTemplateForDataDescription()
{
  vtkCell* cell = nullptr;
  switch (this->DataDescription)
  {
  case VTK_XYZ_GRID:
    cell = this->Wedge;
    break;

  default:
    vtkErrorMacro("Invalid DataDescription.");
    break;
  }
  return cell;
}

//------------------------------------------------------------------------------
bool vtkRaisedGrid::GetCellTemplateForDataDescription(vtkGenericCell* cell)
{
  switch (this->DataDescription)
  {
  case VTK_XYZ_GRID:
    cell->SetCellTypeToWedge();
    break;

  default:
    vtkErrorMacro("Invalid DataDescription.");
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::AddPointsToCellTemplate(vtkCell* cell, vtkIdType cellId)
{
  vtkIdType nSurfPoints = this->SurfaceDelaunay2D->GetOutput()->GetNumberOfPoints();
  vtkIdType nZPoints = ZSpacings.size()+1;

  vtkIdType surfCellId, zCellId;
  this->GetSurfCellIdAndZCellIdFromCellId(cellId, surfCellId, zCellId);

  vtkCell* surfCell =
      this->SurfaceDelaunay2D->GetOutput()->GetCell(surfCellId);
  vtkPoints* surfCellPoints = surfCell->GetPoints();

  double z = 0;
  for (int i = 0; i < zCellId; i++)
    z += this->ZSpacings[i];

  vtkIdType I = surfCellPoints->GetNumberOfPoints();
  vtkIdType surfPointId, pid0, pid1, ploc0, ploc1;
  ploc0 = 0;
  ploc1 = I;

  cell->GetPointIds()->SetNumberOfIds(I);
  if (VerticalEnumerationOrder){
    for (vtkIdType i = 0; i < I; i++){
      surfPointId = surfCell->GetPointIds()->GetId(i);
      double p[3];
      surfCellPoints->GetPoint(surfPointId, p);
      double z0 = z+p[2];
      double z1 = z0+this->ZSpacings[zCellId];

      pid0 = surfPointId * nZPoints + zCellId;
      pid1 = pid0 + 1;

      cell->GetPointIds()->SetId(ploc0, pid0);
      cell->GetPoints()->SetPoint(
            ploc0,
            p[0], p[1], z0);

      cell->GetPointIds()->SetId(ploc1, pid1);
      cell->GetPoints()->SetPoint(
            ploc1,
            p[0], p[1], z1);

      ploc0++;
      ploc1++;
    }
  } else {
    for (vtkIdType i = 0; i < I; i++){
      surfPointId = surfCell->GetPointIds()->GetId(i);
      double p[3];
      surfCellPoints->GetPoint(surfPointId, p);
      double z0 = z+p[2];
      double z1 = z0+this->ZSpacings[zCellId];

      pid0 = surfPointId + nSurfPoints * zCellId;
      pid1 = pid0 + nSurfPoints;

      cell->GetPointIds()->SetId(ploc0, pid0);
      cell->GetPoints()->SetPoint(
            ploc0,
            p[0], p[1], z0);

      cell->GetPointIds()->SetId(ploc1, pid1);
      cell->GetPoints()->SetPoint(
            ploc1,
            p[0], p[1], z1);

      ploc0++;
      ploc1++;
    }
  }
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::SetDataDescription(int desc)
{
  if (desc == this->DataDescription)
  {
    return;
  }

  this->DataDescription = desc;

  if (this->Wedge)
  {
    this->Wedge->Delete();
    this->Wedge = nullptr;
  }
  switch (this->DataDescription)
  {
  case VTK_XYZ_GRID:
    this->Wedge = vtkWedge::New();
    break;

  default:
    vtkErrorMacro("Invalid DataDescription.");
    break;
  }
}

//------------------------------------------------------------------------------
vtkIdType vtkRaisedGrid::GetPointIdFromSurfPointIdAndZPointId(
    vtkIdType surfPointId,
    vtkIdType zPointId)
{
  if (VerticalEnumerationOrder){
    return surfPointId * (ZSpacings.size()+1) + zPointId;
  } else {
    return zPointId * this->SurfaceDelaunay2D->GetOutput()->GetNumberOfPoints() + surfPointId;
  }
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::GetSurfPointIdAndZPointIdFromPointId(
    vtkIdType pointId,
    vtkIdType &surfPointId,
    vtkIdType &zPointId)
{
  if (VerticalEnumerationOrder){
    surfPointId = pointId / (ZSpacings.size()+1);
    zPointId = pointId % (ZSpacings.size()+1);
  } else {
    surfPointId = pointId % this->SurfaceDelaunay2D->GetOutput()->GetNumberOfPoints();
    zPointId = pointId / this->SurfaceDelaunay2D->GetOutput()->GetNumberOfPoints();
  }
}

//------------------------------------------------------------------------------
vtkIdType vtkRaisedGrid::GetCellIdFromSurfCellIdAndZCellId(
    vtkIdType surfCellId,
    vtkIdType zCellId)
{
  if (VerticalEnumerationOrder){
    return surfCellId * ZSpacings.size() + zCellId;
  } else {
    return zCellId * this->SurfaceDelaunay2D->GetOutput()->GetNumberOfCells() + surfCellId;
  }
}

//------------------------------------------------------------------------------
void vtkRaisedGrid::GetSurfCellIdAndZCellIdFromCellId(
    vtkIdType cellId,
    vtkIdType &surfCellId,
    vtkIdType &zCellId)
{
  if (VerticalEnumerationOrder){
    surfCellId = cellId / ZSpacings.size();
    zCellId = cellId % ZSpacings.size();
  } else {
    surfCellId = cellId % this->SurfaceDelaunay2D->GetOutput()->GetNumberOfCells();
    zCellId = cellId / this->SurfaceDelaunay2D->GetOutput()->GetNumberOfCells();
  }
}
