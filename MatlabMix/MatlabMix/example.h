//
// MATLAB Compiler: 5.1 (R2014a)
// Date: Tue Nov 10 12:36:22 2015
// Arguments: "-B" "macro_default" "-W" "cpplib:example" "-T" "link:lib"
// "example3" 
//

#ifndef __example_h
#define __example_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_example
#define PUBLIC_example_C_API __global
#else
#define PUBLIC_example_C_API /* No import statement needed. */
#endif

#define LIB_example_C_API PUBLIC_example_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_example
#define PUBLIC_example_C_API __declspec(dllexport)
#else
#define PUBLIC_example_C_API __declspec(dllimport)
#endif

#define LIB_example_C_API PUBLIC_example_C_API


#else

#define LIB_example_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_example_C_API 
#define LIB_example_C_API /* No special import/export declaration */
#endif

extern LIB_example_C_API 
bool MW_CALL_CONV exampleInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_example_C_API 
bool MW_CALL_CONV exampleInitialize(void);

extern LIB_example_C_API 
void MW_CALL_CONV exampleTerminate(void);



extern LIB_example_C_API 
void MW_CALL_CONV examplePrintStackTrace(void);

extern LIB_example_C_API 
bool MW_CALL_CONV mlxExample3(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__BORLANDC__)

#ifdef EXPORTING_example
#define PUBLIC_example_CPP_API __declspec(dllexport)
#else
#define PUBLIC_example_CPP_API __declspec(dllimport)
#endif

#define LIB_example_CPP_API PUBLIC_example_CPP_API

#else

#if !defined(LIB_example_CPP_API)
#if defined(LIB_example_C_API)
#define LIB_example_CPP_API LIB_example_C_API
#else
#define LIB_example_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_example_CPP_API void MW_CALL_CONV example3(int nargout, mwArray& y, const mwArray& i);

#endif
#endif
