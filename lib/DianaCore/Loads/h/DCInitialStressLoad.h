// DCInitialStressLoad.h: interface for the CInitialStressLoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCINITIALSTRESSLOAD_H__C7D0E563_5549_49F9_B944_932AE646E3CA__INCLUDED_)
#define AFX_DCINITIALSTRESSLOAD_H__C7D0E563_5549_49F9_B944_932AE646E3CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mfTensor.h"
#include "IDCStructuralLoad.h"

namespace dia {

class CInitialStressLoad : public IStructuralLoad  
{
	const geo::IElement &m_element;
	std::vector<CTensor> *m_pvcTensors;
	std::vector<geo::CVector> *m_pvcVectors;
public:
	// same value in all nodes (tensor)
	CInitialStressLoad(CLoadCase &loadcase, const ITensor &values, const geo::IElement &element);
	// same value in all nodes (vector)
	CInitialStressLoad(CLoadCase &loadcase, const geo::IVector &values, const geo::IElement &element);

	// vector of tensor values for each node (vcValues.size() must equal to body.NrOfPoints())
	CInitialStressLoad(CLoadCase &loadcase, const std::vector<CTensor> &vcValues, const geo::IElement &element);
	// vector of vector values for each node (vcValues.size() must equal to body.NrOfPoints())
	CInitialStressLoad(CLoadCase &loadcase, const std::vector<geo::CVector> &vcValues, const geo::IElement &element);
	virtual ~CInitialStressLoad();

	int ValueSize() const;
	const ITensor &ValueTensor(int nIndex) const;
	const geo::IVector &ValueVector(int nIndex) const;

	virtual bool WriteFilos() const;
	const geo::IElement &Element() const;
};

}

#endif // !defined(AFX_DCINITIALSTRESSLOAD_H__C7D0E563_5549_49F9_B944_932AE646E3CA__INCLUDED_)
