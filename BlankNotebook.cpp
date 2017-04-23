/*
* Copyright (c) 2003 Johan Berntsson, j.berntsson@qut.edu.au
* Queensland University of Technology, Brisbane, Australia
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Project.h"
#include "BlankNotebook.h"

BlankNotebook::BlankNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
  : InformNotebook(parent, id, pos, size, style, name)
{
}

void BlankNotebook::LoadPageData(int page, InformData *data)
{
}

bool BlankNotebook::SavePageData(int page, wxTreeEvent *event)
{
  return false;
}
