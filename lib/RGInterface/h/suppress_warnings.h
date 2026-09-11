#pragma once

// This header is automatically force-included in the VS project file; not included on linux

#pragma message(                                                                                                       \
    "WARNING: disable C4018/4389, C4244/67, C4251, C4701 and C4838 in project RGInterface (sign mismatch, loss, dll, potential uninit, float conversion)")
#pragma warning(disable : 4018 4244 4251 4267 4389 4701 4838)
