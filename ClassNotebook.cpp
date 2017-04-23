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

#include "Project.h"
#include "ClassNotebook.h"
#include "ObjectData.h"

#include "kazam.h"
extern MainFrame *frame;

ClassNotebook::ClassNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
  : InformNotebook(parent, id, pos, size, style, name)
{
  classobject=NULL;

  wxPanel* sourcePanel = new wxPanel(this, -1);
  sourceEditor = new InformEditor(sourcePanel, -1);
  wxGridSizer* sizer = new wxGridSizer(1, 1, 0, 0);
  sizer->Add(sourceEditor, 0, wxEXPAND, 0);
  sourcePanel->SetAutoLayout(true);
  sourcePanel->SetSizer(sizer);
  AddPage(sourcePanel, wxT("Source Code"));
}

void ClassNotebook::LoadPageData(int page, InformData *data)
{
  if(data!=NULL) classobject=(ObjectData *) data;
  if(classobject==NULL) return;
  if(page==-1) page=GetSelection();

  switch(page) {
  case 0: // Source code
    frame->ShowToolMenu(frame->editMenu, "&Edit");
    sourceEditor->SetValue(classobject->GetSource(INF_SOURCE));
    break;
  }
}

bool ClassNotebook::SavePageData(int page, wxTreeEvent *event)
{
  if(classobject==NULL) return false;
  if(page==-1) page=GetSelection();

  switch(page) {
  case 0: // Source code
    frame->HideToolMenu("&Edit", &frame->editMenu);
    if(sourceEditor->GetModified()) {
      classobject->SetSource(INF_SOURCE, sourceEditor->GetValue());
    }
    break;
  }

  if(frame->project.UpdateAll(event, classobject)) LoadPageData();
  return false;
}
