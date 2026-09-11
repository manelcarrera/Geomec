
#include "SavePropertyIsReservoir.h"
#include "RGInterface.h"
#include "propertyValues.h"
#include "ModelBase.h"
#include "IInterfaceElement.h"
#include "FormationBase.h"
#include "ExportResultDataEntry.h"
#include "ExportResultData.h"
#include "BaseEntryTypes.h"
#include "ModelBase.h"
#include "RockMechProcessor.h"

namespace GeomecRGI
{

	CSavePropertyIsReservoir::CSavePropertyIsReservoir(const RGProperty& rgProperty)
		: CSavePropertyBase(rgProperty)
	{
	}

	CSavePropertyIsReservoir::~CSavePropertyIsReservoir()
	{
	}

	bool CSavePropertyIsReservoir::saveProperty(RGInterface& rgi,
		CModelBase& modelBase, const CRockMechProcessor& rmp)
	{
    // find reservoirs from the reservoir macro in the model
    CExportResultDataEntry *pEntry = dynamic_cast<CExportResultDataEntry *>(modelBase.GraphEntry(MD_BASE_EXPORT_MACROS));
    CExportResultData *exportResultData = pEntry ? pEntry->retrieveReservoirMacro() : 0;

    std::set<const CFormationBase *> reservoirs;

    if (exportResultData)
    {
      exportResultData->bind();
      for (COpenGLNodeSelection::iterator node = exportResultData->m_selection.begin(); node != exportResultData->m_selection.end(); ++node)
      {
        if (dynamic_cast<const CFormationBase *>(*node))
        {
          reservoirs.insert(static_cast<const CFormationBase *>(*node));
        }
      }
      exportResultData->unbind(false);

      if (reservoirs.empty())
        rmp.AddLogLine("Selection macro of reservoirs defined, but no formations are selected", &rgi, false, false);
    }
    else
    {
      rmp.AddLogLine("No selection macro of reservoirs defined", &rgi, false, true);
    }


    // and check each element's reservoir against the set
		std::vector<int> values(modelBase.Mesh().Mesh().ElementSize(), -9999);

		for (int e = 0; e < modelBase.Mesh().Mesh().ElementSize(); ++e)
		{
			const geo::IElement& element = modelBase.Mesh().Mesh().Element(e);

			if (!element.IsInterfaceElement())
			{
				const CFormationBase* formationBase = modelBase.Mesh().Formation(element);
				values[e] = (reservoirs.find(formationBase) != reservoirs.end() ? 1 : 0);
			}
		}

		propertyValuesMayNotContainNull(m_RGProperty.getType(), values);
		rgi.saveProperty(m_RGProperty, values);

		return true;
	}

} // namespace GeomecRGI
