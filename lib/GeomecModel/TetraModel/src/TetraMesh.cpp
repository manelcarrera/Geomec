// TetraMesh.cpp: implementation of the CTetraMesh class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "TetraMesh.h"
#include "TetraModel.h"
#include "ModelBase.h"
#include "TetraBoundary.h"
#include "TetraSubBoundary.h"
#include "TetraFormation.h"
#include "TetraEntryTypes.h"
#include "GeomecUtils.h"
#include "ShellGoCadFile.h"
#include "PressureSupportNode.h"
#include "DisplacementSupportNode.h"
#include "TetSurface.h"
#include "InterfaceElement.h"
#include "BranchState.h"
#include "GocadMeshImporter.h"
#include "MeshProgress.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "IPlatform.h"
#include "Environment.h"
#include "BaseEntryTypes.h"
#include "GeoSurface.h"
#include "HorizonBase.h"
#include "TetraHorizonBase.h"
#include "GocadImport.h"
#include "Tetrahedron.h"
#include "ElementGroup.h"
#include "TetraSubHorizon.h"
#include "ISupport.h"
#include "TiedBodyTetSurface.h"
#include "TiedBodyTriangle.h"
#include "RotatedSystem.h"

#include "MeshRefinementModel.h"
#include "RBox.h"
#include "Printer.h"
#include "ISettings.h"
#ifdef HAVE_CM2_MESHER
#include "RefinementBox.h"
#include <fstream> // box_from_file()
#endif

#include "SKUAParseData.h"
#include "QuestionThreadWrapper.h"
#include "Interface.h" // IMesherDlg


#include <stdio.h> //remove file

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTetraMesh::CTetraMesh(CTetraModel& model)
: CMeshBase("Mesh", model.DepletionStageEntry()),
  m_bCanInvalidateMesh(true),
  m_bBuildingFromGocad(false),
  m_dMaxGocadSurfaceAngle(50 * PI / 180),
  m_bImportedMesh(false),
#ifdef HAVE_CM2_MESHER
  m_pMesh(geo::CTetMeshBase::GetTetMesher("CM2"))
#else
  m_mesherType(FGV),
  m_pMesh(geo::CTetMeshBase::GetTetMesher())
#endif
{
	// Link to point-set entry
	LinkTo(*model.GraphEntry(MD_BASE_POINTSET));

	m_mpElementType.insert(std::map<std::string, int>::value_type("TE12L", 0)); // the tetrahedra
	m_mpElementType.insert(std::map<std::string, int>::value_type("T9CT", 1));  // the contactelements
	m_mpElementType.insert(std::map<std::string, int>::value_type("T18IF", 2)); // the interface elements

  m_pMesh->Tolerance(1E-2);
}

CTetraMesh::~CTetraMesh()
{
  // allow invalidation now so we can free memory
	m_bCanInvalidateMesh = true;
  m_bImportedMesh = false;
	InvalidateMesh();
  delete m_pMesh;
}

int CTetraMesh::AddSurface(const geo::ISurface& surface, const QString& sName, bool bFault)
{
	if(m_bCanInvalidateMesh && !m_bImportedMesh)
		return m_pMesh->AddSurface(surface, sName.toStdString(), bFault);
	
	// Instantiate shell surface
	int nRet = m_pMesh->NrOfSurfaces();
	new CShellGoCadFile::CShellSurfaceDesc(*m_pMesh, surface, sName.toStdString(), bFault);
	assert((nRet + 1) == m_pMesh->NrOfSurfaces());
	return nRet;
}

int CTetraMesh::InputSurfaceSize() const
{
	assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());
	assert(IsMesh());
	return m_pMesh->NrOfSurfaces();
}

CTetraMesh::TInputSurface CTetraMesh::InputSurface(int nIndex) const
{
	assert(nIndex >= 0);
	assert(nIndex < InputSurfaceSize());
	return TInputSurface(&m_pMesh->SurfaceDesc(nIndex), m_vcSurfaceSource[nIndex]);
}

bool CTetraMesh::IsMesh() const
{
	 return m_pMesh->ElementSize() > 0 && !m_bBuildingFromGocad;
} 

bool CTetraMesh::CanInvalidateMesh() const
{
	return m_bCanInvalidateMesh != false /*&& !m_bImportedMesh*/ && !(static_cast<const CModelBase&>(Model())).BranchState().IsBranch();
}

// Meshing interface
bool CTetraMesh::CanCreateMesh() const
{
	if(Locked()) return false;

	const CTetraModel *pModel = dynamic_cast<const CTetraModel*> (&Model());
	assert(pModel);

	const CTetraBoundary& boundary = dynamic_cast<const CTetraBoundary&>(pModel->Boundary());
	if(boundary.IsSuperModel())
	{
		if(!boundary.IsSuperModelValid())
			return false;
	}

	return (pModel->SubBoundary().TopHorizon().Horizon() != 0) && (pModel->SubBoundary().BottomHorizon().Horizon() != 0);
}

namespace
{

const QString DEFAULT_FORMATION_NAME = QObject::tr("Formation%1");
const size_t DEFAULT_INITIAL_NUMBER = 0;

} // anonymous namespace

#ifdef HAVE_CM2_MESHER
int max( const TGradientV& g_v )
{
	int max = 0;
	for( auto g : g_v )
	{
		int size = g.Size();
		if( size > max )
			max = size;
	}
	return max;
}

void print( const gocadmesh::RefinementBox& box )
{
	const T3da& c = box.Center();
	const T3da& s = box.Size();
	const T3da& r = box.Rotation();
	gocadmesh::GradientType g_t = box.GradientType();

	Printer::instance()->debug(
		"[gocadmesh]RefinementBox > c:[%.2f,%.2f,%.2f] s:[%.2f,%.2f,%.2f] r:[%.2f,%.2f,%.2f] g:%d steps:%d", 
		c[ 0 ], c[ 0 ], c[ 0 ],
		s[ 0 ], s[ 0 ], s[ 0 ],
		r[ 0 ], r[ 0 ], r[ 0 ],
		g_t, box.Profile().size() );

	for( auto& gradient_step : box.Profile() )
		Printer::instance()->debug(
			"[gocadmesh]Gradient > [d:%.2F,s:%.2F]", 
			gradient_step.Distance(), 
			gradient_step.Size() );
}

// two implementations:
// 1) when gradient could be any integer
// 2) now gradient is a double from 0 to 1
/*gocadmesh::RelativeSize convert( const RelativeSize& gradient, int max_ )
{
	int d_ = gradient.Distance();
	int s_ = gradient.Size();

	// 1)
	double d  = (double)d_/10.0;
	double _s = (double)s_/(double)max_;
	double s  = std::cbrt( _s );

	return gocadmesh::RelativeSize( d, s );
}*/

gocadmesh::RelativeSize convert( const RelativeSize& gradient, int max_ )
{
	int d_ = gradient.Distance();
	double s_ = gradient.Size();

	double d  = (double)d_/10.0;
	double s  = std::cbrt( s_ );

	return gocadmesh::RelativeSize( d, s );
}

gocadmesh::RefinementBox box_from_file( std::string file )
{
	gocadmesh::RefinementBox b2 = gocadmesh::RefinementBox();

	// FIXME: No need to be fixed, just for testing
	std::string path = "C:\\aWork\\aGeomec\\Trunk_0525\\x64\\ReleaseWithDebug";
	std::string url = path + "\\" + file;

	std::ifstream infile( url );
	if( !infile.is_open() ) // ERROR
		return b2; //emnpty

	double x, y, z;

	infile >> x >> y >> z;
	std::array<double,3> c{ x, y, z };

	infile >> x >> y >> z;
	std::array<double,3> s{ x, y, z };

	b2.SetCenter(		c );
	b2.SetSize(			s );
	b2.SetRotation(		std::array<double,3>{ 0.0, 0.0, 0.0 } );
	b2.SetGradientType(	gocadmesh::PlanarZ );

	// gradient ...

	std::vector< gocadmesh::RelativeSize > p2;
	while( infile >> x >> y )
	{
		gocadmesh::RelativeSize g( x, y );
		p2.push_back( g );
	}
	b2.SetProfile(	p2 );

	print( b2 );
	return b2;
}

std::vector< gocadmesh::RefinementBox > boxes_from_file()
{
	std::vector< gocadmesh::RefinementBox > diana_boxes_v;

	gocadmesh::RefinementBox data = box_from_file( "refinement.txt" );
	diana_boxes_v.push_back( data );

	//data = box_from_file( "refinement2.txt" );
	//diana_boxes_v.push_back( data );

	return diana_boxes_v;
}


/* 
Comments::
1. OIV and real world coordinates needs to be adapted
2. OIV box size goes from center to the edge, so it needs to be double for real world
*/
gocadmesh::RefinementBox convert( const RefinementBox b1, T3da& t )
{
	gocadmesh::RefinementBox b2 = gocadmesh::RefinementBox();

	const T3da& c = b1.Center();
	//T3da& t = RefinementBox::m_translation;

	const T3da& s = b1.Size();

	b2.SetCenter(		T3da{ c[0]+t[0], c[1]+t[1], c[2]+t[2] } );
	b2.SetSize(			T3da{ 2*s[0], 2*s[1], 2*s[2] } );
	b2.SetRotation(		b1.Rotation() );

	b2.SetGradientType(	(gocadmesh::GradientType)b1.GradientType() );
	// FIXME
	//int type_ = ISettings::instance()->getProfileInt("MeshRefinement","Type", (int)gocadmesh::PlanarZ );
	//b2.SetGradientType( (gocadmesh::GradientType)type_ );

	int max_size = max( b1.Profile() );
	std::vector< gocadmesh::RelativeSize > p2;
	for( auto g : b1.Profile() ) // gradient steps
	{
		gocadmesh::RelativeSize g2 = convert( g, max_size );
		p2.push_back( gocadmesh::RelativeSize( g2 ) );
		//b2->AddToProfile( gocadmesh::RelativeSize( g.Distance(), g.Size() ) ); // doesnt work as expected
	}
	//if( p2.size() > 1 ) // 
		p2.pop_back(); // if empty causes undefined behavior

	b2.SetProfile(	p2 );

	print( b2 );
	
	return b2;
}

std::vector< gocadmesh::RefinementBox > convert( const TBoxV& in_v, T3da& t )
{
	std::vector< gocadmesh::RefinementBox > out_v;
	for( auto& b1 : in_v )
	{
		gocadmesh::RefinementBox b2 = convert( b1, t );
		out_v.push_back( b2 );
	}
	return out_v;
}
#endif


void CTetraMesh::CreateMesh()
{
	CTetraModel *pModel = dynamic_cast<CTetraModel*> (&Model());
	assert(pModel);

#ifdef HAVE_CM2_MESHER
	geo::CTetMeshCM2* cm2_mesher = dynamic_cast< geo::CTetMeshCM2* >( m_pMesh );
	if ( cm2_mesher )
	{
		TBoxV boxes_v = pModel->m_mesh_refinement_data->data_v();

		T3da t = RefinementBox::m_translation;
		if( t == T3da{0,0,0} )
		{
			CBoundaryBase& b = pModel->Boundary();
			geo::CPoint M1 = b.Max();
			geo::CPoint m1 = b.Min();

			t = T3da{ 
				( m1.X() + M1.X() ) / 2, 
				( m1.Y() + M1.Y() ) / 2,
				( m1.Z() + M1.Z() ) / 2 };
		}

		std::vector< gocadmesh::RefinementBox > diana_boxes_v = convert( boxes_v, t ); // from Geomec types scope to diana types scope
		//std::vector< gocadmesh::RefinementBox* > diana_boxes_v = boxes_from_file(); // just for testing

		cm2_mesher->set_mesh_refinement_boxes( diana_boxes_v );
	}
#endif


	std::auto_ptr <IProgressBase> dlg(_g->prog()->create(eProgress::Geo, getStringTableEntry(1), false));
	

	// Start collecting surfaces by the boundary of the super model. Note that the mesh loading
	// routine assumes the same order. So if you ever adapt this order don't forget to adapt
	// the loading routine ....
	CTetraBoundary *pBoundary = dynamic_cast<CTetraBoundary*>(&pModel->Boundary());
	assert(pBoundary);

	for(int i = 0; i < pBoundary->SideSurfaceSize(); i++)
	{
		// Register the side surfaces in the mesh and add the pointer of the super boundary to 
		// the source vector.
		m_pMesh->AddSurface(pBoundary->SideSurface(i), pBoundary->SideSurfaceName(i).toStdString(), false);
		m_vcSurfaceSource.push_back(pBoundary);
		assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());
	}

	// When the super model boundary is equivalent to the sub boundary the super boundary is
	// in the default state. If not register the surfaces of the boundary. 
	if(pBoundary->State() != CTetraBoundary::DEFAULT_DEFINED)
	{
		for(int i = 0; i < pModel->SubBoundary().SideSurfaceSize(); i++)
		{
			// Register the side surfaces in the mesh and add the pointer of the sub boundary to 
			// the source vector.
			m_pMesh->AddSurface(pModel->SubBoundary().SideSurface(i).Surface(), pModel->SubBoundary().SideSurface(i).Name().toStdString(), false);
			m_vcSurfaceSource.push_back(&pModel->SubBoundary());
			assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());
		}
	}

	// The next step is registering all horizons of the model in the mesh.
	THorizonBaseEntry *pEntry = (THorizonBaseEntry*)Model().GraphEntry(MD_BASE_HORIZON);
	assert(pEntry);

	// Walk over all horizons
	THorizonBaseEntry::TNodeSet stHorizon = pEntry->EntryNodes();
	for(THorizonBaseEntry::TNodeSet::iterator it = stHorizon.begin(); it != stHorizon.end(); it++)
	{
		CTetraHorizonBase& horizon = dynamic_cast<CTetraHorizonBase&>(**it);
		if(horizon.ConstantDepth())
		{
			// In the constant depth case we register the horizon's generated constant depth
			// surface and we store the pointer to the horizon
			m_pMesh->AddSurface(horizon.GeneratedConstantDepthSurface(), horizon.Name().toStdString(), (*it)->Slip());
			m_vcSurfaceSource.push_back(&horizon);
			assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());
		}
		else
		{
			// Walk over the horizon's surfaces and register them in the mesh
			for(int i = 0; i < horizon.SurfaceSize(); i++)
			{
				m_pMesh->AddSurface(horizon.Surface(i).Surface(), horizon.Surface(i).Name().toStdString(), (*it)->Slip());
				m_vcSurfaceSource.push_back(&horizon.Surface(i));
				assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());
			}
		}
	}
	
	// create temp directory for mesh creation procedure and change to it
	QString strTempDir = CreateTempDirExt(CTempPath::TEMP_GENERAL).toStdString().c_str();
	QString strOldDir = ChangeDirectory(strTempDir.toStdString().c_str());

	CEnvironment* env = CEnvironment::instance();
	std::string sFgvExeName = env->get( CEnvironment::FgvExeName ).toStdString();
	if(sFgvExeName.empty())
	{
		_m()->msg("The mesher program is not installed");
		ChangeDirectory(strOldDir.toStdString().c_str());
		return;
	}

	CMeshProgress progress(*dlg);
	geo::CTetMeshBase::TRet ret = m_pMesh->CreateMesh(
		env->get( CEnvironment::DIAPATH ).toStdString(), 
		env->get( CEnvironment::DIASHARE ).toStdString(), 
		env->get( CEnvironment::DIALIB ).toStdString(), 
		env->get( CEnvironment::DIASLIB ).toStdString(), 
		sFgvExeName,
		progress);

	ChangeDirectory(strOldDir.toStdString().c_str());
//	VERIFY(RemoveDir(strTempDir));

	auto show_error_dlg=[=]( const std::vector< std::string >& data )
	{
		if( _g->is_gui_thread() )
		{
			CModelBase* model = dynamic_cast< CModelBase* >( &Model() );
			IMesherDlg* dlg = nullptr;
			if( model && ( dlg = model->mesher_dlg() ) )
			{
				dlg->data( data );
				dlg->DoModal_(); // 'OnExport' and 'OnExportPoints' happens inside
			}
		}
		//
		// in principle this case never happens as almost everything happens in the mani gui thread
		//
		else
		{
			if( _e->registered( Question ) )
			{	
				CQuestionThreadWrapper th( CQuestionThreadWrapper::MesherDlg, new std::vector<std::string>{ data } );
				th.join();
			}
			else
			{
				_m()->msg( data );
			}
		}
	};


	if(!ret.second || !m_pMesh->ElementSize())
	{
		if(!m_pMesh->ElementSize())
			ret.first.push_back(std::string("ERROR: No elements generated by the mesher"));

		//globalTetMesherMessage(ret.first);
		show_error_dlg( ret.first );
		InvalidateMesh();
		return;
	}
	
	if(ret.first.size() > 0)
	{
		//globalTetMesherMessage( ret.first );
		show_error_dlg( ret.first );
	}

	// Assign tetra hedrons to volumes of formations
	CTetraFormationEntry *pFormationEntry = dynamic_cast<CTetraFormationEntry*> (Model().GraphEntry(MD_TETRA_FORMATION));
	assert(pFormationEntry);
	CTetraFormationEntry::TNodeSet stFormation = pFormationEntry->EntryNodes();

	// Fetch all existing volumes from the existing formations
	typedef std::set<CTetraFormationVolume*> TVolumeSet;
	TVolumeSet stVolume;
	for(CTetraFormationEntry::TNodeSet::iterator it_for = stFormation.begin(); it_for != stFormation.end(); it_for++)
	{
		CFormationBase& formation = **it_for;
		for(int i = 0; i < formation.ElementSetSize(); i++)
		{
			stVolume.insert((CTetraFormationVolume*)(&formation.ElementSet(i)));
		}
	}

	// Try to couple all volumes generated with mesher
	for(int i = 0; i < m_pMesh->NrOfVolumes(); i++)
	{
		if(stVolume.size() > 0)
		{
      TGraphNodeSet meshVolumeId = VolumeIdentity(m_pMesh->Volume(i));

			// Find the most suitable existing volume: minimal (error) best fitting (rank) set of side surfaces
			int nMaxRank = -1;
      int nMinError = INT_MAX;

			CTetraFormationVolume *pVolume = 0;
			for(TVolumeSet::iterator it = stVolume.begin(); it != stVolume.end(); it++)
			{
				CTetraFormationVolume& volume = **it;
				int nRank = volume.Resemblance(meshVolumeId);
        int nError = std::abs((int)meshVolumeId.size() - (int)volume.Identifier().size());
				if(nRank > nMaxRank || (nRank == nMaxRank && nError < nMinError))
				{
					nMaxRank = nRank;
          nMinError = nError;
					pVolume = &volume;
				}
			}

			assert(pVolume);

			// Assign and remove from set
			stVolume.erase(pVolume);
			VERIFY(pVolume->Volume(m_pMesh->Volume(i)));
			pVolume->Identifier(VolumeIdentity(m_pMesh->Volume(i)));
		}
		else
		{
			// Create a formation and a volume. First we create a proper default formation name
      QString sFormationName = pFormationEntry->createUniqueName(
        DEFAULT_FORMATION_NAME, DEFAULT_INITIAL_NUMBER);
			CTetraFormation *pFormation = new CTetraFormation(sFormationName.toStdString().c_str(), (CModelBase&)Model());
			CTetraFormationVolume *pVolume = new CTetraFormationVolume(*pFormation, (geo::CBodyGroup&)m_pMesh->Volume(i));

			pVolume->Volume(m_pMesh->Volume(i));
			pVolume->Identifier(VolumeIdentity(m_pMesh->Volume(i)));

			/* From IFormationElementSet constructor */
			pVolume->reParent(pFormation);
//			pVolume->LinkTo(pModel->Mesh());
			/* ************************************* */
		}
	}	

	// All volumes are assigned. Delete the left overs
	for(TVolumeSet::iterator it_vol = stVolume.begin(); it_vol != stVolume.end(); it_vol++)
	{
		delete *it_vol;
	}

	assert(IsLinkedTo(pModel->Boundary()));
	Modified();
}

CTetraMesh::eMesherType CTetraMesh::CurrentMesherType() const
{
#ifdef HAVE_CM2_MESHER
  if (dynamic_cast<geo::CTetMeshCM2 *>(m_pMesh))
    return CM2;

  return FGV;
#else
  return m_mesherType;
#endif
}

std::string CTetraMesh::CurrentMesher() const
{
  switch (CurrentMesherType())
  {
  default:
  case FGV: return "";
  case CM2: return "CM2";
  }
}

void CTetraMesh::SwitchMesher(std::string type)
{
#ifdef HAVE_CM2_MESHER
  if (type != CurrentMesher())
  {
    double eps = 1E-2;
    
    if (m_pMesh)
      eps = m_pMesh->Tolerance();

    m_bCanInvalidateMesh = true;
    m_bImportedMesh = false;
    InvalidateMesh();
    delete m_pMesh;

    m_pMesh = geo::CTetMeshBase::GetTetMesher(type);
    m_pMesh->Tolerance(eps);

    assert(type == CurrentMesher());
  }
#endif
}

void CTetraMesh::SwitchMesher(eMesherType type)
{
#ifdef HAVE_CM2_MESHER
  switch (type)
  {
  case FGV: SwitchMesher(""); break;
  case CM2: SwitchMesher("CM2"); break;
  default:
    assert(false);
  }
#else
  m_mesherType = type;
#endif
}


const CSurfaceBase* CTetraMesh::Surface(const geo::ISurface& surface)
{
	CSurfaceEntry *pEntry = (CSurfaceEntry*)(Model().GraphEntry(MD_BASE_SURFACE));
	assert(pEntry);
	CSurfaceEntry::TNodeSet stSurface = pEntry->EntryNodes();
	for(CSurfaceEntry::TNodeSet::const_iterator it = stSurface.begin(); it != stSurface.end(); it++)
	{
		if(&(*it)->Surface() == &surface)
			return *it;
	}

	return 0;
}

void CTetraMesh::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	CTetraModel *pModel = static_cast<CTetraModel*> (&Model());

	// Unlink from the node entries
	UnLink(*Model().GraphEntry(MD_BASE_OPENGL_NODE));
	UnLink(*Model().GraphEntry(MD_BASE_COLOR_NODE));

  int mesherType = 0;
  if (version > CStreamVersion(4, 1, 65))
  {
    stream >> mesherType;

    if (mesherType > 1)
    {
      mesherType = 1;
      _m()->msg("The mesh was created with the CM2 Repair option, but this is no longer supported.\n"
        "We are treating it as a normal CM2 mesh, but you may want to check and/or re-mesh the mesh.");
    }
  }
  SwitchMesher((eMesherType)mesherType);

	// Load base first
	CMeshBase::LoadStream(stream, version, progress);
	progress.StatusMessage("Loading mesh");

	if(CStreamVersion(3, 0, 11) < version)
	{
		int canInvalidateMesh;
		stream >> canInvalidateMesh;
		m_bCanInvalidateMesh = (canInvalidateMesh != 0);
	}

	int nSubSideSurface = 0;
	int nSuperSideSurface = 0;
	int nDuplicateSurfaces = 0;

	if(CStreamVersion(3, 0, 3) < version)	// New in 3.0.4
	{
		// In the 3.0.4 version we have to reinitialize the input surfaces. The boundary surfaces
		// have the same order as in the 'CreateMesh' function so there not loaded. The surfaces and
		// horizons are loaded after the registration of the boundaries. When the nIndexSize == 0 
		// there is no mesh and we have to assure that the 'invalidate mesh' state is not violated.
		int nIndexSize;
		stream >> nIndexSize;
		if(nIndexSize > 0)
		{
			if(version < CStreamVersion(3, 0, 10)) // In version 3.0.10 are boundary surfaces also saved
			{
				CTetraBoundary *pBoundary = static_cast<CTetraBoundary*>(&pModel->Boundary());
				// Register super boundary first
				for(int i = 0; i < pBoundary->SideSurfaceSize(); i++)
				{
					// Register the side surfaces in the mesh and add the pointer of the super boundary to 
					// the source vector.
					AddSurface(pBoundary->SideSurface(i), pBoundary->SideSurfaceName(i), false);
					m_vcSurfaceSource.push_back(pBoundary);
					assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());
				}
				
				// When the super model boundary is equivalent to the sub boundary the super boundary is
				// in the default state. If not register the surfaces of the boundary. 
				if(pBoundary->State() != CTetraBoundary::DEFAULT_DEFINED)
				{
					for(int i = 0; i < pModel->SubBoundary().SideSurfaceSize(); i++)
					{
						// Register the side surfaces in the mesh and add the pointer of the sub boundary to 
						// the source vector.
						AddSurface(pModel->SubBoundary().SideSurface(i).Surface(), pModel->SubBoundary().SideSurface(i).Name(), false);
						m_vcSurfaceSource.push_back(&pModel->SubBoundary());
						assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());
					}
				}
			}

			// Now retrieve the indices of the surfaces and horizons belonging the mesh surfaces
			CTetraBoundary *pBoundary = static_cast<CTetraBoundary*>(&pModel->Boundary());
			for(int i = 0; i < nIndexSize; i++)
			{
				// Read the index and get them from the opengl node entry
				int nIndex;
				stream >> nIndex;

				if(nIndex >= 0)
				{
					if (version < CStreamVersion(4, 1, 14))
					{
						// check if we already have it
						bool bFound = false;
						for (int j = 0; j < m_vcSurfaceSource.size(); ++j)
						{
							if (m_vcSurfaceSource[j] != pBoundary && m_vcSurfaceSource[j] != &pModel->SubBoundary())
							{
								if (m_vcSurfaceSource[j]->Index() == nIndex)
								{
									bFound = true;
									break;
								}
							}
						}
					
						if (bFound)
						{
							++nDuplicateSurfaces;
							continue;
						}
					}

					TOpenGLNodeBaseEntry& opengl_entry = (TOpenGLNodeBaseEntry&)*Model().GraphEntry(MD_BASE_OPENGL_NODE);
					CGraphNode* pNode = opengl_entry.FindIndex(nIndex);
					assert(pNode);
					
					// Should be a surface or a horizon
					CTetraHorizonBase *pHorizon1 = dynamic_cast<CTetraHorizonBase*>(pNode);
					CSurfaceBase *pSurface = dynamic_cast<CSurfaceBase*>(pNode);
					assert(pHorizon1 || pSurface);

					if(pHorizon1)
					{
            assert(pHorizon1);
            assert(pHorizon1->ConstantDepth());
						m_vcSurfaceSource.push_back(pHorizon1);
						AddSurface(pHorizon1->GeneratedConstantDepthSurface(), pHorizon1->Name(), pHorizon1->Slip());
						assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());
					}

					if(pSurface)
					{
						CTetraHorizonBase *pHorizon2 = Horizon(*pSurface);
						assert(pHorizon2);
            assert(&pHorizon2->Model() == &Model());
            //if(pHorizon->Slip() && pHorizon->SurfaceSize() == 2) 
            //	InvalidateAtUnLock(); // Multi surfaces we don't support... hajx: apparently we do now
            AddSurface(pSurface->Surface(), pSurface->Name().toStdString().c_str(), pHorizon2->Slip());
            m_vcSurfaceSource.push_back(pSurface);
            assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());
					}
				}
				else if(nIndex == -1) // Super model side surface
				{
					if (version < CStreamVersion(4, 1, 14) && nSuperSideSurface >= pBoundary->SideSurfaceSize())
					{
						++nDuplicateSurfaces;
						continue;
					}
					if (nSuperSideSurface < pBoundary->SideSurfaceSize())
						m_pMesh->AddSurface(pBoundary->SideSurface(nSuperSideSurface), pBoundary->SideSurfaceName(nSuperSideSurface).toStdString(), false);
					m_vcSurfaceSource.push_back(pBoundary);
					nSuperSideSurface++;
					assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());
				}
				else if(nIndex == -2) // Sub model side sidesurface
				{
					if (version < CStreamVersion(4, 1, 14) && nSubSideSurface >= pModel->SubBoundary().SideSurfaceSize())
					{
						++nDuplicateSurfaces;
						continue;
					}
					if (nSubSideSurface < pModel->SubBoundary().SideSurfaceSize())
						m_pMesh->AddSurface(pModel->SubBoundary().SideSurface(nSubSideSurface).Surface(), pModel->SubBoundary().SideSurface(nSubSideSurface).Name().toStdString(), false);
					m_vcSurfaceSource.push_back(&pModel->SubBoundary());
					nSubSideSurface++;
					assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());
				}
			}
		}
	}// End of 3.0.4 shit
	
	// from version 3.0.68 the mesh is saved and loaded differently
	if(CStreamVersion(3, 0, 67) < version)
	{
		int nPoint, TotalPoints;
		stream >> TotalPoints;
		double dX, dY, dZ;
		for(nPoint = 0; nPoint < TotalPoints; nPoint++)
		{
			stream >> dX;
			stream >> dY;
			stream >> dZ;
			geo::CPoint point(dX, dY, dZ);
			m_pMesh->RegisterNodeInMainMesh(point);
			progress.Step();
		}

		int nElement, TotalElements;
		int nElementType;
		stream >> TotalElements;
    m_pMesh->reserveElementSize(TotalElements);

		for(nElement = 0; nElement < TotalElements; nElement++)
		{
			stream >> nElementType;
			if(nElementType == 0)
			{ // tetrahedron
				stream >> TotalPoints;
				int nPtIdx;
				std::vector<int> vcPtIdx;
        vcPtIdx.reserve(TotalPoints);

				for(nPoint = 0; nPoint < TotalPoints; nPoint++)
				{
					stream >> nPtIdx;
					vcPtIdx.push_back(nPtIdx);
				}
				m_pMesh->RegisterTetrahedronInMainMesh(vcPtIdx);
				progress.Step();
			}
			else if(nElementType == 1)
			{ // contact element
				LoadContactElement(stream, progress);
			}
			else
			{ // interface element
				assert(nElementType == 2);
				LoadInterfaceElement(stream, version, progress);
			}
		}
	}
	int nElementSize1;
	// Read points first
	if(version < CStreamVersion(3, 0, 68)) 
	{
		int nPointSize1;
		stream >> nPointSize1;

		for(int nPoint = 0; nPoint < nPointSize1; nPoint++)
		{
			// Read point
			double x, y ,z;
			stream >> x;
			stream >> y;
			stream >> z;

			// Write
			geo::CPoint point(x,y,z);
			VERIFY(nPoint == m_pMesh->RegisterNodeInMainMesh(point));

			progress.Step();
		}

		// Read elements
		//int nElementSize;
		stream >> nElementSize1;

		for(int nElement = 0; nElement < nElementSize1; nElement++)
		{
			// Load element point size
			int nPointSize2;
			stream >> nPointSize2;

			// Load element connectivity
			std::vector<int> vcPoint(nPointSize2);
			for(int n = 0; n < vcPoint.size(); n++)
			{
				int nIndex;
				stream >> nIndex;
				vcPoint[n] = nIndex;
			}

			VERIFY(nElement == m_pMesh->RegisterTetrahedronInMainMesh(vcPoint));

			progress.Step();
		}
	}

	if(CStreamVersion(3, 0, 3) < version)	// New in 3.0.4
	{
		// Load the volumes of the mesh and their corresponding surfaces
		int nVolumeSize;
		stream >> nVolumeSize;

		for(int nVolume = 0; nVolume < nVolumeSize; nVolume++)
		{
			// Load element indices of the volume and inflate the volume with it
			int nElementSize2;
			stream >> nElementSize2;
			std::vector<int> vcIndex1(nElementSize2);

      int nElement;
			for(nElement = 0; nElement < nElementSize2; nElement++)
			{
				stream >> vcIndex1[nElement];
				progress.Step();
			}

			// Create a volume
			VERIFY(nVolume == m_pMesh->RegisterTetVolume(vcIndex1));

      typedef std::map<std::set<int>, const geo::IFace*> TNodesToFaceMap;
      TNodesToFaceMap mpNodesToFace;

      if(CStreamVersion(3, 0, 104) < version && version < CStreamVersion(4, 1, 13))
      {
        const geo::CBodyGroup& vol = m_pMesh->Volume(nVolume);
        for(nElement = 0; nElement < vol.ElementSize(); ++nElement)
        {
          const geo::IBody* pBody = static_cast<const geo::IBody*>(&vol.Element(nElement));
          int nFace;
          for(nFace = 0; nFace < pBody->NrOfFaces(); ++nFace)
          {
            const geo::IFace& face = pBody->Face(nFace);
            if(face.NrOfNodes() == 3)
            {
              std::set<int> stNodes;
              int i;
              for(i = 0; i < 3; ++i)
                stNodes.insert(face.Node(i).Index());

              assert(stNodes.size() == 3); // should be unique indices

              mpNodesToFace.insert(TNodesToFaceMap::value_type(stNodes, &face)).second;
            }
          }
        }
      }

			// Walk over tet surfaces of volume and restore them
			int nSurfaceSize;
			stream >> nSurfaceSize;

			for(int nSurface = 0; nSurface < nSurfaceSize; nSurface++)
			{
				// Load the index of the corresponding surface descriptor
				int nSurfaceDescriptor;
				stream >> nSurfaceDescriptor;

        if(version < CStreamVersion(3, 0, 105))
        {
				  // Read point indices of the tet surface
				  int nIndexSize;
				  stream >> nIndexSize;
				  std::vector<int> vcIndex2(nIndexSize);
				  for(int n = 0; n < vcIndex2.size(); n++)
				  {
					  stream >> vcIndex2[n];
					  progress.Step();
				  }
				  if(nSurfaceDescriptor < m_pMesh->NrOfSurfaces())	// Bug #1145
				  {
					  m_pMesh->RegisterTetSurface((geo::CSurfaceDesc&)m_pMesh->SurfaceDesc(nSurfaceDescriptor),
											    (geo::CBodyGroup&)m_pMesh->Volume(nVolume),
											    vcIndex2);
				  }
        }
        else // version >= 3.0.105
        {

          int nIsTiedSurface = 0;

          if (CStreamVersion(4, 1, 70) < version)
          {
            stream >> nIsTiedSurface;
          }

          // read face node indices
          int nFaceSize;
          stream >> nFaceSize;

          if(nSurfaceDescriptor < m_pMesh->NrOfSurfaces())
          {
            if (nIsTiedSurface)
            {
              geo::CTiedBodyTetSurface* pSurface = new geo::CTiedBodyTetSurface(const_cast<geo::CSurfaceDesc&>(m_pMesh->SurfaceDesc(nSurfaceDescriptor)),
                const_cast<geo::CBodyGroup&>(m_pMesh->Volume(nVolume)));

              pSurface->reserveFaceSize(nFaceSize);

              for (int n = 0; n < nFaceSize; ++n)
              {
                const geo::IFace* pFace = LoadTiedPointBodyTriangle(stream, version);
                pSurface->AddFace(*pFace);
              }
            }
            else
            {
              geo::CTetSurface* pSurface = new geo::CTetSurface(const_cast<geo::CSurfaceDesc&>(m_pMesh->SurfaceDesc(nSurfaceDescriptor)),
                const_cast<geo::CBodyGroup&>(m_pMesh->Volume(nVolume)));

              pSurface->reserveFaceSize(nFaceSize);

              for (int n = 0; n < nFaceSize; ++n)
              {
                const geo::IFace* pFace;
                if (version < CStreamVersion(4, 1, 13))
                {
                  // read the 3 node indices
                  int i;
                  int nNode;
                  std::set<int> stNodes;
                  for (i = 0; i < 3; ++i)
                  {
                    stream >> nNode;
                    stNodes.insert(nNode);
                  }
                  assert(stNodes.size() == 3);
                  TNodesToFaceMap::iterator it = mpNodesToFace.find(stNodes);
                  assert(it != mpNodesToFace.end());
                  pFace = it->second;
                }
                else
                {
                  int nBodyIndex;
                  int nFaceIndex;
                  stream >> nBodyIndex;
                  stream >> nFaceIndex;
                  assert(dynamic_cast<const geo::IBody*>(&m_pMesh->Element(nBodyIndex)));
                  pFace = &(static_cast<const geo::IBody&>(m_pMesh->Element(nBodyIndex))).Face(nFaceIndex);
                }

                pSurface->AddFace(*pFace);
              }
            }
          }
          else
          {
            // dummy read
            int nFactor = 2;
            if(version < CStreamVersion(4, 1, 13))
              nFactor = 3;

            int n;
            for(n = 0; n < nFaceSize * nFactor; ++n)
            {
              int nDummy;
              stream >> nDummy;
            }
          }
        }
			}
			
			assert(m_pMesh->Volume(nVolume).SideSurfaceSize() == nSurfaceSize);
		}
	}// End of 3.0.4 shit

	// Walk over formations. Fetch formation size
	int nFormationSize;
	stream >> nFormationSize;

	for(int nFormation = 0; nFormation < nFormationSize; nFormation++)
	{
		// Formation index ...
		int nFormationIndex;
		stream >> nFormationIndex;

		// Formation base
		CTetraFormationEntry& formation_entry = (CTetraFormationEntry&)*Model().GraphEntry(MD_TETRA_FORMATION);
		CTetraFormation* pFormation = formation_entry.FindIndex(nFormationIndex);
		assert(pFormation);

		// Creating and filling volumes
		int nVolumeSize;
		stream >> nVolumeSize;
		
		for(int nVolume = 0; nVolume < nVolumeSize; nVolume++)
		{
			if(CStreamVersion(3,0,3) < version)
			{
				// new in 3.0.4
				assert(nVolumeSize == pFormation->ElementSetSize());
				CTetraFormationVolume *pVolume = static_cast<CTetraFormationVolume*>(&pFormation->ElementSet(nVolume));
				
				// Read the index
				int nIndex;
				stream >> nIndex;
				if(nIndex > -1)
				{
					assert(nIndex < m_pMesh->NrOfVolumes());
					pVolume->Volume(m_pMesh->Volume(nIndex));
					if(pVolume->Identifier().size() == 0) pVolume->Identifier(VolumeIdentity(m_pMesh->Volume(nIndex)));
				}
			}
			else
			{
				// Creating a volume
				geo::CBodyGroup* pGroup = new geo::CBodyGroup(*m_pMesh);
				CTetraFormationVolume *pVolume = new CTetraFormationVolume(*pFormation, *pGroup);
				pVolume->reParent(pFormation);

				// Get nr of element
				int nElSize;
				stream >> nElSize;

				for(int nElement = 0; nElement < nElSize; nElement++)
				{
					// Get the element index from the mesh and add it to the volume
					int nElementIndex;
					stream >> nElementIndex;
					const geo::IElement& element = m_pMesh->Element(nElementIndex);
					const geo::IBody* pBody = static_cast<const geo::IBody*>(&element);
					pVolume->Volume().AddBody(*pBody);
				}
				pVolume->Identifier(VolumeIdentity(*pGroup));
			}
		}
	}
	// Load contact surface from 3.0.12. It is dummy for now ...
	if(CStreamVersion(3, 0, 12) < version && version < CStreamVersion(3, 0, 68))
	{
		int nIndex;
		stream >> nIndex;
		assert(nIndex == -1);	// Corrupt when not, but not delivered to customer
	}

	// Load the contact elements here and put them in the surface. 
	if(CStreamVersion(3, 0, 17) < version && version < CStreamVersion(3, 0, 68))
	{
		int nContactElementSize;
		stream >> nContactElementSize;

		for(int nElement = 0; nElement < nContactElementSize; nElement++)
		{
			// Load interface element
			int nSurfaceDescIndex, nBodyIndex, nPointSize;
			int bTarget;
			stream >> nSurfaceDescIndex;	// Surface descriptor index
			assert(m_pMesh->SurfaceDesc(nSurfaceDescIndex).Slip());
			stream >> bTarget;				// Target / Contact
			stream >> nBodyIndex;			// Body index
			stream >> nPointSize;
			assert( nPointSize == 3 );

			// Read points
			int points[3];
			for(int j = 0; j < nPointSize; j++)
				stream >> points[j];

			new geo::CTriangle( Mesh(), points[0], points[1], points[2] );

			InvalidateAtUnLock();

			progress.Step();
		}

	}

	// Loading interface for surfaces
	if(version > CStreamVersion(3, 0, 75)) {
	  int nDescIdx = -1;
		int nMaxToProcess = m_pMesh->NrOfSurfaces();
		if (nDuplicateSurfaces > 0)
			nMaxToProcess -= (nSuperSideSurface + nSubSideSurface);
		do {
  			stream >> nDescIdx;
			assert(nDescIdx < m_pMesh->NrOfSurfaces() + nDuplicateSurfaces);
			if(nDescIdx > -1) {
				if (version >= CStreamVersion(4, 1, 14) || nDescIdx < nMaxToProcess)
				{
					geo::CSurfaceDesc& desc = (geo::CSurfaceDesc&)m_pMesh->SurfaceDesc(nDescIdx);
					assert(desc.Slip());
					int nSize;
					stream >> nSize;
					for(int i = 0; i < nSize; i++) {
						int nElementIdx;
						stream >> nElementIdx;
						const geo::CInterfaceElement* pInterface = 
							dynamic_cast<const geo::CInterfaceElement*>(&m_pMesh->Element(nElementIdx));
						assert(pInterface);
						desc.AddInterfaceElement(const_cast<geo::CInterfaceElement&>(*pInterface));
					}
				}
				else // dummy read
				{
					int nSize;
					stream >> nSize;
					for(int i = 0; i < nSize; i++) {
						int nElementIdx;
						stream >> nElementIdx;
					}
				}
			}
	  } while( nDescIdx > -1 );
	}

	if(Mesh().ElementSize() == 0)	// Resolve inconsistancy of bug #1145
		m_pMesh->Clear();

	// from version 3.0.70 the tolerance of the tetmesher is saved
	if(version > CStreamVersion(3, 0, 69))
	{
		geo::CTetMeshBase &tmb = dynamic_cast<geo::CTetMeshBase&>(Mesh());
		double dTolerance;
		stream >> dTolerance;
		tmb.Tolerance(dTolerance);
	}

  if(version > CStreamVersion(3, 0, 95))
  {
    int iImportedMesh;
    stream >> iImportedMesh;
    m_bImportedMesh = (iImportedMesh != 0);
  }

  if (version > CStreamVersion(4, 1, 67))
    LoadTyings(stream, version, progress);

  if(version >= CStreamVersion(3, 7, 17))
    LoadValueSets(stream, version, progress);

	Modified(MESH_LOADED);
}

void CTetraMesh::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	CTetraModel *pModel = dynamic_cast<CTetraModel*> (&Model());
	assert(pModel);

  stream << CurrentMesherType();

	// Load base
	CMeshBase::SaveStream(stream,progress);
	progress.StatusMessage("Saving Mesh");
	
	// Save Mesh state
	stream << (int) m_bCanInvalidateMesh;

	// New in 3.0.4 !!!
	// We want to restore the input surfaces of the boundary. The load routine register the side surfaces
	// from the main boundary in the same order as in 'CreateMesh' so we don't have to save them. We only
	// need to save the surface's and the horizons in the m_vcSurfaceSource vector. There are only input
	// surfaces when the mesh is valid.
	if(IsMesh())
	{
		assert(m_pMesh->NrOfVolumes() > 0);
		assert(m_pMesh->NrOfSurfaces() > 0);
		assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());

		// Skip indices of the super boundary and if available the sub boundary.
		CTetraBoundary *pBoundary = dynamic_cast<CTetraBoundary*>(&pModel->Boundary());

		// m_vcSurfaceSource should be unique, but isn't always
		std::vector<COpenGLNode *> vcSurfaceSourceUnique;
		vcSurfaceSourceUnique.reserve(m_vcSurfaceSource.size());
		int nSuperSideSurface = 0;
		int nSubSideSurface = 0;
		for (int i = 0; i < m_vcSurfaceSource.size(); ++i)
		{
			if(m_vcSurfaceSource[i] == pBoundary)
			{
				if (nSuperSideSurface < pBoundary->SideSurfaceSize())
				{
					vcSurfaceSourceUnique.push_back(m_vcSurfaceSource[i]);
					++nSuperSideSurface;
				}
			}
			else if (m_vcSurfaceSource[i] == &pModel->SubBoundary())
			{
				if (nSubSideSurface < pModel->SubBoundary().SideSurfaceSize())
				{
					vcSurfaceSourceUnique.push_back(m_vcSurfaceSource[i]);
					++nSubSideSurface;
				}
			}
			else
			{
				bool bFound = false;
				for (int j = 0; j < vcSurfaceSourceUnique.size(); ++j)
				{
					if (vcSurfaceSourceUnique[j] != pBoundary && vcSurfaceSourceUnique[j] != &pModel->SubBoundary())
					{
						if (vcSurfaceSourceUnique[j]->Index() == m_vcSurfaceSource[i]->Index())
						{
							bFound = true;
							break;
						}
					}
				}
					
				if (!bFound)
				{
					vcSurfaceSourceUnique.push_back(m_vcSurfaceSource[i]);
				}
			}
		}
		if (vcSurfaceSourceUnique.size() != m_vcSurfaceSource.size())
		{
			m_vcSurfaceSource.clear();
			m_vcSurfaceSource.reserve(vcSurfaceSourceUnique.size());
			for (int i = 0; i < vcSurfaceSourceUnique.size(); ++i)
				m_vcSurfaceSource.push_back(vcSurfaceSourceUnique[i]);
			vcSurfaceSourceUnique.clear();
		}

		// Now we can actually determine the size
		int nSurfaceReferenceSize = m_vcSurfaceSource.size();
		assert(nSurfaceReferenceSize >= 0);
		stream << nSurfaceReferenceSize;
		// Walk over the vector save the indices of the opengl nodes
		for(int i = 0; i < m_vcSurfaceSource.size(); i++)
		{
			if(m_vcSurfaceSource[i] == pBoundary)
				stream << -1;						// Surface belongs to super boundary
			else if(m_vcSurfaceSource[i] == &pModel->SubBoundary())
				stream << -2;
			else
			{
				// Should be a surface or a horizons
				assert((dynamic_cast<CHorizonBase*>(m_vcSurfaceSource[i]) != 0) ||
					   (dynamic_cast<CSurfaceBase*>(m_vcSurfaceSource[i]) != 0));
				stream << m_vcSurfaceSource[i]->Index();
			}
		}
	}
	else
	{
		// There is no mesh so no input save a zero
		assert(m_pMesh->NrOfVolumes() == 0);
		assert(m_pMesh->NrOfSurfaces() == 0);
		assert(m_pMesh->NrOfSurfaces() == m_vcSurfaceSource.size());
		int nIndexSize = 0;
		stream << nIndexSize;
	}

	stream << m_pMesh->PointSize();
	for(int nPoint = 0; nPoint < m_pMesh->PointSize(); nPoint++)
	{
		stream << m_pMesh->Point(nPoint).X();
		stream << m_pMesh->Point(nPoint).Y();
		stream << m_pMesh->Point(nPoint).Z();
		progress.Step();
	}

	stream << m_pMesh->ElementSize();
	std::map<std::string, int>::iterator it_map;
	for(int nElement = 0; nElement < m_pMesh->ElementSize(); nElement++)
	{
		// first save the element type
		it_map = m_mpElementType.find(m_pMesh->Element(nElement).Type());
		assert(it_map != m_mpElementType.end());
		stream << it_map->second;

		assert(&*m_pMesh == m_pMesh->Element(nElement).IndexingElementSet());
		
		if(m_pMesh->Element(nElement).Type() == "T18IF")
			SaveInterfaceElement(stream, progress, m_pMesh->Element(nElement));
		else
		{
			// has to be a tetrahedron:
			assert(dynamic_cast<const geo::CTetrahedron*>(&m_pMesh->Element(nElement)));
			// save the element size
				stream << m_pMesh->Element(nElement).NrOfPoints();
			// save the connectivity
			for(int nPt = 0; nPt < m_pMesh->Element(nElement).NrOfPoints(); nPt++)
			{
				stream << m_pMesh->Element(nElement).PointIndex(nPt);
			}
			progress.Step();
		}
	}

	// Store the volumes the mesh and their corresponding surfaces
	stream << m_pMesh->NrOfVolumes();			// New in 3.0.4

	for(int nVolume = 0; nVolume < m_pMesh->NrOfVolumes(); nVolume++)
	{
		// Walk over elements of volume and store the index
		const geo::CBodyGroup& volume = m_pMesh->Volume(nVolume);
		stream << volume.ElementSize();
		for(int nElement = 0; nElement < volume.ElementSize(); nElement++)
		{
			const geo::IElement& element = volume.Element(nElement);
			assert(&*m_pMesh == element.IndexingElementSet());
			stream << element.Index();
			progress.Step();
		}

		// Walk over tet surfaces of volume
		stream << volume.SideSurfaceSize();
		for(int nSurface = 0; nSurface < volume.SideSurfaceSize(); nSurface++)
		{
			// Save the index of the corresponding surfaces descriptor and save
			// point indices of the surface
			const geo::CTetSurface& surface = dynamic_cast<const geo::CTetSurface&>(volume.SideSurface(nSurface));
			assert(&surface.BodyGroup() == &volume);

			// Search for index of surface descriptor and save it when found
			bool bFound = false;
			for(int i = 0; i < m_pMesh->NrOfSurfaces(); i++)
			{
				if(&m_pMesh->SurfaceDesc(i) == &surface.SurfaceDesc())
				{
					assert(!bFound);	// The id is unique
					stream << i;		// Save id
					bFound = true;
				}
			}

			assert(bFound);

      stream << (surface.isTiedSurface() ? 1 : 0);

      // save faces
      stream << surface.FaceSize();

      if (surface.isTiedSurface())
      {
        geo::CPoint point;
        int nOffset = 0;

        // each face is defined by its parent index and its local face index, and by 3 tyings to points in interface elements
        for (int n = 0; n < surface.FaceSize(); ++n)
        {
          SaveTiedBodyTriangle(surface.Face(n), stream);
        }
      }
      else
      {
        // each face is defined by its parent index and its local face index
        for (int n = 0; n < surface.FaceSize(); ++n)
        {
          assert(dynamic_cast<const geo::CBodyTriangle *>(&surface.Face(n)));

          const geo::CBodyTriangle& face = static_cast<const geo::CBodyTriangle&>(surface.Face(n));
          stream << face.Parent()->Index();
          stream << face.Index();
        }
      }
		}	
	}// End of 3.0.4 shit

	// Walk over the formation to save body info
	CTetraFormationEntry *pEntry = dynamic_cast<CTetraFormationEntry*> (Model().GraphEntry(MD_TETRA_FORMATION));
	assert(pEntry);
	CTetraFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
	// Write formation size
	int nFormationSize = stNode.size();
	stream << nFormationSize;

	// Walk over formations
	for(CTetraFormationEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
	{
		// Current formation
		CTetraFormation *pFormation = *it;
		
		// Write formation id
		stream << pFormation->Index();

		// For 3.0.4 we only write volume index for each volume
		stream << pFormation->VolumeSize();

		for(int nVolume = 0; nVolume < pFormation->VolumeSize(); nVolume++)
		{
			// Write element size
			CTetraFormationVolume* pVolume = dynamic_cast<CTetraFormationVolume*>(&pFormation->Volume(nVolume));
			assert(pVolume);

			int nIndex = -1;
			// Find the index
			for(int i = 0; i < m_pMesh->NrOfVolumes(); i++)
			{
				if(&pVolume->Volume() == &m_pMesh->Volume(i))
					nIndex = i;
			}
			
			// Write the index
			stream << nIndex;
		}
	}

	// We save the surface descriptor with interface elements here for slipping surfaces...
	for(int i = 0; i < m_pMesh->NrOfSurfaces(); i++)
	{
		const geo::CSurfaceDesc& desc = m_pMesh->SurfaceDesc(i);
		if(desc.Slip()) {
			stream << i; // Surface desc Id
			stream << desc.interfaceElements().ElementSize();
			for(int j = 0; j < desc.interfaceElements().ElementSize(); j++) {
				stream << desc.interfaceElements().Element(j).Index();
			}
		}
	}
	stream << -1; // Stop Id 

	// from version 3.0.70 the tolerance for the tetmesher is saved
	geo::CTetMeshBase &tmb = dynamic_cast<geo::CTetMeshBase&>(Mesh());
	stream << tmb.Tolerance();

  int iImportedMesh = m_bImportedMesh ? 1 : 0;
  stream << iImportedMesh;

  SaveTyings(stream, progress);

  SaveValueSets(stream, progress);
}

/*!
  Loads a body triangle from the stream.
  Returns 0 when the body triangle is not defined.
*/
const geo::IFace* CTetraMesh::LoadBodyTriangle(TSTREAM& stream, CStreamVersion& version)
{
	int nType = -1, nIndex, nTriIdx, nParentIdx;

  if (version > CStreamVersion(4, 1, 53))
    stream >> nType;

  if (nType == 1)
  {
    std::vector<int> nodes(3);
    std::vector<const geo::CBodyTriangle *> triangles(3);

    stream >> nIndex;

    for (int i = 0; i < 3; ++i)
    {
      stream >> nodes[i];
      stream >> nTriIdx;
      stream >> nParentIdx;

      const geo::IBody *pBody = dynamic_cast<const geo::IBody*>(&m_pMesh->Element(nParentIdx));
      assert(pBody);
      const geo::CBodyTriangle* pTriangle = dynamic_cast<const geo::CBodyTriangle*>(&pBody->Face(nTriIdx));
      assert(pTriangle);

      triangles[i] = pTriangle;
    }

    return new geo::CTiedBodyTriangle(Mesh(), nIndex, nodes, triangles);
  }
  else
  {
    stream >> nTriIdx;
    stream >> nParentIdx;

    if (nParentIdx == -1 || nTriIdx == -1) return 0;

    const geo::IBody *pBody = dynamic_cast<const geo::IBody*>(&m_pMesh->Element(nParentIdx));
    assert(pBody);
    const geo::CBodyTriangle* pTriangle = dynamic_cast<const geo::CBodyTriangle*>(&pBody->Face(nTriIdx));
    assert(pTriangle);
    return pTriangle;
  }
}

/*!
  Saves a body triangle. No body triangle is save when 0
*/
void CTetraMesh::SaveBodyTriangle( const geo::IFace* pTriangle, TSTREAM& stream )
{
  int nType = -1;
	int nTriIdx = -1;
	int nParentIdx = -1;	
	if(pTriangle) {
    if (dynamic_cast<const geo::CTiedBodyTriangle *>(pTriangle))
    {
      const geo::CTiedBodyTriangle *pTiedBodyTriangle = static_cast<const geo::CTiedBodyTriangle *>(pTriangle);

      nType = 1;
      stream << nType;
      stream << pTiedBodyTriangle->Index();

      for (int i = 0; i < 3; ++i)
      {
        stream << pTiedBodyTriangle->PointIndex(i);
        stream << pTiedBodyTriangle->BodyTriangle(i)->Index();
        stream << pTiedBodyTriangle->BodyTriangle(i)->Parent()->Index();
      }
    }
    else
    {
      nType = 0;
      nTriIdx = pTriangle->Index();
      nParentIdx = pTriangle->Parent()->Index();
    }
	}

  if (nType < 1)
  {
    stream << nType;
    stream << nTriIdx;
    stream << nParentIdx;
  }
}

const geo::IFace *CTetraMesh::LoadTiedPointBodyTriangle(TSTREAM& stream, CStreamVersion& version)
{
  int nParentIdx = -1, nTriIdx = -1, nOffset = 0;
  double x, y, z;
  geo::CPoint point;

  stream >> nParentIdx;
  stream >> nTriIdx;

  const geo::IBody *pBody = dynamic_cast<const geo::IBody*>(&m_pMesh->Element(nParentIdx));
  assert(pBody);
  const geo::CBodyTriangle* pTriangle = dynamic_cast<const geo::CBodyTriangle*>(&pBody->Face(nTriIdx));

  geo::CTiedPointBodyTriangle *tiedFace = new geo::CTiedPointBodyTriangle(const_cast<geo::IBody&>(*pBody), pTriangle->Index());

  for (int t = 0; t < 3; ++t)
  {
    int nIfElt = -1;

    stream >> nIfElt;

    if (nIfElt >= 0)
    {
      const geo::IInterfaceElement *ifElt = dynamic_cast<const geo::IInterfaceElement*>(&m_pMesh->Element(nIfElt));
      stream >> x;
      stream >> y;
      stream >> z;
      stream >> nOffset;

      point.Set(x, y, z);
      tiedFace->AddTying(t, *ifElt, point, nOffset);
    }
  }

  return tiedFace;
}

void CTetraMesh::SaveTiedBodyTriangle(const geo::IFace& face, TSTREAM & stream)
{
  geo::CPoint point;
  int nOffset;

  assert(dynamic_cast<const geo::CTiedPointBodyTriangle *>(&face));

  const geo::CTiedPointBodyTriangle& tiedFace = static_cast<const geo::CTiedPointBodyTriangle&>(face);
  stream << tiedFace.Parent()->Index();
  stream << tiedFace.Index();

  for (int t = 0; t < 3; ++t)
  {
    const geo::IInterfaceElement *ifElt = tiedFace.GetTying(t, point, nOffset);
    if (ifElt)
    {
      stream << ifElt->Index();
      stream << point.X();
      stream << point.Y();
      stream << point.Z();
      stream << nOffset;
    }
    else
    {
      stream << -1;
    }
  }
}

/*!
  Function loads boundary interface elements and fault interface elements
*/
void CTetraMesh::LoadInterfaceElement(TSTREAM &stream, CStreamVersion& version, TPROGRESS &progress)
{
	// Load adjacent faces
	const geo::IFace* pFront = LoadBodyTriangle( stream, version );
	const geo::IFace* pBack  = 0;
	if(version > CStreamVersion(3, 0, 75)) 
		pBack = LoadBodyTriangle( stream, version );

	// Load nodes
	int nTotalNodes;
	stream >> nTotalNodes;
	std::vector<int> vcNode(nTotalNodes);
	for(int i = 0; i < nTotalNodes; i++)
		stream >> vcNode[i];

	assert(pFront);

	geo::CInterfaceElement* pElement = new geo::CInterfaceElement(*m_pMesh, pFront, pBack, vcNode);
	if(pElement->BackFace() == pElement->FrontFace()) {
	  // Interface element belonging to tetra model
	  CTetraModel *pModel = dynamic_cast<CTetraModel *> (&Model());
	  assert(pModel != 0);
	  CTetraBoundary &boundary = dynamic_cast<CTetraBoundary &> (pModel->Boundary());
	  boundary.AddInterfaceElement(*pElement);
	}

	progress.Step();
}

void CTetraMesh::SaveInterfaceElement(TSTREAM &stream, TPROGRESS &progress, const geo::IElement &element)
{
	const geo::CInterfaceElement &interface_element = dynamic_cast<const geo::CInterfaceElement&>(element);
	SaveBodyTriangle( interface_element.FrontFace(), stream );
	SaveBodyTriangle( interface_element.BackFace(), stream );

  // TODO, perhaps some kind of validation whether an integer is appropriate?

	stream << (int) interface_element.NrOfNodes();
	// save the indices of the nodes
	for(int i = 0; i < interface_element.NrOfNodes(); i++)
		stream << interface_element.PointIndex(i);
	
	progress.Step();
}

void CTetraMesh::LoadContactElement(TSTREAM &stream, TPROGRESS &progress)
{
	// Contact elements are not supported anymore.
	// We make triangle to allow loading the results and simulate the contact elements.
	// After the loading the mesh in validated.
	int nSurfaceDescIndex, nBodyIndex, nPointSize;
	int bTarget;
	stream >> nSurfaceDescIndex;	// Surface descriptor index
	assert(m_pMesh->SurfaceDesc(nSurfaceDescIndex).Slip());
	stream >> bTarget;				// Target / Contact
	stream >> nBodyIndex;			// Body index
	stream >> nPointSize;			// NrOfPoints in face should be 3
	assert( nPointSize == 3 );

	// Read points
	int points[3];
	for(int j = 0; j < nPointSize; j++)
		stream >> points[j];

	new geo::CTriangle( Mesh(), points[0], points[1], points[2] );
	
	InvalidateAtUnLock();

	progress.Step();
}

long CTetraMesh::SavedItems() const
{
	long lRet = CMeshBase::SavedItems();

	lRet += m_pMesh->PointSize();
	lRet += m_pMesh->ElementSize();
  lRet += m_pMesh->TyingSize();

	for(int i = 0; i < m_pMesh->NrOfVolumes(); i++)
	{
		lRet += m_pMesh->Volume(i).ElementSize();

		for(int j = 0; j < m_pMesh->Volume(i).SideSurfaceSize(); j++)
		{
			const geo::CTetSurface *pSurface = dynamic_cast<const geo::CTetSurface*>(&m_pMesh->Volume(i).SideSurface(j));
			assert(pSurface);

      if (!pSurface->isTiedSurface())
			  lRet += pSurface->NrOfNodeIndices();
		}
	}

//	lRet += m_pMesh->ElementSize() - m_pMesh->BodySize();

	return lRet;
}

void CTetraMesh::InvalidateMesh()
{
	if((static_cast<const CModelBase&>(Model())).BranchState().IsBranch())
	{
		IPlatform* platform = IPlatform::instance();

		platform->trace("WARNING: CTetraMesh::InvalidateMesh called in branched state, ignoring call\n");
		return;
	}

	if(!CanInvalidateMesh())
    return;

  if(!Model().getDeleting() && m_bImportedMesh && _m()->msg(IDP_INVALIDATEIMPORTEDMESH, MB_YESNO) == IDNO)
    return;

	COperation operation(Model());

	if(Locked())
		return;

	CMeshBase::InvalidateMesh();
	m_pMesh->Clear();
	m_vcSurfaceSource.clear();
  m_bImportedMesh = false;

	// Walk over all formation and decouple volumes
	CTetraFormationEntry *pEntry = dynamic_cast<CTetraFormationEntry*> (Model().GraphEntry(MD_TETRA_FORMATION));
	if(pEntry)	// By the last invalidatation the entry is cleared in CTetraModel::CloseModel
	{
		CTetraFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
		for(CTetraFormationEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
		{
			CTetraFormation *pFormation = *it;
		//	for(int i = 0; i < pFormation->VolumeSize(); i++)
		//	{
	//			pFormation->Volume(i).Volume().Clear();
		//	}
			pFormation->Destroy();
			//pFormation->Modified();
		}
	}

	CTetraModel& model = (CTetraModel&)Model();
	if(&model.SubBoundary())
		model.SubBoundary().MeshInvalidated();

	Modified(MeshCleared);

}

// less class for two vectors, where the flipped ones are treated as equal (for support directions)
class CVectorLess
{
public:
	bool operator()(const geo::IVector &v1, const geo::IVector &v2) const
	{
		geo::CVector vFlip(v2);
		vFlip.Flip();
		if(vFlip == v1) return false;
		return v1 < v2;
	}
};



const geo::IElementSet &CTetraMesh::ElementSet() const
{
	return *m_pMesh;
}

geo::IElementSet &CTetraMesh::ElementSet()
{
	return *m_pMesh;
}

CTetraMesh::DIMENSION CTetraMesh::Dimension() const
{
	return DIM_3D;
}

unsigned int CTetraMesh::IconId() const
{
	return IDI_MESH;
}

unsigned int CTetraMesh::TypeId() const
{
	return 0;
}

int CTetraMesh::DisplayListSize() const
{
	return 1;
}

const geo::IObject& CTetraMesh::DisplayList(int /*nIndex*/) const
{
	return *m_pMesh;
}


const geo::IMesh &CTetraMesh::Mesh() const
{
	return *m_pMesh;
}

TGraphNodeSet CTetraMesh::VolumeIdentity(const geo::CBodyGroup& volume) const
{
	TGraphNodeSet stRet;
  // Get the index
	for(int nSurface = 0; nSurface < volume.SideSurfaceSize(); nSurface++)
	{
		const geo::CTetSurface *surface = dynamic_cast<const geo::CTetSurface *>(&volume.SideSurface(nSurface));

    if (!surface)
      continue;

		// Find the 
		for(int i = 0; i < m_pMesh->NrOfSurfaces(); i++)
		{
			if(&m_pMesh->SurfaceDesc(i) == &surface->SurfaceDesc())
			{
				stRet.insert(m_vcSurfaceSource[i]);
        break;
      }
		}
	}
	return stRet;
}

CTetraHorizonBase* CTetraMesh::Horizon(const CSurfaceBase& base)
{
	for(size_t i = 0; i < base.referenceSize(); i++)
	{
		const CTetraHorizonBase* pHorizon = dynamic_cast<const CTetraHorizonBase*>(&base.referenceAt(i));
		if(pHorizon)
			return const_cast<CTetraHorizonBase*>(pHorizon);
	}

	return 0;
}

geo::IMesh &CTetraMesh::Mesh()
{
	return *m_pMesh;
}

#ifdef SKUA_NEW
//const std::vector<const gm_skua::SKUAParseData *>

bool CTetraMesh::ImportMeshFromGoCadFile(const QString& sPath,
  bool(*selectGocadSolidDlg)(const std::vector<const gm_skua::SKUAParseData *>& solids,
    std::vector<int>& vcSolids))
{
  CGocadImport gi(true, true);

  bool res = false;

  bool res1 = gi.Import(sPath.toStdString().c_str());
  if (res1)
  {
    res = CreateMeshFromSolids(sPath, gi.getSolids(), selectGocadSolidDlg);
  }
  else
  {
    if (!gi.Error().empty())
      _m()->msg(gi.Error().c_str());
  }
  return res;
}

bool CTetraMesh::CreateMeshFromSolids(const QString& sPath,
  const std::vector<const gm_skua::SKUAParseData *>& solids,
  const std::vector<int>& vcSolidIndices)
{
  std::vector<const gm_skua::SKUAParseData *> vcSolids;

  if (solids.size() == 0)
  {
    QString msg;
    msg = QString("The file '%1' does not contain any TSolid objects").arg(sPath);
    _m()->msg(msg);
    return false;
  }

  for (int i = 0; i < vcSolidIndices.size(); ++i)
  {
    int nSolid = vcSolidIndices[i];
    assert(nSolid >= 0 && nSolid < solids.size());
    const gm_skua::SKUAParseData& tsolid = *solids[nSolid];

    if (tsolid.model_region_names.size() == 0)
    {
      QString msg;
      msg = QString("The %1 object '%2' does not contain any TVolume objects")
        .arg(tsolid.skua_type == gm_skua::SKUAParseData::LightTSolid ? "LightTSolid" : "TSolid")
        .arg(tsolid.name.data);
      _m()->msg(msg);
      return false;
    }
    vcSolids.push_back(&tsolid);
  }

  CGocadMeshImporter gmi(vcSolids, *this);

  Lock();
  m_bBuildingFromGocad = true;

  bool bSuccess = gmi.Import();

  m_bBuildingFromGocad = false;
  Unlock();

  assert(!bSuccess || IsMesh());

  if (!bSuccess)
  {
    InvalidateMesh();
  }

  Modified();

  m_bImportedMesh = true;


  return true;
}

bool CTetraMesh::CreateMeshFromSolids(const QString& sPath,
  const std::vector<const gm_skua::SKUAParseData *>& solids,
  bool(*selectGocadSolidDlg)(const std::vector<const gm_skua::SKUAParseData *>& solids,
    std::vector<int>& vcSolids))
{
  std::vector<int> vcSolids;
  vcSolids.reserve(solids.size());
  if (solids.size() > 1)
  {
    if (!selectGocadSolidDlg(solids, vcSolids))
      return false;
  }
  else if (solids.size() == 1) // the old single-solid case
  {
    vcSolids.push_back(0);
  }

  return CreateMeshFromSolids(sPath, solids, vcSolids);
}
#else
bool CTetraMesh::ImportMeshFromGoCadFile(	const QString& sPath,
											bool (*selectGocadSolidDlg)(	const std::vector <QSharedPointer <CGocadData::CTSolid> >& solids, 
																			std::vector<int>& vcSolids))
{
	CGocadImport gi(true, true);
	
	bool res = false;

	bool res1 = gi.Import( sPath.toStdString().c_str() );
	if( res1 )
	{
		res =  CreateMeshFromSolids(sPath, gi.getSolids(), selectGocadSolidDlg);
	}
	else
	{
		if(!gi.Error().empty())
			_m()->msg(gi.Error().c_str());
	}
	return res;
}

bool CTetraMesh::CreateMeshFromSolids(	const QString& sPath,
										std::vector <QSharedPointer <CGocadData::CTSolid> >& solids, 
										const std::vector<int>& vcSolidIndices)
{
	std::vector<const CGocadData::CTSolid*> vcSolids;

	if(solids.size() == 0)
	{
		QString msg;
		msg = QString("The file '%1' does not contain any TSolid objects").arg(sPath);
		_m()->msg(msg);
		return false;
	}

	for(int i = 0; i < vcSolidIndices.size(); ++i)
	{
		int nSolid = vcSolidIndices[i];
		assert(nSolid >= 0 && nSolid < solids.size());
		const CGocadData::CTSolid& tsolid = *solids[nSolid];

		if(tsolid.VolumeSize() == 0)
		{
			QString msg;
			msg = QString("The TSolid object '%1' does not contain any TVolume objects").arg(tsolid.Name());
			_m()->msg(msg);
			return false;
		}
		vcSolids.push_back(&tsolid);
	}

	CGocadMeshImporter gmi( vcSolids, *this) ;

  Lock();
  m_bBuildingFromGocad = true;

  bool bSuccess = gmi.Import();

  m_bBuildingFromGocad = false;
  Unlock();

  assert(!bSuccess || IsMesh());

  if(!bSuccess)
  {
    InvalidateMesh();
  }

  Modified();

  m_bImportedMesh = true;


  return true;
}

bool CTetraMesh::CreateMeshFromSolids(	const QString& sPath,
										std::vector <QSharedPointer <CGocadData::CTSolid> >& solids,
										bool (*selectGocadSolidDlg)(	const std::vector <QSharedPointer <CGocadData::CTSolid> >& solids,
																		std::vector<int>& vcSolids))
{
  std::vector<int> vcSolids;
  vcSolids.reserve(solids.size());
  if(solids.size() > 1)
  {
    if (!selectGocadSolidDlg(solids, vcSolids))
      return false;
  }
  else if (solids.size() == 1) // the old single-solid case
  {
    vcSolids.push_back(0);
  }

  return CreateMeshFromSolids(sPath, solids, vcSolids);
}
#endif

void CTetraMesh::GenerateSupportsForSurfaceDesc(const geo::CSurfaceDesc &surfacedesc, 
												const CPressureSupportNode &pnode, 
												int /*iRefNode*/,
                                                const geo::IVector& vecNormal, 
												const geo::IVector& vecTangent)
{
	assert(!surfacedesc.Slip()); // a boundary surface cannot slip

	for(int i = 0; i < surfacedesc.TetSurfaceSize(); i++)
	{
		const geo::CTetSurface &surface = surfacedesc.TetSurface(i);
		const CPressureSupportNode::CFaceSupportDef &facedef =
			pnode.FaceSupportDef(CPressureSupportNode::CSurfaceDef(surface));

		if(facedef.SupportType() != CPressureSupportNode::CFaceSupportDef::ST_STRESS)
		{
      int nNodes = surface.NrOfNodeIndices();
			for(int iNode = 0; iNode < nNodes; iNode++)
			{
        int iMeshNodeIndex = surface.NodeIndex(iNode);

				switch(facedef.SupportType())
				{
				case CPressureSupportNode::CFaceSupportDef::ST_NORMAL:
					{
						const geo::INode& node = Mesh().Node(iMeshNodeIndex);
						Mesh().CreateTranslationSupport(iMeshNodeIndex, surface.AverageNormalInPoint(node));
					}
					break;
				case CPressureSupportNode::CFaceSupportDef::ST_FULL:
					Mesh().CreateTranslationSupport(iMeshNodeIndex, vecNormal);
					Mesh().CreateTranslationSupport(iMeshNodeIndex, vecTangent);
        case CPressureSupportNode::CFaceSupportDef::ST_VERTICAL:
					Mesh().CreateTranslationSupport(iMeshNodeIndex, geo::CVector::Zaxis);
					break;
				case CPressureSupportNode::CFaceSupportDef::ST_HORIZONTAL:
					Mesh().CreateTranslationSupport(iMeshNodeIndex, vecNormal);
					Mesh().CreateTranslationSupport(iMeshNodeIndex, vecTangent);
					break;
				case CPressureSupportNode::CFaceSupportDef::ST_UNSUPPORTED:
					// no supports
					break;
				default:
					assert(false);
				}
			}
		}
	}
}

void CTetraMesh::GetModelOrientation(geo::IVector& vecNormal, geo::IVector& vecTangent)
{
	assert(IsMesh());
	const CTetraModel *pModel = dynamic_cast<const CTetraModel *> (&Model());
	assert(pModel != 0);

  // Quick hack to pass on of the direction one of the subboundary surfaces.
	assert(pModel->SubBoundary().SideSurfaceSize() > 0);
  for(int i = 0; i < pModel->SubBoundary().SideSurfaceSize(); ++i)
  {
	  vecNormal = pModel->SubBoundary().SideSurface(0).Surface().AverageNormal();
    if(vecNormal != geo::CVector::NullVector)
      break;
  }

  if(vecNormal == geo::CVector::NullVector)
    vecNormal = geo::CVector::Xaxis;

	vecNormal = vecNormal.UnitVector();
	vecTangent = vecNormal.CrossProduct( geo::CVector::Zaxis ).UnitVector();
}

void CTetraMesh::GenerateSupports()
{
	assert(IsMesh());
	const CTetraModel *pModel = dynamic_cast<const CTetraModel *> (&Model());
	assert(pModel != 0);
	const CTetraBoundary &boundary = dynamic_cast<const CTetraBoundary &> (pModel->Boundary());


	// first remove all existing supports in the mesh
	// we are going to create new ones
	geo::IMesh &mesh = Mesh();
	mesh.ClearSupports();

  geo::CVector vecNormal;
  geo::CVector vecTangent;

  GetModelOrientation(vecNormal, vecTangent);

	if(boundary.CreateInterfaces())
	{
		for(int i = 0; i < boundary.InterfaceNodeSize(); i++)
		{
			m_pMesh->CreateTranslationSupport(boundary.InterfaceNode(i).Index(), vecNormal);
			m_pMesh->CreateTranslationSupport(boundary.InterfaceNode(i).Index(), vecTangent);
			m_pMesh->CreateTranslationSupport(boundary.InterfaceNode(i).Index(), geo::CVector::Zaxis);

			//const geo::IElement &element = m_pMesh->Element(boundary.InterfaceElements()[i]);
			// we are interested in the outside points of the interface element, index 3, 4, 5
		/*	m_pMesh->CreateTranslationSupport(element.PointIndex(3), geo::CVector::Xaxis);
			m_pMesh->CreateTranslationSupport(element.PointIndex(3), geo::CVector::Yaxis);
			m_pMesh->CreateTranslationSupport(element.PointIndex(3), geo::CVector::Zaxis);

			m_pMesh->CreateTranslationSupport(element.PointIndex(4), geo::CVector::Xaxis);
			m_pMesh->CreateTranslationSupport(element.PointIndex(4), geo::CVector::Yaxis);
			m_pMesh->CreateTranslationSupport(element.PointIndex(4), geo::CVector::Zaxis);

			m_pMesh->CreateTranslationSupport(element.PointIndex(5), geo::CVector::Xaxis);
			m_pMesh->CreateTranslationSupport(element.PointIndex(5), geo::CVector::Yaxis);
			m_pMesh->CreateTranslationSupport(element.PointIndex(5), geo::CVector::Zaxis);*/
		}
			
		return;
	}

	const CPressureSupportNode &pnode = boundary.PressureSupportNode();


	// fully support model's reference point
	// find the reference point in mesh node terms
	int iRefNode = ReferencePointIndex();
  mesh.CreateTranslationSupport(iRefNode, vecNormal);
  mesh.CreateTranslationSupport(iRefNode, vecTangent);
	mesh.CreateTranslationSupport(iRefNode, geo::CVector::Zaxis);

	// side surfaces
  int i;
	for(i = 0; i < boundary.SideSurfaceSize(); i++)
	{
		const geo::CSurfaceDesc &surfdesc = boundary.SideSurfaceDesc(i);

		GenerateSupportsForSurfaceDesc(surfdesc, pnode, iRefNode, vecNormal, vecTangent);
	}

	// bottom surface
	const CTetraHorizonBase *pBottom = boundary.BottomHorizon();
	assert(pBottom != 0);

	for(i = 0; i < pBottom->OutputSurfaceSize(); i++)
	{
		const geo::CSurfaceDesc &surfdesc = pBottom->OutputSurface(i);
		GenerateSupportsForSurfaceDesc(surfdesc, pnode, iRefNode, vecNormal, vecTangent);
	}

	// top surface
	const CTetraHorizonBase *pTop = boundary.TopHorizon();
	assert(pBottom != 0);

	for(i = 0; i < pTop->OutputSurfaceSize(); i++)
	{
		const geo::CSurfaceDesc &surfdesc = pTop->OutputSurface(i);
		GenerateSupportsForSurfaceDesc(surfdesc, pnode, iRefNode, vecNormal, vecTangent);
	}
}

// get the 'fault' from a contact element
const CHorizonBase* CTetraMesh::SlipHorizon(const geo::IElement &interface_element) const
{
	// get all faults in the model
	CTetraSubHorizonEntry::TNodeSet stFaults = ((CTetraSubHorizonEntry*)(Model().GraphEntry(MD_TETRA_SUB_FAULT)))->EntryNodes();

//	const geo::CContactSurface &surface = contact.ContactSurface();

	// find the one that contains the contact surface
	for(CTetraSubHorizonEntry::TNodeSet::iterator it = stFaults.begin(); it != stFaults.end(); it++)
	{
		assert((*it)->Slip());
		for(int i = 0; i < (*it)->OutputSurfaceSize(); i++)
		{
			const geo::CSurfaceDesc &surfdesc = (*it)->OutputSurface(i);
			assert(surfdesc.Slip());
			
			for(int j = 0; j < surfdesc.interfaceElements().ElementSize(); j++) {
				if( &interface_element == &surfdesc.interfaceElements().Element(j)) return *it;
			}
		}
	}

	return 0;
}

const double& CTetraMesh::MaxGocadSurfaceAngle() const
{
  return m_dMaxGocadSurfaceAngle;
}

void CTetraMesh::MaxGocadSurfaceAngle(const double& dValue)
{
  m_dMaxGocadSurfaceAngle = dValue;
}

bool CTetraMesh::ImportedMesh() const
{
  return m_bImportedMesh;
}

bool CTetraMesh::OnHorizonSlipChanged(CTetraSubHorizon& hor)
{
  assert(ImportedMesh());

  // get the surface
  assert(hor.SurfaceSize() == 1);
  const geo::ISurface& surface = hor.Surface(0).Surface();

  // get the surface descriptor
  geo::CSurfaceDesc* pSurfDesc = 0;
  int i;
  for(i = 0; i < m_pMesh->NrOfSurfaces(); ++i)
  {
    if(&m_pMesh->SurfaceDesc(i).Surface() == &surface)
      pSurfDesc = const_cast<geo::CSurfaceDesc*>(&m_pMesh->SurfaceDesc(i));
  }

  assert(pSurfDesc != 0);

  if(hor.Slip())
  {
    // changed from horizon to fault
    if(!SplitSurfaceNodes(*pSurfDesc))
      return false;
  }
  else
  {
    // changed from fault to horizon
    if(!CombineSurfaceNodes(*pSurfDesc))
      return false;
  }

  hor.Modified();

	CModelBase& model = static_cast<CModelBase&>(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();

  return true;
}

bool CTetraMesh::SplitSurfaceNodes(geo::CSurfaceDesc& surfdesc)
{
  surfdesc.Slip(true);

  std::set<const geo::CTetSurface*> stFrontSurfaces;
  std::set<const geo::CTetSurface*> stBackSurfaces;

  std::set<const geo::CBodyGroup*> stFrontGroups;
  std::set<const geo::CBodyGroup*> stBackGroups;

  if(!GetFrontAndBack(surfdesc, stFrontGroups, stBackGroups, stFrontSurfaces, stBackSurfaces))
    return false;

  // register all faces
//  typedef CGocadMeshImporter::TPositionFacesMap TPositionFacesMap;
  typedef CGocadMeshImporter::CPositionTriangle TPositionTriangle;
//  typedef CGocadMeshImporter::TFaceSet TFaceSet;
  typedef std::set<const geo::IFace*> TFaceSet;
  typedef std::map<TPositionTriangle, TFaceSet> TPositionFacesMap;

  TPositionFacesMap mpPositionFaces;
  std::set<const geo::CTetSurface*>::iterator its;
  for(its = stFrontSurfaces.begin(); its != stFrontSurfaces.end(); ++its)
  {
    int i;
    for(i = 0; i < (*its)->FaceSize(); ++i)
    {
      const geo::IFace& face = (*its)->Face(i);
      TPositionFacesMap::iterator it = mpPositionFaces.insert(TPositionFacesMap::value_type(TPositionTriangle(face), TFaceSet())).first;
      it->second.insert(&face);
    }
  }

  for(its = stBackSurfaces.begin(); its != stBackSurfaces.end(); ++its)
  {
    int i;
    for(i = 0; i < (*its)->FaceSize(); ++i)
    {
      const geo::IFace& face = (*its)->Face(i);
      TPositionFacesMap::iterator it = mpPositionFaces.insert(TPositionFacesMap::value_type(TPositionTriangle(face), TFaceSet())).first;
      it->second.insert(&face);
    }
  }

  // disconnect the back groups
  std::set<const geo::CBodyGroup*>::iterator itg;
  for(itg = stBackGroups.begin(); itg != stBackGroups.end(); ++itg)
  {
    geo::CBodyGroup& bodygroup = const_cast<geo::CBodyGroup&>(**itg);
    int iss;
    for(iss = 0; iss < bodygroup.SideSurfaceSize(); ++iss)
    {
      geo::CTetSurface& sidesurface = static_cast<geo::CTetSurface&>(bodygroup.SideSurface(iss));
      if(stBackSurfaces.find(&sidesurface) != stBackSurfaces.end())
      {
        typedef std::map<int, int> TOppositeNodesMap;
        TOppositeNodesMap mpOppositeNodes;

        int n;
        for(n = 0; n < sidesurface.PointSize(); ++n)
        {
          const geo::IPoint& pt = sidesurface.Point(n);
          assert(dynamic_cast<const geo::INode*>(&pt) != 0);
          const geo::INode& node = static_cast<const geo::INode&>(pt);

          int nOldNodeIndex = node.Index();

          // register the same point as a new node
          int nNewNodeIndex = m_pMesh->RegisterNode(pt, false);
          IPlatform* platform = IPlatform::instance();
          QString traceString = QString("Created node %1 for node %2\n").arg(nNewNodeIndex).arg(nOldNodeIndex);

          platform->trace(traceString);

          mpOppositeNodes.insert(TOppositeNodesMap::value_type(nNewNodeIndex, nOldNodeIndex));

          // get the attached bodies and modify them
          // make a deep copy of the vector, so it's not modified through the CBodyGroup::ReplaceNode call below
          std::vector<const geo::IBody*> vcBodies = bodygroup.PointToBody(pt);

          int j;
          for(j = 0; j < vcBodies.size(); ++j)
          {
            assert(dynamic_cast<const geo::CTetrahedron*>(vcBodies[j]) != 0);
            geo::CTetrahedron& tet = const_cast<geo::CTetrahedron&>(static_cast<const geo::CTetrahedron&>(*vcBodies[j]));

            bool bFound = false;

            int l;
            for(l = 0; l < tet.NrOfNodes(); ++l)
            {
              if(&tet.Node(l) == &pt)
              {
                // replace it
                tet.ReplaceNode(l, nNewNodeIndex);

                // inform bodygroup
                bodygroup.ReplaceNode(tet, nOldNodeIndex, nNewNodeIndex);

                bFound = true;
              }
            }

            assert(bFound);
          }
        }

        // create interface elements
        for(n = 0; n < sidesurface.FaceSize(); ++n)
        {
          const geo::IFace& face = sidesurface.Face(n);
          assert(face.NrOfNodes() == 3);
          std::vector<int> vcPoint(6);
          int i;
          for(i = 0; i < 3; ++i)
          {
            vcPoint[i] = (face.Node(i).Index());
            TOppositeNodesMap::iterator it = mpOppositeNodes.find(face.Node(i).Index());
            assert(it != mpOppositeNodes.end());
            vcPoint[i + 3] = it->second;
          }

          TPositionFacesMap::iterator it = mpPositionFaces.find(TPositionTriangle(face));
          assert(it != mpPositionFaces.end());

          const TFaceSet& stFaces = it->second;
          assert(stFaces.size() == 2);

          TFaceSet::const_iterator itf = stFaces.begin();
          if(&face == *itf)
            ++itf;

          assert(&face != *itf);

          geo::CInterfaceElement* pIf = new geo::CInterfaceElement(*m_pMesh, &face, *itf, vcPoint);
          surfdesc.AddInterfaceElement(*pIf);
        }
      }
    }
  }

  Modified();
  return true;
}

bool CTetraMesh::CombineSurfaceNodes(geo::CSurfaceDesc& surfdesc)
{

#if 1 // disabled since removing nodes and elements is not properly supported by the geometry mesh structure

  IPlatform* platform = IPlatform::instance();

  // delete the interface elements
  std::set<int> stElementsToRemove;

  for(int i = 0; i < surfdesc.interfaceElements().ElementSize(); ++i)
    stElementsToRemove.insert(surfdesc.interfaceElements().Element(i).Index());

  // walk in reverse order to make sure indexes are not invalidated
  std::set<int>::reverse_iterator rit;
  for(rit = stElementsToRemove.rbegin(); rit != stElementsToRemove.rend(); ++rit)
  {
    const geo::IElement* pElement = &m_pMesh->Element(*rit);
    m_pMesh->RemoveElement(*rit);
    delete pElement;
  }

  surfdesc.Slip(false);

  std::set<const geo::CBodyGroup*> stFrontGroups;
  std::set<const geo::CBodyGroup*> stBackGroups;
  std::set<const geo::CTetSurface*> stFrontSurfaces;
  std::set<const geo::CTetSurface*> stBackSurfaces;

  if(!GetFrontAndBack(surfdesc, stFrontGroups, stBackGroups, stFrontSurfaces, stBackSurfaces))
    return false;

  // register all faces
  typedef CGocadMeshImporter::TPositionFacesMap TPositionFacesMap;
  typedef CGocadMeshImporter::CPositionTriangle TPositionTriangle;
  typedef CGocadMeshImporter::TFaceBodyPairSet TFaceBodyPairSet;
  typedef std::map<geo::CPoint, const geo::INode*> TPositionNodeMap;

  TPositionNodeMap mpFrontNodePositions;
  TPositionFacesMap mpPositionFaces;
  std::set<const geo::CTetSurface*>::iterator its;
  for(its = stFrontSurfaces.begin(); its != stFrontSurfaces.end(); ++its)
  {
    for(int i = 0; i < (*its)->FaceSize(); ++i)
    {
      const geo::IFace& face = (*its)->Face(i);
      TPositionFacesMap::iterator it = mpPositionFaces.insert(TPositionFacesMap::value_type(TPositionTriangle(face), TFaceBodyPairSet())).first;
      it->second.insert(std::make_pair(&face, (const geo::CBodyGroup*)0));
    }

    for(int i = 0; i < (*its)->PointSize(); ++i)
    {
      assert(dynamic_cast<const geo::INode*>(&(*its)->Point(i)) != 0);
      const geo::INode& node = static_cast<const geo::INode&>((*its)->Point(i));
      mpFrontNodePositions.insert(TPositionNodeMap::value_type(node, &node));
    }
  }

  for(its = stBackSurfaces.begin(); its != stBackSurfaces.end(); ++its)
  {
    for(int i = 0; i < (*its)->FaceSize(); ++i)
    {
      const geo::IFace& face = (*its)->Face(i);
      TPositionFacesMap::iterator it = mpPositionFaces.insert(TPositionFacesMap::value_type(TPositionTriangle(face), TFaceBodyPairSet())).first;
      it->second.insert(std::make_pair(&face, (const geo::CBodyGroup*)0));
    }
  }

  // connect the back groups

  std::set<int> stNodesToRemove;

  // collect the side surfaces that need to be recreated
  typedef std::map<geo::CTetSurface*, std::vector<int> > TSideSurfaceMap;
  TSideSurfaceMap mpRecreateSurfaces;

  std::set<const geo::CBodyGroup*>::iterator itg;
  for(itg = stBackGroups.begin(); itg != stBackGroups.end(); ++itg)
  {
    geo::CBodyGroup& bodygroup = const_cast<geo::CBodyGroup&>(**itg);
    int i;
    for(i = 0; i < bodygroup.SideSurfaceSize(); ++i)
    {
      geo::CTetSurface& sidesurface = static_cast<geo::CTetSurface&>(bodygroup.SideSurface(i));

      // is this one of the side surfaces that need modification?
      if(stBackSurfaces.find(&sidesurface) != stBackSurfaces.end())
      {
        // register it for recreation
        std::pair<TSideSurfaceMap::iterator, bool> prInsert = mpRecreateSurfaces.insert(TSideSurfaceMap::value_type(&sidesurface, std::vector<int>()));
        assert(prInsert.second);
        std::vector<int>& vcNodeIndices = prInsert.first->second;
        vcNodeIndices.resize(sidesurface.PointSize());

#ifdef _DEBUG
        std::set<int> stDbgNodes;
        int iDbg;
        for(iDbg = 0; iDbg < sidesurface.PointSize(); ++iDbg)
        {
          const geo::INode& node = static_cast<const geo::INode&>(sidesurface.Point(iDbg));
          platform->trace(QString("Insert node %1\n").arg(node.Index()));
          stDbgNodes.insert(node.Index());
        }

        for(iDbg = 0; iDbg < sidesurface.FaceSize(); ++iDbg)
        {
          int i;
          for(i = 0; i < sidesurface.Face(iDbg).NrOfNodes(); ++i)
          {
            platform->trace(QString("Looking for node %1\n").arg(sidesurface.Face(iDbg).Node(i).Index()));
            assert(stDbgNodes.find(sidesurface.Face(iDbg).Node(i).Index()) != stDbgNodes.end());
          }
        }
#endif

        int n;
        for(n = 0; n < sidesurface.PointSize(); ++n)
        {
          const geo::IPoint& pt = sidesurface.Point(n);
          assert(dynamic_cast<const geo::INode*>(&pt) != 0);
          const geo::INode& node = static_cast<const geo::INode&>(pt);

          TPositionNodeMap::iterator itn = mpFrontNodePositions.find(pt);
          assert(itn != mpFrontNodePositions.end());
          const geo::INode& frontnode = *itn->second;

          // keep the new node's index
          vcNodeIndices[n] = frontnode.Index();

          // get the attached bodies and modify them
          // make a deep copy so the CBodyGroup::ReplaceNode call doesn't modify the vector's contents
          std::vector<const geo::IBody*> vcBodies = bodygroup.PointToBody(pt);
          platform->trace(QString("Found %1 bodies attached to node %2\n").arg(vcBodies.size()).arg(node.Index()));
          int j;
          for(j = 0; j < vcBodies.size(); ++j)
          {
            assert(dynamic_cast<const geo::CTetrahedron*>(vcBodies[j]) != 0);
            geo::CTetrahedron& tet = const_cast<geo::CTetrahedron&>(static_cast<const geo::CTetrahedron&>(*vcBodies[j]));

            bool bFound = false;
            int l;
            for(l = 0; l < tet.NrOfNodes(); ++l)
            {
              if(&tet.Node(l) == &pt)
              {
                int nOldIndex = tet.Node(l).Index();
                int nNewIndex = frontnode.Index();

                platform->trace(QString("Replace node %1 by %2 (remove %3)\n").arg(nOldIndex).arg(nNewIndex).arg(nOldIndex));

                // store old node for removal
                stNodesToRemove.insert(nOldIndex);

                // replace it
                tet.ReplaceNode(l, nNewIndex);

                // inform the bodygroup
                bodygroup.ReplaceNode(tet, nOldIndex, nNewIndex);

                bFound = true;
              }
            }

            assert(bFound);
          }
        }
      }
    }
  }
/*
  // recreate modified side surfaces
  TSideSurfaceMap::iterator itss;
  for(itss = mpRecreateSurfaces.begin(); itss != mpRecreateSurfaces.end(); ++itss)
  {
    geo::CBodyGroup& bodygroup = const_cast<geo::CBodyGroup&>(itss->first->BodyGroup());
    geo::CSurfaceDesc& surfdesc = const_cast<geo::CSurfaceDesc&>(itss->first->SurfaceDesc());
    const std::vector<int>& vcNode = itss->second;

    delete itss->first;
    geo::CTetSurface* pNewSurface = new geo::CTetSurface(surfdesc, bodygroup);
    pNewSurface->CreateSurface(vcNode);
  }
*/
  // remove loose nodes
  // walk in reverse order to make sure indexes are not invalidated
  for(rit = stNodesToRemove.rbegin(); rit != stNodesToRemove.rend(); ++rit)
  {
    platform->trace(QString("RemoveNode(%1), NrOfNodes = %2\n").arg(*rit).arg(m_pMesh->NodeSize()));
    m_pMesh->RemoveNode(*rit);
  }

  return true;

#else

  assert(false);
  return false;

#endif

}

bool CTetraMesh::GetFrontAndBack(const geo::CSurfaceDesc& surfdesc,
                                 std::set<const geo::CBodyGroup*>& stFrontGroups,
                                 std::set<const geo::CBodyGroup*>& stBackGroups,
                                 std::set<const geo::CTetSurface*>& stFrontSurfaces,
                                 std::set<const geo::CTetSurface*>& stBackSurfaces) const
{
  if(surfdesc.TetSurfaceSize() == 0)
    return true; // nothing to be done ??

  std::set<const geo::CBodyGroup::CSideSurface*> stFaultSurfaces;
  std::set<const geo::CBodyGroup*> stAdjacentBodyGroups;

  for(int i = 0; i < surfdesc.TetSurfaceSize(); ++i)
  {
    stFaultSurfaces.insert(&surfdesc.TetSurface(i));
    stAdjacentBodyGroups.insert(&surfdesc.TetSurface(i).BodyGroup());
  }

  // get the first bodygroup
  const geo::CBodyGroup& bodygroup = surfdesc.TetSurface(0).BodyGroup();

  // assign it to the front (arbitrary) and start the process of 'coloring'
  stFrontGroups.insert(&bodygroup);
  SeparateBodyGroups(bodygroup, stAdjacentBodyGroups, stFrontGroups, stBackGroups, stFaultSurfaces);

  // isolated edges are not allowed in horizons
  if(stFrontGroups.empty() || stBackGroups.empty())
    return false;

  assert(stFrontGroups.size() + stBackGroups.size() == stAdjacentBodyGroups.size());

  std::set<const geo::CBodyGroup*>::iterator it;
  for(it = stFrontGroups.begin(); it != stFrontGroups.end(); ++it)
  {
    for(int i = 0; i < (*it)->SideSurfaceSize(); ++i)
    {
      const geo::CTetSurface& tetsurface = static_cast<const geo::CTetSurface&>((*it)->SideSurface(i));
      if(stFaultSurfaces.find(&tetsurface) != stFaultSurfaces.end())
        stFrontSurfaces.insert(&tetsurface);
    }
  }
  for(it = stBackGroups.begin(); it != stBackGroups.end(); ++it)
  {
    for(int i = 0; i < (*it)->SideSurfaceSize(); ++i)
    {
      const geo::CTetSurface& tetsurface = static_cast<const geo::CTetSurface&>((*it)->SideSurface(i));
      if(stFaultSurfaces.find(&tetsurface) != stFaultSurfaces.end())
        stBackSurfaces.insert(&tetsurface);
    }
  }

  assert(!stFrontSurfaces.empty() && !stBackSurfaces.empty());
  assert(stFrontSurfaces.size() + stBackSurfaces.size() == stFaultSurfaces.size());

  return true;
}

/*
 *  Apparently it is necessary to order the surfaces of any body group.
 *  Any surface that is contained in the fault surfaces should be ranked first.
 *  When this ordering has not taken place, surfaces can be wrongly assigned to
 *  either the front or the back.
 *  The following example (in 2D) should make this issue more visual:
 *
 *  -------------
 *  |     1     |
 *  |-hor-ofaulto
 *  |     2     |
 *  -------------
 *
 *  If the body groups are assigned to back and front groups by using
 *  the horizon ('hor'), both groups will be assigned to the front.
 *  If the body groups are assigned to back and front groups by using
 *  the fault ('fault'), the first group will be assigned to the back and
 *  the second to the front.
 *  The assignment will proceed in an orderly fashion
 *  by ordering the surfaces with the fault surfaces ranked first.
 */

namespace
{

std::vector <const geo::CBodyGroup::CSideSurface*>
  orderSurfaces(const geo::CBodyGroup& bodyGroup,
    const std::set <const geo::CBodyGroup::CSideSurface*>& faultSurfaces)
{
  std::vector <const geo::CBodyGroup::CSideSurface*> sideSurfaces;

  for (int i = 0; i < bodyGroup.SideSurfaceSize(); ++i)
  {
    sideSurfaces.push_back(&bodyGroup.SideSurface(i));

    if ((sideSurfaces.size() > 1) &&
      (faultSurfaces.find(sideSurfaces.back()) != faultSurfaces.end()))
    {
      std::swap(sideSurfaces.front(), sideSurfaces.back());
    }
  }

  return sideSurfaces;
}

} // anonymous namespace

void CTetraMesh::SeparateBodyGroups(const geo::CBodyGroup& currentgroup,
                                    const std::set<const geo::CBodyGroup*>& stAdjacentBodyGroups,
                                    std::set<const geo::CBodyGroup*>& stFrontGroups,
                                    std::set<const geo::CBodyGroup*>& stBackGroups,
                                    const std::set<const geo::CBodyGroup::CSideSurface*>& stFaultSurfaces) const
{
  // currentgroup has already been inserted
  assert(stFrontGroups.find(&currentgroup) != stFrontGroups.end() || stBackGroups.find(&currentgroup) != stBackGroups.end());

  bool bCurrentIsFront = (stFrontGroups.find(&currentgroup) != stFrontGroups.end());

  // currentgroup must be part of the adjacent body groups
  assert(stAdjacentBodyGroups.find(&currentgroup) != stAdjacentBodyGroups.end());

  std::vector <const geo::CBodyGroup::CSideSurface*> sideSurfaces =
    orderSurfaces(currentgroup, stFaultSurfaces);

  int i;
  for(i = 0; i < sideSurfaces.size(); ++i)
  {
    const geo::CBodyGroup::CSideSurface& sidesurface = *sideSurfaces[i];
    const geo::CTetSurface& tetsurface =
#ifdef _DEBUG
      dynamic_cast<const geo::CTetSurface&>
#else
      static_cast<const geo::CTetSurface&>
#endif
                                            (sidesurface);

    const geo::CSurfaceDesc& surfacedesc = tetsurface.SurfaceDesc();

    int j;
    for(j = 0; j < surfacedesc.TetSurfaceSize(); ++j)
    {
      const geo::CTetSurface& tsurf = surfacedesc.TetSurface(j);
      if(&tsurf != &tetsurface)
      {
        const geo::CBodyGroup& nextgroup = tsurf.BodyGroup();

        // only process if part of the fault's adjacent body groups and if not processed yet
        if(stAdjacentBodyGroups.find(&nextgroup) != stAdjacentBodyGroups.end() &&
           stFrontGroups.find(&nextgroup) == stFrontGroups.end()               &&
           stBackGroups.find(&nextgroup) == stBackGroups.end())
        {
          // are current and next connected via a fault surface?
          bool bConnectedThroughFault = (stFaultSurfaces.find(&tsurf) != stFaultSurfaces.end());

          if(bCurrentIsFront == bConnectedThroughFault)
            stBackGroups.insert(&nextgroup);
          else
            stFrontGroups.insert(&nextgroup);

          // recurse
          SeparateBodyGroups(nextgroup, stAdjacentBodyGroups, stFrontGroups, stBackGroups, stFaultSurfaces);
        }
      }
    }
  }
}

namespace
{

int direction(geo::CVector rsNormal[], const geo::CVector& v)
{
  for (int i = 0; i < 3; ++i)
    if (rsNormal[i] == v)
      return i;
  assert(false);
  return -1;
}

}

void CTetraMesh::SaveTyings(TSTREAM& stream, TPROGRESS& progress)
{
  int nTyings = m_pMesh->TyingSize();

  stream << nTyings;

  CModelBase& model = static_cast<CModelBase&>(Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());

  geo::CVector rsNormal[3];
  rsNormal[0] = rs.ToGlobal(geo::CVector::Xaxis);
  rsNormal[1] = rs.ToGlobal(geo::CVector::Yaxis);
  rsNormal[2] = rs.ToGlobal(geo::CVector::Zaxis); // should be the same

  for (int i = 0; i < nTyings; ++i)
  {
    const geo::CTying& tying = m_pMesh->Tying(i);

    stream << tying.SlaveIndex();
    stream << direction(rsNormal, tying.SlaveDirection());
    stream << tying.SlaveType();

    int nMaster = tying.MasterSize();

    stream << nMaster;

    for (int j = 0; j < nMaster; ++j)
    {
      stream << tying.MasterIndex(j);
      stream << direction(rsNormal, tying.MasterDirection(j));
      stream << tying.MasterType(j);
      stream << tying.MasterFactor(j);
    }
  }
}

void CTetraMesh::LoadTyings(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  int nTyings;
  stream >> nTyings;

  // We already loaded the boundary information, it is safe to use it here
  CModelBase& model = static_cast<CModelBase&>(Model());
  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());

  geo::CVector rsNormal[3];
  rsNormal[0] = rs.ToGlobal(geo::CVector::Xaxis);
  rsNormal[1] = rs.ToGlobal(geo::CVector::Yaxis);
  rsNormal[2] = rs.ToGlobal(geo::CVector::Zaxis); // should be the same

  std::vector<geo::CTying::_MasterDef> vcMasters(3);

  for (int i = 0; i < nTyings; ++i)
  {
    int nSlaveIndex, nSlaveDirection, nSlaveType, nMaster;

    stream >> nSlaveIndex;
    stream >> nSlaveDirection;
    stream >> nSlaveType;
    stream >> nMaster;

    vcMasters.resize(nMaster);

    for (int j = 0; j < nMaster; ++j)
    {
      int nMasterIndex, nMasterDirection, nMasterType;
      double dMasterFactor;

      stream >> nMasterIndex;
      stream >> nMasterDirection;
      stream >> nMasterType;
      stream >> dMasterFactor;

      vcMasters[j].node = nMasterIndex;
      vcMasters[j].direction = &rsNormal[nMasterDirection];
      vcMasters[j].type = (geo::CTying::TType)nMasterType;
      vcMasters[j].factor = dMasterFactor;
    }

    Mesh().CreateTying(nSlaveIndex, rsNormal[nSlaveDirection], (geo::CTying::TType)nSlaveType, vcMasters);
  }
}
