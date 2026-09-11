#pragma once

class CTest
{
  int m_idx;

public:
  enum eCmd{Open,Run,Export,Goto,Label,Command,Unknown};

  static QString m_cmd_qs[];
  static std::map< QString, CTest::eCmd > m_cmd_map;

  struct TCmd
  {
    eCmd id;
    QStringList params;
  };

  std::vector< TCmd > m_cmd_v;

  QString m_url;

  static const char* ana_type_cp[];

private:
  std::vector< CTest::TCmd > CTest::parse(const QString& url);

public:
  CTest( const QString& url = "" );
  ~CTest();

  bool execute( int idx );
  bool execute( TCmd& cmd );

  int idx(){ return m_idx; };
  void idx( int val ){ m_idx=val; };

  int num(){ return m_cmd_v.size(); }

  eCmd current_cmd(){ return m_cmd_v[ m_idx ].id; }

  QString cmd_s( int idx );

  QString url(){ return m_url; }
};