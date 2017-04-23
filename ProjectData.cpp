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

#ifndef __WXGTK__
#include <wx/filename.h>
#endif

#include "ProjectData.h"

#include "kazam.h"
extern MainFrame *frame;

ProjectData::ProjectData(void)
{
  objects.DeleteContents(true);
  sourcefiles.DeleteContents(true);
}

ProjectData::~ProjectData(void)
{
  Reset();
}

// Remove all data
void ProjectData::Reset()
{
  printf("ProjectData::Reset\n");

  for(int i=0; i<INF_SIZE; i++) {
    b[i]=0;
    e[i]=0;
  }

  objects.Clear();
  sourcefiles.Clear();
}

void ProjectData::InitComboRoom(wxComboBox *combo, const wxString& location)
{
  combo->Clear();
  for(InformDataList::Node *node=frame->project.objects.GetFirst(); node; node=node->GetNext()) {
    InformData *current=node->GetData();
    if(current->type==ObjectData::ROOM) {
      combo->Append(current->GetSource(INF_ID));
    }
  }
  combo->SetValue(location);
}

void ProjectData::InitComboClass(wxComboBox *combo, const wxString& classname)
{
  combo->Clear();
  combo->Append("Object");
  for(InformDataList::Node *node=frame->project.objects.GetFirst(); node; node=node->GetNext()) {
    InformData *current=node->GetData();
    if(current->type==ObjectData::CLASS) {
      combo->Append(current->GetSource(INF_ID));
    }
  }
  if(classname.IsEmpty()) {
    combo->SetSelection(0);
  } else {
    combo->SetValue(classname);
  }
}

ObjectData *ProjectData::AddObject(const wxString& code)
{
  ObjectData *obj=GetSourceFile(-1)->AppendObject(code);
  frame->objectTree->AddNode(obj);
  return obj;
}

void ProjectData::RemoveObject(ObjectData *object)
{
  GetSourceFile(object->file)->RemoveCode(object->b[INF_SOURCE], object->e[INF_SOURCE]);
  frame->objectTree->RemoveNode(object->treeId);
  objects.DeleteObject(object);
  delete object;
}

void ProjectData::UpdateObject(ObjectData *object)
{
  GetSourceFile(object->file)->ParseObject(object);

  // To update the label
  frame->objectTree->UpdateNode(object->treeId, object);
}

FunctionData *ProjectData::AddFunction(const wxString& code)
{
  return NULL;
}

void ProjectData::RemoveFunction(FunctionData *object)
{
}

void ProjectData::UpdateFunction(FunctionData *object)
{
}


SourceFile *ProjectData::AddSourceFile(const wxString& name)
{
  return NULL;
}

void ProjectData::RemoveSourceFile(int fileno)
{
}

void ProjectData::UpdateSourceFile(int fileno)
{
  //TODO: now we remove all objects, while we only should remove
  // the objects that belong to this file. We should also reinsert
  // objects at the same place when we reparse the file.
  // The current approach will break when more than one source
  // file is used.
  SourceFile *src=GetSourceFile(fileno);

  // Remove old objects
  objects.Clear();

  src->Parse();
}

SourceFile *ProjectData::GetSourceFile(int id)
{
  int i=0;
  SourceFile *current=NULL;

  for(SourceFileList::Node *node=sourcefiles.GetFirst(); node; node=node->GetNext()) {
    current=node->GetData();
    if(id==i) return current;
    ++i;
  }
  return current;
}
void ProjectData::SetIndent(int size, bool useTabs)
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
wxString ProjectData::InformStringToC(const wxString& informString)
{
  unsigned int i, j;
  wxString cString=informString;

	for(;;) {
	  i=cString.Find('\\');
		if(i==-1) break;
	  printf("%d\n", i);
	  j=i+1;
	  while(cString[j]==' ' || cString[j]=='\n') ++j;
		cString=cString.Left(i)+cString.Mid(j);
	}

  cString.Replace("~", "\"", TRUE);
  cString.Replace("^", "\n", TRUE);

  return cString.Mid(1, cString.Length()-2);
}

wxString ProjectData::CStringToInform(const wxString& cString, int width, int indentLevel)
{
  wxString informString=cString;
  informString.Replace("\"", "~", TRUE);
  informString.Replace("\n", "^", TRUE);
  return "\""+informString+"\"";
}

// true if error
bool ProjectData::Load(const wxString& path) 
{
  Reset();
  SourceFile *f=new SourceFile(); // deleted by Reset();
  sourcefiles.Append(f);
  frame->filehistory->AddFileToHistory(path); // add to MRU
  return f->ReadSource(path, 0);
}

// true if error
bool ProjectData::Save()
{
  bool res=false;
  for(SourceFileList::Node *node=sourcefiles.GetFirst(); node; node=node->GetNext()) {
    SourceFile *current=node->GetData();
    if(current->GetModified()) {
      res|=current->SaveSource();
    }
  }
  return res;
}

bool ProjectData::IsModified()
{
  bool res=false;
  for(SourceFileList::Node *node=sourcefiles.GetFirst(); node; node=node->GetNext()) {
    SourceFile *current=node->GetData();
    res|=current->GetModified();
  }
  return res;
}

ObjectData *ProjectData::FindRoom(int x, int y)
{
  for(InformDataList::Node *node=objects.GetFirst(); node; node=node->GetNext()) {
    InformData *current=node->GetData();
    if(current->type==ObjectData::ROOM) {
      ObjectData *room=(ObjectData *) current;
      if(room->mRoomX==x && room->mRoomY==y) return room;
    }
  }
  return NULL;
}

ObjectData *ProjectData::FindRoom(const wxString& id)
{
  for(InformDataList::Node *node=objects.GetFirst(); node; node=node->GetNext()) {
    InformData *current=node->GetData();
    if(current->type==ObjectData::ROOM) {
      ObjectData *room=(ObjectData *) current;
      if(id.IsSameAs(room->GetSource(INF_ID))) return room;
    }
  }
  return NULL;
}

// true if error
bool ProjectData::Compile()
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
bool ProjectData::Execute()
{
  wxString name;
#ifndef __WXGTK__
  wxFileName::SplitPath(GetSourceFile(0)->Path(), NULL, NULL, &name, NULL);
#else
  name=GetSourceFile(0)->Path();
  name=name.Left(name.Length()-4);
#endif
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
