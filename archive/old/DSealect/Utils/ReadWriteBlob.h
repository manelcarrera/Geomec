#if !defined(READWRITEBLOB_INCL)
#define READWRITEBLOB_INCL

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma once



#include "ProgressCallBack.h"



BOOL ByteArrayFromFile(const std::string & sFileName,
					   CByteArray& Bytes,
					   const long iChunkSize = 1024,
					   const CProgressCallBack& PCB=CProgressCallBack());


BOOL ByteArrayToFile(const std::string & sFileName,
					 CByteArray& Bytes,
					 const long iChunkSize = 1024,
					 const CProgressCallBack& PCB=CProgressCallBack());
					 

#endif // !defined(READWRITEBLOB_INCL)