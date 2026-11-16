// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the TEST1_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// TEST1_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TEST1_EXPORTS
#define TEST1_API __declspec(dllexport)
#else
#define TEST1_API __declspec(dllimport)
#endif

// This class is exported from the dll
class TEST1_API CTest1 {
public:
	CTest1(void);
	// TODO: add your methods here.
};

extern TEST1_API int nTest1;

TEST1_API int fnTest1(void);
