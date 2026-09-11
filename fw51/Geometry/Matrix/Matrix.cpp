 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Matrix.cpp: implementation of the CMatrix class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include <cassert>
#include "Matrix.h"
#include "IVector.h"


//////////////////////////////////////////////////////////////////////
// Implementation of CMatrix
//////////////////////////////////////////////////////////////////////

namespace geo {
//##ModelId=3BC6995D0044
CMatrix::CMatrix()
:m_rows(0), m_columns(0), m_pData(0)
{
}

//##ModelId=3BC6995D0045
CMatrix::CMatrix(int rows, int columns)
:m_rows(0), m_columns(0), m_pData(0)
{
	ReSize(rows, columns);
}

CMatrix::CMatrix( const IVector& v1, const IVector& v2, const IVector& v3 )
: m_rows( 0 ), m_columns( 0 ), m_pData( 0 )
{
        ReAllocate( 3, 3 );
        for ( int r = 0; r < 3; ++r ) {
                Value( r, 0, v1[r] );
                Value( r, 1, v2[r] );
                Value( r, 2, v3[r] );
        }
}

CMatrix::CMatrix(const IMatrix& rhs)
:m_rows(0), m_columns(0), m_pData(0)
{
	ReAllocate(rhs.RowSize(), rhs.ColumnSize());

	// Assign
	for(int r = 0; r < RowSize(); r++)
		for(int c = 0; c < ColumnSize(); c++)
			Value(r, c, rhs.Value(r, c));
}

CMatrix::CMatrix(const CMatrix& rhs)
:IMatrix(rhs), m_rows(0), m_columns(0), m_pData(0)
{
	ReAllocate(rhs.RowSize(), rhs.ColumnSize());

	// Assign
	for(int r = 0; r < RowSize(); r++)
		for(int c = 0; c < ColumnSize(); c++)
			Value(r, c, rhs.Value(r, c));
}

CMatrix& CMatrix::operator =(const CMatrix &rhs) 
{
	ReAllocate(rhs.RowSize(), rhs.ColumnSize());

	for(int r = 0; r < rhs.RowSize(); r++)
		for(int c = 0; c < rhs.ColumnSize(); c++)
			Value(r, c, rhs.Value(r, c));

	return *this;
}


//CMatrix::CMatrix(const ITensor& tensor)
//:m_rows(0), m_columns(0), m_pData(0)
//{
//	if(tensor.Empty())
//		return;
//
//	// Just creating a 3 x 3 matrix
//	ReAllocate(3, 3);
//
//	// Filling the matrix
//	Value(0, 0, tensor.XX());
//	Value(1, 0, tensor.XY());
//	Value(2, 0, tensor.XZ());
//	Value(0, 1, tensor.XY());
//	Value(1, 1, tensor.YY());
//	Value(2, 1, tensor.YZ());
//	Value(0, 2, tensor.XZ());
//	Value(1, 2, tensor.YZ());
//	Value(2, 2, tensor.ZZ());
//}


	
//##ModelId=3BC6995D0054
CMatrix::~CMatrix()
{
	Clear();
}
//////////////////////////////////////////////////////////////////////
/*


CMatrix CMatrix::GetStressRotMatdeg(const IVector &rotvector, double dangledeg)
{	//returns a stress rotation matrix. the object matrix is changed into a rotation matrix.
	MakeRotationMatrixdeg(rotvector, dangledeg);
	return FillStressRotation();
}


CMatrix CMatrix::GetStressRotMatrad(const IVector &rotvector, double danglerad)
{	//returns a stress rotation matrix. the object matrix is changed into a rotation matrix.
	MakeRotationMatrixrad(rotvector, danglerad);
	return FillStressRotation();
}
*/ 
/*
//##ModelId=3BC6995D0097
CMatrix CMatrix::GetStrainRotMatdeg(const IVector &rotvector, double dangledeg)
{	//returns a strain rotation matrix. the object matrix is changed into a rotation matrix.
	MakeRotationMatrixdeg(rotvector, dangledeg);
	return FillStrainRotation();
}

*/
void CMatrix::ReSize(int rows, int columns)
{
	// Reallocate and fill with zero's
	ReAllocate(rows, columns);

	for(int r = 0; r < rows; r++)
		for(int c  = 0; c < columns; c++)
			Value(r, c, 0.0);
}

void CMatrix::ReAllocate(int nRow, int nColumn)
{
	// Clear if we change size
	if((nRow != RowSize()) || (nColumn != ColumnSize()))
		Clear();

	// Try to allocate
	if(m_pData == 0)
	{
		if((nRow > 0) && (nColumn > 0))
		{
			int nSize = nRow * nColumn;
			m_pData = new double[nSize];
		}
	}

	// Set sizes
	m_rows = nRow;
	m_columns = nColumn;
}

void CMatrix::Clear()
{
	if(m_pData)
	{
		delete []m_pData;
		m_pData = 0;
	}

	m_rows = 0;
	m_columns = 0;

	assert(Empty());
}


const double& CMatrix::Value(int nRow, int nColumn) const
{
	// Configuration
	// | 0  1  2 |
	// | 3  4  5 |
	// | 6  7  8 |

	assert(nRow >= 0);
	assert(nRow < RowSize());
	assert(nColumn >= 0);
	assert(nColumn < ColumnSize());
	
	int pos = nRow * m_columns + nColumn;
	return m_pData[pos];
}
	
void CMatrix::Value(int nRow, int nColumn, const double& dValue) const
{
	// Configuration
	// | 0  1  2 |
	// | 3  4  5 |
	// | 6  7  8 |

	assert(nRow >= 0);
	assert(nRow < RowSize());
	assert(nColumn >= 0);
	assert(nColumn < ColumnSize());
	
	int pos = nRow * m_columns + nColumn;
	m_pData[pos] = dValue;
}

int CMatrix::RowSize() const
{
	return m_rows;
}


int CMatrix::ColumnSize() const
{
	return m_columns;
}

//////////////////////////////////////////////////////////////////////
// Implementation of CMatrixPair
//////////////////////////////////////////////////////////////////////
CMatrixPair::CMatrixPair(const CMatrix& first_matrix, const CMatrix& second_matrix)
: std::pair<CMatrix, CMatrix>(first_matrix, second_matrix)
{
}

CMatrixPair::CMatrixPair(const CMatrixPair& rhs)
: std::pair<CMatrix, CMatrix>(rhs)
{
}

}

