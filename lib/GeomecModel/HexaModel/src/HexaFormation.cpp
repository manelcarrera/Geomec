// HexaFormation.cpp: implementation of the CHexaFormation class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "HexaEntryTypes.h"
#include "HexaFormation.h"
#include "HexaHorizon.h"
#include "Pressure.h"
#include "HexaModel.h"
#include "CrossSection.h"

/* From IFormationElementSet constructor */
#include "MeshBase.h"

/* ************************************* */

#include "BoundaryBase.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "RotatedSystem.h"

//////////////////////////////////////////////////////////////////////
// Implementation of CHexaFormationVolume
//////////////////////////////////////////////////////////////////////
CHexaFormationVolume::CHexaFormationVolume(C3DFormation& formation, geo::CBodyGroup& group)
: CFormationVolume(formation, group)
{
}

CHexaFormationVolume::CHexaFormationVolume(C3DFormation& formation)
: CFormationVolume(formation)
{
}

TGraphNodeSet CHexaFormationVolume::Identifier() const
{
	TGraphNodeSet ret;
	CHexaFormation& formation = (CHexaFormation&)Formation();
	ret.insert(&formation.UpperHorizon());
	ret.insert(&formation.LowerHorizon());
	return ret;
}

int CHexaFormationVolume::DisplayListSize() const
{
	int nRet = CFormationVolume::DisplayListSize();
	if(nRet) {
		// Get formation entry to see or we in skin mode or not
		const CHexaFormationEntry *pEntry = dynamic_cast<const CHexaFormationEntry*>(Model().GraphEntry(MD_HEXA_FORMATION));
		if(pEntry)
		{
			if(pEntry->ShowSkin())
				return 3;
      else if(pEntry->ShowMidpoints())
        return 1;
		}
	}

	return nRet;
}

const geo::IObject& CHexaFormationVolume::DisplayList(int nIndex) const
{
	// Get formation entry to see or we in skin mode or not
	const CHexaFormationEntry *pEntry = dynamic_cast<const CHexaFormationEntry*>(Model().GraphEntry(MD_HEXA_FORMATION));
	assert(pEntry);

	const CModelBase& model = static_cast<const CModelBase&>(Model());
	assert(model.IsMesh());
	
	if(pEntry->ShowSkin())
	{
		if(m_skin.Empty())
    {
      if(model.SceneCanDraw())
			  buildSkin();
    }

		const CHexaFormation& form = dynamic_cast<const CHexaFormation&>(Formation());
		switch(nIndex)
		{
		case 0:
			return m_skin;
		case 1:
			return form.FormationPlane(CFormationPlane::FP_TOP)->PlaneSurface();
		case 2:
			return form.FormationPlane(CFormationPlane::FP_BOTTOM)->PlaneSurface();
		default:
			assert(false); // invalid index
		}
	}
  else if(pEntry->ShowMidpoints())
    return ElementCenterPoints();

	return CFormationVolume::DisplayList(nIndex);
}

void CHexaFormationVolume::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
	CModelBase& model = dynamic_cast<CModelBase&>(Model());
	if(&model.Mesh() == &node)
	{
		// delete skin if no mesh
		if(!model.Mesh().IsMesh())
		{
			int i;
			for(i = 0; i < 4; ++i)
				m_skinSides[i].Clear();
			m_skin.Clear();
		}
	}

	CFormationVolume::OnNeighbourModified(node, uHint);
}

void CHexaFormationVolume::buildSkin() const
{
	assert(m_skin.Empty());

	// wedx 10012007:
	// all faces that are used only once in the volume are part of the skin
	// the faces are sorted in a set of points (pointers)
	// they are collected into surfaces to make sure 3d shading is correct
	// the surface are collected into an array so the opengl system will render them all at once
	// unfortunately the top and bottom planes are destroyed and recreated and can therefore not be added to the array as well,
	//   hence the drawing in three stages

	typedef std::set<int> TNodeSet;
	typedef std::vector<const geo::IFace*> TFaceVec;
	typedef std::map<TNodeSet, TFaceVec> TNodeToFaceMap;

	TNodeToFaceMap mpNodeToFace;

  QString strJob;
  strJob = QString("Building skin of formation '%1'").arg(Formation().Name());
	std::auto_ptr <IProgressBase> pMainFrame(_g->prog()->create(eProgress::MainFrame));
	pMainFrame->StatusMessage(strJob);
	pMainFrame->AddSteps(100);
  int iLastPos = 0;

	int i;
	for(i = 0; i < ElementSet().ElementSize(); ++i)
	{
		const geo::IHexahedron* pBody = dynamic_cast<const geo::IHexahedron*>(&ElementSet().Element(i));

		if(pBody)
		{
			int j;
			for(j = 0; j < pBody->NrOfFaces(); ++j)
			{
				TNodeSet stNodes;
				const geo::IFace& face = pBody->Face(j);
				int k;
				for(k = 0; k < face.NrOfNodes(); ++k)
					stNodes.insert(face.Node(k).Index());

				// insert the face definition into the map
				TNodeToFaceMap::iterator itInsert =
					mpNodeToFace.insert(TNodeToFaceMap::value_type(stNodes, TFaceVec())).first;

				// push the face into the vector
				itInsert->second.push_back(&face);
			}
		}

    int iPos = int((((double)i + 1) / (double)ElementSet().ElementSize()) * 100.);
    if(iPos > iLastPos)
    {
      pMainFrame->Step();
      iLastPos = iPos;
    }
	}

  int nTotalSize = mpNodeToFace.size();
  int nCurrent = 0;
 
	// only the faces that are referenced once and lie on the boundary are part of the skin
  const CHexaModel& model = static_cast<const CHexaModel&>(Model());
  const CBoundaryBase& bound = model.Boundary();

  // use a local axis system so boundary is aligned with axes
  CRotatedSystem rs(bound.BoxAzimuth(), bound.Box().MidPoint());

  geo::CPoint ptMin = rs.LocalMin(bound.Box());
  geo::CPoint ptMax = rs.LocalMax(bound.Box());
	TNodeToFaceMap::iterator it;
	for(it = mpNodeToFace.begin(); it != mpNodeToFace.end(); ++it)
	{
		if(it->second.size() == 1)
		{
			const geo::IFace& face = const_cast<geo::IFace&>(*it->second[0]);

      // only add this face if it's really on the boundary (not part of a mesh refinement boundary)
      geo::CVector vecNormal(rs.ToLocal(face.Normal()));

      bool bX = (vecNormal == geo::CVector::Xaxis || vecNormal == -geo::CVector::Xaxis);
      bool bY = (vecNormal == geo::CVector::Yaxis || vecNormal == -geo::CVector::Yaxis);

      geo::CPoint pt(rs.ToLocal(face.Point(0)));
      geo::CPoint pt2(rs.ToGlobal(face.Point(0)));
      if((bX && (fabs(pt.X() - ptMin.X()) < EPS || fabs(pt.X() - ptMax.X()) < EPS)) ||
         (bY && (fabs(pt.Y() - ptMin.Y()) < EPS || fabs(pt.Y() - ptMax.Y()) < EPS)))
      {
			  const geo::IBody* pBody = face.Parent();

			  int idx = pBody->FaceIndex(face);
			  assert(idx >= 0);

			  if(idx > 0 && idx <= 4)
				  m_skinSides[idx-1].AddFace(face);
      }
		}

    int iPos = int(((double(++nCurrent) + (double)nTotalSize) / (double)nTotalSize) * 100.);
    if(iPos > iLastPos)
    {
      pMainFrame->Step();
      iLastPos = iPos;
    }
	}

	for(i = 0; i < 4; ++i)
		m_skin.PushBack(m_skinSides[i]);
}

const geo::CPtrArray<geo::IObject>& CHexaFormationVolume::Skin() const
{
	if(m_skin.Empty())
		buildSkin();

  return m_skin;
}


//////////////////////////////////////////////////////////////////////
// Implementation of CHexaFormation
//////////////////////////////////////////////////////////////////////
#define ELEMENTS_HEIGHT 2

CHexaFormation::CHexaFormation(CFemAppModel &model)
: C3DFormation(model), m_pUpper(0), m_pLower(0), m_nElements(1)
{
	CHexaFormationVolume *pVolume = new CHexaFormationVolume(*this);
	/* From IFormationElementSet constructor */
	pVolume->reParent(this);
//	CModelBase &pModel = dynamic_cast<CModelBase&>(Model());
//	assert(&pModel);
//	pVolume->LinkTo(pModel.Mesh());
	/* ************************************* */

	m_pFormationPlane[0] = QSharedPointer <CFormationPlane> (0);
	m_pFormationPlane[1] = QSharedPointer <CFormationPlane> (0);
	m_pFormationPlane[2] = QSharedPointer <CFormationPlane> (0);
}

//##ModelId=3BE7E0C70068
CHexaFormation::CHexaFormation(const QString& strName, CHexaHorizon &upper, CHexaHorizon &lower, int nElements, CModelBase &model)
: C3DFormation(strName, model), m_pUpper(&upper), m_pLower(&lower), m_nElements(nElements)
{
	CHexaFormationVolume *pVolume = new CHexaFormationVolume(*this);
	/* From IFormationElementSet constructor */
	pVolume->reParent(this);
//	CModelBase &pModel = dynamic_cast<CModelBase&>(Model());
//	assert(&pModel);
//	pVolume->LinkTo(pModel.Mesh());
	/* ************************************* */

	LinkTo(upper);
	LinkTo(lower);

	// Do Some meshing?
	assert(model.GraphEntry(MD_HEXA_FORMATION));
	reParent(model.GraphEntry(MD_HEXA_FORMATION));

	m_pFormationPlane[0] = QSharedPointer <CFormationPlane> (new CFormationPlane(CFormationPlane::FP_TOP, *this));
	m_pFormationPlane[1] = QSharedPointer <CFormationPlane> (new CFormationPlane(CFormationPlane::FP_MIDDLE, *this));
	m_pFormationPlane[2] = QSharedPointer <CFormationPlane> (new CFormationPlane(CFormationPlane::FP_BOTTOM, *this));
}

CHexaFormation::CHexaFormation(const CHexaFormation& rhs)
: C3DFormation(rhs), m_pUpper(rhs.m_pUpper), m_pLower(rhs.m_pLower), m_nElements(rhs.m_nElements)
{
	m_pFormationPlane[0] = rhs.m_pFormationPlane[0];
	m_pFormationPlane[1] = rhs.m_pFormationPlane[1];
	m_pFormationPlane[2] = rhs.m_pFormationPlane[2];
}

//##ModelId=3BE7E0C70078
CHexaFormation::~CHexaFormation()
{
}

CHexaFormation& CHexaFormation::operator=(const CHexaFormation &rhs)
{
	C3DFormation::operator=(rhs);

	bool bInvalidateMesh((m_nElements != rhs.m_nElements) || (MinThickness().Value() != rhs.MinThickness().Value()));

	m_nElements = rhs.m_nElements;
	if(bInvalidateMesh)
	{
		// Get the horizon model ...
		CHexaModel* pModel = (CHexaModel*)(&Model());
		pModel->InvalidateMesh();
	}
	
	return *this;
}

bool CHexaFormation::operator==(const CHexaFormation &rhs) const
{
	if(!C3DFormation::operator==(rhs))
		return false;

	return (m_nElements == rhs.m_nElements);
}

bool CHexaFormation::ShowCenterPoints() const
{
	const CHexaFormationEntry *pEntry = dynamic_cast<const CHexaFormationEntry*>(Model().GraphEntry(MD_HEXA_FORMATION));
	if(pEntry)
		return pEntry->ShowMidpoints();

	return false;
}
/*
void CHexaFormation::Clear()
{
	Material().Clear();

	// Clear formation planes
	for(int i = 0; i < 3; i++)
	{
		m_pFormationPlane[i]->Clear();
	}
}
*/
const CHexaHorizon& CHexaFormation::UpperHorizon() const
{
	assert(m_pUpper);
	return *m_pUpper;
}

//##ModelId=3BE7E0C70087
CHexaHorizon& CHexaFormation::UpperHorizon()
{
	assert(m_pUpper);
	return *m_pUpper;
}

//##ModelId=3BE7E0C70088
const CHexaHorizon& CHexaFormation::LowerHorizon() const
{
	assert(m_pLower);
	return *m_pLower;
}

//##ModelId=3BE7E0C7008A
CHexaHorizon& CHexaFormation::LowerHorizon()
{
	assert(m_pLower);
	return *m_pLower;
}

//##ModelId=3BE7E0C7008B
void CHexaFormation::UpperHorizon(CHexaHorizon& upper)
{
	UnLink(*m_pUpper);
	if(!IsLinkedTo(upper))
		LinkTo(upper);
	m_pUpper = &upper;

// TODO : Remeshing?
}

bool CHexaFormation::Less(const CGraphNode &node) const
{
	const CHexaFormation *pFor = dynamic_cast<const CHexaFormation*>(&node);
	const CCrossSection::CCSFormation *pCSFor = dynamic_cast<const CCrossSection::CCSFormation*>(&node);

	if(!pFor && pCSFor) pFor = (CHexaFormation*)(&(pCSFor->BaseObject()));

	if(pFor)
	{
		if((&UpperHorizon() == &pFor->UpperHorizon()) &&
		   (&LowerHorizon() == &pFor->LowerHorizon()))
		   return false;	// Formation are equal ....

		// Find upper ...
		const CHexaFormation *pNext = this;
		while(pNext)
		{
			if(&pNext->UpperHorizon() == &pFor->UpperHorizon())
				return false;
			pNext = pNext->UpperFormation();
		}

		// Find lower ...
		const CHexaFormation *pLower = this;
		while(pLower)
		{
			if(&pLower->LowerHorizon() == &pFor->LowerHorizon())
				return true;
			pLower = pLower->LowerFormation();
		}

		assert(false);

		return UpperHorizon().Less(pFor->UpperHorizon());	// Flauwekul ...
	}

	return CGraphNode::Less(node);
}

//##ModelId=3BE7E0C70095
void CHexaFormation::LowerHorizon(CHexaHorizon& lower)
{
	UnLink(*m_pLower);
	if(!IsLinkedTo(lower))
		LinkTo(lower);
	m_pLower = &lower;
	// TODO : Remeshing?
}

const CHexaFormation* CHexaFormation::UpperFormation() const
{
  for(size_t i = 0; i < UpperHorizon().referenceSize(); i++)
	{
		const CHexaFormation *pFor = dynamic_cast<const CHexaFormation*>(&UpperHorizon().referenceAt(i));
		if(pFor && (&pFor->LowerHorizon() == &UpperHorizon()))
			return pFor;
	}

	return 0;
}

const CHexaFormation* CHexaFormation::LowerFormation() const
{
  for(size_t i = 0; i < LowerHorizon().referenceSize(); i++)
	{
		const CHexaFormation *pFor = dynamic_cast<const CHexaFormation*>(&LowerHorizon().referenceAt(i));
		if(pFor && (&pFor->UpperHorizon() == &LowerHorizon()))
			return pFor;
	}

	return 0;
}

bool CHexaFormation::CanSplitFormation(const CHexaHorizon& horizon) const
{
	if((static_cast<const CModelBase&>(Model())).BranchState().IsBranch())
		return false;

	if(UpperHorizon().Min().Z() > horizon.Min().Z())
			return false;


	if(LowerHorizon().Max().Z() < horizon.Max().Z())
			return false;


	return true;
}
		
		// Check
void CHexaFormation::SplitFormation(CHexaHorizon& horizon)
{
	assert(CanSplitFormation(horizon));

	// Make our formation valid ....
	CHexaHorizon *pLowerHorizon = &LowerHorizon();
	LowerHorizon(horizon);

	// Create a new formation
	/*CHexaFormation *pFormation =*/ new CHexaFormation(horizon.Name().toStdString().c_str(), horizon, *pLowerHorizon, ELEMENTS_HEIGHT, (CModelBase&) Model());

	CHexaModel* pModel = (CHexaModel*)(&Model());
	pModel->InvalidateMesh();
	Modified();
}

void CHexaFormation::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	CHexaHorizonEntry& horizon_entry = (CHexaHorizonEntry&)*Model().GraphEntry(MD_HEXA_HORIZON);
	assert(Empty());

	// Load formation base data
	C3DFormation::LoadStream(stream, version, progress);	
	
	// Inform the user
	std::string pr("Loading Formation ");
	pr += Name().toStdString();
	progress.StatusMessage(pr.c_str());

	// Load indices of the horizons
	int nUpperIndex, nLowerIndex;
	stream >> nUpperIndex;
	stream >> nLowerIndex;

	assert(horizon_entry.FindIndex(nUpperIndex));
	assert(horizon_entry.FindIndex(nLowerIndex));

	// Initialize horizons and LinkTo Formation
	m_pUpper = horizon_entry.FindIndex(nUpperIndex);
	m_pLower = horizon_entry.FindIndex(nLowerIndex);

	LinkTo(*m_pUpper);
	LinkTo(*m_pLower);

	// MinThickness and elements
	if(version < CStreamVersion(3, 0, 57))
	{
		double dValue;
		stream >> dValue;
		MinThickness(dValue, CQuantity::SI_UNIT);
	}
	stream >> m_nElements;

	// Load planes
	for(int i = 0; i < 3; i++)
	{
		// Dummy support ...
		int bPlane;
		stream >> bPlane;
	}

	m_pFormationPlane[0] = QSharedPointer <CFormationPlane> (new CFormationPlane(CFormationPlane::FP_TOP, *this));
	m_pFormationPlane[1] = QSharedPointer <CFormationPlane> (new CFormationPlane(CFormationPlane::FP_MIDDLE, *this));
	m_pFormationPlane[2] = QSharedPointer <CFormationPlane> (new CFormationPlane(CFormationPlane::FP_BOTTOM, *this));

	progress.Step();

	// Link to entry
	assert(Model().GraphEntry(MD_HEXA_FORMATION));
	reParent(Model().GraphEntry(MD_HEXA_FORMATION));
}

void CHexaFormation::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	// State that we're saving a formation
	std::string pr("Saving Formation ");
	pr += Name().toStdString();
	progress.StatusMessage(pr.c_str());

	// Save formation base data
	C3DFormation::SaveStream(stream, progress);

	assert(UpperHorizon().Index() > 0);
	assert(LowerHorizon().Index() > 0);

	// Save index of the horizons
	stream << UpperHorizon().Index();
	stream << LowerHorizon().Index();

	// Save MinThickness and elements
	stream << m_nElements;

	for(int i = 0; i < 3; i++)
	{
		bool bPlane = (m_pFormationPlane[i] != 0);
		stream << (int) bPlane;
	}

	progress.Step();
}

long CHexaFormation::SavedItems() const
{
	long lRet = C3DFormation::SavedItems();
	lRet++;
	return lRet;
}


int CHexaFormation::Elements() const
{
	return m_nElements;
}

void CHexaFormation::Elements(int nElements)
{
	m_nElements = nElements;
}

//=============================================================================
IValueDomainScalar::TValueVec CHexaFormation::CalculatePorePressure(const geo::IBody& body, const CDepletionStage& pDepletionStage) const
{
	IValueDomainScalar::TValueVec vcPressure;

	for (int i=0; i<body.NrOfPoints(); i++)
		vcPressure.push_back(Pressure(pDepletionStage).Component().ScalarData().ValuePoint(body.Point(i)));

	return vcPressure;
}

/*
std::vector<CTensor> CHexaFormation::CalculateEffectiveStress(const geo::CHexahedron &hexahedron) const
{
	std::vector<CTensor> vcTensor;

	const CModelBase *pModel = (const CModelBase *) &Model();
	vcTensor = pModel->GlobalInitialStress().TotalStress(*this, hexahedron);

	IValueComponentBase::TValueVec vcPorePressure = CalculatePorePressure(hexahedron, pModel->InitialDepletionStage());

	assert(vcPorePressure.size() == vcTensor.size());

	// make effective stresses by subtracting pore pressures from normal values
	for(size_t i = 0; i < vcTensor.size(); i++)
	{
		vcTensor[i].Set(vcTensor[i].XX() - vcPorePressure[i].Value(),
						vcTensor[i].YY() - vcPorePressure[i].Value(),
						vcTensor[i].ZZ() - vcPorePressure[i].Value(),
						vcTensor[i].XY(),
						vcTensor[i].YZ(),
						vcTensor[i].XZ());

	}

	return vcTensor;
}
*/

QSharedPointer <CFormationPlane> CHexaFormation::FormationPlane(CFormationPlane::ePlaneType type)
{
	switch(type)
	{
	case CFormationPlane::FP_TOP:
		return m_pFormationPlane[0];
		break;
	case CFormationPlane::FP_MIDDLE:
		return m_pFormationPlane[1];
		break;
	case CFormationPlane::FP_BOTTOM:
		return m_pFormationPlane[2];
		break;
	}

	assert(false);
	return m_pFormationPlane[0];
}

const QSharedPointer <CFormationPlane> CHexaFormation::FormationPlane(CFormationPlane::ePlaneType type) const
{
	switch(type)
	{
	case CFormationPlane::FP_TOP:
		return m_pFormationPlane[0];
		break;
	case CFormationPlane::FP_MIDDLE:
		return m_pFormationPlane[1];
		break;
	case CFormationPlane::FP_BOTTOM:
		return m_pFormationPlane[2];
		break;
	}

	assert(false);
	return m_pFormationPlane[0];
}


////

CHexaFormationEntry::CHexaFormationEntry(CHexaModel& model)
: CFormationEntryTempl<CHexaFormation>(MD_HEXA_FORMATION, model)
{
}
