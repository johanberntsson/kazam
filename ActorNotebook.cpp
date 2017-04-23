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
#include "ActorNotebook.h"
#include "ObjectData.h"

#include "kazam.h"
extern MainFrame *frame;

ActorNotebook::ActorNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
  : InformNotebook(parent, id, pos, size, style, name)
{
  actor=NULL;

  // Property page
  wxStaticText* labelClass;
  wxStaticText* labelLocation;
  wxStaticText* labelID;
  wxStaticText* labelName;
  wxStaticText* labelWords;
  wxStaticText* labelDescription;
  wxStaticText* labelLife;
  wxStaticText* labelAttributes;
  wxPanel* propertyPanel;

  // TODO: Another stupid workaround for a wxWindows bug.
  // Clear() and Append() does not update the dropdown
  // box size, and we have to set the expected dropdown
  // size by adding dummies in the constructor.
  const wxString comboClass_choices[] = {
      wxT("dummy1"),
      wxT("dummy2"),
      wxT("dummy3"),
      wxT("dummy4"),
      wxT("dummy5")
  };

  propertyPanel = new wxPanel(this, -1);
  labelClass = new wxStaticText(propertyPanel, -1, wxT("Class"));
  comboClass = new wxComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, comboClass_choices, wxCB_DROPDOWN|wxCB_READONLY);
  labelLocation = new wxStaticText(propertyPanel, -1, wxT("Location"));
  comboLocation = new wxComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, comboClass_choices, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SORT);
  labelID = new wxStaticText(propertyPanel, -1, wxT("ID"));
  editID = new wxTextCtrl(propertyPanel, -1, wxT(""));
  labelName = new wxStaticText(propertyPanel, -1, wxT("Name"));
  editName = new wxTextCtrl(propertyPanel, -1, wxT(""));
  labelWords = new wxStaticText(propertyPanel, -1, wxT("Words"));
  editWords = new wxTextCtrl(propertyPanel, -1, wxT(""));
  labelDescription = new wxStaticText(propertyPanel, -1, wxT("Description"));
  editDescription = new wxTextCtrl(propertyPanel, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxHSCROLL);
  labelLife = new wxStaticText(propertyPanel, -1, wxT("Life"));
  editLife = new wxTextCtrl(propertyPanel, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxHSCROLL);
  labelAttributes = new wxStaticText(propertyPanel, -1, wxT("Attributes"));
  checkboxProper = new wxCheckBox(propertyPanel, -1, wxT("proper"));
  checkboxFemale = new wxCheckBox(propertyPanel, -1, wxT("female"));
  checkboxConcealed = new wxCheckBox(propertyPanel, -1, wxT("concealed"));

    wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer(7, 2, 0, 0);
    wxGridSizer* grid_sizer_2 = new wxGridSizer(1, 3, 0, 0);
    grid_sizer_1->Add(labelClass, 0, 0, 0);
    grid_sizer_1->Add(comboClass, 0, wxEXPAND, 0);
    grid_sizer_1->Add(labelLocation, 0, 0, 0);
    grid_sizer_1->Add(comboLocation, 0, wxEXPAND, 0);
    grid_sizer_1->Add(labelID, 0, 0, 0);
    grid_sizer_1->Add(editID, 0, wxEXPAND, 0);
    grid_sizer_1->Add(labelName, 0, 0, 0);
    grid_sizer_1->Add(editName, 0, wxEXPAND, 0);
    grid_sizer_1->Add(labelWords, 0, 0, 0);
    grid_sizer_1->Add(editWords, 0, wxEXPAND, 0);
    grid_sizer_1->Add(labelDescription, 0, 0, 0);
    grid_sizer_1->Add(editDescription, 0, wxEXPAND, 0);
    grid_sizer_1->Add(labelLife, 0, 0, 0);
    grid_sizer_1->Add(editLife, 0, wxEXPAND, 0);
    grid_sizer_1->Add(labelAttributes, 0, 0, 0);
    grid_sizer_2->Add(checkboxProper, 0, 0, 0);
    grid_sizer_2->Add(checkboxFemale, 0, 0, 0);
    grid_sizer_2->Add(checkboxConcealed, 0, 0, 0);
    grid_sizer_1->Add(grid_sizer_2, 1, wxEXPAND, 0);
    propertyPanel->SetAutoLayout(true);
    propertyPanel->SetSizer(grid_sizer_1);
    grid_sizer_1->Fit(propertyPanel);
    grid_sizer_1->SetSizeHints(propertyPanel);
    grid_sizer_1->AddGrowableRow(5);
    grid_sizer_1->AddGrowableRow(6);
    grid_sizer_1->AddGrowableCol(1);

  // Source code
  wxPanel* sourcePanel = new wxPanel(this, -1);
  sourceEditor = new InformEditor(sourcePanel, -1);
  wxGridSizer* sizer = new wxGridSizer(1, 1, 0, 0);
  sizer->Add(sourceEditor, 0, wxEXPAND, 0);
  sourcePanel->SetAutoLayout(true);
  sourcePanel->SetSizer(sizer);

  AddPage(propertyPanel, wxT("Properties"));
  AddPage(sourcePanel, wxT("Source Code"));
}

void ActorNotebook::LoadPageData(int page, InformData *data)
{
  if(data!=NULL) actor=(ObjectData *) data;
  if(actor==NULL) return;

  if(page==-1) page=GetSelection();

  switch(page) {
  case 0:
    // Properties
    SetComboClass(comboClass, actor);
    SetComboLocation(comboLocation, actor, false);
    SetEdit(editID, INF_ID, actor, false, false);
    SetEdit(editName, INF_NAME, actor, true, false);
    SetEdit(editWords, INF_WORDS, actor, true, true);
    SetEdit(editDescription, INF_DESCRIPTION, actor, true, false);
    SetCheckbox(checkboxProper, INF_A_PROPER, actor);
    SetCheckbox(checkboxConcealed, INF_A_CONCEALED, actor);
    SetCheckbox(checkboxFemale, INF_A_FEMALE, actor);
    break;
  case 1:
    // Source code
    frame->ShowToolMenu(frame->editMenu, "&Edit");
    sourceEditor->SetValue(actor->GetSource(INF_SOURCE));
    break;
  }
}

bool ActorNotebook::SavePageData(int page, wxTreeEvent *event)
{
  if(actor==NULL) return false;

  if(page==-1) page=GetSelection();
  switch(page) {
  case 0: // Properties
    GetComboClass(comboClass, actor);
    GetComboLocation(comboLocation, actor);
    if(GetEdit(editID, INF_ID, actor, false, false, "Actor ID missing")) return true;
    GetEdit(editName, INF_NAME, actor, true, false);
    GetEdit(editWords, INF_WORDS, actor, true, true);
    GetEdit(editDescription, INF_DESCRIPTION, actor, true, false);
    GetCheckbox(checkboxProper, INF_A_PROPER, actor);
    GetCheckbox(checkboxConcealed, INF_A_CONCEALED, actor);
    GetCheckbox(checkboxFemale, INF_A_FEMALE, actor);
  case 1: // Source code
    frame->HideToolMenu("&Edit", &frame->editMenu);
    if(sourceEditor->GetModified()) {
      actor->SetSource(INF_SOURCE, sourceEditor->GetValue());
    }
    break;
  }

  if(frame->project.UpdateAll(event, actor)) LoadPageData();
  return false;
}
