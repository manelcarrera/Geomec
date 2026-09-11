
#include "ModelBase.h"
#include "ResultRegister.h"
#include "GlobalTemperature.h"
#include "Temperature.h"
#include "FemAppModel.h"
#include "resourceIDI.h"
#include "StreamVersion.h"
#include "ValueTypeFactory.h"

CGlobalTemperature::CGlobalTemperature(CFemAppModel& model)
: CGlobalFormationLoad<TTemperature, CTemperatureGradientComponent>("Global Temperature", model)
{
	new CTemperatureGradientComponent(*this);
	Constant().Set(15, 0, 0.02, CQuantity::SI_UNIT);
	reParent(&model);
}

CGlobalTemperature::CGlobalTemperature(const CGlobalTemperature& rhs)
: CGlobalFormationLoad<TTemperature, CTemperatureGradientComponent>(rhs)
{
	new CTemperatureGradientComponent(rhs.Constant());
}

unsigned int CGlobalTemperature::IconId() const
{
	return IDI_INITIAL_TEMPERATURE;
}

void CGlobalTemperature::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	if(CStreamVersion(3, 0, 85) < version)
	{
		CGlobalFormationLoad<TTemperature, CTemperatureGradientComponent>::LoadStream(stream, version, progress);
	}
	else if(CStreamVersion(3, 0, 34) < version)
	{
		IValueComposite::LoadStream(stream, version, progress);
		new CTemperatureGradientComponent(*this);

		double dDepth;
		double dTemp;
		double dGradient;
		stream >> dDepth;
		stream >> dTemp;
		stream >> dGradient;
		Constant().Set(dTemp, dDepth, dGradient, CQuantity::SI_UNIT);

		DistributedOnly(false);
	}

	// make sure name is right...
	Name("Global Temperature");
}

void CGlobalTemperature::OnNewNeighbour(const CGraphNode& node)
{
  if(dynamic_cast<const TTemperature*>(&node))
  {
    CModelBase* pModel = static_cast<CModelBase*> (&Model());
	  pModel->ResultRegister().ClearAll();
  }

  CGlobalFormationLoad<TTemperature, CTemperatureGradientComponent>::OnNewNeighbour(node);
}

void CGlobalTemperature::OnNeighbourDeleted(const CGraphNode& node)
{
  if(dynamic_cast<const TTemperature*>(&node))
  {
    CModelBase* pModel = dynamic_cast<CModelBase*> (&Model());
    if(!pModel->getDeleting())
	    pModel->ResultRegister().ClearAll();
  }

  CGlobalFormationLoad<TTemperature, CTemperatureGradientComponent>::OnNeighbourDeleted(node);
}
