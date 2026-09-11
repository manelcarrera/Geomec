#pragma once

#include <vector>
#include <memory>
#include <array>
#include <assert.h>

#include <Inventor/SbVec.h>

#include <MeshVizXLM/mesh/data/MiDataSetI.h>

#include <MeshVizXLM/MxTimeStamp.h>

#include "IObject.h"
#include "ITensorGroup.h"
#include "MeshMode.h"
#include "OIDICell.h"
#include "IBox.h"
#include "OIDITopology.h"


std::string GetUniqueName (const std::string & prepend);

bool IsUndefined( const double & val );
bool IsUndefined( const MbVec3d& val );
bool IsUndefined( const CTensor& val );

bool operator< (const CTensor &v1, const CTensor &v2);
bool operator> (const CTensor &v1, const CTensor &v2);
  
/**
* Template Class defining a data set associated to the unstructured volume mesh
*/

template <typename T>
class OIDIDatasetI : public MiDataSetI<T> 
{
public:
  OIDIDatasetI(size_t dataSize, MiDataSet::DataBinding binding = PER_CELL)
    : m_binding(binding)
    , m_timestamp(MxTimeStamp::getTimeStamp())
    , m_begin(0)
    , m_min(-1)
    , m_max(-1)
  {
    m_name = GetUniqueName("OIDIDataSet: ");
    m_data.reserve(dataSize);
  }

  size_t dataSize() const
  {
    return m_data.size();
  }

  void add(const T& val)
  {
    size_t index = m_data.size();
    m_data.push_back(val);
    if (IsUndefined(val)) return;

    if (m_min < 0 || val < m_data[m_min])
      m_min = index;
    if (m_max < 0 || val > m_data[m_max])
      m_max = index;
  }

  void set(const std::size_t index, const T& val)
  {
    if (index < m_begin)
    {
      m_data.insert(m_data.begin(), m_begin - index, T(DBL_UNDEFINED));
      m_min += (m_begin - index);
      m_max += (m_begin - index);
      m_begin = index;
    }

    if (index > m_begin && m_data.size() == 0)
    {
      m_begin = index;
    }

    size_t vectorIndex = index - m_begin;
    if (vectorIndex >= m_data.size())
    {
      m_data.insert(m_data.end(), vectorIndex + 1 - m_data.size(), T(DBL_UNDEFINED));
    }

    assert(vectorIndex < m_data.size());
    
    m_data[vectorIndex] = val;
       
    if (IsUndefined(val)) return;

    if (m_min < 0 || val < m_data[m_min])
      m_min = vectorIndex;
    if (m_max < 0 || val > m_data[m_max])
      m_max = vectorIndex;
  }

  void adjustForInterfaceElements(const OIDISurfaceTopologyExplicitI& topology)
  {
      int min = INT_MAX;
      int max = INT_MIN;

      for (size_t i = 0; i < topology.getNumCells(); ++i)
      {
    const geo::IInterfaceElement *ifElt = topology.getCell(i)->getInterfaceElement();
    if (ifElt)
    {
          for (std::size_t j = 0; j < ifElt->NrOfNodes() / 2; ++j)
          {
      min = std::min(min, ifElt->PointIndex(j));
      max = std::max(max, ifElt->PointIndex(j));
          }
    }
      }

      if (min < INT_MAX)
    set(min, T(DBL_UNDEFINED));
      if (max > INT_MIN)
    set(max, T(DBL_UNDEFINED));
  }
  void clear()
  {
    m_data.clear();
    m_begin = 0;
    m_min = -1;
    m_max = -1;
  }

  /**
  * Returns the i-th vector of this set.
  */
  T get(size_t i) const
  {
    if (i < m_begin || i >= m_begin + m_data.size()) return T(DBL_UNDEFINED); // return some undefined value
    else return m_data[i - m_begin];
  }

  const T& operator[](size_t i) const
  {
    static const T undefined (DBL_UNDEFINED);
    if (i < m_begin || i >= m_begin + m_data.size()) return undefined; // return some undefined value
    else return m_data[i - m_begin];
  }

  T& operator[](size_t i)
  {
    static T undefined (DBL_UNDEFINED);
    if (i < m_begin || i >= m_begin + m_data.size()) return undefined; // return some undefined value
    else return m_data[i - m_begin];

  }

  /**
  * Returns the minimum vector of this set.
  */
  T getMin() const
  {
    T min = (m_min >= 0 && m_min < m_data.size()) ? m_data[m_min] : T(DBL_UNDEFINED);
    return min;
  }

  /**
  * Returns the maximum vector of this set.
  */
  T getMax() const
  {
    T max = (m_max >= 0 && m_max < m_data.size()) ? m_data[m_max] : T(DBL_UNDEFINED);
    return max;
  }

  /**
  * Returns a time stamp on this vector set.
  */
  size_t getTimeStamp() const
  {
    // @ADD YOUR CODE HERE (see MxTimeStamp).
    // When the content of the vector set changes, the time stamp must also change.
    // The time stamp allows MeshViz to be aware of any change to this vector set.
    return m_timestamp;
  }

  void updateTimeStamp()
  {
    m_timestamp = MxTimeStamp::getTimeStamp();
  }

  size_t beginIndex() const
  {
    return m_begin;
  }

  size_t endIndex() const
  {
    return m_begin + m_data.size();
  }

  void setBeginIndex(size_t begin)
  {
    assert (m_data.empty());
    m_begin = begin;
  }

  /**
  * Returns the name of this vector set.
  */
  std::string getName() const
  {
    return m_name;
  }

  /**
  * Returns the data binding of this vector set.
  */
  DataBinding  getBinding() const
  {
    return m_binding;
  }

  virtual void setNormal (const SbVec3f & normal, TensorVectorMode tensorVectorMode) {}

private:

  std::string m_name;
  MiDataSet::DataBinding m_binding;
  std::vector<T> m_data;
  size_t m_begin;
  size_t m_timestamp;
  long long m_min;
  long long m_max;
};

class OIDIScalarSetI : public OIDIDatasetI<double>
{
public:
  OIDIScalarSetI(std::size_t dataSize, MiDataSet::DataBinding binding = PER_CELL) : OIDIDatasetI(dataSize, binding)
  {
  }
  virtual bool isTensorVectorSignDataSet() { return false; }
};


/**
* Class defining a vector set associated to the unstructured volume mesh
*/
class OIDIVectorSetI : public OIDIDatasetI<MbVec3d>
{
public:
  OIDIVectorSetI(std::size_t dataSize, MiDataSet::DataBinding binding = PER_CELL) : OIDIDatasetI(dataSize, binding)
  {
  }
};

class OIDITensorSetI : public OIDIDatasetI<CTensor>
{
public:
  OIDITensorSetI(std::size_t dataSize, MiDataSet::DataBinding binding = PER_CELL) 
    : OIDIDatasetI(dataSize, binding)
  {
  }
};

class OIDITensorVectorSetI : public OIDIVectorSetI 
{
  std::shared_ptr<OIDITensorSetI> m_tensors;

public:

  OIDITensorVectorSetI(size_t dataSize, MiDataSet::DataBinding binding, std::shared_ptr<OIDITensorSetI> tensors);

  virtual void setNormal (const SbVec3f & normal, TensorVectorMode tensorVectorMode);
};

class OIDITensorVectorSignSetI : public OIDIScalarSetI
{
  std::shared_ptr<OIDITensorSetI> m_tensors;

public:

  OIDITensorVectorSignSetI(size_t dataSize, MiDataSet::DataBinding binding, std::shared_ptr<OIDITensorSetI> tensors);

  virtual void setNormal (const SbVec3f & normal, TensorVectorMode tensorVectorMode);
  virtual bool isTensorVectorSignDataSet () { return true; }
};

OIDIScalarSetI * GenerateScalarsFromScalars(const OIDIScalarSetI& scalars);
OIDIScalarSetI * GenerateScalarsFromVectors(const OIDIVectorSetI& vectors, unsigned short component);
OIDIScalarSetI * GenerateScalarsFromTensors(const OIDITensorSetI& tensors, unsigned short component);
OIDIVectorSetI * GenerateEigenVectorsFromTensors(const OIDITensorSetI& tensors, unsigned short component);
OIDIScalarSetI * GenerateEigenValuesFromTensors(const OIDITensorSetI& tensors, unsigned short component);

void GenerateEigenVectorsAndValuesFromTensors(const OIDITensorSetI& tensors, unsigned short component, OIDIVectorSetI *& vectors, OIDIScalarSetI * & values);

std::array<OIDIScalarSetI*, 2> generateDefaultBeachballScalarSets(const OIDITensorSetI& tensors);

MbVec3d product (const ITensor & tensor, const SbVec3f & vector);