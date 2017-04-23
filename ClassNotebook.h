#ifndef _CLASSNOTEBOOK_H
#define _CLASSNOTEBOOK_H

#include "InformEditor.h"
#include "InformNotebook.h"

class ClassNotebook : public InformNotebook
{
public:
  ClassNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "notebook");

  void LoadPageData(int page=-1, InformData *data=NULL);
  bool SavePageData(int page=-1, wxTreeEvent *event=NULL);

private:
  ObjectData *classobject;
  InformEditor *sourceEditor;
};

#endif
