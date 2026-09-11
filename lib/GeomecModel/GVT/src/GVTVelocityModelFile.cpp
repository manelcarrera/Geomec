#include <stdexcept>

#include "GVTVelocityModelFile.h"
#include "GVTVelocityModelFromFile.h"
#include "GVTVelocityModelFromMaterial.h"
#include "StreamVersion.h"

namespace GVT {

CGVTVelocityModelFile::CGVTVelocityModelFile(const TVelocityFromFile &, CModelBase &modelBase,
                                             const QString &velocityModelFile)
    : m_GVTVelocityModelSource(new CGVTVelocityModelFromFile(velocityModelFile)), m_modelBase(modelBase) {}

CGVTVelocityModelFile::CGVTVelocityModelFile(const TVelocityFromMaterial &, CModelBase &modelBase)
    : m_modelBase(modelBase) {
  tryVelocityFromMaterial();
}

CGVTVelocityModelFile::~CGVTVelocityModelFile() {}

void CGVTVelocityModelFile::tryVelocityFromMaterial() {
  // we need CGVTVelocityModelFile to be always valid, so we catch here
  try {
    m_GVTVelocityModelSource =
        QSharedPointer<CGVTVelocityModelFromMaterial>(new CGVTVelocityModelFromMaterial(m_modelBase));
  } catch (std::exception &) {
    m_GVTVelocityModelSource = QSharedPointer<CGVTVelocityModelFromFile>(new CGVTVelocityModelFromFile());
  }
}

CGVTVelocityModelFile::CGVTVelocityModelFile(const CGVTVelocityModelFile &rhs)
    : m_GVTVelocityModelSource(rhs.m_GVTVelocityModelSource), m_modelBase(rhs.m_modelBase) {}

CGVTVelocityModelFile &CGVTVelocityModelFile::operator=(const CGVTVelocityModelFile &rhs) {
  CGVTVelocityModelFile gvtVelocityModelFile(rhs);

  gvtVelocityModelFile.swap(*this);

  return *this;
}

void CGVTVelocityModelFile::SetContents(const std::stringstream &buffer) {
  m_GVTVelocityModelSource->SetContents(buffer);
}

const QString &CGVTVelocityModelFile::velocityModelFile() const {
  return m_GVTVelocityModelSource->velocityModelFile();
}

void CGVTVelocityModelFile::velocityModelFile(const QString &velocityModelFile) {
  m_GVTVelocityModelSource->velocityModelFile(velocityModelFile);
}

bool CGVTVelocityModelFile::isVelocityModelFileReady() const {
  return m_GVTVelocityModelSource->isVelocityModelFileReady();
}

int CGVTVelocityModelFile::velocityModelSourceNumerator() const {
  return m_GVTVelocityModelSource->velocityModelSourceNumerator();
}

void CGVTVelocityModelFile::velocityModelSourceNumerator(int velocityModelSourceNumerator) {
  if (velocityModelSourceNumerator != CGVTVelocityModelFile::velocityModelSourceNumerator()) {
    switch (velocityModelSourceNumerator) {
    case VELOCITY_FROM_MATERIAL:
      tryVelocityFromMaterial();
      break;
    case VELOCITY_FROM_FILE:
      /* Fall-through */
    default:
      m_GVTVelocityModelSource = QSharedPointer<CGVTVelocityModelSource>(new CGVTVelocityModelFromFile());
      break;
    }
  }
}

void CGVTVelocityModelFile::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  if (version < CStreamVersion(4, 1, 31)) {
    m_GVTVelocityModelSource = QSharedPointer<CGVTVelocityModelSource>(new CGVTVelocityModelFromFile());
  } else // (version >= CStreamVersion(4, 1, 31))
  {
    int velocitySourceType = 0;

    stream >> velocitySourceType;

    switch (velocitySourceType) {
    case VELOCITY_FROM_MATERIAL:
      tryVelocityFromMaterial();
      break;
    case VELOCITY_FROM_FILE:
      /* Fall-through */
    default: // this shouldn't happen, but if it does, it shouldn't break loading
      m_GVTVelocityModelSource = QSharedPointer<CGVTVelocityModelSource>(new CGVTVelocityModelFromFile());
    }
  }

  m_GVTVelocityModelSource->LoadStream(stream, version, progress);
}

void CGVTVelocityModelFile::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  m_GVTVelocityModelSource->SaveStream(stream, progress);
}

// static

CGVTVelocityModelFile::TVelocityFromFile CGVTVelocityModelFile::velocityFromFile;
CGVTVelocityModelFile::TVelocityFromMaterial CGVTVelocityModelFile::velocityFromMaterial;

// private

void CGVTVelocityModelFile::swap(CGVTVelocityModelFile &gvtVelocityModelFile) {
  QSharedPointer<CGVTVelocityModelSource> temporaryGVTVelocityModelSource =
      gvtVelocityModelFile.m_GVTVelocityModelSource;

  // m_modelBase is not swapped since it currently does not have an assignment
  // operator (and both models should be the same, hence the assert - and sadly
  // no instance based equality operator is defined!)

  assert(&m_modelBase == &(gvtVelocityModelFile.m_modelBase));

  gvtVelocityModelFile.m_GVTVelocityModelSource = m_GVTVelocityModelSource;

  m_GVTVelocityModelSource = temporaryGVTVelocityModelSource;
}

} // namespace GVT
