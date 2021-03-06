// -*- C++ -*- generated by wxGlade 0.3.1 on Fri Oct 31 17:57:26 2003

#include <wx/wx.h>
#include <wx/image.h>
// begin wxGlade: ::dependencies
// end wxGlade

#include "ObjectData.h"
#include "InformComboBox.h"

#ifndef DLGNEWOBJECT_H
#define DLGNEWOBJECT_H


class NewObjectDialog: public wxDialog {
public:
    // begin wxGlade: NewObjectDialog::ids
    // end wxGlade

    NewObjectDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

    wxTextCtrl* editID;
    InformComboBox* comboLocation;
    InformComboBox* comboFile;

private:
    // begin wxGlade: NewObjectDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

    void OnOK(wxCommandEvent& event);

protected:
    // begin wxGlade: NewObjectDialog::attributes
    wxStaticText* labelIntro;
    wxStaticText* labelID;
    wxStaticText* labelLocation;
    wxStaticText* labelSourceFile;
    wxButton* buttonOK;
    wxButton* buttonCancel;
    // end wxGlade
  // any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};


#endif // DLGNEWOBJECT_H
