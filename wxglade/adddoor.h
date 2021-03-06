// -*- C++ -*- generated by wxGlade 0.3.1 on Mon Oct 20 10:21:28 2003

#include <wx/wx.h>
#include <wx/image.h>
// begin wxGlade: ::dependencies
// end wxGlade


#ifndef ADDDOOR_H
#define ADDDOOR_H


class AddDoor: public wxDialog {
public:
    // begin wxGlade: AddDoor::ids
    // end wxGlade

    AddDoor(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

private:
    // begin wxGlade: AddDoor::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: AddDoor::attributes
    wxStaticText* labelFrom;
    wxTextCtrl* text_ctrl_1;
    wxStaticText* labelTo;
    wxTextCtrl* text_ctrl_2;
    wxStaticText* labelStatus;
    wxCheckBox* checkbox_1;
    wxCheckBox* checkbox_2;
    wxStaticText* labelState;
    wxCheckBox* checkbox_3;
    wxCheckBox* checkbox_4;
    wxStaticText* labelKey;
    InformCombo* comboKey;
    wxButton* buttonCreateKey;
    wxButton* buttonOK;
    wxButton* buttonCancel;
    // end wxGlade
};


#endif // ADDDOOR_H
