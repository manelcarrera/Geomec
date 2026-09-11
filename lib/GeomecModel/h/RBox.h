#pragma once

#include <array>
#include <vector>

#include <QMap>
#include <QString>

typedef std::array< double, 3 > T3da;

/////////////////////////////////////////// 
//
//			TLine
//
/////////////////////////////////////////// 

struct TLine
{
	double m;
	double M;

	TLine( double m_, double M_ ) : m( m_ ), M( M_ ){}
};

/////////////////////////////////////////// 
//
//			TBox
//
/////////////////////////////////////////// 

struct TBox
{
private:

	static bool overlapping_1D( const TLine& l1, const TLine& l2 )
	{
		return( l1.M >= l2.m && l2.M >= l1.m );
	}

public:
	T3da m;
	T3da M;

	TBox( const T3da& m_, const T3da& M_ ) : m( m_ ), M( M_ ){}
	TBox() : m( T3da{0,0,0} ), M( T3da{0,0,0} ){}

	bool overlaps( TBox b2 )
	{
		return	overlapping_1D( TLine( m[0], M[0] ), TLine( b2.m[0], b2.M[0] ) ) &&	// x
                overlapping_1D( TLine( m[1], M[1] ), TLine( b2.m[1], b2.M[1] ) ) &&	// y
                overlapping_1D( TLine( m[2], M[2] ), TLine( b2.m[2], b2.M[2] ) ); 	// z
	}

	bool contains( TBox b2 )
	{
		return	(m[0] <= b2.m[0] && M[0] >= b2.M[0]) &&
				(m[1] <= b2.m[1] && M[1] >= b2.M[1]) &&
				(m[2] <= b2.m[2] && M[2] >= b2.M[2]);
	}

	static bool overlapping_3D( TBox b1, TBox b2 )
	{
		return b1.overlaps( b2 );
	}

};

/////////////////////////////////////////// 
//
//			RBox
//
/////////////////////////////////////////// 

class RBox
{
public:
	enum eProp
	{
		Pos,
		Size
	};
	static T3da oiv_to_rw( T3da size, RBox::eProp prop  );
	static T3da rw_to_oiv( T3da val, RBox::eProp prop  );
};

/////////////////////////////////////////// 
//
//			RelativeSize
//
/////////////////////////////////////////// 

class RelativeSize
{
	int m_distance;
	double m_size;

public:

	RelativeSize( const int distance, const double size ) : m_distance( distance ), m_size( size ){};
	int Distance() const { return m_distance; };
	double Size() const { return m_size; };

	bool operator< ( const RelativeSize& other ) const;
	bool operator<=( const RelativeSize& other ) const;
	bool operator> ( const RelativeSize& other ) const;
	bool operator>=( const RelativeSize& other ) const;
	bool operator==( const RelativeSize& other ) const;
	bool operator!=( const RelativeSize& other ) const;

	void print();
};

typedef std::vector< RelativeSize > TGradientV;
typedef QMap< QString, TGradientV > TGradientM;

/////////////////////////////////////////// 
//
//			RefinementBox 
//
/////////////////////////////////////////

class RefinementBox
{
public:

	enum eProperty
	{
		Position,
		Size_
	};

	enum GradientType_ : unsigned int
	{
	  PlanarZ,
	  CenterPoint
	};

	static T3da m_translation;

private:
	T3da m_center;
	T3da m_size;
	T3da m_rotation;
	GradientType_ m_gradientType;
	TGradientV m_profile_v;

	QString m_name;
	bool m_visible;

public:
	RefinementBox(){};

	RefinementBox(	const T3da& center, 
					const T3da& size, 
					const T3da& rotation, 
					GradientType_ direction, 
					const TGradientV& profile,
					const QString& name, 
					const bool& visible );
	
	void SetCenter( const T3da& center ){	m_center = center; };
	void SetSize( const T3da& size ){	m_size = size; };
	void SetRotation( const T3da& rotation ){	m_rotation = rotation; };
	void SetProfile( const std::vector< RelativeSize >& profile_v){	m_profile_v = profile_v; };
	void SetGradientType( const GradientType_ type ){ m_gradientType = type; };
	void SetName( const QString& name ){ m_name = name; };
	void SetVisible( const bool& val ){ m_visible = val; };

	const T3da& Center() const { return m_center; };
	const T3da& Size() const { return m_size; };
	const T3da& Rotation() const { return m_rotation; };
	GradientType_ GradientType() const { return m_gradientType; };
	const std::vector< RelativeSize >& Profile() const { return m_profile_v; };
	const QString& Name() const { return m_name; };
	const bool& IsVisible() const { return m_visible; };

	const T3da min_() const;
	const T3da max_() const;
	const TBox box() const;

	void print();

	bool operator==( const RefinementBox& other ) const;
	bool operator!=( const RefinementBox& other ) const;
};

typedef QMap< QString, RefinementBox > TBoxMap;
typedef std::vector< RefinementBox > TBoxV;
typedef QMap< QString, RefinementBox::GradientType_ > TTypeM;