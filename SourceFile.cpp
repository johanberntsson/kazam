/*
* Copyright (c) 2003 Johan Berntsson, j.berntsson@qut.edu.au
* Queensland University of Technology, Brisbane, Australia
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/stream.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/tokenzr.h>
#include <wx/textfile.h>

#include "Project.h"
#include "SourceFile.h"
#include "ObjectData.h"
#include "FunctionData.h"

#include "kazam.h"
extern MainFrame *frame;
extern wxString informKeyword[];

wxString tokens[]={
  "Object", "Include", "Constant", "[", "Class", "Verb", "Extend",
  "Attribute", "Array", "Release", "#ifdef", "#ifndef", "#endif",
  "replace", "Nearby", "Property",
  "Abbreviate", "Switches", "Message", "Global",
  "" };

#define TT_UNKNOWN    -1
#define TT_OBJECT      0
#define TT_INCLUDE     1
#define TT_CONSTANT    2
#define TT_FUNCTION    3
#define TT_CLASS       4
#define TT_VERB        5
#define TT_EXTEND      6
#define TT_ATTRIBUTE   7
#define TT_ARRAY       8
#define TT_RELEASE     9
#define TT_IFDEF      10
#define TT_IFNDEF     11
#define TT_ENDIF      12
#define TT_REPLACE    13
#define TT_NEARBY     14
#define TT_PROPERTY   15
#define TT_ABBREVIATE 16
#define TT_SWITCHES   17
#define TT_MESSAGE    18
#define TT_GLOBAL     19

#define EQS(T,S) ((strncmp(&src[bToken[T]], S, sToken[T]))==0)
#define NEQS(T,S) ((strncmp(&src[bToken[T]], S, sToken[T])))
#define EQC(T,C) (src[bToken[T]]==C)
#define NEQC(T,C) (src[bToken[T]]!=C)
#define EQC1(T,C) (src[bToken[T]+1]==C)

SourceFile::SourceFile()
{
  frame->project.ResetSourceFileCache();

  path="";
  name="";
  src=NULL;
  modified=false;
  type=INF_T_SOURCEFILE;
}

SourceFile::~SourceFile()
{
  if(src) free(src);
}

// return true if error found
bool SourceFile::CreateSourceFile(const wxString& path)
{
  if(::wxFileExists(path)) ::wxRemoveFile(path);

  wxTextFile f(path);
  if(!f.Exists()) {
    f.Create();
    f.Open();
    f.AddLine("! Inform source file");
    f.Write();
    f.Close();
  } else {
    wxLogError("Can't create new source file "+path);
    return true;
  }

  return ReadSourceFile(path);
}

// return true if error found
bool SourceFile::ReadSourceFile(const wxString& path)
{
  file=frame->project.fileCnt++;

  this->path=path;
  this->name=::wxFileNameFromPath(path);

  buffer="";

  wxFileInputStream fs(path);
  if(!fs.Ok()) {
    wxLogWarning("Can't open %s for reading", path.c_str());
    return true;
  }

  wxTextInputStream ts(fs);
  while(!fs.Eof()) {
    buffer+=ts.ReadLine()+"\n";
  }

//  wxLogDebug("Setting cwd to "+path);
  ::wxSetWorkingDirectory(::wxPathOnly(path));

  modified=false;
  return Parse();
}

// return true if error found
bool SourceFile::SaveSourceFile()
{
  wxFileOutputStream fs(path);
  if(!fs.IsOk()) {
    wxLogWarning("Can't open %s for writing", path.c_str());
    return true;
  }
  wxTextOutputStream ts(fs);

  unsigned int i, j=0;
  for(i=0; i<buffer.Length(); i++) {
    if(buffer[i]=='\n') {
      wxString token=buffer.SubString(j, i-1);
      j=i+1;
      //wxLogDebug("lineno:%s (%d)\n", token.c_str(), token.Length());
      fs.Write(token.c_str(), token.Length());
      if(ts.GetMode()==wxEOL_DOS) {
        fs.Write("\r\n", 2);
      } else {
        fs.Write("\n", 1);
      }
    }
  }
  modified=false;
  return false;
}

bool SourceFile::IsModified()
{
  return modified;
}

wxString SourceFile::GetSource(int infType)
{
  if(infType==INF_ID) return path;
  if(infType==INF_SOURCE) return buffer;
  if(e[infType]==0) return ""; // This segment does not exist
  return buffer.SubString(b[infType], e[infType]);
}

void SourceFile::SetSource(int infType, const wxString& newSource)
{
  SetDirty();
  modified=true;
  if(e[infType]==0) {
    buffer=newSource;
  } else {
    InformData::SetSource(infType, newSource);
  }
}

void SourceFile::InsertCode(const wxString& code, unsigned int begin)
{
  modified=true;
  unsigned int prevSize=buffer.Length();
  buffer=buffer.Left(begin)+code+buffer.Mid(begin);
  if(begin==prevSize) return;

  unsigned int sizeDiff=buffer.Length()-prevSize;
  for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
    InformData *current=&frame->project.objects[i];
    current->AdjustPointers(file, begin, sizeDiff);
  }
}

void SourceFile::ReplaceCode(const wxString& code, unsigned int begin, unsigned int end)
{
  unsigned int prevSize=buffer.Length();

  unsigned int offset=0;
  if(buffer[end+1]==';') {
    // force update of INF_SRC if we append at the end of this object
    offset=1;
  }

  modified=true;

  if(code.IsEmpty()) {
    buffer=buffer.Left(begin)+buffer.Mid(end+1);
  } else {
    //wxLogDebug("%d %d %s\n", begin, end, code.c_str());
    buffer=buffer.Left(begin)+code+buffer.Mid(end+1);
  }

  unsigned int newSize=buffer.Length();
  int sizeDiff=newSize-prevSize;

  for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
    InformData *current=&frame->project.objects[i];
    if(sizeDiff>=0) {
      current->AdjustPointers(file, begin+1-offset, sizeDiff);
    } else {
      current->AdjustPointers(file, begin, sizeDiff);
    }
  }
}

wxString SourceFile::Name()
{
  return name;
}

wxString SourceFile::Path()
{
  return path;
}

wxString SourceFile::ToString()
{
  return path;
}

InformData *SourceFile::AppendObject(int type, const wxString& newCode)
{
  // Make sure that there is an empty line between objects
  wxString code=newCode;
//  wxLogDebug("%d", buffer[buffer.Length()-1]);
  if(buffer[buffer.Length()-1]!='\n' ||
     buffer[buffer.Length()-2]!='\n') code="\n"+newCode;

  unsigned int idx=buffer.Length();
  InsertCode(code, idx);
  Tokenize(idx, buffer.Length());
  InformData *newObject;
  if(type==INF_T_FUNCTION) {
    newObject=ParseFunction();
  } else {
    newObject=ParseObject();
  }
  //wxLogDebug("Appending %s %d %d\n", code.c_str(), newObject->type, type);
  newObject->type=type;
  frame->project.objects.Add(newObject);
  return newObject;
}

// return true if error found
bool SourceFile::CopyBufferToSrc()
{
  size_t len=buffer.Length()+1;
  if(src==NULL) {
    src=(char *) malloc(len);
    if(src==NULL) return true;
     currentSrcLen=len;
  } else if(len>currentSrcLen) {
    src=(char *) realloc(src, len);
    if(src==NULL) return true;
  }

  if(src==NULL) {
    wxFatalError("SourceFile::CopyBufferToSrc: alloc failed\n");
    return true;
  }

  srcModified=false;
  strcpy(src, buffer.c_str());
  return false;
}

// return true if error found
bool SourceFile::Parse() 
{
  unsigned int numTokens=Tokenize(0, buffer.Length());
  for(int i=0; i<MAXLEVEL; i++) parent[i]=NULL;

  ObjectData *obj;
  char filename[100];
  unsigned int value;
  unsigned int variable;

  unsigned int px=0, dx=numTokens/2;
  try {
    for(tokenIdx=0; tokenIdx<numTokens;) {
      int tokenId=ParseToken(tokenIdx);

      if(tokenIdx>px) {
			  px+=dx;
	      if(frame->project.progressDlg) {
	        frame->project.progressDlg->Update((100*tokenIdx)/numTokens, name);
				}
			}

      if(tokenId==TT_UNKNOWN) {
        // Check if it is a class name
        for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
          InformData *current=&frame->project.objects[i];
          if(current->type==INF_T_CLASS) {
            //InformData::Print();
            if(EQS(tokenIdx, current->GetSource(INF_ID))) {
              tokenId=TT_OBJECT;
              break;
            }
          }
        }
      }

      //PrintToken("TOP", tokenIdx);

      switch(tokenId) {
        case TT_ARRAY:
          ++tokenIdx; // arg1
          ++tokenIdx; // arg2
          ParseCommand(); // arg3
          ++tokenIdx; // ;
          break;
        case TT_VERB:
        case TT_EXTEND:
        case TT_ATTRIBUTE:
        case TT_REPLACE:
        case TT_PROPERTY:
        case TT_IFDEF:
        case TT_ENDIF:
        case TT_IFNDEF:
        case TT_GLOBAL:
        case TT_MESSAGE:
        case TT_SWITCHES:
        case TT_ABBREVIATE:
        case TT_RELEASE:
          while(NEQC(tokenIdx, ';')) ++tokenIdx;
          break;
        case TT_NEARBY:
          obj=ParseObject();
          //wxLogDebug("%s\n", parent[0]->GetSource(INF_SOURCE).c_str());
          obj->b[INF_PARENT]=parent[0]->b[INF_ID];
          obj->e[INF_PARENT]=parent[0]->e[INF_ID];
          //wxLogDebug("NEARBY %s\n", obj->GetSource(INF_PARENT).c_str());
          frame->project.objects.Add(obj);
          break;
        case TT_OBJECT:
          frame->project.objects.Add(ParseObject());
          break;
        case TT_CLASS:
          obj=ParseObject();
          obj->type=INF_T_CLASS;
          frame->project.objects.Add(obj);
          break;
        case TT_FUNCTION:
          frame->project.objects.Add(ParseFunction());
          break;
        case TT_INCLUDE:
          value=++tokenIdx; // file
          if(EQC(value, '\"') && EQC1(value, '>')) {
            strncpy(filename, &src[bToken[value]+2], sToken[value]-3);
            filename[sToken[value]-3]='\0';
            // Add source file if not already present
            if(frame->project.FindObject(filename)==NULL) {
              SourceFile *f=new SourceFile(); // deleted by Reset();
              frame->project.objects.Add(f);
              if(f->ReadSourceFile(filename)) return true;
            }
          }
          ++tokenIdx; // ;
          break;
        case TT_CONSTANT:
          variable=++tokenIdx; // variable
          value=++tokenIdx; // value or = or ;
          if(EQC(value, '=')) value=++tokenIdx;
          //PrintToken("variable", variable);
          //PrintToken("value", value);
          if(EQS(variable, "Story")) {
            if(file==0) {
              b[INF_STORY]=bToken[value];
              e[INF_STORY]=eToken[value];
            } else {
              wxLogMessage("Story must be defined in the main source file.\n");
            }
          }
          if(EQS(variable, "Headline")) {
            if(file==0) {
              b[INF_HEADLINE]=bToken[value];
              e[INF_HEADLINE]=eToken[value];
            } else {
              wxLogMessage("Headline must be defined in the main source file.\n");
            }
          }
          if(NEQC(value, ';')) ++tokenIdx;
          break;
        default:
          throw "Parse error";
          break;
      }
      ++tokenIdx;
    }
  } catch(const char *e) {
    wxLogMessage("%s: %s\n", buffer.SubString(bToken[tokenIdx], eToken[tokenIdx]).c_str(), e);
    return true;
  }

  // Tokenize may have modified to code in src to prevent
  // multiple statements on each line.
  if(srcModified) {
    buffer=src;
    srcModified=false;
  }

  return false;
}

ObjectData *SourceFile::ParseObject(ObjectData *obj)
{
  ObjectData *o;

  if(obj==NULL) {
    o=new ObjectData();
    o->b[INF_SOURCE]=bLine[lToken[tokenIdx]];
  } else {
    //wxLogDebug("Reparsing\n%s\n", obj->GetSource(INF_SOURCE).c_str());
    Tokenize(obj->b[INF_SOURCE], obj->e[INF_SOURCE]+1);
    for(int i=0; i<INF_SIZE && i!=INF_PARENT; i++) {
      obj->b[i]=0;
      obj->e[i]=0;
    }
    obj->b[INF_SOURCE]=bToken[0];
    o=obj;
  }

  //PrintToken("Object or Class or class name", tokenIdx);

  int level=0;
  bool isClass=false;
  if(EQS(tokenIdx, "Class")) {
    isClass=true;
  } else if(EQS(tokenIdx, "Nearby")) {
    level=1;
  } else if(NEQS(tokenIdx, "Object")) {
    // check if token is a class name
    for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
      InformData *current=&frame->project.objects[i];
      if(current->type==INF_T_CLASS) {
        if(EQS(tokenIdx, current->GetSource(INF_ID))) {
          o->b[INF_CLASS]=bToken[tokenIdx];
          o->e[INF_CLASS]=eToken[tokenIdx];
          //PrintToken("INF_CLASS", tokenIdx);
          break;
        }
      }
    }
  }

  o->file=file;

  o->mRoomX=-1;
  o->mRoomY=-1;

  ++tokenIdx; // ID or ->

  o->type=INF_T_ITEM;

  // Handle multiple ->
  while(EQC(tokenIdx, '-') && EQC1(tokenIdx, '>')) {
    if(o->b[INF_LEVEL]==0) o->b[INF_LEVEL]=bToken[tokenIdx];
    o->e[INF_LEVEL]=eToken[tokenIdx];
    ++level;
    ++tokenIdx;
  }

  if(level>=MAXLEVEL) level=MAXLEVEL-1;

  // token == ID or ;
  if(NEQC(tokenIdx, ';')) {
    o->b[INF_ID]=bToken[tokenIdx];
    o->e[INF_ID]=eToken[tokenIdx];
    //PrintToken("INF_ID", tokenIdx);

    if(!isClass) parent[level]=o;
    if(level>0 && obj==NULL && parent[level-1]!=NULL) {
      o->type=INF_T_ITEM;
      o->b[INF_PARENT]=parent[level-1]->b[INF_ID];
      o->e[INF_PARENT]=parent[level-1]->e[INF_ID];
      //wxLogDebug("Setting %s.location %s\n", o->GetSource(INF_ID).c_str(), o->GetSource(INF_PARENT).c_str());
    }

    ++tokenIdx; // "name" or class or with or has or ;
    if(EQS(tokenIdx, "class")) {
      ++tokenIdx; // class name
      o->b[INF_CLASS]=bToken[tokenIdx];
      o->e[INF_CLASS]=eToken[tokenIdx];
      //PrintToken("INF_CLASS", tokenIdx);
      ++tokenIdx; // , or with or has
      if(EQC(tokenIdx, ',')) ++tokenIdx; // with or has
    } else if(NEQS(tokenIdx, "with") && NEQS(tokenIdx, "has") && NEQC(tokenIdx, ';')) {
      // This is the name
      o->b[INF_NAME]=bToken[tokenIdx];
      o->e[INF_NAME]=eToken[tokenIdx];
      //PrintToken("INF_NAME", tokenIdx);

      ++tokenIdx; // ; or LOCATION or with or has
      if(NEQS(tokenIdx, "with") && NEQS(tokenIdx, "has") && NEQC(tokenIdx, ';')) {
        o->type=INF_T_ITEM;
        o->b[INF_PARENT]=bToken[tokenIdx];
        o->e[INF_PARENT]=eToken[tokenIdx];
        //PrintToken("INF_PARENT", tokenIdx);
        ++tokenIdx;
      }
    }
  }

  //wxLogDebug("OBJECT '%s' class '%s' name '%s': '%s'\n", o->GetSource(INF_ID).c_str(), o->classname.c_str(), o->GetSource(INF_NAME).c_str(), token.c_str());

  int i;
  unsigned int keyword;
  if(EQS(tokenIdx, "with")) {
    o->b[INF_WITH]=bToken[tokenIdx];
    o->e[INF_WITH]=eToken[tokenIdx];
    //PrintToken("INF_WITH", tokenIdx);

    ++tokenIdx;
    if(NEQS(tokenIdx, "has") && NEQC(tokenIdx, ';')) {
      for(;;) {
        keyword=tokenIdx;
        bStatement=bLine[lToken[tokenIdx]];

        if(EQC(tokenIdx, '[')) --tokenIdx;

        ParseCommand();

        //PrintCommand();
        //PrintStatement();

        if(EQS(keyword, "XY")) {
          o->type=INF_T_ROOM;
          o->mRoomX=atoi(&src[bToken[keyword+1]]);
          o->mRoomY=atoi(&src[bToken[keyword+2]]);
          o->b[INF_KAZAM_XY]=bCommand;
          o->e[INF_KAZAM_XY]=eCommand;
          o->rb[INF_KAZAM_XY]=bStatement;
          o->re[INF_KAZAM_XY]=eStatement;
          //wxLogDebug("Setting XY %d %d\n", o->mRoomX, o->mRoomY);
        }

        for(int i=INF_DIR_N; i<=INF_DIR_OUT; i++) {
          if(EQS(keyword, informKeyword[i])) {
            o->type=INF_T_ROOM;
            o->b[i]=bCommand;
            o->e[i]=eCommand;
            o->rb[i]=bStatement;
            o->re[i]=eStatement;
            break;
          }
        }

        if(EQS(keyword, "name")) {
          o->b[INF_WORDS]=bCommand;
          o->e[INF_WORDS]=eCommand;
          o->rb[INF_WORDS]=bStatement;
          o->re[INF_WORDS]=eStatement;
        }

        if(EQS(keyword, "description")) {
          o->b[INF_DESCRIPTION]=bCommand;
          o->e[INF_DESCRIPTION]=eCommand;
          o->rb[INF_DESCRIPTION]=bStatement;
          o->re[INF_DESCRIPTION]=eStatement;
        }

        if(EQS(keyword, "found_in")) {
          o->b[INF_FOUNDIN]=bCommand;
          o->e[INF_FOUNDIN]=eCommand;
          o->rb[INF_FOUNDIN]=bStatement;
          o->re[INF_FOUNDIN]=eStatement;
        }

        if(EQS(keyword, "with_key")) {
          o->b[INF_WITHKEY]=bCommand;
          o->e[INF_WITHKEY]=eCommand;
          o->rb[INF_WITHKEY]=bStatement;
          o->re[INF_WITHKEY]=eStatement;
        }

        if(EQS(keyword, "door_to")) {
          o->b[INF_DOORTO]=bCommand;
          o->e[INF_DOORTO]=eCommand;
          o->rb[INF_DOORTO]=bStatement;
          o->re[INF_DOORTO]=eStatement;
        }

        if(EQS(keyword, "door_dir")) {
          o->b[INF_DOORDIR]=bCommand;
          o->e[INF_DOORDIR]=eCommand;
          o->rb[INF_DOORDIR]=bStatement;
          o->re[INF_DOORDIR]=eStatement;
        }

        o->e[INF_WITH]=eToken[tokenIdx];
        ++tokenIdx; // has or ; or next keyword
        if(EQS(tokenIdx, "has") || EQC(tokenIdx, ';')) break;
      }
    }
  }

  if(EQS(tokenIdx, "has")) {
    o->b[INF_HAS]=bToken[tokenIdx];
    for(;;) {
      o->e[INF_HAS]=eToken[tokenIdx];
      ++tokenIdx; // attribute or ;
      if(EQC(tokenIdx, ';')) break;
      for(i=INF_A_ABSENT; i<=INF_A_WORN; i++) {
        if(EQS(tokenIdx, informKeyword[i])) {
          o->b[i]=bToken[tokenIdx];
          o->e[i]=eToken[tokenIdx];
          break;
        }
      }
      if(i>INF_A_WORN) {
        //TODO: PrintToken("WARNING: ignoring unknown attribute", tokenIdx);
      }
    }
  }

  o->e[INF_SOURCE]=eToken[tokenIdx];

  if(o->mRoomX>-1) o->type=INF_T_ROOM;
  if(!o->GetSource(INF_A_ANIMATE).IsEmpty()) o->type=INF_T_ACTOR;

  return o;		
}

FunctionData *SourceFile::ParseFunction(FunctionData *obj)
{
  unsigned int ID;
  unsigned int cmd;
  unsigned int arg1;
  unsigned int arg2;
  FunctionData *o;

  if(obj==NULL) {
    o=new FunctionData();
    o->b[INF_SOURCE]=bLine[lToken[tokenIdx]];
  } else {
    Tokenize(obj->b[INF_SOURCE], obj->e[INF_SOURCE]+1);
    for(int i=0; i<INF_SIZE; i++) {
      obj->b[i]=0;
      obj->e[i]=0;
    }
    obj->b[INF_SOURCE]=bToken[0];
    o=obj;
  }

  o->file=file;
  o->type=INF_T_FUNCTION;

  ID=++tokenIdx; // function name
  o->b[INF_ID]=bToken[ID];
  o->e[INF_ID]=eToken[ID];
  //PrintToken("INF_ID", ID);

  // Eat variable list
  do {
    ++tokenIdx;
  } while(NEQC(tokenIdx, ';'));

  // Eat function body
  int numPrint=0;
  do {
    // extract the first three tokens
    arg1=0;
    arg2=0;
    cmd=++tokenIdx;
    if(EQC(cmd, ']')) {
      --tokenIdx;
      break;
    }

    if(NEQC(cmd, ';')) {
      ++tokenIdx;
      if(EQC(tokenIdx, ']')) {
        --tokenIdx;
        break;
      }
      if(NEQC(tokenIdx, ';')) {
        arg1=tokenIdx;
        if(EQS(ID, "Initialise") && EQS(cmd, "print")) {
          ++numPrint;
          if(file==0) {
            b[INF_INITDESC]=bToken[tokenIdx];
            e[INF_INITDESC]=eToken[tokenIdx];
            //PrintToken("INF_INITDESC", tokenIdx);
          } else {
            wxLogDebug("WARNING: [Initialise; ] must be defined in the main source file.\n");
          }
        }
        ++tokenIdx;
        if(EQC(tokenIdx, ']')) {
          --tokenIdx;
          break;
        }
        if(NEQC(tokenIdx, ';')) {
          arg2=tokenIdx;
        }
      }
    }

    if(EQS(ID, "Initialise") && EQS(cmd, "location") && EQC(arg1, '=')) {
      if(file==0) {
        b[INF_STARTPOS]=bToken[tokenIdx];
        e[INF_STARTPOS]=eToken[tokenIdx];
        //PrintToken("INF_STARTPOS", tokenIdx);
      } else {
        wxLogDebug("WARNING: [Initialise; ] must be defined in the main source file.\n");
      }
    }

    if(EQS(ID, "Initialise") && EQS(cmd, "print") && EQC(arg2, ';')) {
      ++numPrint;
    }

    // Eat the rest until ;
    while(NEQC(tokenIdx, ';')) {
      ++tokenIdx;
    }

    //wxLogDebug("FUNCTION %s: %s %s %s\n", ID.c_str(), cmd.c_str(), arg1.c_str(), arg2.c_str());

    // Peek on the next token
  } while(NEQC(tokenIdx+1, ']'));

  if(numPrint!=1) {
    // Too complex init description
    b[INF_INITDESC]=0;
    e[INF_INITDESC]=0;
  }

  ++tokenIdx; // ]
  ++tokenIdx; // ;

  o->e[INF_SOURCE]=eToken[tokenIdx];
  return o;
}

void SourceFile::ParseCommand()
{
  // Eat the whole command, including ,
  ++tokenIdx; // [ or "string" or token
  bCommand=bToken[tokenIdx];

  eCommand=0;
  eStatement=0;

  if(EQC(tokenIdx, '[')) {
    while(NEQC(tokenIdx, ']')) ++tokenIdx;
    eCommand=eToken[tokenIdx];
    ++tokenIdx; // , or ;
  } else {
    while(NEQC(tokenIdx, ',') && NEQC(tokenIdx, ';')) {
      eCommand=eToken[tokenIdx];
      ++tokenIdx;
      eStatement=eToken[tokenIdx];
    }
  }

  if(NEQC(tokenIdx, ',')) {
    eStatement=eCommand;
    --tokenIdx; // leave ; in place
  } else {
    eStatement=eLine[lToken[tokenIdx]];
  }
}

int SourceFile::ParseToken(unsigned int tokenIdx)
{
  int s=sToken[tokenIdx];
  char *t=&src[bToken[tokenIdx]];

  for(int i=0; tokens[i][(unsigned int) 0]!='\0'; i++) {
    if(strncasecmp(tokens[i], t, s)==0) {
      return i;
    }
  }
  return TT_UNKNOWN;
}

// if breakLine=true SkipLine will try to break lines with
// multiple statements into seperate lines that are easier to parse
void SourceFile::SkipLine(unsigned int& idx, bool breakLine)
{
  while(src[idx]!='\n') {
    if(breakLine && src[idx]!=' ') {
      if(src[idx-1]==' ') {
        src[--idx]='\n';
        srcModified=true;
      } else {
        return;
      }
    } else {
      ++idx;
    }
  }

  eLine[lineno]=idx;
  ++lineno;
  ++idx;
  bLine[lineno]=idx;
}

// Divide the source file into tokens.
unsigned int SourceFile::Tokenize(unsigned int startIdx, unsigned int stopIdx)
{
  char c;
  bool commentFound;

  if(CopyBufferToSrc()) return 0;

  unsigned int idx=startIdx;
  tokenIdx=0;
  bToken[0]=idx;
  if(startIdx==0) lineno=0;

  for(int i=0; ; i++) {
    // Skip intial whitespace
    do {
      c=src[idx];
      commentFound=false;
      while(idx<stopIdx) {
        if(c=='\n') {
          eLine[lineno]=idx;
          ++lineno;
          bLine[lineno]=idx+1;
          c=src[++idx];
        } else if(c==' ' || c=='\r' || c=='\t') {
          c=src[++idx];
        } else break;
      }

      if(c=='!') {
        commentFound=true;
        if(src[idx+1]=='@') {
          // This is a Kazam command
          idx+=2;
        } else {
          SkipLine(idx);
        }
      }
    } while(commentFound);

    if(idx>=stopIdx) {
      // End of file
      return i;
    }

    unsigned int start=idx;
    bool stop=false;

    if(src[idx]=='\"') {
      // This is a string. It could span multiple lines but can't be nested
      ++idx;
      while(idx<stopIdx && src[idx]!='\"') {
        if(src[idx]=='\n') {
          eLine[lineno]=idx;
          ++lineno;
          bLine[lineno]=idx+1;
        }
        ++idx;
      }
      ++idx;
      stop=true;
    }

    // Read until no more white space
    while(!stop && idx<stopIdx) {
      c=src[idx];
      if(c==' ' || c=='\r' || c=='\t' || c=='\n') break;
      // Check delimiters
      if(c==';') {
        stop=true;
        if(start==idx) ++idx;
      } else if(c=='-' && src[idx+1]=='-' && src[idx+2]=='>') {
        stop=true;
        if(start==idx) idx+=3;
      } else if(c=='-' && src[idx+1]=='>') {
        stop=true;
        if(start==idx) idx+=2;
      } else if(c=='[') {
        stop=true;
        if(start==idx) ++idx;
      } else if(c==']') {
        stop=true;
        if(start==idx) ++idx;
      } else if(c==',') {
        stop=true;
        if(start==idx) ++idx;
      } else if(c=='=') {
        stop=true;
        if(start==idx) ++idx;
      } else if(c=='<') {
        stop=true;
        if(start==idx) ++idx;
      } else if(c=='>') {
        stop=true;
        if(start==idx) ++idx;
      }
      if(!stop) ++idx;		
    }

    if(i>=MAXTOKENS) {
      wxLogFatalError("MAXTOKENS too small");
    }
    if(lineno>=MAXLINES) {
      wxLogFatalError("MAXLINES too small");
    }

    bToken[i]=start;
    eToken[i]=idx-1;
    sToken[i]=idx-start;
    lToken[i]=lineno;

    //TODO: check if this does works (breaks multiple statement lines)
    if(src[start]==',') SkipLine(idx, true);

    //PrintToken(i);

  } /* for(i */
}

void SourceFile::PrintToken(char *msg, unsigned int idx)
{
  char token[300];
  int s=sToken[idx];
  if(s>299) s=299;
  strncpy(token, &src[bToken[idx]], s);
  token[s]='\0';
  wxLogDebug("%s: (%d/%d) '%s'\n", msg, idx, lToken[idx], token);
}
