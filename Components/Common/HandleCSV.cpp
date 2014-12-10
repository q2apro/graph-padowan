//---------------------------------------------------------------------------
#include "Config.h"
#include <vcl.h>
#pragma hdrstop
#include "HandleCSV.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include "ICompCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
const std::string DefaultSeparators = "\t; ,";

//Returns separator used for line ('\t', ';', ' ', ',')
//Check for separator in priority order. A space cannot be a separator if comes
//after a comma.
char GetSeparator(const std::string &Str2, const std::string &Separators)
{
  if(Separators.size() == 1)
    return Separators[0];
  const std::string &Sep = Separators.empty() ? DefaultSeparators : Separators;
  std::string Str = Trim(Str2);
  for(std::string::const_iterator Ch = Sep.begin(); Ch != Sep.end(); ++Ch)
  {
    size_t n = Str.find(*Ch);
    if(n != std::string::npos && n != 0 && Str[n-1] != ',')
      return *Ch;
  }
  return 0;
}
//---------------------------------------------------------------------------
unsigned CountCols(const std::string &Str, char Separator)
{
	unsigned Count = 1;
  size_t Pos = 0;
  size_t NextPos;
  while((NextPos = Str.find(Separator, Pos)) != std::string::npos)
  {
    if(NextPos != Pos)
      Count++;
    Pos = NextPos + 1;
  }
	return Count;
}
//---------------------------------------------------------------------------
bool ImportCsv(std::istream &Stream, TCsvGrid &CsvGrid, const std::string &Separator)
{
	std::string Line;
	while(Stream && Line.empty())
		std::getline(Stream, Line);
	char Sep = GetSeparator(Line, Separator);
	unsigned LineNo = 1;

	do
	{
		if(!Line.empty() && Line[Line.size()-1] == '\r')
			Line.erase(Line.size()-1);
		//Ignore empty lines
		if(Line.empty())
			continue;

		//Several separators after each other (eg. spaces) are ignored
		size_t FirstPos = Line.find_first_not_of(Sep);
		size_t Pos = Line.find(Sep, FirstPos);
		std::string xText = Trim(Line.substr(FirstPos, Pos - FirstPos));
		TCsvRow CsvRow;
		CsvRow.push_back(xText);
		for(size_t LastPos = Line.find_first_not_of(Sep, Pos);
				Pos != std::string::npos; LastPos = Pos + 1)
		{
			Pos = Line.find(Sep, LastPos);

			//Ignore empty entries
			if(Pos == LastPos || LastPos == Line.size())
				continue;

			std::string yText = Trim(Line.substr(LastPos, Pos - LastPos));
			CsvRow.push_back(yText);
		}

		LineNo++;
    CsvGrid.push_back(CsvRow);
  }
  while(std::getline(Stream, Line));
  return true;
}
//---------------------------------------------------------------------------
bool ImportCsv(const std::string &Str, TCsvGrid &CsvGrid, const std::string &Separator)
{
	std::istringstream Stream(Str);
	return ImportCsv(Stream, CsvGrid, Separator);
}
//---------------------------------------------------------------------------
bool ImportCsvFromFile(const std::wstring &FileName, TCsvGrid &CsvGrid, const std::string &Separator)
{
	std::ifstream Stream(FileName.c_str());
	if(!Stream)
		return false;
	return ImportCsv(Stream, CsvGrid, Separator);
}
//---------------------------------------------------------------------------
bool ImportCsv(std::istream &Stream, TCsvGrid &CsvGrid, char Separator)
{
  return ImportCsv(Stream, CsvGrid, Separator == 0 ? DefaultSeparators : std::string(1, Separator));
}
//---------------------------------------------------------------------------
bool ImportCsv(const std::string &Str, TCsvGrid &CsvGrid, char Separator)
{
  return ImportCsv(Str, CsvGrid, Separator == 0 ? DefaultSeparators : std::string(1, Separator));
}
//---------------------------------------------------------------------------
bool ImportCsvFromFile(const std::wstring &FileName, TCsvGrid &CsvGrid, char Separator)
{
  return ImportCsvFromFile(FileName, CsvGrid, Separator == 0 ? DefaultSeparators : std::string(1, Separator));
}
//---------------------------------------------------------------------------

