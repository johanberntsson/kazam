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
#include "ObjectData.h"

ObjectData::ObjectData()
{
  type=INF_T_ITEM;

  for(int i=0; i<12; i++) {
    dirCache[i].ok=false;
  }

  // Meta defaults
  mRoomX=-1;
  mRoomY=-1;
  selectedConnection=-1; // same as MapWidget::DragMode::NONE
}

wxString ObjectData::ToString(void)
{
  wxString v=GetSource(INF_ID)+" "+GetSource(INF_NAME)+" "+GetSource(INF_PARENT);
  return v;
}
