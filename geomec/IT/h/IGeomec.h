#ifndef _IGeomec
#define _IGeomec

#include "IT_Geomec_Interface.h" //FIXME

namespace IGeomec
{
	namespace base
	{
		enum eType
		{
			Surface,
			Volume
		};
		CModelBase& model();
		CGeomecDoc& doc();
		geo::IElementSet& get_elements( CFormationBase& formation );
		IPointSet& get_points( CFormationBase& formation );
		void branch( int depletion_id = IT_Geomec_Interface::LAST_DEPLETION_STAGE, bool is_branch = true );
	};
	/*namespace reservoir
	{
		static TReservoirL& list();
		static CFormationBase& get_formation( const std::string formation_id );
		static CFormationBase& get_formation( int idx );
		static Reservoir& reservoir( int reservoir_id, base::eType _type = base::eType::Surface );
	};*/
	namespace depletion
	{
		//static TDepletionL& list();
		TPointValueV pressures( CFormationBase& formation, const IResultComponent& component );
	};
	namespace print
	{
		void formation( const CFormationBase& formation );
		void point_value( const TPointValueV point_value_v );
		void elem_values( const TElemValuesV elem_values_v );
		void elements( const geo::IElementSet& elements );
		void points( const geo::IElementSet& elements );
		void point( const int point_index, const geo::IPoint& point );
		void element( const geo::IElement& element );
		void list( const TStringL list );
	};
	namespace displacement
	{
		void get_v1();
		TPointValueV get( CFormationBase& formation, IVectorResult::CVectorComponent& component );
	};
	namespace draft
	{
		void do_something();
	};
};

#endif
