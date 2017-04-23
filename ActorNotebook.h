#ifndef _ACTORNOTEBOOK_H
#define _ACTORNOTEBOOK_H

#include "InformEditor.h"
#include "InformNotebook.h"

class ActorNotebook : public InformNotebook
{
public:
  ActorNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "notebook");

  void LoadPageData(int page=-1, InformData *data=NULL);
  bool SavePageData(int page=-1, wxTreeEvent *event=NULL);

private:
  ObjectData *actor;

  wxComboBox* comboClass;
  wxComboBox* comboLocation;
  wxTextCtrl* editID;
  wxTextCtrl* editName;
  wxTextCtrl* editWords;
  wxTextCtrl* editDescription;
  wxTextCtrl* editLife;
  wxCheckBox* checkboxProper;
  wxCheckBox* checkboxFemale;
  wxCheckBox* checkboxConcealed;

  InformEditor *sourceEditor;
};

#endif
