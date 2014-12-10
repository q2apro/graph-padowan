/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
// OleServer.h : Mini declaration of the interface TOleServerImpl. Doesn't need COM/ATL headers

#ifndef OleServerH
#define OleServerH

enum TAdviseCode
{
  acDataSaved,
  acClose,
  acRenamed,
  acSaveObject,
  acDataChanged,
  acShowWindow,
  acHideWindow,
  acShowObject
};

void SendOleAdvise(TAdviseCode AdviseCode);
bool OleServerRunning();

#endif
