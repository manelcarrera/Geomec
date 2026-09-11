#include "OIDIMesh.h"
#include "OIDIDataSetI.h"

#include "Global.h"
#include "IProgressFactory.h"
#include "MPKernel.h"
#include "MeshVizInterface\extractors\MiPointProbeUnstructured.h"
#include "OIDITaskCollector.h"
#include "ValueMapper.h"

// explicit template instantiations
template class OIDIMeshTemplate<OIDIVolumeTopologyExplicitI, OIDIVolumeCell, MiVolumeMeshUnstructured>;
template class OIDIMeshTemplate<OIDISurfaceTopologyExplicitI, OIDISurfaceCell, MiSurfaceMeshUnstructured>;
template class OIDIMeshTemplate<OIDILineTopologyExplicitI, OIDILineCell, MiLineMeshUnstructured>;

// we get multiple values, which need to be averaged; if one of the values is invalid, the average is invalid
// TODO: check in Diana code how they handle addition of invalid CValue; if they do it right, we can simplify
double computeAverage(const IValueDataInterfaceScalar::TValueVec &valueVec, bool isInterfaceElt) {
  if (valueVec.size() == 0)
    return DBL_UNDEFINED;

  geo::CValue average = valueVec[0];

  bool valid = average.Valid();

  size_t size = isInterfaceElt ? valueVec.size() / 2 : valueVec.size();

  for (std::size_t j = 1; valid && j < size; ++j) {
    if (valueVec[j].Valid())
      average += valueVec[j];
    else
      valid = false;
  }

  return valid ? average.Value() / size : DBL_UNDEFINED;
}

MbVec3d computeAverage(const std::vector<geo::CVector> &vectors) {
  MbVec3d v(0);

  std::size_t processed = 0;

  for (std::size_t i = 0; i < vectors.size(); ++i) {
    if (!vectors[i].Empty()) // not sure if empty vectors can happen
    {
      v += MbVec3d(vectors[i].X(), vectors[i].Y(), vectors[i].Z());
      ++processed;
    }
  }

  if (processed > 0)
    v /= processed;

  return v;
}

/////////////////////////////////////////////////////////////////////
// CollectValuesByGeometry
/////////////////////////////////////////////////////////////////////

class CTaskCollectValuesByGeometry {
  const OIDIGeometryI *m_geometry;

  const IValueComponentBase *m_pValueComponent;
  IQuantityDouble::UNIT m_unit;

  CScalarCollector &m_collector;

  mp::CKernelDispatcher *m_dispatcher;

  bool m_bSequential;

public:
  CTaskCollectValuesByGeometry(const OIDIGeometryI &g, const IValueComponentBase *pValueComponent,
                               IQuantityDouble::UNIT unit, CScalarCollector &collector)
      : m_geometry(&g), m_pValueComponent(pValueComponent), m_unit(unit), m_collector(collector), m_dispatcher(0),
        m_bSequential(false) {}

  // geometry is not a vector format, but since we don't use data(), we just return a dummy vector
  MP_DUMMY_DATA

  size_t size() const { return m_geometry->getSize(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void setSequential() { m_bSequential = true; }

  void operator()(const tbb::blocked_range<size_t> &r) const {
    geo::IParallelInitializationCallback *callback = 0;

    if (!m_bSequential && m_pValueComponent->NeedParallelInitializationCallback()) {
      callback = const_cast<IValueComponentBase *>(m_pValueComponent)->GetParallelInitializationCallback();
      if (!callback)
        callback = const_cast<IValueComposite &>(m_pValueComponent->Parent()).GetParallelInitializationCallback();
      assert(callback);
    }

    size_t offset = r.begin();

    CScalarCollector::record_type *record = new CScalarCollector::record_type(offset, r.size());
    CScalarCollector::value_vector_type &values = record->getValues();

    for (size_t i = r.begin(); i != r.end(); ++i) {
      geo::CValue result = m_pValueComponent->ScalarData().ValuePoint(*m_geometry->getPoint(i), m_unit, callback);

      if (result.Valid())
        values[i - offset] = result.Value();

      if ((i % 100) == 0 && m_dispatcher && m_bSequential)
        m_dispatcher->Step(100);
    }
    if (m_dispatcher)
      m_dispatcher->Step(m_bSequential ? r.size() % 100 : r.size());

    m_collector.add(record);
  }
};

OIDIScalarSetI *OIDIMesh::collectValuesByGeometry(const IValueComponentBase *pValueComponent,
                                                  IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  std::size_t geometrySize = m_geometry.getSize();
  OIDIScalarSetI *values = 0;

  values = new OIDIScalarSetI(geometrySize, MiDataSet::PER_NODE);

  CValueMapper *vm = CValueMapper::instance();

  if (vm->UseLegacyCode()) {
    for (std::size_t i = 0; i < geometrySize; ++i) {
      geo::CValue result = pValueComponent->ScalarData().ValuePoint(*m_geometry.getPoint(i), unit);

      if (result.Valid())
        values->add(result.Value());
      else // *shrug*
        values->add(DBL_UNDEFINED);

      if (i % 100 == 0)
        progress.Step(100);
    }
  } else {
    const geo::IElementSet *pElementSet = 0;

    typedef CTaskCollectValuesByGeometry CTaskCollect;

    CScalarCollector collector;

    CTaskCollect taskCollect(m_geometry, pValueComponent, unit, collector);

    mp::CKernelDispatcher kernelDispatcher;

    if (vm->PrepareMapping(pElementSet, pValueComponent) && vm->AllowParallelMapping()) {
      mp::CKernelParallel<CTaskCollect> parKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelParallel, CTaskCollect)(parKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    } else {
      taskCollect.setSequential();
      mp::CKernelSequential<CTaskCollect> seqKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelSequential, CTaskCollect)(seqKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    }

    vm->FinishMapping();

    if (kernelDispatcher.Canceled())
      throw new CProgressCancel();

    collector.collect(*values);
  }

  return values;
}

OIDIVectorSetI *OIDIMesh::collectVectorsByGeometry(const IVectorResult::IVectorComponent *pVectorComponent,
                                                   IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  std::size_t geometrySize = m_geometry.getSize();

  OIDIVectorSetI *values = new OIDIVectorSetI(geometrySize, MiDataSet::PER_NODE);

  const IVectorResult *pResult = dynamic_cast<const IVectorResult *>(&pVectorComponent->Parent());

  for (std::size_t i = 0; i < geometrySize; ++i) {
    const geo::IVector &v = pResult->VectorPoint(*m_geometry.getPoint(i), *pVectorComponent);

    if (unit == IQuantityDouble::FIELD_UNIT)
      values->add(
          MbVec3d(pResult->ConvertToField(v.X()), pResult->ConvertToField(v.Y()), pResult->ConvertToField(v.Z())));
    else
      values->add(MbVec3d(v.X(), v.Y(), v.Z()));
    if (i % 100 == 0)
      progress.Step(100);
  }

  return values;
}

OIDITensorSetI *OIDIMesh::collectTensorsByGeometry(const IValueComponentBase *pTensorComponent,
                                                   IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  std::size_t geometrySize = m_geometry.getSize();

  const IValueDataInterfaceTensor *tensorInterface = pTensorComponent->TensorData();

  OIDITensorSetI *tensorSet = new OIDITensorSetI(geometrySize, MiDataSet::PER_NODE);

  for (std::size_t i = 0; i < geometrySize; ++i) {
    CTensor t = tensorInterface->ValuePoint(*m_geometry.getPoint(i), unit);

    tensorSet->add(t);

    if (i % 100 == 0)
      progress.Step(100);
  }

  return tensorSet;
}

OIDIScalarSetI *OIDIMeshSU::collectValuesByTopology(const IValueComponentBase *pValueComponent,
                                                    IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  if (!isFault())
    return OIDIMeshTemplate<OIDISurfaceTopologyExplicitI, OIDISurfaceCell,
                            MiSurfaceMeshUnstructured>::collectValuesByTopology(pValueComponent, unit, progress);
  else
    return collectFaultedValuesByTopology(progress, pValueComponent);
}

bool OIDIMeshSU::isFault() const {
  return getTopology().getNumCells() && getTopology().getCell(0)->getInterfaceElement();
}

bool OIDIMeshSU::needFault(const CGraphNode *node) const {
  if (getTopology().getNumCells() && getTopology().getCell(0)->getInterfaceElement()) {
    assert(dynamic_cast<const IValueComposite *>(node));
    return static_cast<const IValueComposite *>(node)->isFaultResult();
  }
  return false;
}

OIDIScalarSetI *OIDIMeshSU::collectValuesByGeometry(const IValueComponentBase *pValueComponent,
                                                    IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  if (!isFault())
    return collectGeometryValuesByTopology(pValueComponent, unit, progress);
  else
    return collectFaultedValuesByGeometry(pValueComponent, unit, progress);
}

// X001
OIDIScalarSetI *OIDIMeshSU::collectFaultedValuesByGeometry(const IValueComponentBase *pValueComponent,
                                                           IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  std::size_t numTopologyCells = getTopology().getNumCells();
  std::size_t geometrySize = getGeometry().getSize();

  OIDIScalarSetI *values = new OIDIScalarSetI(geometrySize, MiDataSet::PER_NODE);

  for (std::size_t i = 0; i < numTopologyCells; ++i) {
    const geo::IInterfaceElement *ifElt = getTopology().getCell(i)->getInterfaceElement();

    IValueDataInterfaceScalar::TValueVec results = pValueComponent->ScalarData().ValueElement(*ifElt, unit);

    for (std::size_t j = 0; j < ifElt->NrOfNodes() / 2; ++j)
      if (results[j].Valid())
        values->set(ifElt->PointIndex(j), results[j].Value());

    if (i % 100 == 0)
      progress.Step(100);
  }

  return values;
}

OIDIVectorSetI *OIDIMeshSU::collectVectorsByTopology(const IVectorResult::IVectorComponent *pVectorComponent,
                                                     IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  if (!isFault()) {
    return OIDIMeshTemplate<OIDISurfaceTopologyExplicitI, OIDISurfaceCell,
                            MiSurfaceMeshUnstructured>::collectVectorsByTopology(pVectorComponent, unit, progress);
  } else {
    return collectFaultedVectorsByTopology(pVectorComponent, unit, progress);
  }
}

OIDIVectorSetI *OIDIMeshSU::collectVectorsByGeometry(const IVectorResult::IVectorComponent *pVectorComponent,
                                                     IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  if (!isFault())
    return collectGeometryVectorsByTopology(pVectorComponent, unit, progress);
  else
    return collectFaultedVectorsByGeometry(pVectorComponent, unit, progress);
}

/////////////////////////////////////////////////////////////////////
// CollectFaultedVectorsByGeometry
/////////////////////////////////////////////////////////////////////

class CTaskCollectFaultedVectorsByGeometry {
  const OIDISurfaceTopologyExplicitI *m_topology;

  const IVectorResult::IVectorComponent *m_pVectorComponent;
  IQuantityDouble::UNIT m_unit;

  CInterfaceVectorCollector &m_collector;

  mp::CKernelDispatcher *m_dispatcher;

  bool m_bSequential;

public:
  CTaskCollectFaultedVectorsByGeometry(const OIDISurfaceTopologyExplicitI &t,
                                       const IVectorResult::IVectorComponent *pVectorComponent,
                                       IQuantityDouble::UNIT unit, CInterfaceVectorCollector &collector)
      : m_topology(&t), m_pVectorComponent(pVectorComponent), m_unit(unit), m_collector(collector), m_dispatcher(0),
        m_bSequential(false) {}

  // topology is not a vector format, but since we don't use data(), we just return a dummy vector
  MP_DUMMY_DATA

  size_t size() const { return m_topology->getNumCells(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void setSequential() { m_bSequential = true; }

  void operator()(const tbb::blocked_range<size_t> &r) const {
#ifdef DEBUG_MPKERNEL
    mp::MPOutput::instance()->write(QString("START range from %1 to %2\n").arg(r.begin()).arg(r.end()).toStdString());
#endif
    std::vector<geo::CVector> results;

    geo::IParallelInitializationCallback *callback =
        !m_bSequential ? const_cast<IValueComposite &>(m_pVectorComponent->Parent()).GetParallelInitializationCallback()
                       : 0;

    size_t offset = r.begin();

    CInterfaceVectorCollector::record_type *record = new CInterfaceVectorCollector::record_type(r.begin(), r.size());

    CInterfaceVectorCollector::value_vector_type &values = record->getValues();

    for (size_t i = r.begin(); i != r.end(); ++i) {
      const geo::IInterfaceElement *ifElt = m_topology->getCell(i)->getInterfaceElement();

      m_pVectorComponent->MapValueElementCB(*ifElt, results, CValueType::MT_NONE, m_unit,
                                            callback); // no weighting for now

      CInterfaceVectorCollector::value_type &value = values[i - offset];

      value.size = ifElt->NrOfNodes() / 2;

      for (std::size_t j = 0; j < ifElt->NrOfNodes() / 2; ++j) {
        value.index[j] = ifElt->PointIndex(j);
        value.value[j][0] = results[j].X();
        value.value[j][1] = results[j].Y();
        value.value[j][2] = results[j].Z();
      }

      if ((i % 100) == 0 && m_dispatcher && m_bSequential)
        m_dispatcher->Step(100);
    }

    delete callback;

    if (m_dispatcher)
      m_dispatcher->Step(m_bSequential ? r.size() % 100 : r.size());

    m_collector.add(record);
#ifdef DEBUG_MPKERNEL
    mp::MPOutput::instance()->write(
        QString("END range from %1 to %2, size is %3\n").arg(r.begin()).arg(r.end()).arg(r.size()).toStdString());
#endif
  }
};

OIDIVectorSetI *OIDIMeshSU::collectFaultedVectorsByGeometry(const IVectorResult::IVectorComponent *pVectorComponent,
                                                            IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  std::size_t numTopologyCells = getTopology().getNumCells();
  std::size_t geometrySize = getGeometry().getSize();

  OIDIVectorSetI *vectors = new OIDIVectorSetI(geometrySize, MiDataSet::PER_NODE);

  CValueMapper *vm = CValueMapper::instance();

  if (vm->UseLegacyCode()) {
    std::vector<geo::CVector> results;
    for (std::size_t i = 0; i < numTopologyCells; ++i) {
      const geo::IInterfaceElement *ifElt = getTopology().getCell(i)->getInterfaceElement();

      pVectorComponent->MapValueElement(*ifElt, results, CValueType::MT_NONE, unit); // no weighting for now

      for (std::size_t j = 0; j < ifElt->NrOfNodes() / 2; ++j)
        vectors->set(ifElt->PointIndex(j), MbVec3d(results[j].X(), results[j].Y(), results[j].Z()));

      if (i % 100 == 0)
        progress.Step(100);
    }
  } else {
    const geo::IElementSet *pElementSet =
        numTopologyCells > 0 ? getTopology().getCell(0)->getParent()->IndexingElementSet() : 0;

    typedef CTaskCollectFaultedVectorsByGeometry CTaskCollect;

    CInterfaceVectorCollector collector;

    CTaskCollect taskCollect(getTopology(), pVectorComponent, unit, collector);

    mp::CKernelDispatcher kernelDispatcher;

    vectors->adjustForInterfaceElements(
        getTopology()); // not needed, but should lead to less allocation actions in the set

    if (vm->PrepareMapping(pElementSet, pVectorComponent) && vm->AllowParallelMapping()) {
      mp::CKernelParallel<CTaskCollect> parKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelParallel, CTaskCollect)(parKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    } else {
      taskCollect.setSequential();
      mp::CKernelSequential<CTaskCollect> seqKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelSequential, CTaskCollect)(seqKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    }

    vm->FinishMapping();

    if (kernelDispatcher.Canceled())
      throw new CProgressCancel();

    collector.collect(*vectors);
  }

  return vectors;
}

OIDITensorSetI *OIDIMeshSU::collectTensorsByGeometry(const IValueComponentBase *pTensorComponent,
                                                     IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  return collectGeometryTensorsByTopology(pTensorComponent, unit, progress);
}

OIDIMesh::MeshType OIDIMeshSU::meshType() const { return OIDIMesh::SurfaceMesh; }

/////////////////////////////////////////////////////////////////////
// CollectFaultedVectorsByGeometry
/////////////////////////////////////////////////////////////////////

class CTaskCollectFaultedVectorsByTopology {
  const OIDISurfaceTopologyExplicitI *m_topology;

  const IVectorResult::IVectorComponent *m_pVectorComponent;
  IQuantityDouble::UNIT m_unit;

  CVectorCollector &m_collector;

  mp::CKernelDispatcher *m_dispatcher;

  bool m_bSequential;

public:
  CTaskCollectFaultedVectorsByTopology(const OIDISurfaceTopologyExplicitI &t,
                                       const IVectorResult::IVectorComponent *pVectorComponent,
                                       IQuantityDouble::UNIT unit, CVectorCollector &collector)
      : m_topology(&t), m_pVectorComponent(pVectorComponent), m_unit(unit), m_collector(collector), m_dispatcher(0),
        m_bSequential(false) {}

  // topology is not a vector format, but since we don't use data(), we just return a dummy vector
  MP_DUMMY_DATA

  size_t size() const { return m_topology->getNumCells(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void setSequential() { m_bSequential = true; }

  void operator()(const tbb::blocked_range<size_t> &r) const {
#ifdef DEBUG_MPKERNEL
    mp::MPOutput::instance()->write(QString("START range from %1 to %2\n").arg(r.begin()).arg(r.end()).toStdString());
#endif
    std::vector<geo::CVector> results;

    geo::IParallelInitializationCallback *callback =
        !m_bSequential ? const_cast<IValueComposite &>(m_pVectorComponent->Parent()).GetParallelInitializationCallback()
                       : 0;

    size_t offset = r.begin();

    CVectorCollector::record_type *record = new CVectorCollector::record_type(offset, r.size());
    CVectorCollector::value_vector_type &values = record->getValues();

    for (size_t i = r.begin(); i != r.end(); ++i) {
      const geo::IInterfaceElement *ifElt = m_topology->getCell(i)->getInterfaceElement();

      m_pVectorComponent->MapValueElementCB(*ifElt, results, CValueType::MT_NONE, m_unit,
                                            callback); // no weighting for now

      // for (std::size_t j = 0; j < ifElt->NrOfNodes() / 2; ++j)
      values[i - offset] = computeAverage(results);

      if ((i % 100) == 0 && m_dispatcher && m_bSequential)
        m_dispatcher->Step(100);
    }

    delete callback;

    if (m_dispatcher)
      m_dispatcher->Step(m_bSequential ? r.size() % 100 : r.size());

    m_collector.add(record);
#ifdef DEBUG_MPKERNEL
    mp::MPOutput::instance()->write(
        QString("END range from %1 to %2, size is %3\n").arg(r.begin()).arg(r.end()).arg(r.size()).toStdString());
#endif
  }
};

OIDIVectorSetI *OIDIMeshSU::collectFaultedVectorsByTopology(const IVectorResult::IVectorComponent *pVectorComponent,
                                                            IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  std::size_t numTopologyCells = getTopology().getNumCells();

  OIDIVectorSetI *vectors = new OIDIVectorSetI(numTopologyCells, MiDataSet::PER_CELL);

  CValueMapper *vm = CValueMapper::instance();

  if (vm->UseLegacyCode()) {
    std::vector<geo::CVector> results;
    for (std::size_t i = 0; i < numTopologyCells; ++i) {
      pVectorComponent->MapValueElement(*getTopology().getCell(i)->getInterfaceElement(), results, CValueType::MT_NONE,
                                        unit); // no weighting for now
      vectors->add(computeAverage(results));
      if (i % 100 == 0)
        progress.Step(100);
    }
  } else {
    const geo::IElementSet *pElementSet =
        numTopologyCells > 0 ? getTopology().getCell(0)->getParent()->IndexingElementSet() : 0;

    typedef CTaskCollectFaultedVectorsByTopology CTaskCollect;

    CVectorCollector collector;

    CTaskCollect taskCollect(getTopology(), pVectorComponent, unit, collector);

    mp::CKernelDispatcher kernelDispatcher;

    if (vm->PrepareMapping(pElementSet, pVectorComponent) && vm->AllowParallelMapping()) {
      mp::CKernelParallel<CTaskCollect> parKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelParallel, CTaskCollect)(parKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    } else {
      taskCollect.setSequential();
      mp::CKernelSequential<CTaskCollect> seqKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelSequential, CTaskCollect)(seqKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    }

    vm->FinishMapping();

    if (kernelDispatcher.Canceled())
      throw new CProgressCancel();

    collector.collect(*vectors);
  }

  return vectors;
}

OIDIScalarSetI *OIDIMeshSU::collectFaultedValuesByTopology(IProgressBase &progress,
                                                           const IValueComponentBase *pValueComponent) const {
  std::size_t numTopologyCells = getTopology().getNumCells();

  OIDIScalarSetI *values = new OIDIScalarSetI(numTopologyCells, MiDataSet::PER_CELL);

  for (std::size_t i = 0; i < numTopologyCells; ++i) {
    values->add(computeAverage(
        pValueComponent->ScalarData().ValueElement(*getTopology().getCell(i)->getInterfaceElement()), true));
    if (i % 100 == 0)
      progress.Step(100);
  }

  return values;
}

OIDIMesh::MeshType OIDIMeshLU::meshType() const { return OIDIMesh::LineMesh; }

OIDIMesh::MeshType OIDIMeshVU::meshType() const { return OIDIMesh::VolumeMesh; }

OIDIScalarSetI *OIDIMeshVU::collectValuesByGeometry(const IValueComponentBase *pValueComponent,
                                                    IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  return collectGeometryValuesByTopology(pValueComponent, unit, progress);
}

OIDIVectorSetI *OIDIMeshVU::collectVectorsByGeometry(const IVectorResult::IVectorComponent *pVectorComponent,
                                                     IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  return collectGeometryVectorsByTopology(pVectorComponent, unit, progress);
}

OIDITensorSetI *OIDIMeshVU::collectTensorsByGeometry(const IValueComponentBase *pTensorComponent,
                                                     IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  return collectGeometryTensorsByTopology(pTensorComponent, unit, progress);
}

bool OIDIMeshVU::computeCellMap(std::string meshName, const OIDIMeshVU *mainMesh, std::vector<int> &cellMap) const {
#if 0
  std::auto_ptr <IProgressBase> prog;

  char progressTitle[256];
  sprintf (progressTitle, "Mapping cells for formation '%s'", meshName.c_str());
  prog.reset(_g->prog()->create(eProgress::Geo, progressTitle, true));
  prog->AddSteps(getNumCells());
#endif
  size_t numCells = getNumCells();

  cellMap.reserve(numCells);
  for (size_t i = 0; i < numCells; ++i) {
    const OIDIVolumeCell *cell = getCell(i);
    int mainMeshCellId = mainMesh->findSimilarCell(cell);
    if (mainMeshCellId < 0)
      return false;

    cellMap.push_back(mainMeshCellId);
#if 0
    if (i % 100 == 99) prog->Step(100);
#endif
  }

  return true;
}

int OIDIMeshVU::findSimilarCell(const OIDIVolumeCell *cell) const {
  int numCells = getNumCells();

  // start where we left off.
  int i, n;
  for (n = 0, i = m_lastFound + 1; n < numCells; ++n, ++i) {
    i = i % numCells;

    if (getCell(i)->hasSameNodeIndices(cell)) {
      m_lastFound = i;
      return i;
    }
  }

  return -1;
}

MiPointProbeUnstructured *OIDIMeshVU::getProbe() const {
  if (!m_probe)
    m_probe = MiPointProbeUnstructured::getNewInstance(*this);
  return m_probe;
}

OIDIMeshVU::~OIDIMeshVU() { delete m_probe; }

template <class TopologyType, class CellType, class MiMeshUnstructuredType>
void OIDIMeshTemplate<TopologyType, CellType, MiMeshUnstructuredType>::checkConsistency(MbVec3d &min,
                                                                                        MbVec3d &max) const {
  min = DBL_MAX;
  max = -DBL_MAX;

  size_t numCells = getNumCells();
  for (std::size_t c = 0; c < numCells; c++) {
    const CellType *cell = getCell(c);

    for (std::size_t n = 0; n < cell->getNumNodes(); ++n) {
      std::size_t id = cell->getNodeIndex(n);
      size_t geometrySize = getGeometry().getSize();
      assert(id < geometrySize);

      MbVec3d coord = getGeometry().getCoord(id);
      for (int j = 0; j < 3; ++j) {
        min[j] = std::min(min[j], coord[j]);
        max[j] = std::max(max[j], coord[j]);
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////
// CollectGeometryValuesByTopology
/////////////////////////////////////////////////////////////////////

template <class TopologyType, class CellType> class CTaskCollectValuesByTopology {
  const TopologyType *m_topology;

  const IValueComponentBase *m_pValueComponent;
  IQuantityDouble::UNIT m_unit;

  CScalarCollector &m_collector;

  mp::CKernelDispatcher *m_dispatcher;

  bool m_bSequential;

public:
  CTaskCollectValuesByTopology(const TopologyType &t, const IValueComponentBase *pValueComponent,
                               IQuantityDouble::UNIT unit, CScalarCollector &collector)
      : m_topology(&t), m_pValueComponent(pValueComponent), m_unit(unit), m_collector(collector), m_dispatcher(0),
        m_bSequential(false) {}

  // topology is not a vector format, but since we don't use data(), we just return a dummy vector
  MP_DUMMY_DATA

  size_t size() const { return m_topology->getNumCells(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void setSequential() { m_bSequential = true; }

  void operator()(const tbb::blocked_range<size_t> &r) const {

    geo::IParallelInitializationCallback *callback = 0;

    if (!m_bSequential && m_pValueComponent->NeedParallelInitializationCallback()) {
      callback = const_cast<IValueComponentBase *>(m_pValueComponent)->GetParallelInitializationCallback();
      if (!callback)
        callback = const_cast<IValueComposite &>(m_pValueComponent->Parent()).GetParallelInitializationCallback();
      assert(callback);
    }

    size_t offset = r.begin();

    CScalarCollector::record_type *record = new CScalarCollector::record_type(r.begin(), r.size());
    CScalarCollector::value_vector_type &values = record->getValues();

    for (size_t i = r.begin(); i != r.end(); ++i) {
      const CellType *cell = m_topology->getCell(i);
      const IValueDataInterfaceScalar::TValueVec elementValues =
          m_pValueComponent->ScalarData().ValueElement(*cell->getParent(), m_unit, callback);

      values[i - offset] = computeAverage(elementValues);

      if ((i % 100) == 0 && m_dispatcher && m_bSequential)
        m_dispatcher->Step(100);
    }

    delete callback;

    if (m_dispatcher)
      m_dispatcher->Step(m_bSequential ? r.size() % 100 : r.size());

    m_collector.add(record);
  }
};

template <class TopologyType, class CellType, class MiMeshUnstructuredType>
OIDIScalarSetI *OIDIMeshTemplate<TopologyType, CellType, MiMeshUnstructuredType>::collectValuesByTopology(
    const IValueComponentBase *pValueComponent, IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  std::size_t numTopologyCells = m_topology.getNumCells();

  OIDIScalarSetI *values = new OIDIScalarSetI(numTopologyCells, MiDataSet::PER_CELL);

  CValueMapper *vm = CValueMapper::instance();

  if (vm->UseLegacyCode()) {
    for (std::size_t i = 0; i < numTopologyCells; ++i) {
      values->add(
          computeAverage(pValueComponent->ScalarData().ValueElement(*m_topology.getCell(i)->getParent(), unit)));
      if (i % 100 == 0)
        progress.Step(100);
    }
  } else {
    const geo::IElementSet *pElementSet =
        numTopologyCells > 0 ? m_topology.getCell(0)->getParent()->IndexingElementSet() : 0;

    typedef CTaskCollectValuesByTopology<TopologyType, CellType> CTaskCollect;

    CScalarCollector collector;

    CTaskCollect taskCollect(m_topology, pValueComponent, unit, collector);

    mp::CKernelDispatcher kernelDispatcher;

    if (vm->PrepareMapping(pElementSet, pValueComponent) && vm->AllowParallelMapping()) {
      mp::CKernelParallel<CTaskCollect> parKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelParallel, CTaskCollect)(parKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    } else {
      taskCollect.setSequential();
      mp::CKernelSequential<CTaskCollect> seqKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelSequential, CTaskCollect)(seqKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    }

    vm->FinishMapping();

    if (kernelDispatcher.Canceled())
      throw new CProgressCancel();

    collector.collect(*values);
  }

  return values;
}

/////////////////////////////////////////////////////////////////////
// CollectGeometryValuesByTopology
/////////////////////////////////////////////////////////////////////

template <class TopologyType, class CellType> class CTaskCollectGeometryValuesByTopology {
  const TopologyType *m_topology;

  const IValueComponentBase *m_pValueComponent;
  IQuantityDouble::UNIT m_unit;

  std::vector<double> &m_accumulatedValues;
  std::vector<size_t> &m_accumulationCounts;
  std::vector<size_t> &m_invalidValueCounts;
  std::vector<tbb::spin_mutex> &m_locks;

  mp::CKernelDispatcher *m_dispatcher;

  bool m_bSequential;

public:
  CTaskCollectGeometryValuesByTopology(const TopologyType &t, const IValueComponentBase *pValueComponent,
                                       IQuantityDouble::UNIT unit, std::vector<double> &accV, std::vector<size_t> &accC,
                                       std::vector<size_t> &invVC, std::vector<tbb::spin_mutex> &locks)
      : m_topology(&t), m_pValueComponent(pValueComponent), m_unit(unit), m_accumulatedValues(accV),
        m_accumulationCounts(accC), m_invalidValueCounts(invVC), m_locks(locks), m_dispatcher(0), m_bSequential(false) {
  }

  // topology is not a vector format, but since we don't use data(), we just return a dummy vector
  MP_DUMMY_DATA

  size_t size() const { return m_topology->getNumCells(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void setSequential() { m_bSequential = true; }

  void operator()(const tbb::blocked_range<size_t> &r) const {
#ifdef DEBUG_MPKERNEL
    mp::MPOutput::instance()->write(QString("START range from %1 to %2\n").arg(r.begin()).arg(r.end()).toStdString());
#endif

    geo::IParallelInitializationCallback *callback = 0;

    if (!m_bSequential && m_pValueComponent->NeedParallelInitializationCallback()) {
      callback = const_cast<IValueComponentBase *>(m_pValueComponent)->GetParallelInitializationCallback();
      if (!callback)
        callback = const_cast<IValueComposite &>(m_pValueComponent->Parent()).GetParallelInitializationCallback();
      assert(callback);
    }

    for (size_t i = r.begin(); i != r.end(); ++i) {
      const CellType *cell = m_topology->getCell(i);
      const IValueDataInterfaceScalar::TValueVec elementValues =
          m_pValueComponent->ScalarData().ValueElement(*cell->getParent(), m_unit, callback);

      size_t numCellNodes = cell->getNumNodes();
      for (size_t nodeIndex = 0; nodeIndex < numCellNodes; ++nodeIndex) {
        size_t cellNodeIndex = cell->getNodeIndex(nodeIndex);

        tbb::spin_mutex::scoped_lock lock(m_locks[cellNodeIndex]);

        if (nodeIndex < elementValues.size() && elementValues[nodeIndex].Valid()) {
          m_accumulatedValues[cellNodeIndex] += elementValues[nodeIndex].Value();
          ++m_accumulationCounts[cellNodeIndex];
        } else {
          ++m_invalidValueCounts[cellNodeIndex];
        }
      }
      if ((i % 100) == 0 && m_dispatcher && m_bSequential)
        m_dispatcher->Step(100);
    }
    if (m_dispatcher)
      m_dispatcher->Step(m_bSequential ? r.size() % 100 : r.size());

    delete callback;

#ifdef DEBUG_MPKERNEL
    mp::MPOutput::instance()->write(
        QString("END range from %1 to %2, size is %3\n").arg(r.begin()).arg(r.end()).arg(r.size()).toStdString());
#endif
  }
};

/// This function determines values for the nodes in a topology for a given property.
/// The values are returned as a list of values indexed according to the index of the node in the mesh geometry.
template <class TopologyType, class CellType, class MiMeshUnstructuredType>
OIDIScalarSetI *OIDIMeshTemplate<TopologyType, CellType, MiMeshUnstructuredType>::collectGeometryValuesByTopology(
    const IValueComponentBase *pValueComponent, IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  std::size_t numTopologyCells = m_topology.getNumCells();
  std::size_t geometrySize = getSize();
  OIDIScalarSetI *values = 0;

  values = new OIDIScalarSetI(0, MiDataSet::PER_NODE);

  std::vector<double> accumulatedValues(geometrySize, 0);
  std::vector<size_t> accumulationCounts(geometrySize, 0);
  std::vector<size_t> invalidValueCounts(geometrySize, 0);

  CValueMapper *vm = CValueMapper::instance();

  if (vm->UseLegacyCode()) {
    for (std::size_t i = 0; i < numTopologyCells; ++i) {
      const CellType *cell = m_topology.getCell(i);
      const IValueDataInterfaceScalar::TValueVec elementValues =
          pValueComponent->ScalarData().ValueElement(*cell->getParent(), unit);

      size_t numCellNodes = cell->getNumNodes();
      for (size_t nodeIndex = 0; nodeIndex < numCellNodes; ++nodeIndex) {
        if (nodeIndex < elementValues.size() && elementValues[nodeIndex].Valid()) {
          accumulatedValues[cell->getNodeIndex(nodeIndex)] += elementValues[nodeIndex].Value();
          ++accumulationCounts[cell->getNodeIndex(nodeIndex)];
        } else {
          ++invalidValueCounts[cell->getNodeIndex(nodeIndex)];
        }
      }
      if (i % 100 == 0)
        progress.Step(100);
    }

    for (std::size_t i = 0; i < geometrySize; ++i) {
      if (accumulationCounts[i] != 0) {
        values->set(i, accumulatedValues[i] / accumulationCounts[i]);
      } else if (invalidValueCounts[i] != 0) {
        // apparently all contributing cell nodes had invalid values, so set the node value to undefined
        values->set(i, DBL_UNDEFINED);
      }
    }

    return values;

  } else {
    const geo::IElementSet *pElementSet =
        numTopologyCells > 0 ? m_topology.getCell(0)->getParent()->IndexingElementSet() : 0;

    typedef CTaskCollectGeometryValuesByTopology<TopologyType, CellType> CTaskCollect;

    std::vector<tbb::spin_mutex> locks(geometrySize);

    CTaskCollect taskCollect(m_topology, pValueComponent, unit, accumulatedValues, accumulationCounts,
                             invalidValueCounts, locks);

    mp::CKernelDispatcher kernelDispatcher;

    if (vm->PrepareMapping(pElementSet, pValueComponent) && vm->AllowParallelMapping()) {
      mp::CKernelParallel<CTaskCollect> parKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelParallel, CTaskCollect)(parKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    } else {
      taskCollect.setSequential();
      mp::CKernelSequential<CTaskCollect> seqKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelSequential, CTaskCollect)(seqKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    }

    vm->FinishMapping();

    if (kernelDispatcher.Canceled())
      throw new CProgressCancel();

    for (std::size_t i = 0; i < geometrySize; ++i) {
      if (accumulationCounts[i] != 0) {
        values->set(i, accumulatedValues[i] / accumulationCounts[i]);
      } else if (invalidValueCounts[i] != 0) {
        // apparently all contributing cell nodes had invalid values, so set the node value to undefined
        values->set(i, DBL_UNDEFINED);
      }
    }

    return values;
  }
}

template <class TopologyType, class CellType, class MiMeshUnstructuredType>
OIDIVectorSetI *OIDIMeshTemplate<TopologyType, CellType, MiMeshUnstructuredType>::collectVectorsByTopology(
    const IVectorResult::IVectorComponent *pVectorComponent, IQuantityDouble::UNIT unit,
    IProgressBase &progress) const {
  std::size_t numTopologyCells = m_topology.getNumCells();

  OIDIVectorSetI *vectors = new OIDIVectorSetI(numTopologyCells, MiDataSet::PER_CELL);

  assert(dynamic_cast<const IVectorResult *>(&pVectorComponent->Parent()));
  const IVectorResult *pResult = static_cast<const IVectorResult *>(&pVectorComponent->Parent());

  std::vector<geo::CVector> results;
  for (std::size_t i = 0; i < numTopologyCells; ++i) {
    pVectorComponent->MapValueElement(*m_topology.getCell(i)->getParent(), results, CValueType::MT_NONE,
                                      unit); // no weighting for now
    MbVec3d v = computeAverage(results);

    vectors->add(v);
    if (i % 100 == 0)
      progress.Step(100);
  }

  return vectors;
}

/////////////////////////////////////////////////////////////////////
// CollectGeometryVectorsByTopology
/////////////////////////////////////////////////////////////////////

template <class TopologyType, class CellType> class CTaskCollectGeometryVectorsByTopology {
  const TopologyType *m_topology;

  const IVectorResult::IVectorComponent *m_pVectorComponent;
  IQuantityDouble::UNIT m_unit;

  std::vector<geo::CVector> &m_accumulatedVectors;
  std::vector<size_t> &m_accumulationCounts;
  std::vector<tbb::spin_mutex> &m_locks;

  mp::CKernelDispatcher *m_dispatcher;

  bool m_bSequential;

public:
  CTaskCollectGeometryVectorsByTopology(const TopologyType &t, const IVectorResult::IVectorComponent *pVectorComponent,
                                        IQuantityDouble::UNIT unit, std::vector<geo::CVector> &accV,
                                        std::vector<size_t> &accC, std::vector<tbb::spin_mutex> &locks)
      : m_topology(&t), m_pVectorComponent(pVectorComponent), m_unit(unit), m_accumulatedVectors(accV),
        m_accumulationCounts(accC), m_locks(locks), m_dispatcher(0), m_bSequential(false) {}

  // topology is not a vector format, but since we don't use data(), we just return a dummy vector
  MP_DUMMY_DATA

  size_t size() const { return m_topology->getNumCells(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void setSequential() { m_bSequential = true; }

  void operator()(const tbb::blocked_range<size_t> &r) const {
#ifdef DEBUG_MPKERNEL
    mp::MPOutput::instance()->write(QString("START range from %1 to %2\n").arg(r.begin()).arg(r.end()).toStdString());
#endif
    std::vector<geo::CVector> results;

    geo::IParallelInitializationCallback *callback =
        !m_bSequential ? const_cast<IValueComposite &>(m_pVectorComponent->Parent()).GetParallelInitializationCallback()
                       : 0;

    for (size_t i = r.begin(); i != r.end(); ++i) {
      const CellType *cell = m_topology->getCell(i);
      // const IValueDataInterfaceVector::TValueVec elementVectors =
      // m_pVectorComponent->VectorData().ValueElement(*cell->getParent(), m_unit);
      m_pVectorComponent->MapValueElementCB(*cell->getParent(), results, CValueType::MT_NONE, m_unit,
                                            callback); // no weighting for now

      size_t numCellNodes = cell->getNumNodes();
      for (size_t nodeIndex = 0; nodeIndex < numCellNodes; ++nodeIndex) {
        size_t cellNodeIndex = cell->getNodeIndex(nodeIndex);

        tbb::spin_mutex::scoped_lock lock(m_locks[cellNodeIndex]);

        m_accumulatedVectors[cellNodeIndex] += results[nodeIndex];
        ++m_accumulationCounts[cellNodeIndex];
      }

      if ((i % 100) == 0 && m_dispatcher && m_bSequential)
        m_dispatcher->Step(100);
    }

    delete callback;

    if (m_dispatcher)
      m_dispatcher->Step(m_bSequential ? r.size() % 100 : r.size());
#ifdef DEBUG_MPKERNEL
    mp::MPOutput::instance()->write(
        QString("END range from %1 to %2, size is %3\n").arg(r.begin()).arg(r.end()).arg(r.size()).toStdString());
#endif
  }
};

template <class TopologyType, class CellType, class MiMeshUnstructuredType>
OIDIVectorSetI *OIDIMeshTemplate<TopologyType, CellType, MiMeshUnstructuredType>::collectGeometryVectorsByTopology(
    const IVectorResult::IVectorComponent *pVectorComponent, IQuantityDouble::UNIT unit,
    IProgressBase &progress) const {
  std::size_t numTopologyCells = m_topology.getNumCells();
  std::size_t geometrySize = getSize();

  OIDIVectorSetI *vectors = new OIDIVectorSetI(0, MiDataSet::PER_NODE);

  std::vector<geo::CVector> accumulatedVectors(geometrySize, geo::CVector(0.0, 0.0, 0.0));
  std::vector<size_t> accumulationCounts(geometrySize, 0);

  std::vector<geo::CVector> results;

  CValueMapper *vm = CValueMapper::instance();

  if (vm->UseLegacyCode()) {
    for (std::size_t i = 0; i < numTopologyCells; ++i) {
      const CellType *cell = m_topology.getCell(i);
      pVectorComponent->MapValueElement(*cell->getParent(), results, CValueType::MT_NONE, unit); // no weighting for now

      size_t numCellNodes = cell->getNumNodes();
      for (size_t nodeIndex = 0; nodeIndex < numCellNodes; ++nodeIndex) {
        accumulatedVectors[cell->getNodeIndex(nodeIndex)] += results[nodeIndex];
        ++accumulationCounts[cell->getNodeIndex(nodeIndex)];
      }

      if (i % 100 == 0)
        progress.Step(100);
    }
  } else {
    const geo::IElementSet *pElementSet =
        numTopologyCells > 0 ? m_topology.getCell(0)->getParent()->IndexingElementSet() : 0;

    typedef CTaskCollectGeometryVectorsByTopology<TopologyType, CellType> CTaskCollect;

    std::vector<tbb::spin_mutex> locks(geometrySize);

    CTaskCollect taskCollect(m_topology, pVectorComponent, unit, accumulatedVectors, accumulationCounts, locks);

    mp::CKernelDispatcher kernelDispatcher;

    if (vm->PrepareMapping(pElementSet, pVectorComponent) && vm->AllowParallelMapping()) {
      mp::CKernelParallel<CTaskCollect> parKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelParallel, CTaskCollect)(parKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    } else {
      taskCollect.setSequential();
      mp::CKernelSequential<CTaskCollect> seqKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelSequential, CTaskCollect)(seqKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    }

    vm->FinishMapping();

    if (kernelDispatcher.Canceled())
      throw new CProgressCancel();
  }

  for (std::size_t i = 0; i < geometrySize; ++i) {
    if (accumulationCounts[i] != 0) {
      geo::CVector average(accumulatedVectors[i] / accumulationCounts[i]);
      vectors->set(i, MbVec3d(average[0], average[1], average[2]));
    }
  }

  return vectors;
}

template <class TopologyType, class CellType> class CTaskCollectTensorsByTopology {
  const TopologyType *m_topology;

  const IValueComponentBase *m_pTensorComponent;
  IQuantityDouble::UNIT m_unit;

  CTensorCollector &m_collector;

  mp::CKernelDispatcher *m_dispatcher;

  bool m_bSequential;

public:
  CTaskCollectTensorsByTopology(const TopologyType &t, const IValueComponentBase *pTensorComponent,
                                IQuantityDouble::UNIT unit, CTensorCollector &collector)
      : m_topology(&t), m_pTensorComponent(pTensorComponent), m_unit(unit), m_collector(collector), m_dispatcher(0),
        m_bSequential(false) {}

  // topology is not a vector format, but since we don't use data(), we just return a dummy vector
  MP_DUMMY_DATA

  size_t size() const { return m_topology->getNumCells(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void setSequential() { m_bSequential = true; }

  void operator()(const tbb::blocked_range<size_t> &r) const {
#ifdef DEBUG_MPKERNEL
    mp::MPOutput::instance()->write(QString("START range from %1 to %2\n").arg(r.begin()).arg(r.end()).toStdString());
#endif
    std::vector<geo::CVector> results;

    geo::IParallelInitializationCallback *callback = 0;

    if (!m_bSequential && m_pTensorComponent->NeedParallelInitializationCallback()) {
      callback = const_cast<IValueComponentBase *>(m_pTensorComponent)->GetParallelInitializationCallback();
      if (!callback)
        callback = const_cast<IValueComposite &>(m_pTensorComponent->Parent()).GetParallelInitializationCallback();
      assert(callback);
    }

    size_t offset = r.begin();

    CTensorCollector::record_type *record = new CTensorCollector::record_type(offset, r.size());
    CTensorCollector::value_vector_type &values = record->getValues();

    for (size_t i = r.begin(); i != r.end(); ++i) {
      CTensor v(0); // we compute average in situ as we have to loop over nodes ourselves, instead of getting a vector

      std::size_t processed = 0;

      IValueDomainTensor::TValueVec tensors =
          m_pTensorComponent->TensorData()->ValueElement(*m_topology->getCell(i)->getParent(), m_unit, callback);

      for (int j = 0; j < m_topology->getCell(i)->getParent()->NrOfNodes(); ++j) {
        CTensor t = tensors[j];
        if (!t.Empty()) {
          v += t;
          ++processed;
        }
      }

      if (processed > 0)
        v /= processed;

      // ScaleTensor(v, unit, pTensorGroup);
      //  TODO: move this stuff to the tensor group code

      values[i - offset] = v;

      if ((i % 100) == 0 && m_dispatcher && m_bSequential)
        m_dispatcher->Step(100);
    }

    delete callback;

    if (m_dispatcher)
      m_dispatcher->Step(m_bSequential ? r.size() % 100 : r.size());

    m_collector.add(record);
#ifdef DEBUG_MPKERNEL
    mp::MPOutput::instance()->write(
        QString("END range from %1 to %2, size is %3\n").arg(r.begin()).arg(r.end()).arg(r.size()).toStdString());
#endif
  }
};

template <class TopologyType, class CellType, class MiMeshUnstructuredType>
OIDITensorSetI *OIDIMeshTemplate<TopologyType, CellType, MiMeshUnstructuredType>::collectTensorsByTopology(
    const IValueComponentBase *pTensorComponent, IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  std::size_t numTopologyCells = m_topology.getNumCells();

  OIDITensorSetI *tensorSet = new OIDITensorSetI(numTopologyCells, MiDataSet::PER_CELL);

  CValueMapper *vm = CValueMapper::instance();

  const IValueDataInterfaceTensor *pTensorInterface = pTensorComponent->TensorData();

  if (vm->UseLegacyCode()) {
    for (std::size_t i = 0; i < numTopologyCells; ++i) {
      CTensor v(0); // we compute average in situ as we have to loop over nodes ourselves, instead of getting a vector

      std::size_t processed = 0;

      IValueDomainTensor::TValueVec tensors = pTensorInterface->ValueElement(*m_topology.getCell(i)->getParent(), unit);

      for (int j = 0; j < m_topology.getCell(i)->getParent()->NrOfNodes(); ++j) {
        CTensor t = tensors[j];
        if (!t.Empty()) {
          v += t;
          ++processed;
        }
      }

      if (processed > 0)
        v /= processed;

      // ScaleTensor(v, unit, pTensorGroup);
      //  TODO: move this stuff to the tensor group code

      tensorSet->add(v);

      if (i % 100 == 0)
        progress.Step(100);
    }
  } else {
    const geo::IElementSet *pElementSet =
        numTopologyCells > 0 ? m_topology.getCell(0)->getParent()->IndexingElementSet() : 0;

    typedef CTaskCollectTensorsByTopology<TopologyType, CellType> CTaskCollect;

    CTensorCollector collector;

    CTaskCollect taskCollect(m_topology, pTensorComponent, unit, collector);

    mp::CKernelDispatcher kernelDispatcher;

    if (vm->PrepareMapping(pElementSet, pTensorComponent) && vm->AllowParallelMapping()) {
      mp::CKernelParallel<CTaskCollect> parKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelParallel, CTaskCollect)(parKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    } else {
      taskCollect.setSequential();
      mp::CKernelSequential<CTaskCollect> seqKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelSequential, CTaskCollect)(seqKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    }

    vm->FinishMapping();

    if (kernelDispatcher.Canceled())
      throw new CProgressCancel();

    collector.collect(*tensorSet);
  }

  return tensorSet;
}

template <class TopologyType, class CellType> class CTaskCollectGeometryTensorsByTopology {
  const TopologyType *m_topology;

  const IValueComponentBase *m_pTensorComponent;
  IQuantityDouble::UNIT m_unit;

  std::vector<CTensor> &m_accumulatedTensors;
  std::vector<size_t> &m_accumulationCounts;
  std::vector<tbb::spin_mutex> &m_locks;

  mp::CKernelDispatcher *m_dispatcher;

  bool m_bSequential;

public:
  CTaskCollectGeometryTensorsByTopology(const TopologyType &t, const IValueComponentBase *pTensorComponent,
                                        IQuantityDouble::UNIT unit, std::vector<CTensor> &accumulatedTensors,
                                        std::vector<size_t> &accumulationCounts, std::vector<tbb::spin_mutex> &locks)
      : m_topology(&t), m_pTensorComponent(pTensorComponent), m_unit(unit), m_accumulatedTensors(accumulatedTensors),
        m_accumulationCounts(accumulationCounts), m_locks(locks), m_dispatcher(0), m_bSequential(false) {}

  // topology is not a vector format, but since we don't use data(), we just return a dummy vector
  MP_DUMMY_DATA

  size_t size() const { return m_topology->getNumCells(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void setSequential() { m_bSequential = true; }

  void operator()(const tbb::blocked_range<size_t> &r) const {
#ifdef DEBUG_MPKERNEL
    mp::MPOutput::instance()->write(QString("START range from %1 to %2\n").arg(r.begin()).arg(r.end()).toStdString());
#endif
    std::vector<geo::CVector> results;

    geo::IParallelInitializationCallback *callback = 0;

    if (!m_bSequential && m_pTensorComponent->NeedParallelInitializationCallback()) {
      callback = const_cast<IValueComponentBase *>(m_pTensorComponent)->GetParallelInitializationCallback();
      if (!callback)
        callback = const_cast<IValueComposite &>(m_pTensorComponent->Parent()).GetParallelInitializationCallback();
      assert(callback);
    }

    for (size_t i = r.begin(); i != r.end(); ++i) {
      const CellType *cell = m_topology->getCell(i);

      IValueDomainTensor::TValueVec tensors =
          m_pTensorComponent->TensorData()->ValueElement(*cell->getParent(), m_unit, callback);

      size_t numCellNodes = cell->getNumNodes();
      for (size_t nodeIndex = 0; nodeIndex < numCellNodes; ++nodeIndex) {
        if (nodeIndex >= tensors.size())
          continue;

        CTensor t = tensors[nodeIndex];
        if (!t.Empty()) {
          size_t cellNodeIndex = cell->getNodeIndex(nodeIndex);

          tbb::spin_mutex::scoped_lock lock(m_locks[cellNodeIndex]);

          m_accumulatedTensors[cellNodeIndex] += t;
          ++m_accumulationCounts[cellNodeIndex];
        }
      }

      if ((i % 100) == 0 && m_dispatcher && m_bSequential)
        m_dispatcher->Step(100);
    }

    delete callback;

    if (m_dispatcher)
      m_dispatcher->Step(m_bSequential ? r.size() % 100 : r.size());
#ifdef DEBUG_MPKERNEL
    mp::MPOutput::instance()->write(
        QString("END range from %1 to %2, size is %3\n").arg(r.begin()).arg(r.end()).arg(r.size()).toStdString());
#endif
  }
};

template <class TopologyType, class CellType, class MiMeshUnstructuredType>
OIDITensorSetI *OIDIMeshTemplate<TopologyType, CellType, MiMeshUnstructuredType>::collectGeometryTensorsByTopology(
    const IValueComponentBase *pTensorComponent, IQuantityDouble::UNIT unit, IProgressBase &progress) const {
  std::size_t numTopologyCells = m_topology.getNumCells();
  std::size_t geometrySize = getSize();

  OIDITensorSetI *tensorSet = new OIDITensorSetI(0, MiDataSet::PER_NODE);

  std::vector<CTensor> accumulatedTensors(geometrySize, CTensor(0.0));
  std::vector<size_t> accumulationCounts(geometrySize, 0);

  const IValueDataInterfaceTensor *tensorInterface = pTensorComponent->TensorData();

  CValueMapper *vm = CValueMapper::instance();

  if (vm->UseLegacyCode()) {

    for (std::size_t i = 0; i < numTopologyCells; ++i) {
      const CellType *cell = m_topology.getCell(i);

      IValueDomainTensor::TValueVec tensors = tensorInterface->ValueElement(*cell->getParent(), unit);

      size_t numCellNodes = cell->getNumNodes();
      for (size_t nodeIndex = 0; nodeIndex < numCellNodes; ++nodeIndex) {
        if (nodeIndex >= tensors.size())
          continue;

        CTensor t = tensors[nodeIndex];
        if (!t.Empty()) {
          accumulatedTensors[cell->getNodeIndex(nodeIndex)] += t;
          ++accumulationCounts[cell->getNodeIndex(nodeIndex)];
        }
      }
      if (i % 100 == 0)
        progress.Step(100);
    }
  } else {
    const geo::IElementSet *pElementSet =
        numTopologyCells > 0 ? m_topology.getCell(0)->getParent()->IndexingElementSet() : 0;

    typedef CTaskCollectGeometryTensorsByTopology<TopologyType, CellType> CTaskCollect;

    std::vector<tbb::spin_mutex> locks(geometrySize);

    CTaskCollect taskCollect(m_topology, pTensorComponent, unit, accumulatedTensors, accumulationCounts, locks);

    mp::CKernelDispatcher kernelDispatcher;

    if (vm->PrepareMapping(pElementSet, pTensorComponent) && vm->AllowParallelMapping()) {
      mp::CKernelParallel<CTaskCollect> parKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelParallel, CTaskCollect)(parKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    } else {
      taskCollect.setSequential();
      mp::CKernelSequential<CTaskCollect> seqKernelCollect;
      mp::IDispatchedTask *disTaskCollect =
          NEW_DISPATCH_TASK(mp::CKernelSequential, CTaskCollect)(seqKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    }

    vm->FinishMapping();

    if (kernelDispatcher.Canceled())
      throw new CProgressCancel();
  }

  for (std::size_t i = 0; i < geometrySize; ++i) {
    if (accumulationCounts[i] != 0) {
      CTensor average(accumulatedTensors[i] / accumulationCounts[i]);
      // ScaleTensor(average, unit, pTensorGroup);
      tensorSet->set(i, average);
    }
  }

  return tensorSet;
}

bool OIDIVolumeCell::hasSameNodeIndices(const OIDIVolumeCell *cell) const {
  // should also check the facets ...
  for (int n = 0; n < getNumNodes(); ++n) {
    if (!cell->findNodeIndex(getNodeIndex(n)))
      return false;
  }
  return (getNumNodes() == cell->getNumNodes());
}

bool OIDIVolumeCell::findNodeIndex(size_t nodeIndex) const {
  for (int n = 0; n < getNumNodes(); ++n) {
    if (getNodeIndex(n) == nodeIndex)
      return true;
  }
  return false;
}

geo::CPtrArray<geo::IPoint> *CreateArray(const geo::IPoint &Point) {
  geo::CPtrArray<geo::IPoint> *array = new geo::CPtrArray<geo::IPoint>;

  // we used to need to span a surface here, drawing the point with as a triangle with some minor offsets which created
  // ugly artifacts it seems now (OIV 9.5.1) we can just use the same coordinates, creating an actual point if we push
  // less than 3 points, OIV doesn't render it -- perhaps in a future version? :/ (two points seems to crash OIV)
  array->PushBack(*new geo::CPoint(Point.X(), Point.Y(), Point.Z()));
  array->PushBack(*new geo::CPoint(Point.X(), Point.Y(), Point.Z()));
  array->PushBack(*new geo::CPoint(Point.X(), Point.Y(), Point.Z()));

  return array;
}
