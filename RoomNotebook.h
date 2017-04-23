#ifndef _ROOMNOTEBOOK_H
#define _ROOMNOTEBOOK_H

#include <wx/textctrl.h>

#include "MapWidget.h"
#include "ObjectData.h"
#include "InformEditor.h"
#include "InformComboBox.h"
#include "InformNotebook.h"

class RoomNotebook : public InformNotebook
{
public:
  RoomNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "notebook");

  void LoadPageData(int page=-1, InformData *data=NULL);
  bool SavePageData(int page=-1, wxTreeEvent *event=NULL);

  MapWidget *map; // TODO: make private when ZoomIn/Out works

private:
  ObjectData *room;
  wxTextCtrl* editID;
  wxTextCtrl* editName;
  wxTextCtrl* editScenery;
  wxTextCtrl* editDescription;
  InformComboBox* comboClass;
  InformComboBox* comboNW;
  InformComboBox* comboN;
  InformComboBox* comboNE;
  InformComboBox* comboW;
  InformComboBox* comboE;
  InformComboBox* comboSW;
  InformComboBox* comboS;
  InformComboBox* comboSE;
  InformComboBox* comboUp;
  InformComboBox* comboDown;
  InformComboBox* comboIn;
  InformComboBox* comboOut;
  wxCheckBox* checkboxLight;

  InformEditor *sourceEditor;
};

#endif
