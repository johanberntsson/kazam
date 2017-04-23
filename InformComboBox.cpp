/*
* Copyright (c) 2003 Johan Berntsson, j.berntsson@qut.edu.au
* Queensland University of Technology, Brisbane, Australia
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/

/* This class works around the initiation problem of wxComboBox */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "InformComboBox.h"

BEGIN_EVENT_TABLE(InformComboBox, wxComboBox)
EVT_LEFT_DOWN(InformComboBox::OnLeftDoubleClick)
EVT_RIGHT_DOWN(InformComboBox::OnLeftDoubleClick)
EVT_LEFT_DCLICK(InformComboBox::OnLeftDoubleClick)
END_EVENT_TABLE()

// TODO: Another stupid workaround for a wxWindows bug.
// Clear() and Append() does not update the dropdown
// box size, and we have to set the expected dropdown
// size by adding dummies in the constructor.
wxString comboClasses[] = { "1", "2", "3", "4", "5" };

InformComboBox::InformComboBox(wxWindow* parent, wxWindowID id,
    const wxString& value, const wxPoint& pos,
    const wxSize& size, int n, const wxString choices[],
    long style, const wxValidator& validator,
    const wxString& name) :
  wxComboBox(parent, id, value, pos, size, 5, comboClasses,
    style, validator, name)
{
}

void InformComboBox::OnLeftDoubleClick(wxMouseEvent& event)
{
  wxLogDebug("Combo click event %d\n", event.GetEventType());
  event.Skip();
}
