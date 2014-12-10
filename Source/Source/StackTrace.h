/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef StackTraceH
#define StackTraceH
//---------------------------------------------------------------------------
class EDllLoadError : public Exception
{
public:
  EDllLoadError(const String &Str) : Exception(Str) {}
};

void SetApplicationExceptionHandler(bool ALogAllExceptions);
void LogUncaughtException(TObject *Sender, Exception *E);
void LogUncaughtCppException(const char *Message, const char *Thread);
//---------------------------------------------------------------------------
#endif
