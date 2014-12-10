//---------------------------------------------------------------------------
#ifndef TEvalFrameH
#define TEvalFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TEvalFrame : public TFrame
{
__published:	// IDE-managed Components
private:	// User declarations
public:		// User declarations
	__fastcall TEvalFrame(TComponent* Owner);
	virtual std::string GetErrorPrefix() =0;
	virtual void Eval(const TGraphElem *Elem) =0;
	virtual void SetPoint(const TGraphElem *Elem, int X, int Y) =0;
	virtual void SetEndPoint(const TGraphElem *Elem, int X, int Y) =0;
	virtual TFrame* GetFrame() =0;
};
//---------------------------------------------------------------------------
#endif
