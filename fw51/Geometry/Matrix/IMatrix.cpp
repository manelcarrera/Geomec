 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IMatrix.cpp: implementation of the IMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include <cassert>
#include <cmath>
#include "IObject.h"
#include "Matrix.h"
#include "Vector.h"

#define EPS1	1E-9
#define EPS2	1E-6

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {
IMatrix::IMatrix()
{

}

IMatrix::~IMatrix()
{

}

bool IMatrix::Empty() const
{
	return ((ColumnSize() == 0) || (RowSize() == 0));
}

CMatrix IMatrix::GetTransposed() const
{
	CMatrix ret(ColumnSize(), RowSize());

	for(int r = 0; r < RowSize(); r++)
	{
		for(int c = 0; c < ColumnSize(); c++)
		{
			ret.Value(c, r, Value(r, c));
		}
	}
	return ret;
}

void IMatrix::CreateIdentityMatrix(int nSize)
{
	// Resize default fills with zero's
	ReSize(nSize, nSize);

	for(int i = 0; i < nSize; i++)
		Value(i, i, 1.0);
}

void IMatrix::CreateRotationMatrixDeg(const IVector &rot_vector, const double& dAngle)
{
	CreateRotationMatrixRad(rot_vector, dAngle * DEG_TO_RAD);
}

void IMatrix::CreateRotationMatrixRad(const IVector &rot_vector, const double& dAngle)
{
	ReSize(3,3);
	double cosine = cos(dAngle);
	double sine = sin(dAngle);
	CVector unitvec = rot_vector.UnitVector();
	Value(0, 0,  (1 - cosine) * pow(unitvec.X(),2) + cosine);
	Value(0, 1,  (1 - cosine) * unitvec.X() * unitvec.Y() - sine * unitvec.Z());
	Value(0, 2,  (1 - cosine) * unitvec.X() * unitvec.Z() + sine * unitvec.Y());
	Value(1, 0,  (1 - cosine) * unitvec.X() * unitvec.Y() + sine * unitvec.Z());
	Value(1, 1,  (1 - cosine) * pow(unitvec.Y(),2) + cosine);
	Value(1, 2,  (1 - cosine) * unitvec.Y() * unitvec.Z() - sine * unitvec.X());
	Value(2, 0,  (1 - cosine) * unitvec.X() * unitvec.Z() - sine * unitvec.Y());
	Value(2, 1,  (1 - cosine) * unitvec.Y() * unitvec.Z() + sine * unitvec.X());
	Value(2, 2,  (1 - cosine) * pow(unitvec.Z(),2) + cosine);
}

void IMatrix::NullMatrix()
{
	for(int i = 0; i < RowSize(); i++)
	{
		for(int j = 0; j < ColumnSize(); j++)
		{
			Value(i, j, 0.0);
		}
	}
}

CMatrix IMatrix::GetInverse() const//uses Gauss elimination with partial pivoting for matrices of order 3 and higher.
{
	double D = GetDeterminant();
	assert(fabs(D) > 0);

	int i;
	int j;
	int k;
	int l;
	int m;
	int irowidx;
	int irownr;
	double dmaxim;
	double dmultfact;
	CMatrix Inverse(RowSize(), ColumnSize());

	if(ColumnSize() == 2)
	{
		Inverse.Value(0, 0,  Value(1, 1) / D);
		Inverse.Value(0, 1, -Value(0, 1) / D);
		Inverse.Value(1, 0, -Value(1, 0) / D);
		Inverse.Value(1, 1,  Value(0, 0) / D);
	}
	else if(ColumnSize() == 3)
	{
		Inverse.Value(0, 0, (Value(1, 1) * Value(2, 2) - Value(1, 2) * Value(2, 1)) / D);
		Inverse.Value(0, 1, (Value(0, 2) * Value(2, 1) - Value(0, 1) * Value(2, 2)) / D);
		Inverse.Value(0, 2, (Value(0, 1) * Value(1, 2) - Value(0, 2) * Value(1, 1)) / D);
		Inverse.Value(1, 0, (Value(1, 2) * Value(2, 0) - Value(1, 0) * Value(2, 2)) / D);
		Inverse.Value(1, 1, (Value(0, 0) * Value(2, 2) - Value(0, 2) * Value(2, 0)) / D);
		Inverse.Value(1, 2, (Value(0, 2) * Value(1, 0) - Value(0, 0) * Value(1, 2)) / D);
		Inverse.Value(2, 0, (Value(1, 0) * Value(2, 1) - Value(1, 1) * Value(2, 0)) / D);
		Inverse.Value(2, 1, (Value(0, 1) * Value(2, 0) - Value(0, 0) * Value(2, 1)) / D);
		Inverse.Value(2, 2, (Value(0, 0) * Value(1, 1) - Value(0, 1) * Value(1, 0)) / D);
	}
	else if(ColumnSize() > 3)
	{
		CMatrix mattemp(RowSize(), 2 * ColumnSize());

		for(i = 0; i < ColumnSize(); i++)
		{
			for(j = 0; j < RowSize(); j++)
			{
				mattemp.Value(i, j, Value(i, j));
			}
		}

		for(i = 0; i < RowSize(); i++)
		{
			mattemp.Value(i, i + ColumnSize(), 1.0);
		}

		j = 0;
		for(i = 0; i < mattemp.RowSize(); i++)
		{
			dmaxim = 0.0;
			irownr = 0;
			for(irowidx = i; irowidx < mattemp.RowSize(); irowidx++)//Find row with highest pivot.
			{
				if(fabs(mattemp.Value(irowidx, j)) > dmaxim)
				{
					dmaxim = fabs(mattemp.Value(irowidx, j)); 
					irownr = irowidx;
				}
			}

			std::vector<double> v(mattemp.ColumnSize());
			for(k = j; k < mattemp.ColumnSize(); k++)
			{
				v[k] = mattemp.Value(irownr, k);
				mattemp.Value(irownr, k, mattemp.Value(i, k));
				mattemp.Value(i, k, v[k] / v[j]);
			}

			for(l = 0; l < i; l++)//subtract row from all other rows.
			{
				dmultfact = mattemp.Value(l, j);
				for(m = j; m < mattemp.ColumnSize(); m++)
				{
					mattemp.Value(l, m, mattemp.Value(l, m) - mattemp.Value(i, m) * dmultfact);
				}
			}

			for(l = i+1; l < mattemp.RowSize(); l++)
			{
				dmultfact = mattemp.Value(l, j);
				for(m = j; m < mattemp.ColumnSize(); m++)
				{
					mattemp.Value(l, m, mattemp.Value(l, m) - mattemp.Value(i, m) * dmultfact);
				}
			}

			j++;
		}

		for(i = 0; i < RowSize(); i++)
		{
			for(j = 0; j < ColumnSize(); j++)
			{
				Inverse.Value(i, j, mattemp.Value(i, j + ColumnSize()));
			}
		}

	}
	return Inverse;	

}

CMatrixPair IMatrix::GetEigenPairs(bool bCreateRightHandSystem /* = false */) const	
/*Jacobi iteration for real symmetric matrix. Returns a pair of CMatrices, first containing the eigenvalues 
on the main diagonal, second containing the corresponding eigenvectors (in columns). If the eigenvector matrix 
is of order three and bCreateRightHandSystem = true, a check is carried out if the vectors still span a right hand side system. If not, the third vector
has its sign switched.*/
{
	int i;
	int j;
	int k;
	int iitr;
	int irownr;
	int icolumnnr;
	int iidx = 0;
	int isign;
	double dsigma;
	double dsignor;
	double delmax;
	double delmin;
	double dLambda;
	double dMu;
	double dcosin;
	double dsinus;
	double dQ;
	double dR;
	double dX;
	double dY;
	double dtemp;
	const int imitr = 50;

	CMatrix Diagonal(*this), Rotation;
	
	assert(Diagonal == Diagonal.GetTransposed());		//matrix has to be symmetrical.

	if(RowSize() == 2)
	{	
		Rotation.ReSize(2, 2);
		double dAlpha;
		if(fabs(Value(0, 0) - Value(1, 1)) < EPS1)
			dAlpha = 0.5 * PI; // Alpha is set to ninety degrees....
		else
			dAlpha = 0.5 * atan((2 * Value(1, 0)) / (Value(0, 0) - Value(1, 1)));
		double dcos = cos(dAlpha);
		double dsin = sin(dAlpha);
		double Sig1 = pow(dcos, 2) * Value(0, 0) + pow(dsin, 2) * Value(1, 1) + 2 * dcos * dsin * Value(1, 0);
		double Sig2 = pow(dsin, 2) * Value(0, 0) + pow(dcos, 2) * Value(1, 1) - 2 * dcos * dsin * Value(1, 0);

		if(Sig1 < Sig2)
		{
			Diagonal.Value(0, 0, Sig1);
			Diagonal.Value(1, 1, Sig2);
		}
		else
		{
			Diagonal.Value(0, 0, Sig2);
			Diagonal.Value(1, 1, Sig1);
		}
		
		Diagonal.Value(1, 0, 0.0);
		Diagonal.Value(0, 1, 0.0);
		Rotation.Value(0, 0, -1 * dsin);
		Rotation.Value(1, 0, dcos);
		Rotation.Value(0, 1, dcos);
		Rotation.Value(1, 1, dsin);

		return CMatrixPair(Diagonal, Rotation);
	}

	Rotation.CreateIdentityMatrix(RowSize());

	dsigma = 0.0;
	for(i = 0; i < RowSize(); i++)
	{
		for(j = 0; j < RowSize(); j++)
		{
			dsigma += Diagonal.Value(i, j) * Diagonal.Value(i, j);
		}
	}

	// dsigma is always positive...
	dsignor = sqrt(dsigma);
	
	// dsignor is either 0 or > 0....
	if(dsignor == 0.0)
	{
		// a 'this' matrix consits of only zeros, trivial...
		Diagonal.NullMatrix();
		Rotation.NullMatrix();
		return CMatrixPair(Diagonal, Rotation);
	}
	
	Diagonal = Diagonal * (1.0 / dsignor);

	for(iitr = 0; iitr < imitr; iitr++)	//maximum of mitr iterations.
	{
		delmax    = 0;
        irownr    = 0;
        icolumnnr = 0;
		for(i = 0; i < RowSize()-1; i++)
		{
			for(j = i + 1; j < ColumnSize(); j++)
			{
				if(fabs(Diagonal.Value(i, j)) > delmax)
				{
					delmax = fabs(Diagonal.Value(i, j));
					irownr    = i;
					icolumnnr = j;
				}
			}
		}
		
		if(delmax < EPS2) break;			//largest element of off-diagonal terms is small enough.

		dLambda = Diagonal.Value(irownr, icolumnnr);
		dMu = 0.5 * (Diagonal.Value(irownr, irownr) - Diagonal.Value(icolumnnr, icolumnnr));
		dR = 0.5 / sqrt(dLambda * dLambda + dMu * dMu);
		dcosin = sqrt(0.5 + fabs(dMu) * dR);

		if(fabs(dMu) < EPS1)
		{
			dMu = 0.0;
			dQ = fabs(dR);
		}
        else if ( dMu < 0.0)
        {
           dQ = -1 * fabs(dR);
        }
        else
        {
		   dQ = fabs(dR);
        }
		
		dsinus = -1 * dLambda * dQ / dcosin;

		for(i = 0; i < RowSize(); i++)
		{
			dX = Diagonal.Value(irownr, i);
			dY = Diagonal.Value(icolumnnr, i);
			Diagonal.Value(irownr, i, dX * dcosin - dY * dsinus);
			Diagonal.Value(icolumnnr, i, dX * dsinus + dY * dcosin);
		}

		for(i = 0; i < RowSize(); i++)
		{
			dX = Diagonal.Value(i, irownr);
			dY = Diagonal.Value(i, icolumnnr);
			Diagonal.Value(i, irownr, dX * dcosin - dY * dsinus);
			Diagonal.Value(i, icolumnnr, dX * dsinus + dY * dcosin);
		}

		for(i = 0; i < RowSize(); i++)
		{
			dX = Rotation.Value(i, irownr);
			dY = Rotation.Value(i, icolumnnr);
			Rotation.Value(i, irownr, dX * dcosin - dY * dsinus);
			Rotation.Value(i, icolumnnr, dX * dsinus + dY * dcosin);
		}
	}

	Diagonal = Diagonal * dsignor;
	
	//sort eigenvalues, smallest value left, largest right:
	for(i = 0; i < RowSize(); i++)			
	{
		delmin = Diagonal.Value(i, i);
		for(j = i + 1; j < RowSize(); j++)
		{
			if(Diagonal.Value(j, j) < delmin) 
			{
				delmin = Diagonal.Value(j, j);
				Diagonal.Value(j, j, Diagonal.Value(i, i));
				Diagonal.Value(i, i, delmin);
				//switch the eigenvectors in the matrix according to the way the eigenvalues are sorted:
				for(k = 0; k < ColumnSize(); k++)		
				{
					dtemp = Rotation.Value(k, i);
					Rotation.Value(k, i, Rotation.Value(k, j));
					Rotation.Value(k, j, dtemp);
				}
			}
		}
	}
	
	if(RowSize() == 3 && bCreateRightHandSystem)		//Check if eigenvector matrix still spans right hand side system, change sign of third vector if not.
	{
		CVector v1, v2;
		v1.X(Rotation.Value(0, 0));
		v1.Y(Rotation.Value(1, 0));
		v1.Z(Rotation.Value(2, 0));
		v2.X(Rotation.Value(0, 1));
		v2.Y(Rotation.Value(1, 1));
		v2.Z(Rotation.Value(2, 1));
		
		CVector v3new = v1.CrossProduct(v2);

		delmax = 0.0;
		for(i = 0; i < RowSize(); i++)
		{
			if(fabs(Rotation.Value(i, 2)) > delmax)
			{
				delmax = fabs(Rotation.Value(i, 2));
				iidx = i;
			}
		}

		if(iidx == 0)
		{
			assert((fabs(v3new.X()) > EPS1) && (fabs(Rotation.Value(0, 2)) > EPS1));		//Don't divide by zero!!
			isign = (Rotation.Value(0, 2) * v3new.X() < 0 ? -1 : 1 );

			for(i = 0; i < RowSize(); i++)
			{
				Rotation.Value(i, 2, Rotation.Value(i, 2) * isign);
			}
		}
		else if(iidx == 1)
		{
			assert((fabs(v3new.Y()) > EPS1) && (fabs(Rotation.Value(1, 2)) > EPS1));		//Don't divide by zero!!
			isign = (Rotation.Value(1, 2) * v3new.Y() < 0 ? -1 : 1 );

			for(i = 0; i < RowSize(); i++)
			{
				Rotation.Value(i, 2, Rotation.Value(i, 2) * isign);
			}
		}
		else 
		{
			assert((fabs(v3new.Z()) > EPS1) && (fabs(Rotation.Value(2, 2)) > EPS1));		//Don't divide by zero!!
			isign = (Rotation.Value(2, 2) * v3new.Z() < 0 ? -1 : 1 );

			for(i = 0; i < RowSize(); i++)
			{
				Rotation.Value(i, 2, Rotation.Value(i, 2) * isign);
			}
		}
	}

	return CMatrixPair(Diagonal, Rotation);
}

CMatrix IMatrix::EigenValues() const
{
	return GetEigenPairs(false).first;
}


CMatrix IMatrix::EigenVectors(bool bCreateRightHandSystem /* = false */) const
{
	return GetEigenPairs(bCreateRightHandSystem).second;
}

double IMatrix::GetDeterminant() const/*For matrices of order 4 and higher the determinant is calculated by 
										creating an U matrix and multiplying the diagonal terms.*/
{
	assert(RowSize() == ColumnSize());
	assert(RowSize() > 0);
	
	int i;
	double dDet;
	switch(RowSize())
	{
	case 1:
		dDet = Value(0, 0);
		break;
	case 2:
		dDet = Value(0,0) * Value(1,1) - Value(0, 1) * Value(1,0);
		break;
	case 3:
		dDet =	Value(0, 0) * (Value(1, 1) * Value(2, 2) - Value(2, 1) * Value(1, 2)) 
				- Value(0, 1) * (Value(1, 0) * Value(2, 2) - Value(2, 0) * Value(1, 2))	
				+ Value(0, 2) * (Value(1, 0) * Value(2, 1) - Value(1, 1) * Value(2, 0));
		break;
	default:
		{
			// The upper matrix
			CMatrix upper = UpperMatrix();
			dDet = 1;
			
			for(i = 0; i < upper.RowSize(); i++)
				dDet *= upper.Value(i, i);
		}
		break;
	}
			
	
	
	return dDet;
}

CMatrix IMatrix::UpperMatrix() const
{
	int i;
	int k;
	int l;
	int irownr;
	int irowidx;
	double dmaxim;
	double dmultfact;

	// Just making a copy ...
	CMatrix upper_matrix(*this);

	for(i = 0; i < upper_matrix.RowSize(); i++)
	{
		dmaxim = 0;
		irownr = 0;
		//Find row with highest pivot.
		for(irowidx = i; irowidx < upper_matrix.RowSize(); irowidx++)		
		{
			if(fabs(upper_matrix.Value(irowidx,i)) > dmaxim)
			{
				dmaxim = fabs(upper_matrix.Value(irowidx, i)); 
				irownr = irowidx;
			}
		}

		//switch current row with row with highest pivot.
		if(i != irownr)
		{
			for(k = i; k < upper_matrix.ColumnSize(); k++) 
			{
				double tmp  = upper_matrix.Value(i, k);
				upper_matrix.Value(i, k, upper_matrix.Value(irownr, k));

				//if rows are switched then the sign of the determinant also switch.
				//so switch sign.
				upper_matrix.Value(irownr, k, -tmp);
			}
		}
		
		//create upper matrix.
		for(l = i + 1; l < upper_matrix.RowSize(); l++)				
		{
			//Do not divide by zero!!
			if( fabs(upper_matrix.Value(i, i)-upper_matrix.Value(l, i) ) < EPS)
				dmultfact=1.0;
			else
				dmultfact = upper_matrix.Value(l, i) / upper_matrix.Value(i, i);

			for(k = i; k < upper_matrix.ColumnSize(); k++)
			{
				upper_matrix.Value(l, k, upper_matrix.Value(l, k) - upper_matrix.Value(i, k) * dmultfact);
			}
		}
		
	}
	
	return upper_matrix;
}

CMatrix IMatrix::StressRotation() const
{
	// Get stress rotation from 3 x 3 matrix
	assert((ColumnSize() == 3) && (RowSize() == 3));

	CMatrix ret(6,6);

	//First column:
	ret.Value(0, 0,  Value(0, 0) * Value(0, 0));
	ret.Value(1, 0,  Value(0, 1) * Value(0, 1));
	ret.Value(2, 0,  Value(0, 2) * Value(0, 2));
	ret.Value(3, 0,  Value(0, 0) * Value(0, 1));
	ret.Value(4, 0,  Value(0, 1) * Value(0, 2));
	ret.Value(5, 0,  Value(0, 2) * Value(0, 1));
	//Second column:
	ret.Value(0, 1,  Value(1, 0) * Value(1, 0));
	ret.Value(1, 1,  Value(1, 1) * Value(1, 1));
	ret.Value(2, 1,  Value(1, 2) * Value(1, 2));
	ret.Value(3, 1,  Value(1, 0) * Value(1, 1));
	ret.Value(4, 1,  Value(1, 1) * Value(1, 2));
	ret.Value(5, 1,  Value(1, 2) * Value(1, 0));
	//Third column:
	ret.Value(0, 2,  Value(2, 0) * Value(2, 0));
	ret.Value(1, 2,  Value(2, 1) * Value(2, 1));
	ret.Value(2, 2,  Value(2, 2) * Value(2, 2));
	ret.Value(3, 2,  Value(2, 0) * Value(2, 1));
	ret.Value(4, 2,  Value(2, 1) * Value(2, 2));
	ret.Value(5, 2,  Value(2, 2) * Value(2, 0));
	//Fourth column:
	ret.Value(0, 3,  2.0 * Value(0, 0) * Value(1, 0));
	ret.Value(1, 3,  2.0 * Value(0, 1) * Value(1, 1));
	ret.Value(2, 3,  2.0 * Value(0, 2) * Value(1, 2));
	ret.Value(3, 3,  Value(0, 0) * Value(1, 1) + Value(0, 1) * Value(1, 0));
	ret.Value(4, 3,  Value(0, 1) * Value(1, 2) + Value(0, 2) * Value(1, 1));
	ret.Value(5, 3,  Value(0, 2) * Value(1, 0) + Value(0, 0) * Value(1, 2));
	//Fifth column:
	ret.Value(0, 4,  2.0 * Value(1, 0) * Value(2, 0));
	ret.Value(1, 4,  2.0 * Value(1, 1) * Value(2, 1));
	ret.Value(2, 4,  2.0 * Value(1, 2) * Value(2, 2));
	ret.Value(3, 4,  Value(1, 0) * Value(2, 1) + Value(1, 1) * Value(2, 0));
	ret.Value(4, 4,  Value(1, 1) * Value(2, 2) + Value(1, 2) * Value(2, 1));
	ret.Value(5, 4,  Value(1, 2) * Value(2, 0) + Value(1, 0) * Value(2, 2));
	//Sixth column:
	ret.Value(0, 5,  2.0 * Value(2, 0) * Value(0, 0));
	ret.Value(1, 5,  2.0 * Value(2, 1) * Value(0, 1));
	ret.Value(2, 5,  2.0 * Value(2, 2) * Value(0, 2));
	ret.Value(3, 5,  Value(2, 0) * Value(0, 1) + Value(2, 1) * Value(0, 0));
	ret.Value(4, 5,  Value(2, 1) * Value(0, 2) + Value(2, 2) * Value(0, 1));
	ret.Value(5, 5,  Value(2, 2) * Value(0, 0) + Value(2, 0) * Value(0, 2));

	return ret;
}

CMatrix IMatrix::StrainRotation() const
{
	// Get strain rotation from 3 x 3 matrix
	assert((ColumnSize() == 3) && (RowSize() == 3));

	CMatrix ret(6,6);

	//First column:
	ret.Value(0, 0,  Value(0, 0) * Value(0, 0));
	ret.Value(1, 0,  Value(0, 1) * Value(0, 1));
	ret.Value(2, 0,  Value(0, 2) * Value(0, 2));
	ret.Value(3, 0,  2.0 * Value(0, 0) * Value(0, 1));
	ret.Value(4, 0,  2.0 * Value(0, 1) * Value(0, 2));
	ret.Value(5, 0,  2.0 * Value(0, 2) * Value(0, 1));
	//Second column:
	ret.Value(0, 1,  Value(1, 0) * Value(1, 0));
	ret.Value(1, 1,  Value(1, 1) * Value(1, 1));
	ret.Value(2, 1,  Value(1, 2) * Value(1, 2));
	ret.Value(3, 1,  2.0 * Value(1, 0) * Value(1, 1));
	ret.Value(4, 1,  2.0 * Value(1, 1) * Value(1, 2));
	ret.Value(5, 1,  2.0 * Value(1, 2) * Value(1, 0));
	//Third column:
	ret.Value(0, 2,  Value(2, 0) * Value(2, 0));
	ret.Value(1, 2,  Value(2, 1) * Value(2, 1));
	ret.Value(2, 2,  Value(2, 2) * Value(2, 2));
	ret.Value(3, 2,  2.0 * Value(2, 0) * Value(2, 1));
	ret.Value(4, 2,  2.0 * Value(2, 1) * Value(2, 2));
	ret.Value(5, 2,  2.0 * Value(2, 2) * Value(2, 0));
	//Fourth column:
	ret.Value(0, 3,  Value(0, 0) * Value(1, 0));
	ret.Value(1, 3,  Value(0, 1) * Value(1, 1));
	ret.Value(2, 3,  Value(0, 2) * Value(1, 2));
	ret.Value(3, 3,  Value(0, 0) * Value(1, 1) + Value(0, 1) * Value(1, 0));
	ret.Value(4, 3,  Value(0, 1) * Value(1, 2) + Value(0, 2) * Value(1, 1));
	ret.Value(5, 3,  Value(0, 2) * Value(1, 0) + Value(0, 0) * Value(1, 2));
	//Fifth column:
	ret.Value(0, 4,  Value(1, 0) * Value(2, 0));
	ret.Value(1, 4,  Value(1, 1) * Value(2, 1));
	ret.Value(2, 4,  Value(1, 2) * Value(2, 2));
	ret.Value(3, 4,  Value(1, 0) * Value(2, 1) + Value(1, 1) * Value(2, 0));
	ret.Value(4, 4,  Value(1, 1) * Value(2, 2) + Value(1, 2) * Value(2, 1));
	ret.Value(5, 4,  Value(1, 2) * Value(2, 0) + Value(1, 0) * Value(2, 2));
	//Sixth column:
	ret.Value(0, 5,  Value(2, 0) * Value(0, 0));
	ret.Value(1, 5,  Value(2, 1) * Value(0, 1));
	ret.Value(2, 5,  Value(2, 2) * Value(0, 2));
	ret.Value(3, 5,  Value(2, 0) * Value(0, 1) + Value(2, 1) * Value(0, 0));
	ret.Value(4, 5,  Value(2, 1) * Value(0, 2) + Value(2, 2) * Value(0, 1));
	ret.Value(5, 5,  Value(2, 2) * Value(0, 0) + Value(2, 0) * Value(0, 2));

	return ret;
}

CMatrix IMatrix::RotateMatStiffnessDeg(const IVector &rot_vector, const double& dAngle)
{	//Will return a Rotated 6x6 matrix. Any 6x6 matrix is accepted. Make sure you pass the right one. 
	assert(RowSize() == 6 && ColumnSize() == 6);

	CMatrix rot_mat;
	rot_mat.CreateRotationMatrixDeg(rot_vector, dAngle);

	return rot_mat.StressRotation().GetInverse() * (*this * rot_mat.StrainRotation());
}

CMatrix IMatrix::RotateMatStiffnessRad(const IVector &rot_vector, const double& dAngle)
{	//Will return a Rotated 6x6 matrix. Any 6x6 matrix is accepted. Make sure you pass the right one. 
	assert(RowSize() == 6 && ColumnSize() == 6);

	CMatrix rot_mat;
	rot_mat.CreateRotationMatrixRad(rot_vector, dAngle);

	return rot_mat.StressRotation().GetInverse() * (*this * rot_mat.StrainRotation());
}

CMatrix IMatrix::operator *(const IMatrix &rhs) const
{
	assert(ColumnSize() == rhs.RowSize());

	CMatrix ret(RowSize(), rhs.ColumnSize());

	for(int i = 0; i < RowSize(); i++)
	{
		for(int j = 0; j < rhs.ColumnSize(); j++)
		{
			double ddummy = 0;
			for(int k = 0; k < rhs.RowSize(); k++)
				ddummy += Value(i, k) * rhs.Value(k, j);
			
			ret.Value(i, j, ddummy);
		}
	}
	return ret;
}

//##ModelId=3BC6995D00CF
CMatrix IMatrix::operator *(const double &rhs) const
{
	CMatrix ret(RowSize(), ColumnSize());

	for(int r = 0; r < RowSize(); r++)
		for(int c = 0; c < ColumnSize(); c++)
			ret.Value(r, c, Value(r, c) * rhs);
		
	return ret;
}

CVector IMatrix::operator *(const IVector &rhs) const
{
	assert(RowSize() == 3);
	assert(ColumnSize() == 3);

	CMatrix vector(3, 1);
	vector.Value(0, 0, rhs.X());
	vector.Value(1, 0, rhs.Y());
	vector.Value(2, 0, rhs.Z());

	CMatrix ret = *this * vector;
	assert(ret.RowSize() == 3);
	assert(ret.ColumnSize() == 1);

	return CVector(ret.Value(0, 0), ret.Value(1, 0), ret.Value(2, 0));
}

//##ModelId=3BC6995D00D1
CMatrix IMatrix::operator +(const IMatrix &rhs) const
{
	assert(RowSize() == rhs.RowSize() && ColumnSize() == rhs.ColumnSize());

	CMatrix ret(RowSize(), ColumnSize());

	for(int r = 0; r < rhs.RowSize(); r++)
		for(int c = 0; c < rhs.ColumnSize(); c++)
			ret.Value(r, c, Value(r, c) + rhs.Value(r, c));

	return ret;
}

CMatrix IMatrix::operator -(const IMatrix &rhs) const
{
	assert(RowSize() == rhs.RowSize() && ColumnSize() == rhs.ColumnSize());

	CMatrix ret(RowSize(), ColumnSize());

	for(int r = 0; r < rhs.RowSize(); r++)
		for(int c = 0; c < rhs.ColumnSize(); c++)
			ret.Value(r, c, Value(r, c) - rhs.Value(r, c));

	return ret;
}


bool IMatrix::operator ==(const IMatrix &rhs) const
{
	// Size equal?
	if((RowSize() != rhs.RowSize()) || (ColumnSize() != rhs.ColumnSize()))
		return false;

	// Check values
	for(int r = 0; r < rhs.RowSize(); r++)
	{
		for(int c = 0; c < rhs.ColumnSize(); c++)
		{
			if(Value(r, c) - rhs.Value(r, c) > EPS1)
				return false;
		}
	}

	return true;
}


IMatrix& IMatrix::operator =(const IMatrix &rhs) 
{
	ReSize(rhs.RowSize(), rhs.ColumnSize());

	for(int r = 0; r < rhs.RowSize(); r++)
		for(int c = 0; c < rhs.ColumnSize(); c++)
			Value(r, c, rhs.Value(r, c));
	return *this;
}


bool IMatrix::operator !=(const IMatrix &rhs) const
{
	return !(this == &rhs);
}

}

