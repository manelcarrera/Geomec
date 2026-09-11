
#include <memory>

#ifndef WIN32
#include <spawn.h>
#endif // WIN32

#include "BodyGroup.h"
#include "CoordinateSet.h"
#include "DepletionStage.h"
#include "FormationBase.h"
#include "GeomecUtils.h"
#include "Global.h"
#include "IProgressBase.h"
#include "IProgressFactory.h"
#include "MeshBase.h"
#include "ModelBase.h"
#include "PointSet.h"
#include "Pressure.h"
#include "QBSettings.h"
#include "Temperature.h"

CQBSettings::CQBSettings(CModelBase &model) : CStorageNode(model), m_fpLog(0), m_Model(model), m_QBPointSet(0) {
  TFormationBaseEntry &entry = (TFormationBaseEntry &)*Model().GraphEntry(MD_BASE_FORMATION);
  LinkTo(entry);
}

CQBSettings::~CQBSettings() {
  if (m_fpLog)
    fclose(m_fpLog);
}

void CQBSettings::StartQB() {
  QString appPath = GetAppPath();
  // appPath = appPath.left(appPath.indexOf("\\Geomec.exe"));
  appPath = appPath + "\\QuickBlocks\\QuickBlocks.exe";

#ifdef WIN32
  _spawnl(_P_NOWAIT, appPath.toStdString().c_str(), " ", NULL);
#else  // !WIN32
  pid_t pid;
  char *const argv[2] = {(char *)" ", NULL};

  posix_spawn(&pid, appPath.toStdString().c_str(), NULL, NULL, argv, NULL);
#endif // WIN32
}
QString CQBSettings::QuickBlocksLogPath() {
  QString logPath = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL);
  logPath.append("\\QuickBlocks.log");
  return logPath;
}

void CQBSettings::WriteDat(const QString &path) {
  m_Intersections.clear();
  std::auto_ptr<IProgressBase> progress(_g->prog()->create(eProgress::Dual, "QuickBlocks Analysis", false, 1));
  CQBInputWriter qbIW(*this);

  GeneratePointSet(*progress);
  assert(m_QBPointSet);

  progress->AddSteps(m_QBPointSet->PointSize() + qbIW.StepSize());

  AddDepths(*progress);
  if (!qbIW.WriteInput(path.toStdString().c_str(), *progress)) {
    _m()->msg(qbIW.Error());
  } else if (m_fpLog) {
    QString message;
    message = QString("Multiple intersections found for some of the sample points, see %1 for more details")
                  .arg(QuickBlocksLogPath());
    _m()->msg(message);
  }
  if (m_fpLog) {
    fclose(m_fpLog);
    m_fpLog = 0;
  }
  delete m_QBPointSet;
  m_QBPointSet = 0;
}

void CQBSettings::GeneratePointSet(IProgressBase &progress) {
  if (m_QBPointSet)
    delete m_QBPointSet;

  m_QBPointSet = new CPointSet("QB", m_Model, 0, CPointSet::DIM_2D);
  progress.StatusMessage("Constructing grid");
  m_Grid.GenerateGrid(*m_QBPointSet);
}

void CQBSettings::AddDepths(IProgressBase &progress) {
  geo::CPoint mesh_max = m_Model.Mesh().Max();
  geo::CPoint mesh_min = m_Model.Mesh().Min();
  geo::CPoint intersect(0, 0, mesh_max.Z());

  geo::CCoordinateSet<geo::CPoint> filteredPoints;
  std::set<geo::CPoint> intersectPoints;

  progress.StatusMessage("Determining analysis depths");
  for (int i = 0; i < m_QBPointSet->PointSize(); ++i) {
    TFormationIntersects formIntersections;

    std::set<const CFormationBase *>::const_iterator it;
    for (it = m_stSelectedFormations.begin(); it != m_stSelectedFormations.end(); ++it) {
      intersectPoints.clear();
      filteredPoints.clear();

      geo::CPoint pt = m_QBPointSet->PointAt(i);
      pt.Z(mesh_min.Z());
      intersect.X(pt.X());
      intersect.Y(pt.Y());
      geo::CLine line(pt, intersect);

      // calculate intersection points with top and bottom of the reservoir
      for (int j = 0; j < (*it)->ElementSetSize(); j++) {
        const geo::CBodyGroup *pGeoVolume = dynamic_cast<const geo::CBodyGroup *>(&(*it)->ElementSet(j).ElementSet());
        assert(pGeoVolume);

        // get the intersection points with the volume and the vertical line
        pGeoVolume->IntersectionWithEdgeFaces(line, intersectPoints);
      }

      geo::CPoint top;
      geo::CPoint bot;

      // filter the points, maybe it's just a rounding error
      std::set<geo::CPoint>::iterator ptIt;
      for (ptIt = intersectPoints.begin(); ptIt != intersectPoints.end(); ++ptIt) {
        filteredPoints.insert(*ptIt);
      }

      if (filteredPoints.size() > 1) {
        if (filteredPoints.size() > 2) {

          // still more than two points? write logline
          if (filteredPoints.size() > 2) {
            if (!m_fpLog) {
              m_fpLog = fopen(QuickBlocksLogPath().toStdString().c_str(), "w");
              fprintf(m_fpLog, "********** QuickBlocks Logfile **********\n");
            }
            assert(m_fpLog);
            fprintf(m_fpLog, "Multiple intersections found for formation \"%s\" at Northing: %f; Easting: %f; Depths: ",
                    (*it)->Name().toStdString().c_str(), intersect.Y(), intersect.X());
            geo::CCoordinateSet<geo::CPoint>::iterator ptFilIt;
            for (ptFilIt = filteredPoints.begin(); ptFilIt != filteredPoints.end(); ++ptFilIt) {
              fprintf(m_fpLog, "%f; ", (*ptFilIt).Z());
            }
            fprintf(m_fpLog, "\n");
          }
        }

        geo::CCoordinateSet<geo::CPoint>::iterator filtPtsIt;
        for (filtPtsIt = filteredPoints.begin(); filtPtsIt != filteredPoints.end(); ++filtPtsIt) {
          if (filtPtsIt == filteredPoints.begin()) {
            top = *filtPtsIt;
            bot = *filtPtsIt;
            continue;
          }

          if ((*filtPtsIt).Z() < top.Z())
            top = *filtPtsIt;
          if ((*filtPtsIt).Z() > bot.Z())
            bot = *filtPtsIt;
        }

      } else {
        top = geo::CPoint::NullPoint;
        bot = geo::CPoint::NullPoint;
      }
      formIntersections.push_back(new CQBFormationIntersection(*it, top, bot));
    }
    m_Intersections.insert(TGrid2Intersects::value_type(i, formIntersections));
    progress.Step();
  }
}

const std::vector<std::pair<CDepletionStage *, CDepletionStage *>> &CQBSettings::DeltaPressures() {
  return m_DeltaPressures;
}

const std::vector<std::pair<CDepletionStage *, CDepletionStage *>> &CQBSettings::DeltaTemperatures() {
  return m_DeltaTemperatures;
}

void CQBSettings::AddDeltaPressure(CDepletionStage *stage1, CDepletionStage *stage2) {
  m_DeltaPressures.push_back(std::pair<CDepletionStage *, CDepletionStage *>(stage1, stage2));
}

void CQBSettings::RemoveDeltaPressure(int nIndex) {
  assert(nIndex >= 0 && nIndex < m_DeltaPressures.size());
  m_DeltaPressures.erase(m_DeltaPressures.begin() + nIndex);
}

void CQBSettings::AddDeltaTemperature(CDepletionStage *stage1, CDepletionStage *stage2) {
  m_DeltaTemperatures.push_back(std::pair<CDepletionStage *, CDepletionStage *>(stage1, stage2));
}

void CQBSettings::RemoveDeltaTemperature(int nIndex) {
  assert(nIndex >= 0 && nIndex < m_DeltaTemperatures.size());
  m_DeltaTemperatures.erase(m_DeltaTemperatures.begin() + nIndex);
}

void CQBSettings::SetTimeLapse(int nIndex, CDepletionStage *time1, CDepletionStage *time2, TTimeLapseType TLT) {
  if (TLT == Pressure) {
    assert(nIndex >= 0 && nIndex < m_DeltaPressures.size());
    m_DeltaPressures[nIndex].first = time1;
    m_DeltaPressures[nIndex].second = time2;
  } else if (TLT == Temperature) {
    assert(nIndex >= 0 && nIndex < m_DeltaTemperatures.size());
    m_DeltaTemperatures[nIndex].first = time1;
    m_DeltaTemperatures[nIndex].second = time2;
  } else {
    assert(false);
  }
}

QString CQBSettings::ListObjectName() const { return "dP"; }

void CQBSettings::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  if (uHint == NEW_FORMATION_ATTACHED) {
    TFormationBaseEntry &entry = (TFormationBaseEntry &)*Model().GraphEntry(MD_BASE_FORMATION);
    TFormationBaseEntry::TNodeSet stNodes = entry.EntryNodes();
    for (TFormationBaseEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
      if (!IsLinkedTo(**it))
        LinkTo(**it);
    }
    Modified(uHint);
  }
}

void CQBSettings::OnNeighbourDeleted(const CGraphNode &node) {
  const CFormationBase *pFormation = reinterpret_cast<const CFormationBase *>(&node);
  std::set<const CFormationBase *>::iterator it = m_stSelectedFormations.find(pFormation);
  if (it != m_stSelectedFormations.end()) {
    m_stSelectedFormations.erase(it);
    Modified();
  }
}

void CQBSettings::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS & /*progress*/) {
  double length, width, azi, cnorth, ceast, track, bin;
  stream >> length;
  stream >> width;
  stream >> azi;
  stream >> cnorth;
  stream >> ceast;
  stream >> track;
  stream >> bin;
  m_Grid.Azimuth(azi);
  m_Grid.CornerNorthing(cnorth);
  m_Grid.CornerEasting(ceast);
  m_Grid.TrackSpacing(track);
  m_Grid.BinSpacing(bin);
  m_Grid.SetLength(length);
  m_Grid.SetWidth(width);

  int temp;
  stream >> temp;
  for (int i = 0; i < temp; ++i) {
    int idx1, idx2;
    stream >> idx1;
    stream >> idx2;
    TDepletionStageEntry &form_entry = (TDepletionStageEntry &)(*m_Model.GraphEntry(MD_BASE_DEPLETION_STAGE));
    CDepletionStage *ds1 = form_entry.FindIndex(idx1);
    CDepletionStage *ds2 = form_entry.FindIndex(idx2);
    AddDeltaPressure(ds1, ds2);
  }

  if (version >= CStreamVersion(4, 1, 15)) {
    stream >> temp;
    for (int i = 0; i < temp; ++i) {
      int idx1, idx2;
      stream >> idx1;
      stream >> idx2;
      TDepletionStageEntry &form_entry = (TDepletionStageEntry &)(*m_Model.GraphEntry(MD_BASE_DEPLETION_STAGE));
      CDepletionStage *ds1 = form_entry.FindIndex(idx1);
      CDepletionStage *ds2 = form_entry.FindIndex(idx2);
      AddDeltaTemperature(ds1, ds2);
    }
  }

  // selected formations are saved from version 3.7.0
  if (version >= CStreamVersion(3, 7, 0)) {
    int size, nIndex;
    stream >> size;
    for (int i = 0; i < size; ++i) {
      stream >> nIndex;
      TFormationBaseEntry &form_entry = (TFormationBaseEntry &)(*m_Model.GraphEntry(MD_BASE_FORMATION));
      CFormationBase *fb = form_entry.FindIndex(nIndex);
      if (fb) // Bug 208350
        m_stSelectedFormations.insert(fb);
    }
  }
}

void CQBSettings::SaveStream(TSTREAM &stream, TPROGRESS & /*progress*/) {
  stream << m_Grid.GetLength();
  stream << m_Grid.GetWidth();
  stream << m_Grid.Azimuth();
  stream << m_Grid.CornerNorthing();
  stream << m_Grid.CornerEasting();
  stream << m_Grid.TrackSpacing();
  stream << m_Grid.BinSpacing();

  stream << (int)m_DeltaPressures.size();
  for (size_t i = 0; i < m_DeltaPressures.size(); ++i) {
    stream << m_DeltaPressures[i].first->Index();
    stream << m_DeltaPressures[i].second->Index();
  }

  stream << (int)m_DeltaTemperatures.size();
  for (size_t i = 0; i < m_DeltaTemperatures.size(); ++i) {
    stream << m_DeltaTemperatures[i].first->Index();
    stream << m_DeltaTemperatures[i].second->Index();
  }

  stream << (int)m_stSelectedFormations.size();
  std::set<const CFormationBase *>::const_iterator it;
  for (it = m_stSelectedFormations.begin(); it != m_stSelectedFormations.end(); ++it) {
    stream << (*it)->Index();
  }
}

// CQBInputWriter implementation *********************************
// ***************************************************************
CQBInputWriter::CQBInputWriter(CQBSettings &settings) : m_QBSettings(settings), m_fp(0), m_strError("") {}

CQBInputWriter::~CQBInputWriter(void) {
  if (m_fp)
    fclose(m_fp);
}

int CQBInputWriter::StepSize() const { return 2 + m_QBSettings.PointSet()->PointSize(); }

bool CQBInputWriter::WriteInput(const QString &sPath, IProgressBase &progress) {
  CPointSet *ptSet = m_QBSettings.PointSet();
  progress.StatusMessage("Writing QuickBlocks input file");

  m_fp = fopen(sPath.toStdString().c_str(), "w");
  if (!m_fp) {
    m_strError = QString("Failed to open file '%1' for writing").arg(sPath);
    return false;
  }
  fprintf(m_fp, "MESSAGE_FILE\n");
  fprintf(m_fp, "REAL_GEO_X R %f\n", m_QBSettings.Grid().CornerNorthing());
  fprintf(m_fp, "REAL_GEO_Y R %f\n", m_QBSettings.Grid().CornerEasting());
  fprintf(m_fp, "REAL_DELTA_X R %f\n", m_QBSettings.Grid().TrackSpacing());
  fprintf(m_fp, "REAL_DELTA_Y R %f\n", m_QBSettings.Grid().BinSpacing());
  fprintf(m_fp, "n_x I %d\n", m_QBSettings.Grid().PointsNorthing());
  fprintf(m_fp, "n_y I %d\n", m_QBSettings.Grid().PointsEasting());
  fprintf(m_fp, "~MESSAGE_FILE\n\n");
  progress.Step();

  fprintf(m_fp, "POINTSET\n");
  PrintHeader();
  progress.Step();

  fprintf(m_fp, "\n");

  for (int i = 0; i < ptSet->PointSize(); ++i) {
    fprintf(m_fp, "%f ", ptSet->PointAt(i).X());
    fprintf(m_fp, "%f ", ptSet->PointAt(i).Y());
    PrintDepthsAndPressures(i);
    fprintf(m_fp, "\n");
    progress.Step();
  }

  fprintf(m_fp, "~POINTSET");

  return true;
}
void CQBInputWriter::PrintHeader() {
  fprintf(m_fp, "Northing Easting ");
  TGrid2Intersects::const_iterator it;

  TFormationIntersects formIntersects1 = m_QBSettings.Intersections().begin()->second;
  for (size_t i = 0; i < formIntersects1.size(); ++i) {
    QString depthName;
    depthName = QString("top_%1").arg(formIntersects1[i]->Formation()->Name());
    depthName.replace(" ", "_");
    fprintf(m_fp, depthName.toStdString().c_str());
    fprintf(m_fp, " ");
    depthName = QString("base_%1").arg(formIntersects1[i]->Formation()->Name());
    depthName.replace(" ", "_");
    fprintf(m_fp, depthName.toStdString().c_str());
    fprintf(m_fp, " ");
  }

  for (size_t i = 0; i < m_QBSettings.DeltaPressures().size(); ++i) {
    TFormationIntersects formIntersects2 = m_QBSettings.Intersections().begin()->second;
    for (size_t j = 0; j < formIntersects2.size(); ++j) {
      QString press;
      press = QString("dP%1_%2").arg(i + 1).arg(formIntersects2[j]->Formation()->Name());
      press.replace(" ", "_");
      fprintf(m_fp, press.toStdString().c_str());
      fprintf(m_fp, " ");
    }
  }

  for (size_t i = 0; i < m_QBSettings.DeltaTemperatures().size(); ++i) {
    TFormationIntersects formIntersects = m_QBSettings.Intersections().begin()->second;
    for (size_t j = 0; j < formIntersects.size(); ++j) {
      QString temp;
      temp = QString("dP%1_%2").arg(i + 1).arg(formIntersects[j]->Formation()->Name());
      temp.replace(" ", "_");
      fprintf(m_fp, temp.toStdString().c_str());
      fprintf(m_fp, " ");
    }
  }
}

void CQBInputWriter::PrintDepthsAndPressures(int index) {
  TGrid2Intersects::const_iterator it = m_QBSettings.Intersections().find(index);
  assert(it != m_QBSettings.Intersections().end());
  TFormationIntersects formIntersects = it->second;

  for (size_t i = 0; i < formIntersects.size(); ++i) {
    if (formIntersects[i]->TopIntersect() == geo::CPoint::NullPoint)
      fprintf(m_fp, "NaN NaN ");
    else
      fprintf(m_fp, "%f %f ", formIntersects[i]->TopIntersect().Z(), formIntersects[i]->BotIntersect().Z());
  }

  const std::vector<std::pair<CDepletionStage *, CDepletionStage *>> &pressures = m_QBSettings.DeltaPressures();
  geo::CValue val1, val2;
  for (size_t i = 0; i < pressures.size(); ++i) {
    for (size_t j = 0; j < formIntersects.size(); ++j) {
      const CFormationBase *form = formIntersects[j]->Formation();
      if (formIntersects[j]->TopIntersect() == geo::CPoint::NullPoint)
        fprintf(m_fp, "NaN ");
      else {
        const geo::CPoint &top = formIntersects[j]->TopIntersect();
        const geo::CPoint &bot = formIntersects[j]->BotIntersect();
        geo::CPoint presPoint(top.X(), top.Y(), (top.Z() + bot.Z()) / 2.0);
        val1 = form->Pressure(*(pressures[i].first)).Component().ScalarData().ValuePoint(presPoint);
        val2 = form->Pressure(*(pressures[i].second)).Component().ScalarData().ValuePoint(presPoint);
        fprintf(m_fp, "%f ", val2.Value() - val1.Value());
      }
    }
  }

  const std::vector<std::pair<CDepletionStage *, CDepletionStage *>> &temperatures = m_QBSettings.DeltaTemperatures();
  for (size_t i = 0; i < temperatures.size(); ++i) {
    for (size_t j = 0; j < formIntersects.size(); ++j) {
      const CFormationBase *form = formIntersects[j]->Formation();
      if (formIntersects[j]->TopIntersect() == geo::CPoint::NullPoint)
        fprintf(m_fp, "NaN ");
      else {
        const geo::CPoint &top = formIntersects[j]->TopIntersect();
        const geo::CPoint &bot = formIntersects[j]->BotIntersect();
        geo::CPoint presPoint(top.X(), top.Y(), (top.Z() + bot.Z()) / 2.0);
        val1 = form->UserTemperature(*(temperatures[i].first)).Component().ScalarData().ValuePoint(presPoint);
        val2 = form->UserTemperature(*(temperatures[i].second)).Component().ScalarData().ValuePoint(presPoint);
        fprintf(m_fp, "%f ", val2.Value() - val1.Value());
      }
    }
  }
}
