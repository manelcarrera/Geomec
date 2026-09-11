#include "IDCValueTable.h"

#include "DCLoadManager.h"
#include "IDCDianaRunner.h"
#include "IProgressBase.h"

#include "IElement.h"

#include "lbcx.h"
#include "lbfl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

IValueTable::IValueTable(const IDianaRunner &runner)
    : m_mesh(runner.Mesh()), m_dMinVal(DBL_MAX), m_dMaxVal(-DBL_MAX), m_bCleanUp(true) {}

IValueTable::~IValueTable() {
  for (size_t i = 0; i < m_vcTimes.size(); ++i)
    delete m_vcTimes[i];
}

const IValueTable::CTimePoint &IValueTable::AddTime(double dTime) {
  CTimePoint *pTimePoint = new CTimePoint(dTime);
  m_vcTimes.push_back(pTimePoint);

  return *pTimePoint;
}

int IValueTable::Size() const { return (int)m_vcTimes.size(); }

const IValueTable::CTimePoint &IValueTable::Time(int nIndex) const {
  assert(nIndex >= 0 && nIndex < Size());
  return *m_vcTimes[nIndex];
}

void IValueTable::Write(IProgressBase &progress) {
  bool bValuesWritten = false;

  size_t timeSize = m_vcTimes.size();
  size_t meshSize = m_mesh.ElementSize();

  assert(timeSize > 1);
  assert(meshSize > 0);

  size_t oldTime = m_vcElementNodeBasedValues.size();

  if (oldTime > timeSize) // something odd is going on, so start fresh
  {
    DoCleanUp();
    oldTime = 0;
  }

  m_vcElementNodeBasedValues.resize(timeSize); // time X elements X nodes, indexed below by t, e and n

  for (size_t t = oldTime; t < timeSize;
       ++t) // in case of incremental building of m_vcElementNodeBasedValues, we need to add the new times
  {
    m_vcElementNodeBasedValues[t].resize(meshSize);

    for (size_t e = 0; e < meshSize; e++) {
      if (HasValue(m_mesh.Element(e))) {
        m_vcElementNodeBasedValues[t][e].resize(m_mesh.Element(e).NrOfNodes());

        std::vector<double> vcValues; // values for all nodes in element(e)
        ValueAt(m_mesh.Element(e), *m_vcTimes[t], vcValues);

        assert(m_mesh.Element(e).NrOfNodes() == vcValues.size());

        for (size_t n = 0; n < m_mesh.Element(e).NrOfNodes(); ++n) {
          m_vcElementNodeBasedValues[t][e][n] = vcValues[n];

          m_dMaxVal = std::max(m_dMaxVal, vcValues[n]);
          m_dMinVal = std::min(m_dMinVal, vcValues[n]);
        }
      }
    }
  }

  double dRange = m_dMaxVal - m_dMinVal;
  double dRelRange = dRange / (m_vcTimes[timeSize - 1]->Time() - m_vcTimes[0]->Time());

  // sort out which values are actually necessary to write to DIANA
  // we only need the first and the last values, and the ones where
  // the graph bends (direction coefficient changes).

  // this vector keeps track of the necessity to write the values
  std::vector<int> vcWriteValues(timeSize, 0);

  // this vector keeps track of whether values for each node of an element must be written
  std::vector<int> vcNodalValues(meshSize, 0);

  // we always need the first and the last
  vcWriteValues[0] = 1;
  vcWriteValues[timeSize - 1] = 1;

  if (!CompressTable()) // write all
  {
    for (size_t t = 1; t < timeSize - 1; ++t)
      vcWriteValues[t] = 1;
  } else {
    for (size_t t = 1; t < timeSize - 1; ++t) {
      for (size_t e = 0; e < meshSize && !vcWriteValues[t]; ++e) {
        for (size_t n = 0; n < m_vcElementNodeBasedValues[t][e].size(); ++n) // loop over element e's nodes
        {
          double dir1 = (m_vcElementNodeBasedValues[t][e][n] - m_vcElementNodeBasedValues[t - 1][e][n]) /
                        (m_vcTimes[t] - m_vcTimes[t - 1]);
          double dir2 = (m_vcElementNodeBasedValues[t + 1][e][n] - m_vcElementNodeBasedValues[t][e][n]) /
                        (m_vcTimes[t + 1] - m_vcTimes[t]);
          if (fabs(dir1 - dir2) > 1e-8 * dRelRange) {
            vcWriteValues[t] = 1;
            break; // no need to look further, we can start processing next t
          }
        }
      }
    }
  }

  // this compression (only write single values when all nodal values are equal) is always performed
  // switching t- and e-loop, as we can stop for element e as soon as we find different values in any t
  for (size_t e = 0; e < meshSize; ++e) {
    for (size_t t = 0; t < timeSize && !vcNodalValues[e]; ++t) {
      for (size_t n = 1; n < m_vcElementNodeBasedValues[t][e].size(); ++n) {
        if (fabs(m_vcElementNodeBasedValues[t][e][n] - m_vcElementNodeBasedValues[t][e][n - 1]) > 1e-8 * dRange) {
          vcNodalValues[e] = 1;
          break;
        }
      }
    }
  }

  size_t ns = 0;
  for (size_t t = 0; t < timeSize; ++t)
    ns += vcWriteValues[t];

  size_t msize = 0;
  ftn_double_t *values = 0;
  for (size_t e = 0; e < meshSize; ++e) {
    if (!m_vcElementNodeBasedValues[0][e].empty()) {
      PushDir();

      ftn_int_t idx = (ftn_int_t)(e + 1);
      ChangeIndexedDir("/ELEMEN/", &idx);
      size_t size = m_vcElementNodeBasedValues[0][e].size() * ns;
      if (size > msize)
        values = (ftn_double_t *)DiRealloc(values, msize * sizeof(ftn_double_t), size * sizeof(ftn_double_t),
                                           "IValueTable::Write");
      size_t values_idx = 0;
      bool bNonZero = false;
      size_t nodeSize = vcNodalValues[e] ? m_vcElementNodeBasedValues[0][e].size() : 1; // write all or only first one
      for (size_t n = 0; n < nodeSize; ++n) {
        for (size_t t = 0; t < timeSize; ++t) {
          if (vcWriteValues[t]) {
            assert(values_idx < size);
            values[values_idx] = (ftn_double_t)(m_vcElementNodeBasedValues[t][e][n]);
            if (!bNonZero && fabs(values[values_idx]) > 1e-8 * dRange)
              bNonZero = true;
            ++values_idx;
          }
        }
      }

      if (bNonZero) {
        bValuesWritten = true;
        PutItemLength(TableName().c_str(), values, values_idx);
      }

      PopDir();
    }

    progress.Step();
  }

  DiFree(values, "IValueTable::Write");

  if (bValuesWritten) {
    for (int t = timeSize - 1; t >= 0; --t) {
      if (!vcWriteValues[t])
        m_vcTimes.erase(m_vcTimes.begin() + t);
    }

    WriteTable();
  }

  if (m_bCleanUp) {
    DoCleanUp();
  }
}

void IValueTable::SetRunner(IDianaRunner * /*runner*/) {}

void IValueTable::CleanUp(bool cleanup) { m_bCleanUp = cleanup; }

bool IValueTable::CompressTable() const { return true; }

void IValueTable::DoCleanUp() {
  m_dMinVal = DBL_MAX;
  m_dMaxVal = -DBL_MAX;
  m_vcElementNodeBasedValues.clear();
}

void IValueTable::WriteTable() {
  // the table is written in a karray
  ftn_int_t ref[1]; // the reference array
  ftn_int_t lout;

  std::string sKarrayName = TimeTableName();

  // F_WKarray returns a Fortran index, so decrease with 1
  ftn_int_t idx = F_WKarray(sKarrayName.c_str(), ref, 'R', m_vcTimes.size(), &lout) - 1;

  // the real array
  ftn_double_t *ar = (ftn_double_t *)&ref[idx];

  for (size_t i = 0; i < m_vcTimes.size(); ++i) {
    ar[i] = (ftn_double_t)m_vcTimes[i]->Time();
  }

  F_Release(sKarrayName.c_str());
}

///// IValueTable::CTimePoint
IValueTable::CTimePoint::CTimePoint(double time) : m_time(time) {}

const double &IValueTable::CTimePoint::Time() const { return m_time; }

} // namespace dia
