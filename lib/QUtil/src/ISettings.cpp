//#include <cassert>
#include "ISettings.h"

ISettings::~ISettings()
{
}

ISettings* ISettings::instance()
{
  //assert(m_singleton != 0);
  return m_singleton;
}

void ISettings::cleanup()
{
  //assert(m_singleton != 0);
  delete m_singleton;
  m_singleton = 0;
}

// protected

ISettings::ISettings()
{
}

ISettings* ISettings::m_singleton = 0;

namespace settings{
	QString get_s(const char* section, const char* key)
	{
		return ISettings::instance()->getProfileString(section, key);
	}
	//
	// default: false
	// 
	bool get(const char* section, const char* key, bool default_ )
	{
		QString val = settings::get_s(section, key).toUpper();

		if(val.isEmpty())
			return default_;

		bool b_ = default_;
		if( default_ == true )
			b_ = !(val == "N" || val == "NO");
		else
			b_ = (val == "Y" || val == "YES");

		return b_;
	}
	//
	// returns 0 if key not found
	//
	int get_i(const char* section, const char* key, int default_)
	{
		QString val = settings::get_s(section, key);
		return val.isEmpty() ? default_ : val.toInt();
	}
	//
	// set
	//
	void set_i(const char* section, const char* key, int val )
	{
		ISettings::instance()->writeProfileInt(section, key, val);
	}
} //namespace settings
