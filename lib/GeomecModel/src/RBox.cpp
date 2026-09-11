#include "RBox.h" 

#include "Printer.h" 

/////////////////////////////////////// static functions /////////////////////////////////////

// distance
T3da oiv_to_rw_POS( T3da pos )
{
  T3da& t = RefinementBox::m_translation;
  return T3da{ pos[0]+t[0], pos[1]+t[1], pos[2]+t[2] };
}

T3da rw_to_oiv_POS( T3da pos )
{
  T3da& t = RefinementBox::m_translation;
  return T3da{ pos[0]-t[0], pos[1]-t[1], pos[2]-t[2] };
}

//size
T3da rw_to_oiv_SIZE( T3da size )
{
  return T3da{ size[0]/2, size[1]/2, size[2]/2 };
}

T3da oiv_to_rw_SIZE( T3da size )
{
  return T3da{ 2*size[0], 2*size[1], 2*size[2] };
}

// public static funcions
T3da RBox::oiv_to_rw( T3da val, RBox::eProp prop  )
{
  if( prop == RBox::Size )
    return oiv_to_rw_SIZE( val );
  else
    return oiv_to_rw_POS( val );
}
T3da RBox::rw_to_oiv( T3da val, RBox::eProp prop  )
{
  if( prop == RBox::Size )
    return rw_to_oiv_SIZE( val );
  else
    return rw_to_oiv_POS( val );
}

/////////////////////////////////////////// RefinementBox /////////////////////////////////////////

const T3da RefinementBox::min_() const 
{ 
  const T3da& c = m_center;
  const T3da& s = m_size;

  return T3da{ c[0]-s[0], c[1]-s[1], c[2]-s[2] }; 
};

const T3da RefinementBox::max_() const 
{ 
  const T3da& c = m_center;
  const T3da& s = m_size;

  return T3da{ c[0]+s[0], c[1]+s[1], c[2]+s[2] }; 
};

const TBox RefinementBox::box() const
{
  return TBox{ min_(), max_() }; 
}


/////////////////////////////////////// RelativeSize /////////////////////////////////////////

bool RelativeSize::operator< ( const RelativeSize& other ) const 
{ 
  return	m_distance < other.m_distance &&
      m_size < other.m_size; 
};
bool RelativeSize::operator<=( const RelativeSize& other ) const 
{ 
  return	m_distance <= other.m_distance &&
      m_size <= other.m_size; 
};
bool RelativeSize::operator> ( const RelativeSize& other ) const 
{ 
  return	m_distance > other.m_distance &&
      m_size > other.m_size; 
};
bool RelativeSize::operator>=( const RelativeSize& other ) const 
{ 
  return	m_distance >= other.m_distance &&
      m_size >= other.m_size; 
};
bool RelativeSize::operator==( const RelativeSize& other ) const 
{ 
  return	m_distance == other.m_distance && 
      m_size == other.m_size; 
};
bool RelativeSize::operator!=( const RelativeSize& other ) const 
{ 
  return	m_distance != other.m_distance ||
      m_size != other.m_size; 
};

void RelativeSize::print()
{
  Printer::instance()->debug("Gradient > [d:%d,s:%.2f]", m_distance, m_size );
}


/////////////////////////////////////////// RefinementBox /////////////////////////////////////////

T3da RefinementBox::m_translation{0,0,0};

RefinementBox::RefinementBox( 

  const T3da& center, 
  const T3da& size, 
  const T3da& rotation, 
  GradientType_ direction, 
  const TGradientV& profile_v,
  const QString& name, 
  const bool& visible )

  : m_center( center ), m_size( size ), m_rotation( rotation ), m_gradientType( direction ), m_profile_v( profile_v ), m_name( name ), m_visible( visible )
{
};

void RefinementBox::print()
{
  const T3da& c = this->Center();
  const T3da& s = this->Size();
  const T3da& r = this->Rotation();
  RefinementBox::GradientType_ g = this->GradientType();

  Printer::instance()->debug("RefinementBox > n:[%s] v:[%d] c:[%.2f,%.2f,%.2f] s:[%.2f,%.2f,%.2f] r:[%.2f,%.2f,%.2f] g:%d steps:%d", 
    this->Name().toStdString().c_str(),
    this->IsVisible(),
    c[ 0 ], c[ 0 ], c[ 0 ],
    s[ 0 ], s[ 0 ], s[ 0 ],
    r[ 0 ], r[ 0 ], r[ 0 ],
    g, 
    this->Profile().size() );

  for( auto gradient_step : this->Profile() )
    gradient_step.print();
}

bool RefinementBox::operator==( const RefinementBox& other ) const
{
  bool res1 = 
    Name() == other.Name() &&
    IsVisible() == other.IsVisible() &&
    Center() == other.Center() &&
    Size() == other.Size() &&
    Rotation() == other.Rotation() &&
    GradientType() == other.GradientType();

  typedef std::vector< RelativeSize > TGradient;
  const TGradient& g1 = Profile();
  const TGradient& g2 = other.Profile();

  bool res2 = g1 == g2;

  return res1 && res2;
}

bool RefinementBox::operator!=( const RefinementBox& other ) const
{
  return !( *this == other );
}


