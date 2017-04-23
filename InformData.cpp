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

#include "InformData.h"
#include "SourceFile.h"
#include "ObjectData.h"
#include "FunctionData.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(InformDataArray);

#include "kazam.h"
extern MainFrame *frame;

wxString informKeyword[INF_SIZE]={
  "", "", "", "", "",
  "n_to", "s_to", "e_to", "w_to",
  "ne_to", "nw_to", "se_to", "sw_to",
  "u_to", "d_to", "in_to", "out_to",
  "", "", "location=", "print",
  "description", "absent",
  "animate", "clothing", "concealed", "container", "door", "edible",
  "enterable", "female", "general", "light", "lockable", "locked",
  "moved", "neuter", "on", "open", "openable", "pluralname", "proper",
  "scenery", "scored", "static", "supporter", "switchable", "talkable",
  "transparent", "visited", "workflag", "worn",
  "", "", "name", "", "!@ XY", "found_in", "door_to", "door_dir",
  "with_key"
};

InformData::InformData()
{
  dirty=false;
  for(int i=0; i<INF_SIZE; i++) {
    b[i]=0;
    e[i]=0;
  }
}

InformData::~InformData()
{
//  if(type==INF_T_SOURCEFILE) ~SourceFile();
//  if(src) free(src);
}

wxString InformData::ToString()
{
  return "InformData";
}

bool InformData::IsDirty()
{
  return dirty;
}

void InformData::SetDirty()
{
//  wxLogDebug("%s DIRTY\n", GetSource(INF_ID).c_str());
  dirty=true;
}

void InformData::Update()
{
  if(!dirty) return;

  int i;
  dirty=false;
  switch(type) {
  case INF_T_ROOM:
  case INF_T_ACTOR:
  case INF_T_ITEM:
  case INF_T_CLASS:
    frame->project.GetSourceFile(file)->ParseObject((ObjectData *) this);
    for(i=0; i<12; i++) {
      ((ObjectData *) this)->dirCache[i].ok=false;
    }
    break;
  case INF_T_FUNCTION:
    frame->project.GetSourceFile(file)->ParseFunction((FunctionData *) this);
    break;
  case INF_T_SOURCEFILE:
    frame->project.DeleteObjects(file);
    frame->project.GetSourceFile(file)->Parse();
    break;
  default:
    wxLogDebug("ERROR: TODO: missing type in InformData::Update");
  }  
}

void InformData::Print()
{
  wxLogDebug("Inform objects list %d\n", frame->project.objects.GetCount());

  for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
    InformData *current=&frame->project.objects[i];
    wxLogDebug("Type %d, ID '%s'\n", current->type, current->GetSource(INF_ID).c_str());
  }
}

void InformData::AdjustPointers(unsigned int file, unsigned int start, unsigned int offset)
{
  if(this->file!=file) return;
  for(int i=0; i<INF_SIZE; i++) {
    if(b[i]>=start) b[i]+=offset;
    if(e[i]>=start) e[i]+=offset;
    if(rb[i]>=start) rb[i]+=offset;
    if(re[i]>=start) re[i]+=offset;
  }
}

wxString InformData::GetSource(int infType)
{
  if(e[infType]==0) return ""; // This segment does not exist
  return frame->project.GetSourceFile(file)->GetSource(INF_SOURCE).SubString(b[infType], e[infType]);
}


void InformData::SetSource(int infType, const wxString& newSource)
{
  SetDirty();
  //wxLogDebug("%s DIRTY %d '%s'\n", GetSource(INF_ID).c_str(), infType, newSource.c_str());

  wxString s;
  if(newSource.IsEmpty()) {
    if(e[infType]==0) return; // This segment does not exist
//    wxLogDebug("Before:\n%s\n", GetSource(INF_SOURCE).c_str());
    if(infType>=INF_A_ABSENT && infType<=INF_A_WORN) {
      // Remove attribute (has)
      frame->project.GetSourceFile(file)->ReplaceCode(newSource, b[infType]-1, e[infType]);
    } else if(!informKeyword[infType].IsEmpty()) {
      // Remove property (where)
      frame->project.GetSourceFile(file)->ReplaceCode(newSource, rb[infType], re[infType]);
    } else {
      // Other code (include, constants etc)
      frame->project.GetSourceFile(file)->ReplaceCode(newSource, b[infType], e[infType]);
    }
    b[infType]=0;
    e[infType]=0;
//    wxLogDebug("After:\n%s\n", GetSource(INF_SOURCE).c_str());
    return;
  }

  // Modify segment

  if(e[infType]==0) {
    // This segment does not exist in the code
    if(infType>=INF_A_ABSENT && infType<=INF_A_WORN) {
      // Attibute: insert in "has"
      if(e[INF_HAS]==0) {
        // No "has" available
//        wxLogDebug("Before %d:\n%s\n", e[INF_WITH], GetSource(INF_SOURCE).c_str());
        if(e[INF_WITH]==0) s=""; else s=",";
        s=s+"\n"+frame->project.indent+"has "+informKeyword[infType];
        frame->project.GetSourceFile(file)->ReplaceCode(s, e[INF_SOURCE], e[INF_SOURCE]-1);
//        wxLogDebug("After:\n%s\n", GetSource(INF_SOURCE).c_str());
        Update();
      } else {
//        wxLogDebug("Adding attribute in '%s'\n", GetSource(INF_WITH).c_str());
        //wxLogDebug("Before:\n%s\n", GetSource(INF_SOURCE).c_str());
        //wxLogDebug("Has: '%s'\n", GetSource(INF_HAS).c_str());
        s=" "+informKeyword[infType];
        frame->project.GetSourceFile(file)->ReplaceCode(s, e[INF_HAS]+1, e[INF_HAS]);
        //wxLogDebug("After:\n%s\n", GetSource(INF_SOURCE).c_str());
      }
    } else if(!informKeyword[infType].IsEmpty()) {
      // property: insert in "with"
      if(e[INF_WITH]==0) {
//        wxLogDebug("Before:\n%s\n", GetSource(INF_SOURCE).c_str());
        s="with\n"+frame->project.indent+frame->project.indent+
          informKeyword[infType]+" "+newSource;
        if(e[INF_HAS]>0) frame->project.GetSourceFile(file)->ReplaceCode(s+",\n"+frame->project.indent, b[INF_HAS], b[INF_HAS]-1);
        else if(e[INF_NAME]>0) frame->project.GetSourceFile(file)->ReplaceCode("\n"+frame->project.indent+s, e[INF_NAME]+1, e[INF_NAME]);
        else if(e[INF_ID]>0) frame->project.GetSourceFile(file)->ReplaceCode("\n"+frame->project.indent+s, e[INF_ID]+1, e[INF_ID]);
        else wxLogError("Failed to insert with block in InformData::SetSource");
//        wxLogDebug("After:\n%s\n", GetSource(INF_SOURCE).c_str());
        Update();
      } else {
        //wxLogDebug("Adding property in '%s'\n", GetSource(INF_WITH).c_str());
        s="\n"+frame->project.indent+frame->project.indent+informKeyword[infType]+" "+newSource+",";
        //wxLogDebug("Before:\n%s\n", GetSource(INF_SOURCE).c_str());
// problem with "with u_to X;"  frame->project.GetSourceFile(file)->ReplaceCode(s, e[INF_WITH]+1, e[INF_WITH]);
        frame->project.GetSourceFile(file)->ReplaceCode(s, b[INF_WITH]+4, b[INF_WITH]+3);
        //wxLogDebug("After:\n%s\n", GetSource(INF_SOURCE).c_str());
      }
    } else {
      // Other code, such as function, constant etc.
      switch(infType) {
      case INF_PARENT:
        //wxLogDebug("Before:\n%s\n", GetSource(INF_SOURCE).c_str());
        frame->project.GetSourceFile(file)->InsertCode(" "+newSource, e[INF_NAME]+1);
        //wxLogDebug("After:\n%s\n", GetSource(INF_SOURCE).c_str());
        break;
      default:
        wxLogDebug("ERROR: cannot insert %d\n", infType);
      }
    }
  } else {
    // Modify existing item
        //wxLogDebug("Before %d:\n%s\n", infType, GetSource(INF_SOURCE).c_str());
        //wxLogDebug("xxx %s\n", GetSource(infType).c_str());
    frame->project.GetSourceFile(file)->ReplaceCode(newSource, b[infType], e[infType]);
    e[infType]=b[infType]+newSource.Length()-1;
        //wxLogDebug("xxx %s\n", GetSource(infType).c_str());
        //wxLogDebug("After:\n%s\n", GetSource(INF_SOURCE).c_str());
  }
}
