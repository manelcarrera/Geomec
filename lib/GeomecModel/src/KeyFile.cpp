// KeyFile.cpp: implementation of the KeyFile class.
//
//////////////////////////////////////////////////////////////////////

#include "KeyFile.h"
#include "ISettings.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

namespace
{

// set to 1 to invert unlocked and locked for testing
#define TESTING 0

const QString MAGIC = "GEOMEC3_UNLOCK_EXPERIMENTAL_FEATURES";
const QString DCASINT_MAGIC = "GEOMEC3_UNLOCK_DCASINT_FEATURES";
const QString MODEL2D_MAGIC = "GEOMEC3_UNLOCK_2DMODEL_FEATURES";

} // anonymous namespace

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKeyFile::CKeyFile() : m_bUnlocked(false), m_bDCasintUnlocked(false), m_bModel2DUnlocked(false)
{
	ISettings* registry = ISettings::instance();

	// general experimental feature lock
	// try system setting
	QString strKey = registry->getSystemString("Settings", MAGIC);
	if(strKey.compare("YES") != 0)
	{
		// try user setting
		strKey = registry->getProfileString("Settings", MAGIC);
	}

	if(strKey.compare("YES") == 0) m_bUnlocked = true;

	// DCasint lock
	strKey = registry->getSystemString("Settings", DCASINT_MAGIC);
	if(strKey.compare("YES") != 0)
	{
		// try user setting
		strKey = registry->getProfileString("Settings", DCASINT_MAGIC);
	}
	
	if(strKey.compare("YES") == 0) m_bDCasintUnlocked = true;

	// 2DModel lock
	strKey = registry->getSystemString("Settings", MODEL2D_MAGIC);
	if(strKey.compare("YES") != 0)
	{
		// try user setting
		strKey = registry->getProfileString("Settings", MODEL2D_MAGIC);
	}

	if(strKey.compare("YES") == 0) m_bModel2DUnlocked = true;

#if TESTING
	m_bUnlocked = !m_bUnlocked;
#endif
}

CKeyFile::~CKeyFile()
{
}

bool CKeyFile::Unlocked() const
{
	return m_bUnlocked;
}

bool CKeyFile::DCasintUnlocked() const
{
#ifdef DCASINT_KEYFILE
	return m_bDCasintUnlocked;
#else
	return true;
#endif
}

bool CKeyFile::Model2DUnlocked() const
{
#ifdef MODEL2D_KEYFILE
	return m_bModel2DUnlocked;
#else
	return true;
#endif
}