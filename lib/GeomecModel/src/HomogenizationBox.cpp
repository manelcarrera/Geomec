
#include "HomogenizationBox.h"
#include "BaseEntryTypes.h"
#include "ValueTypeFactory.h"
#include "NodalValueSet.h"
#include "FormationBase.h"
#include "SpringElement.h"
#include "GeomecUtils.h"
#include "TetraModel.h"
#include "ResultTree.h"
#include "GlobalInitialStressNode.h"
#include "ValueTensor.h" 
#include "ValueVector.h"
#include "ElementValueSet.h"
#include "BlockMesh.h"
#include "FemAppModel.h"
#include "GeoSurface.h"
#include "Result.h"
#include "ISupport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHomogenizationBox::CHomogenizationBox(CFemAppModel &model, const QString &strName)
:IScalingBox(model, strName), m_mesh_ptset(0)
{
	m_mesh = new geo::CBlockMesh();
	m_display_mesh = new geo::CBlockMesh();

	// Link to homogenization box entry
	assert(Model().GraphEntry(MD_BASE_HOMO_BOX));
	create(Model().GraphEntry(MD_BASE_HOMO_BOX));
}

CHomogenizationBox::CHomogenizationBox(CFemAppModel &model)
:IScalingBox(model), m_mesh_ptset(0)
{
	m_mesh = new geo::CBlockMesh();
	m_display_mesh = new geo::CBlockMesh();
}

CHomogenizationBox::CHomogenizationBox(const CHomogenizationBox &rhs)
:IScalingBox(rhs), m_mesh_ptset(0)
{
	m_mesh = new geo::CBlockMesh();
	m_display_mesh = new geo::CBlockMesh();
	*m_mesh		 = *rhs.m_mesh;
	*m_display_mesh = *rhs.m_display_mesh;
}

CHomogenizationBox::~CHomogenizationBox()
{
	m_mesh->Clear();
	delete m_mesh;
	m_display_mesh->Clear();
	delete m_display_mesh;
}

void CHomogenizationBox::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	TPointSetEntry& pointset_entry = (TPointSetEntry&)*Model().GraphEntry(MD_BASE_POINTSET);

	int nIndex1;
	stream >> nIndex1;
	if(nIndex1 != -1)
		m_mesh_ptset = (CPointSet*)pointset_entry.FindIndex(nIndex1);
	else
		m_mesh_ptset = 0;
	progress.Step();

	QString name;

	stream >> name;
	Name(name);
	progress.Step();

	QString strJob;
	strJob = QString("Loading homogenization box %1").arg(Name());
	progress.StatusMessage(strJob);

  if(version < CStreamVersion(3, 7, 24) || (version >= CStreamVersion(3, 8, 0) && version < CStreamVersion(3, 8, 3)))
  {
	  stream >> nIndex1;
	  progress.Step();
	  stream >> nIndex1;
	  progress.Step();
  }

	double Xb, Yb, Zb;
	stream >> Xb;
	stream >> Yb;
	stream >> Zb;

	double Xsp, Ysp, Zsp;
	int DivX, DivY, DivZ;
	double azi;

	if(version < CStreamVersion(3, 0, 78))
	{
		// old style, single division and spacing parameters, no rotation
		stream >> DivX;
		stream >> Xsp;

		DivY = DivZ = DivX;
		Ysp = Zsp = Xsp;
		azi = 0;
	}
	else
	{
		// new style, divisions and sizes per axis and a rotation around the Z axis
		stream >> DivX;
		stream >> DivY;
		stream >> DivZ;
		stream >> Xsp;
		stream >> Ysp;
		stream >> Zsp;
		stream >> azi;
	}
	progress.Step();

	geo::CPoint ptMin(Xb, Yb, Zb);
	double edge_length_x = Xsp * (double)DivX;
	double edge_length_y = Ysp * (double)DivY;
	double edge_length_z = Zsp * (double)DivZ;

	geo::CPoint ptCenter(Xb + (edge_length_x / 2.0), Yb + (edge_length_y / 2.0), Zb + (edge_length_z / 2.0));
	
	m_mesh = new geo::CBlockMesh();
	m_display_mesh = new geo::CBlockMesh();

	SetMesh(ptCenter, edge_length_x, edge_length_y, edge_length_z, DivX, DivY, DivZ, azi);

	// First all the tyings have to be sorted out....
	std::map<int, std::set<int> > connections;
	SortOutTyings(connections);
	// Create the springelements...
	SetupSpringElements();
	// Next the tyings can be created....
	SetupTyings(connections);
	
	create(Model().GraphEntry(MD_BASE_HOMO_BOX));

  if(version < CStreamVersion(3, 7, 24) || (version >= CStreamVersion(3, 8, 0) && version < CStreamVersion(3, 8, 3)))
  {
	  int nIndex2;
	  unsigned int type;
    // young
	  stream >> nIndex2;
	  stream >> type;
    // poisson
	  stream >> nIndex2;
	  stream >> type;
  	progress.Step();

	  int nResultSize;
	  stream >> nResultSize;
	  double dResult;
	  if(nResultSize != 0)
	  {
		  for(int i = 0; i < nResultSize; i++)
		  {
			  stream >> dResult;
		  }
	  }
	  progress.Step();
  }

	if(version > CStreamVersion(3, 0, 62))
		CColorNode::LoadStream(stream, version, progress);
}

void CHomogenizationBox::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	if(m_mesh_ptset)
		stream << m_mesh_ptset->Index(); 
	else
		stream << -1;
	progress.Step();

	stream << Name();
	progress.Step();

	stream << m_mesh->XBase();
	stream << m_mesh->YBase();
	stream << m_mesh->ZBase();

	stream << m_mesh->LengthDivision();
	stream << m_mesh->WidthDivision();
	stream << m_mesh->HeightDivision();

	stream << m_mesh->XSpace();
	stream << m_mesh->YSpace();
	stream << m_mesh->ZSpace();

	stream << Azimuth();

	progress.Step();
	
	CColorNode::SaveStream(stream, progress);
}

long CHomogenizationBox::SavedItems() const
{
	return 3 + CColorNode::SavedItems();
}

unsigned int CHomogenizationBox::IconId() const
{
	return IDI_SCALING_BOX; 
}

unsigned int CHomogenizationBox::TypeId() const
{
	return 0;
}

bool CHomogenizationBox::CanConnectItem(const CGraphNode& /*item*/) const
{
	return false;
}

bool CHomogenizationBox::ConnectItem(const CGraphNode& item)
{
	return CColorNode::ConnectItem(item);
}

bool CHomogenizationBox::CanCutSurafceInDatastoreWithBox() const
{
	const CModelBase &model = dynamic_cast<const CModelBase&>(Model());
	CSurfaceEntry::TNodeSet nodes = ((CSurfaceEntry*) model.GraphEntry(MD_BASE_SURFACE))->EntryNodes();
	CSurfaceEntry::TNodeSet::iterator it = nodes.begin();
	if(it == nodes.end())
		return false;

	return true;
}

namespace
{

const QString DEFAULT_CAPTURED_NAME = QObject::tr("%1_captured");
const QString DEFAULT_CAPTURED_NUMBER = QObject::tr(" %1");
const size_t DEFAULT_INITIAL_NUMBER = 0;

} // anonymous namespace

/*!
  Creates an elementset with the mesh elements in or intersecting the zoom in box.
*/
CElementSet* CHomogenizationBox::CaptureElementSet(CModelBase& target_model) const
{

	const CModelBase &source_model = dynamic_cast<const CModelBase&>(Model());
	TFormationBaseEntry::TNodeSet formations = ((TFormationBaseEntry*) source_model.GraphEntry(MD_BASE_FORMATION))->EntryNodes();
	TFormationBaseEntry::TNodeSet::iterator it;

	// make 6 planes describing the faces
	geo::CVector x_plane(geo::CPoint(DisplayMesh().XBase(), DisplayMesh().YBase(), DisplayMesh().ZBase()),
						 geo::CPoint(DisplayMesh().XBase() + DisplayMesh().XSpace(), DisplayMesh().YBase(), DisplayMesh().ZBase()));
	geo::CVector y_plane(geo::CPoint(DisplayMesh().XBase(), DisplayMesh().YBase(), DisplayMesh().ZBase()),
						 geo::CPoint(DisplayMesh().XBase(), DisplayMesh().YBase() + DisplayMesh().YSpace(), DisplayMesh().ZBase()));
	geo::CVector z_plane(geo::CPoint(DisplayMesh().XBase(), DisplayMesh().YBase(), DisplayMesh().ZBase()),
						 geo::CPoint(DisplayMesh().XBase(), DisplayMesh().YBase(), DisplayMesh().ZBase() + DisplayMesh().ZSpace()));

	geo::CPlane p1(Min(), y_plane, x_plane);
	geo::CPlane p2(Max(), x_plane, y_plane);
	geo::CPlane p3(Max(), z_plane, x_plane);
	geo::CPlane p4(Min(), x_plane, z_plane);
	geo::CPlane p5(Min(), z_plane, y_plane);
	geo::CPlane p6(Max(), y_plane, z_plane);

	int i, j, count;
	const geo::IBody *pBody = 0;

	std::vector<const geo::IElement*> elms;
	for(it = formations.begin(); it != formations.end(); it++)
	{
		for(i = 0; i < (*it)->ElementSetSize(); i++)
		{
			for(j = 0; j < (*it)->ElementSet(i).ElementSet().ElementSize(); j++)
			{
				count = 0;
				pBody = (const geo::IBody*)(&(*it)->ElementSet(i).ElementSet().Element(j));
				
				if(pBody->Intersects(p1, true))
					count++;
				else if(p1.SignedDistance(pBody->Point(0)) < 0.0)
					count++;

				if(pBody->Intersects(p2, true))
					count++;
				else if(p2.SignedDistance(pBody->Point(0)) < 0.0)
					count++;

				if(pBody->Intersects(p3, true))
					count++;
				else if(p3.SignedDistance(pBody->Point(0)) < 0.0)
					count++;
			
				if(pBody->Intersects(p4, true))
					count++;
				else if(p4.SignedDistance(pBody->Point(0)) < 0.0)
					count++;
			
				if(pBody->Intersects(p5, true))
					count++;
				else if(p5.SignedDistance(pBody->Point(0)) < 0.0)
					count++;

				if(pBody->Intersects(p6, true))
					count++;
				else if(p6.SignedDistance(pBody->Point(0)) < 0.0)
					count++;

				if(count == 6)
					elms.push_back(pBody);
			}
		}
	}

	CElementSet::eElementType type( CElementSet::HEXA );
	if( dynamic_cast<const CTetraModel*>(&source_model) )
		type = CElementSet::TETRA;

  QString name = target_model.GraphEntry(MD_BASE_POINTSET)->createUniqueName(
    DEFAULT_CAPTURED_NAME.arg(Name()) + DEFAULT_CAPTURED_NUMBER,
    DEFAULT_INITIAL_NUMBER);
	CElementSet *pElSet = new CElementSet(elms, name.toStdString().c_str(), type, target_model);
	return pElSet;	
}

bool CHomogenizationBox::Destroy()
{
	return CColorNode::Destroy();
}

void CHomogenizationBox::SetMesh(const geo::IPoint& center, const double& sizeX, const double& sizeY, const double& sizeZ,
								 int divX, int divY, int divZ, const double& azimuth)
{
	Center(center);
	SideX(sizeX);
	SideY(sizeY);
	SideZ(sizeZ);
	Azimuth(azimuth);
	UpdateBox();

	geo::CPoint ptMin(center.X()-sizeX / 2.0,
					  center.Y()-sizeY / 2.0,
					  center.Z()-sizeZ / 2.0);

	m_mesh->SetMesh(ptMin, sizeX, sizeY, sizeZ, divX, divY, divZ);
	m_mesh->Move(-geo::CVector(center));
	m_mesh->Rotate(geo::CVector::Zaxis, azimuth);
	m_mesh->Move(geo::CVector(center));

	m_display_mesh->SetMesh(ptMin, sizeX, sizeY, sizeZ, divX, divY, divZ);
	m_display_mesh->Move(-geo::CVector(center));
	m_display_mesh->Rotate(geo::CVector::Zaxis, azimuth);
	m_display_mesh->Move(geo::CVector(center));
}

void CHomogenizationBox::CreatePointSet()
{
	if(m_mesh_ptset) 
	{
		delete m_mesh_ptset;
	}

	m_mesh_ptset = new CPointSet(Name().toStdString().c_str(), Model(), 0, CPointSet::DIM_3D);
	
	for(int i = 0; i < m_mesh->ElementSize(); i++)
	{
		const geo::IElement &el = m_mesh->Element(i);
		m_mesh_ptset->PushBack(((el.Max() - el.Min()) / 2) + el.Min(), std::vector<double>());
	}
}

void CHomogenizationBox::Name(const QString &strName)
{
	if(m_mesh_ptset)
		m_mesh_ptset->Name(strName);
	COpenGLNode::Name(strName);
}

const QString &CHomogenizationBox::Name() const
{
	return CGraphNode::Name();
}

void CHomogenizationBox::assignMesh(geo::CBlockMesh& mesh, const CHomogenizationBox& hbox) const
{
	geo::CPoint ptMin(hbox.Center().X() - hbox.SideX() / 2.0,
					  hbox.Center().Y() - hbox.SideY() / 2.0,
					  hbox.Center().Z() - hbox.SideZ() / 2.0);

	mesh.SetMesh(ptMin, hbox.SideX(), hbox.SideY(), hbox.SideZ(),
				 hbox.Mesh().LengthDivision(), hbox.Mesh().WidthDivision(), hbox.Mesh().HeightDivision());
	mesh.Move(-geo::CVector(hbox.Center()));
	mesh.Rotate(geo::CVector::Zaxis, hbox.Azimuth());
	mesh.Move(geo::CVector(hbox.Center()));
}

CHomogenizationBox& CHomogenizationBox::operator=(const CHomogenizationBox &rhs)
{
	IScalingBox::operator =(rhs);
	delete m_mesh;
	m_mesh = new geo::CBlockMesh();
	assignMesh(*m_mesh, rhs);

	delete m_display_mesh;
	m_display_mesh = new geo::CBlockMesh();
	assignMesh(*m_display_mesh, rhs);

	CreatePointSet();

	SetUpCalculationGeometry();

	Modified();

	return *this;
}

void CHomogenizationBox::SetUpCalculationGeometry()
{
	// First all the tyings have to be sorted out....
	std::map<int, std::set<int> > connections;
	SortOutTyings(connections);
	// Create the springelements...
	SetupSpringElements();
	// Next the tyings can be created....
	SetupTyings(connections);
}

bool CHomogenizationBox::operator==(const CHomogenizationBox &rhs) const
{
	if(rhs.Name() != Name())
		return false;
	if(rhs.Azimuth() != Azimuth())
		return false;
	return *m_mesh == rhs.Mesh();
}

int CHomogenizationBox::DisplayListSize() const
{
	return 1;
}

const geo::IObject &CHomogenizationBox::DisplayList(int /*nIndex*/) const
{
	return *m_display_mesh;
}

// This functions figures out which node should be tied to which node
void CHomogenizationBox::SortOutTyings(std::map<int, std::set<int> > &connections) const
{
	const std::vector<int> &back = m_mesh->BackFace();
	const std::vector<int> &front = m_mesh->FrontFace();
	const std::vector<int> &top = m_mesh->TopFace();
	const std::vector<int> &bottom = m_mesh->BottomFace();
	const std::vector<int> &right = m_mesh->RightFace();
	const std::vector<int> &left = m_mesh->LeftFace();

	assert(back.size() == front.size());
	assert(top.size() == bottom.size());
	assert(left.size() == right.size());

	std::map<int, std::set<int> >::iterator it;

	for(size_t i = 0; i < right.size(); i++)
	{
		it = connections.find(right[i]);
		if(it == connections.end())
		{
			std::set<int> cnctns;
			cnctns.insert(left[i]);
			connections.insert(std::map<int, std::set<int> >::value_type(right[i], cnctns));
		}
		else
		{
			assert(it != connections.end());
			(*it).second.insert(right[i]);
		}
	}

	for(size_t j = 0; j < top.size(); j++)
	{
		it = connections.find(top[j]);
		if(it == connections.end())
		{
			std::set<int> cnctns;
			cnctns.insert(bottom[j]);
			connections.insert(std::map<int, std::set<int> >::value_type(top[j], cnctns));
		}
		else
		{
			assert(it != connections.end());
			(*it).second.insert(bottom[j]);
		}
	}
	
	for(size_t k = 0; k < back.size(); k++)
	{
		it = connections.find(back[k]);
		if(it == connections.end())
		{
			std::set<int> cnctns;
			cnctns.insert(front[k]);
			connections.insert(std::map<int, std::set<int> >::value_type(back[k], cnctns));
		}
		else
		{
			assert(it != connections.end());
			(*it).second.insert(front[k]);
		}
	}

	/*
		Now the connections map is filled with indices of slave nodes as key and a vector of indices
		to which the slave nodes are tied. 

		Slave Node		Master Node(s)
		1				2, 3, 8
		3				4, 5
		2				4, 6
		4				7
		5				7
		8				5, 6
		6				7

		Clearly node nr. 7 is the only node that is only a master node. In the end all the nodes will have to be
		tied to this node since Diana won't accept a situation like:
		
		Fix		-
		1	3	 |
		Fix		 >	node 3 is slave as well as master
		3	4	 |
				-

		This algorithm below fixes this. In the end each node will either be a slave or a master node.
	*/
	for(it = connections.begin(); it != connections.end(); it++)
	{
		std::set<int> current = (*it).second;

		if(current.size() == 1) 
			continue;

		int first = *(current.begin());
		while(connections.find(first) != connections.end())
		{ // as soon as this conditions is not satisfied we found a node which is only a mater node!
		  // see node nr. 7 in the example above.

			first = *(connections.find(first)->second.begin());
		}
			
		// now first is the index of the node to which we have to be tied.....
		current.clear();
		current.insert(first);
		(*it).second = current;
	}
}

// This functions actually creates the tyings in the mesh. Use SortOutTyings() to fill the connections map
// Make sure that the springs are set up by calling SetupSpringElements() first
void CHomogenizationBox::SetupTyings(const std::map<int, std::set<int> > &connections)
{
	//		CreateTying(int nSlave, const IVector &vecSlaveDir, CTying::TType nSlaveType,
//	int nMaster, const IVector &vecMasterDir, CTying::TType nMasterType, double dFactor)
	geo::CVector x = geo::CVector::Xaxis;
	geo::CVector y = geo::CVector::Yaxis;
	geo::CVector z = geo::CVector::Zaxis;

	std::map<int, std::set<int> >::const_iterator it;
		
	// Make sets of the right, back and top face
	std::set<int> top;
	std::set<int> back;
	std::set<int> right;

	for(size_t i = 0; i < m_mesh->RightFace().size(); i++)
		right.insert(m_mesh->RightFace()[i]);
	
	for(size_t j = 0; j < m_mesh->BackFace().size(); j++)
		back.insert(m_mesh->BackFace()[j]);

	for(size_t k = 0; k < m_mesh->TopFace().size(); k++)
		top.insert(m_mesh->TopFace()[k]);

	for(it = connections.begin(); it != connections.end(); it++)
	{
		std::vector<geo::CTying::TMasterDef> masters;
		
// First the x-direction tyings *************************************************
		geo::CTying::TMasterDef m1, m2, m3, m4;
		m1.node = *((*it).second.begin());
		m1.direction = &x;
		m1.type = geo::CTying::TR;
		m1.factor = 1.0;
			
		masters.push_back(m1);

		if(top.find((*it).first) != top.end())
		{
			m2.node = m_vcTopSprings[0];
			m2.direction = &x;
			m2.type = geo::CTying::TR;
			m2.factor = 1.0;
			masters.push_back(m2);
		}

		if(back.find((*it).first) != back.end())
		{
			m3.node = m_vcBackSprings[0];
			m3.direction = &x;
			m3.type = geo::CTying::TR;
			m3.factor = 1.0;
			masters.push_back(m3);
		}

		if(right.find((*it).first) != right.end())
		{
			m4.node = m_vcRightSprings[0];
			m4.direction = &x;
			m4.type = geo::CTying::TR;
			m4.factor = 1.0;
			masters.push_back(m4);
		}

		int nSlave = it->first;
		assert(nSlave >= 0 && nSlave < m_mesh->NodeSize());
		

		// Now create the tying
		m_mesh->CreateTying((*it).first, x, geo::CTying::TR, masters);
// ********************************************************************************
		
// Now the y-direction tyings *************************************************
		m1.direction = &y;
		masters.clear();
		masters.push_back(m1);

		if(top.find((*it).first) != top.end())
		{
			m2.node = m_vcTopSprings[1];
			m2.direction = &y;
			m2.type = geo::CTying::TR;
			m2.factor = 1.0;
			masters.push_back(m2);
		}

		if(back.find((*it).first) != back.end())
		{
			m3.node = m_vcBackSprings[1];
			m3.direction = &y;
			m3.type = geo::CTying::TR;
			m3.factor = 1.0;
			masters.push_back(m3);
		}

		if(right.find((*it).first) != right.end())
		{
			m4.node = m_vcRightSprings[1];
			m4.direction = &y;
			m4.type = geo::CTying::TR;
			m4.factor = 1.0;
			masters.push_back(m4);
		}

		// Now create the tying
		m_mesh->CreateTying((*it).first, y, geo::CTying::TR, masters);
// ********************************************************************************

// And finally the z-direction tyings *************************************************
		m1.direction = &z;
		masters.clear();
		masters.push_back(m1);

		if(top.find((*it).first) != top.end())
		{
			m2.node = m_vcTopSprings[2];
			m2.direction = &z;
			m2.type = geo::CTying::TR;
			m2.factor = 1.0;
			masters.push_back(m2);
		}

		if(back.find((*it).first) != back.end())
		{
			m3.node = m_vcBackSprings[2];
			m3.direction = &z;
			m3.type = geo::CTying::TR;
			m3.factor = 1.0;
			masters.push_back(m3);
		}

		if(right.find((*it).first) != right.end())
		{
			m4.node = m_vcRightSprings[2];
			m4.direction = &z;
			m4.type = geo::CTying::TR;
			m4.factor = 1.0;
			masters.push_back(m4);
		}

		// Now create the tying
		m_mesh->CreateTying((*it).first, z, geo::CTying::TR, masters);
// ********************************************************************************
	}

}

// Function inserts the springelements in the mesh that are required for the homogenization calculation
void CHomogenizationBox::SetupSpringElements()
{
	int division = m_mesh->HeightDivision() + 1; // same for all 3 directions, + 1 because we want to be outside the mesh
	double spacing = m_mesh->XSpace();		// same for all 3 directions
	double x_base = m_mesh->XBase();
	double y_base = m_mesh->YBase();
	double z_base = m_mesh->ZBase();

	// springelements next to right face of the mesh
	geo::CPoint p1(x_base + (spacing * (double)division), y_base, z_base);
	geo::CPoint p2(x_base + (spacing * (double)division), y_base + spacing, z_base);
	geo::CPoint p3(x_base + (spacing * (double)division), y_base, z_base + spacing);

	// springelements above top face of the mesh
	geo::CPoint p4(x_base + spacing, y_base, z_base + (spacing * (double)division));
	geo::CPoint p5(x_base, y_base + spacing, z_base + (spacing * (double)division));
	geo::CPoint p6(x_base, y_base, z_base + (spacing * (double)division));

	// springelements behind back face of the mesh
	geo::CPoint p7(x_base + spacing, y_base + (spacing * (double)division), z_base);
	geo::CPoint p8(x_base, y_base + (spacing * (double)division), z_base);
	geo::CPoint p9(x_base, y_base + (spacing * (double)division), z_base + spacing);


	// The indices of the nodes belonging to the springelements are saved in the member variables
	// m_vcRightSprings, m_vcTopSprings and m_vcBackSprings. We will need them when we are applying the
	// supports.
	int nIndex;
	
	m_vcRightSprings.clear();
	m_vcTopSprings.clear();
	m_vcBackSprings.clear();

	nIndex = m_mesh->RegisterNode(p1);
	m_vcRightSprings.push_back(nIndex);
	nIndex = m_mesh->RegisterNode(p2);
	m_vcRightSprings.push_back(nIndex);
	nIndex = m_mesh->RegisterNode(p3);
	m_vcRightSprings.push_back(nIndex);

	nIndex = m_mesh->RegisterNode(p4);
	m_vcTopSprings.push_back(nIndex);
	nIndex = m_mesh->RegisterNode(p5);
	m_vcTopSprings.push_back(nIndex);
	nIndex = m_mesh->RegisterNode(p6);
	m_vcTopSprings.push_back(nIndex);

	nIndex = m_mesh->RegisterNode(p7);
	m_vcBackSprings.push_back(nIndex);
	nIndex = m_mesh->RegisterNode(p8);
	m_vcBackSprings.push_back(nIndex);
	nIndex = m_mesh->RegisterNode(p9);
	m_vcBackSprings.push_back(nIndex);
	
	// finally we can create the elements!
	new CSpringElement(*m_mesh, m_vcRightSprings[0]);
	new CSpringElement(*m_mesh, m_vcRightSprings[1]);
	new CSpringElement(*m_mesh, m_vcRightSprings[2]);
	new CSpringElement(*m_mesh, m_vcTopSprings[0]);
	new CSpringElement(*m_mesh, m_vcTopSprings[1]);
	new CSpringElement(*m_mesh, m_vcTopSprings[2]);
	new CSpringElement(*m_mesh, m_vcBackSprings[0]);
	new CSpringElement(*m_mesh, m_vcBackSprings[1]);
	new CSpringElement(*m_mesh, m_vcBackSprings[2]);
}

void CHomogenizationBox::SetupSupports()
{
	m_mesh->CreateTranslationSupport(m_mesh->BaseNode(), geo::CVector::Xaxis);
	m_mesh->CreateTranslationSupport(m_mesh->BaseNode(), geo::CVector::Yaxis);
	m_mesh->CreateTranslationSupport(m_mesh->BaseNode(), geo::CVector::Zaxis);
	
	m_mesh->CreateTranslationSupport(m_vcRightSprings[0], geo::CVector::Xaxis);
	m_mesh->CreateTranslationSupport(m_vcRightSprings[1], geo::CVector::Yaxis);
	m_mesh->CreateTranslationSupport(m_vcRightSprings[2], geo::CVector::Zaxis);

	m_mesh->CreateTranslationSupport(m_vcTopSprings[0], geo::CVector::Xaxis);
	m_mesh->CreateTranslationSupport(m_vcTopSprings[1], geo::CVector::Yaxis);
	m_mesh->CreateTranslationSupport(m_vcTopSprings[2], geo::CVector::Zaxis);

	m_mesh->CreateTranslationSupport(m_vcBackSprings[0], geo::CVector::Xaxis);
	m_mesh->CreateTranslationSupport(m_vcBackSprings[1], geo::CVector::Yaxis);
	m_mesh->CreateTranslationSupport(m_vcBackSprings[2], geo::CVector::Zaxis);
}

/* 
	CHomoBoxEntry implementation ********************************************************
*/
CHomoBoxEntry::CHomoBoxEntry(CModelBase &model)
:CStorageNodeEntry<CHomogenizationBox>(MD_BASE_HOMO_BOX, IDI_SCALINGBOX, IDS_SCALINGBOXES, (CFemAppModel&)model)
{
	
}

/* 
	*************************************************************************************
*/
// For each point in the native pointset of the homogenization a check has to be performed to see
// if it is inside the model, this is necessary when the Young's modulus and poisson ratio have to
// determined from the model.
bool CHomogenizationBox::InsideModel() const
{
	TFormationBaseEntry& entry = (TFormationBaseEntry&)*Model().GraphEntry(MD_BASE_FORMATION);
	TFormationBaseEntry::TNodeSet stNode = entry.EntryNodes();

	const CPointSet *pSet = m_mesh_ptset;
	bool bInside;
	for(int i = 0; i < pSet->PointSize(); i++)
	{
		bInside = false;
		for(TFormationBaseEntry::TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
		{
			if((*it)->Contains(pSet->PointAt(i), true))
			{
				bInside = true;
				break;
			}
		}
		if(!bInside)
			return false;
	}

	return true;
}


