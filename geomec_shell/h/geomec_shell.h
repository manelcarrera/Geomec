#ifndef _GEOMEC_SHELL_H_
#define _GEOMEC_SHELL_H_

#include "version.h"
#include <QString>

namespace gm_shell {

const QString APPLICATION = "geomec_shell";
const QString VERSION =
    xstr(GM_MAJOR) "." xstr(GM_MINOR) "." xstr(GM_BUILD) "." xstr(GM_REVISION) " " GM_BUILD_PREFIX
                                                                               " " xstr(GM_BUILD_STRING);

} // namespace gm_shell

#endif