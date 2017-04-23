#ifndef _NOTEBOOKBASE_H
#define _NOTEBOOKBASE_H

#include <wx/notebook.h>
#include <wx/treectrl.h>

#define HGAP 10
#define VGAP 5

class NotebookBase : public wxTreeItemData
{
public:
  virtual void Show(wxNotebook *notebook)=0;
  virtual void Hide(wxNotebook *notebook)=0;
};

#endif
