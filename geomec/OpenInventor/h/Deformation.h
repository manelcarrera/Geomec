#pragma once

#include <MeshVizXLM/MbVec3.h>

#include <math.h> // sqrt (class Algebra)
#include "OIDIDataSetI.h"

#include "IDeformation.h" // -> Invalidate()

class OIDIGeometryI;
class OIDISet;
class OIDIMesh;

class Deformation : public IDeformation
{
public:
	enum eStatus{ Valid, Invalid };

	enum eFormationType{ Default_, Cement /*Casing3DFormation*/, CementInterface, Steel, All, Invalid_ };

	struct Data
	{
		struct Input
		{
			struct TScale
			{ 
				double gen; 
				double def; 

				bool operator==(const TScale& other){ return (gen == other.gen && def == other.def); };
				bool operator!=(const TScale& other) { return !(*this == other); };
			};

			int stage;
			eFormationType type;
			TScale scale;
			bool active;

			bool operator==(const Input& rhs){ return (stage == rhs.stage && /*type == rhs.type &&*/ scale == rhs.scale && active == rhs.active ); };
			bool operator!=(const Input& rhs) { return !(*this == rhs); };
		};

		struct Result
		{
			struct Info
			{
				std::vector<MbVec3d> deformation;
				OIDIGeometryI* geo;
				OIDIMesh* mesh;
				std::string name;
			};

			struct Geo
			{
				struct TMinMax{ MbVec3d min; MbVec3d max; };

				std::vector<MbVec3d> points;
				TMinMax min_max;
			};

			Info info;
			Geo geo;
		};

		typedef std::vector<Result> TResultsV;

		Input input;
		TResultsV results_v;

		bool operator==(const Data& rhs){ return (input == rhs.input); };
		bool operator!=(const Data& rhs) { return !(*this == rhs); };
	};

	static const Data::Input INVALID_INPUT;
	static const Data::Result::Info INVALID_INFO;
	static const Data::Result::Geo INVALID_GEO;
	static const Data::Result INVALID_RESULT;
	static const Data INVALID;
	

private:

	Data m_data;

private:

	eFormationType formation_type( COpenGLNode* node );
	eFormationType formation_type( OIDIMesh *mesh );
	bool is( COpenGLNode* node, eFormationType _type  );
	bool is( OIDIMesh *mesh, eFormationType _type );
	const IVectorResult::CFullVectorComponent* displacement_component( int stage_idx );
	OIDIMesh* mesh( eFormationType _type, OIDISet& meshSet );
	std::vector<MbVec3d> get( OIDIMesh *mesh, const IVectorResult::CFullVectorComponent* component );
	std::vector<MbVec3d> get_FAKE( OIDIMesh *mesh );
	Data& current(){ return m_data; };
	void add_result( OIDIMesh *mesh, const IVectorResult::CFullVectorComponent* component, Data::Input::TScale scale );
	const Data::Result& result( int i );

public:
	Deformation();
	Deformation::eStatus get( Data::Input input, OIDISet& meshes, bool refresh = false );
	bool apply();
	Deformation::eStatus status();
	Deformation::eStatus refresh( OIDISet& meshes );
	int num_meshes();
	void invalidate_soft();
	const Deformation::Data::Input& input(){ return m_data.input; };
  const Deformation::Data::Result& result(size_t i) { return m_data.results_v[i]; }
	void undo();

	void Invalidate();
};


class Deformation_TEST
{
public:

	typedef std::vector< MbVec3d > TElemVectors; //FIXME
	typedef std::vector< TElemVectors > TElemVectorsV; //FIXME

	static void log_displacement( std::vector<MbVec3d>& offset_v );
	static std::vector<MbVec3d> convert_data( OIDIVectorSetI* vectors, MbVec3d scale_def_3d );
	static std::vector<MbVec3d> convert_data_FAKE( OIDIVectorSetI* vectors, MbVec3d scale_def_3d );
	static MbVec3d _vector( const geo::IElement& e, int ini, int end );
	static void explore_geometry_TEST( const OIDIGeometryI& g, double scale_gen );
	static TElemVectorsV _vectors( const OIDIGeometryI& g );
};

