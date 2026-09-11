#ifndef _OIDI_TOPOLOGY_H_
#define _OIDI_TOPOLOGY_H_

#include <MeshVizXLM/mesh/topology/MiVolumeTopologyExplicitI.h>
#include <MeshVizXLM/mesh/topology/MiSurfaceTopologyExplicitI.h>
#include <MeshVizXLM/mesh/topology/MiLineTopologyExplicitI.h>


/// template base class for all OIDI...TopologyExplicitI classes
template <class CellType, class MiTopologyExplicitIType>
class OIDITopologyExplicitI : public MiTopologyExplicitIType
{

private:
  const geo::IElementSet *m_Parent;

  std::vector<CellType> *m_Cells;

  std::size_t m_TimeStamp;

  std::size_t m_beginNodeId;
  std::size_t m_endNodeId;

public:
  OIDITopologyExplicitI(const geo::IElementSet *parent, const std::vector<CellType>& cells) : m_Parent(parent), m_Cells(0), m_TimeStamp(MxTimeStamp::getTimeStamp())
  {
  if (!m_Parent)
      return;

  assert(m_Parent);
  assert(m_Parent->ElementSize() > 0);
  assert(cells.size() > 0 && cells.size() <= m_Parent->ElementSize());

  m_beginNodeId = (std::size_t) - 1;
  m_endNodeId = 0;

  std::vector<size_t>::iterator searchIter;
  for (std::size_t i = 0; i < cells.size(); ++i)
  {
      for (std::size_t j = 0; j < cells[i].getNumNodes(); ++j)
      {
    std::size_t id = cells[i].getNodeIndex(j);

    m_beginNodeId = std::min(id, m_beginNodeId);
    m_endNodeId = std::max(id, m_endNodeId);
      }
  }

  m_endNodeId++;

  m_Cells = new std::vector<CellType>(cells);
  }

  OIDITopologyExplicitI(const geo::IArray *parent, const std::vector<CellType>& cells) : m_Parent(0), m_Cells(0), m_TimeStamp(MxTimeStamp::getTimeStamp())
  {
  assert(cells.size() > 0 && cells.size() <= parent->Size());


  if (dynamic_cast<const geo::ILine *>(&parent->ObjectBase(0)))
  {
      m_beginNodeId = 0;
      for (size_t i = 0; i < parent->Size() && dynamic_cast<const geo::ILine *>(&parent->ObjectBase(i)); ++i)
      {
    m_endNodeId = 2 * i + 1;
      }

  }
  else
  {
      m_beginNodeId = (std::size_t) - 1;
      m_endNodeId = 0;

      std::vector<size_t>::iterator searchIter;
      for (std::size_t i = 0; i < cells.size(); ++i)
      {
    for (std::size_t j = 0; j < cells[i].getNumNodes(); ++j)
    {
          std::size_t id = cells[i].getNodeIndex(j);

          m_beginNodeId = std::min(id, m_beginNodeId);
          m_endNodeId = std::max(id, m_endNodeId);
    }
      }
  }

  m_endNodeId++;

  m_Cells = new std::vector<CellType>(cells);
  }

  OIDITopologyExplicitI(const geo::IBox *parent, const std::vector<CellType>& cells) : m_Parent(0), m_Cells(0), m_TimeStamp(MxTimeStamp::getTimeStamp())
  {
  // assert(cells.size() > 0 && cells.size() <= parent->Size());

  m_beginNodeId = 0;
  m_endNodeId = parent->NrOfLines() * 2;

  m_Cells = new std::vector<CellType>(cells);
  }


  ~OIDITopologyExplicitI()
  {
  assert(m_Cells);
  delete m_Cells;
  }

  const geo::IElementSet *getParent() { return m_Parent; }

  size_t getNumCells() const
  {
  assert(m_Cells);

  return m_Cells->size();
  }

  const CellType* getCell(size_t id) const
  {
  assert(m_Cells);
  if (id >= m_Cells->size())
      return 0;

  return &(*m_Cells)[id];
  }

  size_t getBeginNodeId() const
  {
  return m_beginNodeId;
  }

  size_t getEndNodeId() const
  {
  return m_endNodeId;
  }

  size_t getTimeStamp() const
  {
  return m_TimeStamp;
  }
};

class OIDIVolumeTopologyExplicitI : public OIDITopologyExplicitI<OIDIVolumeCell, MiVolumeTopologyExplicitI>
{

public:
  OIDIVolumeTopologyExplicitI(const geo::IElementSet *parent, const std::vector<OIDIVolumeCell>& cells) : OIDITopologyExplicitI(parent, cells)
  {
  }
  OIDIVolumeTopologyExplicitI(const geo::IArray *parent, const std::vector<OIDIVolumeCell>& cells) : OIDITopologyExplicitI(parent, cells)
  {
  }
  OIDIVolumeTopologyExplicitI(const geo::IBox *parent, const std::vector<OIDIVolumeCell>& cells) : OIDITopologyExplicitI(parent, cells)
  {
  }
};

class OIDISurfaceTopologyExplicitI : public OIDITopologyExplicitI<OIDISurfaceCell, MiSurfaceTopologyExplicitI>
{
public:
  OIDISurfaceTopologyExplicitI(const geo::IElementSet *parent, const std::vector<OIDISurfaceCell>& cells) : OIDITopologyExplicitI(parent, cells)
  {
  }
  OIDISurfaceTopologyExplicitI(const geo::IArray *parent, const std::vector<OIDISurfaceCell>& cells) : OIDITopologyExplicitI(parent, cells)
  {
  }
  OIDISurfaceTopologyExplicitI(const geo::IBox *parent, const std::vector<OIDISurfaceCell>& cells) : OIDITopologyExplicitI(parent, cells)
  {
  }
};

class OIDILineTopologyExplicitI : public OIDITopologyExplicitI<OIDILineCell, MiLineTopologyExplicitI>
{
public:
  OIDILineTopologyExplicitI(const geo::IElementSet *parent, const std::vector<OIDILineCell>& cells) : OIDITopologyExplicitI(parent, cells)
  {
  }
  OIDILineTopologyExplicitI(const geo::IArray *parent, const std::vector<OIDILineCell>& cells) : OIDITopologyExplicitI(parent, cells)
  {
  }
  OIDILineTopologyExplicitI(const geo::IBox *parent, const std::vector<OIDILineCell>& cells) : OIDITopologyExplicitI(parent, cells)
  {
  }
};



class OIDIPointSetTopology : public MiSurfaceTopologyExplicitI
{
  const geo::IArray * m_pointSet;
  OIDIPointSetCell * m_cell;
  size_t m_timeStamp;

public:
  OIDIPointSetTopology(const geo::IArray *pointSet) : m_pointSet(pointSet), m_timeStamp(MxTimeStamp::getTimeStamp())
  {
  m_cell = new OIDIPointSetCell(m_pointSet);
  }

  virtual size_t getBeginNodeId() const
  {
  return 0;
  }
  virtual size_t getEndNodeId() const
  {
  return m_pointSet->Size();
  }

  virtual const MiSurfaceCell* getCell(size_t id) const
  {
  return m_cell;
  }

  virtual size_t getNumCells() const
  {
  return 1;
  }

  virtual size_t getTimeStamp() const
  {
  return m_timeStamp;
  }
};

#endif