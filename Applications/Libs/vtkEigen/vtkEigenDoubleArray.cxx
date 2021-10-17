// Instantiate superclass first to give the template a DLL interface.
#define VTK_AOS_DATA_ARRAY_TEMPLATE_INSTANTIATING
#include "vtkAOSDataArrayTemplate.txx"
VTK_AOS_DATA_ARRAY_TEMPLATE_INSTANTIATE(double);

#include "vtkEigenDoubleArray.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkEigenDoubleArray);

vtkEigenDoubleArray::vtkEigenDoubleArray()
{
  eigArr = new Eigen::MatrixXd();
}

vtkEigenDoubleArray::~vtkEigenDoubleArray()
{
  delete eigArr;
  eigArr = nullptr;
}

void vtkEigenDoubleArray::PrintSelf(ostream& os, vtkIndent indent)
{
  this->RealSuperclass::PrintSelf(os, indent);
}

void vtkEigenDoubleArray::SetEigenArray(Eigen::MatrixXd* arr)
{
  delete eigArr;
  eigArr = arr;
  UpdateEigenBuffer();
}

Eigen::MatrixXd* vtkEigenDoubleArray::GetEigenArray()
{
  return eigArr;
}

void vtkEigenDoubleArray::UpdateEigenBuffer()
{
  // `save` mode must be set to 1 or in destructor runtime error
  this->SetArray(eigArr->data(), eigArr->size(), 1);
}
