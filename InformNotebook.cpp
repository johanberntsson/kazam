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

#include "InformNotebook.h"

#include "kazam.h"
extern MainFrame *frame;

BEGIN_EVENT_TABLE(InformNotebook, wxNotebook)
EVT_NOTEBOOK_PAGE_CHANGING(-1, InformNotebook::OnSelChanging)
EVT_NOTEBOOK_PAGE_CHANGED(-1, InformNotebook::OnSelChanged)
END_EVENT_TABLE()

InformNotebook::InformNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
  : wxNotebook(parent, id, pos, size, style, name) 
{
}

// Convert "aaa" "bbb" -> aaa bbb
wxString InformNotebook::InformToCStrings(const wxString& data)
{
  wxString r=data;
  r.Replace("\"", "", TRUE);
  return r;
}

wxString InformNotebook::CToInformStrings(const wxString& data)
{
  wxString r;
  //wxString s=data.Strip(wxString::stripType::both);
  wxString s=data.Strip(wxString::both);

  if(s.IsEmpty()) return "";

  bool isChar=true;
  for(unsigned i=0; i<s.Length(); i++) {
    if((isChar && s[i]==' ') || (!isChar && s[i]!=' ')) {
      isChar=!isChar;
      r+="\"";
    }
    r+=s[i];
  }

  //wxLogDebug("CToInformStrings: '%s' -> \"%s\"\n", data.c_str(), r.c_str());
  return "\""+r+"\"";
}

wxString InformNotebook::InformToC(const wxString& informdata, bool *isString)
{
  unsigned int i, j;
  wxString cString=informdata;

  if(informdata.StartsWith("[")) return informdata;

  for(;;) {
	  i=cString.Find('\\');
		if(i==-1) break;
	  //wxLogDebug("%d\n", i);
	  j=i+1;
	  while(cString[j]==' ' || cString[j]=='\n') ++j;
		cString=cString.Left(i)+cString.Mid(j);
	}

  cString.Replace("~", "\"", TRUE);
  cString.Replace("^", "\n", TRUE);

  return cString.Mid(1, cString.Length()-2);
}

wxString InformNotebook::CToInform(const wxString& cdata, bool isString, unsigned int width, unsigned int indentLevel)
{
  if(cdata.StartsWith("[")) return cdata;

  wxString informString=cdata;
  informString.Replace("\"", "~", TRUE);
  informString.Replace("\n", "^", TRUE);

  wxString line="";
  wxString output="";
  bool isChar=true;
  bool skip=false;
  unsigned int i, j;
  for(i=0; i<informString.Length(); i++) {
    if((isChar && informString[i]==' ') || (!isChar && informString[i]!=' ')) {
      isChar=!isChar;
      if(!isChar && line.Length()>width) {
        output+=line;
        output+=" \\\n";
        for(j=0; j<indentLevel; j++) output+=frame->project.indent;
        skip=true;
        line="";
      } else skip=false;
    }
    if(!skip) line+=informString[i];
  }
  output+=line;

  return "\""+output+"\"";
}

void InformNotebook::SetComboLocation(wxComboBox *combo, InformData *object, bool onlyRooms)
{
  wxString location;

  combo->Clear();
  if(object->type==INF_T_SOURCEFILE) {
    location=object->GetSource(INF_STARTPOS);
  } else {
    combo->Append("<none>");
    location=object->GetSource(INF_PARENT);
  }

  for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
    InformData *current=&frame->project.objects[i];
    if(current->type==INF_T_ROOM) {
      combo->Append(current->GetSource(INF_ID));
    } else if(!onlyRooms) {
      if(current->type==INF_T_ITEM || current->type==INF_T_ACTOR) {
        combo->Append(current->GetSource(INF_ID));
      }
    }
  }

  if(location.IsEmpty()) {
    if(object->type!=INF_T_SOURCEFILE) combo->SetValue("<none>");
  } else if(frame->project.FindObject(location)) {
    combo->SetValue(location);
  }
}

void InformNotebook::GetComboLocation(wxComboBox *combo, InformData *object)
{
  if(object->type==INF_T_SOURCEFILE) {
    if(!combo->GetValue().IsSameAs(object->GetSource(INF_STARTPOS))) {
      object->SetSource(INF_STARTPOS, combo->GetValue());
    }
  } else {
    if(!combo->GetValue().IsSameAs(object->GetSource(INF_PARENT))) {
      frame->project.MoveObject(object, combo->GetValue());
    }
  }
}

void InformNotebook::GetComboDirs(wxComboBox *combo, InformData *object, int type)
{
  if(!combo->GetValue().IsSameAs(object->GetSource(type))) {
    wxLogDebug("Setting new dir\n");
    object->SetSource(type, combo->GetValue());
  }
}

void InformNotebook::SetComboDirs(wxComboBox *combo, InformData *object, int type)
{
  combo->Clear();
  const wxString& direction=object->GetSource(type);
  for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
    InformData *current=&frame->project.objects[i];
    if(current->type==INF_T_ROOM) {
      combo->Append(current->GetSource(INF_ID));
    }
  }
  combo->SetValue(direction);
}
void InformNotebook::GetComboClass(wxComboBox *combo, InformData *object)
{
  wxString classname=object->GetSource(INF_CLASS);
  if(classname.IsEmpty()) classname="Object";

  if(!combo->GetValue().IsSameAs(classname)) {
    object->SetSource(INF_CLASS, combo->GetValue());
  }
}

void InformNotebook::SetComboClass(wxComboBox *combo, InformData *object)
{
  combo->Clear();
  combo->Append("Object");
  const wxString& classname=object->GetSource(INF_CLASS);
  for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
    InformData *current=&frame->project.objects[i];
    if(current->type==INF_T_CLASS) {
      combo->Append(current->GetSource(INF_ID));
    }
  }
  if(classname.IsEmpty()) {
    combo->SetSelection(0);
  } else {
    combo->SetValue(classname);
  }
}

void InformNotebook::OnSelChanging(wxNotebookEvent& event)
{
  if(this->SavePageData()) event.Veto();
}

void InformNotebook::OnSelChanged(wxNotebookEvent& event)
{
  this->LoadPageData(event.GetSelection(), NULL);
  event.Skip();
}

void InformNotebook::SetCheckbox(wxCheckBox *cb, int type, InformData *object)
{
	cb->SetValue(!object->GetSource(type).IsEmpty());
}

void InformNotebook::GetCheckbox(wxCheckBox *cb, int type, InformData *object)
{
  if(object->GetSource(type).IsEmpty()==cb->GetValue()) {
    if(cb->GetValue()) {
      object->SetSource(type, "true");
    } else {
      object->SetSource(type, "");
    }
  }
}

void InformNotebook::SetEdit(wxTextCtrl *edit, int type, InformData *object, bool informString, bool strings)
{
  if(informString) {
    if(strings) {
      edit->SetValue(InformToCStrings(object->GetSource(type)));
    } else {
      wxString msg=object->GetSource(type);
      if(msg.StartsWith("[")) {
        edit->SetValue(msg);
      } else {
        edit->SetValue(InformToC(msg));
      }
    }
  } else {
    edit->SetValue(object->GetSource(type));
  }
}

// true if incomplete input
bool InformNotebook::GetEdit(wxTextCtrl *edit, int type, InformData *object, bool informString, bool strings, const wxString& emptyMessage)
{
  if(edit->IsModified()) {
    if(!emptyMessage.IsEmpty() && edit->GetValue().IsEmpty()) {
      wxMessageBox(emptyMessage, _T("Incomplete Data"), wxOK | wxICON_WARNING, this);
      return true;
    }
    if(informString) {
      if(strings) {
        object->SetSource(type, CToInformStrings(edit->GetValue()));
      } else {
        object->SetSource(type, CToInform(edit->GetValue()));
      }
    } else {
      object->SetSource(type, edit->GetValue());
    }
  }
  return false;
}
