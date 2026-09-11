 // Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CFORMATION_3B66583300AC_INCLUDED
#define _INC_CFORMATION_3B66583300AC_INCLUDED



#include "IComponent.h"
#include "Dependency.h"
#include "ValueProfile.h"

class CDataModel;
class CFormation;
class CFormationMaterial;

//##ModelId=3C0DDDAD005E
class CFormation 
: public IComponent
{
friend CDependency;

private:

	//##ModelId=3C60D567008E
	CDataModel* m_pDataModel;
	//##ModelId=3C2043F10271
	void InitQuantities();
	//##ModelId=3C0DDDAD007F
	void CreateDependencies();
	//##ModelId=3C0DDDAD0080
	void DeleteDependencies();

	//##ModelId=3CAAA659013C
	CLengthQuantity m_qBottomTVD;
	//##ModelId=3CAAA659015C
	CTemperatureGradientQuantity m_qGeothermalGradient;
	//##ModelId=3CAAA659018A
	CNoneQuantity m_qMaxHorizontalStressRatio;
	//##ModelId=3CAAA65901AA
	CNoneQuantity m_qMinHorizontalStressRatio;
	//##ModelId=3CAAA65901C9
	CAngleQuantity m_qAzimuthOfSHWithNorth;
	//##ModelId=3CAAA65901E8
	CSinglePressure m_qTopPorePressure;
	//##ModelId=3CAAA6590207
	CSinglePressure m_qBottomPorePressure;
	
	//##ModelId=3C0DDDAD011A
	bool m_bPorous;
	
	//##ModelId=3C0DDDAD0129
	CString m_strFormationName;
	//##ModelId=3C0DDDAD012A
	CString m_strMaterialName;
	//##ModelId=3C60D56701D5
	class CFormationDependency1: public CDependency
	{
		public:
	
		//##ModelId=3C60D5670204
		CFormationDependency1(CFormation* form);
		//##ModelId=3C60D5670206
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const ;
		//##ModelId=3C60D5670209
		virtual std::string GetDependencyString() const;
		
	};


	//##ModelId=3DDA0C450284
	class CFormationDependency2: public CDependency
	{
		private:
		//##ModelId=3DDA0C450296
			CFormation* m_pFormation;
		//##ModelId=3DDA0C4502A3
			mutable std::string m_DependencyString;
		public:
		//##ModelId=3DDA0C4502A4
			CFormationDependency2(CFormation* form);
		//##ModelId=3DDA0C4502A6
			virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const ;
		//##ModelId=3DDA0C4502A9
			virtual std::string GetDependencyString() const;
	};


	//##ModelId=3DDA0C4502C2
	class CFormationDependency3: public CDependency
	{
		private:
		//##ModelId=3DDA0C4502D4
			CFormation* m_pFormation;

			mutable std::string m_DependencyString;

		public:
		//##ModelId=3DDA0C4502D8
			CFormationDependency3(CFormation* form);
		//##ModelId=3DDA0C4502E3
			virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const ;
		//##ModelId=3DDA0C4502E6
			virtual std::string GetDependencyString() const;
	};

	friend CFormationDependency1;
	friend CFormationDependency2;
	friend CFormationDependency3;

	//##ModelId=3CAAA6590227
	CFormationDependency1* m_pFormationDependency1;
	//##ModelId=3DDA0C450208
	CFormationDependency2* m_pFormationDependency2;
	//##ModelId=3DDA0C450218
	CFormationDependency3* m_pFormationDependency3;

public:
  typedef CValueProfile::TDepthValueVector TDepthValueVector;

private:
  TDepthValueVector HorStresses(double dK) const;
  double ValueAtDepth(const TDepthValueVector& vcValues, double dDepth) const;
	
public:

	//##ModelId=3C0DDDAD013A
	CFormation(CString Name,double bottomTVD,CDataModel* pDataModel);

	//##ModelId=3C0DDDAD014A
	~CFormation();

	//##ModelId=3C0DDDAD0158
	bool operator < (const CFormation& rhs) const;
	//##ModelId=3C0DDDAD015B
	CFormation& operator = (const CFormation& rhs);
	//##ModelId=3C0DDDAD0169
	CFormation(const CFormation& rhs);


	//const definition of BottomTVD
	//From: Formation definition
	//Label: bottom TVD
	//Descr: Formation bottom TVD, only for formation
	//##ModelId=3C0DDDAD0187
	const CLengthQuantity* BottomTVD() const
	{
		return &m_qBottomTVD;
	}
	//##ModelId=3C0DDDAD0189
	CLengthQuantity* BottomTVD()
	{
		return &m_qBottomTVD;
	}

	//From: Formation Definition
	//Label: Formation name
	//Descr: Unique identifier for formation
	//##ModelId=3C0DDDAD018A
	CString FormationName() const
	{
		return m_strFormationName;	
	}

	//##ModelId=3C0DDDAD0198
	void FormationName(CString strFormationName)
	{
		m_strFormationName= strFormationName;
	}
	
	//From: Insitu stresses
	//Label: Geothermal gradient
	//Descr: Geothermal gradient
	//##ModelId=3C0DDDAD019A
	CTemperatureGradientQuantity* GeothermalGradient();
	const CTemperatureGradientQuantity* GeothermalGradient() const;
		
	//From: Insitu Stresses
	//Label: maximum horizontal stress ratio (SH)
	//Descr: maximum initial stress ratio in formation
	//##ModelId=3C0DDDAD01A7
	CNoneQuantity *MaxHorizontalStressRatio();
	const CNoneQuantity *MaxHorizontalStressRatio() const;

	//From: Insitu Stresses
	//Label: Minimum horizontal stress ratio (Sh)
	//Descr: Minimum initial stress ratio in formation
	//##ModelId=3C0DDDAD01A8
	CNoneQuantity *MinHorizontalStressRatio();
	const CNoneQuantity *MinHorizontalStressRatio() const;

	//From: Insitu Stresses
	//Label: Azimuth of SH with North
	//Descr: Azimuth angle of maximum horizontal stress with 
	//North in formation
	//##ModelId=3C0DDDAD01A9
	CAngleQuantity *AzimuthOfSHWithNorth();
	const CAngleQuantity *AzimuthOfSHWithNorth() const;
	
	//From: Insitu Stresses
	//Label: 'check box'
	//Descr: true is formation is porous
	//##ModelId=3C0DDDAD01AA
	bool Porous() const;
	//##ModelId=3C0DDDAD01B6
	void Porous(bool bPorous);
	
	//From: Insitu Stresses
	//Label: Top pore pressure
	//Descr: pore pressure at reservoir top if bPorous=true
	//##ModelId=3C0DDDAD01B8
	CSinglePressure *TopPorePressure();
	const CSinglePressure *TopPorePressure() const;

	//From: Insitu Stresses
	//Label: Bottom pore pressure
	//Descr: pore pressure at reservoir bottom if bPorous=true
	//##ModelId=3C0DDDAD01B9
	CSinglePressure *BottomPorePressure();
	const CSinglePressure *BottomPorePressure() const;

  // uses value profile if it exists, user-defined values otherwise
  double GetActualTopPorePressure();
  double GetActualBottomPorePressure();

  std::vector<std::pair<double, double> > PorePressureValues() const;

  double GetPorePressureAt(double dDepth) const;

	//pointer to a material name in the library
	//##ModelId=3C0DDDAD01C6
	CString MaterialName() const
	{
		return m_strMaterialName;	
	}

	//##ModelId=3C0DDDAD01C8
	void MaterialName(CString strMaterialName)
	{
		m_strMaterialName= strMaterialName;
	}

  const CFormationMaterial* Material() const;
  CFormationMaterial* Material();

  // depth and stress at that depth
  TDepthValueVector VerticalStresses() const;
  TDepthValueVector MaxHorStresses() const;
  TDepthValueVector MinHorStresses() const;
  TDepthValueVector Temperatures() const;

  double VerticalStressAtDepth(double dDepth) const;
  double MaxHorizontalStressAtDepth(double dDepth) const;
  double MinHorizontalStressAtDepth(double dDepth) const;
  double TemperatureAtDepth(double dDepth) const;

	//##ModelId=3C8D9DFC0040
//	double GetGeoThermalTemperatureAtTop() const;
	//##ModelId=3C8D9DFC00BD
//	double GetGeoThermalTemperatureAtBottom() const;

#if 0 // TODOwedx
	//##ModelId=3C8D9DFC02FF
	double GetEffectiveVerticalStressAtBottom() const;
	//##ModelId=3C8D9DFC036C
	double GetEffectiveVerticalStressAtTop() const;
	//##ModelId=3C8D9DFD0001
	double GetEffectiveMaxHorStressAtBottom() const;
	//##ModelId=3C8D9DFD006F
	double GetEffectiveMaxHorStressAtTop() const;
	//##ModelId=3C8D9DFD00DC
	double GetEffectiveMinHorStressAtBottom() const;
	//##ModelId=3C8D9DFD0159
	double GetEffectiveMinHorStressAtTop() const;
#endif // TODOwedx
	//##ModelId=3C8D9DFD01C6
	double GetHeight() const;
	//##ModelId=3C8D9DFD0234
	double GetTopTVD() const;

	//##ModelId=3C0DDDAD01D9
	bool CheckComplete() const;

	//##ModelId=3C0DDDAD01E5
	CString GetInfo();

	//##ModelId=3C2043F102FE
	bool CheckInsituStressesComplete() const;
	
	
};


#endif /* _INC_CFORMATION_3B66583300AC_INCLUDED */
