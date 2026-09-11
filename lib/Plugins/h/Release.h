#pragma once

class CRelease
{
	int major;
	int minor;
	int build;

public:
	CRelease( std::string release_ ) : major( -1 ), minor( -1 ), build( -1 )
	{
		QRegExp rx("(\\.)"); //RegEx for ' ' or ',' or '.' or ':' or '\t'
		QStringList release = QString::fromStdString( release_ ).split( rx );

		int size = release.size();

		if( size >= 2 )
		{
			major = release.at( 0 ).toInt();
			minor = release.at( 1 ).toInt();

			if( size > 2 )
				build = release.at( 2 ).toInt();
		}
		else
		{
			// NON VALID
		}
	};

	bool valid(){ return ( major != -1 && minor != -1 ); };

	const bool operator < ( const CRelease &rhs ) const
	{
        return (	( major < rhs.major ) || 
					( major == rhs.major && minor < rhs.minor ) || 
					( major == rhs.major && minor == rhs.minor && build < rhs.build ) ); 
    };

	/*const bool operator > ( const CRelease& lhs, const CRelease& rhs ) const
	{
		return rhs < lhs;
	};*/
};