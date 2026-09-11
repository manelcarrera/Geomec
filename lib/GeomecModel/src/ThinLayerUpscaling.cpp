#include "3dmodel.h"
#include "BaseEntryTypes.h"
#include "ThinLayerUpscaling.h"
#include "MeshBase.h"
#include "FormationBase.h"
#include "Material.h"
#include "ValueTypes.h"
#include "ivaluecomposite.h"
#include "IBody.h"
#include "Pressure.h"
#include "NodalValueSet.h"
#include "IProgressFactory.h"

CThinLayerUpscalingNode::CThinLayerUpscalingNode(CFemAppModel& model)
: IUpscalingNodeBase(model)
{
}

CThinLayerUpscalingNode::CThinLayerUpscalingNode(const QString& strName, C3DModel& model)
: IUpscalingNodeBase(strName, model)
{
  assert(model.GraphEntry(MD_BASE_UPSCALING));
  reParent(model.GraphEntry(MD_BASE_UPSCALING));
  Modified();
}

CThinLayerUpscalingNode::CThinLayerUpscalingNode(const CThinLayerUpscalingNode &rhs)
: IUpscalingNodeBase(rhs)
{
}

CThinLayerUpscalingNode::~CThinLayerUpscalingNode()
{
}

int CThinLayerUpscalingNode::FactoryType() const
{
  return FACTORYTYPE_THINLAYERUPSCALING;
}

void CThinLayerUpscalingNode::Execute(IProgressBase& prog)
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CDepletionStageEntry& deplentry = model.DepletionStageEntry();

  m_mpMat2SMatrix.clear();
  m_mpMat2CMatrix.clear();

  int nStartStages = 0;
  CDepletionStageEntry::const_iterator dsit;
  for(dsit = deplentry.begin(); dsit != deplentry.end(); ++dsit)
  {
    if(dsit->IsPhaseStartStage())
      ++nStartStages;
  }

  int nStages = deplentry.EntryNodes().size();

  for(CPillarMap::const_iterator it = TargetPointsetBranch().begin(); it != TargetPointsetBranch().end(); ++it)
  {
    const CPillar& pillar = it->second;
    prog.AddSteps((pillar.size() + 1) * nStartStages + pillar.size() * nStages);
  }

  // the maps with resulting upscaled values
  TMatParamsMap mpMatParams;
  TLoadParamsMap mpLoadParams;

  typedef geo::CCoordinateMap<CPillarPos, geo::CMatrix> TPillarMatrixMap;
  TPillarMatrixMap mpPillarMatrix;

  CPointSet* pPointset = 0;
  QString s3DName;

  try
  {

    for(dsit = deplentry.begin(); dsit != deplentry.end(); ++dsit)
    {
      const CDepletionStage& stage = *dsit;
      if(stage.IsPhaseStartStage())
      {
        prog.StatusMessage(QString("Computing elastic parameters for '%1'").arg(QString(stage.Name())));
        // reinitialize pillar matrix map
        mpPillarMatrix.clear();

        // calculate upscaled elastic parameters for this depletion stage
        for(CPillarMap::const_iterator it = TargetPointsetBranch().begin(); it != TargetPointsetBranch().end(); ++it)
        {
          const CPillarPos& pos = it->first;
          const CPillar& pillar = it->second;
          CPillar::const_iterator itP;
          geo::CPoint ptPrev;
          // the sum of the used thicknesses
          double dSumThick = 0;
          // the weighted sum of the variables as outlined in the design document
          double v1 = 0; // <a>
          double v2 = 0; // <f*f/c>
          double v3 = 0; // <1/c>
          double v4 = 0; // <f/c>
          double v5 = 0; // <b>
          double v6 = 0; // <1/d>
          double v7 = 0; // <m>

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
                double dThickness = pt.Z() - ptPrev.Z();
                if(dThickness > 0)
                {
                  // get the S matrix
                  const geo::CMatrix& matS = GetSMatrix(*pBody, stage);
                  // get the values a, b, c, d, f and m for this body
                  double a = matS.Value(0, 0);
                  double b = matS.Value(0, 1);
                  double c = matS.Value(2, 2);
                  double d = matS.Value(4, 4);
                  double f = matS.Value(0, 2);
                  double m = matS.Value(3, 3);

                  // contribute to the sums
                  if(fabs(c) > 1e-12 && fabs(d) > 1e-12)
                  {
                    v1 += dThickness * a;
                    v2 += dThickness * f*f / c;
                    v3 += dThickness / c;
                    v4 += dThickness * f / c;
                    v5 += dThickness * b;
                    v6 += dThickness / d;
                    v7 += dThickness * m;

                    dSumThick += dThickness;
                  }
                }
              }
            }

            ptPrev = pt;
          }

          if(dSumThick > 1e-4)
          {
            // build the resulting S matrix
            double dA = (v1 - v2 + v4 * v4 / v3) / dSumThick;
            double dB = (v5 - v2 + v4 * v4 / v3) / dSumThick;
            double dC = 1 / (v3 / dSumThick);
            double dD = 1 / (v6 / dSumThick);
            double dF = dC * v4 / dSumThick;
            double dM = v7 / dSumThick;

            geo::CMatrix matS(6, 6);
            matS.Value(0, 0, dA);
            matS.Value(0, 1, dB);
            matS.Value(0, 2, dF);

            matS.Value(1, 0, dB);
            matS.Value(1, 1, dA);
            matS.Value(1, 2, dF);

            matS.Value(2, 0, dF);
            matS.Value(2, 1, dF);
            matS.Value(2, 2, dC);

            matS.Value(3, 3, dM);
            matS.Value(4, 4, dD);
            matS.Value(5, 5, dD);

            if(fabs(matS.GetDeterminant()) > 0)
            {
              // invert to C matrix
              geo::CMatrix matC = matS.GetInverse();
              BoundPoissonRatios(matC);
              VERIFY(mpPillarMatrix.insert(TPillarMatrixMap::value_type(pos, matC)).second);
            }
          }
        }
      }

      // compute upscaled pressures and temperatures
      for(CPillarMap::const_iterator it = TargetPointsetBranch().begin(); it != TargetPointsetBranch().end(); ++it)
      {
        prog.StatusMessage(QString("Computing pressures and temperatures for '%1'").arg(QString(stage.Name())));

        const CPillarPos& pos = it->first;
        const CPillar& pillar = it->second;
        CPillar::const_iterator itP;
        geo::CPoint ptPrev;

        // the sum of the used thicknesses
        double dSumThick = 0;

        // the sum of the thicknesses used for temperatures and pressures
        double dSumThickP = 0;
        double dSumThickT = 0;

        // thickness weighted sums of pressure and temperature
        double dSumP = 0;
        double dSumT = 0;

        // fetch the computed upscaled compliance matrix for this position
        TPillarMatrixMap::const_iterator itCMat = mpPillarMatrix.find(pos);
        if(itCMat != mpPillarMatrix.end())
        {
          const geo::CMatrix& matC = itCMat->second;
          assert(!matC.Empty());

          // the contributions to the values
          double dA = 0;
          double dD = 0;
          double dG = 0;
          double dH = 0;

          // upscaled expansion factors
          double dBetat = 0;
          double dBetan = 0;

          // the flattened (upscaled) absolute initial pressure and temperature for this position
          double dIniPressure = 0;
          double dIniTemper = 0;

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
                  dSumThick += di;

                  // midpoint of the segment
                  geo::CPoint ptMid((pt.X() + ptPrev.X()) / 2, (pt.Y() + ptPrev.Y()) / 2, (pt.Z() + ptPrev.Z()) / 2);

                  const CMeshBase& mesh = model.Mesh();
                  const CFormationBase& formation = *mesh.Formation(*pBody);
                  const CFFMaterial& mat = formation.Material(stage).Material(*pBody);

                  // Biot-alpha is 0 for undrained, otherwise 1
                  double alphai = 1;
                  if(formation.Material(stage).MaterialModel() == MM_UNDRAINED)
                    alphai = 0;

                  double betani;
                  double betati;
                  GetExpansionFactors(mat, betani, betati);

                  const geo::CMatrix& matCi = GetCMatrix(mat, *pBody);
                  double C11i = matCi.Value(0, 0);
                  double C12i = matCi.Value(0, 1);
                  double C13i = matCi.Value(0, 2);
                  double C23i = matCi.Value(1, 2);
                  double C33i = matCi.Value(2, 2);

                  double Cmi = -2 * C13i * C13i / (C11i + C12i) + C33i;

                  if(stage.Initial())
                  {
                    dIniPressure += GetLoadValue(*pBody, ptMid, formation.Pressure(stage)).Value() * di;
                    dIniTemper += GetLoadValue(*pBody, ptMid, formation.EffectiveTemperature(stage)).Value() * di;
                  }
                  else
                  {
                    const CFFMaterial&
                      ffMaterial = formation.Material(stage).Material(*pBody);

                    geo::CValue prevpressure = GetLoadValue(*pBody, ptMid, formation.Pressure(stage.Previous()));
                    geo::CValue pressure = GetLoadValue(*pBody, ptMid, formation.Pressure(stage));
                    geo::CValue prevtemper = GetLoadValue(*pBody, ptMid, formation.EffectiveTemperature(stage.Previous()));
                    geo::CValue temper = GetLoadValue(*pBody, ptMid, formation.EffectiveTemperature(stage));

                    double deltaPi = pressure.Value() - prevpressure.Value();
                    if(fabs(deltaPi) > EPS)
                    {
                      dSumThickP += di;
                      dSumP += deltaPi * di;
                    }

                    if(ffMaterial.IsParameter(IDT_VALUETYPE_BULKSTIFFNESS) &&
                       ffMaterial.IsParameter(IDT_VALUETYPE_GRAINSTIFFNESS))
                    {
                      double bulkModulus =
                        ffMaterial.ParameterValue(IDT_VALUETYPE_BULKSTIFFNESS);
                      double grainModulus =
                        ffMaterial.ParameterValue(IDT_VALUETYPE_GRAINSTIFFNESS);

                      if (grainModulus != 0)
                      {
                        deltaPi = (1 - bulkModulus / grainModulus) * deltaPi;
                      }
                    }

                    double deltaTi = temper.Value() - prevtemper.Value();
                    if(fabs(deltaTi) > EPS)
                    {
                      dSumThickT += di;
                      dSumT += deltaTi * di;
                    }

                    dA += alphai * deltaPi * di * (C11i + C12i + C13i) / (C11i + C12i);
                    dD += Cmi * alphai * deltaPi * di;
                    dG += (-betani + (C13i + C23i) / (C11i + C12i) * betati) * deltaTi * di;
                    dH += betati * deltaTi * di / (C11i + C12i);
                  }

                  dBetat += betati * di / (C11i + C12i);

                  double Ei = (C13i + C23i) / (C11i + C12i);
                  dBetan += (betani - Ei * betati) * di;
                }
              }
            }

            ptPrev = pt;
          }

          // finalize sum values
          double C11 = matC.Value(0, 0);
          double C12 = matC.Value(0, 1);
          double C13 = matC.Value(0, 2);
          double C23 = matC.Value(1, 2);
          double C33 = matC.Value(2, 2);
          double C55 = matC.Value(4, 4);

          double dB = C11 + C12 + C13;
          double dCm = -2 * C13 * C13 / (C11 + C12) + C33;
          double dE = (C13 + C23) / (C11 + C12);

          dBetat *= (C11 + C12) / dSumThick;
          dBetan /= dSumThick;
          dBetan += dE * dBetat;

          // fix for singularity when dBetat (almost) equals dBetan
          if(0.9 * dBetan < dBetat || dBetat < 1.1 * dBetan)
            dBetan *= (dBetat > dBetan ? 0.9 : 1.1);


          if(stage.IsPhaseStartStage())
          {
            // get upscaled anisotropic elastic values
            TUpscaledMatParams matparams;
            matparams.Et = 1 / C11;
            matparams.En = 1 / C33;
            matparams.nut = -C12 * matparams.Et;
            matparams.nun = -C13 * matparams.En;
            matparams.G = 1 / C55;
            matparams.betat = dBetat;
            matparams.betan = dBetan;
            mpMatParams.insert(TMatParamsMap::value_type(std::make_pair(pos, &stage), matparams));
          }

          TUpscaledLoadParams loadparams;
          if(stage.Initial())
          {
            dIniPressure /= dSumThick;
            dIniTemper /= dSumThick;
            loadparams.P = dIniPressure;
            loadparams.T = dIniTemper;
            loadparams.eps_n = 0;
            loadparams.eps_t = 0;
          }
          else
          {
            dA *= (C11 + C12) / dSumThick;
            dD /= -dSumThick;
            dG /= dSumThick;
            dH *= (C11 + C12) / dSumThick;

            // upscaled Biot alpha is always 1

            double dDeltaP = 0;
            if(dSumThickP > 0)
              dDeltaP = dSumP / dSumThickP;


            double dDeltaT = 0;
            if(dSumThickT > 0)
              dDeltaT = dSumT / dSumThickT;
            double dEpsT = -dA + dB * dDeltaP - dH + dBetat * dDeltaT;
            double dEpsN = dD + dCm * dDeltaP + dG + (dBetan - dE * dBetat) * dDeltaT + dE * dEpsT;

            // get previous values of P, T and psi
            TLoadParamsMap::const_iterator itLP = mpLoadParams.find(std::make_pair(pos, &stage.Previous()));
            assert(itLP != mpLoadParams.end());

            loadparams.P = itLP->second.P + dDeltaP;
            loadparams.T = itLP->second.T + dDeltaT;
            loadparams.eps_n = itLP->second.eps_n + dEpsN;
            loadparams.eps_t = itLP->second.eps_t + dEpsT;
          }
          mpLoadParams.insert(TLoadParamsMap::value_type(std::make_pair(pos, &stage), loadparams));
        }
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

    int nValueSetSize = nStartStages * sizeof(TUpscaledMatParams) / sizeof(double) + nStages * sizeof(TUpscaledLoadParams) / sizeof(double) - 2;
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
        s = getStringTableEntry(IDS_VALUENAME_YOUNGMODULUS_NORM);
        t = QString("%1_D%2").arg(s).arg(idxStage);
        pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_YOUNGMODULUS_NORM,  t)->Component());
        s = getStringTableEntry(IDS_VALUENAME_YOUNGMODULUS_TRANS);
        t = QString("%1_D%2").arg(s).arg(idxStage);
        pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_YOUNGMODULUS_TRANS, t)->Component());
        s = getStringTableEntry(IDS_VALUENAME_POISSONRATIO_NORM);
        t = QString("%1_D%2").arg(s).arg(idxStage);
        pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_POISSONRATIO_NORM, t)->Component());
        s = getStringTableEntry(IDS_VALUENAME_POISSONRATIO_TRANS);
        t = QString("%1_D%2").arg(s).arg(idxStage);
        pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_POISSONRATIO_TRANS, t)->Component());
        s = getStringTableEntry(IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS);
        t = QString("%1_D%2").arg(s).arg(idxStage);
        pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS, t)->Component());
        s = getStringTableEntry(IDS_VALUENAME_THERM_LIN_EXP_NORM);
        t = QString("%1_D%2").arg(s).arg(idxStage);
        pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_THERM_LIN_EXP_NORM, t)->Component());
        s = getStringTableEntry(IDS_VALUENAME_THERM_LIN_EXP_LAT);
        t = QString("%1_D%2").arg(s).arg(idxStage);
        pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_THERM_LIN_EXP_LAT, t)->Component());
      }

      s = getStringTableEntry(IDS_VALUENAME_PRESSURE);
      t = QString("%1_D%2").arg(s).arg(idxStage);
      pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_PRESSURE, t)->Component());
      s = getStringTableEntry(IDS_VALUENAME_TEMPERATURE);
      t = QString("%1_D%2").arg(s).arg(idxStage);
      pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_TEMPERATURE, t)->Component());
      if(!stage.Initial())
      {
        s = getStringTableEntry(IDS_VALUENAME_NRMSTRAIN);
        t = QString("%1_D%2").arg(s).arg(idxStage);
        pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_NRMSTRAIN, t)->Component());

        s = getStringTableEntry(IDS_VALUENAME_LATSTRAIN);
        t = QString("%1_D%2").arg(s).arg(idxStage);
        pPointset->NodalValueSet(++p).LinkTo(factory->BuildValueType(*pPointset, IDT_VALUETYPE_LATSTRAIN, t)->Component());
      }
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
        TMatParamsMap::const_iterator itMat = mpMatParams.find(std::make_pair(pos, &stage));
        if(itMat != mpMatParams.end())
        {
          vcRow[++n] = itMat->second.En;
          vcRow[++n] = itMat->second.Et;
          vcRow[++n] = itMat->second.nun;
          vcRow[++n] = itMat->second.nut;
          vcRow[++n] = itMat->second.G;
          vcRow[++n] = itMat->second.betan;
          vcRow[++n] = itMat->second.betat;
        }

        // the load parameters
        TLoadParamsMap::const_iterator itLoad = mpLoadParams.find(std::make_pair(pos, &stage));
        if(itLoad != mpLoadParams.end())
        {
          vcRow[++n] = itLoad->second.P;
          vcRow[++n] = itLoad->second.T;
          if(!stage.Initial())
          {
            vcRow[++n] = itLoad->second.eps_n;
            vcRow[++n] = itLoad->second.eps_t;
          }
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

geo::CValue CThinLayerUpscalingNode::GetLoadValue(const geo::IBody& body, const geo::IPoint& pt, const IValueComposite& loadcomposite) const
{
  IValueDomainScalar::TValueVec vcValues = loadcomposite.Component().ScalarData().ValueElement(body);
  return body.InterpolateValue(pt, vcValues);
}

bool CThinLayerUpscalingNode::CanExecute() const
{
  const TFormationBaseEntry* formationBaseEntry =
    dynamic_cast <const TFormationBaseEntry*> (
      Model().GraphEntry(MD_BASE_FORMATION));
  const TFormationBaseEntry::TNodeSet formations =
    formationBaseEntry->EntryNodes();
  const CDepletionStageEntry& depletionStageEntry =
    static_cast <const CModelBase&> (Model()).DepletionStageEntry();
  bool materialAssigned = true;
  bool fractureApertureModel = false;

  for (CDepletionStageEntry::const_iterator
    depletionStage = depletionStageEntry.begin();
    (depletionStage != depletionStageEntry.end()) && materialAssigned;
    ++depletionStage)
  {
    for (TFormationBaseEntry::TNodeSet::const_iterator
      formation = formations.begin();
      (formation != formations.end()) && materialAssigned; ++formation)
    {
      const CMaterialServer&
        materialServer = (*formation)->Material(*depletionStage);
      const IMaterialRock* libraryMaterial = materialServer.LibraryMaterial();
      int materialModel = (libraryMaterial == 0 ? MM_LINEAR :
        materialServer.MaterialModel());

      materialAssigned = materialAssigned && (libraryMaterial != 0);
      fractureApertureModel = fractureApertureModel ||
        (materialModel == MM_FRACTURE_APERTURE) ||
        (materialModel == MM_FRACTURE_APERTURE2);
    }
  }

  return !fractureApertureModel && materialAssigned &&
    FormationsBranch().HasFormations() && TargetPointsetBranch().HasPointset();
}

const geo::CMatrix& CThinLayerUpscalingNode::GetCMatrix(const CFFMaterial& mat, const geo::IBody& /*body*/) const
{
  TMat2MatrixMap::iterator it = m_mpMat2CMatrix.find(mat);
  if(it == m_mpMat2CMatrix.end())
  {
    geo::CMatrix matC(6, 6);
    BuildCMatrix(matC, mat);
    it = m_mpMat2CMatrix.insert(TMat2MatrixMap::value_type(mat, matC)).first;
  }

  return it->second;
}

const geo::CMatrix& CThinLayerUpscalingNode::GetSMatrix(const geo::IBody& body, const CDepletionStage& stage) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CFormationBase& formation = *model.Mesh().Formation(body);
  const CFFMaterial& mat = formation.Material(stage).Material(body);

  TMat2MatrixMap::iterator it = m_mpMat2SMatrix.find(mat);
  if(it == m_mpMat2SMatrix.end())
  {
    // build the S matrix and add it to the map
    const geo::CMatrix& matC = GetCMatrix(mat, body);
    it = m_mpMat2SMatrix.insert(TMat2MatrixMap::value_type(mat, matC.GetInverse())).first;
  }

  return it->second;
}

void CThinLayerUpscalingNode::BuildCMatrix(geo::IMatrix& mat, const CFFMaterial& ffmat) const
{
  assert(mat.RowSize()    == 6); // requires pre-allocated matrix
  assert(mat.ColumnSize() == 6);

  double En;
  double Et;
  double nun;
  double nut;

  if(ffmat.IsParameter(IDT_VALUETYPE_YOUNGMODULUS_NORM))
    En = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM);
  else if(ffmat.IsParameter(IDT_VALUETYPE_UNDRAINEDYOUNG))
    En = ffmat.ParameterValue(IDT_VALUETYPE_UNDRAINEDYOUNG);
  else
    En = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS);

  if(ffmat.IsParameter(IDT_VALUETYPE_YOUNGMODULUS_TRANS))
    Et = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS);
  else if(ffmat.IsParameter(IDT_VALUETYPE_UNDRAINEDYOUNG))
    Et = ffmat.ParameterValue(IDT_VALUETYPE_UNDRAINEDYOUNG);
  else
    Et = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS);

  if(ffmat.IsParameter(IDT_VALUETYPE_POISSONRATIO_NORM))
    nun = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM);
  else if(ffmat.IsParameter(IDT_VALUETYPE_UNDRAINEDPOISSON))
    nun = ffmat.ParameterValue(IDT_VALUETYPE_UNDRAINEDPOISSON);
  else
    nun = ffmat.ParameterValue(IDT_VALUETYPE_POISSONS_RATIO);

  if(ffmat.IsParameter(IDT_VALUETYPE_POISSONRATIO_TRANS))
    nut = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS);
  else if(ffmat.IsParameter(IDT_VALUETYPE_UNDRAINEDPOISSON))
    nut = ffmat.ParameterValue(IDT_VALUETYPE_UNDRAINEDPOISSON);
  else
    nut = ffmat.ParameterValue(IDT_VALUETYPE_POISSONS_RATIO);

  double G = En / (2 * (1 + nun));

  // row 0
  mat.Value(0, 0, 1 / Et);
  mat.Value(0, 1, -nut / Et);
  mat.Value(0, 2, -nun / En);

  // row 1
  mat.Value(1, 0, -nut / Et);
  mat.Value(1, 1, 1 / Et);
  mat.Value(1, 2, -nun / En);

  // row 2
  mat.Value(2, 0, -nun / En);
  mat.Value(2, 1, -nun / En);
  mat.Value(2, 2, 1 / En);

  // row 3
  mat.Value(3, 3, 2 * (1 + nut) / Et);

  // row 4
  mat.Value(4, 4, 1 / G);

  // row 5
  mat.Value(5, 5, 1 / G);
}

bool CThinLayerUpscalingNode::IsTarget(const geo::IBody& body) const
{
  return FormationsBranch().IsInRange(body) && PointsetRangesBranch().IsInRange(body.MidPoint());
}

void CThinLayerUpscalingNode::GetExpansionFactors(const CFFMaterial& mat, double& betan, double& betat) const
{
  if(mat.IsParameter(IDT_VALUETYPE_THERM_LIN_EXP_NORM))
    betan = mat.ParameterValue(IDT_VALUETYPE_THERM_LIN_EXP_NORM);
  else if(mat.IsParameter(IDT_VALUETYPE_UNDRAINEDTHERMX))
    betan = mat.ParameterValue(IDT_VALUETYPE_UNDRAINEDTHERMX) / 3;
  else
    betan = mat.ParameterValue(IDT_VALUETYPE_THERMAL_EXPANSION) / 3;

  if(mat.IsParameter(IDT_VALUETYPE_THERM_LIN_EXP_LAT))
    betat = mat.ParameterValue(IDT_VALUETYPE_THERM_LIN_EXP_LAT);
  else if(mat.IsParameter(IDT_VALUETYPE_UNDRAINEDTHERMX))
    betat = mat.ParameterValue(IDT_VALUETYPE_UNDRAINEDTHERMX) / 3;
  else
    betat = mat.ParameterValue(IDT_VALUETYPE_THERMAL_EXPANSION) / 3;
}

void CThinLayerUpscalingNode::BoundPoissonRatios(geo::IMatrix& matC) const
{
  double dEt = 1 / matC.Value(0, 0);
  double dEn = 1 / matC.Value(2, 2);
  double dnut = -matC.Value(0, 1) * dEt;
  double dnun = -matC.Value(0, 2) * dEn;

  bool bMod = false;

  if(dnut < 0)
  {
    dnut = 0;
    bMod = true;
  }
  else if(dnut > 0.999 * sqrt(dEn / dEt))
  {
    dnut = 0.999 * sqrt(dEn / dEt);
    bMod = true;
  }

  if(dnun < 0)
  {
    dnun = 0;
    bMod = true;
  }
  else if(dnun > 0.999 * sqrt((1 - dnut) * dEn / (2 * dEt)))
  {
    dnun = 0.999 * sqrt((1 - dnut) * dEn / (2 * dEt));
    bMod = true;
  }

  if(bMod)
  {
    // update the matrix
    double dGinv = 2 * (1 + dnun) / dEn;
    matC.Value(0, 1, -dnut / dEt);
    matC.Value(0, 2, -dnun / dEn);
    matC.Value(1, 0, -dnut / dEt);
    matC.Value(1, 2, -dnun / dEn);
    matC.Value(2, 0, -dnun / dEn);
    matC.Value(2, 1, -dnun / dEn);
    matC.Value(3, 3, 2 * (1 + dnut) / dEt);
    matC.Value(4, 4, dGinv);
    matC.Value(5, 5, dGinv);
  }
}