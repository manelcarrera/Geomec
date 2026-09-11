 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// SpreadFile.h: interface for the CSpreadFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPREADFILE_H__B8BB145C_A667_4E11_A564_9EFD10BD45FF__INCLUDED_)
#define AFX_SPREADFILE_H__B8BB145C_A667_4E11_A564_9EFD10BD45FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TextFile.h"
#include <vector>

#include "FieldValueFileExports.h"

// Last Reviewed: 22 November 2002, tnma

// REVIEW 
// Add more comment including a short description desribing the use of the class.
// END REVIEW							

//##ModelId=3BA8A4B200E1
template<class T>
class CSpreadFile : public CTextFile  
{
	// Type defs for external use ...
public:
	// Define the vector architecture ... // REVIEW Bullshit comment END REVIEW//
	//##ModelId=3BA8A4B201DB
	typedef T TValueType;
	//##ModelId=3BA8A4B20217
	typedef std::vector<TValueType> TValueVec;
private:
	//##ModelId=3BA8A4B20256
	typedef std::vector<TValueVec> TFileVec;
	//##ModelId=3BA8A4B2018A
	TFileVec m_vcFile;
	//##ModelId=3BA8A4B2019B
	std::vector<std::string> m_vcHeader;
	// Helper function for line reading ...
	//##ModelId=3BA8A4B2019F
	bool ReadLine();
public:
	//##ModelId=3BA8A4B20284
	typedef TFileVec::iterator iterator;
	//##ModelId=3BA8A4B202C3
	typedef TFileVec::const_iterator const_iterator;
	
	// Construction destruction ...
	//##ModelId=3BA8A4B201AA
	CSpreadFile();
	//##ModelId=3BA8A4B201AB
	CSpreadFile(const std::vector<TValueVec> &vcContent, const std::vector<std::string> &vcHeader);
	//##ModelId=3BA8A4B201AE
	virtual ~CSpreadFile();

	// Iterator ...
	//##ModelId=3BA8A4B201B9
	iterator begin() { return m_vcFile.begin(); }
	//##ModelId=3BA8A4B201BA
	iterator end() { return m_vcFile.end(); }
	//##ModelId=3BA8A4B201BB
	const_iterator begin() const { return m_vcFile.begin(); }
	//##ModelId=3BA8A4B201BD
	const_iterator end() const { return m_vcFile.end(); }

	// Sizes of the file ...
	//##ModelId=3BA8A4B201BF
	TValueVec::size_type LineLength() const;
	//##ModelId=3BA8A4B201C9
	TValueVec::size_type Size() const;
	
protected:
	// Overrides from textfile for comment parsing ...
	//##ModelId=3BA8A4B201CB
	virtual ParseRet OnRead();
	//##ModelId=3BA8A4B201CD
	virtual CToken::TokenRet OnParseFail(const std::string &sToken);
	//##ModelId=3BA8A4B201D9
	virtual void OnNewToken();
};

//##ModelId=3BA8A4B201AA
template<class T>
CSpreadFile<T>::CSpreadFile() 
{
}

//##ModelId=3BA8A4B201AB
template<class T>
CSpreadFile<T>::CSpreadFile(const std::vector<TValueVec>& vcContent, const std::vector<std::string>& vcHeader) 
: m_vcFile(vcContent), m_vcHeader(vcHeader)
{
}

//##ModelId=3BA8A4B201AE
template<class T>
CSpreadFile<T>::~CSpreadFile()
{
}
/*
template<class T>
void CSpreadFile<T>::Clear()
{
	m_vcFile.clear();
	m_vcHeader.clear();
}
*/
//##ModelId=3BA8A4B201BF
template<class T>
CSpreadFile<T>::TValueVec::size_type CSpreadFile<T>::LineLength() const
{
	if( m_vcFile.size() == 0) 
	{
		return 0;
	}
	
	return m_vcFile[0].size();
}

//##ModelId=3BA8A4B201C9
template<class T>
CSpreadFile<T>::TValueVec::size_type CSpreadFile<T>::Size() const
{
	return m_vcFile.size();
}

//##ModelId=3BA8A4B201CB
template<class T>
CTextFile::ParseRet CSpreadFile<T>::OnRead()
{
	// Clear the content of the file
//	Clear(); // REVIEW Delete or not? END REVIEW//
	
	// Read one line ...
	while(!InputStream().eof()) {
		// Store file position // REVIEW Bullshit comment END REVIEW//
		StoreFilePosition();
		
		// Try to read one line 
		if(!ReadLine()) {
			// Readline failed. Can be comment ... Restore and try to parse
			RestoreFilePosition();
			if(!Parse()) 
			{
				// Parsing did also failed 
				return FORMAT_UNKNOWN;
			}
		}

		ItemRead();

	}

	// Job done ...
	return OK;
}

//##ModelId=3BA8A4B2019F
template<class T>
bool CSpreadFile<T>::ReadLine()
{
	ASSERT(!InputStream().eof());
	ASSERT(InputStream().good());

	// Increase file vector ...
	m_vcFile.resize( m_vcFile.size() + 1);

	while( (InputStream().peek() != '\n') && (!InputStream().eof()) )
	{
		T value;

		// Try to read a double ...
		InputStream() >> value;

		// Succeed? // REVIEW Bullshit comment END REVIEW//
 		if(!InputStream().good())
			return false;

		// Add the value to our temporary vector ...
		m_vcFile[ m_vcFile.size() - 1 ].push_back(value);

		// Eat some white space except endl
		while(( InputStream().peek() == ' ' ) ||
			  ( InputStream().peek() == '\t' ) )
		{
			// Eat the whitespace character
			InputStream().get();
		}
	};
	
	// Is the terminate condition OK?
	ASSERT( (InputStream().peek() == '\n') || (InputStream().eof()) );

	// Check consistency ..
	if( LineLength() != m_vcFile[ m_vcFile.size() - 1 ].size() )
	{
		// Inconsistent line length ... Destroy last read vector ...
		m_vcFile.resize(m_vcFile.size() - 1);
		return false;
	}

	// Eat the \n char
	if(InputStream().peek() == '\n')
		InputStream().get();

	return true;
}

//##ModelId=3BA8A4B201CD
template<class T>
CToken::TokenRet CSpreadFile<T>::OnParseFail(const std::string &sToken)
{
	// Comment should be readed function failed on the new double so restore and try again ...
	RestoreFilePosition();
	if(ReadLine())
		return OK_CONTINUE;
	
	return FORMAT_UNKNOWN;
}

//##ModelId=3BA8A4B201D9
template<class T>
void CSpreadFile<T>::OnNewToken()
{
	// Store file position for recovery after a failed read function call...
	StoreFilePosition();
}

#endif // !defined(AFX_SPREADFILE_H__B8BB145C_A667_4E11_A564_9EFD10BD45FF__INCLUDED_)
