#ifndef _KAZAM_H
#define _KAZAM_H

#define VERSION "0.2"
#define URL "http://www.geocities.com/milbus/kazam.htm"

#ifdef WIN32
#define snprintf _snprintf
#define strncasecmp strnicmp
#endif

#include <wx/dnd.h>
#include <wx/splitter.h>
#include <wx/docview.h>
#include <wx/html/helpfrm.h>
#include <wx/html/helpctrl.h>

#ifdef wxUSE_FINDREPLDLG
#include <wx/fdrepdlg.h>
#endif

#include "ObjectTree.h"
#include "Project.h"

class InformNotebook;

enum {
  MENU_FILE_QUIT = 1,
  MENU_FILE_NEW,
  MENU_FILE_OPEN,
  MENU_FILE_SAVE,
  MENU_FILE_ADD,
  MENU_ADD_ROOM,
  MENU_ADD_ACTOR,
  MENU_ADD_OBJECT,
  MENU_ADD_CLASS,
  MENU_ADD_FUNCTION,
  MENU_ADD_SOURCEFILE,
  MENU_TREE_DELETE,
  MENU_EDIT_UNDO,
  MENU_EDIT_REDO,
  MENU_EDIT_CUT,
  MENU_EDIT_COPY,
  MENU_EDIT_PASTE,
#ifdef wxUSE_FINDREPLDLG
  MENU_EDIT_FIND,
  MENU_EDIT_REPLACE,
#endif
  MENU_BUILD_PREFERENCES,
  MENU_VIEW_TOOLBAR,
  MENU_VIEW_STATUSBAR,
  MENU_VIEW_LOGWINDOW,
  MENU_VIEW_TREE_HIERARCHY,
  MENU_VIEW_FINDOBJECT,
  MENU_BUILD_COMPILE,
  MENU_BUILD_EXECUTE,
  MENU_MAP_ADD,
  MENU_MAP_DELETE,
  MENU_MAP_ZOOM100,
  MENU_MAP_ZOOM50,
  MENU_MAP_ZOOM10,
  MENU_MAP_ZOOMIN,
  MENU_MAP_ZOOMOUT,
  MENU_MAP_AUTOARRANGE,
  MENU_MAP_INCREASE_CANVAS,
  TREE_PROJECT,
  MENU_HELP_CONTENTS,
  MENU_HELP_HOMEPAGE,
  MENU_HELP_ABOUT = wxID_ABOUT
};

class Kazam : public wxApp
{
public:
  virtual bool OnInit();
private:
};

class FileDropTarget : public wxFileDropTarget 
{
  bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
};

class MainFrame : public wxFrame
{
public:
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
    long style = wxDEFAULT_FRAME_STYLE);
  ~MainFrame();

  void ShowToolMenu(wxMenu *menu, const wxString& title);
  void HideToolMenu(const wxString& title, wxMenu **menu);
  wxMenu *mapMenu;
  wxMenu *editMenu;

  Project project;

  // All notebooks are created and deleted by MainFrame
  InformNotebook *blankNotebook;
  InformNotebook *roomNotebook;
  InformNotebook *classNotebook;
  InformNotebook *actorNotebook;
  InformNotebook *objectNotebook;
  InformNotebook *sourceNotebook;
  InformNotebook *functionNotebook;
  InformNotebook *settingsNotebook;

  // Returns the currently used notebook
  InformNotebook *GetNotebook();
  wxWindow *IsMapActive();
  wxWindow *IsEditorActive();

  ObjectTree* objectTree;
  wxSplitterWindow* frameSplitter;
  wxSplitterWindow* mainSplitter;
  wxTextCtrl* logWindow;

  wxMenuBar *menuBar;
  wxFileHistory *filehistory;

  // preferences
  wxString author;
  wxString cmdCompiler;
  wxString cmdLibrary;
  wxString cmdExecute;
  wxString activeProject;
  wxString templatePath;
  bool useActiveProject;
  bool useTabs;
  int indentSize;
  bool warnOnExit;

  void LoadPreferences();
  void SavePreferences();

private:
#ifdef wxUSE_FINDREPLDLG
  wxFindReplaceData findData;
  wxFindReplaceDialog *dlgFind;
  wxFindReplaceDialog *dlgReplace;
  void OnEditFind(wxCommandEvent& event);
  void OnEditReplace(wxCommandEvent& event);
  void OnFindDialog(wxFindDialogEvent& event);
  void OnUpdateEditFind(wxUpdateUIEvent& event);
  void OnUpdateEditReplace(wxUpdateUIEvent& event);
#endif

  wxHtmlHelpController help;

  // event handlers (these functions should _not_ be virtual)
  void OnClose(wxCloseEvent& event);
  void OnNew(wxCommandEvent& event);
  void OnOpen(wxCommandEvent& event);
  void OnOpenMRU(wxCommandEvent& event);
  void OnSave(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);
  void OnCompile(wxCommandEvent& event);
  void OnExecute(wxCommandEvent& event);
  void OnHelpContents(wxCommandEvent& event);
  void OnHelpHomePage(wxCommandEvent& event);
  void OnHelpAbout(wxCommandEvent& event);
  void OnAddRoom(wxCommandEvent& event);
  void OnAddActor(wxCommandEvent& event);
  void OnAddObject(wxCommandEvent& event);
  void OnAddClass(wxCommandEvent& event);
  void OnAddFunction(wxCommandEvent& event);
  void OnAddSourceFile(wxCommandEvent& event);
  void OnEditUndo(wxCommandEvent& event);
  void OnEditRedo(wxCommandEvent& event);
  void OnEditCut(wxCommandEvent& event);
  void OnEditCopy(wxCommandEvent& event);
  void OnEditPaste(wxCommandEvent& event);
  void OnEditPreferences(wxCommandEvent& event);
  void OnViewToolbar(wxCommandEvent& event);
  void OnViewStatusbar(wxCommandEvent& event);
  void OnViewLogWindow(wxCommandEvent& event);
  void OnViewTree(wxCommandEvent& event);
  void OnViewFindObject(wxCommandEvent& event);
  void OnUpdateEditUndo(wxUpdateUIEvent& event);
  void OnUpdateEditRedo(wxUpdateUIEvent& event);
  void OnUpdateEditCut(wxUpdateUIEvent& event);
  void OnUpdateEditCopy(wxUpdateUIEvent& event);
  void OnUpdateEditPaste(wxUpdateUIEvent& event);
  void OnUpdateViewToolbar(wxUpdateUIEvent& event);
  void OnUpdateViewStatusbar(wxUpdateUIEvent& event);
  void OnUpdateViewLogWindow(wxUpdateUIEvent& event);
  void OnUpdateViewTree(wxUpdateUIEvent& event);

  void OnMapMenu(wxCommandEvent& event);
  void OnUpdateMapZoom(wxUpdateUIEvent& event);

  // any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
