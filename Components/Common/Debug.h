//---------------------------------------------------------------------------
#ifndef DebugH
#define DebugH

struct TNameValue
{
  const char *Name;
  unsigned Value;
};

extern const TNameValue OleUIResultList[];
extern const TNameValue HResultList[];

//Defines and functions used for logging OLE calls
#ifdef _DEBUG
  #define DEBUG_CALL() AddToDebugLog("\n" __FUNC__)
  #define LOG_RESULT(x) DebugLogReturn(HResultList, x)
  #define LOG_ARG(x) AddToDebugLog(AnsiString("(") + x + ")")
  #define LOG_DATA(x) AddToDebugLog(AnsiString(", [") + x + "]")
  #define LOG_FUNCTION_CALL(x) (AddToDebugLog("\n  {" #x), DebugLogReturn(HResultList, (x), " }"))
  #define LOG_OLEUI_CALL(x) (AddToDebugLog("\n  {" #x), DebugLogReturn(OleUIResultList, (x), " }"))
#else
  #define DEBUG_CALL()
  #define LOG_RESULT(x) (x)
  #define LOG_ARG(x)
  #define LOG_DATA(x)
  #define LOG_FUNCTION_CALL(x) (x)
  #define LOG_OLEUI_CALL(x) (x)
#endif

#define NAME_VALUE_ENTRY(x) {#x, x}
#define NAME_VALUE_END {NULL, 0}

void AddToDebugLog(const char *Str);
void AddToDebugLog(const AnsiString &Str);
HRESULT DebugLogReturn(const TNameValue List[], HRESULT Result, const char *Str = "");

AnsiString ValueToStr(const TNameValue List[], unsigned Value);
AnsiString FlagsToStr(const TNameValue List[], unsigned Value);

void InitDebug();

//---------------------------------------------------------------------------
#endif
 