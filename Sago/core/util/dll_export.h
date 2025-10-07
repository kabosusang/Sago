#ifndef SG_DLL_EXPORT_H
#define SG_DLL_EXPORT_H

#if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#define SAGA_DEF_DLLEXPORT __declspec(dllexport)
#define SAGA_DEF_DLLIMPORT __declspec(dllimport)
#elif defined __GNUC__ && __GNUC__ >= 4
#define SAGA_DEF_DLLEXPORT __attribute__((visibility("default")))
#define SAGA_DEF_DLLIMPORT __attribute__((visibility("default")))
#else /* Unsupported compiler */
#define SAGA_DEF_DLLEXPORT
#define SAGA_DEF_DLLIMPORT
#endif

#ifndef SAGA_API
#if defined SAGA_API_EXPORT
#define SAGA_API SAGA_DEF_DLLEXPORT
#elif defined SAGA_API_IMPORT
#define SAGA_API SAGA_DEF_DLLIMPORT
#else /* No API */
#define SAGA_API
#endif
#endif



#endif