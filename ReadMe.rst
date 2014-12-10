Graph by padowan.dk - Version 4.4.2

Graph is an application used for drawing functions in a coordinate system. The
program is licensed under the GNU Gneral Public License.
This pacakge contains the following parts necesarry to rebuild the application:
=============

Source
The program itself written by Ivan Johansen in Borland C++ Builder. The GPL
license (see COPYING) covers all files found in the Graph catalog and
subcatalogs.

Help
The help files in English and translations of the English version.

Locale
Language translations in po format of the program.

Components
C++ Builder components written by Ivan Johansen specific for use in Graph but
can be used in any application. See COPYING for license.

Func32
Library for handling mathematical functions written by Ivan Johansen specific
for use in Graph, though the library should be portable to any C++ compiler.
Distributed under GNU GPL (See COPYING).

Thumbnail Handler
Shell extension for showing thumbnails of grf files. This is written in Delphi
and uses the EasyNSE library. It is basically just an example that follows
EasyNSE modified to show grf files.

Lib
Files used to support Python plugins.

Install
Files used to create the installation program.

Libraries\dxgettext
The file gnugettext.pas from dxgettext (http://dybdahl.dk/dxgettext), which is
an improved version of GNU gettext for Delphi/BCB, is used as translation
library.

Libraries\pdflib
C++ bindings for the PDFlib-Lite library.

Libraries\gsl
A part of the GNU Scientific Library (GSL) used by Graph. Only the part needed
by Graph is found here. Distributed under the GNU GPL (See COPYING).

Libraries\Python
Header files needed to use the Python DLL.

The following third party libraries are needed to build Graph:
=============
Boost
Boost (http://boost.org) is used as a general purpose library collection.
Boost is installed with C++ Builder.

PDFlib-Lite
PDFlib-Lite (http://www.pdflib.com) is used to create the PDFlib.dll, which is
used for generating PDF documents.

EasyNSE
EasyNSE (http://www.mustangpeak.net/) is used for shell extensions.


The program can be compiled with Borland C++ Builder XE Pro/Enterprise. To
compile with other versions some changes to the code is probably requeried.


To compile the help file, see Help\Scripts\HowTo.txt


