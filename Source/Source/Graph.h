/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Func32 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * Graph is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Func32; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
//---------------------------------------------------------------------------
#ifndef GraphH
#define GraphH
//---------------------------------------------------------------------------
//Common includes
//Make sure that Microsoft headers are included correct
#define NO_WIN32_LEAN_AND_MEAN

//Don't include all ATL headers
#undef INC_ATL_HEADERS
#define USING_ATLVCL //Remove warning

#include "Config.h"
#include <vcl.h>
#undef MAXINT //Avoid double definition
#undef PACKAGE
#define PACKAGE //Remove the PACKAGE macro; Only used to add export section to exe file and for packages
#include "GnuGetText.hpp"
#include "Common.h"
#include "Draw.h"
#include "Convert.h"
#include "Resource.h"
#include "GraphUndo.h"
#include "GuiUtil.h"

#define NAME L"Graph"
#define MINVERSION L"2.5"  //Minimum version needed to read files written by this version
#define MIN_SAVED_VERSION L"2.5" //Minimum version used to write file to load with this version

#define HOMEPAGE L"http:\/\/www.padowan.dk"
#define EMAIL L"graph@padowan.dk"
#define DOWNLOADPAGE L"http:\/\/www.padowan.dk/download"
#define DONATE_PAGE L"http:\/\/www.padowan.dk/donate"
#define SUPPORT_PAGE L"http:\/\/forum.padowan.dk"
#define INF_FILE L"http:\/\/www.padowan.dk/graph/GraphVersion.php"
#define CLIPBOARDFORMAT L"Graph Unicode Data"

#define REGISTRY_KEY L"Software\\Ivan\\Graph"//Registry key to use

//The default font used if the chosen one can't be found
#define DEFAULT_FONT L"Times New Roman"

//Default font used for labels and numbers
#define DEFAULT_NUMBER_FONT     L"Times New Roman,10,clBlack"
#define DEFAULT_LABEL_FONT      L"Times New Roman,12,clBlack,B" //Used for axes labels
#define DEFAULT_TEXT_LABEL_FONT L"Times New Roman,12,clBlack"   //Used for text labels
#define DEFAULT_LEGEND_FONT     L"Times New Roman,8,clBlack"
#define DEFAULT_TITLE_FONT      L"Times New Roman,20,clBlack,B"
#define DEFAULT_POINT_FONT      L"Tahoma,8,clBlack" //Tahoma has the angle symbol

#define CLIPBOARD_FORMAT L"Ivan's Graph"

#define MIN_ZERO   1E-14 //Numbers less than this is zero

//---------------------------------------------------------------------------
#endif
