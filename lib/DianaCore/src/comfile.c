 /*                                         Copyright (c) 2003 TNO DIANA B.V. */
//#include "MachDep.h"
//#include "dbaccess.h"
#include "comfile.h"
#include "lbci.h"
#include "lbgc.h"

//#include "logging.h"

#define BUFLEN 1024
static char buf[BUFLEN];
//static char errbuf[BUFLEN];

//static int scncmndscurline;

//static char **ScanErrors = NULL;
//static int nScanErrors = 0;

static void InitSubDirInfoStruct(DiSubDirInfo_t *pStruct, DiSubDirInfo_t *pParent)
{
  memset( pStruct, 0, sizeof( DiSubDirInfo_t ) );
  pStruct->pParent = pParent;
}

static void FreeSubDirInfoStruct(DiSubDirInfo_t *pStruct)
{
  int i;

  for ( i=0; i<pStruct->nChildren; i++ )
  FreeSubDirInfoStruct( (DiSubDirInfo_t *) pStruct->pChildren[i] );
  if ( pStruct->sName ) DiFree( pStruct->sName, "FreeSubDirInfoStruct" );
  if ( pStruct->sValue ) {
  for ( i=0; i < pStruct->nValues; i++ )
      DiFree( pStruct->sValue[i], "FreeSubDirInfoStruct" );
  DiFree( pStruct->sValue, "FreeSubDirInfoStruct" );
  }
}

static void SetAllParentsDefault(DiSubDirInfo_t *pRoot, int iValue)
{
  if( pRoot->pParent ) {
  DiSubDirInfo_t *pParent = (DiSubDirInfo_t *) pRoot->pParent;
  pParent->iIsDefault     = iValue;
  SetAllParentsDefault( pParent, iValue );
  }
}

static void GetRangeItems(DiSubDirInfo_t *pRoot, const char *sRangeItem)
{
  int        sz;
  ftn_int_t *pData;
  NumBuffer_t b;
  Range_t* r = 0;

  if ( !ReadUinfo( sRangeItem ) ) return;
  pRoot->sValue = DiMalloc( sizeof (char **), "GetRangeItems" );
  sz = Inquire( sRangeItem, "FLENGT" );
  if ( sz < 0 ) {
  pRoot->sValue[0] = NULL;
  pRoot->nValues   = 0;
  } else {
  pData = DiMalloc( sz * sizeof( ftn_int_t ), "GetRangeItems" );
  GetItemLength( sRangeItem, pData, sz );
  NumBufferInit(&b, pData, sz);
  r = RangeCreate(RT_NUM);
  RangeReadValues(r, (Buffer_t*)&b);
  pRoot->sValue[0] = RangeToString(r);
//    pRoot->sValue[0] = DiGetNumRangeAsText( pData, sz );
  pRoot->nValues   = 1;
  RangeDestroy(r);
  DiFree( pData, "GetRangeItems" );
  pRoot->iIsDefault = 0;
  SetAllParentsDefault( pRoot, 0 );
  }

}

static DiSubDirInfo_t *AddChild(DiSubDirInfo_t *pRoot, const char *Name)
{
  DiSubDirInfo_t *pChild;

  pRoot->nChildren++;
  pRoot->pChildren = DiRealloc( pRoot->pChildren, 0,
               pRoot->nChildren * sizeof( DiSubDirInfo_t * ), "GetSubDirInfo" );
  pRoot->pChildren[pRoot->nChildren-1] =
                         DiMalloc( sizeof( DiSubDirInfo_t ), "GetSubDirInfo" );
  pChild = (DiSubDirInfo_t *) pRoot->pChildren[pRoot->nChildren-1];
  InitSubDirInfoStruct( pChild, pRoot );
  pChild->sName = DiStrsave( Name );

  return pChild;
}

static int NumberOfNonDefaultChildren(DiSubDirInfo_t *pRoot)
{
  int rv = 0;
  int i;

  for( i=0; i<pRoot->nChildren; i++ )
  rv += ( ( (DiSubDirInfo_t *) pRoot->pChildren[i] )->iIsDefault == 0 );

  return rv;
}

static int GetSubDirInfo(DiSubDirInfo_t *pRoot, int iWriteDefaultValues)
{
  int          j;
  UserInfo_t  *ui;
  ftn_int_t    idx;
  ftn_int_t    iValue;
  ftn_double_t dValue;
  ftn_real_t   fValue;
  char        *p;
  int          iAddQuotes;
  int          iRange;
  char        *sRange = 0;
  int          iValueOnly;
  int          iDefault;
  int          iSingleEntry;
  char        *tb = "GetSubDirInfo";
  int          InActive;
  void        *pSearch;
  ftn_int_t    type;
  ftn_int_t    length;
  char        *Name;
  int          iMultipleDefaults = 0;
  NumBuffer_t    iBuffer;
  DoubleBuffer_t dBuffer;
  Range_t     *range = 0;

  // wedx 08082007:
  // Temporary (and dirty) hack for SOLVE, which has multiple default children,
  // so it still needs to be written.
  // This should be implemented by looking in the SCHEMA file to see whether a
  // directory has multiple child directories with a default flag.
  if(pRoot->pParent && strcmp(pRoot->sName, "SOLVE") == 0 && !((DiSubDirInfo_t*)pRoot->pParent)->pParent)
  	iMultipleDefaults = 1;

  pSearch = OpenSearch( "." );
  if ( !pSearch ) return 0;

  while ( (ui = ReadSearch( pSearch )) ) {

  Name   = ui->Name;
  type   = ui->Type;
  length = Inquire( Name, "FLENGT" );

  if ( !XistIndexed( Name, NULL ) ) continue;

  if ( strncmpcb( ui->Uitype, "gui", 3 ) ) {
      DiSubDirInfo_t *pChild = 0;
      iRange     = ui->Bits & FL_UI_RANGE;
      iValueOnly = ui->Bits & FL_UI_VALUE_ONLY;
      iDefault   = ui->Bits & FL_UI_DEFAULT && !iWriteDefaultValues;
      if( iRange ) sRange = DiStrsave( ui->Range );
      if ( (type % FL_INQ_TYPE_DIMENSIONED) != FL_INQ_TYPE_DIR )
    pChild = AddChild( pRoot, iValueOnly ? NULL : Name );

      switch( type ) {

      case FL_INQ_TYPE_DIR:
    InActive = (IsActive( Name ) == FTN_FALSE);
    if ( InActive && !iDefault ) break;

    pChild = AddChild( pRoot, iValueOnly ? NULL : Name );
    pChild->iIsDir     = 1;
    pChild->iIsDefault = iDefault && !iMultipleDefaults;
    if( !pChild->iIsDefault ) SetAllParentsDefault( pChild, 0 );

    if ( InActive ) {
          pChild = AddChild( pChild, "OFF" ); 
          SetAllParentsDefault( pChild, 0 );
    } else {
          PushDir();
          ChangeDir( Name );
      if( iRange ) GetRangeItems( pChild, ui->Range );
      if( GetSubDirInfo( pChild, iWriteDefaultValues ) ) pRoot->iBlockFormat = 1;
          PopDir();
    }
    break;

      case FL_INQ_TYPE_DIR+FL_INQ_TYPE_DIMENSIONED:
    iSingleEntry = ( Inquire( Name, "COUNT" ) == 1 );
    PushDir();
    ChangeDir( Name );
    {
          int ActiveFound = 0;
          idx = InquireIndex( "." );
          do {
      if ( IsActive( "." ) == FTN_FALSE ) continue;
      ActiveFound = 1;
      pChild = AddChild( pRoot, iValueOnly ? NULL : Name );
      if( pRoot->nChildren > 1 ) pRoot->iBlockFormat = 1;
      pChild->iIsDir     = 1;
      pChild->iIsDefault = iDefault && iSingleEntry && !iMultipleDefaults;
      if( !pChild->iIsDefault ) SetAllParentsDefault( pChild, 0 );
      if( iRange ) GetRangeItems( pChild, sRange );
      if( GetSubDirInfo( pChild, iWriteDefaultValues ) ) pRoot->iBlockFormat = 1;
          } while( NextIndex( &idx, 0 ) );

          if ( !ActiveFound && iDefault ) {
      pChild         = AddChild( pRoot, Name );
      pChild->iIsDir = 1;
      pChild         = AddChild( pChild, "OFF" ); 
      SetAllParentsDefault( pChild, 0 );
          }
    }
    PopDir();
    break;

      case FL_INQ_TYPE_STRING:
    pChild->sValue  = DiMalloc( sizeof( char * ), tb );
    pChild->nValues = 1;
    GetCharItem( Name, buf, BUFLEN-1 );
    if( iDefault && !strcmp( buf, ui->Default.u.cdef[0] ) )
          pChild->iIsDefault = 1;
    if( !pChild->iIsDefault ) SetAllParentsDefault( pChild, 0 );
    p          = buf;
    iAddQuotes = 0;
    while( *p ) if( islower( *( p++ ) ) ) { iAddQuotes = 1; break; }
    if ( !iAddQuotes && strchr( buf, '.' ) ) iAddQuotes = 1;

    /* skip trailing spaces */
    p = buf + strlen( buf );
    while( (p > buf) && (p[-1] == ' ') ) p--; *p = '\0';
    if ( iAddQuotes ) {
          *pChild->sValue = vDiStrsave( "\"", buf, "\"", NULL );
    } else {
          *pChild->sValue = DiStrsave( buf );
    }
    break;

      case FL_INQ_TYPE_STRING+FL_INQ_TYPE_DIMENSIONED:
    {
          char      **pData;
          ftn_int_t   nDim = 0;
          int         nLength;

          nLength = Inquire( Name, "LENGTH" );
          pData   = GetCharItemLength( Name, nLength, &nDim );
          if ( !pData ) break;

          if( iDefault && nDim == ui->Default.n ) {
      pChild->iIsDefault = 1;
      for( j=0; j<nLength; j++ )
              if( strcmp( pData[j], ui->Default.u.cdef[j] ) )
               { pChild->iIsDefault = 0; break; }
          }
          if( !pChild->iIsDefault ) SetAllParentsDefault( pChild, 0 );

          pChild->nValues = nDim;
          pChild->sValue  =
                   DiMalloc( nDim * sizeof( char * ), tb );
          for ( j=0; j<nDim; j++ ) pChild->sValue[j] = pData[j];
          DiFree( pData, tb );
    }
    break;

      case FL_INQ_TYPE_INT:
      case FL_INQ_TYPE_POINTER:
    pChild->sValue  = DiMalloc( sizeof( char * ), tb );
    pChild->nValues = 1;
    GetItem( Name, &iValue );
    if( iDefault && iValue == ui->Default.u.idef[0] )
          pChild->iIsDefault = 1;
    if( !pChild->iIsDefault ) SetAllParentsDefault( pChild, 0 );
    snprintf( buf, BUFLEN, "%lld", (long long) iValue );
    *pChild->sValue = DiStrsave( buf );
    break;

      case FL_INQ_TYPE_INT+FL_INQ_TYPE_DIMENSIONED:
    {
          ftn_int_t *iValues;
          pChild->nValues = 1;
          pChild->sValue  = DiMalloc( sizeof( char * ), tb );
          iValues = DiMalloc( length * sizeof( ftn_int_t ), tb );
          GetItemLength( Name, iValues, length );
          if( iDefault && length == ui->Default.n ) {
      pChild->iIsDefault = 1;
      for( j=0; j<length; j++ ) {
              if( iValues[j] != ui->Default.u.idef[j] ) {
        pChild->iIsDefault = 0;
        break;
              }
      }
          }
          if( !pChild->iIsDefault ) SetAllParentsDefault( pChild, 0 );

          //pChild->sValue[0] = DiDecomposeIntRange( iValues, length );
          //pChild->sValue[0] = IntRangeArrayToText(iValues, length);
          range = RangeCreate(RT_NUM);

          NumBufferInit(&iBuffer, iValues, length);
          RangeReadValues(range, (Buffer_t*)&iBuffer);
          pChild->sValue[0] = RangeToString(range);
          
          RangeDestroy(range);

          DiFree( iValues, tb );
    }
    break;

      case FL_INQ_TYPE_REAL:
    pChild->sValue = DiMalloc( sizeof( char * ), tb );
    pChild->nValues = 1;
    GetItem( Name, &fValue );
    if ( iDefault && fValue == ui->Default.u.ddef[0] )
          pChild->iIsDefault = 1;
    if ( !pChild->iIsDefault ) SetAllParentsDefault( pChild, 0 );
    snprintf( buf, BUFLEN, "%g", fValue );
    *pChild->sValue = DiStrsave( buf );
    break;

      case FL_INQ_TYPE_REAL+FL_INQ_TYPE_DIMENSIONED:
    {
          ftn_real_t *fValues;
          pChild->nValues = length;
          pChild->sValue  =
      DiMalloc( length * sizeof( char * ), tb );
          fValues =
      DiMalloc( length * sizeof( ftn_real_t ), tb );
          GetItemLength( Name, fValues, length );
          if ( iDefault && length == ui->Default.n ) {
      pChild->iIsDefault = 1;
      for ( j=0; j<length; j++ ) {
              if( fValues[j] != ui->Default.u.ddef[j] ) {
        pChild->iIsDefault = 0;
        break;
              }
      }
          }
          if ( !pChild->iIsDefault ) SetAllParentsDefault( pChild, 0 );

          for ( j=0; j<length; j++ ) {
      snprintf( buf, BUFLEN, "%g", fValues[j] );
      pChild->sValue[j] = DiStrsave( buf );
          }
          DiFree( fValues, tb );
    }
    break;

      case FL_INQ_TYPE_DBLE:
    pChild->sValue  = DiMalloc( sizeof( char * ), tb );
    pChild->nValues = 1;
    GetItem( Name, &dValue );
    if ( iDefault && dValue == ui->Default.u.ddef[0] )
          pChild->iIsDefault = 1;
    if ( !pChild->iIsDefault ) SetAllParentsDefault( pChild, 0 );
    snprintf( buf, BUFLEN, "%g", dValue );
    *pChild->sValue = DiStrsave( buf );
    break;

      case FL_INQ_TYPE_DBLE+FL_INQ_TYPE_DIMENSIONED:
    {
          ftn_double_t *dValues;
          pChild->nValues = 1;
          pChild->sValue  = DiMalloc( sizeof( char * ), tb );
          dValues = DiMalloc( length * sizeof (ftn_double_t), tb );
          GetItemLength( Name, dValues, length );
          if( iDefault && length == ui->Default.n ) {
      pChild->iIsDefault = 1;
      for ( j=0; j<length; j++ ) {
              if ( dValues[j] != ui->Default.u.ddef[j] ) {
        pChild->iIsDefault = 0;
        break;
              }
      }
          }
          if( !pChild->iIsDefault ) SetAllParentsDefault( pChild, 0 );

          //pChild->sValue[0] = DiDecomposeDbleRange( dValues, length );
          //pChild->sValue[0] = DoubleRangeArrayToText(dValues, length);


          range = RangeCreate(RT_DOUBLE);

          DoubleBufferInit(&dBuffer, dValues, length);
          RangeReadValues(range, (Buffer_t*)&dBuffer);
          pChild->sValue[0] = RangeToString(range);
          
          RangeDestroy(range);

          DiFree( dValues, tb );
    }
    break;

      case FL_INQ_TYPE_LOG:
    iValue = IsActive( Name );
    if ( ( !iDefault && !iValue ) ||
             ( iDefault && iValue == ui->Default.u.idef[0] ))
          pChild->iIsDefault = 1;
    if ( !pChild->iIsDefault ) SetAllParentsDefault( pChild, 0 );
    if ( !iValue ) {
          pChild->sValue = DiMalloc( sizeof(char *), tb );
          pChild->nValues = 1;
          *pChild->sValue = DiStrsave( "OFF" );
    }
    break;

      case FL_INQ_TYPE_DUMMY:
    pChild->iIsDefault = iDefault;
    if( !pChild->iIsDefault ) SetAllParentsDefault( pChild, 0 );
    break;

      case FL_INQ_TYPE_LOG+FL_INQ_TYPE_DIMENSIONED:
    assert( 0 );
    break;

      }

      if ( NumberOfNonDefaultChildren( pRoot ) > 1 ) pRoot->iBlockFormat = 1;
  }

  }

  /* special case for results in subdir 'user' */
  if ( XistIndexed( "USER", NULL ) && XistIndexed( "SELTYP", NULL ) ) {
  char seltyp[7];
  GetCharItem( "SELTYP", seltyp, 6 );
  if ( strncmp( seltyp, "USER", 4 ) == 0 ) {
      PushDir();
      ChangeDir( "USER" );
    GetSubDirInfo( pRoot, iWriteDefaultValues );
      PopDir();
  }
  }

  CloseSearch( pSearch );

  return pRoot->iBlockFormat;
}

static void WriteCommandsToFile(FILE *fp, DiSubDirInfo_t *pRoot, int iIndent)
{
  int   i, j;
  char *p;
  int   newindent;
  char  localbuf[BUFLEN];

  p = localbuf;
  for ( i=0; i<iIndent; i++ ) *( p++ ) = ' ';

  if ( !NumberOfNonDefaultChildren( pRoot ) ) return;

  for( i = 0; i < pRoot->nChildren; i++ ) {
  DiSubDirInfo_t *pChild = (DiSubDirInfo_t *) pRoot->pChildren[i];
  if ( pChild->iIsDefault ) continue;
  if( pChild->iIsDir ) {
      assert( pChild->sName );
      if ( pChild->iBlockFormat ) {
    sprintf( p, "BEGIN %s", pChild->sName );
    newindent = iIndent + 2;
      } else {
    sprintf( p, "%s", pChild->sName );
    newindent = 1;
      }
      fputs( localbuf, fp );

      /* Ranges have values stored here! */
      for( j = 0; j < pChild->nValues; j++ )
    fprintf( fp, " %s", pChild->sValue[j] );
      if ( pChild->iBlockFormat ) fputs( "\n", fp );
      else if ( pChild->nValues ) fputs( " /",  fp );

      WriteCommandsToFile( fp, pChild, newindent );

      if ( pChild->iBlockFormat ) {
    sprintf( p, "END %s\n", pChild->sName );
    fputs( localbuf, fp );
      } else if ( !NumberOfNonDefaultChildren( pChild ) ) {
    fputs( "\n", fp );
      }
  } else {
      if( pChild->sName ) sprintf( p, "%s", pChild->sName );
      else *p = '\0';
      for( j=0; j<pChild->nValues; j++ ) {
    if( pChild->sName || j ) strcat( p, " " );
    strcat( p, pChild->sValue[j] );
      }
      strcat( p, "\n" );
      fputs( localbuf, fp );
  }
  }
}

static int WriteAnalysisTypeToFile(FILE *fp, int iWriteDefaultValues)
{
  int i;
  int rv = 1;
  int count;
  DiSubDirInfo_t *pRoot;

  count = Inquire( ".", "COUNT" );
  pRoot = DiMalloc( sizeof( DiSubDirInfo_t ), "WriteAnalysisTypeToFile" );
  InitSubDirInfoStruct( pRoot, NULL );

  for( i=1; i<=count; i++ ) {
  ftn_int_t type;
  ftn_int_t length;
  char *Name = GetItemName( i, &type, &length );
  if ( Name ) {
      assert(  XistIndexed( Name, NULL ) );
      if( type == FL_INQ_TYPE_STRING && !strcmp( Name, "SEGMEN" ) ) {
    GetCharItem( "SEGMEN", buf, BUFLEN-1 );
    if ( !strncmp( buf, "END", 3 ) ) {
          fputs( "*END\n", fp );
          rv = 0;
    }
      }

      if( type == FL_INQ_TYPE_DIR ) {
    fprintf( fp, "*%s\n", Name );
    PushDir();
    ChangeDir( Name );
          MakeDefaultTree( ".", NULL );
          pRoot->iBlockFormat = GetSubDirInfo( pRoot, iWriteDefaultValues );
          WriteCommandsToFile( fp, pRoot, 1 );
          FreeSubDirInfoStruct( pRoot );
    PopDir();
      }

      DiFree( Name, "WriteAnalysisTypeToFile" );
  }
  }

  DiFree( pRoot, "WriteAnalysisTypeToFile" );
  return rv;
}

/*
 *ddoc SaveComFile:
 *ddoc   Writes a com-file from a given /CMNDS/SEGINF directory in a database
 *ddoc
 *ddoc Arguments:
 *ddoc   PathName     char*     In - Filename of the file to be written
 *ddoc   InitialText char*     In - Initial text written to the com-file (e.g. *FILOS\n INITIA\n), may be NULL
 */
extern int SaveComFile(const char *PathName, const char* InitialText, int iWriteDefaultValues)
{
  ftn_int_t  seginfidx;
  FILE      *fp;
  int        rva, rv;
  int        svfi;

  assert( fcisop_() );

  rv = 1;

  if ( !XistIndexed( "/CMNDS/SEGINF/", NULL ) ) return 0;
  PushDir();
  ChangeDir( "/CMNDS/SEGINF/" );

  seginfidx = InquireIndex( "." );

  if( !( fp = fopen( PathName, "w" ) ) ) {

      // This should be there, but it isn't; for now commenting out
      // DiWriteLogLine( "!Save COM file: unable to open file for write", NULL );
      rv = 0;

  } else {
  
      svfi = SetFunctionInfo( DS_FUNC_CHECK | DS_FUNC_CHKPARENTS );

      if(InitialText)
    fputs(InitialText, fp);

      do {
    rva = WriteAnalysisTypeToFile( fp, iWriteDefaultValues );
      } while( NextIndex( &seginfidx, 0 ) );

      if ( rva ) {
    /* no *END after last analysis type, write it now */
    fputs( "*END\n", fp );
      }

      fclose( fp );
      SetFunctionInfo( svfi );

  }

  PopDir();

  return rv;
}

/*
 * Return lowest index in CMNDS/SEGINF with commands for analysis type AnType
 * which is greater or equal to startidx
 */
/*
static int AnalysisCommandsExist(const char *AnType, int startidx)
{
  ftn_int_t idx;
  int       rv = -1;

  PushDir();
  ChangeDir( "/CMNDS/SEGINF" );
  idx = InquireIndex( "." );
  do {
  if ( idx < startidx ) continue;
  if ( XistIndexed( AnType, NULL ) ) {
      rv = idx;
      break;
  }
  } while ( NextIndex( &idx, 0 ) );
  PopDir();

  return rv;
}
*/

/*
 * Remove all commands starting from startidx
 */
/*
static void RemoveCommands(int startidx)
{
  ftn_int_t idx;
  int       midx;

  PushDir();
  ChangeDir( "/CMNDS" );
  midx = Inquire( "SEGINF", "DIM" );
  for ( idx = startidx; idx <= midx; idx++ )
      RemoveIndexedItem( "SEGINF/", &idx );
  PopDir();
}
*/

extern int GetActiveIndex( char **Entries, int count );

#ifndef UNUSED
# define UNUSED(x) (void)(x)
#endif  // UNUSED

/*
static void scan_error_gui(const char *pos, const char *fmt, ...)
VA_DCL
{
  va_list args;
  char chbuf[2048];

  UNUSED(pos);

  VA_START( args, fmt );
  vsnprintf(errbuf, BUFLEN-1, fmt, args );
  va_end( args );

  ScanErrors = DiRealloc( ScanErrors, 0, ( nScanErrors + 1 ) * sizeof( char * ), "scan_error_gui" );
  snprintf( chbuf, 2047, "Scan error on line %d: %s", scncmndscurline, errbuf );
  ScanErrors[nScanErrors++] = DiStrsave( chbuf );
}
*/
