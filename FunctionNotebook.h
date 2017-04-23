#ifndef _FUNCTIONNOTEBOOK_H
#define _FUNCTIONNOTEBOOK_H

#include "InformEditor.h"
#include "InformNotebook.h"

class FunctionNotebook : public InformNotebook
{
public:
  FunctionNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "notebook");

  void LoadPageData(int page=-1, InformData *data=NULL);
  bool SavePageData(int page=-1, wxTreeEvent *event=NULL);

private:
  FunctionData *function;
  InformEditor *sourceEditor;
};

#endif
