#ifndef __GM_VERSION_H_
#define __GM_VERSION_H_

// GM_REVISION, RGI_REVISION, CORA_REVISION are automatically increased by the Release Build Agent
// GM_BUILD_STRING is automatically updated by the Release and Nightly Release Agent
// The format of the GM_BUILD_STRING is defined in the build definitions, and currently set to
//   YD.# with Y = 2-digit year, D = day of year, # = lowest integer to make it unique
// It is safe to adjust anything in this file manually. Changes to GM_BUILD_STRING not following
// the format in the build agents will simply be overwritten, and the build agents expect the
// lines defining the macros to use the following syntax:
// <START OF LINE>#define<SPACE>VARIABLE<SPACE>TOKENLIST<WHITESPACE>*<END OF LINE>
// with no spaces in the TOKENLIST.

#define GM_MAJOR 5
#define GM_MINOR 8
#define GM_BUILD 0
#define GM_REVISION 0

#define RGI_MAJOR 5
#define RGI_MINOR 8
#define RGI_BUILD 0
#define RGI_REVISION 0

#define CORA_MAJOR 5
#define CORA_MINOR 8
#define CORA_BUILD 0
#define CORA_REVISION 0

#define GM_BUILD_STRING 20105.1

// empty for Trunk
#define GM_BUILD_BRANCH_INFO ""

#if BUILD_LICENSE_OPTION == -1
#define GM_BUILD_LICENSE_INFO "no license "
#else
#define GM_BUILD_LICENSE_INFO ""
#endif

#ifndef WIN32
#define GM_BUILD_PLATFORM "linux "
#else
#define GM_BUILD_PLATFORM ""
#endif

#ifdef GM_BUILD_TYPE_CI
#ifdef GM_BUILD_QUALIFIER
#error "Only one GM_BUILD_TYPE_* can be defined."
#endif
#define GM_BUILD_QUALIFIER "CI "
#endif

#ifdef GM_BUILD_TYPE_TEST
#ifdef GM_BUILD_QUALIFIER
#error "Only one GM_BUILD_TYPE_* can be defined."
#endif
#define GM_BUILD_QUALIFIER "test "
#endif

#ifdef GM_BUILD_TYPE_NIGHTLY
#ifdef GM_BUILD_QUALIFIER
#error "Only one GM_BUILD_TYPE_* can be defined."
#endif
#define GM_BUILD_QUALIFIER "dev nightly "
#endif

#ifdef GM_BUILD_TYPE_RELEASE_NIGHTLY
#ifdef GM_BUILD_QUALIFIER
#error "Only one GM_BUILD_TYPE_* can be defined."
#endif
#define GM_BUILD_QUALIFIER "nightly "
#endif

#ifdef GM_BUILD_TYPE_RELEASE
#ifdef GM_BUILD_QUALIFIER
#error "Only one GM_BUILD_TYPE_* can be defined."
#endif
#define GM_BUILD_QUALIFIER ""
#endif

#ifndef GM_BUILD_QUALIFIER
#define GM_BUILD_QUALIFIER "dev "
#endif

#define GM_BUILD_PREFIX GM_BUILD_BRANCH_INFO GM_BUILD_LICENSE_INFO GM_BUILD_PLATFORM GM_BUILD_QUALIFIER "build"

#define xstr_helper(s) #s
#define xstr(s) xstr_helper(s)

#endif
