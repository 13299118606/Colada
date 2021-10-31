#ifndef vtkRaisedGrid_h
#define vtkRaisedGrid_h

#include "vtkEigen_export.h"
#include "vtkDataSet.h"

#include "vtkStructuredGrid.h"

class vtkDataArray;
class vtkLine;
class vtkMatrix3x3;
class vtkMatrix4x4;
class vtkPixel;
class vtkVertex;
class vtkVoxel;
class vtkPoints;
class vtkDoubleArray;
class vtkCellArray;
class vtkDelaunay2D;
class vtkWedge;
class vtkPolyData;

class VTKEIGEN_EXPORT vtkRaisedGrid : public vtkDataSet
{
public:
  static vtkRaisedGrid* New();
  static vtkRaisedGrid* ExtendedNew();

  vtkTypeMacro(vtkRaisedGrid, vtkDataSet);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Copy the geometric and topological structure of an input image data
   * object.
   */
  void CopyStructure(vtkDataSet* ds) override;
  void Initialize() override;

  /**
   * Return `VTK_DATA_SET` instead of `VTK_IMAGE_DATA`
   */
  int GetDataObjectType() override { return VTK_DATA_SET; }

  vtkIdType GetNumberOfPoints() override;
  vtkIdType GetNumberOfCells() override;
  double* GetPoint(vtkIdType ptId) VTK_SIZEHINT(3) override;
  void GetPoint(vtkIdType ptId, double x[3]) override;
  vtkCell* GetCell(vtkIdType cellId) override;
  void GetCell(vtkIdType cellId, vtkGenericCell* cell) override;
  vtkCell* GetCell(int i, int j, int k) override  {
    vtkErrorMacro("ijk indices are only valid with structured data!");
    return nullptr;
  }
  int GetCellType(vtkIdType cellId) override;
  void GetCellPoints(vtkIdType cellId, vtkIdList* ptIds) override;
  void GetPointCells(vtkIdType ptId, vtkIdList* cellIds) override;
  vtkIdType FindPoint(double x[3]) override;
  vtkIdType FindCell(double x[3], vtkCell* cell, vtkIdType cellId, double tol2, int& subId,
    double pcoords[3], double* weights) override;
  vtkIdType FindCell(double x[3], vtkCell* cell, vtkGenericCell* gencell, vtkIdType cellId,
    double tol2, int& subId, double pcoords[3], double* weights) override;
  void ComputeBounds() override;
  int GetMaxCellSize() override { return 6; } // wedge is the largest

  //@{
  /**
   * Retrieve an instance of this class from an information object.
   */
  static vtkRaisedGrid* GetData(vtkInformation* info);
  static vtkRaisedGrid* GetData(vtkInformationVector* v, int i = 0);
  //@}

  void GetPointsNearVerticalSlice(
      double xmin, double xmax,
      double ymin, double ymax,
      vtkIdList* ptIds);

  /**
   * Not implemented yet!
   */
//  void InterpolateScalarsOnImageData(
//      double origin[3],
//      double spacings[3],
//      vtkDataArray* dataArray);

  void SetSurfacePoints(vtkPoints* points);
  vtkPoints* GetSurfacePoints();

  void SetSurfacePolydata(vtkPolyData* polyData);
  vtkPolyData* GetSurfacePolydata();

  void SetZSpacings(double* spacings, vtkIdType spacingsSize);
  vtkIdType GetZSpacingsSize();
  /**
   * Use in pair with `GetZSpacingsSize()` to allocate array
   */
  void GetZSpacings(double* spacings);

  vtkSetMacro(VerticalEnumerationOrder, vtkTypeBool);
  vtkGetMacro(VerticalEnumerationOrder, vtkTypeBool);

protected:
  vtkRaisedGrid();
  ~vtkRaisedGrid() override;

  // Cell utilities
  vtkCell* GetCellTemplateForDataDescription();
  bool GetCellTemplateForDataDescription(vtkGenericCell* cell);
  void AddPointsToCellTemplate(vtkCell* cell, vtkIdType cellId);

  void SetDataDescription(int desc);
  int GetDataDescription() { return this->DataDescription; }

  vtkIdType GetPointIdFromSurfPointIdAndZPointId(
      vtkIdType surfPointId,
      vtkIdType zPointId);
  void GetSurfPointIdAndZPointIdFromPointId(
      vtkIdType pointId,
      vtkIdType &surfPointId,
      vtkIdType &zPointId);

  vtkIdType GetCellIdFromSurfCellIdAndZCellId(
      vtkIdType surfCellId,
      vtkIdType zCellId);
  void GetSurfCellIdAndZCellIdFromCellId(
      vtkIdType cellId,
      vtkIdType &surfCellId,
      vtkIdType &zCellId);


public:
  vtkDelaunay2D* SurfaceDelaunay2D;
  std::vector<double> ZSpacings;
  bool VerticalEnumerationOrder;

private:
  // for the GetCell method
  vtkWedge* Wedge;

  // for the GetPoint method
  double Point[3];

  int DataDescription;

  vtkRaisedGrid(const vtkRaisedGrid&) = delete;
  void operator=(const vtkRaisedGrid&) = delete;
};



inline void vtkRaisedGrid::GetPoint(vtkIdType id, double x[3])
{
  double* pt = this->GetPoint(id);
  x[0] = pt[0];
  x[1] = pt[1];
  x[2] = pt[2];
}

#endif
