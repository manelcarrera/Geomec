// KeyFile.h: interface for the CKeyFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYFILE_H__C3933D75_2457_4EFF_8745_254A4172235F__INCLUDED_)
#define AFX_KEYFILE_H__C3933D75_2457_4EFF_8745_254A4172235F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// this define makes that the quadratic elements implementation is hidden behind the keyfile mechanism
// remove it if this is not needed
//#define QUADRATIC_KEYFILE

// tetra models behind key
//#define TETRA_KEYFILE

// tetra faults behind key
//#define TETRA_FAULTS_KEYFILE

// tetra super model behind key
#define TETRA_SUPERMODEL_KEYFILE

// Eclipse behind key
#define ECLIPSE_KEYFILE

// Element to element mapping behind key
#define MAPPING_KEYFILE

// Large deformations behind key
//#define LARGEDEF_KEYFILE

// Cam-clay material model behind key
//#define CAMCLAY_KEYFILE

// Fokker-creep material model behind key
//#define FOKKER_KEYFILE

// Fist export behind key
//#define FIST_KEYFILE

// DCasint behind key
#define DCASINT_KEYFILE

// 2D Model behind key
//#define MODEL2D_KEYFILE

// HomoBox behind keyfile
//#define HOMOBOX_KEYFILE

// Zoom-in functionallity behind key
//#define ZOOMIN_KEYFILE

// Result import key
//#define RESULTS_IMPORT_KEYFILE

// Fracture aperture material model
//#define FRACTURE_APERTURE_KEYFILE

class CKeyFile  
{
private:
	bool m_bUnlocked;
	bool m_bDCasintUnlocked;
	bool m_bModel2DUnlocked;
public:
	CKeyFile();
	virtual ~CKeyFile();

	bool Unlocked() const;
	bool DCasintUnlocked() const;
	bool Model2DUnlocked() const;
};

#endif // !defined(AFX_KEYFILE_H__C3933D75_2457_4EFF_8745_254A4172235F__INCLUDED_)
