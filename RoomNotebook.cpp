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

#include <wx/scrolwin.h>

#include "RoomNotebook.h"
#include "SourceFile.h"
#include "ObjectData.h"

#include "kazam.h"
extern MainFrame *frame;

RoomNotebook::RoomNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
  : InformNotebook(parent, id, pos, size, style, name)
{
  room=NULL;

  // Map panel
  map = new MapWidget(this, -1);
//  wxBoxSizer* mapSizer = new wxBoxSizer(wxHORIZONTAL);
//  mapSizer->Add(map, 1, wxEXPAND, 0);

  // Property panel
  wxStaticText* labelClass;
  wxStaticText* labelID;
  wxStaticText* labelAttributes;
  wxStaticText* labelExits;
  wxStaticText* labelUp;
  wxStaticText* labelDown;
  wxStaticText* labelIn;
  wxStaticText* labelName;
  wxStaticText* labelOut;
  wxStaticText* labelScenery;
  wxStaticText* labelDescription;
  wxPanel* propertyPanel;

  propertyPanel = new wxPanel(this, -1);
  labelClass = new wxStaticText(propertyPanel, -1, wxT("Class"));
  comboClass = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN|wxCB_READONLY);
  labelID = new wxStaticText(propertyPanel, -1, wxT("ID"));
  editID = new wxTextCtrl(propertyPanel, -1, wxT(""));
  labelName = new wxStaticText(propertyPanel, -1, wxT("Name"));
  editName = new wxTextCtrl(propertyPanel, -1, wxT(""));
  labelScenery = new wxStaticText(propertyPanel, -1, wxT("Scenery"));
  editScenery = new wxTextCtrl(propertyPanel, -1, wxT(""));
  labelDescription = new wxStaticText(propertyPanel, -1, wxT("Description"));
  editDescription = new wxTextCtrl(propertyPanel, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxHSCROLL);
  labelAttributes = new wxStaticText(propertyPanel, -1, wxT("Attributes"));
  checkboxLight = new wxCheckBox(propertyPanel, -1, wxT("light"));
  comboNW = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN);
  comboN = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN);
  comboNE = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN);
  comboW = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN);
  labelExits = new wxStaticText(propertyPanel, -1, wxT("EXITS"));
  comboE = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN);
  comboSW = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN);
  comboS = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN);
  comboSE = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN);
  labelUp = new wxStaticText(propertyPanel, -1, wxT("Up"));
  comboUp = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN);
  labelDown = new wxStaticText(propertyPanel, -1, wxT("Down"));
  comboDown = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN);
  labelIn = new wxStaticText(propertyPanel, -1, wxT("In"));
  comboIn = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN);
  labelOut = new wxStaticText(propertyPanel, -1, wxT("Out"));
  comboOut = new InformComboBox(propertyPanel, -1, "", wxDefaultPosition, wxDefaultSize, 5, NULL, wxCB_DROPDOWN);

  comboClass->SetToolTip(wxT("The class of this room. Default is \"Object\" (no class)"));
  editID->SetToolTip(wxT("This is the object ID"));
  editName->SetToolTip(wxT("The name of this room will appear in the status bar"));
  editScenery->SetToolTip(wxT("A list of \"fake\" scenery objects"));
  editDescription->SetToolTip(wxT("Text that describes this room"));
  checkboxLight->SetToolTip(wxT("Decides if your can see anything"));

  wxString dirHelp=wxT("Each direction can contain the name of a room, a string (\"like this\"), or a function");
  comboN->SetToolTip(dirHelp);
  comboS->SetToolTip(dirHelp);
  comboE->SetToolTip(dirHelp);
  comboW->SetToolTip(dirHelp);
  comboNW->SetToolTip(dirHelp);
  comboNE->SetToolTip(dirHelp);
  comboSW->SetToolTip(dirHelp);
  comboSE->SetToolTip(dirHelp);
  comboUp->SetToolTip(dirHelp);
  comboDown->SetToolTip(dirHelp);
  comboIn->SetToolTip(dirHelp);
  comboOut->SetToolTip(dirHelp);

  wxFlexGridSizer* sizer_2 = new wxFlexGridSizer(3, 1, 0, 0);
  wxFlexGridSizer* grid_sizer_4 = new wxFlexGridSizer(1, 2, 0, 0);
  wxFlexGridSizer* grid_sizer_3 = new wxFlexGridSizer(2, 4, 0, 0);
  wxGridSizer* grid_sizer_1 = new wxGridSizer(3, 3, 0, 0);
  wxFlexGridSizer* grid_sizer_2 = new wxFlexGridSizer(6, 2, 0, 0);
  grid_sizer_2->Add(labelClass, 0, 0, 0);
  grid_sizer_2->Add(comboClass, 0, wxEXPAND, 0);
  grid_sizer_2->Add(labelID, 0, 0, 0);
  grid_sizer_2->Add(editID, 0, wxEXPAND, 0);
  grid_sizer_2->Add(labelName, 0, 0, 0);
  grid_sizer_2->Add(editName, 0, wxEXPAND, 0);
  grid_sizer_2->Add(labelScenery, 0, 0, 0);
  grid_sizer_2->Add(editScenery, 0, wxEXPAND, 0);
  grid_sizer_2->Add(labelDescription, 0, 0, 0);
  grid_sizer_2->Add(editDescription, 0, wxEXPAND, 0);
  grid_sizer_2->Add(labelAttributes, 0, 0, 0);
  grid_sizer_2->Add(checkboxLight, 0, 0, 0);
  grid_sizer_2->Add(20, 20, 0, 0, 0);
  grid_sizer_2->AddGrowableRow(4);
  grid_sizer_2->AddGrowableCol(1);
  sizer_2->Add(grid_sizer_2, 1, wxEXPAND, 0);
  grid_sizer_1->Add(comboNW, 0, wxEXPAND, 0);
  grid_sizer_1->Add(comboN, 0, wxEXPAND, 0);
  grid_sizer_1->Add(comboNE, 0, wxEXPAND, 0);
  grid_sizer_1->Add(comboW, 0, wxEXPAND, 0);
  grid_sizer_1->Add(labelExits, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
  grid_sizer_1->Add(comboE, 0, wxEXPAND, 0);
  grid_sizer_1->Add(comboSW, 0, wxEXPAND, 0);
  grid_sizer_1->Add(comboS, 0, wxEXPAND, 0);
  grid_sizer_1->Add(comboSE, 0, wxEXPAND, 0);
  sizer_2->Add(grid_sizer_1, 1, wxEXPAND, 0);
  grid_sizer_3->Add(labelUp, 0, 0, 0);
  grid_sizer_3->Add(comboUp, 0, wxEXPAND, 0);
  grid_sizer_3->Add(labelDown, 0, 0, 0);
  grid_sizer_3->Add(comboDown, 0, wxEXPAND, 0);
  grid_sizer_3->Add(labelIn, 0, 0, 0);
  grid_sizer_3->Add(comboIn, 0, wxEXPAND, 0);
  grid_sizer_3->Add(labelOut, 0, 0, 0);
  grid_sizer_3->Add(comboOut, 0, wxEXPAND, 0);
  grid_sizer_3->AddGrowableCol(1);
  grid_sizer_3->AddGrowableCol(3);
  sizer_2->Add(grid_sizer_3, 1, wxEXPAND, 0);
  grid_sizer_4->AddGrowableRow(0);
  grid_sizer_4->AddGrowableCol(1);
  sizer_2->Add(grid_sizer_4, 1, wxEXPAND, 0);
  propertyPanel->SetAutoLayout(true);
  propertyPanel->SetSizer(sizer_2);
  sizer_2->Fit(propertyPanel);
  sizer_2->SetSizeHints(propertyPanel);
  sizer_2->AddGrowableRow(0);
  sizer_2->AddGrowableCol(0);

  // Source panel
  wxPanel* sourcePanel=new wxPanel(this, -1);
  wxGridSizer* sourceSizer=new wxGridSizer(1, 1, 0, 0);

  sourceEditor = new InformEditor(sourcePanel, -1);
  sourceSizer->Add(sourceEditor, 0, wxEXPAND, 0);
  sourcePanel->SetAutoLayout(true);
  sourcePanel->SetSizer(sourceSizer);

  AddPage(map, wxT("Map"));
  AddPage(propertyPanel, wxT("Properties"));
  AddPage(sourcePanel, wxT("Source Code"));
}

void RoomNotebook::LoadPageData(int page, InformData *data)
{
  if(data!=NULL) room=(ObjectData *) data;
  if(room==NULL) return;
  if(page==-1) page=GetSelection();

  switch(page) {
  case 0:
    map->Init();
    frame->ShowToolMenu(frame->mapMenu, "&Map");
    break;
  case 1:
    SetComboClass(comboClass, room);
    SetEdit(editID, INF_ID, room, false, false);
    SetEdit(editName, INF_NAME, room, true, false);
    SetEdit(editScenery, INF_WORDS, room, true, true);
    SetEdit(editDescription, INF_DESCRIPTION, room, true, false);
    SetCheckbox(checkboxLight, INF_A_LIGHT, room);
    SetComboDirs(comboN, room, INF_DIR_N);
    SetComboDirs(comboS, room, INF_DIR_S);
    SetComboDirs(comboE, room, INF_DIR_E);
    SetComboDirs(comboW, room, INF_DIR_W);
    SetComboDirs(comboNE, room, INF_DIR_NE);
    SetComboDirs(comboNW, room, INF_DIR_NW);
    SetComboDirs(comboSE, room, INF_DIR_SE);
    SetComboDirs(comboSW, room, INF_DIR_SW);
    SetComboDirs(comboUp, room, INF_DIR_U);
    SetComboDirs(comboDown, room, INF_DIR_D);
    SetComboDirs(comboIn, room, INF_DIR_IN);
    SetComboDirs(comboOut, room, INF_DIR_OUT);
    break;
  case 2:
    frame->ShowToolMenu(frame->editMenu, "&Edit");
    sourceEditor->SetValue(room->GetSource(INF_SOURCE));
    break;
  }
}

bool RoomNotebook::SavePageData(int page, wxTreeEvent *event)
{
  if(room==NULL) return false;
  if(page==-1) page=GetSelection();

  wxString classname=room->GetSource(INF_CLASS);
  if(classname.IsEmpty()) classname="Object";

  switch(page) {
  case 0: // MAP
    frame->HideToolMenu("&Map", &frame->mapMenu);
    break;
  case 1: // Properties
    GetComboClass(comboClass, room);
    if(GetEdit(editID, INF_ID, room, false, false, "Room ID missing")) return true;
    GetEdit(editName, INF_NAME, room, true, false);
    GetEdit(editScenery, INF_WORDS, room, true, true);
    GetEdit(editDescription, INF_DESCRIPTION, room, true, false);
    GetCheckbox(checkboxLight, INF_A_LIGHT, room);
    GetComboDirs(comboN, room, INF_DIR_N);
    GetComboDirs(comboS, room, INF_DIR_S);
    GetComboDirs(comboE, room, INF_DIR_E);
    GetComboDirs(comboW, room, INF_DIR_W);
    GetComboDirs(comboNE, room, INF_DIR_NE);
    GetComboDirs(comboNW, room, INF_DIR_NW);
    GetComboDirs(comboSE, room, INF_DIR_SE);
    GetComboDirs(comboSW, room, INF_DIR_SW);
    GetComboDirs(comboUp, room, INF_DIR_U);
    GetComboDirs(comboDown, room, INF_DIR_D);
    GetComboDirs(comboIn, room, INF_DIR_IN);
    GetComboDirs(comboOut, room, INF_DIR_OUT);
    break;
  case 2: // Source code
    frame->HideToolMenu("&Edit", &frame->editMenu);
    if(sourceEditor->GetModified()) {
      room->SetSource(INF_SOURCE, sourceEditor->GetValue());
    }
    break;
  }

  if(frame->project.UpdateAll(event, room)) LoadPageData();
  return false;
}
