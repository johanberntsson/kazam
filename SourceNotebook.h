#ifndef _SOURCENOTEBOOK_H
#define _SOURCENOTEBOOK_H

#include "InformEditor.h"
#include "InformNotebook.h"

class SourceNotebook : public InformNotebook
{
public:
  SourceNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "notebook");

  void LoadPageData(int page=-1, InformData *data=NULL);
  bool SavePageData(int page=-1, wxTreeEvent *event=NULL);

private:
  SourceFile *source;
  InformEditor *sourceEditor;
};

#endif
