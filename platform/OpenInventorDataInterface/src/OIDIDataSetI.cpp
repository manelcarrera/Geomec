#include "OIDIDataSetI.h"

template <typename T> int sgn(T val)
{
  return (T(0) < val) - (val < T(0));
}

OIDITensorVectorSetI::OIDITensorVectorSetI(size_t dataSize, MiDataSet::DataBinding binding, std::shared_ptr<OIDITensorSetI> tensors)
  : OIDIVectorSetI (dataSize, binding)
  , m_tensors(tensors)
{
  SbVec3f normal(1, 0, 0);
  setNormal(normal, TensorVectorFull);
}

void OIDITensorVectorSetI::setNormal (const SbVec3f & normal, TensorVectorMode tensorVectorMode)
{
  MbVec3d zeroVector (0.0, 0.0, 0.0);

  clear();
  size_t beginIndex = m_tensors->beginIndex();
  setBeginIndex(beginIndex);

  MbVec3d mNormal (normal[0], normal[1], normal[2]);

  switch (tensorVectorMode)
  {
  case TensorVectorFull:
    for(size_t i=0; i < m_tensors->dataSize(); ++i)
    {
      MbVec3d vector = product((*m_tensors)[beginIndex + i], normal);
      add(vector);
    }
    break;

  case TensorVectorNormal:
    for(size_t i=0; i < m_tensors->dataSize(); ++i)
    {
      MbVec3d vector = product ((*m_tensors)[beginIndex + i], normal);
      double dot = vector.dot (mNormal);
      MbVec3d resNormal = mNormal * dot;
      add (resNormal);
    }
    break;

  case TensorVectorShear:
    for(size_t i=0; i < m_tensors->dataSize(); ++i)
    {
      MbVec3d vector = product((*m_tensors)[beginIndex + i], normal);
      double dot = vector.dot (mNormal);
      MbVec3d resNormal = mNormal * dot;
      MbVec3d resShear = vector - resNormal;
      add (resShear);
    }
    break;
  }
  updateTimeStamp();
}

OIDITensorVectorSignSetI::OIDITensorVectorSignSetI(size_t dataSize, MiDataSet::DataBinding binding, std::shared_ptr<OIDITensorSetI> tensors)
  : OIDIScalarSetI (dataSize, binding)
  , m_tensors(tensors)
{
  SbVec3f normal(1, 0, 0);
  setNormal(normal, TensorVectorFull);
}

void OIDITensorVectorSignSetI::setNormal (const SbVec3f & normal, TensorVectorMode tensorVectorMode)
{
  clear();
  size_t beginIndex = m_tensors->beginIndex();
  setBeginIndex(beginIndex);

  MbVec3d mNormal (normal[0], normal[1], normal[2]);

  for(size_t i=0; i < m_tensors->dataSize(); ++i)
  { 
    switch(tensorVectorMode)
    {
    case TensorVectorFull:
    case TensorVectorNormal:
    case TensorVectorShear:
      {
        MbVec3d vector = product ((*m_tensors)[beginIndex + i], normal );
        double dot = vector.dot (mNormal);
        add (sgn(dot));
      }
      break;
      /*
    case TensorVectorShear:
      add(0);
      break;
      */

    default:
      assert (false);
    }
  }
  updateTimeStamp();
}

OIDIScalarSetI* GenerateScalarsFromScalars(const OIDIScalarSetI& scalars)
{
  OIDIScalarSetI* values = new OIDIScalarSetI(scalars.dataSize(), scalars.getBinding());

  for (std::size_t i = scalars.beginIndex(); i < scalars.endIndex(); ++i)
    values->set(i, scalars.get(i));

  return values;
}

OIDIScalarSetI* GenerateScalarsFromVectors(const OIDIVectorSetI& vectors, unsigned short component)
{
  OIDIScalarSetI* values = new OIDIScalarSetI(vectors.dataSize(), vectors.getBinding());

  if (component < 3)
  {   
    for (std::size_t i = vectors.beginIndex(); i < vectors.endIndex(); ++i)
      values->set(i, vectors.get(i)[component]);
  }
  else
  {
    for (std::size_t i = vectors.beginIndex(); i < vectors.endIndex(); ++i)
      if (vectors.get(i)[0] == DBL_UNDEFINED)
        values->set(i, DBL_UNDEFINED);
      else
        values->set(i, vectors.get(i).length());
  }

  return values;
}


OIDIScalarSetI*  GenerateScalarsFromTensors(const OIDITensorSetI& tensors, unsigned short component)
{
  enum TENSOR_COMPONENT { TC_XX = 0, TC_YY, TC_ZZ, TC_XY, TC_YZ, TC_ZX, TC_NORM };

  OIDIScalarSetI* values = new OIDIScalarSetI(tensors.dataSize(), tensors.getBinding());

  for (std::size_t i = tensors.beginIndex(); i < tensors.endIndex(); ++i)
  {
    double value = DBL_UNDEFINED;

    if (!IsUndefined(tensors[i]))
    {
      switch (TENSOR_COMPONENT (component))
      {
      case TC_XX:
        value = tensors[i].XX();
        break;
      case TC_YY:
        value = tensors[i].YY();
        break;
      case TC_ZZ:
        value = tensors[i].ZZ();
        break;
      case TC_XY:
        value = tensors[i].XY();
        break;
      case TC_YZ:
        value = tensors[i].YZ();
        break;
      case TC_ZX:
        value = tensors[i].XZ();
        break;
      case TC_NORM:
        value = tensors[i].Norm();
        break;
      default:
        assert(false);
      }
    }
    values->set(i, value);
  }
  return values;
}

OIDIScalarSetI*  GenerateEigenValuesFromTensors(const OIDITensorSetI& tensors, unsigned short component)
{
  assert(component <= 2);
  component = 2 - component; // reverse as smallest = first

  OIDIScalarSetI* eigenValues = new OIDIScalarSetI(tensors.dataSize(), tensors.getBinding());


  for (std::size_t i = tensors.beginIndex(); i < tensors.endIndex(); ++i)
  {
    CTensor tensor = tensors.get(i);
    if (IsUndefined(tensor))
      eigenValues->set(i,DBL_UNDEFINED);
    else
    {
      geo::CMatrix eigenValueMatrix = tensor.GetPrincipalStr();
      double eigenValue = eigenValueMatrix.Value(component, component);
      eigenValues->set(i,eigenValue);
    }
  }
  return eigenValues;
}

OIDIVectorSetI* GenerateEigenVectorsFromTensors(const OIDITensorSetI& tensors, unsigned short component)
{
  assert(component <= 2);
  component = 2 - component; // reverse as smallest = first
  OIDIVectorSetI* eigenVectors = new OIDIVectorSetI(tensors.dataSize(), tensors.getBinding());

  static MbVec3d undefined(DBL_UNDEFINED, DBL_UNDEFINED, DBL_UNDEFINED);
  for (std::size_t i = tensors.beginIndex(); i < tensors.endIndex(); ++i)
  {
    CTensor tensor = tensors.get(i);
    if (IsUndefined(tensor))
  {
      eigenVectors->set(i,undefined);
  }
    else
    {
      geo::CMatrixPair eigenPair = tensor.GetEigenPairs();
      geo::CMatrix eigenVectorMatrix = eigenPair.second; // contains unit vectors
      geo::CMatrix eigenValueMatrix = eigenPair.first;

#ifndef NDEBUG
      MbVec3d eigenValueVector(eigenValueMatrix.Value(0, 0), eigenValueMatrix.Value(1, 1), eigenValueMatrix.Value(2, 2));
#endif
      double eigenValue = eigenValueMatrix.Value(component, component);

      MbVec3d eigenVector(eigenValue * eigenVectorMatrix.Value(0, component), eigenValue * eigenVectorMatrix.Value(1, component), eigenValue * eigenVectorMatrix.Value(2, component));
#ifndef NDEBUG
      double length = eigenVector.length();
#endif

      eigenVectors->set(i,eigenVector);
    }
  }

  return eigenVectors;
}

void GenerateEigenVectorsAndValuesFromTensors(const OIDITensorSetI& tensors, unsigned short component, OIDIVectorSetI *& eigenVectors, OIDIScalarSetI * & eigenValues)
{
  assert(component <= 2);
  component = 2 - component; // reverse as smallest = first

  eigenVectors = new OIDIVectorSetI(tensors.dataSize(), tensors.getBinding());
  eigenValues = new OIDIScalarSetI(tensors.dataSize(), tensors.getBinding());

  MbVec3d undefined(DBL_UNDEFINED, DBL_UNDEFINED, DBL_UNDEFINED);
  for (std::size_t i = tensors.beginIndex(); i < tensors.endIndex(); ++i)
  {
    CTensor tensor = tensors.get(i);
    if (IsUndefined(tensor))
    {
      eigenVectors->set(i,undefined);
      eigenValues->set(i,DBL_UNDEFINED);
    }
    else
    {
      geo::CMatrixPair eigenPair = tensor.GetEigenPairs();
      geo::CMatrix eigenVectorMatrix = eigenPair.second; // contains unit vectors
      geo::CMatrix eigenValueMatrix = eigenPair.first;

#ifndef NDEBUG
      MbVec3d eigenValueVector(eigenValueMatrix.Value(0, 0), eigenValueMatrix.Value(1, 1), eigenValueMatrix.Value(2, 2));
#endif
      double eigenValue = eigenValueMatrix.Value(component, component);

      MbVec3d eigenVector(
        eigenValue * eigenVectorMatrix.Value(0, component), 
        eigenValue * eigenVectorMatrix.Value(1, component), 
        eigenValue * eigenVectorMatrix.Value(2, component));
#ifndef NDEBUG
      double length = eigenVector.length();
#endif

      eigenVectors->set(i,eigenVector);
      eigenValues->set(i,eigenValue);
    }
  }
}

/**
 * Creates the 2 defaults scalar sets for coloring beachballs: the first dataset contains the 
 * values for the minimum principal direction: 0.0 for tensile (eigenvalue < 0) stress, and 1.0 
 * for compressive (eigenvalue > 0) stress . The second dataset is used for coloring the part
 * corresponding to the medium principal direction, and has values 2.0 (tensile) or 3.0 (compressive).
 * These values are chosen so the beachballs can be colored with a single colormap, in most cases
 * a MoLevelColorMapping that specifies 4 colors / intervals.
 */
std::array<OIDIScalarSetI*, 2> generateDefaultBeachballScalarSets(const OIDITensorSetI& tensors)
{
  OIDIScalarSetI* scalars1 = new OIDIScalarSetI(tensors.dataSize(), tensors.getBinding());
  OIDIScalarSetI* scalars2 = new OIDIScalarSetI(tensors.dataSize(), tensors.getBinding());

  for(size_t i=tensors.beginIndex(); i < tensors.endIndex(); ++i)
  {
    CTensor tensor = tensors.get(i);
    geo::CMatrix eigenValues = tensor.GetPrincipalStr();

    // eigenValues are on the diagonal, sorted from smallest to largets, so
    // scalars1 has the minimum eigenvalue, and scalars2 has the medium eigenvalue
    scalars1->set(i, eigenValues.Value(0, 0) < 0.0 ? 0.0 : 1.0);
    scalars2->set(i, eigenValues.Value(1, 1) < 0.0 ? 2.0 : 3.0);
  }

  std::array<OIDIScalarSetI*, 2> result;
  result[0] = scalars1;
  result[1] = scalars2;

  return result;
}

MbVec3d product (const ITensor & tensor, const SbVec3f & vector)
{
  if (tensor.XX() == DBL_UNDEFINED)
    return MbVec3d (DBL_UNDEFINED);
  else
    return MbVec3d (
      tensor.XX() * vector[0] + tensor.XY() * vector[1] + tensor.XZ() * vector[2],
      tensor.XY() * vector[0] + tensor.YY() * vector[1] + tensor.YZ() * vector[2],
      tensor.XZ() * vector[0] + tensor.YZ() * vector[1] + tensor.ZZ() * vector[2]
    );
}

std::string GetUniqueName( const std::string & prepend )
{
  static int sequenceNumber = -1;
  char sequenceString[9];
  std::string result = prepend + itoa(++sequenceNumber, sequenceString, 10);
  return result;
}

bool IsUndefined( const double & val )
{
  return val == DBL_UNDEFINED;
}

bool IsUndefined( const MbVec3d& val )
{
  return val[0] == DBL_UNDEFINED;
}

bool IsUndefined( const CTensor& val )
{
  return val.XX() == DBL_UNDEFINED;
}

bool operator< (const CTensor &v1, const CTensor &v2)
{
  return v1.Norm() < v2.Norm();
}

bool operator> (const CTensor &v1, const CTensor &v2)
{
  return v1.Norm() > v2.Norm();  // cannot really compare them
}
