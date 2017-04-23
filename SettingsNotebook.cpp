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
#include "SourceFile.h"
#include "SettingsNotebook.h"

#include "kazam.h"
extern MainFrame *frame;

SettingsNotebook::SettingsNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
  : InformNotebook(parent, id, pos, size, style, name)
{
  source=NULL;

  // Properties
  wxPanel* propertyPanel;
  wxStaticText* labelStory;
  wxStaticText* labelHeadline;
  wxStaticText* labelStart;
  wxStaticText* labelIntroduction;

  // TODO: Another stupid workaround for a wxWindows bug.
  // Clear() and Append() does not update the dropdown
  // box size, and we have to set the expected dropdown
  // size by adding dummies in the constructor.
  const wxString comboStart_choices[] = {
      wxT("1"),
      wxT("2"),
      wxT("3"),
      wxT("4"),
      wxT("5")
  };
  propertyPanel = new wxPanel(this, -1);
  labelStart = new wxStaticText(propertyPanel, -1, wxT("Start"));
  comboStart = new wxComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, comboStart_choices, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SORT);
  labelStory = new wxStaticText(propertyPanel, -1, wxT("Story"));
  editStory = new wxTextCtrl(propertyPanel, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
  labelHeadline = new wxStaticText(propertyPanel, -1, wxT("Headline"));
  editHeadline = new wxTextCtrl(propertyPanel, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
  labelIntroduction = new wxStaticText(propertyPanel, -1, wxT("Introduction"));
  editIntroduction = new wxTextCtrl(propertyPanel, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

  wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer(4, 2, 0, 0);
  grid_sizer_1->Add(labelStart, 0, 0, 0);
  grid_sizer_1->Add(comboStart, 0, wxEXPAND, 0);
  grid_sizer_1->Add(labelStory, 0, 0, 0);
  grid_sizer_1->Add(editStory, 0, wxEXPAND, 0);
  grid_sizer_1->Add(labelHeadline, 0, 0, 0);
  grid_sizer_1->Add(editHeadline, 0, wxEXPAND, 0);
  grid_sizer_1->Add(labelIntroduction, 0, 0, 0);
  grid_sizer_1->Add(editIntroduction, 0, wxEXPAND, 0);
  propertyPanel->SetAutoLayout(true);
  propertyPanel->SetSizer(grid_sizer_1);
  grid_sizer_1->Fit(propertyPanel);
  grid_sizer_1->SetSizeHints(propertyPanel);
  grid_sizer_1->AddGrowableRow(1);
  grid_sizer_1->AddGrowableRow(2);
  grid_sizer_1->AddGrowableRow(3);
  grid_sizer_1->AddGrowableCol(1);

  AddPage(propertyPanel, wxT("Settings"));
}

void SettingsNotebook::LoadPageData(int page, InformData *data)
{
  if(data!=NULL) source=(SourceFile *) data;
  if(source==NULL) return;
  if(page==-1) page=GetSelection();

  switch(page) {
  case 0:
    SetEdit(editStory, INF_STORY, source, true, false);
    SetEdit(editHeadline, INF_HEADLINE, source, true, false);
    SetEdit(editIntroduction, INF_INITDESC, source, true, false);
    SetComboLocation(comboStart, source, true);
    break;
  }
}

bool SettingsNotebook::SavePageData(int page, wxTreeEvent *event)
{
  if(source==NULL) return false;
  if(page==-1) page=GetSelection();

  switch(page) {
  case 0: // Properties
    GetComboLocation(comboStart, source);
    if(GetEdit(editStory, INF_STORY, source, true, false, "Story string missing")) return true;
    if(GetEdit(editHeadline, INF_HEADLINE, source, true, false, "Headline string missing")) return true;
    GetEdit(editIntroduction, INF_INITDESC, source, true, false);
    break;
  }

  frame->project.UpdateAll(event);
  LoadPageData();
  return false;
}
