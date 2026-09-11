 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Matrix.h: interface for the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__4AA3CF7C_C2CF_4EAC_8F1D_446C7752BB96__INCLUDED_)
#define AFX_MATRIX_H__4AA3CF7C_C2CF_4EAC_8F1D_446C7752BB96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "IMatrix.h"

#include "GeometryExports.h"

//Matrix class. Standard Matrix operations implemented like 
//determinant, inverse, eigenvalues, eigenvectors plus some 
//additional functionality like construction of rotation 
//matrix.

namespace geo{

class IVector;

class GEOMETRY_EXPORT  CMatrix : public IMatrix
{
	//##ModelId=3BC6995D00E1
	int m_rows;									// Nr of rows of the matrix
	//##ModelId=3BC6995D00E2
	int m_columns;								// Nr of columns of the matrix
	double* m_pData;							// Array of size rows * columns with doubles
	void ReAllocate(int nRow, int nColumn);		// Function to allocate the array
	void Clear();								// Function to clear the matrix to (0, 0)
public:
	//Default constructor: creates a 0 by 0 matrix. Use 
	//ReSize() to adjust the size of the matrix.
	CMatrix();

	// Create a matrix from another unknown
	CMatrix(const IMatrix& rhs);
	CMatrix(const CMatrix& rhs);
	//CMatrix(const ITensor& rhs);
	//If two integers are passed (i, j) into the constructor 
	//: creates an i by j matrix filled with zeros.
	//##ModelId=3BC6995D0045
	CMatrix(int rows, int columns);
	//##ModelId=3BC6995D0054
        CMatrix( const IVector& v1, const IVector& v2, const IVector& v3 );
	virtual ~CMatrix();

	CMatrix& operator =(const CMatrix &rhs);

	//Returns nr. of rows.
	virtual int RowSize() const;
	//Returns nr. of columns.
	virtual int ColumnSize() const;

	virtual void ReSize(int rows, int columns);
	virtual const double& Value(int nRow, int nColumn) const;
	virtual void Value(int nRow, int nColumn, const double& dValue) const;
private:

	//##ModelId=3BC6995D00E3
	CMatrix FillStressRotation();
	//##ModelId=3BC6995D00EF
	CMatrix FillStrainRotation();

};

class GEOMETRY_EXPORT  CMatrixPair : public std::pair<CMatrix, CMatrix>
{
public:
	CMatrixPair(const CMatrix& first_matrix, const CMatrix& second_matrix);
	CMatrixPair(const CMatrixPair& rhs);
};


}
#endif // !defined(AFX_MATRIX_H__4AA3CF7C_C2CF_4EAC_8F1D_446C7752BB96__INCLUDED_)
