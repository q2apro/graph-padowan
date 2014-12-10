//---------------------------------------------------------------------------
#ifndef ConfigH
#define ConfigH
//---------------------------------------------------------------------------
#define STRICT
#ifdef _DEBUG
  #define BOOST_ENABLE_ASSERT_HANDLER
#else
  //Nasty hack: C++ Builder does not inline functions with throw specifications
	#define throw(...)

  #define NDEBUG
  #define BOOST_DISABLE_ASSERTS

  //Used with COM in utilcls.h; Disables assert checks and prevent linker warnings in release build
  #define _INC_CRTDBG
  #define NO_PROMPT_ON_ASSERTE_FAILURE
  #define NO_PROMPT_ON_HRCHECK_FAILURE
  #undef _ASSERTE
  #undef _ASSERTE_
	#undef OLECHECK
	#undef PROMPT_ON_ASSERTE_FAILURE
	#undef PROMPT_ON_HRCHECK_FAILURE
  #define _ASSERTE(expr) expr
  #define _ASSERTE_(expr) expr
	#define OLECHECK(hrexpr) hrexpr
	#define PROMPT_ON_ASSERTE_FAILURE 0
	#define PROMPT_ON_HRCHECK_FAILURE 0
#endif

//---------------------------------------------------------------------------
#endif
