#ifdef ARGUMENT_EXPORTS
#define ARGUMENT_API __declspec(dllexport)
#else
#define ARGUMENT_API __declspec(dllimport)
#endif

class ARGUMENT_API CArgument {
public:
	CArgument(void);
};

extern ARGUMENT_API int nArgument;

ARGUMENT_API int fnArgument(void);
