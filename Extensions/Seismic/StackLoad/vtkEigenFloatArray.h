#ifndef __vtkEigenFloatArray_h
#define __vtkEigenFloatArray_h

#include <vtkFloatArray.h>
#include <Eigen/Dense>

#include "qSlicerStackLoadModuleExport.h"

class Q_SLICER_QTMODULES_STACKLOAD_EXPORT vtkEigenFloatArray : public vtkFloatArray
{
public:
  static vtkEigenFloatArray* New();
  vtkTypeMacro(vtkEigenFloatArray, vtkFloatArray);

  void PrintSelf(ostream& os, vtkIndent indent) override;
  // This macro expands to the set of method declarations that
  // make up the interface of vtkAOSDataArrayTemplate, which is ignored
  // by the wrappers.
#if defined(__VTK_WRAP__) || defined(__WRAP_GCCXML__)
  vtkCreateWrappedArrayInterface(float);
#endif

  /**
   * A faster alternative to SafeDownCast for downcasting vtkAbstractArrays.
   */
  static vtkEigenFloatArray* FastDownCast(vtkAbstractArray* source)
  {
    return static_cast<vtkEigenFloatArray*>(Superclass::FastDownCast(source));
  }

  /**
   * Get the minimum data value in its native type.
   */
  static float GetDataTypeValueMin() { return VTK_FLOAT_MIN; }

  /**
   * Get the maximum data value in its native type.
   */
  static float GetDataTypeValueMax() { return VTK_FLOAT_MAX; }

  void SetEigenArray(Eigen::MatrixXf* arr);

  /**
   * Don't delete Eigen::Matrix from outside!
   */
  Eigen::MatrixXf* GetEigenArray();

  /**
   * Must be manually called each time inner Eigen::Matrix resized
   */
  void UpdateEigenBuffer();

protected:
  vtkEigenFloatArray();

  ~vtkEigenFloatArray() override;

private:
  typedef vtkAOSDataArrayTemplate<float> RealSuperclass;

  vtkEigenFloatArray(const vtkEigenFloatArray&) = delete;
  void operator=(const vtkEigenFloatArray&) = delete;

  friend class vtkNew<vtkEigenFloatArray>;

  Eigen::MatrixXf* eigArr;
};

#endif
