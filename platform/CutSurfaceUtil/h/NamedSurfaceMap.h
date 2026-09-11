
#if !defined(NAMED_SURFACE_MAP)
#define NAMED_SURFACE_MAP

#include "TriaIntersecBox.h"
#include <map>
#include <string>
#include <vector>

namespace geo {
class ISurface;
};

#ifdef EXP_CUT_SURFACE
#define DECLSPECIFIER __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define DECLSPECIFIER __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif

// Instantiate classes vector<int> and vector<char>
// This does not create an object. It only forces the generation of all
// of the members of classes vector<int> and vector<char>. It exports
// them from the DLL and imports them into the .exe file.
// EXPIMP_TEMPLATE template class DECLSPECIFIER std::map< SIDE_ID ,const geo::ISurface* >;
// EXPIMP_TEMPLATE template class DECLSPECIFIER std::map< const void* ,const geo::ISurface*>;
// EXPIMP_TEMPLATE template class DECLSPECIFIER std::map< const geo::ISurface*,std::string   >;
// EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector< const geo::ISurface*   >;
// EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector< STNamedSurface >;
// EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector< STNamedSurface >;

// typedef std::vector<STNamedSurface> TNamedSurfaces;

typedef std::map<SIDE_ID, const geo::ISurface *> TSideSurfMap;
typedef std::map<const void *, const geo::ISurface *> TObjectSurfMap;
typedef std::map<const geo::ISurface *, std::string> TNamedSurfMap;
typedef std::vector<const geo::ISurface *> TSurfVector;

#endif