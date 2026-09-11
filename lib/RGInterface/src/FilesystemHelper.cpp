// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "FilesystemHelper.h"

#include <cassert>
#include <RescueModel.h>

#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#include <direct.h>  // _mkdir etc.
#include <time.h>
#include <Windows.h>
#else
#include <values.h>
#include <unistd.h>
#endif
#include <stdexcept>
#include <sys/stat.h>

///////////////////////////////////////////////////////////////////////////////
/// @brief Extract model name from path
/// @return short name which is the last element of path (not a file name)
///////////////////////////////////////////////////////////////////////////////
std::string FilesystemHelper::DirName2ModelName( const std::string & modelName ) 
{
   size_t len = modelName.length();
   if ( len > 4 && ( modelName.substr( len - 4 ) == ".bin" || modelName.substr( len - 4 ) == ".txt" ) )
   {  
      // full file name is given extract last dir name
      size_t pos = modelName.find_last_of("/\\");
      if ( pos == std::string::npos )
      {
         return modelName.substr( 0, len - 4 );  // construct model name from file name by cutting extension
      }
      size_t ppos = modelName.substr( 0, pos ).find_last_of( "/\\" );
      if ( ppos == std::string::npos )
      {
         return modelName.substr( 0, pos ); // no more elements in path
      }
      return modelName.substr( ppos+1, pos - ppos - 1 );
   }

   size_t pos = modelName.find_last_of("/\\");
   if ( pos == std::string::npos )
   {
      return modelName; // no path elements just modelName
   }

   if ( pos == len - 1 )
   {
      size_t ppos = modelName.substr( 0, pos ).find_last_of( "/\\" ); // look for the second one
      return modelName.substr( ppos+1, pos - ppos - 1 ); // return last path element as model name
   }
   return modelName.substr( pos+1 );
}


///////////////////////////////////////////////////////////////////////////////
/// @brief Returns full path to current working directory as string. OS dependent
///////////////////////////////////////////////////////////////////////////////
std::string FilesystemHelper::RGGetCWD()
{
   size_t bufSize = 64000;
   char * ccwd = new char[bufSize];
   assert( ccwd );

#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
   const long ret = GetCurrentDirectory( bufSize, ccwd );
   assert( ret+1 < bufSize );
#else
   const char * ret = getcwd( ccwd, bufSize );
   assert( ret );
#endif

   std::string retWD( ccwd );
   delete [] ccwd;

   return retWD;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Returns true if given path starts with root, false otherwise
///////////////////////////////////////////////////////////////////////////////
bool FilesystemHelper::IsFullPath( const std::string & somePath )
{
#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
   if ( somePath.find( "\\\\" ) == 0 )                        return true; // win network path
   if ( somePath.find( ":" ) == 1 && isalpha( somePath[0] ) ) return true; // path starts with letter drive
#else
   if ( somePath.find( "/" ) == 0 )                           return true;
#endif

   return false;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Construct file name from path
/// @return fully qualified file name for the Rescue model
///////////////////////////////////////////////////////////////////////////////
std::string FilesystemHelper::RGGetFileName( std::string modelName, bool isTxtFormat ) 
{
   std::string rescueFileName;
   const std::string & ccwd = RGGetCWD();

   // define OS dependent path delimiter 
#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
   std::string osPathDelimit = "\\";
   std::string pathDelimit = "/";
#else
   std::string osPathDelimit = "/";
   std::string pathDelimit = "\\";
#endif

   std::string fileExt( isTxtFormat ? ".txt" : ".bin" );

   size_t pos;
   while ( (pos = modelName.find( pathDelimit )) != std::string::npos )
   { 
      modelName.replace( pos, pathDelimit.length(), osPathDelimit  );
   }

   // check for absolute given path in form /.../.. or \...\.. or c:\...\..
#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
   if ( !(modelName.find( "\\\\" ) == 0) && modelName.find( osPathDelimit ) == 0 ) // \some\thing\like\this - just add current disk prefix
   {
      if ( ccwd.find( ":" ) == 1 && isalpha( ccwd[0] ) ) // if current folder on local computer
      {
         rescueFileName = ccwd.substr( 0, 2 ); // add c:
      }
      else // on network
      {
         rescueFileName = ccwd.substr( 0, 1 ); // add one folder delimiter at the beginning
      }
   }
#else
   if ( ( modelName.find( ":" ) == 1 && isalpha( modelName[0] ) ) ) // windows full path is given just cut off c:
   {
      modelName = modelName.substr( 2, std::string::npos );
   }
   else if ( modelName.find( "//" ) == 0 ) // windows network path is given just replace \\ with /
   {
      modelName = modelName.substr( 1, std::string::npos );
   }
#endif
   else if ( !IsFullPath( modelName ) )
   {
      rescueFileName = ccwd;
      if ( rescueFileName.rfind( osPathDelimit ) != rescueFileName.length() - 1 ) 
      {
         rescueFileName += osPathDelimit;
      }
   }
   size_t len = modelName.length();
   if ( len > 4 && ( modelName.substr( len - 4 ) == ".bin" || modelName.substr( len - 4 ) == ".txt" ) )
   { // full file name is given just return what was given
      rescueFileName += modelName;
   }
   else
   {
      pos = modelName.rfind( osPathDelimit );
      if ( pos == std::string::npos ) // no path is given construct simple file name
      {
         rescueFileName += modelName + osPathDelimit + modelName + fileExt;
      }
      else if ( pos == len - 1 ) // else split last part of given path as model name and create file name using this model name
      {
         rescueFileName += modelName + DirName2ModelName( modelName ) + fileExt;
      }
      else
      {
         rescueFileName += modelName + osPathDelimit + DirName2ModelName( modelName ) + fileExt;
      }
   } 
   return rescueFileName;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Extract directory name from fully qualified file name
/// @return directory name where Rescue model could be written
///////////////////////////////////////////////////////////////////////////////
std::string FilesystemHelper::RGGetDirName( const std::string & fileName )
{
   size_t len = fileName.length();
   if ( len > 4 && ( fileName.substr( len - 4 ) == ".bin" || fileName.substr( len - 4 ) == ".txt" ) )
   {  
      // full file name is given
      size_t pos = fileName.find_last_of("/\\");
      if ( pos == std::string::npos )
      {
         return fileName.substr( 0, len - 4 );  // construct dir name from file name by cutting extension
      }
      return fileName.substr( 0, pos ); // cut file name
   }

   size_t pos = fileName.find_last_of("/\\");
   return pos == (len - 1) ? fileName.substr( 0, pos ) : fileName;
}


///////////////////////////////////////////////////////////////////////////////
/// @brief if directory alredy exist, will do nothing
///////////////////////////////////////////////////////////////////////////////
void FilesystemHelper::mkdir( const std::string & dirName )
{
#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
   int failed = _mkdir( dirName.c_str() );
   if (failed == -1){
     throw std::runtime_error("FilesystemHelper::mkdir() - mkdir system call Failed to create directory");
   }
#else
   bool failed = ::mkdir( dirName.c_str(), S_IRWXU );
   assert( ! failed );
#endif
}



///////////////////////////////////////////////////////////////////////////////
/// @return process name 
///////////////////////////////////////////////////////////////////////////////
std::string FilesystemHelper::getProcessName()
{
#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
   char result[ MAX_PATH ];
   return std::string( result, GetModuleFileName( NULL, result, MAX_PATH ) );
#else
   char result[ PATH_MAX ];
   size_t count = readlink( "/proc/self/exe", result, PATH_MAX );
   return std::string( result, ( count > 0 ) ? count : 0 );
#endif
}


///////////////////////////////////////////////////////////////////////////////
/// @brief Sleep for a number of milli seconds.
/// @param milliSconds The number of milli seconds to sleep.
///////////////////////////////////////////////////////////////////////////////
void FilesystemHelper::sleepFor( unsigned int milliSeconds )
{
   const unsigned int Thousand = 1000;

#ifndef WIN32
   for (; ( milliSeconds / Thousand ) > 0; milliSeconds -= Thousand )
   {
      usleep( Thousand * Thousand );
   }

   if ( milliSeconds > 0 )
   {
      usleep( Thousand * milliSeconds );
   }
#else   // WIN32
   Sleep( milliSeconds );
#endif // WIN32
}

