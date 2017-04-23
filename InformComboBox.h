#ifndef _INFORMCOMBOBOX_H
#define _INFORMCOMBOBOX_H

#include <wx/combobox.h>

class InformComboBox: public wxComboBox 
{
public:
  InformComboBox(wxWindow* parent, wxWindowID id,
    const wxString& value = "", const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize, int n=0, const wxString choices[]=NULL,
    long style = 0, const wxValidator& validator = wxDefaultValidator,
    const wxString& name = "comboBox");    

private:
  void OnLeftDoubleClick(wxMouseEvent& event);

  DECLARE_EVENT_TABLE()
};

#endif