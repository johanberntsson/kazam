#ifndef _INFORMNOTEBOOK_H
#define _INFORMNOTEBOOK_H

#include <wx/notebook.h>
#include "InformData.h"

#define VGAP 5
#define HGAP 10

class InformNotebook : public wxNotebook
{
public:
  InformNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "notebook");

  // page=-1 for current page, or page number.
  // the internal data object will be set to data, if not NULL
  virtual void LoadPageData(int page=-1, InformData *data=NULL)=0;

  // page=-1 for current, or page number.
  // returns true if error (non-complete data)
  virtual bool SavePageData(int page=-1, wxTreeEvent *event=NULL)=0;

  void SetCheckbox(wxCheckBox *cb, int type, InformData *object);
  void GetCheckbox(wxCheckBox *cb, int type, InformData *object);
  void SetEdit(wxTextCtrl *edit, int type, InformData *object, bool informString, bool strings);
  bool GetEdit(wxTextCtrl *edit, int type, InformData *object, bool informString, bool strings, const wxString& emptyMessage="");

  void SetComboDirs(wxComboBox *combo, InformData *object, int type);
  void SetComboClass(wxComboBox *combo, InformData *object);
  void SetComboLocation(wxComboBox *combo, InformData *object, bool onlyRooms);

  void GetComboDirs(wxComboBox *combo, InformData *object, int type);
  void GetComboClass(wxComboBox *combo, InformData *object);
  void GetComboLocation(wxComboBox *combo, InformData *object);

  wxString InformToC(const wxString& informdata, bool *isString=NULL);
  wxString CToInform(const wxString& cdata, bool isString=true, unsigned int width=30, unsigned int indentLevel=3);

  wxString InformToCStrings(const wxString& data);
  wxString CToInformStrings(const wxString& data);

private:
  void OnSelChanging(wxNotebookEvent& event);
  void OnSelChanged(wxNotebookEvent& event);

  // any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
