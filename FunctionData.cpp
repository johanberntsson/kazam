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

#include "FunctionData.h"

FunctionData::FunctionData()
{
  type=INF_T_FUNCTION;
}

wxString FunctionData::ToString(void)
{
  return GetSource(INF_ID);
}
