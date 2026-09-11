#include "IT_Data.h"
/*
  Data object that encapsules some reservoir properties: points, triangles, tehtrahedrons, parameters and header.

  Memebers:
  ---------
    points:
    triangles:
    tehtrahedrons:
    parameters:
    header:

  Header member defines with parameters member content.
*/
class Reservoir : public IT_Data {
public:
  enum eElemType { Tri, Tetra };

private:
  enum Type { Points, Elems, Params, Num_Objects };

  eElemType m_elem_type;

  int m_num_values;

  // data
  TPointsV m_points;
  TTetraV m_tetra;
  TTriV m_tri;
  T4ValuesV m_params;

  // data ptr
  TData points_d();
  TData tri_d();
  TData tetra_d();
  TData params_d();

public:
  Reservoir(eElemType val = Tetra);
  // set
  void points(TPointsV _points) { m_points = _points; };
  void tri(TTriV val) { m_tri = val; };
  void tetra(TTetraV val) { m_tetra = val; };
  void params(T4ValuesV val) { m_params = val; };
  // get
  TPointsV &points() { return m_points; };
  TTetraV &tetra() { return m_tetra; };
  T4ValuesV &params() { return m_params; };

  virtual TData object(int _type = 0);
  virtual int num_objects() { return Type::Num_Objects; };
};
