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

#include <wx/timer.h>

#include "Project.h"
#include "ObjectData.h"
#include "SourceFile.h"

#include "kazam.h"
extern MainFrame *frame;

Project::Project(void)
{
  progressDlg=NULL;
  ResetSourceFileCache();
}

Project::~Project(void)
{
  if(progressDlg) delete progressDlg;
}

#define TEST2
void Project::Test()
{
  Reset();
  SourceFile *src=(SourceFile *) AddObject("file", INF_T_SOURCEFILE, false);
#ifdef TEST1
  src->ReplaceCode(
    "\n\n"
    "Object Test1;\n\n"
    "Object Test2;\n\n"
    "Object Test3;\n\n"
    "Object Test4;\n\n"
    , 21, 20);
#endif
#ifdef TEST2
  src->ReplaceCode(
    "\n\n"
    "Object Test1 \"test\"\n  with\n    number 0;\n\n"
    "Object Test2 \"test\"\n  with\n    number 0;\n\n"
    "Object Test3 \"test\"\n  with\n    number 0;\n\n"
    "Object Test4 \"test\"\n  with\n    number 0;\n\n"
    , 21, 20);
#endif
  src->Parse();

  InformData *t1=FindObject("Test1");
  InformData *t2=FindObject("Test2");
  InformData *t3=FindObject("Test3");
  InformData *t4=FindObject("Test4");

  t1->SetSource(INF_A_GENERAL, "general");

  t2->SetSource(INF_DESCRIPTION, "This is a test");

  t3->SetSource(INF_DESCRIPTION, "This is a test");
  t3->SetSource(INF_A_GENERAL, "general");

  t4->SetSource(INF_A_GENERAL, "general");
  t4->SetSource(INF_A_SCENERY, "scenery");
  t4->SetSource(INF_DESCRIPTION, "This is a test");
  t4->Update();
  t4->SetSource(INF_DESCRIPTION, "");
  t4->SetSource(INF_A_SCENERY, "");

  frame->objectTree->BuildTree("file");
}

// Remove all data
void Project::Reset()
{
	fileCnt=0;
  objects.Clear();
  ResetSourceFileCache();
}

InformData *Project::AddObject(const wxString& ID, int type, bool changeSelected, int file, int mx, int my)
{
  wxString src;
  wxString name=ID.Lower();
  wxString Name=name.Left(1).Upper()+name.Mid(1);

  switch(type) {
  case INF_T_SOURCEFILE:
    // This is taken care of by CreateSourceFile below
    break;
  case INF_T_ROOM:
    src=wxString::Format(
      "Object $ID \"$Name\"\n"
      "\twith\n"
      "\t\t!@ XY %d %d\n"
      "\t\tdescription \"$name\",\n"
      "\thas light;\n",
      mx, my);
    break;
  case INF_T_ACTOR:
    src="Object $ID \"$name\"\n"
        "\twith\n"
        "\t\tname \"$name\",\n"
        "\t\tdescription \"$name\",\n"
        "\thas animate;\n";
    break;
  case INF_T_ITEM:
    src="Object $ID \"$name\"\n"
        "\twith\n"
        "\t\tname \"$name\",\n"
        "\t\tdescription \"$name\",\n"
        "\thas;\n";
    break;
  case INF_T_CLASS:
    src="Class $ID\n"
        "\twith\n"
        "\thas;\n";
    break;
  case INF_T_FUNCTION:
    src="[ $ID;\n"
        "];\n";
    break;
  default:
    wxLogDebug("ERROR: TODO: missing type in Project::AddObject");
  }  

  src.Replace("$ID", ID, true);
  src.Replace("$Name", Name, true);
  src.Replace("$name", name, true);
  src.Replace("\t", indent, true);

  InformData *obj;
  if(type==INF_T_SOURCEFILE) {
    obj=new SourceFile();
    objects.Add(obj);
    if(((SourceFile *) obj)->CreateSourceFile(ID)) {
      wxLogWarning("Source file creation/parse failed");
      objects.Remove(obj);
      return NULL;
    }
  } else {
    SourceFile *srcfile=NULL;
    if(file==-1) srcfile=GetLastSourceFile(); else srcfile=GetSourceFile(file);
    if(srcfile==NULL) {
      wxLogError("Add failed: no source file found");
      return NULL;
    }
    obj=srcfile->AppendObject(type, src);
  }

  obj->treeId=frame->objectTree->GetRootItem();
  frame->objectTree->AddNode(obj, changeSelected);
  return obj;
}

int Project::GetLevel(ObjectData *o)
{
  int i=0;
  wxString loc;
  for(;;) {
    loc=o->GetSource(INF_PARENT);
    if(loc.IsEmpty()) return i;
    ++i;
    o=(ObjectData *) FindObject(loc);
  }
}

void Project::MoveObject(InformData *object, const wxString& newLocation)
{
  InformData *target=FindObject(newLocation);
  if(target==NULL) return;

  // Cannot move between classes, functions etc.
  if(object->type!=INF_T_ROOM && object->type!=INF_T_ACTOR && object->type!=INF_T_ITEM) return;
  if(target->type!=INF_T_ROOM && target->type!=INF_T_ACTOR && target->type!=INF_T_ITEM) return;

  ObjectData *o=(ObjectData *) object;
  ObjectData *t=(ObjectData *) target;

  // Check if there are any relative location references to object
  wxString id=o->GetSource(INF_ID);
  for(unsigned int i=0; i<objects.GetCount(); i++) {
    InformData *current=&objects[i];
    if(current->type!=INF_T_ROOM && current->type!=INF_T_ITEM && current->type!=INF_T_ACTOR) continue;
    ObjectData *obj=(ObjectData *) current;
    if(obj->GetSource(INF_PARENT).IsSameAs(id)) {
      // TODO: recursive move of all children as well
      wxMessageBox("This object contains other objects that has to be moved before moving it", "Sorry");
      return;
    }
  }

  if(o->e[INF_LEVEL]>0) {
    // This is -> or nearby type of location reference
    int sl=GetLevel(o);
    int tl=GetLevel(t);

    if((sl-1)!=tl) {
      // adjust -> string
      wxString depth="->";
      for(int i=0; i<tl; i++) depth+=" ->";
      o->SetSource(INF_LEVEL, depth);
      //wxLogDebug("Setting depth %s\n", depth.c_str());
    }

    // Get a copy of the code
    wxString code="\n"+o->GetSource(INF_SOURCE);
    //wxLogDebug("o %d %d '%s'\n", o->b[INF_SOURCE], o->e[INF_SOURCE], code.c_str());
    //wxLogDebug("t %d %d\n", t->b[INF_SOURCE], t->e[INF_SOURCE]);

    // Remove the source code
    GetSourceFile(o->file)->ReplaceCode("", o->b[INF_SOURCE], o->e[INF_SOURCE]+1);
    //wxLogDebug("o %d %d\n", o->b[INF_SOURCE], o->e[INF_SOURCE]);

    // Insert code after target object
    GetSourceFile(t->file)->InsertCode(code, t->e[INF_SOURCE]+1);

    int offset=(t->e[INF_SOURCE]+2)-o->b[INF_SOURCE];
    o->file=t->file;
    //wxLogDebug("o %d %d\n", o->b[INF_SOURCE], o->e[INF_SOURCE]);
    //wxLogDebug("t %d %d\n", t->b[INF_SOURCE], t->e[INF_SOURCE]);
    o->AdjustPointers(t->file, o->b[INF_SOURCE], offset);
    //wxLogDebug("offset %d\n", offset);
    //wxLogDebug("o %d %d\n", o->b[INF_SOURCE], o->e[INF_SOURCE]);
    //wxLogDebug("t %d %d\n", t->b[INF_SOURCE], t->e[INF_SOURCE]);

    // Update to the new parent
    o->b[INF_PARENT]=t->b[INF_ID];
    o->e[INF_PARENT]=t->e[INF_ID];

    // Force rebuild of the object browser
    t->SetDirty();
    //wxLogDebug("%s new location %s\n", o->GetSource(INF_ID).c_str(), o->GetSource(INF_PARENT).c_str());
  } else {
    // This is a direct reference
    o->SetSource(INF_PARENT, newLocation);
  }
}

void Project::DeleteObject(InformData *object)
{
  //wxLogDebug("Deleting %s %d\n", object->GetSource(INF_ID).c_str(), object->type);
  unsigned int i;
  bool roomModified;

  // Check if there are any relative location references to object
  wxString id=object->GetSource(INF_ID);
  for(i=0; i<objects.GetCount(); i++) {
    InformData *current=&objects[i];
    if(current->type!=INF_T_ROOM && current->type!=INF_T_ITEM && current->type!=INF_T_ACTOR) continue;
    ObjectData *obj=(ObjectData *) current;
    if(obj->GetSource(INF_PARENT).IsSameAs(id)) {
      // TODO: recursive move of all children as well
      wxMessageBox("This object contains other objects that has to be moved before deleting it", "Sorry");
      return;
    }
  }

  if(object->type==INF_T_ROOM) {
    // Find and remove all connections to this room
    for(i=0; i<objects.GetCount(); i++) {
      InformData *current=&objects[i];
      if(current->type==INF_T_ROOM && current!=object) {
        roomModified=false;
        for(int i=0; i<=12; i++) {
          if(FindRoom(current->GetSource(i+INF_DIR_N))==object) {
            // there is a connection from current to objects
            roomModified=true;
            current->SetSource(i+INF_DIR_N, "");
          }
        }
        if(roomModified) current->Update();
      }
    }
  }

  // Find another item of the same category
  wxString newLabel="";
  for(i=0; i<objects.GetCount(); i++) {
    InformData *current=&objects[i];
    if(current!=object && current->type==object->type) {
      newLabel=frame->objectTree->GetItemText(current->treeId);
      break;
    }
  }

  object->SetSource(INF_SOURCE, "");
  if(object->type==INF_T_SOURCEFILE) ResetSourceFileCache();
  objects.Remove(object);
  frame->objectTree->BuildTree(newLabel);
}

void Project::DeleteObjects(unsigned int fileno)
{
  for(unsigned int i=0; i<objects.GetCount(); ) {
    InformData *current=&objects[i];
    if(current->type!=INF_T_SOURCEFILE && current->file==fileno) {
      objects.Remove(current);
    } else {
      ++i;
    }
  }
}

bool Project::UpdateAll(wxTreeEvent *event, InformData *currObj)
{
  // true if currObj updated
  int oldType;
  int updated=0;
  bool rebuild=false;
  bool currUpdated=false;

  for(unsigned int i=0; i<objects.GetCount(); i++) {
    InformData *current=&objects[i];
    if(current->IsDirty()) {
      ++updated;
      if(current==currObj) currUpdated=true;
      oldType=current->type;
      current->Update();
      if(oldType!=current->type) {
        // We must rebuild the tree because catagories have changed
        rebuild=true;
      }
    }
  }

  wxString label="";
  if(currObj) {
    label=currObj->GetSource(INF_ID);
    if(currUpdated && updated==1) {
      // Only the active object was updated
    } else if(updated>0) {
      // Other objects were updated
      rebuild=true;
    }
  } else {
    // No active project (updating source or similar)
    if(updated>0) {
      rebuild=true;
    }
  }

  if(event!=NULL) {
    label=frame->objectTree->GetItemText(event->GetItem());
    if(rebuild) event->Veto();
  }

  if(rebuild) {
    frame->objectTree->BuildTree(label);
  } else if(currObj!=NULL) {
    frame->objectTree->UpdateNode(currObj->treeId, currObj);
  }

  return currUpdated;
}

bool Project::IsUniqueID(const wxString& ID, wxWindow *errorDlgParent)
{
  // TODO: also check if same as Inform keyword
  bool unique=(!ID.IsEmpty() && FindObject(ID)==NULL);

  if(!unique && errorDlgParent!=NULL) {
    wxMessageBox("The ID is not unique. Please enter another.",
      "Object ID error", wxOK, errorDlgParent);
  }

  return unique;
}

void Project::ResetSourceFileCache()
{
  // Should be called when a source file has been added or deleted!
  numSrc=-1;
}

SourceFile *Project::GetSourceFile(unsigned int id)
{
  // return file #id, or last file not found
  if(numSrc==-1) {
    //wxLogDebug("Initiating source file cache\n");
    numSrc=0;
    for(unsigned int i=0; i<objects.GetCount(); i++) {
      InformData *current=&objects[i];
      if(current->type==INF_T_SOURCEFILE) {
        if(numSrc==MAX_SOURCE_CACHE) {
          wxLogFatalError("MAX_SOURCE_CACHE too small\n");
          break;
        }
        src[numSrc++]=(SourceFile *) current;
      }
    }
  }

  for(int i=0; i<numSrc; i++) {
    if(src[i]->file==id) {
      return src[i];
    }
  }

  if(numSrc>0) return src[numSrc-1];
  return NULL;
}

SourceFile *Project::GetLastSourceFile()
{
  if(numSrc>0) return src[numSrc-1];
  return GetSourceFile(MAX_SOURCE_CACHE);
}

void Project::SetIndent(int size, bool useTabs)
{
  indent="";
  for(int i=0; i<size; i++) {
    if(useTabs) {
      indent+="\t";
    } else {
      indent+=" ";
    }
  }
}

// true if error
bool Project::Load(const wxString& path) 
{
  // Remove selection so that SaveData won't be called
  // with references to old (deleted) objects
  frame->objectTree->UnselectAll();

  Reset();
  SourceFile *f=new SourceFile(); // deleted by Reset();
  objects.Add(f);

  frame->filehistory->AddFileToHistory(path); // add to MRU

  progressDlg=new wxProgressDialog("Loading ", "Loading...");

  //wxStopWatch sw;
  bool res=f->ReadSourceFile(path);
  //wxLogDebug("ReadSourceFile took %ldms to execute", sw.Time());

  delete progressDlg;
  progressDlg=NULL;

  return res;
}

// true if error
bool Project::Save()
{
  bool res=false;

  for(unsigned int i=0; i<objects.GetCount(); i++) {
    InformData *current=&objects[i];
    if(current->type==INF_T_SOURCEFILE) {
      SourceFile *src=(SourceFile *) current;
      if(src->IsModified()) {
        res|=src->SaveSourceFile();
      }
    }
  }

  return res;
}

bool Project::IsModified()
{
  bool res=false;

  for(unsigned int i=0; i<objects.GetCount(); i++) {
    InformData *current=&objects[i];
    if(current->type==INF_T_SOURCEFILE) {
      SourceFile *src=(SourceFile *) current;
      res|=src->IsModified();
    }
  }

  return res;
}

void Project::FindFreeRoomPosition(int *xc, int *yc)
{
  bool found;
  int x, y, k;

  for(k=1;;k++) {
    for(x=0; x<k; x++) {
      for(y=0; y<k; y++) {
        found=false;
        for(unsigned int i=0; i<objects.GetCount(); i++) {
          InformData *current=&objects[i];
          if(current->type==INF_T_ROOM) {
            ObjectData *room=(ObjectData *) current;
            if(room->mRoomX==x && room->mRoomY==y) found=true;
          }
        }
        if(!found) {
          *xc=x;
          *yc=y;
          return;
        }
      }
    }
  }
}

ObjectData *Project::FindRoom(int x, int y)
{
  for(unsigned int i=0; i<objects.GetCount(); i++) {
    InformData *current=&objects[i];
    if(current->type==INF_T_ROOM) {
      ObjectData *room=(ObjectData *) current;
      if(room->mRoomX==x && room->mRoomY==y) return room;
    }
  }

  return NULL;
}

InformData *Project::FindObject(const wxString& id, int type)
{
  for(unsigned int i=0; i<objects.GetCount(); i++) {
    InformData *current=&objects[i];
    if(current->type==type || type==-1) {
      if(id.IsSameAs(current->GetSource(INF_ID))) return current;
    }
  }
  return NULL;
}

ObjectData *Project::FindRoom(const wxString& id)
{
  return (ObjectData *) FindObject(id, INF_T_ROOM);
}

// true if error
bool Project::Compile()
{
  if(IsModified()) Save();

  wxString cmd="\""+frame->cmdCompiler+"\" +\""+frame->cmdLibrary+"\" "+GetSourceFile(0)->Name();
  wxString msg="Compiling: "+cmd+"\n\n";
  frame->logWindow->AppendText(msg);

  wxArrayString output;
  long status=wxExecute(cmd, output);

  msg="";
	unsigned int i;
  for(i=0; i<output.GetCount() && i<20; i++) {
    msg+=output[i]+"\n";
  }
  frame->logWindow->AppendText(msg);

  if(output.GetCount()>i) {
    frame->logWindow->AppendText("[Compiler output truncated]\n");
  }

  if(status!=0) {
    frame->logWindow->AppendText("\nCompilation failed\n");
    if(status==-1) frame->logWindow->AppendText("Check Compiler and Library settings in Edit/Preferences\n");
    return true;
  }

  frame->logWindow->AppendText("\nCompilation successful!\n");
  return false;
}

// true if error
bool Project::Execute()
{
  wxString name;
  ::wxSplitPath(GetSourceFile(0)->Path(), NULL, &name, NULL);
  wxString file=::wxFindFirstFile(name+".z*");

  if(file.IsEmpty()) {
    Compile();
    file=::wxFindFirstFile(name+".z*");
    if(file.IsEmpty()) {
      frame->logWindow->AppendText("No Inform data file found.");
      return true;
    }
  }

  wxString cmd="\""+frame->cmdExecute+"\" "+file;
  frame->logWindow->AppendText("Executing: "+cmd+"\n");

  long status=wxExecute(cmd);

  if(status==0) {
    frame->logWindow->AppendText("Failed: Check Terp settings in Edit/Preferences\n");
    return true;
  }

  return false;
}
