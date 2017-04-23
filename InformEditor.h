#ifndef _INFORMEDITOR_H
#define _INFORMEDITOR_H

// Comment out USE_STC if you want to use wxTextWidget
// instead of wxStyledWidget (Scintilla for wxWindows)
#define USE_STC

#ifdef USE_STC
  #include <wx/stc/stc.h>
  class InformEditor : public wxStyledTextCtrl
#else
  #include <wx/textctrl.h>
  class InformEditor : public wxTextCtrl
#endif
{
public:
  InformEditor(wxWindow* parent, wxWindowID id);

  wxString GetValue();
  void SetValue(const wxString& text);

  bool GetModified();
  void SetModified(bool modified);

  bool CanUndo();
  bool CanRedo();
  bool CanCut();
  bool CanCopy();
  bool CanPaste();

  void Undo();
  void Redo();
  void Cut();
  void Copy();
  void Paste();

  bool Find(int flags, const wxString& text);
  bool FindNext(int flags, const wxString& text);
  bool Replace(int flags, const wxString& text, const wxString& newText);
  bool ReplaceAll(int flags, const wxString& text, const wxString& newText);
};


#endif