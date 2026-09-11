#pragma once

// the _WIN32 guard is a bit superfluous, as there are guards also in the files that include this
#ifdef _WIN32
#pragma message(                                                                                                       \
    "WARNING: disable C4055, C4152/4204/4214, C4457/8 and C4701/2/3 in project TSMesh/Gts (data/function, nonstandard extensions, decl hides, potential uninit)")
#pragma warning(disable : 4055 4152 4204 4214 4456 4457 4701 4702 4703)
#endif
