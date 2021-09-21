// Instantiate superclass first to give the template a DLL interface.
#define VTK_AOS_DATA_ARRAY_TEMPLATE_INSTANTIATING
#include "vtkAOSDataArrayTemplate.txx"
VTK_AOS_DATA_ARRAY_TEMPLATE_INSTANTIATE(float);

#include "vtkEigenFloatArray.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkEigenFloatArray);

vtkEigenFloatArray::vtkEigenFloatArray()
{
  eigArr = new Eigen::MatrixXf();
}

vtkEigenFloatArray::~vtkEigenFloatArray()
{
  delete eigArr;
  eigArr = nullptr;
}

void vtkEigenFloatArray::PrintSelf(ostream& os, vtkIndent indent)
{
  this->RealSuperclass::PrintSelf(os, indent);
}

void vtkEigenFloatArray::SetEigenArray(Eigen::MatrixXf* arr)
{
  delete eigArr;
  eigArr = arr;
  UpdateEigenBuffer();
}

Eigen::MatrixXf* vtkEigenFloatArray::GetEigenArray()
{
  return eigArr;
}

void vtkEigenFloatArray::UpdateEigenBuffer()
{
  // `save` mode must be set to 1 or in destructor runtime error
  this->SetArray(eigArr->data(), eigArr->size(), 1);
}
