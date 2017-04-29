
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OASISLIBRARY_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OASISLIBRARY_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef OASISLIBRARY_EXPORTS
#define OASISLIBRARY_API __declspec(dllexport)
#else
#define OASISLIBRARY_API __declspec(dllimport)
#endif

// This class is exported from the Oasis Library.dll
class OASISLIBRARY_API COasisLibrary {
public:
	COasisLibrary(void);
	// TODO: add your methods here.
};

extern OASISLIBRARY_API int nOasisLibrary;

OASISLIBRARY_API int fnOasisLibrary(void);

