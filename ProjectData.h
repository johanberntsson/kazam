#ifndef _PROJECTDATA_H
#define _PROJECTDATA_H

#include "InformData.h"

#include "ObjectData.h"
#include "FunctionData.h"
#include "SourceFile.h"

class ProjectData : public InformData
{
public:
  ProjectData(void);
  ~ProjectData(void);

  void Reset();
  void SetIndent(int size, bool useTabs);
  static wxString InformStringToC(const wxString& informString);
  static wxString CStringToInform(const wxString& cString, int width=-1, int indentLevel=1);

  ObjectData *AddObject(const wxString& code);
  void RemoveObject(ObjectData *object);
  void UpdateObject(ObjectData *object);

  FunctionData *AddFunction(const wxString& code);
  void RemoveFunction(FunctionData *object);
  void UpdateFunction(FunctionData *object);

  SourceFile *AddSourceFile(const wxString& name);
  void RemoveSourceFile(int fileno);
  void UpdateSourceFile(int fileno);

  // if -1, get last source file
  SourceFile *GetSourceFile(int id);

  bool IsModified();
  bool Load(const wxString& path);
  bool Save();

  bool Compile();
  bool Execute();

  void InitComboRoom(wxComboBox *combo, const wxString& location);
  void InitComboClass(wxComboBox *combo, const wxString& classname);

  ObjectData *FindRoom(int x, int y);
  ObjectData *FindRoom(const wxString& id);

  InformDataList objects;
  SourceFileList sourcefiles;

  wxString indent; // set by setIndent()
};

#endif
