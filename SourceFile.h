#ifndef _SOURCEFILE_H
#define _SOURCEFILE_H

#include "InformData.h"

#define MAXLEVEL 10
#define MAXLINES 5000
#define MAXTOKENS 10000

class ObjectData;
class FunctionData;

class SourceFile : public InformData
{
public:
  SourceFile();
  ~SourceFile();

  bool IsModified();
  bool SaveSourceFile();
  bool ReadSourceFile(const wxString& path);
  bool CreateSourceFile(const wxString& path);

  virtual wxString GetSource(int infType);
  virtual void SetSource(int infType, const wxString& newSource);

  wxString Name();
  wxString Path();
  wxString ToString();

  void InsertCode(const wxString& code, unsigned int begin);
  void ReplaceCode(const wxString& code, unsigned int begin, unsigned int end);
  virtual InformData *AppendObject(int type, const wxString& code);

  bool Parse();
  ObjectData *ParseObject(ObjectData *obj=NULL);
 	FunctionData *ParseFunction(FunctionData *obj=NULL);

protected:
  bool CopyBufferToSrc();
  unsigned int Tokenize(unsigned int startIdx, unsigned int stopIdx);
  void SkipLine(unsigned int& idx, bool breakLine=false);
  void PrintToken(char *msg, unsigned int idx);
  int ParseToken(unsigned int tokenIdx);
  void ParseCommand();
  void PrintCommand();
  void PrintStatement();

  wxString buffer;
  unsigned int lineno;
  unsigned int bCommand; // argument to keyword
  unsigned int eCommand;
  unsigned int bStatement; // whole property including final , or ;
  unsigned int eStatement;
  ObjectData *parent[MAXLEVEL];

	wxString name;
	wxString path;
  bool modified;

  char *src;
  unsigned int tokenIdx;
  bool srcModified;
  size_t currentSrcLen;

  int bToken[MAXTOKENS]; // beginning
  int eToken[MAXTOKENS]; // end
  int sToken[MAXTOKENS]; // size
  int lToken[MAXTOKENS]; // line number

  int bLine[MAXLINES]; // beginning
  int eLine[MAXLINES]; // end
};

#endif
