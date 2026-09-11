#pragma once

#include <MeshVizXLM/MbVec3.h>

class OIDIGeometryI;
class OIDIVectorSetI;

/*
      Intersection 1
         /b\
    d /   \
       /a    \
    p3 - - p0        (BOTTOM FACE)
     /  v1 d1  \
    / v2        \
  p2-------------p1

  d: distance (from p2 to Intersection)
  a: alfa
  b: beta
  v2: vector p3-p2
  v2_norm = normalized v2
  v1: vector p1-p0
  d1: distance between p0 and p1

  Hexahedron
    Bottom face:
      p0: element.Point[0]
      p1: element.Point[1]
      p2: element.Point[2]
      p3: element.Point[3]
    Top face:
      p0: element.Point[4]
      p1: element.Point[5]
      p2: element.Point[6]
      p3: element.Point[7]

      Intersection 2
         /b\
    d /   \
       /a    \
    p7 - - p4        (TOP FACE)
     /  v1 d1  \
    / v2        \
  p6-------------p5

*/
class Algebra {
  static const double EPSILON;

public:
  // typedef std::map<const geo::IElement&, std::pair<MbVec3d,MbVec3d> > TAxisM;
  typedef std::map<int, std::pair<MbVec3d, MbVec3d>> TAxisM;

  enum eSurface { Top, Bottom };

  class Data {
  public:
    double alfa;
    double beta;
    double d1;
    double distance;
    MbVec3d intersection;

  public:
    Data() : alfa(0.0), beta(0.0), d1(0.0), distance(0.0), intersection(MbVec3d(0.0, 0.0, 0.0)) {}
    Data(double alfa_, double beta_, double d1_, double distance_, MbVec3d intersection_)
        : alfa(alfa_), beta(beta_), d1(d1_), distance(distance_), intersection(intersection_) {}
    Data(const Data &d) : alfa(d.alfa), beta(d.beta), d1(d.d1), distance(d.distance), intersection(d.intersection) {}
    void _print();
  };

  static Data m_data;

  static double dot(MbVec3d p1, MbVec3d p2) { return (p1[0] * p2[0] + p1[1] * p2[1] + p1[2] * p2[2]); }
  static double len(MbVec3d p) { return sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]); }
  static MbVec3d norm(MbVec3d p) {
    double len = Algebra::len(p);
    return MbVec3d(p[0] / len, p[1] / len, p[2] / len);
  }
  static MbVec3d intersection_eq(MbVec3d p, MbVec3d v2_norm, double distance) { return p + (v2_norm * distance); }
  static double cos_a(MbVec3d v1, MbVec3d v2) { return (dot(v1, v2) / (len(v1) * len(v2))); }
  static MbVec3d vector(MbVec3d p_ini, MbVec3d p_end) {
    return MbVec3d(p_end[0] - p_ini[0], p_end[1] - p_ini[1], p_end[2] - p_ini[2]);
  }
  static double alfa(MbVec3d v1, MbVec3d v2) { return acos(cos_a(v1, v2)); }
  static double beta(double alfa) { return M_PI - 2 * alfa; }
  static double distance_to_intersection(double distance1, double alfa, double beta) {
    return distance1 * (sin(alfa) / cos(beta));
  }
  static MbVec3d intersection_method_A(MbVec3d p0, MbVec3d p2, MbVec3d p3);
  static MbVec3d intersection(const geo::IElement &e, eSurface _type = Bottom);
  static MbVec3d intersection_method_A(const geo::IElement &e, eSurface _type = Bottom);
  static MbVec3d convert(const geo::IPoint &p) { return MbVec3d(p.X(), p.Y(), p.Z()); }

  static int fractional(double number, int number_of_decimal_places = 3) {
    double dummy;
    double frac = modf(number, &dummy);
    return round(frac * pow(10, number_of_decimal_places));
  }

  //---------------------- Aproach B: Triangle scaling

  static MbVec3d intersection_method_B(const geo::IElement &e, eSurface _type = Bottom);
  static void print_points(std::vector<MbVec3d> v, int size_ = 0);
  static void print_point(MbVec3d p);
  static void print_elem(const geo::IElement &e);
  static void print_point(const geo::IPoint &p);
  static std::vector<MbVec3d> scaled_elem(const geo::IElement &e, double scale_);
  static MbVec3d scaled_point(MbVec3d p, MbVec3d intersection_, double scale);
  static MbVec3d deformed_point(MbVec3d p, MbVec3d deformation, double scale);
  static double scale(const geo::IElement &e, eSurface _type = Bottom);
  static double scale(const geo::IPoint &p0, const geo::IPoint &p1, const geo::IPoint &p2, const geo::IPoint &p3);
  static double scale(MbVec3d p0, MbVec3d p1, MbVec3d p2, MbVec3d p3);
  static double equal(MbVec3d p0, MbVec3d p1);
  static std::vector<bool> iface_flags(const geo::IElementSet *elems);
  static TAxisM axis_map(const geo::IElementSet *elems);
  static std::vector<MbVec3d> points_mod_DEPRECATED(const OIDIGeometryI &g, OIDIVectorSetI *vectors, double scale_gen,
                                                    double scale_def);
  static std::vector<MbVec3d> points_mod(const OIDIGeometryI &g, std::vector<MbVec3d> deformation_v, double scale_gen,
                                         double scale_def);
  static std::vector<MbVec3d> points_mod_SAME(const OIDIGeometryI &g, std::vector<MbVec3d> deformation_v,
                                              double scale_gen, double scale_def);
  static std::vector<MbVec3d> convert(OIDIVectorSetI *vectors);
  static bool regular(const geo::IPoint *p);
  static bool iface(const geo::IPoint *p);
  static bool orphan(const geo::IPoint *p);
  static int index(const geo::IPoint *p);
  static bool outside(const geo::IPoint *p);
  static bool inside(const geo::IPoint *p);
  static void geometry_points_TEST(const OIDIGeometryI &g);
  static MbVec3d min(std::vector<MbVec3d> points_v);
  static MbVec3d max(std::vector<MbVec3d> points_v);
  static std::pair<MbVec3d, MbVec3d> min_max(std::vector<MbVec3d> points_v);
};
