#include "stdafx.h"
#include "ReadWriteBlob.h"


BOOL ByteArrayFromFile(const std::string & sFileName,
					   CByteArray& Bytes,
					   const long iChunkSize,
					   const CProgressCallBack& PCB)
{
	class BytesFromFileIterator {
		// Inline Helper class to takeover resposibility of proper iterations through
		// the CByteArray.  It does so in chunks of iChunkSize_ and include the partial
		// chunk when a file is not exactly x * iChunkSize_ large.
		public:
		BytesFromFileIterator( HANDLE hFile, CByteArray& Bytes,long iChunkSize ) 
		: iChunkSize_( iChunkSize ) {
			DWORD dwFileLow = GetFileSize( (HANDLE)hFile,0 ); // assuming file < 4GB big
			Bytes.RemoveAll( );
			// Assuming filesize is < 2GB as Setsize accepts only 32bit integers
			Bytes.SetSize( dwFileLow );
			iChunkCount_ = dwFileLow / iChunkSize_;
			iPartialChunk_ = dwFileLow % iChunkSize_;
			pCurrentByte_ = Bytes.GetData( );
		}
		~BytesFromFileIterator( ) {
		}
		LPVOID operator( )( ) {return pCurrentByte_ ? reinterpret_cast<LPVOID>( pCurrentByte_ ) : 0; 
		}
		LPVOID operator++(int) {
			BYTE * pCurrentByte =pCurrentByte_;
			if ( iPartialChunk_ ) {
				pCurrentByte_+=iPartialChunk_;
				iPartialChunk_ = 0;
				if( !iChunkCount_ ){
					// check of only chunk
					pCurrentByte_ = 0;
				}
			} else {
				if ( --iChunkCount_ ) {
					pCurrentByte_ += iChunkSize_;
				} else {
					pCurrentByte_ = 0;
				}
			}
			return reinterpret_cast<LPVOID>( pCurrentByte_ );
		}
		int Size( ) const { return iPartialChunk_ ? iPartialChunk_ :iChunkSize_;
		}
		private:
		long iPartialChunk_;
		long iChunkCount_;
		long iChunkSize_;
		BYTE* pCurrentByte_;
	};

	// Lets Setup the FileHandling code here
	// Goal have open file that can be read from
	std::string sFile = sFileName;
	HANDLE hFileHandle = CreateFile( sFile.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0 );
	if ( hFileHandle!=INVALID_HANDLE_VALUE ) {

		// File exist and is open
		// Initialize Iterator So we can read from the file.
		// Note the cleanliness of this code no hassles with the Partial chunk here.
		BytesFromFileIterator ChunkToRead( hFileHandle,Bytes,iChunkSize );

		while ( ChunkToRead( ) ) {
			DWORD dwBytesRead;
			ReadFile( hFileHandle,ChunkToRead( ),ChunkToRead.Size( ),&dwBytesRead,0 );
			if ( dwBytesRead == ChunkToRead.Size( ) ) {
				PCB.OnUpdateProgress(1);//TODO
				if ( PCB.CancelProcess() ) {
					CloseHandle( (HANDLE)hFileHandle );
					throw "ByteArrayFromFile Canceled";
				}
			} else {
				PCB.OnFinished(TRUE);
				CloseHandle( (HANDLE)hFileHandle );
				throw "Error in ByteArrayFromFile";
			}
			ChunkToRead++;
		}
		CloseHandle( (HANDLE)hFileHandle );
		return TRUE;
		PCB.OnFinished(FALSE);
	}
	
	throw "Invalid FileHandle in ByteArrayFromFile()";
	return FALSE;	
}


BOOL ByteArrayToFile(const std::string & sFileName,
					 CByteArray& Bytes,
					 const long iChunkSize,
					 const CProgressCallBack& PCB)
{
	class BytesToFileIterator {
		// Inline Helper class to takeover resposibility of proper iterations through
		// the CByteArray.  It does so in chunks of iChunkSize_ and include the partial
		// chunk when a file is not exactly x * iChunkSize_ large.
		public:
		BytesToFileIterator( CByteArray& Bytes,long iChunkSize ) 
		: iChunkSize_( iChunkSize ) {
			iChunkCount_ = Bytes.GetSize( ) / iChunkSize_;
			iPartialChunk_ = Bytes.GetSize( ) % iChunkSize_;
			pCurrentByte_ = Bytes.GetData( );
		}
		~BytesToFileIterator( ) {
		}
		LPVOID operator( )( ) {return pCurrentByte_ ? reinterpret_cast<LPVOID>( pCurrentByte_ ) : 0; 
		}
		LPVOID operator++(int) {
			BYTE * pCurrentByte =pCurrentByte_;
			if ( iPartialChunk_ ) {
				pCurrentByte_+=iPartialChunk_;
				iPartialChunk_ = 0;
				if( !iChunkCount_ ){
					// check of only chunk
					pCurrentByte_ = 0;
				}
			} else {
				if ( --iChunkCount_ ) {
					pCurrentByte_ += iChunkSize_;
				} else {
					pCurrentByte_ = 0;
				}
			}
			return reinterpret_cast<LPVOID>( pCurrentByte_ );
		}
		int Size( ) const { return iPartialChunk_ ? iPartialChunk_ :iChunkSize_;
		}
		private:
		long iPartialChunk_;
		long iChunkCount_;
		long iChunkSize_;
		BYTE* pCurrentByte_;
	};

	if ( Bytes.GetSize( ) == 0 ) {
		return FALSE;
	}

	std::string sFile = sFileName;

	HANDLE hFileHandle = CreateFile( sFile.c_str(),GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0 );
	if ( hFileHandle!=INVALID_HANDLE_VALUE ) 
	{
		BytesToFileIterator ChunkToWrite( Bytes,iChunkSize );

		while ( ChunkToWrite( ) ) 
		{
			DWORD dwBytesWritten;
			WriteFile( (HANDLE)hFileHandle,ChunkToWrite( ),ChunkToWrite.Size( ),&dwBytesWritten,0 );
			if ( dwBytesWritten == ChunkToWrite.Size( ) ) 
			{
				PCB.OnUpdateProgress(1);
				if ( PCB.CancelProcess() ) 
				{
					PCB.OnFinished(TRUE);
					CloseHandle( (HANDLE)hFileHandle );
					throw "ByteArrayToFile Canceled";
				}
			} 
			else 
			{
				PCB.OnFinished(TRUE);
				CloseHandle( (HANDLE)hFileHandle );
				throw "Error in ByteArrayToFile";
			}
			ChunkToWrite++;
		}
		PCB.OnFinished(FALSE);
		CloseHandle( (HANDLE)hFileHandle );
		return TRUE;
	}

	throw "Invalid FileHandle in ByteArrayToFile()";
	return FALSE;
}
