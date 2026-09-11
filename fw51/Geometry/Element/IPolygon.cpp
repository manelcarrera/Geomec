 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ipolygon.cpp: implementation of the IPolygon class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include <cmath>
#include "Plane.h"
#include "Polygon.h"
#include "PolyArea.h"
#include "Matrix.h"
#include "Line.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo { 

IPolygon::IPolygon(bool bIsHole)
: m_bIsHole(bIsHole),
  m_Normal(CVector::NullVector),
  m_bNormalCalculated(false)
{
}

IPolygon::~IPolygon()
{
}

#define SETBITS(n,mask,s) (((n)->Flags & ~(mask)) | ((s) & mask))
#define GETBITS(n, mask) ((n)->Flags & mask)

static CPolygon* GetNewPolyLine(PLINE* pline)
{
	CArray<CPoint> points;
    VNODE *cur;

    cur = &pline->head;
    do
    {
		CPoint p(cur->point[0], cur->point[1]);
		points.PushBack(p);
    } while ( ( cur = cur->next ) != &pline->head );
    
	CPolygon* p=new CPolygon(points, (GETBITS(pline, PLF_ORIENT) == PLF_INV));

	return p;
}


static PLINE* GetNewPolyLine(IPolygon& Poly)
{
    int i;
    PLINE *res = NULL;
    Vector v;

	
    if (Poly.NrOfPoints()<0) return NULL;
    for ( i = 0; i < Poly.NrOfPoints(); i++ )
    {
		v[0] = (double)Poly.Point(i).X();
		v[1] = (double)Poly.Point(i).Y();
		double dum = Poly.Point(i).Z();
		assert(fabs(dum) <EPS);
        v[2] = (double)0.0;

        if ( res == NULL )
        {
            if ((res = poly_NewContour(v)) == NULL)
                return NULL;
        }
        else
            poly_InclVertex(res->head.prev, poly_CreateNode(v));
    }
    poly_PreContour(res, true);


    return res;
}


static CPolygonPtrArray GetNewPolyArea(POLYAREA* poly)
{
    PLINE *cntr;
    POLYAREA * curpa;
	
	CPolygonPtrArray ret;

    curpa = poly; 
    do
    {
        //for ( cntr = curpa->contours, cnt = 0; cntr != NULL; cntr = cntr->next, cnt++ ) {}
        //fprintf(f, "%d\n", cnt);
        for ( cntr = curpa->contours; cntr != NULL; cntr = cntr->next )
		{
             CPolygon* pb = GetNewPolyLine(cntr);
			 ret.PushBack(*pb);
		}
    }   while ( (curpa = curpa->f) != poly);

    return ret;
}

static POLYAREA* GetNewPolyArea(CPolygonPtrArray& poly)
{
    int cnt;
    PLINE *cntr;
    POLYAREA *p=NULL;
	POLYAREA *PA;

    PA = NULL;

	cnt=poly.Size();
		
    if (cnt < 1) return NULL;
    
    for (size_t i = 0; i < cnt; i++ )
    {

        if ( (cntr = GetNewPolyLine(poly.Object(i))) == NULL)
            return NULL;

		if(poly.Object(i).IsHole())
		{
			if(i==0) //first is never a hole
			{
				assert(false);
				return NULL;
			}
			
			if(p==NULL) //no previous poly created
			{
				assert(false);
				return NULL;
			}

			if ( (cntr->Flags & PLF_ORIENT) !=  PLF_INV )
				poly_InvContour(cntr);

			poly_InclContour(p, cntr);
			cntr = NULL;

			if (!poly_Valid(p))
			{
				assert(false);
				return NULL;
			}
		}
		else
		{
			p = NULL;

			if ( (cntr->Flags & PLF_ORIENT) !=  PLF_DIR )
				poly_InvContour(cntr);

			if (p == NULL)
				p = poly_Create();

			if (p == NULL)
			{
				assert(false);
				return NULL;
			}

			poly_InclContour(p, cntr);
			cntr = NULL;

			if (!poly_Valid(p))
			{
				assert(false);
				return NULL;
			}


			poly_M_Incl(&PA, p);	
		}
		
    }
    
    return PA;
}

CPolygonPtrArray IPolygon::PolyClip(IPolygon::ClipOperation ClipType,CPolygonPtrArray& Poly1,CPolygonPtrArray& Poly2)
{
	POLYAREA *a = NULL, *b = NULL, *ret = NULL;
	
	CPolygonPtrArray Ret;

	if(Poly1.Size() ==0 || Poly2.Size()==0)
		return Ret; //empty array


	CVector n1 = Poly1.Object(0).Normal();
	CVector n2 = Poly2.Object(0).Normal();
	
	n1=n1.UnitVector();
	n2=n2.UnitVector();

//	double Angle = n1.AngleDeg(n2);
	//must be parallel
	assert(n1 == n2 || n1 == (n2*-1));

	geo::CMatrix TM,TMI;
	Poly1.Object(0).GetTransMatInPlane(geo::CPlane::PlaneXY,TM,TMI);

	//Trace(Poly1, "Poly1");
	//Trace(Poly2, "Poly2");
	
	Poly1.Transform(TM);	
	Poly2.Transform(TM);	
	

	//Trace(Poly1, "Poly1");
	//Trace(Poly2, "Poly2");

	a=GetNewPolyArea(Poly1);
	b=GetNewPolyArea(Poly2);


	//Trace(Poly1, "Poly1");
	//Trace(Poly2, "Poly2");
	/*
	UNITE,
	ISECT,
	SUB,
	XOR
	*/

	poly_Boolean(a,b,&ret,ClipType);
	
	

	if(ret)
	{
		Ret=GetNewPolyArea(ret);
		Ret.Transform(TMI);	
		poly_Free(&ret);
	}

	
	Poly1.Transform(TMI);	
	Poly2.Transform(TMI);	



	//copy back for testing
	//ret=GetNewPolyArea(Ret);
	//SavePOLYAREA(ret,"PolyClip.wlr");
	
    poly_Free(&a);
    poly_Free(&b);

	return Ret;
}

bool IPolygon::PointOnBoundary(const IPoint& p)
{
	for(int i=0;i< NrOfLines(); i++)
	{
		if(Line(i).Contains(p,true))
			return true;
	}
	return false;
}

CVector IPolygon::Normal() const
{
	if (Empty())
		return geo::CVector::NullVector;
	if(m_bNormalCalculated)
		return m_Normal;

	m_Normal = IFace::Normal();
	m_bNormalCalculated = true;

	return m_Normal;
}

void IPolygon::InvalidateCache()
{
	m_bNormalCalculated = false;
	IFace::InvalidateCache();
}

bool IPolygon::IsHole() const
{
	return m_bIsHole;
}

void IPolygon::IsHole(bool bIsHole)
{
	m_bIsHole = bIsHole;
}

}
