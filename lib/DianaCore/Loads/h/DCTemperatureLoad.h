// DCTemperatureLoad.h: interface for the CTemperatureLoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCTEMPERATURELOAD_H__04EE394D_DE4C_4335_9235_F82010B52641__INCLUDED_)
#define AFX_DCTEMPERATURELOAD_H__04EE394D_DE4C_4335_9235_F82010B52641__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IDCStructuralLoad.h"

namespace dia {

class CLoadManager;

class CTemperatureLoad : public IStructuralLoad  
{
public:
	// same value in all nodes
	CTemperatureLoad(CLoadCase &loadcase, double size, const geo::IElement &element);

	// vector of values for each node (vcValues.size() must equal to body.NrOfPoints())
	CTemperatureLoad(CLoadCase &loadcase, const std::vector<double> &vcValues, const geo::IElement &element);
	virtual ~CTemperatureLoad();

	virtual bool WriteFilos() const;
	const geo::IElement &Element() const;

private:
	const geo::IElement& m_Element;
};

} // namespace dia

#endif // !defined(AFX_DCTEMPERATURELOAD_H__04EE394D_DE4C_4335_9235_F82010B52641__INCLUDED_)
