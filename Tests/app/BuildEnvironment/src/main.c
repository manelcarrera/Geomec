/* Small program with no dependencies besides c-lib, to print arguments and specific environment variables.
   Mainly intended as short stand-alone project to test TFS build parameters.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <version.h>


int interactive = 0;


void printVersion()
{
  printf("Version: " xstr(GM_MAJOR) "." xstr(GM_MINOR) "." xstr(GM_BUILD) "." xstr(GM_REVISION) " " GM_BUILD_PREFIX " " xstr(GM_BUILD_STRING) "\n");
}

void printEnvironment(char *vars[])
{
  int i;

  printf("Environment:\n");
  for (i = 0; vars[i]; ++i)
  {
    printf("  %s=%s\n", vars[i], getenv(vars[i]));
  }
}

void printArguments(int argc, char *argv[])
{
  int i;

  printf("Arguments:\n");
  for (i = 0; i < argc; ++i)
  {
    printf("  %d: %s\n", i, argv[i]);
    if (!strncmp(argv[i], "--interactive", 13))
      interactive = 1;
  }
}


int main(int argc, char *argv[])
{
  char *env[] = {
    "QTDIR",
    "QWTDIR",
    "DIAROOT",
    "INTEL",
    "PATH",
    NULL
  };
  printVersion();
  printEnvironment(env);
  printArguments(argc, argv);

  if (interactive)
  {
    printf("Press a key to continue...\n");
    getchar();
  }

  return 0;
}
