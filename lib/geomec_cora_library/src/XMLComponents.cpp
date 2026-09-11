#include "XMLComponents.h"
#include "GetFaultInfo.h"
#include "GetFormationInfo.h"
#include "GetHorizonInfo.h"
#include "GetNonMeshedSurfaceInfo.h"
#include "GetSurfaceInfo.h"
#include "GetWellPathInfo.h"
#include "ModelData.h"

namespace cora {

CXMLComponents::CXMLComponents(CModelData &modelData) : m_components(createComponents(modelData)) {}

namespace {

const QString COMPONENTS_OPEN = "<Components>";
const QString COMPONENTS_CLOSE = "</Components>";
const QString COMPONENTS_EMPTY = "<Components/>";

} // anonymous namespace

std::ostream &CXMLComponents::operator()(std::ostream &stream) const {
  if (m_components.empty()) {
    stream << COMPONENTS_EMPTY.toStdString() << std::endl;
  } else {
    stream << COMPONENTS_OPEN.toStdString() << std::endl;

    stream << m_components;

    stream << COMPONENTS_CLOSE.toStdString() << std::endl;
  }

  return stream;
}

// private

namespace {

struct CAdd2Components {
  CAdd2Components(TXMLComponents &components, CModelData &modelData)
      : m_components(components), m_modelData(modelData) {}

  void operator()(const TObject &object) const {
    m_components.push_back(TXMLComponent(new CXMLComponent(m_modelData, object)));
  }

  TXMLComponents &m_components;
  CModelData &m_modelData;
};

} // anonymous namespace

// static

TXMLComponents CXMLComponents::createComponents(CModelData &modelData) {
  TXMLComponents components;
  CAdd2Components add2Components(components, modelData);
  TObjects objects;

  CGetFormationInfo &formationInfo(CGetFormationInfo::instance(modelData()));
  CGetFaultInfo &faultInfo(CGetFaultInfo::instance(modelData()));
  CGetWellPathInfo &wellPathInfo(CGetWellPathInfo::instance(modelData()));
  CGetHorizonInfo &horizonInfo(CGetHorizonInfo::instance(modelData()));
  CGetSurfaceInfo &surfaceInfo(CGetSurfaceInfo::instance(modelData()));
  CGetNonMeshedSurfaceInfo &getNonMeshedSurfaceInfo(CGetNonMeshedSurfaceInfo::instance(modelData()));

  objects = formationInfo.getObjects();
  std::for_each(objects.begin(), objects.end(), add2Components);
  objects = faultInfo.getObjects();
  std::for_each(objects.begin(), objects.end(), add2Components);
  objects = wellPathInfo.getObjects();
  std::for_each(objects.begin(), objects.end(), add2Components);
  objects = horizonInfo.getObjects();
  std::for_each(objects.begin(), objects.end(), add2Components);
  objects = surfaceInfo.getObjects();
  std::for_each(objects.begin(), objects.end(), add2Components);
  objects = getNonMeshedSurfaceInfo.getObjects();
  std::for_each(objects.begin(), objects.end(), add2Components);

  return components;
}

} // namespace cora

// global

std::ostream &operator<<(std::ostream &stream, const cora::CXMLComponents &rhs) { return rhs(stream); }
