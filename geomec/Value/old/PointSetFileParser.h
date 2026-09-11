// PointSetFileParser.h: interface for the CPointSetFileParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTSETFILEPARSER_H__CFF71B7A_7EE6_4F85_A9BE_4B85E4657B02__INCLUDED_)
#define AFX_POINTSETFILEPARSER_H__CFF71B7A_7EE6_4F85_A9BE_4B85E4657B02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPointSetFileParser  
{
public:
	enum FORMAT { UNDEFINED, NODAL, ELEMENT };
	enum STATE	{ NONE, FORMAT_ERROR, 													// Undefined state
				  READ_NONE, READ_NODES, READ_ELEMENTS, READ_ELEMENT_TYPE, READ_READY,		// Read states
				  WRITE_NONE, WRITE_HEADER, WRITE_VALUE, WRITE_READY }; 	
	enum ELEMENT_TYPE { TYPE_UNKNOWN, HX24L, Q24IF, TE12L, T9CT, T3CMP, Q4CMP4 };
private:
	STATE m_state;
	FORMAT m_format;	
	ELEMENT_TYPE m_element_type;
public:
	CPointSetFileParser();
	virtual ~CPointSetFileParser();


	enum CToken::TokenRet ParseTag(const CString &sHeaderTag);

// Insert

private:
	enum CToken::TokenRet OnElementSet(std::string &sToken);
	enum CToken::TokenRet OnPointSet(std::string &sToken);
	enum CToken::TokenRet OnSectionPoints(std::string &sToken);	
	enum CToken::TokenRet OnSectionElements(std::string &sToken);	
	enum CToken::TokenRet OnSectionElementType(std::string &sToken);	

	enum CToken::TokenRet OnEndFile(std::string &sToken);
	
};

#endif // !defined(AFX_POINTSETFILEPARSER_H__CFF71B7A_7EE6_4F85_A9BE_4B85E4657B02__INCLUDED_)
