#ifndef __vtkEigenDoubleArray_h
#define __vtkEigenDoubleArray_h

#include <vtkDoubleArray.h>
#include <Eigen/Dense>

#include "vtkEigen_export.h"

class VTKEIGEN_EXPORT vtkEigenDoubleArray : public vtkDoubleArray
{
public:
  static vtkEigenDoubleArray* New();
  vtkTypeMacro(vtkEigenDoubleArray, vtkDoubleArray);

  void PrintSelf(ostream& os, vtkIndent indent) override;
  // This macro expands to the set of method declarations that
  // make up the interface of vtkAOSDataArrayTemplate, which is ignored
  // by the wrappers.
#if defined(__VTK_WRAP__) || defined(__WRAP_GCCXML__)
  vtkCreateWrappedArrayInterface(double);
#endif

  /**
   * A faster alternative to SafeDownCast for downcasting vtkAbstractArrays.
   */
  static vtkEigenDoubleArray* FastDownCast(vtkAbstractArray* source)
  {
    return static_cast<vtkEigenDoubleArray*>(Superclass::FastDownCast(source));
  }

  /**
   * Get the minimum data value in its native type.
   */
  static double GetDataTypeValueMin() { return VTK_DOUBLE_MIN; }

  /**
   * Get the maximum data value in its native type.
   */
  static double GetDataTypeValueMax() { return VTK_DOUBLE_MAX; }

  void SetEigenArray(Eigen::MatrixXd* arr);

  /**
   * Don't delete Eigen::Matrix from outside!
   */
  Eigen::MatrixXd* GetEigenArray();

  /**
   * Must be manually called each time inner Eigen::Matrix resized
   */
  void UpdateEigenBuffer();

protected:
  vtkEigenDoubleArray();

  ~vtkEigenDoubleArray() override;

private:
  typedef vtkAOSDataArrayTemplate<double> RealSuperclass;

  vtkEigenDoubleArray(const vtkEigenDoubleArray&) = delete;
  void operator=(const vtkEigenDoubleArray&) = delete;

  friend class vtkNew<vtkEigenDoubleArray>;

  Eigen::MatrixXd* eigArr;
};

#endif
