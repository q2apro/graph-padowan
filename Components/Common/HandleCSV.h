//---------------------------------------------------------------------------
#ifndef HandleCSVH
#define HandleCSVH
#include <string>
#include <vector>
#include <iostream>
#include <exception>
//---------------------------------------------------------------------------
typedef std::vector<std::string> TCsvRow;
typedef std::vector<TCsvRow> TCsvGrid;
extern const std::string DefaultSeparators;
char GetSeparator(const std::string &Str2, const std::string &Separators = DefaultSeparators);
bool ImportCsv(std::istream &Stream, TCsvGrid &CsvGrid, const std::string &Separator=DefaultSeparators);
bool ImportCsv(const std::string &Str, TCsvGrid &CsvGrid, const std::string &Separator=DefaultSeparators);
bool ImportCsvFromFile(const std::wstring &FileName, TCsvGrid &CsvGrid, const std::string &Separator=DefaultSeparators);

bool ImportCsv(std::istream &Stream, TCsvGrid &CsvGrid, char Separator);
bool ImportCsv(const std::string &Str, TCsvGrid &CsvGrid, char Separator);
bool ImportCsvFromFile(const std::wstring &FileName, TCsvGrid &CsvGrid, char Separator);
#endif
