
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "RGSync.h"

namespace
{

const char* programName(const char* argv0)
{
  const char* p = strrchr(argv0, '/');

  if (p == 0)
  {
    p = strrchr(argv0, '\\');
  }

  if (p != 0)
  {
    ++p; 
  }

  if ((p != 0) && (*p != 0))
  {
    return p;
  }

  return argv0;
}

void usage(const char* argv0)
{
  fprintf(stderr, "Usage: %s <rockmechfile>\n", programName(argv0));
  fprintf(stderr, "\n");
}

} // anonymous namespace

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    usage(argv[0]);

    exit(1);
  }

  RGSync sync(RGSync::Lead, argv[1]);

  sync.init();
  sync.release();
  sync.waitFor(0);

	return 0;
}
