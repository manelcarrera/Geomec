// 2DDocument.cpp: implementation of the C2DDocument class.
//
//////////////////////////////////////////////////////////////////////

#include "2DDocument.h"
#include "2DSegment.h"
#include "version.h"
#include "RingFactory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2DDocument::C2DDocument()
: m_pBodies(0),
  m_pPolyLines(0),
  m_pSegments(0),
  m_pVertices(0)
{

}

C2DDocument::~C2DDocument()
{

}

QString C2DDocument::documentType() const
{
	return QString("LineDocument");
}

CVersion C2DDocument::documentVersion() const
{
	return CVersion(0, 0, 0);
}

void C2DDocument::createContainers()
{
	m_pVertices = new C2DVertexContainer("Vertices", *this);
	m_pSegments = new C2DSegmentContainer(*this);
	m_pPolyLines = new T2DPolyLineContainer("PolyLines", *this);
	m_pBodies = new T2DBodyContainer("bodies", *this);
}

void C2DDocument::createDefaults()
{
	C2DVertex *p1 = new C2DVertex(0, 0, Vertices());
	C2DVertex *p2 = new C2DVertex(100, 0, Vertices());
	C2DVertex *p3 = new C2DVertex(100, 100, Vertices());
	C2DVertex *p4 = new C2DVertex(0, 100, Vertices());

	C2DSegment *s1 = new C2DSegment(Segments(), *p1, *p2);
	C2DSegment *s2 = new C2DSegment(Segments(), *p2, *p3);
	C2DSegment *s3 = new C2DSegment(Segments(), *p3, *p4);
	C2DSegment *s4 = new C2DSegment(Segments(), *p4, *p1);
	C2DSegment *s5 = new C2DSegment(Segments(), *p1, *p3);

	C2DPolyLine *plA = new C2DPolyLine("Line1", PolyLines());
	plA->PolyLine().PushBack(*s1);
	plA->PolyLine().PushBack(*s2);

	C2DPolyLine *plB = new C2DPolyLine("Line2", PolyLines());
	plB->PolyLine().PushBack(*s3);
	plB->PolyLine().PushBack(*s4);

	C2DPolyLine *plC = new C2DPolyLine("Line3", PolyLines());
	plC->PolyLine().PushBack(*s5);


	updateBodies();
}

void C2DDocument::updateBodies()
{
	// Add segments to factory
	geo::CRingFactory factory;
	int i;
	for(i = 0; i < Segments().size(); i++)
		factory.AddSegment(Segments().at(i).Segment());

	// Calculate rings
	factory.CreateRings();

	// Get the entry set
	std::set<C2DBody*> stBody;
	for(i = 0; i < Bodies().size(); i++)
		stBody.insert(&Bodies().at(i));
	
	typedef std::vector<const C2DVertex*> TVertexVec;
	for(int nRing = 0; nRing < factory.RingSize(); nRing++)
	{
		const geo::CRingFactory::TRing& ring = factory.Ring(nRing);
		TVertexVec vcVertex(ring.size());
		for(int nVertex = 0; nVertex < ring.size(); nVertex++)
		{
			const C2DVertex::CSegmentPoint* pVertex = dynamic_cast<const C2DVertex::CSegmentPoint*>(ring[nVertex]);
			ASSERT(pVertex);
			vcVertex[nVertex] = &pVertex->Vertex();
		}

		std::set<IModelObject*> identifier = Identifier(vcVertex);

		// Find the moste suitable compartment
		C2DBody *pBody = 0;
		if(stBody.size() > 0)
		{	
			int nMin = INT_MAX;
			// Do the perfect matches first
			for(std::set<C2DBody*>::iterator it = stBody.begin(); it != stBody.end(); it++)
			{
				if((*it)->Resemblance(identifier) < nMin)
				{
					nMin = (*it)->Resemblance(identifier);
					pBody = *it;
				}
			}

			ASSERT(pBody);
			stBody.erase(pBody);
		}
		else
		{
			int nMin = INT_MAX;
			// Do the perfect matches first
			for(i = 0; i < Bodies().size(); i++)
			{
				if(Bodies().at(i).Resemblance(identifier) < nMin)
				{
					nMin = Bodies().at(i).Resemblance(identifier);
					pBody = &Bodies().at(i);
				}
			}
			ASSERT(pBody);
			pBody = new C2DBody(Bodies()); 
		}
		
		// Compartment created or found lets init
		pBody->SetPolygon(vcVertex);
		pBody->Indentifier(identifier);
	}

	// All compartments are assigned. Delete the left overs
	for(std::set<C2DBody*>::iterator itc = stBody.begin(); itc != stBody.end(); itc++)
	{
		delete *itc;
	}

}


std::set<IModelObject*> C2DDocument::Identifier(const std::vector<const C2DVertex*>& vcVertex) const
{
	std::set<IModelObject*> stNode;
	for(int i = 0; i < (vcVertex.size() - 1); i++)
	{
		const C2DSegment *pSegment = 0;
		const C2DVertex& first = *vcVertex[i];
		const C2DVertex& second = *vcVertex[i + 1];
		for(int i = 0; i < first.SegmentSize(); i++)
		{
			for(int j = 0; j < second.SegmentSize(); j++)
			{
				if(&first.Segment(i) == &second.Segment(j))
					pSegment = &first.Segment(i);
			}
		}

		ASSERT(pSegment);
		for(int j = 0; j < pSegment->PolyLineSize(); j++)
		{
			stNode.insert((IModelObject*)&pSegment->PolyLine(j));
		}
	}

	return stNode;
}