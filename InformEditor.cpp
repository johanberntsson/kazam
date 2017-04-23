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

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifdef wxUSE_FINDREPLDLG
#include <wx/fdrepdlg.h>
#else
#define wxFR_WHOLEWORD 0
#define wxFR_MATCHCASE 0
#define wxFR_DOWN 0
#endif

#include "InformEditor.h"

#ifdef USE_STC
// TODO: Inform-support has been added to Scintilla by adding
// this at line 242 in contrib\src\stc\scintilla\src\LexCPP.cxx
//      } else if (sc.ch == '!') {
//				sc.SetState(SCE_C_COMMENTLINE);
// This fix should be replaced with more permanent solution.

#define NO

static wxChar* InformWordlist1 =
      "Abbreviate absent Achieved action actor add_to_scope after AfterLife \
      AfterPrompt AfterRoutines AllowPushDir ALWAYS_BIT Amusing AMUSING_PROVIDED \
      ANIMA_PE animate Array article articles ASKSCOPE_PE Attribute Banner before \
      BeforeParsing bold box break call cant_go CANTSEE_PE capacity ChangePlayer \
      child children ChooseObjects Class clothing CommonAncestor compass \
      CONCEAL_BIT concealed Constant container continue copy create creature d_obj \
      d_to daemon DarkToDark deadflag DEATH_MENTION_UNDO DeathMessage DEBUG \
      DEFART_BIT default Default describe description destroy DictionaryLookup \
      do door door_dir door_to DrawStatusLine e_obj e_to each_turn edible \
      else End Endif ENGLISH_BIT enterable EXCEPT_PE Extend false female first \
      fixed font for found_in FULLINV_BIT GamePostRoutine GamePreRoutine general \
      GetGNAOfObject give Global grammar has HasLightSource hasnt Headline held \
      if Ifdef Iffalse Ifndef Ifnot Iftrue Ifv3 Ifv5 Import in in_obj in_to \
      Include INDENT_BIT indirect IndirectlyContains initial Initialise InScope \
      inside_description invent inventory_stage inversion ISARE_BIT IsSeeThrough \
      ITGONE_PE jump JUNKAFTER_PE keep_silent last LibraryMessages life light \
      Link list_together Locale location lockable locked lookmode LookRoutine \
      LoopOver Lowstring male MANUAL_PRONOUNS MAX_CARRIED MAX_SCORE MAX_TIMERS \
      Message meta MMULTI_PE move moved MoveFloatingObjects multi MULTI_PE \
      multiexcept multiheld multiinside n_obj n_to name ne_obj ne_to neuter \
      new_line NEWLINE_BIT NewRoom NextWord NextWordStopped NO_PLACES \
      NOARTICLE_BIT NOTHELD_PE nothing NOTHING_PE notify_mode notin noun \
      NounDomain NULL number NUMBER_PE NUMBER_TASKS nw_obj nw_to Object \
      OBJECT_SCORE ObjectIsUntouchable objectloop ofclass off OffersLight \
      on only open openable or orders out_obj out_to parent parse_name";
static wxChar* InformWordlist2 =
      "ParseNoun ParseNumber ParserError ParseToken PARTINV_BIT PlaceInScope \
      player PlayerTo plural pluralname print print_ret print_to_array \
      PrintOrRun PrintRank PrintTaskName PrintVerb private PronounNotice \
      PronounValue proper Property provides quit random react_after \
      react_before read real_location recreate RECURSE_BIT Release remaining \
      remove Replace restore return reverse rfalse roman ROOM_SCORE rtrue \
      RunLife RunRoutines s_obj s_to SACK_OBJECT save scenery SCENERY_PE \
      scope ScopeWithin score score scored se_obj se_to second self selfobj \
      sender Serial SetPronoun SetTime short_name short_name_indef sibling \
      StartDaemon StartTimer static Statusline StopDaemon StopTimer Story \
      string STUCK_PE style supporter sw_obj sw_to switch switchable Switches \
      System_file table talkable task_scores TASKS_PROVIDED TERSE_BIT TestScope \
      the_time thedark time time_left time_out TimePasses to TOOFEW_PE TOOLIT_PE \
      topic transparent true TryNumber turns u_obj u_to underline UnknownVerb \
      UnsignedCompare until UPTO_PE USE_MODULES VAGUE_PE Verb VERB_PE visited \
      w_obj w_to when_closed when_off when_on when_open while with with_key \
      WITHOUT_DIRECTIONS wn WordAddress WordInProperty WordLength workflag \
      WORKFLAG_BIT worn WriteListFrom YesOrNo Zcharacter ZRegion";
static wxChar* InformOperatorlist1 =
      "- # % & * , / : ; ? @add @and @aread @art_shift @buffer_mode @call \
      @call_1n @call_1s @call_2n @call_2s @call_vn @call_vn2 @call_vs @call_vs2 \
      @catch @check_arg_count @check_unicode @clear_attr @copy_table @dec \
      @dec_chk @div @draw_picture @encode_text @erase_line @erase_picture \
      @erase_window @get_child @get_cursor @get_next_prop @get_parent @get_prop \
      @get_prop_addr @get_prop_len @get_sibling @get_window_prop @inc @inc_chk \
      @input_stream @insert_obj @je @jg @jin @jl @jump @jz @load @loadb @loadw \
      @log_shift @make_menu @mod @mouse_window @move_window @mul @new_line @nop \
      @not @or @output_stream @picture_data @picture_table @piracy @pop \
      @pop_stack @print @print_addr @print_char @print_form @print_num @print_obj \
      @print_paddr @print_ret @print_table @print_unicode @pull @push @push_stack \
      @put_prop @put_wind_prop @quit @random @read @read_char @read_mouse \
      @remove_obj @restart @restore @restore_undo @ret @ret_popped @rfalse @rtrue \
      @save @save_undo @scan_table @scroll_window @set_attr @set_colour @set_cursor \
      @set_font @set_margins @set_text_style @set_window @show_status @sound_effect \
      @split_window @sread @store @storeb @storew @sub @test @test_attr @throw \
      @tokenise @verify @window_size @window_style ^ | ~ + < = >";

InformEditor::InformEditor(wxWindow* parent, wxWindowID id)
  : wxStyledTextCtrl(parent, id, wxDefaultPosition, wxDefaultSize, 0, "InformEditor")
{
  SetLexer(wxSTC_LEX_CPP);
  SetKeyWords(0, InformWordlist1);
  SetKeyWords(1, InformWordlist2);
  SetKeyWords(2, InformOperatorlist1);

  // Comments
  StyleSetForeground(wxSTC_C_COMMENT, wxColor("FOREST GREEN"));
  StyleSetForeground(wxSTC_C_COMMENTLINE, wxColor("FOREST GREEN"));

  // Keywords
  StyleSetForeground(wxSTC_C_WORD, wxColor("MEDIUM BLUE"));
  StyleSetForeground(wxSTC_C_WORD2, wxColor("MEDIUM BLUE"));

  // Numbers
  StyleSetForeground(wxSTC_C_NUMBER, wxColor("RED"));

  // Strings
  StyleSetForeground(wxSTC_C_STRING, wxColor("GREY"));

  // Operators
  StyleSetForeground(wxSTC_C_OPERATOR, wxColor("BROWN"));
}
#else
InformEditor::InformEditor(wxWindow* parent, wxWindowID id)
  : wxTextCtrl(parent, id, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxHSCROLL, wxDefaultValidator, "InformEditor")
{
}
#endif

void InformEditor::SetValue(const wxString& text)
{
#ifdef USE_STC
  wxStyledTextCtrl::SetText(text);
#else
  wxTextCtrl::SetValue(text);
  wxTextCtrl::ShowPosition(0);
#endif
  SetModified(false);
}

wxString InformEditor::GetValue()
{
#ifdef USE_STC
  return wxStyledTextCtrl::GetText();
#else
  return wxTextCtrl::GetValue();
#endif
}

bool InformEditor::GetModified()
{
#ifdef USE_STC
  return wxStyledTextCtrl::GetModify();
#else
  return wxTextCtrl::IsModified();
#endif
}

void InformEditor::SetModified(bool modified)
{
#ifdef USE_STC
  if(modified) {
    // This is done by the control itself
  } else {
    // Avoid problem with undo past save points
    wxStyledTextCtrl::EmptyUndoBuffer();
    wxStyledTextCtrl::SetSavePoint();
  }
#else
  if(modified) {
    // This is done by the control itself
  } else {
    wxTextCtrl::DiscardEdits();
  }
#endif
}

// TODO: implement undo,cut,paste for wxTextCtrl
bool InformEditor::CanUndo()
{
#ifdef USE_STC
  return wxStyledTextCtrl::CanUndo();
#else
  return false;
#endif
}

bool InformEditor::CanRedo()
{
#ifdef USE_STC
  return wxStyledTextCtrl::CanRedo();
#else
  return false;
#endif
}

bool InformEditor::CanCut()
{
#ifdef USE_STC
  int b,e;
  GetSelection(&b, &e);
  return (b!=e);
#else
  return false;
#endif
}

bool InformEditor::CanCopy()
{
#ifdef USE_STC
  int b,e;
  GetSelection(&b, &e);
  return (b!=e);
#else
  return false;
#endif
}

bool InformEditor::CanPaste()
{
#ifdef USE_STC
  return wxStyledTextCtrl::CanPaste();
#else
  return false;
#endif
}

void InformEditor::Undo()
{
#ifdef USE_STC
  wxStyledTextCtrl::Undo();
#else
#endif
}

void InformEditor::Redo()
{
#ifdef USE_STC
  wxStyledTextCtrl::Redo();
#else
#endif
}

void InformEditor::Cut()
{
#ifdef USE_STC
  wxStyledTextCtrl::Cut();
#else
#endif
}

void InformEditor::Copy()
{
#ifdef USE_STC
  wxStyledTextCtrl::Copy();
#else
#endif
}

void InformEditor::Paste()
{
#ifdef USE_STC
  wxStyledTextCtrl::Paste();
#else
#endif
}

int findPos; // TODO: must be a better way
bool InformEditor::Find(int flags, const wxString& text)
{
#ifdef USE_STC
  int f=0;
  if(flags&wxFR_WHOLEWORD) f|=wxSTC_FIND_WHOLEWORD;
  if(flags&wxFR_MATCHCASE) f|=wxSTC_FIND_MATCHCASE;

  // Quick and dirty check that the string exists
  if(FindText(0, GetLength(), text, f)==-1) {
    wxLogWarning("Cannot find the string '%s'", text.c_str());
    return true;
  }

  // Proper search with wrap-around
  SearchAnchor();
  if(flags&wxFR_DOWN) {
    findPos=SearchNext(f, text);
    if(findPos==-1) {
      GotoPos(0);
      SearchAnchor();
      findPos=SearchNext(f, text);
    }
  } else {
    findPos=SearchPrev(f, text);
    if(findPos==-1) {
      GotoPos(GetLength());
      SearchAnchor();
      findPos=SearchPrev(f, text);
    }
  }

  // make caret visible and highlight selection again
  GotoPos(findPos);
  if(flags&wxFR_DOWN) {
    findPos=SearchNext(f, text);
  } else {
    findPos=SearchPrev(f, text);
  }
#else
  wxMessageBox("Find is not implemented");
#endif
  return false;
}

bool InformEditor::FindNext(int flags, const wxString& text)
{
#ifdef USE_STC
  int f=0;
  if(flags&wxFR_WHOLEWORD) f|=wxSTC_FIND_WHOLEWORD;
  if(flags&wxFR_MATCHCASE) f|=wxSTC_FIND_MATCHCASE;

  // Proper search with wrap-around
  if(flags&wxFR_DOWN) {
    GotoPos(findPos+1);
    SearchAnchor();
    findPos=SearchNext(f, text);
    if(findPos==-1) {
      GotoPos(0);
      SearchAnchor();
      findPos=SearchNext(f, text);
    }
  } else {
    GotoPos(findPos-1);
    SearchAnchor();
    findPos=SearchPrev(f, text);
    if(findPos==-1) {
      GotoPos(GetLength());
      SearchAnchor();
      findPos=SearchPrev(f, text);
    }
  }

  // make caret visible and highlight selection again
  GotoPos(findPos);
  if(flags&wxFR_DOWN) {
    findPos=SearchNext(f, text);
  } else {
    findPos=SearchPrev(f, text);
  }
#else
  wxMessageBox("FindNext is not implemented");
#endif
  return true;
}

bool InformEditor::Replace(int flags, const wxString& text, const wxString& newText)
{
#ifdef USE_STC
  int b,e;
  GetSelection(&b, &e);
  if(b==e) return true; // empty selection
  ReplaceSelection(newText);
#else
  wxMessageBox("Replace not implemented");
#endif
  return true;
}

bool InformEditor::ReplaceAll(int flags, const wxString& text, const wxString& newText)
{
#ifdef USE_STC
  int f=0;
  if(flags&wxFR_WHOLEWORD) f|=wxSTC_FIND_WHOLEWORD;
  if(flags&wxFR_MATCHCASE) f|=wxSTC_FIND_MATCHCASE;

  SearchAnchor();
  findPos=SearchNext(f, text);
  while(findPos!=-1) {
    ReplaceSelection(newText);
    GotoPos(findPos+newText.Length());
    SearchAnchor();
    findPos=SearchNext(f, text);
  }
#else
  wxMessageBox("ReplaceAll not implemented");
#endif
  return true;
}

