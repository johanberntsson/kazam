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
#include "ObjectNotebook.h"
#include "ObjectData.h"

#include "kazam.h"
extern MainFrame *frame;

ObjectNotebook::ObjectNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
  : InformNotebook(parent, id, pos, size, style, name)
{
  object=NULL;
  // Property panel
  wxPanel* propertyPanel;
  wxStaticText* labelClass;
  wxStaticText* labelLocation;
  wxStaticText* labelID;
  wxStaticText* labelName;
  wxStaticText* labelWords;
  wxStaticText* labelDescription;
  wxStaticText* labelAttributes;
  // TODO: Another stupid workaround for a wxWindows bug.
  // Clear() and Append() does not update the dropdown
  // box size, and we have to set the expected dropdown
  // size by adding dummies in the constructor.
  const wxString comboClass_choices[] = {
      wxT("1"),
      wxT("2"),
      wxT("3"),
      wxT("4"),
      wxT("5")
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
  editDescription = new wxTextCtrl(propertyPanel, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
  labelAttributes = new wxStaticText(propertyPanel, -1, wxT("Attributes"));
  checkboxGeneral = new wxCheckBox(propertyPanel, -1, wxT("general"));
  checkboxScenery = new wxCheckBox(propertyPanel, -1, wxT("scenery"));
  checkboxConcealed = new wxCheckBox(propertyPanel, -1, wxT("concealed"));
  checkboxAnimate = new wxCheckBox(propertyPanel, -1, wxT("animate"));
  checkboxLight = new wxCheckBox(propertyPanel, -1, wxT("light"));
  checkboxClothing = new wxCheckBox(propertyPanel, -1, wxT("clothing"));
  checkboxWorn = new wxCheckBox(propertyPanel, -1, wxT("worn"));
  checkboxFemale = new wxCheckBox(propertyPanel, -1, wxT("female"));
  checkboxTransparent = new wxCheckBox(propertyPanel, -1, wxT("transparent"));

  wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer(6, 2, 0, 0);
  wxGridSizer* grid_sizer_2 = new wxGridSizer(3, 3, 0, 0);
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
  grid_sizer_1->Add(labelAttributes, 0, 0, 0);
  grid_sizer_2->Add(checkboxGeneral, 0, 0, 0);
  grid_sizer_2->Add(checkboxScenery, 0, 0, 0);
  grid_sizer_2->Add(checkboxConcealed, 0, 0, 0);
  grid_sizer_2->Add(checkboxAnimate, 0, 0, 0);
  grid_sizer_2->Add(checkboxLight, 0, 0, 0);
  grid_sizer_2->Add(checkboxClothing, 0, 0, 0);
  grid_sizer_2->Add(checkboxWorn, 0, 0, 0);
  grid_sizer_2->Add(checkboxFemale, 0, 0, 0);
  grid_sizer_2->Add(checkboxTransparent, 0, 0, 0);
  grid_sizer_1->Add(grid_sizer_2, 1, wxEXPAND, 0);
  propertyPanel->SetAutoLayout(true);
  propertyPanel->SetSizer(grid_sizer_1);
  grid_sizer_1->Fit(propertyPanel);
  grid_sizer_1->SetSizeHints(propertyPanel);
  grid_sizer_1->AddGrowableRow(5);
  grid_sizer_1->AddGrowableCol(1);

  // Filter panel
#ifdef ERRORINGTK
  wxPanel* filterPanel;
  wxStaticText* labelUsed;
  wxStaticText* labelAvailable;  
  const wxString comboFilter_choices[] = {
      wxT("Before"),
      wxT("After"),
      wxT("React Before"),
      wxT("React After")
  };
  const wxString listboxUsed_choices[] = {
      wxT("Enter"),
      wxT("Take")
  };
  const wxString listboxAvailable_choices[] = {
      wxT("Drop"),
      wxT("Leave"),
      wxT("Open")
  };
  filterPanel = new wxPanel(this, -1);
  comboFilter = new wxComboBox(filterPanel, -1, "", wxDefaultPosition, wxDefaultSize, 4, comboFilter_choices, wxCB_DROPDOWN|wxCB_READONLY);
  labelUsed = new wxStaticText(filterPanel, -1, wxT("Used filters"));
  labelAvailable = new wxStaticText(filterPanel, -1, wxT("Available"));
  listboxUsed = new wxListBox(filterPanel, -1, wxDefaultPosition, wxDefaultSize, 2, listboxUsed_choices, 0);
  buttonAdd = new wxButton(filterPanel, -1, wxT("<== Add"));
  buttonDelete = new wxButton(filterPanel, -1, wxT("==> Delete"));
  listboxAvailable = new wxListBox(filterPanel, -1, wxDefaultPosition, wxDefaultSize, 3, listboxAvailable_choices, 0);
  buttonCode = new wxButton(filterPanel, -1, wxT("View Code"));

  wxFlexGridSizer* grid_sizer_3 = new wxFlexGridSizer(3, 1, 0, 0);
  wxFlexGridSizer* grid_sizer_4 = new wxFlexGridSizer(1, 3, 0, 0);
  wxFlexGridSizer* grid_sizer_7 = new wxFlexGridSizer(2, 3, 0, 0);
  wxFlexGridSizer* grid_sizer_8 = new wxFlexGridSizer(2, 1, 0, 0);
  grid_sizer_3->Add(comboFilter, 0, wxEXPAND, 0);
  grid_sizer_7->Add(labelUsed, 0, 0, 0);
  grid_sizer_7->Add(20, 20, 0, 0, 0);
  grid_sizer_7->Add(labelAvailable, 0, 0, 0);
  grid_sizer_7->Add(listboxUsed, 0, wxEXPAND, 0);
  grid_sizer_8->Add(buttonAdd, 0, wxALIGN_CENTER_VERTICAL, 0);
  grid_sizer_8->Add(buttonDelete, 0, wxALIGN_CENTER_VERTICAL, 0);
  grid_sizer_8->Add(buttonCode, 0, wxALIGN_CENTER_VERTICAL, 0);
  grid_sizer_8->AddGrowableRow(0);
  grid_sizer_8->AddGrowableRow(1);
  grid_sizer_8->AddGrowableRow(2);
  grid_sizer_7->Add(grid_sizer_8, 1, wxEXPAND, 0);
  grid_sizer_7->Add(listboxAvailable, 0, wxEXPAND, 0);
  grid_sizer_7->AddGrowableRow(1);
  grid_sizer_7->AddGrowableCol(0);
  grid_sizer_7->AddGrowableCol(2);
  grid_sizer_3->Add(grid_sizer_7, 1, wxEXPAND, 0);
  grid_sizer_4->AddGrowableCol(0);
  grid_sizer_4->AddGrowableCol(1);
  grid_sizer_3->Add(grid_sizer_4, 1, wxEXPAND, 0);
  filterPanel->SetAutoLayout(true);
  filterPanel->SetSizer(grid_sizer_3);
  grid_sizer_3->Fit(filterPanel);
  grid_sizer_3->SetSizeHints(filterPanel);
  grid_sizer_3->AddGrowableRow(1);
  grid_sizer_3->AddGrowableCol(0);
#endif

  // Source code
  wxPanel* sourcePanel = new wxPanel(this, -1);
  sourceEditor = new InformEditor(sourcePanel, -1);
  wxGridSizer* sizer = new wxGridSizer(1, 1, 0, 0);
  sizer->Add(sourceEditor, 0, wxEXPAND, 0);
  sourcePanel->SetAutoLayout(true);
  sourcePanel->SetSizer(sizer);

  AddPage(propertyPanel, wxT("Properties"));
//  AddPage(filterPanel, wxT("Command Filters"));
  AddPage(sourcePanel, wxT("Source Code"));
}

void ObjectNotebook::LoadPageData(int page, InformData *data)
{
  if(data!=NULL) object=(ObjectData *) data;
  if(object==NULL) return;
  if(page==-1) page=GetSelection();

  switch(page) {
  case 0:
    // Properties
    SetComboClass(comboClass, object);
    SetComboLocation(comboLocation, object, false);
    SetEdit(editID, INF_ID, object, false, false);
    SetEdit(editName, INF_NAME, object, true, false);
    SetEdit(editWords, INF_WORDS, object, true, true);
    SetEdit(editDescription, INF_DESCRIPTION, object, true, false);
    SetCheckbox(checkboxGeneral, INF_A_GENERAL, object);
    SetCheckbox(checkboxScenery, INF_A_SCENERY, object);
    SetCheckbox(checkboxConcealed, INF_A_CONCEALED, object);
    SetCheckbox(checkboxAnimate, INF_A_ANIMATE, object);
    SetCheckbox(checkboxLight, INF_A_LIGHT, object);
    SetCheckbox(checkboxClothing, INF_A_CLOTHING, object);
    SetCheckbox(checkboxWorn, INF_A_WORN, object);
    SetCheckbox(checkboxFemale, INF_A_FEMALE, object);
    SetCheckbox(checkboxTransparent, INF_A_TRANSPARENT, object);
    break;
  case 1:
    // Filter
    comboFilter->SetSelection(0);
    break;
  case 2:
    // Source code
    frame->ShowToolMenu(frame->editMenu, "&Edit");
    sourceEditor->SetValue(object->GetSource(INF_SOURCE));
    break;
  }
}

bool ObjectNotebook::SavePageData(int page, wxTreeEvent *event)
{
  if(object==NULL) return false;
  if(page==-1) page=GetSelection();

  switch(page) {
  case 0: // Properties
    GetComboClass(comboClass, object);
    if(GetEdit(editID, INF_ID, object, false, false, "Object ID missing")) return true;
    GetEdit(editName, INF_NAME, object, true, false);
    GetComboLocation(comboLocation, object);
    GetEdit(editWords, INF_WORDS, object, true, true);
    GetEdit(editDescription, INF_DESCRIPTION, object, true, false);
    GetCheckbox(checkboxGeneral, INF_A_GENERAL, object);
    GetCheckbox(checkboxScenery, INF_A_SCENERY, object);
    GetCheckbox(checkboxConcealed, INF_A_CONCEALED, object);
    GetCheckbox(checkboxAnimate, INF_A_ANIMATE, object);
    GetCheckbox(checkboxLight, INF_A_LIGHT, object);
    GetCheckbox(checkboxClothing, INF_A_CLOTHING, object);
    GetCheckbox(checkboxWorn, INF_A_WORN, object);
    GetCheckbox(checkboxFemale, INF_A_FEMALE, object);
    GetCheckbox(checkboxTransparent, INF_A_TRANSPARENT, object);
    break;
  case 2: // Source code
    frame->HideToolMenu("&Edit", &frame->editMenu);
    if(sourceEditor->GetModified()) {
      object->SetSource(INF_SOURCE, sourceEditor->GetValue());
    }
    break;
  }

  if(frame->project.UpdateAll(event, object)) LoadPageData();
  return false;
}
