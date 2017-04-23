// -*- C++ -*- generated by wxGlade 0.3.1 on Mon Oct 20 12:00:39 2003

#include <wx/wx.h>
#include <wx/image.h>

#ifndef DLGADDDOOR_H
#define DLGADDDOOR_H

#include "ObjectData.h"
#include "InformComboBox.h"

enum {
  BUTTON_NEWDOOR=1,
  BUTTON_NEWKEY
};

// begin wxGlade: dependencies
// end wxGlade


class DlgAddDoor: public wxDialog {
public:
    // begin wxGlade: DlgAddDoor::ids
    // end wxGlade

    DlgAddDoor(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

    void SetValue(const wxString& from, const wxString& to, int fromDir, int toDir);
    ObjectData *GetValue();

private:
    int fromDir, toDir;
    void FillKeyCombo(wxComboBox *combo, const wxString &showID);
    void FillDoorCombo(wxComboBox *combo, const wxString &showID);
    void OnOK(wxCommandEvent& event);
    void OnNewKey(wxCommandEvent &event);
    void OnNewDoor(wxCommandEvent &event);

    // begin wxGlade: DlgAddDoor::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: DlgAddDoor::attributes
    wxStaticText* label_1;
    InformComboBox* comboDoor;
    wxButton* buttonNewDoor;
    wxStaticText* label_2;
    wxStaticText* labelFrom;
    wxStaticText* label_3;
    wxStaticText* labelTo;
    wxStaticText* label_4;
    wxCheckBox* checkboxOpenable;
    wxCheckBox* checkboxOpen;
    wxStaticText* label_5;
    wxCheckBox* checkboxLockable;
    wxCheckBox* checkboxLocked;
    wxStaticText* label_6;
    InformComboBox* comboKey;
    wxButton* buttonNewKey;
    wxButton* buttonOK;
    wxButton* buttonCancel;
    // end wxGlade
  // any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};


#endif // DLGADDDOOR_H
