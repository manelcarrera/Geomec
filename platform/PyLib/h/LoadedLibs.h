#ifndef _LoadedLibs
#define _LoadedLibs

#include <vector>

class CLoadedLibs {
  std::vector<std::string> m_loaded_libraries;

  // std::size_t m_start_idx;

public:
  CLoadedLibs() {};

  // std::size_t start_idx(){ return m_start_idx; };
  // void start_idx( std::size_t val ){ m_start_idx = val ; };

  std::size_t CLoadedLibs::size() { return m_loaded_libraries.size(); }

  void CLoadedLibs::save();
  void CLoadedLibs::print_added_libs(int idx);
  void CLoadedLibs::unload(int idx);
};

#endif //_LoadedLibs