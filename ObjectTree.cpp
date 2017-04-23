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

#include "ObjectTree.h"
#include "ObjectData.h"
#include "SourceFile.h"

#include "kazam.h"
extern MainFrame *frame;

BEGIN_EVENT_TABLE(ObjectTree, wxTreeCtrl)
EVT_TREE_SEL_CHANGED(TREE_PROJECT, ObjectTree::OnChanged)
EVT_TREE_SEL_CHANGING(TREE_PROJECT, ObjectTree::OnChanging)
EVT_TREE_ITEM_RIGHT_CLICK(TREE_PROJECT, ObjectTree::OnRightClick)
EVT_MENU(MENU_TREE_DELETE, ObjectTree::OnDelete)
END_EVENT_TABLE()

ObjectTree::ObjectTree(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
  :wxTreeCtrl(parent, id, pos, size, style, validator, name)
{
  BuildRoot();

  // The map menu
  treeMenu = new wxMenu;
  treeMenu->Append(MENU_TREE_DELETE, _T("&Delete...\tAlt-X"), _T("Delete the selected object"));
  treeMenu->AppendSeparator();
  treeMenu->Append(MENU_ADD_ROOM, _T("Add &Room...\tAlt-R"), _T("Add a new room"));
  treeMenu->Append(MENU_ADD_ACTOR, _T("Add &Actor...\tAlt-A"), _T("Add a new actor"));
  treeMenu->Append(MENU_ADD_OBJECT, _T("Add &Object...\tAlt-O"), _T("Add a new object"));
  treeMenu->Append(MENU_ADD_CLASS, _T("Add &Class...\tAlt-C"), _T("Add a new class"));
  treeMenu->Append(MENU_ADD_FUNCTION, _T("Add &Function...\tAlt-F"), _T("Add a new function"));
  treeMenu->Append(MENU_ADD_SOURCEFILE, _T("Add &Source file...\tAlt-S"), _T("Add a new source file"));
}

ObjectTree::~ObjectTree()
{
  delete treeMenu;
}

bool ObjectTree::Find(int flags, const wxString& text)
{
  InformData *o=frame->project.FindObject(text);
  if(o==NULL) {
    wxLogWarning("Cannot find object '%s'", text.c_str());
    return true;
  } else {
    SelectItem(o->treeId);
  }
  return false;
}

bool ObjectTree::FindNext(int flags, const wxString& text)
{
  return Find(flags, text);
}

void ObjectTree::AddNode(InformData *object, bool select)
{
  bool plainTree=false;
  wxMenuBar *menu=frame->GetMenuBar();
  if(menu!=NULL && menu->IsChecked(MENU_VIEW_TREE_HIERARCHY)) plainTree=true;

  wxTreeItemId p=root;

  // Check if already inserted
  if(object->treeId!=root) return;

  if(!plainTree) {
    // Recursively call AddNode for the parent, if any
    wxString location=object->GetSource(INF_PARENT);
    if(!location.IsEmpty()) {
      InformData *parent=frame->project.FindObject(location);
      if(parent) {
        if(parent->treeId==root) AddNode(parent, select);
        p=parent->treeId;
      }
    }
  }

  //wxLogDebug("Adding %s\n", object->GetSource(INF_ID).c_str());
  switch(object->type) {
  case INF_T_ROOM:
    if(p==root) p=rooms;
    object->treeId=AppendItem(p, object->GetSource(INF_ID), -1, -1, new ObjectTreeData(object, frame->roomNotebook));
    break;
  case INF_T_ACTOR:
    if(p==root) p=actors;
    object->treeId=AppendItem(p, object->GetSource(INF_ID), -1, -1, new ObjectTreeData(object, frame->actorNotebook));
    break;
  case INF_T_ITEM:
    if(p==root) p=objects;
    object->treeId=AppendItem(p, object->GetSource(INF_ID), -1, -1, new ObjectTreeData(object, frame->objectNotebook));
    break;
  case INF_T_CLASS:
    if(p==root) p=classes;
    object->treeId=AppendItem(p, object->GetSource(INF_ID), -1, -1, new ObjectTreeData(object, frame->classNotebook));
    break;
  case INF_T_FUNCTION:
    if(p==root) p=functions;
    object->treeId=AppendItem(p, object->GetSource(INF_ID), -1, -1, new ObjectTreeData(object, frame->functionNotebook));
    break;
  case INF_T_SOURCEFILE:
    if(p==root) p=sourcefiles;
    object->treeId=AppendItem(p, ((SourceFile *) object)->Name(), -1, -1, new ObjectTreeData(object, frame->sourceNotebook));
    break;
  default:
    wxLogDebug("ERROR: TODO: missing type in ObjectTree::AddNode");
  }

  Expand(p);

  if(select) {
    SelectItem(object->treeId);
  }
}

void ObjectTree::RemoveNode(wxTreeItemId id)
{
  Delete(id);
}

void ObjectTree::UpdateNode(wxTreeItemId id, InformData *object)
{
  ObjectTreeData *item=(ObjectTreeData *) GetItemData(id);
  item->data=object;
  SetItemText(id, object->GetSource(INF_ID));
}

void ObjectTree::OnChanging(wxTreeEvent& event)
{
  ObjectTreeData *olditem=(ObjectTreeData *) GetItemData(event.GetOldItem());

  // Save old data before changing object
  if(olditem) {
    if(olditem->notebook->SavePageData(-1, &event)) event.Veto();
  }
}

void ObjectTree::OnChanged(wxTreeEvent& event)
{
  ObjectTreeData *newitem=(ObjectTreeData *) GetItemData(event.GetItem());

  // Change notebook if necessary
  InformNotebook *newNotebook=frame->blankNotebook;
  InformNotebook *currentNotebook=frame->GetNotebook();
  if(newitem) newNotebook=newitem->notebook;

  if(currentNotebook!=newNotebook) {
    // Silly that you have to hide and show the window manually.
    // This should be done by ReplaceWindow()
    currentNotebook->Hide();
    frame->mainSplitter->ReplaceWindow(currentNotebook, newNotebook);
    newNotebook->Show();
  }

  // Show new object
  if(newitem) newNotebook->LoadPageData(-1, newitem->data);
}

void ObjectTree::OnRightClick(wxTreeEvent& event)
{
  PopupMenu(treeMenu, event.GetPoint());
}

void ObjectTree::OnDelete(wxCommandEvent& event)
{
  bool fail=false;
  InformData *obj=NULL;

  wxTreeItemId id=GetSelection();
  if(!id.IsOk()) {
    fail=true;
  } else {
    ObjectTreeData *node=(ObjectTreeData *) GetItemData(id);
    if(node==NULL) {
      fail=true;
    } else {
      obj=node->data;
      if(obj==NULL || node->notebook==frame->settingsNotebook) {
        fail=true;
      }
    }
  }

  if(fail) {
    wxMessageBox("This tree item cannot be deleted", "Delete error");
  } else {
    frame->project.DeleteObject(obj);
  }
}

void ObjectTree::BuildRoot()
{
  root=AddRoot("Project Data", -1, -1, NULL);
  settings=AppendItem(root, "Settings", -1, -1, NULL);
  rooms=AppendItem(root, "Rooms", -1, -1, NULL);
  objects=AppendItem(root, "Objects", -1, -1, NULL);
  actors=AppendItem(root, "Actors", -1, -1, NULL);
  classes=AppendItem(root, "Classes", -1, -1, NULL);
  functions=AppendItem(root, "Functions", -1, -1, NULL);
  sourcefiles=AppendItem(root, "Source Files", -1, -1, NULL);
}

void ObjectTree::BuildTree(const wxString& label) 
{
  //wxLogDebug("Building tree\n");
//  wxStopWatch sw;
  DeleteAllItems();

  BuildRoot();
  SetItemData(settings, new ObjectTreeData(frame->project.GetSourceFile(0), frame->settingsNotebook));

  wxTreeItemId selectionID=root;
  if(GetItemText(settings).IsSameAs(label)) selectionID=settings;

  // Setting default so AddNode will know if this items has been added
  unsigned int i;
  for(i=0; i<frame->project.objects.GetCount(); i++) {
    frame->project.objects[i].treeId=root;
  }

  for(i=0; i<frame->project.objects.GetCount(); i++) {
    InformData *current=&frame->project.objects[i];
    AddNode(current, false);
    if(GetItemText(current->treeId).IsSameAs(label)) selectionID=current->treeId;
  }

  Expand(root);
  SelectItem(selectionID);
//  wxLogDebug("BuildTree took %ldms to execute", sw.Time());
}
