#ifndef _OBJECTNOTEBOOK_H
#define _OBJECTNOTEBOOK_H

#include "InformEditor.h"
#include "InformNotebook.h"

class ObjectNotebook : public InformNotebook
{
public:
  ObjectNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "notebook");

  void LoadPageData(int page=-1, InformData *data=NULL);
  bool SavePageData(int page=-1, wxTreeEvent *event=NULL);

private:
  ObjectData *object;

  wxComboBox* comboClass;
  wxComboBox* comboLocation;
  wxTextCtrl* editID;
  wxTextCtrl* editName;
  wxTextCtrl* editWords;
  wxTextCtrl* editDescription;
  wxCheckBox* checkboxGeneral;
  wxCheckBox* checkboxScenery;
  wxCheckBox* checkboxConcealed;
  wxCheckBox* checkboxAnimate;
  wxCheckBox* checkboxLight;
  wxCheckBox* checkboxClothing;
  wxCheckBox* checkboxWorn;
  wxCheckBox* checkboxFemale;
  wxCheckBox* checkboxTransparent;

  wxComboBox* comboFilter;
  wxListBox* listboxUsed;
  wxButton* buttonAdd;
  wxButton* buttonDelete;
  wxListBox* listboxAvailable;
  wxButton* buttonCode;

  InformEditor *sourceEditor;
};

#endif
