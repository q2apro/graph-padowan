//---------------------------------------------------------------------------
#ifndef TokenizerH
#define TokenizerH
//---------------------------------------------------------------------------
#include <string>
#include <sstream>

typedef wchar_t TTokenChar;
typedef std::basic_string<TTokenChar> TTokenString;
typedef std::basic_istringstream<TTokenChar> TTokenStream;

struct Delimiter
{
  TTokenChar FDelimiter;
  Delimiter(TTokenChar ADelimiter) : FDelimiter(ADelimiter) {}
};

class TTokenizer
{
  TTokenString Str;
  TTokenChar FDelimiter;
  TTokenChar NextDelimiter;
  TTokenString::const_iterator Iter;
  bool FFailed;
  char FQuote;

  void Extract(TTokenString &S);

public:
  TTokenizer()
    : FDelimiter(L','), Iter(Str.begin()), FFailed(true), NextDelimiter(L','), FQuote(0) {}
  TTokenizer(const TTokenString &S, TTokenChar Delimiter = L',', TTokenChar Quote = 0)
    : Str(S), FDelimiter(Delimiter), Iter(Str.begin()), FFailed(false), NextDelimiter(Delimiter), FQuote(Quote) {}
  TTokenizer(const TTokenChar *S, TTokenChar Delimiter = TTokenChar(','), TTokenChar Quote = 0)
    : Str(S), FDelimiter(Delimiter), Iter(Str.begin()), FFailed(false), NextDelimiter(Delimiter), FQuote(Quote) {}
  TTokenizer(const TTokenizer &A)
    : Str(A.Str), FDelimiter(A.FDelimiter), NextDelimiter(A.NextDelimiter), Iter(Str.begin()), FFailed(false), FQuote(A.FQuote) {}

  TTokenString Next() {TTokenString Temp; Extract(Temp); return Temp;}
  TTokenString Next(char Delimiter) {TTokenString Temp; NextDelimiter = Delimiter; Extract(Temp); return Temp;};
  void SetText(const TTokenString &S) {Str = S; Iter = Str.begin(); FFailed = false; NextDelimiter = FDelimiter;}
  void SetDelimiter(TTokenChar Delimiter) {FDelimiter = Delimiter;}
  void Ignore(unsigned Length);
  bool Failed() const {return FFailed;}

  operator void*() const {return reinterpret_cast<void*>(!FFailed);}
  TTokenizer& operator>>(TTokenString &S) {Extract(S); return *this;}
  TTokenizer& operator>>(const Delimiter& ADelimiter) {NextDelimiter = ADelimiter.FDelimiter; return *this;}
  template<typename T> TTokenizer& operator>>(T &Var)
  {
    if(!FFailed)
    {
      TTokenStream Stream(Next());
      Stream >> Var;
      FFailed = !Stream;
    }
    return *this;
  }
  const TTokenizer& operator=(const TTokenizer &A)
  {
    Str = A.Str;
    FDelimiter = A.FDelimiter;
    NextDelimiter = A.NextDelimiter;
    Iter = Str.begin();
    FFailed = false;
    FQuote = A.FQuote;
    return *this;
  }

  const TTokenizer& operator=(const TTokenString &Str)
  {
    SetText(Str);
    return *this;
  }
};
#endif
