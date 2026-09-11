#pragma once

// This header is automatically force-included in the VS project file; not included on linux

#pragma message("WARNING: disable C4210, C4244 and C4456 in project zlibvc (nonstandard extension, size mismatches and decl hides)")
#pragma warning(disable: 4210 4244 4456)