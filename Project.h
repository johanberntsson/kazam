#ifndef _PROJECT_H
#define _PROJECT_H

#include <wx/progdlg.h>

#include "InformData.h"

class SourceFile;
class ObjectData;
class FunctionData;

class Project
{
public:
  Project(void);
  ~Project(void);

  void Test();

  void Reset();
  void SetIndent(int size, bool useTabs);
  wxString indent; // set by setIndent()

  InformData *AddObject(const wxString& ID, int type, bool changeSelected=true, int file=-1, int mx=-1, int my=-1);
  InformData *FindObject(const wxString& id, int type=-1);
  void DeleteObject(InformData *object);
  void DeleteObjects(unsigned int fileno);
  void MoveObject(InformData *object, const wxString& newLocation);

  bool UpdateAll(wxTreeEvent *event, InformData *currObj=NULL);

  bool IsUniqueID(const wxString& ID, wxWindow *errorDlgParent=NULL);

  SourceFile *GetSourceFile(unsigned int id);
  SourceFile *GetLastSourceFile();

  bool IsModified();
  bool Save();
  bool Load(const wxString& path);

  bool Compile();
  bool Execute();

  ObjectData *FindRoom(const wxString& id);
  ObjectData *FindRoom(int x, int y);
  void FindFreeRoomPosition(int *xc, int *yc);
  int GetLevel(ObjectData *o);

  InformDataArray objects;
  int fileCnt;

  void ResetSourceFileCache();

  wxProgressDialog *progressDlg;

private:
#define MAX_SOURCE_CACHE 20
  int numSrc;
  SourceFile *src[MAX_SOURCE_CACHE];
};

#endif
