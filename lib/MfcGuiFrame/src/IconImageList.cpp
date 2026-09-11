// IconImageList.cpp: implementation of the CIconImageList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IconImageList.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3BC55D380088
CIconImageList::CIconImageList()
{

}

//##ModelId=3BC55D380092
CIconImageList::~CIconImageList()
{

}

//##ModelId=3BC55D380094
int CIconImageList::IconToImage(const unsigned int uIconID)
{
	if(uIconID == 0)
		return 0;

	if(m_mpResToImage.find(uIconID) == m_mpResToImage.end())
	{
		int nImage = Add(AfxGetApp()->LoadIcon(uIconID));

		if(nImage == 0)
			nImage = Add(AfxGetApp()->LoadIcon(uIconID));

		assert(nImage != 0);

		if(nImage != -1)
			m_mpResToImage.insert(std::pair<unsigned int, int>(uIconID, nImage));
	}

  std::map <unsigned int, int>::const_iterator image =
    m_mpResToImage.find(uIconID);

  return ((image != m_mpResToImage.end()) ? (*image).second : 0);
}


