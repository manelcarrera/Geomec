#ifndef _Data_
#define _Data_

#include <vector>

enum DataType
{
  DoubleV,
  IntegerV,
  StringV,

  Double,
  Integer,
  String,
    
  _Default = DoubleV,
  Undefined
};

typedef void* TMemoryChunkPtr;
//typedef struct { int rows; int cols; TMemoryChunkPtr data; DataType data_type = DataType::_Default; } TData;
typedef struct { int rows; int cols; TMemoryChunkPtr data; DataType data_type; } TData;

static const TData TData_INVALID = TData{ -1, -1, nullptr, Undefined }; 

template< typename T >
static TData data( std::vector<T>& t, int cols, DataType data_type = DataType::_Default ){ return TData{ (int)t.size(), cols, t.data(), data_type }; }

typedef std::vector< TData > TDataV;

#endif
