#include <cmath>

#include "GVTAnalysis.h"

#include "GVTData.h"
#include "GVTVelocityModel.h"
#include "GlobalMessage.h"
#include "IProgressBase.h"
#include "MPKernel.h"
#include "ModelBase.h"

CGVTAnalysis::CGVTAnalysis() {}

class CTaskCalculate {
  double m_dWaterSurfaceDepth;

  CGVTDataInput &m_input;
  CGVTDataOutput &m_output;

  unsigned int m_key;
  unsigned int m_d0;
  unsigned int m_d1;

  mp::CKernelDispatcher *m_dispatcher;

  bool m_bSequential;

public:
  CTaskCalculate(double dWaterSurfaceDepth, CGVTDataInput &input, CGVTDataOutput &output, unsigned int key)
      : m_dWaterSurfaceDepth(dWaterSurfaceDepth), m_input(input), m_output(output), m_key(key), m_d0(key >> 16),
        m_d1(key & 0xffff), m_dispatcher(0), m_bSequential(false) {}

  MP_DUMMY_DATA

  size_t size() const { return m_input.CalculationSize(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void setSequential() { m_bSequential = true; }

  void CalculatePillar(double waterDepth, const CGVTDataTypes::TPillarVelocityModelVector &velocityModels,
                       CGVTDataTypes::TValueMap &input_0, CGVTDataTypes::TValueMap &input_1,
                       CGVTDataTypes::TValueMap &output, size_t pillar) const;

  void operator()(const tbb::blocked_range<size_t> &r) const {
    for (size_t i = r.begin(); i != r.end(); ++i) {
      CalculatePillar(m_dWaterSurfaceDepth, const_cast<const CGVTDataInput &>(m_input).VelocityModels(),
                      m_input.ValueMap(m_d0), m_input.ValueMap(m_d1), m_output.ValueMap(m_key), i);

      if ((i % 100) == 0 && m_dispatcher && m_bSequential)
        m_dispatcher->Step(100);
    }

    if (m_dispatcher)
      m_dispatcher->Step(m_bSequential ? r.size() % 100 : r.size());
  }
};

void CTaskCalculate::CalculatePillar(double waterDepth, const CGVTDataTypes::TPillarVelocityModelVector &velocityModels,
                                     CGVTDataTypes::TValueMap &input_0, CGVTDataTypes::TValueMap &input_1,
                                     CGVTDataTypes::TValueMap &output, size_t pillar) const {
  CGVTDataTypes::TPillarPointValueVector::iterator dist = output[CGVTDataTypes::LENGTH].second[pillar].begin();
  CGVTDataTypes::TPillarPointValueVector::iterator out_vstrain = output[CGVTDataTypes::VSTRAIN].second[pillar].begin();

  // Calculate 'VStrain' based on R- and S-models, and deltaV
  size_t i = 0;
  for (CGVTDataTypes::TPillarPointVelocityModelVector::const_iterator velocityModel = velocityModels[pillar].begin();
       velocityModel != velocityModels[pillar].end(); ++velocityModel, ++out_vstrain, ++i) {
    double dVStrain = 0;

    switch ((*velocityModel)->ModelType()) {
    case RFactor: {
      bool valid_0 =
          input_0[CGVTDataTypes::StrainZZ].second.size() && input_0[CGVTDataTypes::StrainZZ].second[pillar][i].Valid();
      bool valid_1 = input_1[CGVTDataTypes::StrainZZ].second[pillar][i].Valid();

      if (valid_1 && valid_0)
        dVStrain = input_1[CGVTDataTypes::StrainZZ].second[pillar][i].Value() -
                   input_0[CGVTDataTypes::StrainZZ].second[pillar][i].Value();
      else if (valid_1)
        dVStrain = input_1[CGVTDataTypes::StrainZZ].second[pillar][i].Value();

      if (dVStrain < 0)
        dVStrain *= (*velocityModel)->Parameter(RFactor_EX);
      else
        dVStrain *= (*velocityModel)->Parameter(RFactor_C);
    } break;
    case SFactor: {
      bool valid_0 = input_0[CGVTDataTypes::MeanStress].second[pillar][i].Valid() &&
                     input_0[CGVTDataTypes::StressZZ].second[pillar][i].Valid();
      bool valid_1 = input_1[CGVTDataTypes::MeanStress].second[pillar][i].Valid() &&
                     input_1[CGVTDataTypes::StressZZ].second[pillar][i].Valid();

      if (valid_1 && valid_0) {
        dVStrain = input_0[CGVTDataTypes::MeanStress].second[pillar][i].Value() -
                   input_1[CGVTDataTypes::MeanStress].second[pillar][i].Value(); // Note sign-reversal
        dVStrain *= (*velocityModel)->Parameter(SFactor_NB);
        dVStrain += input_1[CGVTDataTypes::StressZZ].second[pillar][i].Value() -
                    input_0[CGVTDataTypes::StressZZ].second[pillar][i].Value();
        dVStrain *= (*velocityModel)->Parameter(SFactor_S);
      }
    } break;
    }

    out_vstrain->Value(dVStrain);
  }

  // reset iterator and initialize the others
  out_vstrain = output[CGVTDataTypes::VSTRAIN].second[pillar].begin();

  // CGVTDataTypes::TPillarPointValueVector::iterator out_tstrain =
  // output[CGVTDataTypes::TSTRAIN].second[pillar].begin();
  CGVTDataTypes::TPillarPointValueVector::iterator out_timeshift =
      output[CGVTDataTypes::TIMESHIFT].second[pillar].begin();
  CGVTDataTypes::TPillarPointValueVector::iterator out_deltav = output[CGVTDataTypes::DELTAV].second[pillar].begin();

  CGVTDataTypes::TPillarPointValueVector::const_iterator dist_end = output[CGVTDataTypes::LENGTH].second[pillar].end();

  // calculate for the first point

  i = 0;

  double TWT = 0; // two-way traveltime
  double deltaTWT = 0;

  if (dist != dist_end) {
    bool valid_0 =
        input_0[CGVTDataTypes::StrainZZ].second.size() && input_0[CGVTDataTypes::StrainZZ].second[pillar][i].Valid();
    bool valid_1 = input_1[CGVTDataTypes::StrainZZ].second[pillar][i].Valid();
    bool valid_2 = out_vstrain->Valid();
    bool valid_3 = input_0[CGVTDataTypes::Displacement].second.size() &&
                   input_0[CGVTDataTypes::Displacement].second[pillar][i].Valid();

    double Vp = input_0[CGVTDataTypes::Velocity].second[pillar][i].Value();
    double VStrain = 0;

    if (valid_2)
      VStrain = out_vstrain->Value();

    out_deltav->Value(Vp * VStrain); // DELTAV

    double TStrain = 0;

    double depth = dist->Value();

    if (depth > waterDepth) {

      if (valid_2 && valid_1) {
        if (valid_0)
          TStrain = -(input_1[CGVTDataTypes::StrainZZ].second[pillar][i].Value() -
                      input_0[CGVTDataTypes::StrainZZ].second[pillar][i].Value());
        else
          TStrain = -input_1[CGVTDataTypes::StrainZZ].second[pillar][i].Value();

        TStrain -= VStrain;
      }

      double subsidence = input_1[CGVTDataTypes::Displacement].second[pillar][i].Value();
      if (valid_3)
        subsidence -= input_1[CGVTDataTypes::Displacement].second[pillar][i].Value();

      deltaTWT = 2 * subsidence / 1500 /* m/s */;
    }

    out_timeshift->Value(deltaTWT * TStrain); // DELTAT

    ++dist;
    ++out_vstrain;
    //++out_tstrain;
    ++out_timeshift;
    ++out_deltav;
    ++i;
  }

  while (dist != dist_end) {
    bool valid_0 =
        input_0[CGVTDataTypes::StrainZZ].second.size() && input_0[CGVTDataTypes::StrainZZ].second[pillar][i].Valid();
    bool valid_1 = input_1[CGVTDataTypes::StrainZZ].second[pillar][i].Valid();
    bool valid_2 = out_vstrain->Valid();

    double Vp = input_0[CGVTDataTypes::Velocity].second[pillar][i].Value();
    double len = dist->Value() - (dist - 1)->Value();
    double VStrain = 0;

    if (valid_2)
      VStrain = out_vstrain->Value();

    out_deltav->Value(Vp * VStrain); // DELTAV

    TWT = 2 * len / Vp;

    double TStrain = 0;

    if (valid_2 && valid_1) {
      if (valid_0)
        TStrain = -(input_1[CGVTDataTypes::StrainZZ].second[pillar][i].Value() -
                    input_0[CGVTDataTypes::StrainZZ].second[pillar][i].Value());
      else
        TStrain = -input_1[CGVTDataTypes::StrainZZ].second[pillar][i].Value();

      TStrain -= out_vstrain->Value();
    }

    deltaTWT += TStrain * TWT;

    out_timeshift->Value(deltaTWT);

    ++dist;
    ++out_vstrain;
    //++out_tstrain;
    ++out_timeshift;
    ++out_deltav;
    ++i;
  }
}

void CGVTAnalysis::Calculate(IProgressBase &progress, CGVTData &data) {
  if (data.Output().Empty())
    return; // nothing to do

  progress.StatusMessage("Calculating...");
  progress.AddSteps(data.Output().CalculationSize());

  typedef CTaskCalculate CTaskCollect;

  std::vector<unsigned int> keys;
  data.Output().GetKeys(keys);

  for (std::vector<unsigned int>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
    CTaskCollect taskCollect(data.WaterSurfaceDepth(), data.Input(), data.Output(), *it);

    mp::CKernelDispatcher kernelDispatcher;

#if 0
  // if debugging:
  taskCollect.setSequential();
  mp::CKernelSequential<CTaskCollect> seqKernelCollect;
  mp::IDispatchedTask *disTaskCollect = NEW_DISPATCH_TASK(mp::CKernelSequential, CTaskCollect)(seqKernelCollect, taskCollect);
#else
    mp::CKernelParallel<CTaskCollect> parKernelCollect;
    mp::IDispatchedTask *disTaskCollect =
        NEW_DISPATCH_TASK(mp::CKernelParallel, CTaskCollect)(parKernelCollect, taskCollect);
#endif

    kernelDispatcher.launch(progress, disTaskCollect);
  }
}