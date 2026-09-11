
#include "EclipseBody.h"
#include "IMesh.h"

// bodyface names
static const char XI1[]   = "XI1";
static const char XI2[]   = "XI2";
static const char ETA1[]  = "ETA1";
static const char ETA2[]  = "ETA2";
static const char ZETA1[] = "ZETA1";
static const char ZETA2[] = "ZETA2";

#define SQRT3_6 (0.28867513459481288225457439025098)

CEclipseBody::CEclipseBody(geo::IMesh &mesh, const std::vector<int> &points)
: m_mesh(mesh), m_vcNodes(points)
{
}

CEclipseBody::~CEclipseBody()
{
}

const geo::IPoint &CEclipseBody::Point(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfPoints());
	return m_mesh.Node(m_vcNodes[nIndex]);
}

void CEclipseBody::Point(int nIndex, const geo::IPoint &pt)
{
	assert(nIndex >= 0 && nIndex < NrOfPoints());
	m_mesh.Node(nIndex, pt);
}

/*
geo::CPoint CEclipseBody::GlobalFromIsoparametric(const double &xi, const double &eta, const double &zeta) const
{
	geo::CPoint ptret(
		ShapeFunction(xi, eta, zeta, geo::IPoint::X),
		ShapeFunction(xi, eta, zeta, geo::IPoint::Y),
		ShapeFunction(xi, eta, zeta, geo::IPoint::Z));

	return ptret;
}

// returns the xi, eta and zeta for the point in a CPoint
// if xi, eta or zeta are outside -0.5 to 0.5 the point is NOT in the element
// returns empty point if maximum number of iterations is exceeded
geo::CPoint CEclipseBody::IsoparametricFromGlobal(const geo::IPoint &point) const
{
	// a little iteration needs to be done to find the right xi, eta, zeta values for the given point

	// define a maximum number of iterations to prevent deadlocking
	const int maxiter = 10;

	double xi = 0;
	double eta = 0;
	double zeta = 0;

	geo::CPoint estimate = GlobalFromIsoparametric(xi, eta, zeta);
	int iter = 0;

	geo::CPoint diff(estimate - point);

	while(fabs(diff.X()) > 1e-8 || fabs(diff.Y()) > 1e-8 || fabs(diff.Z()) > 1e-8)
	{
		if(++iter > maxiter) return geo::CPoint();

		// the delta values
		geo::CVector delta(
			point.X() - estimate.X(),
			point.Y() - estimate.Y(),
			point.Z() - estimate.Z());

		// the matrix with derivatives
		geo::CMatrix mat(3, 3);

		Jacobian(mat, xi, eta, zeta);

		// invert it, and multiply by the delta vector to get
		// delta in terms of xi, eta and zeta
		geo::CMatrix matinv = mat.GetInverse();
		geo::CVector delta_iso = matinv * delta;

		xi   += delta_iso.X();
		eta  += delta_iso.Y();
		zeta += delta_iso.Z();

//		if(xi < -0.5) xi = -0.5;
//		if(xi > 0.5) xi = 0.5;
//		if(eta < -0.5) eta = -0.5;
//		if(eta > 0.5) eta = 0.5;
//		if(zeta < -0.5) zeta = -0.5;
//		if(zeta > 0.5) zeta = 0.5;

		estimate = GlobalFromIsoparametric(xi, eta, zeta);
		diff = estimate - point;
	}

	return geo::CPoint(xi, eta, zeta);
}
*/
// checks whether the element is degenerate (e.g. concave)
bool CEclipseBody::Degenerate() const
{
/*
	// loop over nodes, and see if det(J) is <= 0 for any of them
	geo::CMatrix J(3, 3);

	Jacobian(J, -0.5, -0.5, -0.5);
	if(J.GetDeterminant() < EPS) return true;
	Jacobian(J,  0.5, -0.5, -0.5);
	if(J.GetDeterminant() < EPS) return true;
	Jacobian(J,  0.5,  0.5, -0.5);
	if(J.GetDeterminant() < EPS) return true;
	Jacobian(J, -0.5,  0.5, -0.5);
	if(J.GetDeterminant() < EPS) return true;
	Jacobian(J, -0.5, -0.5,  0.5);
	if(J.GetDeterminant() < EPS) return true;
	Jacobian(J,  0.5, -0.5,  0.5);
	if(J.GetDeterminant() < EPS) return true;
	Jacobian(J,  0.5,  0.5,  0.5);
	if(J.GetDeterminant() < EPS) return true;
	Jacobian(J, -0.5,  0.5,  0.5);
	if(J.GetDeterminant() < EPS) return true;
*/

	return false;
}
