#ifndef _SETTINGSNOTEBOOK_H
#define _SETTINGSNOTEBOOK_H

#include "InformNotebook.h"

class SettingsNotebook : public InformNotebook
{
public:
  SettingsNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "notebook");

  void LoadPageData(int page=-1, InformData *data=NULL);
  bool SavePageData(int page=-1, wxTreeEvent *event=NULL);

private:
  SourceFile *source;
  wxTextCtrl* editStory;
  wxTextCtrl* editHeadline;
  wxComboBox* comboStart;
  wxTextCtrl* editIntroduction;
};

#endif
