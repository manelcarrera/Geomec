class CPointSetEntry;
class CNewWellPathInput;
class CModelBase;

class WP_Helper
{
  CPointSetEntry* m_ps;

private:

  //void print_ps_wp();
  //void print_model_wp()

  bool is_present_ps( std::vector< CNewWellPathInput* >& inputs_v, CNewWellPathInput* input );
  bool is_present_in_model( CNewWellPathInput* input );
  void remove_inputs( std::vector< CNewWellPathInput* > inputs_v );
  std::vector< CNewWellPathInput* > duplicates_ps_wp();

  bool is_in_box( CModelBase& rootModel, CNewWellPathInput* input );

public:

  WP_Helper( CPointSetEntry* ps ) : m_ps( ps ) {}
  void CopyAllWellpaths();
};