#ifndef OBJECTTREE_H
#define OBJECTTREE_H

#include <wx/treectrl.h>

#include "Project.h"
#include "InformNotebook.h"

class ObjectTreeData : public wxTreeItemData
{
public:
  ObjectTreeData(InformData *d, InformNotebook *nb) {
    data=d;
    notebook=nb;
  }

  InformData *data;
  InformNotebook *notebook;
};

class ObjectTree : public wxTreeCtrl
{
public:
  ObjectTree(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listCtrl");
  ~ObjectTree();

  void BuildTree(const wxString& label="");

  void AddNode(InformData *object, bool select);
  void RemoveNode(wxTreeItemId id);
  void UpdateNode(wxTreeItemId id, InformData *object);

  bool Find(int flags, const wxString& text);
  bool FindNext(int flags, const wxString& text);

private:
  void BuildRoot();

  void OnChanging(wxTreeEvent& event);
  void OnChanged(wxTreeEvent& event);
  void OnRightClick(wxTreeEvent& event);
  void OnDelete(wxCommandEvent& event);

  wxTreeItemId root;
  wxTreeItemId settings;
  wxTreeItemId rooms;
  wxTreeItemId objects;
  wxTreeItemId actors;
  wxTreeItemId classes;
  wxTreeItemId functions;
  wxTreeItemId sourcefiles;

  wxMenu *treeMenu;

  // any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
