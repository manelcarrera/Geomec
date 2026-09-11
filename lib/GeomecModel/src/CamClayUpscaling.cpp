#include "CamClayUpscaling.h"
#include "3dmodel.h"
#include "BaseEntryTypes.h"
#include "MeshBase.h"
#include "ValueTypes.h"
#include "NodalValueSet.h"
#include "Pressure.h"
#include "IProgressFactory.h"

CCamClayUpscalingNode::CCamClayUpscalingNode(CFemAppModel& model)
: IUpscalingNodeBase(model)
{
}

CCamClayUpscalingNode::CCamClayUpscalingNode(const QString& strName, C3DModel& model)
: IUpscalingNodeBase(strName, model)
{
  m_formationFilter = new CCamClayFormationFilter();
  FormationsBranch().FormationFilter(m_formationFilter);

  assert(model.GraphEntry(MD_BASE_UPSCALING));
  reParent(model.GraphEntry(MD_BASE_UPSCALING));
  Modified();
}

CCamClayUpscalingNode::CCamClayUpscalingNode(const CCamClayUpscalingNode &rhs)
: IUpscalingNodeBase(rhs)
{
  m_formationFilter = new CCamClayFormationFilter();
  FormationsBranch().FormationFilter(m_formationFilter);
}

CCamClayUpscalingNode::~CCamClayUpscalingNode()
{
  delete m_formationFilter;
}

int CCamClayUpscalingNode::FactoryType() const
{
  return FACTORYTYPE_CAMCLAYUPSCALING;
}

void CCamClayUpscalingNode::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  IUpscalingNodeBase::LoadStream(stream, version, progress);

  m_formationFilter = new CCamClayFormationFilter();
  FormationsBranch().FormationFilter(m_formationFilter);
}

bool CCamClayUpscalingNode::CanExecute() const
{
  bool canExecute = false;

  if(FormationsBranch().HasFormations() && TargetPointsetBranch().HasPointset())
  {
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  for(size_t i = 0; i < FormationsBranch().referenceSize(); ++i)
  {
      const CFormationBase* pFormation = dynamic_cast<const CFormationBase*>(&FormationsBranch().referenceAt(i));
      if(pFormation)
      {
    int matmodel = pFormation->Material(model.InitialDepletionStage()).MaterialModel();
    if(matmodel == MM_CAMCLAY || matmodel == MM_DUALCAP_LINELA)
          canExecute = true;
    if(matmodel == MM_FRACTURE_APERTURE || matmodel == MM_FRACTURE_APERTURE2)
          return false;
      }
  }
  }

  return canExecute;
}

void CCamClayUpscalingNode::Execute(IProgressBase& prog)
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CDepletionStageEntry& deplentry = model.DepletionStageEntry();
  CDepletionStageEntry::const_iterator dsit;

  TMatParamsMap mpParams;
  TLoadParamsMap mpLoads;

  CPillarMap::const_iterator it;

  int nStartStages = 0;
  for(dsit = deplentry.begin(); dsit != deplentry.end(); ++dsit)
  {
  if(dsit->IsPhaseStartStage())
      ++nStartStages;
  }

  int nStages = deplentry.EntryNodes().size();

  for(it = TargetPointsetBranch().begin(); it != TargetPointsetBranch().end(); ++it)
  {
  const CPillar& pillar = it->second;
  prog.AddSteps((pillar.size() + 1) * nStartStages + pillar.size() * nStages);
  }

  CPointSet* pPointset = 0;
  QString s3DName;

  try
  {

  for(dsit = deplentry.begin(); dsit != deplentry.end(); ++dsit)
  {
      const CDepletionStage& stage = *dsit;
      if(stage.IsPhaseStartStage())
      {
    prog.StatusMessage(QString("Computing material parameters for '%1'").arg(QString(stage.Name())));
    for(it = TargetPointsetBranch().begin(); it != TargetPointsetBranch().end(); ++it)
    {
          const CPillarPos& pos = it->first;
          const CPillar& pillar = it->second;
          CPillar::const_iterator itP;
          geo::CPoint ptPrev;
          // current delta thickness
          double dThickness;
          // the sum of the total thicknesses
          double dSumThick = 0;
          // the sum of the thickness inside CamClay bodies
          double dCCSumThick = 0;

          // terms from equations that need to be calculated using the sum. equation number in brackets.
          double v1 = 0; // di * (Nui / (1 - Nui) {42}
          double v2 = 0; // Ci * di {43}
          double v3 = 0; // di * xi {53} xi represents Rho
          double v4 = 0; // di * xi {48} xi represents n0i
          double v5 = 0; // di * xi {48} xi represents Cp1i
          double v6 = 0; // di * xi {48} xi represents Cp2i
          double v7 = 0; // di * xi {48} xi represents Pc1i
          double v8 = 0; // di * xi {48} xi represents Pc2i
          double v9 = 0; // di * xi {42} xi represents alphai
          double v10 = 0; // di * xi {48} xi represents gammai
          double v11 = 0; // di * xi {48} xi represents KHi
          double v12 = 0; // di * xi {48} xi represents Khi
          double v13 = 0; // di * xi {48} xi represents OHi
          double v14 = 0; // (1 - 1.3*sqrt(alphai)*pow(sin(phii), 2))*di {49}
          double v15 = 0; // ((1 + Nui) / (1 - Nui)) * Betai * di {57}
          double v16 = 0; // di / Cp1i (or di / Cei if Cp1i not present) {59}
          double v17 = 0; // di * Cp1i (or di * Cei if Cp1i not present)

          for(itP = pillar.begin(); itP != pillar.end(); ++itP)
          {
      prog.Step();
      CPillar::CPillarPoint pt = (*itP).first;
      if(!ptPrev.Empty())
      {
              const geo::IBody* pBody = (*itP).second;
              assert(pBody != 0);
              if(IsTarget(*pBody))
              {
        dThickness = pt.Z() - ptPrev.Z();
        if(dThickness > 0)
        {
                  FillNonCCValues(*pBody, stage);
                  
                  calculatedValues.Cei = ElasticCompressibility(materialModelParameters.Nui, materialModelParameters.Ei);//( (1 + Nui)*(1 - 2 * Nui) ) / ( Ei*(1 - Nui) );

                  v1 += dThickness * (materialModelParameters.Nui / (1 - materialModelParameters.Nui));
                  v2 += calculatedValues.Cei * dThickness;
                  v3 += materialModelParameters.Rhoi * dThickness;
                  v15 += ( (1 + materialModelParameters.Nui) / (1 - materialModelParameters.Nui) ) * materialModelParameters.Betai * dThickness;
                  v16 += dThickness / calculatedValues.Cei;
                  v17 += dThickness * calculatedValues.Cei;

                  dSumThick += dThickness;
        }
              }

              // CamClay specific
              if(IsTarget(*pBody) && IsCCBody(*pBody, stage))
              {
        dThickness = pt.Z() - ptPrev.Z();
        if(dThickness > 0)
        {
                  // first the material parameters
                  FillCCValues(*pBody, stage);
                 // psii = 1 - ( 0.5 * sqrt(alphai) * pow(sin(Phii), 2) ); // {40}
                  calculatedValues.Cp1i = PlasticCompressibility1(materialModelParameters.n0i, materialModelParameters.Lam1i, materialModelParameters.Pc1i, calculatedValues.Cei, materialModelParameters.alphai, materialModelParameters.Phii); // ( ( (1 - n0i) * Lam1i * psii ) / Pc1i ) + Cei; // {45}
                  calculatedValues.Cp2i = PlasticCompressibility2(materialModelParameters.n0i, materialModelParameters.Lam1i, materialModelParameters.Lam2i, materialModelParameters.Pc2i, calculatedValues.Cei, materialModelParameters.alphai, materialModelParameters.Phii);//( ( ((1 - n0i) * (Lam1i + Lam2i)) * psii ) / Pc2i ) + Cei; // {47}
                  
                  v4 += materialModelParameters.n0i * dThickness;
                  v5 += calculatedValues.Cp1i * dThickness;
                  v6 += calculatedValues.Cp2i * dThickness;
                  v7 += materialModelParameters.Pc1i * dThickness;
                  v8 += materialModelParameters.Pc2i * dThickness;
                  v9 += materialModelParameters.alphai * dThickness;
                  v10 += materialModelParameters.gammai * dThickness;
                  v11 += materialModelParameters.KHi * dThickness;
                  v12 += materialModelParameters.Khi * dThickness;
                  v13 += materialModelParameters.OHi * dThickness;
                  v14 += ( (1 - 1.3 * sqrt(materialModelParameters.alphai) * pow(sin(DEG2RAD(materialModelParameters.Phii)), 2)) ) * dThickness;
                  v16 += dThickness / calculatedValues.Cp1i;
                  v17 += dThickness * calculatedValues.Cp1i;

                  dCCSumThick += dThickness;
        }
              }
      }
      ptPrev = pt;
          }
          
          TUpscaledParams matParams;
          matParams.nu = 1 / (1 + (dSumThick / v1)); // {36}
          matParams.Ce = v2 / dSumThick;
          matParams.E  = ( (1 + matParams.nu)*(1 - 2 * matParams.nu) ) / ( matParams.Ce * (1 - matParams.nu) ); // {38}
          matParams.Rho = v3 / dSumThick;
          matParams.n0 = v4 / dCCSumThick;
          matParams.Cp1 = v5 / dSumThick;
          matParams.Cp2 = v6 / dSumThick;
          matParams.Pc1 = v7 / dCCSumThick;
          matParams.Pc2 = v8 / dCCSumThick;
          matParams.alpha = v9 / dCCSumThick;
          matParams.gamma = v10 / dCCSumThick;
          matParams.KH = v11 / dCCSumThick;
          matParams.Kh = v12 / dCCSumThick;
          matParams.OH = v13 / dCCSumThick;
          matParams.Beta = v15 / ( ((1 + matParams.nu) / (1 - matParams.nu)) * dSumThick ); // {57}
          matParams.Et = std::max(v16 / dSumThick - dSumThick / v17, 1.); // {59}
          matParams.En = 1;
          matParams.nut = 0;
          matParams.nun = 0;
          matParams.Ga = 1;
          
          calculatedValues.Kp0 = v14 / dCCSumThick;
          matParams.Phi = RAD2DEG(asin( sqrt((1 - calculatedValues.Kp0) / (1.3 * sqrt(matParams.alpha))) )); // {50}

          calculatedValues.psib = 1 - ( (1 - calculatedValues.Kp0) / 2.6 ); // {51}
          matParams.Lam1 = ( (matParams.Cp1 - matParams.Ce) * matParams.Pc1 ) / ( (1 - matParams.n0) * calculatedValues.psib ); // {52}
          matParams.Lam2 = ( (matParams.Cp2 - matParams.Cp1) * matParams.Pc2 ) / ( (1 - matParams.n0) * calculatedValues.psib ); // {52}
          mpParams.insert(TMatParamsMap::value_type(std::make_pair(pos, &stage), matParams));
    }
      }

      for(it = TargetPointsetBranch().begin(); it != TargetPointsetBranch().end(); ++it)
      {
    prog.StatusMessage(QString("Computing pressures and temperatures for '%1'").arg(QString(stage.Name())));

    const CPillarPos& pos = it->first;
    const CPillar& pillar = it->second;
    CPillar::const_iterator itP;
    geo::CPoint ptPrev;

    double p1 = 0; // Cei * dPi *di {54}
    double t1 = 0; // ((1 + nui)/(1 - nui))*betai*dTi {58}
    double lam1 = 0; // Cp1 * dPi * di {55}
    double lam2 = 0; // Cp2 * dPi * di {56}

    double dSumThick = 0;
    double dSumCCThick = 0;
    
    // the flattened (upscaled) absolute initial pressure and temperature for this position
    double dInitPressure = 0;
    double dInitTemper = 0;
    
    for(itP = pillar.begin(); itP != pillar.end(); ++itP)
    {
          prog.Step();
          CPillar::CPillarPoint pt = (*itP).first;
          if(!ptPrev.Empty())
          {
      const geo::IBody* pBody = (*itP).second;
      assert(pBody != 0);
      if(IsTarget(*pBody))
      {
              double di = pt.Z() - ptPrev.Z();
              if(di > 0)
              {
        // midpoint of the segment
        geo::CPoint ptMid((pt.X() + ptPrev.X()) / 2, (pt.Y() + ptPrev.Y()) / 2, (pt.Z() + ptPrev.Z()) / 2);
        const CMeshBase& mesh = model.Mesh();
        const CFormationBase& formation = *mesh.Formation(*pBody);

        dSumThick += di;
        
        FillNonCCValues(*pBody, stage);
        calculatedValues.Cei = ElasticCompressibility(materialModelParameters.Nui, materialModelParameters.Ei);
        if(IsCCBody(*pBody, stage))
        {
                  FillCCValues(*pBody, stage);
                  calculatedValues.Cp1i = PlasticCompressibility1(materialModelParameters.n0i, materialModelParameters.Lam1i, materialModelParameters.Pc1i, calculatedValues.Cei, materialModelParameters.alphai, materialModelParameters.Phii); 
                  calculatedValues.Cp2i = PlasticCompressibility2(materialModelParameters.n0i, materialModelParameters.Lam1i, materialModelParameters.Lam2i, materialModelParameters.Pc2i, calculatedValues.Cei, materialModelParameters.alphai, materialModelParameters.Phii);

                  dSumCCThick += di;
        }

        if (stage.Initial())
        {
                  dInitPressure += GetLoadValue(*pBody, ptMid, formation.Pressure(stage)).Value() * di;
                  dInitTemper += GetLoadValue(*pBody, ptMid, formation.EffectiveTemperature(stage)).Value() * di;

                  if(IsCCBody(*pBody, stage))
                  {
          lam1 += calculatedValues.Cp1i * dInitPressure;
          lam2 += calculatedValues.Cp2i * dInitPressure;
                  }
        }
        else
        {
                  geo::CValue prevpressure = GetLoadValue(*pBody, ptMid, formation.Pressure(stage.Previous()));
                  geo::CValue pressure = GetLoadValue(*pBody, ptMid, formation.Pressure(stage));
                  geo::CValue prevtemperature = GetLoadValue(*pBody, ptMid, formation.EffectiveTemperature(stage.Previous()));
                  geo::CValue temperature = GetLoadValue(*pBody, ptMid, formation.EffectiveTemperature(stage));

                  double deltaPi = pressure.Value() - prevpressure.Value();
                  double deltaTi = temperature.Value() - prevtemperature.Value();

                  p1 += calculatedValues.Cei * deltaPi * di;
                  t1 += ( (1 + materialModelParameters.Nui)/(1 - materialModelParameters.Nui) ) * materialModelParameters.Betai * deltaTi;

                  if(IsCCBody(*pBody, stage))
                  {
          lam1 += calculatedValues.Cp1i * deltaPi * di;
          lam2 += calculatedValues.Cp2i * deltaPi * di;
                  }
        }
              }
      }
          }

          ptPrev = pt;
    }
    TUpscaledParams matParams = UpscaledParameters(mpParams, stage, pos);
    // divide pressure sum by Ce,m,b (mantis #3370)
    p1 /= matParams.Ce;
    // correct hardening parameters
    matParams.Lam1 = matParams.Lam1 * (matParams.Ce / matParams.Cp1) * (lam1 / p1) * (dSumThick / dSumCCThick);
    matParams.Lam2 = matParams.Lam2 * (matParams.Ce / matParams.Cp2) * (lam2 / p1) * (dSumThick / dSumCCThick);

    TUpscaledLoadParams loadParams;

    if(stage.Initial())
    {
          dInitPressure /= dSumThick;
          dInitTemper /= dSumThick;
          loadParams.P = dInitPressure;
          loadParams.T = dInitTemper;
    }
    else
    {
          // get previous values of P and T
          TLoadParamsMap::const_iterator it2 = mpLoads.find(std::make_pair(pos, &stage.Previous()));
          assert(it2 != mpLoads.end());
          loadParams.P = it2->second.P + p1 / dSumThick;
          loadParams.T = it2->second.T + t1 / ( (1 + matParams.nu) / (1 - matParams.nu) ) * matParams.Beta * dSumThick;
    }
    mpLoads.insert(TLoadParamsMap::value_type(std::make_pair(pos, &stage), loadParams));
      }
  }

  const TPointSetEntry& psentry = (const TPointSetEntry&)*model.GraphEntry(MD_BASE_POINTSET);

  // create a new pointset

  std::pair <QString, QString> uniqueNames = psentry.createUniqueNames(
      DEFAULT_2D_POINT_SET_NAME.arg(Name()) + DEFAULT_2D_POINT_SET_NUMBER,
      DEFAULT_3D_POINT_SET_NAME.arg(Name()) + DEFAULT_3D_POINT_SET_NUMBER,
      DEFAULT_INITIAL_NUMBER);
  QString sName = uniqueNames.first;

  s3DName = uniqueNames.second;

  prog.StatusMessage(QString("Generating pointset '%1'").arg(QString(sName)));

  int nValueSetSize = nStartStages * sizeof(TUpscaledParams) / sizeof(double) + nStages * sizeof(TUpscaledLoadParams) / sizeof(double);
  pPointset = new CPointSet(sName, Model(), nValueSetSize, IPointSet::DIM_2D);

  // counter for the value set index
  int p = 1; // index of the second coordinate valueset

  CDepletionStageEntry::const_iterator itds;
  for(itds = deplentry.begin(); itds != deplentry.end(); ++itds)
  {
      const CDepletionStage& stage = *itds;
      int idxStage = stage.Index();

      // value types
      const CValueTypeFactory *factory = CValueTypeFactory::instance();

      QString s;
      QString t;

      if(stage.IsPhaseStartStage())
      {
    s = getStringTableEntry(IDS_VALUENAME_ELASCOMPRES);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_ELASCOMPRES, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_POISSONS_RATIO);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_POISSONS_RATIO, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_YOUNGS_MODULUS);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_YOUNGS_MODULUS, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_POROSITY);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_POROSITY, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_PLASCOMPRES);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_PLASCOMPRES, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_PLASCOMPRES);
    t = QString("%1_2_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_PLASCOMPRES, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_PRECONSOLIDATION);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_PRECONSOLIDATION, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_SEC_PRECON);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_SEC_PRECON, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_CAPSHAPE);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_CAPSHAPE, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_TENSILE_STRETCH);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_TENSILE_STRETCH, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_LATRATIO_MAX);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_LATRATIO_MAX, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_LATRATIO_MIN);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_LATRATIO_MIN, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_AZIMUTH);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_AZIMUTH, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_INITFRICTION);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_INITFRICTION, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_HARDENING);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_HARDENING, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_SEC_HARDENING);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_SEC_HARDENING, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_DENSITY);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_RHOB, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_THERMAL_EXPANSION);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_THERMAL_EXPANSION, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_YOUNGMODULUS_TRANS);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_YOUNGMODULUS_TRANS, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_YOUNGMODULUS_NORM);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_YOUNGMODULUS_NORM, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_POISSONRATIO_TRANS);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_POISSONRATIO_TRANS, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_POISSONRATIO_NORM);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_POISSONRATIO_NORM, t)->Component());
    s = getStringTableEntry(IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS);
    t = QString("%1_D%2").arg(s).arg(idxStage);
    pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS, t)->Component());
      }

      s = getStringTableEntry(IDS_VALUENAME_PRESSURE);
      t = QString("%1_D%2").arg(s).arg(idxStage);
      pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_PRESSURE, t)->Component());
      s = getStringTableEntry(IDS_VALUENAME_TEMPERATURE);
      t = QString("%1_D%2").arg(s).arg(idxStage);
      pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_TEMPERATURE, t)->Component());
  }

  // loop over the 2D points
  CPillarMap::const_iterator itPos;
  for(itPos = TargetPointsetBranch().begin(); itPos != TargetPointsetBranch().end(); ++itPos)
  {
      const CPillarPos& pos = itPos->first;
      std::vector<double> vcRow(nValueSetSize + 2);

      // the coordinates
      vcRow[0] = pos.X();
      vcRow[1] = pos.Y();

      int n = 1; // valueset index

      // loop over all stages
      for(itds = deplentry.begin(); itds != deplentry.end(); ++itds)
      {
    const CDepletionStage& stage = *itds;

    // the material parameters
    TMatParamsMap::const_iterator itMat = mpParams.find(std::make_pair(pos, &stage));
    if(itMat != mpParams.end())
    {
          vcRow[++n] = itMat->second.Ce;    // elastic compressibility 
          vcRow[++n] = itMat->second.nu;    // Poisson ratio
          vcRow[++n] = itMat->second.E;     // Young's modulus
          vcRow[++n] = itMat->second.n0;    // porosity
          vcRow[++n] = itMat->second.Cp1;   // plastic compressibility 1
          vcRow[++n] = itMat->second.Cp2;   // plastic compressibility 2
          vcRow[++n] = itMat->second.Pc1;   // Preconsolidation stress 1
          vcRow[++n] = itMat->second.Pc2;   // Presonsolidation stress 2
          vcRow[++n] = itMat->second.alpha; // capshape  
          vcRow[++n] = itMat->second.gamma; // Tensile stretch
          vcRow[++n] = itMat->second.KH;    // SHtot/Stot ratio
          vcRow[++n] = itMat->second.Kh;    // Shtot/Svtot ratio
          vcRow[++n] = itMat->second.OH;    // SH azimuth
          vcRow[++n] = itMat->second.Phi;   // friction angle
          vcRow[++n] = itMat->second.Lam1;  // Hardening coeff 1
          vcRow[++n] = itMat->second.Lam2;  // Hardening coeff 2
          vcRow[++n] = itMat->second.Rho;   // density
          vcRow[++n] = itMat->second.Beta;  // thermal expansion coeff
          vcRow[++n] = itMat->second.Et;    // transverse Young's Modulus
          vcRow[++n] = itMat->second.En;    // normal Young's Modulus
          vcRow[++n] = itMat->second.nut;   // transverse Poisson's ratio
          vcRow[++n] = itMat->second.nun;   // normal Poisson's ratio
          vcRow[++n] = itMat->second.Ga;    // anisotropic shear modulus
    }

    // the load parameters
    TLoadParamsMap::const_iterator itLoad = mpLoads.find(std::make_pair(pos, &stage));
    if(itLoad != mpLoads.end())
    {
          vcRow[++n] = itLoad->second.P;
          vcRow[++n] = itLoad->second.T;
    }
      }

      if(n > 1)
    pPointset->PushBack(vcRow);

      prog.Step();
  }

  }
  catch(CProgressCancel* e)
  {
  delete e;
  delete pPointset;
  pPointset = 0;
  }

  if(pPointset && pPointset->PointSize() == 0)
  {
  delete pPointset;
  pPointset = 0;
  _m()->msg("No points are included in the defined range. "
      "No output is generated");
  }

  if(pPointset) // only on success
  TargetPointsetBranch().CopyPillarMapAsPointset(*pPointset, s3DName);
}

geo::CValue CCamClayUpscalingNode::GetLoadValue(const geo::IBody& body, const geo::IPoint& pt, const IValueComposite& loadcomposite) const
{
  IValueDomainScalar::TValueVec vcValues = loadcomposite.Component().ScalarData().ValueElement(body);
  return body.InterpolateValue(pt, vcValues);
}

void CCamClayUpscalingNode::FillNonCCValues(const geo::IBody &body, const CDepletionStage &stage)
{
  if ((materialModelParameters.Nui = MaterialValue(body, stage, IDT_VALUETYPE_UNDRAINEDPOISSON)) == -1)
  materialModelParameters.Nui = MaterialValue(body, stage, IDT_VALUETYPE_POISSONS_RATIO);
  if ((materialModelParameters.Ei = MaterialValue(body, stage, IDT_VALUETYPE_UNDRAINEDYOUNG)) == -1)
  materialModelParameters.Ei = MaterialValue(body, stage, IDT_VALUETYPE_YOUNGS_MODULUS);
  materialModelParameters.Rhoi = MaterialValue(body, stage, IDT_VALUETYPE_RHOB);
  if ((materialModelParameters.Betai = MaterialValue(body, stage, IDT_VALUETYPE_UNDRAINEDTHERMX)) == -1)
  materialModelParameters.Betai = MaterialValue(body, stage, IDT_VALUETYPE_THERMAL_EXPANSION);
}

void CCamClayUpscalingNode::FillCCValues(const geo::IBody &body, const CDepletionStage &stage)
{
  materialModelParameters.alphai = MaterialValue(body, stage, IDT_VALUETYPE_CAPSHAPE);
  materialModelParameters.gammai = MaterialValue(body, stage, IDT_VALUETYPE_TENSILE_STRETCH);
  materialModelParameters.n0i = MaterialValue(body, stage, IDT_VALUETYPE_POROSITY);
  materialModelParameters.Lam1i = MaterialValue(body, stage, IDT_VALUETYPE_HARDENING);
  if(IsDualCap(body, stage))
  materialModelParameters.Lam2i = MaterialValue(body, stage, IDT_VALUETYPE_SEC_HARDENING);
  else
  materialModelParameters.Lam2i = 0;
  materialModelParameters.Pc1i = MaterialValue(body, stage, IDT_VALUETYPE_PRECONSOLIDATION);
  if(IsDualCap(body, stage))
  materialModelParameters.Pc2i = MaterialValue(body, stage, IDT_VALUETYPE_SEC_PRECON);
  else
  materialModelParameters.Pc2i = materialModelParameters.Pc1i;
  materialModelParameters.OHi = MaterialValue(body, stage, IDT_VALUETYPE_AZIMUTH);
  materialModelParameters.Phii = MaterialValue(body, stage, IDT_VALUETYPE_INITFRICTION);
  materialModelParameters.KHi = MaterialValue(body, stage, IDT_VALUETYPE_LATRATIO_MAX);
  materialModelParameters.Khi = MaterialValue(body, stage, IDT_VALUETYPE_LATRATIO_MIN);
}

bool CCamClayUpscalingNode::IsTarget(const geo::IBody& body) const
{
  return FormationsBranch().IsInRange(body) && PointsetRangesBranch().IsInRange(body.MidPoint());
}

CCamClayUpscalingNode::TUpscaledParams& CCamClayUpscalingNode::UpscaledParameters(TMatParamsMap &mpParams, const CDepletionStage &stage, const CPillarPos &pos)
{
  const CDepletionStage *tempStage = &stage;
  
  TMatParamsMap::iterator it = mpParams.end();
  while(it == mpParams.end())
  {
  it = mpParams.find(std::make_pair(pos, tempStage));
  if(it == mpParams.end())
      tempStage = &tempStage->Previous();
  }

  return it->second;
  
}

double CCamClayUpscalingNode::ElasticCompressibility(const double &nu, const double &E)
{
  return ( (1 + nu)*(1 - 2 * nu) ) / ( E*(1 - nu) );
}

double CCamClayUpscalingNode::PlasticCompressibility1(const double &n0, const double &Lam1,
                                                      const double &Pc1, const double &Ce, 
                                                      const double &alpha, const double &phi)
{
  double psi = 1 - ( 0.5 * sqrt(alpha) * pow(sin(DEG2RAD(phi)), 2) ); // {40}
  return ( ( (1 - n0) * Lam1 * psi ) / Pc1 ) + Ce; // {39}
}
double CCamClayUpscalingNode::PlasticCompressibility2(const double &n0, const double &Lam1,
                                                      const double &Lam2, const double &Pc2,
                                                      const double &Ce, const double &alpha,
                                                      const double &phi)
{
  double psi = 1 - ( 0.5 * sqrt(alpha) * pow(sin(DEG2RAD(phi)), 2) ); // {40}
  return ( ( ((1 - n0) * (Lam1 + Lam2)) * psi ) / Pc2 ) + Ce; // {41}
}

bool CCamClayUpscalingNode::IsCCBody(const geo::IBody &body, const CDepletionStage &stage) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CFormationBase& formation = *model.Mesh().Formation(body);
  const CMaterialServer& matServ = formation.Material(stage);

  switch(matServ.MaterialModel())
  {
  case MM_CAMCLAY:
  case MM_DUALCAP_LINELA:
  return true;
  default:
  return false;
  }
}

bool CCamClayUpscalingNode::IsDualCap(const geo::IBody &body, const CDepletionStage &stage) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CFormationBase& formation = *model.Mesh().Formation(body);
  const CMaterialServer& matServ = formation.Material(stage);

  switch(matServ.MaterialModel())
  {
  case MM_DUALCAP_LINELA:
  return true;
  default:
  return false;
  }
}

double CCamClayUpscalingNode::MaterialValue(const geo::IBody &body, const CDepletionStage &stage, unsigned int parameterID)
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CFormationBase& formation = *model.Mesh().Formation(body);
  const CFFMaterial& mat = formation.Material(stage).Material(body);

  if(mat.IsParameter(parameterID))
  return mat.ParameterValue(parameterID);

  return -1;
}

bool CCamClayUpscalingNode::ParameterExists(const geo::IBody &body, const CDepletionStage &stage, unsigned int parameterID)
{
  const CModelBase &model = static_cast<const CModelBase&>(Model());
  const CFormationBase& formation = *model.Mesh().Formation(body);
  const CFFMaterial& mat = formation.Material(stage).Material(body);

  return mat.IsParameter(parameterID);
}

