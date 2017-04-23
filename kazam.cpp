/*
* Copyright (c) 2003 Johan Berntsson, j.berntsson@qut.edu.au
* Queensland University of Technology, Brisbane, Australia
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/notebook.h>
#include <wx/filedlg.h>
#include <wx/textfile.h>
#include <wx/config.h>
#include <wx/textdlg.h>
#include <wx/fs_zip.h> 

#ifdef __WXMAC__
#include <Carbon/Carbon.h>
#endif

#include "kazam.h"
#include "InformData.h"
#include "DlgPreferences.h"
#include "DlgNewObject.h"
#include "SourceFile.h"

#include "BlankNotebook.h"
#include "RoomNotebook.h"
#include "ClassNotebook.h"
#include "ActorNotebook.h"
#include "ObjectNotebook.h"
#include "SourceNotebook.h"
#include "FunctionNotebook.h"
#include "SettingsNotebook.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
#include "winres/mondrian.xpm"
#include "winres/help.xpm"
#include "winres/compile.xpm"
#include "winres/execute.xpm"
#include "winres/room.xpm"
#include "winres/actor.xpm"
#include "winres/object.xpm"
#include "winres/class.xpm"
#include "winres/function.xpm"
#include "winres/source.xpm"
#include "winres/zoomin.xpm"
#include "winres/zoomout.xpm"
#endif

// Some global variables
MainFrame *frame;

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_CLOSE(MainFrame::OnClose)
EVT_MENU(MENU_FILE_NEW, MainFrame::OnNew)
EVT_MENU(MENU_FILE_OPEN, MainFrame::OnOpen)
EVT_MENU(MENU_FILE_SAVE, MainFrame::OnSave)
EVT_MENU(MENU_FILE_QUIT,  MainFrame::OnQuit)
EVT_MENU(MENU_ADD_ROOM,  MainFrame::OnAddRoom)
EVT_MENU(MENU_ADD_ACTOR,  MainFrame::OnAddActor)
EVT_MENU(MENU_ADD_OBJECT,  MainFrame::OnAddObject)
EVT_MENU(MENU_ADD_CLASS,  MainFrame::OnAddClass)
EVT_MENU(MENU_ADD_FUNCTION,  MainFrame::OnAddFunction)
EVT_MENU(MENU_ADD_SOURCEFILE,  MainFrame::OnAddSourceFile)
EVT_UPDATE_UI(MENU_EDIT_UNDO,  MainFrame::OnUpdateEditUndo)
EVT_UPDATE_UI(MENU_EDIT_REDO,  MainFrame::OnUpdateEditRedo)
EVT_UPDATE_UI(MENU_EDIT_CUT,  MainFrame::OnUpdateEditCut)
EVT_UPDATE_UI(MENU_EDIT_COPY,  MainFrame::OnUpdateEditCopy)
EVT_UPDATE_UI(MENU_EDIT_PASTE,  MainFrame::OnUpdateEditPaste)
EVT_MENU(MENU_EDIT_UNDO,  MainFrame::OnEditUndo)
EVT_MENU(MENU_EDIT_REDO,  MainFrame::OnEditRedo)
EVT_MENU(MENU_EDIT_CUT,  MainFrame::OnEditCut)
EVT_MENU(MENU_EDIT_COPY,  MainFrame::OnEditCopy)
EVT_MENU(MENU_EDIT_PASTE,  MainFrame::OnEditPaste)
EVT_MENU(MENU_BUILD_PREFERENCES,  MainFrame::OnEditPreferences)
EVT_MENU(MENU_VIEW_TOOLBAR,  MainFrame::OnViewToolbar)
EVT_MENU(MENU_VIEW_STATUSBAR,  MainFrame::OnViewStatusbar)
EVT_MENU(MENU_VIEW_LOGWINDOW,  MainFrame::OnViewLogWindow)
EVT_MENU(MENU_VIEW_TREE_HIERARCHY,  MainFrame::OnViewTree)
EVT_UPDATE_UI(MENU_VIEW_TOOLBAR,  MainFrame::OnUpdateViewToolbar)
EVT_UPDATE_UI(MENU_VIEW_STATUSBAR,  MainFrame::OnUpdateViewStatusbar)
EVT_UPDATE_UI(MENU_VIEW_LOGWINDOW,  MainFrame::OnUpdateViewLogWindow)
//EVT_UPDATE_UI(MENU_VIEW_TREE_HIERARCHY,  MainFrame::OnUpdateViewTree)
EVT_MENU(MENU_BUILD_COMPILE,  MainFrame::OnCompile)
EVT_MENU(MENU_BUILD_EXECUTE,  MainFrame::OnExecute)
EVT_MENU(MENU_HELP_CONTENTS, MainFrame::OnHelpContents)
EVT_MENU(MENU_HELP_HOMEPAGE, MainFrame::OnHelpHomePage)
EVT_MENU(MENU_HELP_ABOUT, MainFrame::OnHelpAbout)
EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, MainFrame::OnOpenMRU)
#ifdef wxUSE_FINDREPLDLG
EVT_MENU(MENU_EDIT_FIND, MainFrame::OnEditFind)
EVT_MENU(MENU_VIEW_FINDOBJECT, MainFrame::OnViewFindObject)
EVT_MENU(MENU_EDIT_REPLACE, MainFrame::OnEditReplace)
EVT_UPDATE_UI(MENU_EDIT_FIND, MainFrame::OnUpdateEditFind)
EVT_UPDATE_UI(MENU_EDIT_REPLACE, MainFrame::OnUpdateEditReplace)
EVT_FIND(-1, MainFrame::OnFindDialog)
EVT_FIND_NEXT(-1, MainFrame::OnFindDialog)
EVT_FIND_REPLACE(-1, MainFrame::OnFindDialog)
EVT_FIND_REPLACE_ALL(-1, MainFrame::OnFindDialog)
EVT_FIND_CLOSE(-1, MainFrame::OnFindDialog)
#endif
EVT_MENU_RANGE(MENU_MAP_ADD, MENU_MAP_INCREASE_CANVAS, MainFrame::OnMapMenu)
EVT_UPDATE_UI_RANGE(MENU_MAP_ZOOMIN, MENU_MAP_ZOOMOUT, MainFrame::OnUpdateMapZoom)
END_EVENT_TABLE()

IMPLEMENT_APP(Kazam)

// Find the absolute path where this application has been run from.
// argv0 is wxTheApp->argv[0]
// cwd is the current working directory (at startup)
// appVariableName is the name of a variable containing the directory for this app, e.g.
// MYAPPDIR. This is checked first.
wxString wxFindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName)
{
    wxString str;

    // Try appVariableName
    if (!appVariableName.IsEmpty())
    {
        str = wxGetenv(appVariableName);
        if (!str.IsEmpty())
            return str;
    }

#if defined(__WXMAC__) && !defined(__DARWIN__)
    // On Mac, the current directory is the relevant one when
    // the application starts.
    return cwd;
#endif

    if (wxIsAbsolutePath(argv0))
        return wxPathOnly(argv0);
    else
    {
        // Is it a relative path?
        wxString currentDir(cwd);
        if (currentDir.Last() != wxFILE_SEP_PATH)
            currentDir += wxFILE_SEP_PATH;

        str = currentDir + argv0;
        if (wxFileExists(str))
            return wxPathOnly(str);
    }

    // OK, it's neither an absolute path nor a relative path.
    // Search PATH.

    wxPathList pathList;
    pathList.AddEnvList(wxT("PATH"));
    str = pathList.FindAbsoluteValidPath(argv0);
    if (!str.IsEmpty())
        return wxPathOnly(str);

    // Failed
    return wxEmptyString;
}

bool Kazam::OnInit()
{
  // Setting CWD so that we can find the documentation file
	wxString cwd=::wxGetWorkingDirectory();
  wxString path=wxFindAppPath(argv[0], cwd, "");
  //wxLogMessage("cwd %s\n", cwd.c_str());
  if(!path.IsEmpty() && path.Find("Debug")==-1 && path.Find("Release")==-1) {
    ::wxSetWorkingDirectory(path);
  }
	//wxLogMessage("Path %d %s -> %s\n", argc, argv[0], path.c_str());

  frame = new MainFrame(_T("Kazam"), wxPoint(50, 50), wxSize(800, 600));
  frame->LoadPreferences();

  wxString file;
  if(argc==2) {
    file=argv[1];
  } else if(argc!=1) {
    wxFatalError("Wrong number of input arguments. Usage: kazam [inform source file]");
  } else if(frame->useActiveProject && !frame->activeProject.IsEmpty()) {
    file=frame->activeProject;
  }

  if(!file.IsEmpty() && !frame->project.Load(file)) {
    frame->objectTree->BuildTree("Settings");
  }

  frame->Show(TRUE);
  return TRUE;
}

bool FileDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
  if(filenames.GetCount()!=1) {
    wxLogError("Only one source file can be the project main file");
    return false;
  }

  InformNotebook *nb=(InformNotebook *) frame->mainSplitter->GetWindow2();
  nb->SavePageData();

  if(frame->warnOnExit && frame->project.IsModified()) {
    if(wxMessageBox(_T("The current project source files have changed.\n\nOpen new project anyway?"),
                    _T("KAZAM"),
                    wxYES_NO|wxICON_WARNING, frame)==wxNO) return false;
  }

  if(frame->project.Load(filenames[0])) {
    wxLogError("Load failed");
    return false;
  } else {
    frame->objectTree->BuildTree("Settings");
  }
  return true;
}

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
: wxFrame(NULL, -1, title, pos, size, style)
{
#ifdef wxUSE_FINDREPLDLG
  dlgFind=NULL;
  dlgReplace=NULL;
  findData.SetFlags(wxFR_DOWN);
#endif

  filehistory = new wxFileHistory(4);
  filehistory->Load(*wxConfigBase::Get());

#ifdef __WXMAC__
  // we need this in order to allow the about menu relocation, since ABOUT is
	// not the default id of the about menu
	wxApp::s_macAboutMenuItemId = Minimal_About;
#endif

  // set the frame icon
  SetIcon(wxICON(mondrian));

  wxMenu *addMenu = new wxMenu();
  addMenu->Append(MENU_ADD_ROOM, _T("&Room...\tAlt-R"), _T("Add a new room"));
  addMenu->Append(MENU_ADD_ACTOR, _T("&Actor...\tAlt-A"), _T("Add a new actor"));
  addMenu->Append(MENU_ADD_OBJECT, _T("&Object...\tAlt-O"), _T("Add a new object"));
  addMenu->Append(MENU_ADD_CLASS, _T("&Class...\tAlt-C"), _T("Add a new class"));
  addMenu->Append(MENU_ADD_FUNCTION, _T("&Function...\tAlt-F"), _T("Add a new function"));
  addMenu->Append(MENU_ADD_SOURCEFILE, _T("&Source file...\tAlt-S"), _T("Add a new source file"));

  wxMenu *fileMenu = new wxMenu();
  fileMenu->Append(MENU_FILE_NEW, _T("&New Project...\tCtrl-N"), _T("Create new project"));
  fileMenu->Append(MENU_FILE_OPEN, _T("&Open Project...\tCtrl-O"), _T("Open project files"));
  fileMenu->Append(MENU_FILE_SAVE, _T("&Save Project\tCtrl-S"), _T("Save project files"));
  fileMenu->AppendSeparator();
  fileMenu->Append(MENU_FILE_ADD, _T("&Add"), addMenu, _T("Add a project item"));
  fileMenu->AppendSeparator();
  fileMenu->Append(MENU_FILE_QUIT, _T("E&xit"), _T("Quit this program"));
  // Add MRU
  filehistory->UseMenu(fileMenu);
  filehistory->AddFilesToMenu(fileMenu);

  wxMenu *viewMenu = new wxMenu();
  viewMenu->AppendCheckItem(MENU_VIEW_TOOLBAR, _T("&Toolbar"), _T("Show or hide the toolbar"));
  viewMenu->AppendCheckItem(MENU_VIEW_STATUSBAR, _T("&Statusbar"), _T("Show or hide the statusbar"));
  viewMenu->AppendCheckItem(MENU_VIEW_LOGWINDOW, _T("&Log Window"), _T("Show or hide the log window"));

  wxMenu *buildMenu = new wxMenu();
  buildMenu->Append(MENU_BUILD_COMPILE, _T("&Compile\tF7"), _T("Compile"));
  buildMenu->Append(MENU_BUILD_EXECUTE, _T("&Execute\tF5"), _T("Compile and execute"));
  buildMenu->AppendSeparator();
  buildMenu->Append(MENU_VIEW_FINDOBJECT, _T("&Find Object...\tF3"), _T("Find a object"));
  buildMenu->AppendSeparator();
  buildMenu->Append(MENU_BUILD_PREFERENCES, _T("&Preferences..."), _T("Edit the preferences for this application"));
#ifdef wxUSE_FINDREPLDLG
  buildMenu->AppendCheckItem(MENU_VIEW_TREE_HIERARCHY, _T("Toggle object &browser mode"), _T("Choose between plain and hierachical browser view"));
#endif

  wxMenu *helpMenu = new wxMenu();
  helpMenu->Append(MENU_HELP_CONTENTS, _T("&Contents...\tF1"), _T("Show table of contents for documentation"));
  helpMenu->Append(MENU_HELP_HOMEPAGE, _T("Kazam &homepage..."), _T("Show the official Kazam homepage"));
  helpMenu->Append(MENU_HELP_ABOUT, _T("&About..."), _T("Show about dialog"));

  // now append the freshly created menu to the menu bar...
  menuBar = new wxMenuBar();
  menuBar->Append(fileMenu, "&File");
  menuBar->Append(buildMenu, "&Project");
  menuBar->Append(viewMenu, "&View");
  menuBar->Append(helpMenu, "&Help");

  // ... and attach this menu bar to the frame
  SetMenuBar(menuBar);

  // The edit menu
  editMenu = new wxMenu();
  editMenu->Append(MENU_EDIT_UNDO, _T("&Undo\tCtrl-Z"), _T("Undo the last change"));
  editMenu->Append(MENU_EDIT_REDO, _T("&Redo\tCtrl-Y"), _T("Undo the last undo"));
  editMenu->AppendSeparator();
  editMenu->Append(MENU_EDIT_CUT, _T("Cu&t\tCtrl-X"), _T("Cut the selection"));
  editMenu->Append(MENU_EDIT_COPY, _T("&Copy\tCtrl-C"), _T("Copy the selection to the clipboard"));
  editMenu->Append(MENU_EDIT_PASTE, _T("&Paste\tCtrl-V"), _T("Paste data from the clipboard into the application"));
#ifdef wxUSE_FINDREPLDLG
  editMenu->AppendSeparator();
  editMenu->AppendCheckItem(MENU_EDIT_FIND, _T("&Find..\tCtrl-F"), _T("Find data in the application"));
  editMenu->AppendCheckItem(MENU_EDIT_REPLACE, _T("&Replace..\tCtrl-H"), _T("Find and replace data in the application"));
#endif

  // The map menu
  mapMenu = new wxMenu;
  mapMenu->Append(MENU_MAP_ADD, _T("&Add..."), _T("Add a new room"));
  mapMenu->Append(MENU_MAP_DELETE, _T("&Delete..."), _T("Delete a room or connection"));
  mapMenu->Append(MENU_MAP_AUTOARRANGE, _T("&Auto arrange"), _T("Reposition all rooms"));
  mapMenu->Append(MENU_MAP_INCREASE_CANVAS, _T("&Increase canvas size"), _T("Make the map bigger"));
  mapMenu->AppendSeparator();
  mapMenu->Append(MENU_MAP_ZOOM100, _T("&Zoom 100%..."), _T("Set zoom to 100%"));
  mapMenu->Append(MENU_MAP_ZOOM50, _T("&Zoom 50%..."), _T("Set zoom to 100%"));
  mapMenu->Append(MENU_MAP_ZOOM10, _T("&Zoom 10%..."), _T("Set zoom to 100%"));
  mapMenu->Append(MENU_MAP_ZOOMIN, _T("&Zoom In..."), _T("Increase zoom"));
  mapMenu->Append(MENU_MAP_ZOOMOUT, _T("&Zoom Out..."), _T("Decrease zoom"));

#if wxUSE_TOOLBAR
    wxToolBar* toolbar = new wxToolBar(this, -1);
    SetToolBar(toolbar);

		wxBitmap toolBarBitmaps[11];
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    toolBarBitmaps[0]=wxBitmap(help_xpm);
    toolBarBitmaps[1]=wxBitmap(compile_xpm);
    toolBarBitmaps[2]=wxBitmap(execute_xpm);
    toolBarBitmaps[3]=wxBitmap(room_xpm);
    toolBarBitmaps[4]=wxBitmap(actor_xpm);
    toolBarBitmaps[5]=wxBitmap(object_xpm);
    toolBarBitmaps[6]=wxBitmap(class_xpm);
    toolBarBitmaps[7]=wxBitmap(function_xpm);
    toolBarBitmaps[8]=wxBitmap(source_xpm);
    toolBarBitmaps[9]=wxBitmap(zoomin_xpm);
    toolBarBitmaps[10]=wxBitmap(zoomout_xpm);
#else
    toolBarBitmaps[0]=wxBITMAP(HELP);
    toolBarBitmaps[1]=wxBITMAP(COMPILE);
    toolBarBitmaps[2]=wxBITMAP(EXECUTE);
    toolBarBitmaps[3]=wxBITMAP(ROOM);
    toolBarBitmaps[4]=wxBITMAP(ACTOR);
    toolBarBitmaps[5]=wxBITMAP(OBJECT);
    toolBarBitmaps[6]=wxBITMAP(CLASS);
    toolBarBitmaps[7]=wxBITMAP(FUNCTION);
    toolBarBitmaps[8]=wxBITMAP(SOURCE);
    toolBarBitmaps[9]=wxBITMAP(ZOOMIN);
    toolBarBitmaps[10]=wxBITMAP(ZOOMOUT);
#endif
    toolbar->AddTool(MENU_HELP_ABOUT, toolBarBitmaps[0], wxT("About"), wxT("Show About box"));
    toolbar->AddTool(MENU_BUILD_COMPILE, toolBarBitmaps[1], wxT("Build"), wxT("Compile the game"));
    toolbar->AddTool(MENU_BUILD_EXECUTE, toolBarBitmaps[2], wxT("Execute"), wxT("Execute the game"));
    toolbar->AddSeparator();
    toolbar->AddTool(MENU_ADD_ROOM, toolBarBitmaps[3], wxT("Room"), wxT("Add a new room"));
    toolbar->AddTool(MENU_ADD_ACTOR, toolBarBitmaps[4], wxT("Actor"), wxT("Add a new actor"));
    toolbar->AddTool(MENU_ADD_OBJECT, toolBarBitmaps[5], wxT("Object"), wxT("Add a new object"));
    toolbar->AddTool(MENU_ADD_CLASS, toolBarBitmaps[6], wxT("Class"), wxT("Add a new class"));
    toolbar->AddTool(MENU_ADD_FUNCTION, toolBarBitmaps[7], wxT("Function"), wxT("Add a new function"));
    toolbar->AddTool(MENU_ADD_SOURCEFILE, toolBarBitmaps[8], wxT("Source File"), wxT("Add a new source file"));
    toolbar->AddSeparator();
    toolbar->AddTool(MENU_MAP_ZOOMIN, toolBarBitmaps[9], wxT("Zoom In"), wxT("Increase zoom"));
    toolbar->AddTool(MENU_MAP_ZOOMOUT, toolBarBitmaps[10], wxT("Zoom Out"), wxT("Decrease zoom"));
    toolbar->Realize();
#endif // wxUSE_TOOLBAR

#if wxUSE_STATUSBAR
  // create a status bar just for fun (by default with 1 pane only)
  CreateStatusBar(2);
  SetStatusText("Welcome to Kazam!");
#endif // wxUSE_STATUSBAR

  frameSplitter = new wxSplitterWindow(this, -1);
  logWindow=new wxTextCtrl(frameSplitter, -1, "", wxDefaultPosition, wxSize(300,50), wxTE_MULTILINE|wxHSCROLL);
  mainSplitter = new wxSplitterWindow(frameSplitter, -1);
  objectTree = new ObjectTree(mainSplitter, TREE_PROJECT, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxSUNKEN_BORDER);

  // Create the notebooks
  blankNotebook = new BlankNotebook(mainSplitter, -1, wxDefaultPosition, wxDefaultSize, 0);
  roomNotebook = new RoomNotebook(mainSplitter, -1, wxDefaultPosition, wxDefaultSize, 0);
  classNotebook = new ClassNotebook(mainSplitter, -1, wxDefaultPosition, wxDefaultSize, 0);
  actorNotebook = new ActorNotebook(mainSplitter, -1, wxDefaultPosition, wxDefaultSize, 0);
  objectNotebook = new ObjectNotebook(mainSplitter, -1, wxDefaultPosition, wxDefaultSize, 0);
  sourceNotebook = new SourceNotebook(mainSplitter, -1, wxDefaultPosition, wxDefaultSize, 0);
  functionNotebook = new FunctionNotebook(mainSplitter, -1, wxDefaultPosition, wxDefaultSize, 0);
  settingsNotebook = new SettingsNotebook(mainSplitter, -1, wxDefaultPosition, wxDefaultSize, 0);

	roomNotebook->Hide();
	classNotebook->Hide();
	actorNotebook->Hide();
	objectNotebook->Hide();
	sourceNotebook->Hide();
	functionNotebook->Hide();
	settingsNotebook->Hide();

  frameSplitter->SplitHorizontally(mainSplitter, logWindow, (3*size.GetHeight())/5);
  mainSplitter->SplitVertically(objectTree, blankNotebook, size.GetWidth()/4);

  // Drag and drop
  SetDropTarget(new FileDropTarget());

  // Help files
  wxFileSystem::AddHandler(new wxZipFSHandler());
  help.UseConfig(wxConfig::Get());
  help.SetTempDir(wxT("."));
  if(!help.AddBook(wxFileName(wxT("kazam.htb"), wxPATH_UNIX)))
    wxMessageBox(wxT("Failed adding documentation file kazam.htb"));
}

MainFrame::~MainFrame()
{
  delete filehistory;

  // Remove the detachable menues
  HideToolMenu("&Map", &mapMenu);
  HideToolMenu("&Edit", &mapMenu);

  // Make sure that we know which notebook the splitter window will delete
  mainSplitter->ReplaceWindow(mainSplitter->GetWindow2(), blankNotebook);

  // Delete the other notebooks manually
  delete roomNotebook;
  delete classNotebook;
  delete actorNotebook;
  delete objectNotebook;
  delete sourceNotebook;
  delete functionNotebook;
  delete settingsNotebook;
  delete mapMenu;
  delete editMenu;
}

InformNotebook *MainFrame::GetNotebook()
{
  return (InformNotebook *) mainSplitter->GetWindow2();
}

void MainFrame::ShowToolMenu(wxMenu *menu, const wxString& title)
{
  wxMenuBar *menuBar=frame->GetMenuBar();
  if(menuBar==NULL) return;

  int menuIndex=menuBar->FindMenu(title);
  if(menuIndex==wxNOT_FOUND) {
    menuBar->Insert(1, menu, title);
  }
}

void MainFrame::HideToolMenu(const wxString& title, wxMenu **menu)
{
  wxMenuBar *menuBar=frame->GetMenuBar();
  if(menuBar==NULL) return;

  int menuIndex=menuBar->FindMenu(title);
  if(menuIndex!=wxNOT_FOUND) {
    *menu=menuBar->Remove(menuIndex);
  }
}

void MainFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
  InformNotebook *nb=(InformNotebook *) mainSplitter->GetWindow2();
  nb->SavePageData();

  if(warnOnExit && frame->project.IsModified()) {
    if(wxMessageBox(_T("The project source files have changed.\n\nOpen new files anyway?"),
                    _T("KAZAM"),
                    wxYES_NO|wxICON_WARNING, this)==wxNO) return;
  }

  wxFileDialog fd(this, "Open Inform source file", "", "",
    "Inform source file (*.inf)|*.inf", wxCHANGE_DIR);

  if(fd.ShowModal()==wxID_OK) {
    if(frame->project.Load(fd.GetPath())) {
      wxLogError("Load failed");
    } else {
      frame->objectTree->BuildTree("Settings");
    }
  }
}

void MainFrame::OnOpenMRU(wxCommandEvent& event)
{
  InformNotebook *nb=(InformNotebook *) mainSplitter->GetWindow2();
  nb->SavePageData();

  if(warnOnExit && frame->project.IsModified()) {
    if(wxMessageBox(_T("The project source files have changed.\n\nOpen new files anyway?"),
                    _T("KAZAM"),
                    wxYES_NO|wxICON_WARNING, this)==wxNO) return;
  }

  int i=event.GetId()-wxID_FILE1;
  wxString filename;
  if(i>=0 && (i<filehistory->GetNoHistoryFiles())) {
    if(frame->project.Load(filehistory->GetHistoryFile(i))) {
      wxLogError("Load failed");
    } else {
      frame->objectTree->BuildTree("Settings");
    }
  }
}

void MainFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
  InformNotebook *nb=(InformNotebook *) mainSplitter->GetWindow2();
  nb->SavePageData();

  frame->project.Save();
}

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  // TRUE is to force the frame to close
  Close(false);
}

void MainFrame::OnClose(wxCloseEvent& event)
{
  SavePreferences();
  if(event.CanVeto()) {
    InformNotebook *nb=(InformNotebook *) mainSplitter->GetWindow2();
    nb->SavePageData();

    if(warnOnExit && frame->project.IsModified()) {
      if(wxMessageBox(_T("The project source files have changed.\n\nQuit anyway?"),
                      _T("KAZAM"),
                      wxYES_NO|wxICON_WARNING, this)==wxNO) {
        event.Veto();
        return;
      }
    }
  }
  Destroy();
}

void MainFrame::OnHelpContents(wxCommandEvent& WXUNUSED(event))
{
  if(IsMapActive()) {
    help.Display("Map View");
  } else {
    help.Display("Contents");
  }
}

void MainFrame::OnHelpHomePage(wxCommandEvent& WXUNUSED(event))
{
  wxString url=URL;

#ifdef __WXMAC__
  ICInstance icInstance;
  OSType psiSignature = '????';
  OSStatus error = ICStart(&icInstance, psiSignature);
  if(error!=noErr) return;

  ConstStr255Param hint = 0;
  long length = url.Length();
  long start = 0;
  long end = length;
  ICLaunchURL( icInstance, hint, url.c_str(), length, & start, & end);
  ICStop( icInstance );
  return;
#endif

#ifdef __WXMSW__
     HKEY hKey;
     TCHAR szCmdName[1024];
     DWORD dwType, dw = sizeof(szCmdName);
     LONG lRes;
     lRes = RegOpenKey(HKEY_CLASSES_ROOT, "htmlfile\\shell\\open\\command", &hKey);
     if(lRes == ERROR_SUCCESS && RegQueryValueEx(hKey,(LPTSTR)NULL, NULL,
         &dwType, (LPBYTE)szCmdName, &dw) == ERROR_SUCCESS) {
         strcat(szCmdName, (const char*) url);
         PROCESS_INFORMATION  piProcInfo;
         STARTUPINFO          siStartInfo;
         memset(&siStartInfo, 0, sizeof(STARTUPINFO));
         siStartInfo.cb = sizeof(STARTUPINFO);
         CreateProcess(NULL, szCmdName, NULL, NULL, FALSE, NULL, NULL,
             NULL, &siStartInfo, &piProcInfo );
     }
     if(lRes == ERROR_SUCCESS) RegCloseKey(hKey);
#else
     wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(wxT("html"));
     if ( !ft )  {
         wxLogError(_T("Impossible to determine the file type for extension html."
                        " Please edit your MIME types."));
         return ;
     }

     wxString cmd;
     bool ok = ft->GetOpenCommand(&cmd, wxFileType::MessageParameters(url, _T("")));
     delete ft;

     if (!ok) {
         // TODO: some kind of configuration dialog here.
         wxMessageBox(_("Could not determine the command for running the browser."),
                    wxT("Browsing problem"), wxOK|wxICON_EXCLAMATION);
         return;
     }

     ok = (wxExecute(cmd, FALSE) != 0);
#endif
}

void MainFrame::OnHelpAbout(wxCommandEvent& WXUNUSED(event))
{
  wxString msg="Kazam version ";
  msg+=VERSION;
  msg+="\n\nCopyright (c) 2003 Johan Berntsson";
  wxMessageBox(msg, _T("About Kazam"), wxOK | wxICON_INFORMATION, this);
}

void MainFrame::OnCompile(wxCommandEvent& WXUNUSED(event))
{
  InformNotebook *nb=(InformNotebook *) mainSplitter->GetWindow2();
  nb->SavePageData();

  frame->logWindow->Clear();

  project.Compile();
}

void MainFrame::OnExecute(wxCommandEvent& WXUNUSED(event))
{
  InformNotebook *nb=(InformNotebook *) mainSplitter->GetWindow2();
  nb->SavePageData();

  frame->logWindow->Clear();

  // First save and compile if needed
  if(project.IsModified()) {
    project.Save();
    project.Compile();
  }

  project.Execute();
}
#include <wx/stream.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/tokenzr.h>
#include <wx/textfile.h>

void MainFrame::OnNew(wxCommandEvent& WXUNUSED(event))
{
  InformNotebook *nb=(InformNotebook *) mainSplitter->GetWindow2();
  nb->SavePageData();

  if(frame->project.IsModified()) {
    if(wxMessageBox(_T("The current project source files have changed.\n\nCreaate a new project anyway?"),
                    _T("KAZAM"),
                    wxYES_NO|wxICON_WARNING, this)==wxNO) return;
  }

  wxFileDialog fd(this, "Enter main source file name", "", "",
    "Inform source file (*.inf)|*.inf", 
    wxSAVE|wxOVERWRITE_PROMPT|wxCHANGE_DIR);

  if(fd.ShowModal()==wxID_OK) {
    // Copy template to new file
    wxTextFile fout(fd.GetPath());

    if(!fout.Exists()) fout.Create();
    if(fout.Open()) {
      // Remove previous contents
      while(fout.GetLineCount()>0) fout.RemoveLine(0);

      wxTextFile fin("template.inf");
      if(!fin.Exists()) {
        // No template file found
				wxString i1=frame->project.indent;
				wxString i2=i1+i1;
        wxString nl=fin.GetEOL();
        fout.AddLine("! \"!@\" denotes Kazam editor instructions."
                     "Do not remove or edit them manually.");
        fout.AddLine("Constant Story \"Unknown\";");
        fout.AddLine("Constant Headline \"A sample adventure\";");
        fout.AddLine("Include \"Parser\";");
        fout.AddLine("Include \"Verblib\";");
        fout.AddLine("Include \"Grammar\";"+nl);
        fout.AddLine("Object StartRoom \"An empty room\"");
        fout.AddLine(i1+"with\n"+i2+"description \"This is a bare room\",");
        fout.AddLine(i2+"!@ XY 0 0,");
        fout.AddLine(i1+"has light;"+nl);
        fout.AddLine("[ Initialise;");
        fout.AddLine(i1+"location=StartRoom;");
        fout.AddLine("];");
      } else if(fin.Open()) {
        for(unsigned int i=0; i<fin.GetLineCount(); i++) {
          fout.AddLine(fin.GetLine(i));
        }
        fin.Close();
      }
      fout.Write();
      fout.Close();
    }

    // Open the new project file
    if(frame->project.Load(fd.GetPath())) {
      wxLogError("Project creation failed");
    } else {
      frame->objectTree->BuildTree("Settings");
    }
  }
}

void MainFrame::OnAddRoom(wxCommandEvent& WXUNUSED(event))
{
  wxTextEntryDialog dlg(frame, "Enter the object ID");
  do {
    if(dlg.ShowModal()==wxID_CANCEL) return;
  } while(!frame->project.IsUniqueID(dlg.GetValue(), frame));

  int x, y;
  frame->project.FindFreeRoomPosition(&x, &y);
  frame->project.AddObject(dlg.GetValue(), INF_T_ROOM, true, -1, x, y);
}

void MainFrame::OnAddActor(wxCommandEvent& WXUNUSED(event))
{
  wxTextEntryDialog dlg(frame, "Enter the object ID");
  do {
    if(dlg.ShowModal()==wxID_CANCEL) return;
  } while(!frame->project.IsUniqueID(dlg.GetValue(), frame));

  frame->project.AddObject(dlg.GetValue(), INF_T_ACTOR);
}

void MainFrame::OnAddObject(wxCommandEvent& WXUNUSED(event))
{
  NewObjectDialog dlg(this, -1, "New Object");
  if(dlg.ShowModal()==wxID_CANCEL) return;
  frame->project.AddObject(dlg.editID->GetValue(), INF_T_ITEM);
}

void MainFrame::OnAddClass(wxCommandEvent& WXUNUSED(event))
{
  wxTextEntryDialog dlg(frame, "Enter the class ID");
  do {
    if(dlg.ShowModal()==wxID_CANCEL) return;
  } while(!frame->project.IsUniqueID(dlg.GetValue(), frame));

  frame->project.AddObject(dlg.GetValue(), INF_T_CLASS);
}

void MainFrame::OnAddFunction(wxCommandEvent& WXUNUSED(event))
{
  wxTextEntryDialog dlg(frame, "Enter the function name");
  do {
    if(dlg.ShowModal()==wxID_CANCEL) return;
  } while(!frame->project.IsUniqueID(dlg.GetValue(), frame));

  frame->project.AddObject(dlg.GetValue(), INF_T_FUNCTION);
}

void MainFrame::OnAddSourceFile(wxCommandEvent& WXUNUSED(event))
{
  wxFileDialog dlg(this, "Enter new source file name", "", "",
    "Inform source file (*.inf)|*.inf", wxSAVE|wxOVERWRITE_PROMPT|wxCHANGE_DIR);

  if(dlg.ShowModal()==wxID_CANCEL) return;
  if(frame->project.AddObject(dlg.GetPath(), INF_T_SOURCEFILE)==NULL) {
    wxMessageBox("Add SourceFile failed", "SourceFile Creation Error");
  }
}

void MainFrame::OnEditUndo(wxCommandEvent& event)
{
  wxWindow *w=IsEditorActive();
  if(w==NULL) {
    return;
  } else {
    ((InformEditor *) w)->Undo();
  }
}

void MainFrame::OnEditRedo(wxCommandEvent& event)
{
  wxWindow *w=IsEditorActive();
  if(w==NULL) {
    return;
  } else {
    ((InformEditor *) w)->Redo();
  }
}

void MainFrame::OnEditCut(wxCommandEvent& event)
{
  wxWindow *w=IsEditorActive();
  if(w==NULL) {
    return;
  } else {
    ((InformEditor *) w)->Cut();
  }
}

void MainFrame::OnEditCopy(wxCommandEvent& event)
{
  wxWindow *w=IsEditorActive();
  if(w==NULL) {
    return;
  } else {
    ((InformEditor *) w)->Copy();
  }
}

void MainFrame::OnEditPaste(wxCommandEvent& event)
{
  wxWindow *w=IsEditorActive();
  if(w==NULL) {
    return;
  } else {
    ((InformEditor *) w)->Paste();
  }
}

void MainFrame::OnUpdateEditUndo(wxUpdateUIEvent& event)
{
  wxWindow *w=IsEditorActive();
  if(w==NULL) {
    event.Enable(false);
  } else {
    event.Enable(((InformEditor *) w)->CanUndo());
  }
}

void MainFrame::OnUpdateEditRedo(wxUpdateUIEvent& event)
{
  wxWindow *w=IsEditorActive();
  if(w==NULL) {
    event.Enable(false);
  } else {
    event.Enable(((InformEditor *) w)->CanRedo());
  }
}

void MainFrame::OnUpdateEditCut(wxUpdateUIEvent& event)
{
  wxWindow *w=IsEditorActive();
  if(w==NULL) {
    event.Enable(false);
  } else {
    event.Enable(((InformEditor *) w)->CanCut());
  }
}

void MainFrame::OnUpdateEditCopy(wxUpdateUIEvent& event)
{
  wxWindow *w=IsEditorActive();
  if(w==NULL) {
    event.Enable(false);
  } else {
    event.Enable(((InformEditor *) w)->CanCopy());
  }
}

void MainFrame::OnUpdateEditPaste(wxUpdateUIEvent& event)
{
  wxWindow *w=IsEditorActive();
  if(w==NULL) {
    event.Enable(false);
  } else {
    event.Enable(((InformEditor *) w)->CanPaste());
  }
}

// Return MapWidget if it is active, else NULL
wxWindow *MainFrame::IsMapActive()
{
  InformNotebook *nb=GetNotebook();
  if(nb->GetSelection()>-1) {
    wxPanel* p=(wxPanel *) nb->GetPage(nb->GetSelection());
    wxLogDebug(p->GetName());
    if(p->GetName().IsSameAs("MapWidget")) {
      return p;
    }
  }
  return NULL;
}

// Return InformEditor if it is active, else NULL
wxWindow *MainFrame::IsEditorActive()
{
  InformNotebook *nb=GetNotebook();
  if(nb->GetSelection()>-1) {
    wxPanel* p=(wxPanel *) nb->GetPage(nb->GetSelection());
    wxWindow *w=p->GetChildren().GetFirst()->GetData();
    if(w->GetName().IsSameAs("InformEditor")) {
      return w;
    }
  }
  return NULL;
}

#ifdef wxUSE_FINDREPLDLG
void MainFrame::OnUpdateEditFind(wxUpdateUIEvent& event)
{
  event.Check(dlgFind!=NULL);
  event.Enable(IsEditorActive()!=NULL);
}

void MainFrame::OnUpdateEditReplace(wxUpdateUIEvent& event)
{
  event.Check(dlgReplace!=NULL);
  event.Enable(IsEditorActive()!=NULL);
}

void MainFrame::OnViewFindObject(wxCommandEvent& WXUNUSED(event))
{
  if(dlgReplace) {
    delete dlgReplace;
    dlgReplace=NULL;
  }

  if(dlgFind) {
    delete dlgFind;
    dlgFind=NULL;
  } else {
    dlgFind = new wxFindReplaceDialog(objectTree, &findData, _T("Find Object"), wxFR_NOWHOLEWORD);
    dlgFind->Show(TRUE);
  }
}

void MainFrame::OnEditFind(wxCommandEvent& WXUNUSED(event))
{
  if(dlgReplace) {
    delete dlgReplace;
    dlgReplace=NULL;
  }

  if(dlgFind) {
    delete dlgFind;
    dlgFind=NULL;
  } else {
    wxWindow *w=IsEditorActive();

    if(w==NULL) {
      wxLogWarning("Find is not defined for the active notebook page");
      return;
    }

    w->SetFocus();
    
    dlgFind = new wxFindReplaceDialog(w, &findData, _T("Find dialog"), wxFR_NOWHOLEWORD);
    dlgFind->Show(TRUE);
  }
}

void MainFrame::OnEditReplace(wxCommandEvent& WXUNUSED(event))
{
  if(dlgFind) {
    delete dlgFind;
    dlgFind=NULL;
  }

  if(dlgReplace) {
    delete dlgReplace;
    dlgReplace=NULL;
  } else {
    // Check which window to use
    wxWindow *w=IsEditorActive();

    if(w==NULL) {
      wxLogWarning("Replace is not defined for the active notebook page");
      return;
    }
    
    dlgReplace = new wxFindReplaceDialog(w, &findData, _T("Find and replace dialog"), wxFR_REPLACEDIALOG);
    dlgReplace->Show(TRUE);
  }
}

void MainFrame::OnFindDialog(wxFindDialogEvent& event)
{
  int flags=event.GetFlags();
  wxEventType type = event.GetEventType();
  wxWindow *w=event.GetDialog()->GetParent();

  if(type==wxEVT_COMMAND_FIND) {
    if(w==objectTree) {
      objectTree->Find(flags, event.GetFindString());
    } else {
      ((InformEditor *) w)->Find(flags, event.GetFindString());
    }
  } else if(type==wxEVT_COMMAND_FIND_NEXT) {
    if(w==objectTree) {
      objectTree->FindNext(flags, event.GetFindString());
    } else {
      ((InformEditor *) w)->FindNext(flags, event.GetFindString());
    }
  } else if(type==wxEVT_COMMAND_FIND_REPLACE) {
    ((InformEditor *) w)->Replace(flags, event.GetFindString(), event.GetReplaceString());
  } else if(type==wxEVT_COMMAND_FIND_REPLACE_ALL) {
    ((InformEditor *) w)->ReplaceAll(flags, event.GetFindString(), event.GetReplaceString());
  } else if(type==wxEVT_COMMAND_FIND_CLOSE) {
    wxFindReplaceDialog *dlg = event.GetDialog();
    if(dlg==dlgFind) {
      dlgFind=NULL;
    } else if(dlg==dlgReplace) {
      dlgReplace=NULL;
    }
    dlg->Destroy();
  }
}
#endif

void MainFrame::LoadPreferences()
{
  long i;
  wxString s;

  author=wxConfigBase::Get()->Read("Author", "");
  cmdCompiler=wxConfigBase::Get()->Read("Compiler", "bin\\i615");
  cmdLibrary=wxConfigBase::Get()->Read("Library", "lib");
  cmdExecute=wxConfigBase::Get()->Read("Terp", "bin\\frotz");
  templatePath=wxConfigBase::Get()->Read("Template", "");
  activeProject=wxConfigBase::Get()->Read("Active Project", "");
  s=wxConfigBase::Get()->Read("Warn on Exit", "true");
  warnOnExit=s.IsSameAs("true");
  s=wxConfigBase::Get()->Read("Automatic Reload", "false");
  useActiveProject=s.IsSameAs("true");
  wxConfigBase::Get()->Read("Indent Size", "2").ToLong(&i);
  indentSize=i;
  s=wxConfigBase::Get()->Read("Use Tabs", "false");
  useTabs=s.IsSameAs("true");

  frame->project.SetIndent(indentSize, useTabs);
}

void MainFrame::SavePreferences()
{
  wxConfigBase::Get()->Write("Author", author);
  wxConfigBase::Get()->Write("Compiler", cmdCompiler);
  wxConfigBase::Get()->Write("Library", cmdLibrary);
  wxConfigBase::Get()->Write("Terp", cmdExecute);
  wxConfigBase::Get()->Write("Template", templatePath);
  SourceFile *src=project.GetSourceFile(0);
  if(src) wxConfigBase::Get()->Write("Active Project", src->Path());
  if(warnOnExit) {
    wxConfigBase::Get()->Write("Warn on Exit", "true");
  } else {
    wxConfigBase::Get()->Write("Warn on Exit", "false");
  }
  if(useActiveProject) {
    wxConfigBase::Get()->Write("Automatic Reload", "true");
  } else {
    wxConfigBase::Get()->Write("Automatic Reload", "false");
  }
  char buffer[11];
  snprintf(buffer, 10, "%d", indentSize);
  wxConfigBase::Get()->Write("Indent Size", buffer);
  if(useTabs) {
    wxConfigBase::Get()->Write("Use Tabs", "true");
  } else {
    wxConfigBase::Get()->Write("Use Tabs", "false");
  }
  filehistory->Save(*wxConfigBase::Get());
}

void MainFrame::OnEditPreferences(wxCommandEvent& WXUNUSED(event))
{
  DlgPreferences dlg(this, -1, "Preferences");

  dlg.Set(cmdCompiler, cmdLibrary, cmdExecute, templatePath, author, useActiveProject, warnOnExit, indentSize, useTabs);
  if(dlg.ShowModal()==wxID_OK) {
    dlg.Get(&cmdCompiler, &cmdLibrary, &cmdExecute, &templatePath, &author, &useActiveProject, &warnOnExit, &indentSize, &useTabs);
    project.SetIndent(indentSize, useTabs);
  }
}

void MainFrame::OnViewToolbar(wxCommandEvent& WXUNUSED(event))
{
  wxToolBar *tbar=GetToolBar();

  if(!tbar) {
//    RecreateToolbar();
  } else {
    delete tbar;
    SetToolBar(NULL);
  }
}

void MainFrame::OnUpdateViewToolbar(wxUpdateUIEvent& event)
{
  event.Enable(false);
}

void MainFrame::OnUpdateViewStatusbar(wxUpdateUIEvent& event)
{
  event.Enable(false);
}

void MainFrame::OnUpdateViewLogWindow(wxUpdateUIEvent& event)
{
  event.Enable(false);
}

void MainFrame::OnViewStatusbar(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox("Not implemented - YET!", "Under Construction", wxOK|wxICON_INFORMATION, this);
}

void MainFrame::OnViewLogWindow(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox("Not implemented - YET!", "Under Construction", wxOK|wxICON_INFORMATION, this);
}

void MainFrame::OnViewTree(wxCommandEvent& WXUNUSED(event))
{
  objectTree->BuildTree();
}

void MainFrame::OnUpdateMapZoom(wxUpdateUIEvent& event)
{
  bool enabled=false;
  InformNotebook *nb=GetNotebook();
  if(nb->GetSelection()>-1) {
    wxPanel* p=(wxPanel *) nb->GetPage(nb->GetSelection());
    enabled=p->GetName().IsSameAs("MapWidget");
  }
  event.Enable(enabled);
}

void MainFrame::OnMapMenu(wxCommandEvent& event)
{
  InformNotebook *nb=GetNotebook();
  if(nb->GetSelection()>-1) {
    wxPanel* p=(wxPanel *) nb->GetPage(nb->GetSelection());
    if(p->GetName().IsSameAs("MapWidget")) {
      p->GetChildren()[0]->AddPendingEvent(event);
    }
  }
}
