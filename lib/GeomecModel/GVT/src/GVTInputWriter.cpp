
#include <memory>

#include "GVTInputWriter.h"
#include "GVTSettings.h"
#include "GVTSeismicGridDefinition.h"
#include "FormationBase.h"
#include "PropertyFile.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "unitnode.h"

CGVTInputWriter::CGVTInputWriter(const CGVTSettings &settings)
: m_GVTSettings(settings),
  m_fp(0),
  m_strError("")
{
}

CGVTInputWriter::~CGVTInputWriter(void)
{
  if(m_fp)
    fclose(m_fp);
}


bool CGVTInputWriter::WriteInput(const QString &sPath)
{
  m_fp = fopen(sPath.toStdString().c_str(), "w");
  if(!m_fp)
  {
    m_strError = QString("Failed to open file '%1' for writing").arg(sPath);
    return false;
  }

  if(!WriteOutputSection())
    return false;
  if(!WriteMessageFileSection())
    return false;
  if(!WriteVelModelSection())
    return false;
  if(!WriteCalcTimesSection())
    return false;
  if(!WritePointSetSection())
    return false;

  return true;
}

bool CGVTInputWriter::WriteOutputSection()
{
  fprintf(m_fp, "OUTPUT\n");
  fprintf(m_fp, "PREFIX %s\n", m_GVTSettings.OutputPrefix().toStdString().c_str());
  fprintf(m_fp, "WRITEPS %s\n", m_GVTSettings.ImportBack() ? "TRUE" : "FALSE");
  if(m_GVTSettings.DeltaVp())
    fprintf(m_fp, "WRITEVT DV\n");
  if(m_GVTSettings.Deltat())
    fprintf(m_fp, "WRITEVT DT\n");
  fprintf(m_fp, "~OUTPUT\n");
  return true;
}

bool CGVTInputWriter::WriteMessageFileSection()
{
  const CGVTSeismicGridDefinition &grid = m_GVTSettings.Grid(m_GVTSettings.CurrentGrid());
  fprintf(m_fp, "MESSAGE_FILE\n");
  if(!(grid.KeyFile() == ""))
  {
    // Append the keyfile to the inputfile
    FILE *fp;
    long len;
    char *buf;
    fp = fopen(grid.KeyFile().toStdString().c_str(),"r");
    if(!fp)
    {
      m_strError = QString("Failed to open file '%1'").arg(grid.KeyFile());
      return false;
    }
    fseek(fp, 0, SEEK_END); //go to end
    len = ftell(fp); //get position at end (length)
    fseek(fp, 0, SEEK_SET); //go to beg.
    buf=(char*)malloc(len); //malloc buffer
    fread(buf, sizeof(char), len, fp); //read into buffer
    fclose(fp);

    size_t count = fwrite(buf, sizeof(char), len, m_fp);
    if(count != len)
    {
      m_strError = QString("Error writing contents of the keyfile '%1' to the input file").arg(grid.KeyFile());
      return false;
    }
  }
  else
  {
    fprintf(m_fp, "DELTA_BIN_NUM I %d\n", grid.DeltaBin());
    fprintf(m_fp, "DELTA_TRACK_NUM I %d\n", grid.DeltaTrack());
    fprintf(m_fp, "DIGITIZATION I 4\n");
    fprintf(m_fp, "FIRST_BIN I %d\n", grid.FirstBin());
    fprintf(m_fp, "FIRST_TRACK I %d\n", grid.FirstTrack());
    fprintf(m_fp, "N_BIN I %d\n", grid.NumBin());

    int n_samp = 0;
    if((int)grid.Depth() % 250 == 0)
      n_samp = (int)grid.Depth();
    else if((int)grid.Depth() % 250 < 125)
      n_samp = (int)grid.Depth() / 250;
    else if((int)grid.Depth() % 250 >= 125)
      n_samp = ((int)grid.Depth() / 250) + 250;
    fprintf(m_fp, "N_SAMP I %d\n", n_samp);

    fprintf(m_fp, "N_TRACK I %d\n", grid.NumTrack());
    fprintf(m_fp, "REAL_DELTA_X R %f\n", grid.TrackSpacing());
    fprintf(m_fp, "REAL_DELTA_Y R %f\n", grid.BinSpacing());
    fprintf(m_fp, "REAL_GEO_X R %f\n", grid.CornerEasting());
    fprintf(m_fp, "REAL_GEO_Y R %f\n", grid.CornerNorthing());
    fprintf(m_fp, "SKEW_ANGLE R %f\n", grid.Azimuth());
    fprintf(m_fp, "ZERO_TIME I 0\n");
  }
  fprintf(m_fp, "~MESSAGE_FILE\n");
  return true;
}

bool CGVTInputWriter::WriteVelModelSection()
{
  fprintf(m_fp, "VELMODS\n");
  CGVTSettings &settings = const_cast<CGVTSettings&>(m_GVTSettings);
  std::map<CFormationBase*, CGVTVelocityModel*>::const_iterator it;
  for(it = settings.FormationToVelModelMp().begin(); it != settings.FormationToVelModelMp().end(); ++it)
  {
    QString name = it->first->Name();
    name.replace(" ","_");
    fprintf(m_fp, "%s ", name.toStdString().c_str());
    CGVTVelocityModel *velmod = it->second;
    fprintf(m_fp, "%s", velmod->ModelName().toStdString().c_str());
    std::map<QString, double>::iterator itParam;
    for(itParam = velmod->Parameters().begin(); itParam != velmod->Parameters().end(); ++itParam)
    {
      fprintf(m_fp, " ");
      fprintf(m_fp, "%s=%f", itParam->first.toStdString().c_str(), itParam->second);
    }
    fprintf(m_fp, "\n");
  }
  fprintf(m_fp, "~VELMODS\n");
  return true;
}

bool CGVTInputWriter::WriteCalcTimesSection()
{
  const std::vector<std::pair<CDepletionStage*, CDepletionStage*> > &times = m_GVTSettings.TimeLapses();
  if(times.size() == 0)
  {
    m_strError = "No TimeLapse Scenarios Found";
    return false;
  }

  fprintf(m_fp, "CALCTIMES\n");
  
  for(size_t i = 0; i < times.size(); ++i)
  {
    fprintf(m_fp, "D%d D%d\n", times[i].first->Index(), times[i].second->Index());
  }
  fprintf(m_fp, "~CALCTIMES\n");
  return true;
}

bool CGVTInputWriter::WritePointSetSection()
{
  CUnitNode unitNode;
  IExportFormat::CExportArg arg(unitNode.Unit(),
                                IExportFormat::AXIS3D,
                                CValueType::MT_NONE,
                                0,
                                0,
                                CAnalysisType::AT_NONLIN,
                                0,
                                IExportFormat::FULL_ELEMENT,
                                IExportFormat::INPUT_POINTS,
                                false,
                                true,
                                true);
  CNodalExportFormat format(*m_GVTSettings.AnalysisPointSet(), m_GVTSettings.DataVec(), arg);
  CElementSetFile file(format);
  file.SetDelimiter(" ");
  file.WriteHeader(false);
  std::auto_ptr <IProgressBase> prog;

  try
  {
    prog.reset(_g->prog()->create(eProgress::Geo, ""));
    if(!file.Save(m_fp, *prog))
    {
      m_strError = "Error writing pointset";
      return false;
    }
  }
  catch(CProgressCancel* e)
  {
    delete e;
    m_strError = "Cancelled";
    return false;
  }

  fprintf(m_fp, "~POINTSET");
  return true;
}
