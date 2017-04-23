#ifndef _BLANKNOTEBOOK_H
#define _BLANKNOTEBOOK_H

#include "InformNotebook.h"

class BlankNotebook : public InformNotebook
{
public:
  BlankNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "notebook");

  void LoadPageData(int page=-1, InformData *data=NULL);
  bool SavePageData(int page=-1, wxTreeEvent *event=NULL);
};

#endif
