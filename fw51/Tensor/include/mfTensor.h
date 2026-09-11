#if !defined(AFX_TENSOR_H__INCLUDED_)
#define AFX_TENSOR_H__INCLUDED_

#include "mfITensor.h"

#include "TensorExports.h"

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description	: The CTensor object represents a tensor class (3 by 3 matrix)
// Status		: Finished (100%) 
// Remarks		: 
//				:  	
// Last review	: 27-03-2002 
// Update		: 11-06-2002, TNMA, added constructor (double, double, double, vector, vector, vector)
//				  and interface to tensor elements.
class TENSOR_EXPORT CTensor : public ITensor  
{
  double m_pData[6];
  bool   m_bEmpty;
public:
  //Deafault tensor is a 3 by 3 matrix.
  CTensor();

  //If a 3 by 3 matrix is passed into the constructor, a 
  //tensor is created with the exact same contents.
  CTensor(const geo::IMatrix &InitMat);

  // initialize the tensor with the given values, mind the order!
  CTensor(const double& xx,
      const double& yy,
      const double& zz,
      const double& xy, 
      const double& yz, 
      const double& xz);

  // creates a tensor in which all the elements have the value of the double...
  CTensor(const double& dInitVal);

  // Constructs a tensor in the x, y, z system from the eigenvalues and the eigenvectors...
  // (= Principal values and directions)
  CTensor(const double& dPrincVal1, const double& dPrincVal2, const double& dPrincVal3, const geo::IVector &xaxis, const geo::IVector &yaxis, const geo::IVector &zaxis);
  CTensor(const CTensor& rhs);
  CTensor(const ITensor& rhs);
  CTensor& operator=(const CTensor& rhs);

  //##ModelId=3BC6995B02D3
  virtual ~CTensor();

  // Interface to elements of the tensor...
  virtual const double& XX() const;
  virtual const double& YY() const;
  virtual const double& ZZ() const;
  virtual const double& XY() const;
  virtual const double& YZ() const;
  virtual const double& XZ() const;

  // The and only set function
  virtual	void Set(const double& xx,
           const double& yy,
           const double& zz,
           const double& xy, 
           const double& yz, 
           const double& xz);

  virtual void XX(const double& val);
  virtual void YY(const double& val);
  virtual void ZZ(const double& val);
  virtual void XY(const double& val);
  virtual void YZ(const double& val);
  virtual void XZ(const double& val);

  virtual bool Empty() const;
  virtual void Invalidate();
};

/**
 * Output the tensor to an ostream, added mainly for compatibility with OIV's MiDataSetI<> interface.
 */
inline std::ostream& operator<<(std::ostream& out, const CTensor& tensor)
{
  out << '[' << tensor.XX() << ", " << tensor.YY() << ", " << tensor.ZZ() << ", " << tensor.XY() << ", " << tensor.YZ() << ", " << tensor.XZ() << ']';
  return out;
}

#endif // !defined(AFX_TENSOR_H__INCLUDED_)
