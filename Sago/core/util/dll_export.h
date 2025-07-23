#ifndef SG_DLL_EXPORT_H
#define SG_DLL_EXPORT_H

#ifdef SAGA_DEF_DLLEXPORT
#ifdef SAGA_PLATFORM_WIN32
#define SAGA_API __declspec(dllexport)
#endif
#endif

#ifndef SAGA_API
#define SAGA_API
#endif





#endif