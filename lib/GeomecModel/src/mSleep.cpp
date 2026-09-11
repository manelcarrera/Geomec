#ifndef WIN32
#include <cassert>

#include <unistd.h>
#else // WIN32
#include <Windows.h>
#endif // WIN32

#include "mSleep.h"

namespace {

const unsigned int THOUSAND = 1000;

} // anonymous namespace

void mSleep(unsigned int milliSeconds) {
#ifndef WIN32
  assert(milliSeconds < THOUSAND);

  usleep(THOUSAND * milliSeconds);
#else  // WIN32
  Sleep(milliSeconds);
#endif // WIN32
}
