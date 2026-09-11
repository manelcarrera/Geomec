#pragma once

// This header is automatically force-included in the VS project file; not included on linux

#pragma message("WARNING: disable C4244, C4456/7/8 and C4701 in project Rescue (size mismatches, decl hides, potential uninit)")
#pragma warning(disable: 4244 4456 4457 4458 4701)