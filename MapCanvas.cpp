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

#include <wx/tokenzr.h>

/* Same order as INF_DIR_* in InformData.cpp */
#define NONE     -1
#define DIR_N     0
#define DIR_S     1
#define DIR_E     2
#define DIR_W     3
#define DIR_NE    4
#define DIR_NW    5
#define DIR_SE    6
#define DIR_SW    7
#define DIR_U     8
#define DIR_D     9
#define DIR_IN   10
#define DIR_OUT  11
#define ROOM     12

#include "MapCanvas.h"
#include "MapWidget.h"
#include "DlgAddDoor.h"
#include "ObjectData.h"

#include "kazam.h"
extern MainFrame *frame;
extern wxString informKeyword[];

BEGIN_EVENT_TABLE(MapCanvas, wxPanel)
EVT_PAINT(MapCanvas::OnPaint)
EVT_LEFT_UP(MapCanvas::OnLeftUp)
EVT_LEFT_DOWN(MapCanvas::OnLeftDown)
EVT_LEFT_DCLICK(MapCanvas::OnLeftDClick)
EVT_RIGHT_DOWN(MapCanvas::OnRightDown)
EVT_MOTION(MapCanvas::OnMouseMove)
EVT_LEAVE_WINDOW(MapCanvas::OnMouseLeave)
EVT_MENU(MENU_MAP_ADD, MapCanvas::OnMapAdd)
EVT_MENU(MENU_MAP_DELETE, MapCanvas::OnMapDelete)
EVT_MENU(MENU_MAP_INCREASE_CANVAS, MapCanvas::OnMapIncreaseCanvas)
EVT_MENU(MENU_MAP_ZOOM100, MapCanvas::OnMapZoom100)
EVT_MENU(MENU_MAP_ZOOM50, MapCanvas::OnMapZoom50)
EVT_MENU(MENU_MAP_ZOOM10, MapCanvas::OnMapZoom10)
EVT_MENU(MENU_MAP_AUTOARRANGE, MapCanvas::OnMapAutoArrange)
EVT_MENU(MENU_MAP_ZOOMIN, MapCanvas::OnMapZoomIn)
EVT_MENU(MENU_MAP_ZOOMOUT, MapCanvas::OnMapZoomOut)
EVT_KEY_DOWN(MapCanvas::OnKeyDown)
END_EVENT_TABLE()

MapCanvas::MapCanvas(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style):
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
  blockInit=false;
  gridOffset=10;
  unitLength=100;
  map=(MapWidget *) parent;
  Resize(20, 20, 100);
}

MapCanvas::~MapCanvas()
{
}

void MapCanvas::Init()
{
  if(blockInit) return;
  dragging=false;
  rubberband=false;

  int numNew=0;
  char newCoord[11];
  for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
    InformData *current=&frame->project.objects[i];
    if(current->type!=INF_T_ROOM) continue;
    ObjectData *room=(ObjectData *) current;

    if(room->mRoomX==-1 || room->mRoomY==-1) {
      ++numNew;
      FindFreeRoomPosition(&room->mRoomX, &room->mRoomY);
      snprintf(newCoord, 10, "%d %d", room->mRoomX, room->mRoomY);
      // insert a XY command and call Update() to update INF_KAZAM_XY
      room->SetSource(INF_KAZAM_XY, newCoord);
      room->Update();
      wxLogDebug("Setting %s to %d %d\n", room->GetSource(INF_NAME).c_str(), room->mRoomX, room->mRoomY);
    }
    if(room->mRoomX>=width) Resize(room->mRoomX+1, -1, -1);
    if(room->mRoomY>=height) Resize(-1, room->mRoomY+1, -1);

    room->selected=false;
    if(room->treeId==frame->objectTree->GetSelection()) {
      room->selected=true;
      EnsureVisible(room);
    }
  }

  if(numNew>0) {
    wxString msg="Map coordinates were missing for some rooms. Do you want "
      " to let Kazam find the best location for these rooms?";
    if(wxMessageBox(msg, "Room Coordinate Conversion", wxYES_NO, this)==wxYES) {
      AutoArrange();
    }
  }

  Refresh();
}

// Set size, -1 leaves old value
void MapCanvas::Resize(int w, int h, int z)
{
  if(w>-1) width=w;
  if(h>-1) height=h;
  if(z>-1) zoom=z;

  dragging=false;
  rubberband=false;

  // Must call FitInside after resize to update scrollbars
  MapToCanvas(width, height, &widthPixel, &heightPixel);
  map->SetVirtualSizeHints(widthPixel, heightPixel);
  map->FitInside();
}

void MapCanvas::CanvasToMap(int cx, int cy, int *mx, int *my)
{
  *mx=(100*cx)/(unitLength*zoom);
  *my=(100*cy)/(unitLength*zoom);
}

void MapCanvas::MapToCanvas(int mx, int my, int *cx, int *cy)
{
  *cx=(mx*unitLength*zoom)/100;
  *cy=(my*unitLength*zoom)/100;
}

void MapCanvas::EnsureVisible(ObjectData *room)
{
  int x, y;
  MapToCanvas(room->mRoomX, room->mRoomY, &x, &y);

  int mx1, my1, mx2, my2;
  GetWindowSize(&mx1, &my1, &mx2, &my2, false);

  if(room->mRoomX<=mx2 && room->mRoomX>=mx1 && room->mRoomY<=my2 && room->mRoomY>=my1) {
    // already visible
    return;
  }

  int xUnit, yUnit;
  map->GetScrollPixelsPerUnit(&xUnit, &yUnit);
  map->Scroll(x/xUnit, y/yUnit);
}

void MapCanvas::OnKeyDown(wxKeyEvent& event)
{
  wxCommandEvent e;
  int key=event.GetKeyCode();

  if(key==WXK_BACK || key==WXK_DELETE) {
    OnMapDelete(e);
  } else {
    event.Skip();
  }
}

void MapCanvas::OnMapZoomIn(wxCommandEvent& WXUNUSED(event))
{
  zoom-=10;
  if(zoom<1) zoom=1;
  Resize(-1, -1, zoom);
}

void MapCanvas::OnMapZoomOut(wxCommandEvent& WXUNUSED(event))
{
  zoom+=10;
  Resize(-1, -1, zoom);
}

void MapCanvas::OnMapZoom100(wxCommandEvent& WXUNUSED(event))
{
  Resize(-1, -1, 100);
}

void MapCanvas::OnMapZoom50(wxCommandEvent& WXUNUSED(event))
{
  Resize(-1, -1, 50);
}

void MapCanvas::OnMapZoom10(wxCommandEvent& WXUNUSED(event))
{
  Resize(-1, -1, 10);
}

void MapCanvas::OnMapAdd(wxCommandEvent& WXUNUSED(event))
{
  int x, y;
  FindFreeRoomPosition(&x, &y);

  dragging=false;
  rubberband=false;
  AddRoom(x, y);
  Refresh();
}

void MapCanvas::FindFreeRoomPosition(int *xc, int *yc)
{
  frame->project.FindFreeRoomPosition(xc, yc);
  if(*xc>width) Resize(*xc, -1, -1);
  if(*yc>height) Resize(-1, *yc, -1);
}

void MapCanvas::SetRoomDir(ObjectData *room, int direction, ObjectData *target)
{
  wxString destination="";
  if(target) destination=target->GetSource(INF_ID);

  wxLogDebug("Setting %s %d to %s\n", room->GetSource(INF_ID).c_str(), direction, destination.c_str());

  room->SetSource(direction+INF_DIR_N, destination);
  room->Update();
}

ObjectData *MapCanvas::GetRoomDir(ObjectData *room, int direction, bool *isdoor)
{
  if(room->dirCache[direction].ok) {
    // To speed things up
    if(isdoor) *isdoor=room->dirCache[direction].isdoor;
    return room->dirCache[direction].target;
  }

  if(isdoor) *isdoor=false;

  wxString d=room->GetSource(direction+INF_DIR_N);

  // Check if room
  ObjectData *target=frame->project.FindRoom(d);
  if(target) {
    room->dirCache[direction].ok=true;
    room->dirCache[direction].target=target;
    room->dirCache[direction].isdoor=false;
    return target;
  }

  // Check if door
  wxString loc, x;
  ObjectData *door=(ObjectData *) frame->project.FindObject(d, INF_T_ITEM);
  while(door) {
    x=door->GetSource(INF_FOUNDIN);
    if(!x.IsEmpty()) loc=x;

    if(door->GetSource(INF_A_DOOR)) {
      // We found a door!
      if(isdoor) *isdoor=true;
      wxStringTokenizer tkz(loc, " ");
      x=tkz.GetNextToken();
      if(x==room->GetSource(INF_ID)) {
        x=tkz.GetNextToken();
      }
      //wxLogDebug("door %s: loc '%s' -> target '%s'\n", door->GetSource(INF_ID).c_str(), loc.c_str(), x.c_str());
      room->dirCache[direction].ok=true;
      room->dirCache[direction].target=frame->project.FindRoom(x);
      room->dirCache[direction].isdoor=true;
      return room->dirCache[direction].target;
    }

    // Try the parent class instead
    door=(ObjectData *) frame->project.FindObject(door->GetSource(INF_CLASS), INF_T_CLASS);
  }

  room->dirCache[direction].ok=true;
  room->dirCache[direction].target=NULL;
  room->dirCache[direction].isdoor=false;
  return NULL;
}

void MapCanvas::AddRoom(int x, int y)
{
  wxTextEntryDialog dlg(this, "Enter the object ID");
  do {
    if(dlg.ShowModal()==wxID_CANCEL) return;
  } while(!frame->project.IsUniqueID(dlg.GetValue(), this));

  frame->project.AddObject(dlg.GetValue(), INF_T_ROOM, true, -1, x, y);
}

void MapCanvas::OnMapDelete(wxCommandEvent& WXUNUSED(event))
{
  // We need to block init since deleting an object causes
  // automatic selection of another node in the tree, which
  // will trigger an ShowData in its notebook, and
  // RoomNotebook::ShowData will call Init, which will
  // automatically select the room. This new room will then
  // be deleted in the for loop below.
  if(wxMessageBox("Delete the selected object?", "Confirm Delete", wxYES_NO)!=wxYES) return;

  blockInit=true;
  for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
    InformData *current=&frame->project.objects[i];
    if(current->type!=INF_T_ROOM) continue;
    ObjectData *room=(ObjectData *) current;
    if(room->selected) {
      if(room->selectedConnection==NONE) {
        frame->project.DeleteObject(room);
      } else {
        // Remove the connection
        ObjectData *target=GetRoomDir(room, room->selectedConnection, NULL);
        for(int i=DIR_N; i<=DIR_SW; i++) {
          if(GetRoomDir(target, i, NULL)==room) {
            SetRoomDir(target, i, NULL);
          }
        }

        SetRoomDir(room, room->selectedConnection, NULL);
        room->selectedConnection=NONE;
      }
    }
  }

  blockInit=false;
  Init();
}

void MapCanvas::OnMapIncreaseCanvas(wxCommandEvent& WXUNUSED(event))
{
  Resize(width+1, height+1, -1);
  Refresh();
}

void MapCanvas::OnMapAutoArrange(wxCommandEvent& WXUNUSED(event))
{
  AutoArrange();
  Refresh();
}

void MapCanvas::OnLeftDown(wxMouseEvent &event)
{
  int x,y;
  mouseX=event.GetX();
  mouseY=event.GetY();
  CanvasToMap(mouseX,mouseY, &x, &y);

  // Check if already selected
  unsigned int i;
  for(i=0; i<frame->project.objects.GetCount(); i++) {
    InformData *current=&frame->project.objects[i];
    if(current->type!=INF_T_ROOM) continue;
    ObjectData *room=(ObjectData *) current;
    if(room->mRoomX==x && room->mRoomY==y && room->selected) {
      // The room is already selected. Check if a hotspot
      // was clicked, otherwise leave the current selection
      // status untouched.
      dragging=true;
      dragMode=ROOM;
      room->selectedConnection=NONE;
      for(int i=DIR_N; i<=DIR_SW && dragMode==ROOM; i++) {
        if(Hotspot(x, y, i).Inside(mouseX, mouseY)) {
          dragMode=i;
        }
      }
      if(dragMode!=ROOM) break; // a hotspot was selected.
      dragX=x;
      dragY=y;
      Refresh();
      return;
    }
  }

  // Select the room under the cursor
  blockInit=true;
  for(i=0; i<frame->project.objects.GetCount(); i++) {
    InformData *current=&frame->project.objects[i];
    if(current->type!=INF_T_ROOM) continue;
    ObjectData *room=(ObjectData *) current;
    if(room->mRoomX==x && room->mRoomY==y) {
      if(!dragging) {
        dragging=true;
        dragX=x;
        dragY=y;
        frame->objectTree->SelectItem(room->treeId);
      }
      room->selected=true;
      room->dragX=x;
      room->dragY=y;

      dragMode=ROOM;
      room->selectedConnection=NONE;
      for(int i=DIR_N; i<=DIR_SW && dragMode==ROOM; i++) {
        if(Hotspot(x, y, i).Inside(mouseX, mouseY)) {
          //wxLogDebug("Hotspot %d\n", i);
          dragMode=i;
          lastMouseX=0;
          room->selectedConnection=i;
        }
      }
    } else if(!event.ControlDown()) {
      room->selected=false;
    }
  }
  blockInit=false;

  if(!dragging) {
    rubberband=true;
    lastMouseX=0;
    rubberX=mouseX;
    rubberY=mouseY;
  }

  Refresh();
}

void MapCanvas::OnLeftUp(wxMouseEvent &event)
{
  int x,y;
  char newCoord[11];
  CanvasToMap(event.GetX(), event.GetY(), &x, &y);

  // Find target room
  ObjectData *target=frame->project.FindRoom(x, y);

  if(dragging && dragMode==ROOM) { 
    int dx=x-dragX;
    int dy=y-dragY;

    // Check if any selected room is in an illegal position
    bool ok=true;
    for(unsigned int i=0; ok && i<frame->project.objects.GetCount(); i++) {
      InformData *current=&frame->project.objects[i];
      if(current->type!=INF_T_ROOM) continue;
      ObjectData *room=(ObjectData *) current;
      if(room->selected) {
        room->dragX=room->mRoomX+dx;
        room->dragY=room->mRoomY+dy;
        // Check position
        if(room->dragX<0 || room->dragX>=width) ok=false;
        if(room->dragY<0 || room->dragY>=height) ok=false;
        ObjectData *newLoc=frame->project.FindRoom(room->dragX, room->dragY);
        if(newLoc!=NULL && !newLoc->selected) ok=false;
      }
    }

    // Move if no conflicts were found
    if(ok) {
      for(unsigned int i=0; ok && i<frame->project.objects.GetCount(); i++) {
        InformData *current=&frame->project.objects[i];
        if(current->type!=INF_T_ROOM) continue;
        ObjectData *room=(ObjectData *) current;
        if(room->selected) {
          room->mRoomX+=dx;
          room->mRoomY+=dy;
          snprintf(newCoord, 10, "%d %d", room->mRoomX, room->mRoomY);
          //wxLogDebug("Moving %d %d %s\n", dragX, dragY, room->GetSource(INF_ID).c_str());
          room->SetSource(INF_KAZAM_XY, newCoord);
          room->Update();
        }
      }
    }
  } else if(dragging && dragMode>=DIR_N && target!=NULL) {
    // Find the selected room (only one)
    ObjectData *room=NULL;
    for(unsigned int ii=0; ii<frame->project.objects.GetCount(); ii++) {
      InformData *current=&frame->project.objects[ii];
      if(current->type!=INF_T_ROOM) continue;
      room=(ObjectData *) current;
      if(room->selected) break;
    }

    // Add a connection to the new room
    int hotspot=NONE;
    for(int i=DIR_N; i<=ROOM && hotspot==NONE; i++) {
      if(Hotspot(target->mRoomX, target->mRoomY, i).Inside(mouseX, mouseY)) {
        hotspot=i;
      }
    }

    if(hotspot!=NONE) {
      // Make connection
      if(room!=target) {
        //wxLogDebug("Target hotspot %d\n", hotspot);
        if(event.ShiftDown()) {
          int olddrag=dragMode; // old value destroyed when showing dlg
          // Add a door at this room
          DlgAddDoor dlg(this, -1, "Add Door");
          dlg.SetValue(room->GetSource(INF_ID), target->GetSource(INF_ID), dragMode, hotspot);
          if(dlg.ShowModal()==wxID_OK) {
            ObjectData *door=dlg.GetValue();
            door->Update();
            SetRoomDir(room, olddrag, door);
            if(hotspot!=ROOM) SetRoomDir(target, hotspot, door);
          }
        } else {
          SetRoomDir(room, dragMode, target);
          if(hotspot!=ROOM) SetRoomDir(target, hotspot, room);
        }
      }
    }    
  }

  dragging=false;
  rubberband=false;
  Refresh();
}

void MapCanvas::OnMouseMove(wxMouseEvent &event)
{
  int x,y;
  static int ox, oy;
  mouseX=event.GetX();
  mouseY=event.GetY();
  CanvasToMap(mouseX,mouseY, &x, &y);

  if(rubberband) {
    // Rubber-band

    // Select all rooms inside the rubber-band
    bool prevSelected, erase=false;
    int ox, oy, tx, ty;
    CanvasToMap(rubberX, rubberY, &tx, &ty);

    if(tx<x) ox=tx; else { ox=x; x=tx; }
    if(ty<y) oy=ty; else { oy=y; y=ty; }

    for(unsigned int k=0; k<frame->project.objects.GetCount(); k++) {
      InformData *current=&frame->project.objects[k];
      if(current->type!=INF_T_ROOM) continue;
      ObjectData *room=(ObjectData *) current;
      prevSelected=room->selected;
      room->selected=false;
      for(int i=ox; i<=x; i++) {
        for(int j=oy; j<=y; j++) {
          if(room->mRoomX==i && room->mRoomY==j) {
            room->dragX=i;
            room->dragY=j;
            room->selected=true;
          }
        }
      }

      if(prevSelected!=room->selected) erase=true;
    }

    int dx, dy, dw, dh;
    MapToCanvas(ox-1, oy, &dx, &dy);
    MapToCanvas(3+x-ox, 2+y-oy, &dw, &dh);
    wxRect dirtyRectangle(dx, dy, dw, dh);
    if(erase) lastMouseX=0;
    Refresh(erase, &dirtyRectangle);
  }

  if(dragging) {
    if(dragMode==ROOM) {
      // Move all selected rooms
      if(x!=ox || y!=oy) {
        ox=x;
        oy=y;
      
        int dx=x-dragX;
        int dy=y-dragY;
        for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
          InformData *current=&frame->project.objects[i];
          if(current->type!=INF_T_ROOM) continue;
          ObjectData *room=(ObjectData *) current;
          if(room->selected) {
            room->dragX=room->mRoomX+dx;
            room->dragY=room->mRoomY+dy;
          }
        }
        // We only update when we move to a new grid, so
        // flickering and paint events is no high priority
        Refresh();
      }
    } else if(dragMode>=DIR_N) {
      // But here we redraw every time the mouse is moved!
      Refresh(false);
    }
  }
}

void MapCanvas::OnMouseLeave(wxMouseEvent &event)
{
  if(dragging || rubberband) {
    dragging=false;
    rubberband=false;
    Refresh();
  }
}

void MapCanvas::OnLeftDClick(wxMouseEvent &event)
{
  int x, y;
  CanvasToMap(event.GetX(), event.GetY(), &x, &y);

  dragging=false;
  rubberband=false;

  bool refresh=false;
  ObjectData *room=frame->project.FindRoom(x, y);
  if(room==NULL) {
    AddRoom(x, y);
    refresh=true;
  } else {
    // Show room properties
    frame->objectTree->SelectItem(room->treeId);
    wxNotebook *nb=(wxNotebook *) frame->mainSplitter->GetWindow2();
    nb->SetSelection(1);
  }

  if(refresh) Refresh();
}

void MapCanvas::OnRightDown(wxMouseEvent &event)
{
  dragging=false;
  rubberband=false;
}

// N, S, E, W, NE, NW, SE, SW, U, D, IN, OUT
int offsetX[]={0, 0, 1, -1, 1, -1, 1, -1, 0, 0, 1, -1 };
int offsetY[]={-1, 1, 0, 0, -1, -1, 1, 1, -1, 1, 0, 0 };

void MapCanvas::AppendRoomSet(int set, ObjectData *room)
{
  ObjectData *target;
  setlist[set].Add(room);

  if(room->mRoomX<setTopLeft[set].x) setTopLeft[set].x=room->mRoomX;
  if(room->mRoomY<setTopLeft[set].y) setTopLeft[set].y=room->mRoomY;
  if(room->mRoomX>setBottomRight[set].x) setBottomRight[set].x=room->mRoomX;
  if(room->mRoomY>setBottomRight[set].y) setBottomRight[set].y=room->mRoomY;

  wxLogDebug("%d %d - %d %d\n",
    setTopLeft[set].x, setTopLeft[set].y,
    setBottomRight[set].x, setBottomRight[set].y);

  for(int i=DIR_N; i<=DIR_OUT; i++) {
    target=GetRoomDir(room, i, NULL);
    if(target && target->mRoomX==-1) {
      int ox=room->mRoomX;
      int oy=room->mRoomY;
      do {
        ox+=offsetX[i];
        oy+=offsetY[i];
        // Check that we don't put two rooms at the same location
      } while(frame->project.FindRoom(ox, oy));
      target->mRoomX=ox;
      target->mRoomY=oy;
      wxLogDebug("Setting %d %s %d %d\n", set, target->GetSource(INF_ID).c_str(), target->mRoomX, target->mRoomY);
      AppendRoomSet(set, target);
    }
  }
}

void MapCanvas::AutoArrange()
{
  // Autoarrange
  unsigned int ii;
  ObjectData *room;

  // Reset all mapdata
  for(ii=0; ii<frame->project.objects.GetCount(); ii++) {
    InformData *current=&frame->project.objects[ii];
    if(current->type!=INF_T_ROOM) continue;
    room=(ObjectData *) current;
    room->mRoomX=-1;
    room->mRoomY=-1;
  }

  // Add rooms to sets
  int set=0;

  for(ii=0; ii<frame->project.objects.GetCount(); ii++) {
    InformData *current=&frame->project.objects[ii];
    if(current->type!=INF_T_ROOM) continue;
    ObjectData *room=(ObjectData *) current;
    if(room->mRoomX==-1) {
      // Create new set with room as seed
      room->mRoomX=1000;
      room->mRoomY=room->mRoomX;
      setTopLeft[set].x=room->mRoomX;
      setTopLeft[set].y=room->mRoomX;
      setBottomRight[set].x=room->mRoomX;
      setBottomRight[set].y=room->mRoomX;

      wxLogDebug("Setting %d %s %d %d\n", set, room->GetSource(INF_ID).c_str(), room->mRoomX, room->mRoomY);
      AppendRoomSet(set, room);
      ++set;
      if(set>=MAX_SET) wxFatalError("MAX_SET too small");
    }
  }

  // Converting sets to real coordinates
  int i, dx, dy;
  int xc=0, yc=0;
  char newCoord[11];
  for(i=0; i<set; i++) {
    dx=setTopLeft[i].x;
    dy=setTopLeft[i].y;

    while(setlist[i].GetCount()>0) {
      room=(ObjectData *) &setlist[i][0];
      room->mRoomX=xc+(room->mRoomX-dx);
      room->mRoomY-=dy;
      snprintf(newCoord, 10, "%d %d", room->mRoomX, room->mRoomY);
      // insert a XY command and call Update() to update INF_KAZAM_XY
      room->SetSource(INF_KAZAM_XY, newCoord);
      room->Update();

      // Remove item (but don't delete!)
      setlist[i].Detach(0);
    }
    xc+=(2+setBottomRight[i].x-dx);
    if(yc<(setBottomRight[i].y-dy)) yc=1+(setBottomRight[i].y-dy);
  }

  Resize(xc, yc, -1);
}

void MapCanvas::GetWindowSize(int *x1, int *y1, int *x2, int *y2, bool expand)
{
  // Find the window size (in Map coordinates)
  int xUnit, yUnit;
  int vx, vy, vw, vh;
  map->GetViewStart(&vx, &vy);
  map->GetClientSize(&vw, &vh);
  map->GetScrollPixelsPerUnit(&xUnit, &yUnit);
  vx*=xUnit;
  vy*=yUnit;

  int mx1, my1, mx2, my2;
  CanvasToMap(vx, vy, &mx1, &my1);
  CanvasToMap(vx+vw, vy+vh, &mx2, &my2);
  //wxLogDebug("%d %d %d %d -> %d %d %d %d\n", vx, vy, vw, vh, mx1, my1, mx2, my2);
  if(expand) {
    if(mx1>0) --mx1;
    if(my1>0) --my1;
    if(++mx2>=width) --mx2;
    if(++my2>=height) --my2;
  }

  *x1=mx1;
  *y1=my1;
  *x2=mx2;
  *y2=my2;
}

void MapCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
  if(!this->IsShown()) return;

  wxPaintDC dc(this);

  // Draw black outside the canvas (if visible)
  int cw, ch;
  GetClientSize(&cw, &ch);
  if(cw>widthPixel) {
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(widthPixel,0, cw-widthPixel, ch);
    dc.SetBrush(wxNullBrush);
  }
  if(ch>heightPixel) {
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(0, heightPixel, cw, ch-heightPixel);
    dc.SetBrush(wxNullBrush);
  }


  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list
  int mx1, my1, mx2, my2;
  while(upd) {
    CanvasToMap(upd.GetX(), upd.GetY(), &mx1, &my1);
    CanvasToMap(upd.GetX()+upd.GetW(), upd.GetY()+upd.GetH(), &mx2, &my2);
    for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
      InformData *current=&frame->project.objects[i];
      if(current->type!=INF_T_ROOM) continue;
      ObjectData *room=(ObjectData *) current;
      if(room->mRoomX<mx1 || room->mRoomX>mx2) continue;
      if(room->mRoomY<my1 || room->mRoomY>my2) continue;
      wxLogDebug("Painting %d %d %s", room->mRoomX, room->mRoomY, room->GetSource(INF_ID).c_str());
      DrawRoom(dc, room);
    }
    ++upd;
  }

  // Selection rubber band
  if(rubberband) {
    dc.SetLogicalFunction(wxINVERT);
    if(lastMouseX>0) {
      dc.DrawLine(rubberX, rubberY, lastMouseX, rubberY);
      dc.DrawLine(lastMouseX, rubberY, lastMouseX, lastMouseY);
      dc.DrawLine(lastMouseX, lastMouseY, rubberX, lastMouseY);
      dc.DrawLine(rubberX, lastMouseY, rubberX, rubberY);
    }
    dc.DrawLine(rubberX, rubberY, mouseX, rubberY);
    dc.DrawLine(mouseX, rubberY, mouseX, mouseY);
    dc.DrawLine(mouseX, mouseY, rubberX, mouseY);
    dc.DrawLine(rubberX, mouseY, rubberX, rubberY);
    lastMouseX=mouseX;
    lastMouseY=mouseY;
    dc.SetLogicalFunction(wxCOPY);
  }
}

#ifdef OLDCODE
void MapCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
  wxPaintDC dc(this);

  // Draw black outside the canvas (if visible)
  int cw, ch;
  GetClientSize(&cw, &ch);
  if(cw>widthPixel) {
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(widthPixel,0, cw-widthPixel, ch);
    dc.SetBrush(wxNullBrush);
  }
  if(ch>heightPixel) {
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(0, heightPixel, cw, ch-heightPixel);
    dc.SetBrush(wxNullBrush);
  }

  int mx1, my1, mx2, my2;
  GetWindowSize(&mx1, &my1, &mx2, &my2, true);
  DrawGrid(dc, mx1, my1, mx2, my2);

  // Selection rubber band
  if(rubberband) {
    dc.SetLogicalFunction(wxINVERT);
    if(lastMouseX>0) {
      dc.DrawLine(rubberX, rubberY, lastMouseX, rubberY);
      dc.DrawLine(lastMouseX, rubberY, lastMouseX, lastMouseY);
      dc.DrawLine(lastMouseX, lastMouseY, rubberX, lastMouseY);
      dc.DrawLine(rubberX, lastMouseY, rubberX, rubberY);
    }
    dc.DrawLine(rubberX, rubberY, mouseX, rubberY);
    dc.DrawLine(mouseX, rubberY, mouseX, mouseY);
    dc.DrawLine(mouseX, mouseY, rubberX, mouseY);
    dc.DrawLine(rubberX, mouseY, rubberX, rubberY);
    lastMouseX=mouseX;
    lastMouseY=mouseY;
    dc.SetLogicalFunction(wxCOPY);
  }

  // Rooms (selected/moving/normal)
  for(unsigned int i=0; i<frame->project.objects.GetCount(); i++) {
    InformData *current=&frame->project.objects[i];
    if(current->type!=INF_T_ROOM) continue;
    ObjectData *room=(ObjectData *) current;
    if(room->mRoomX<mx1 || room->mRoomX>mx2) continue;
    if(room->mRoomY<my1 || room->mRoomY>my2) continue;
    DrawRoom(dc, room);
  }
}
#endif

void MapCanvas::DrawGrid(wxPaintDC &dc, int mx1, int my1, int mx2, int my2)
{
  int x, y;
	wxPen oldpen=dc.GetPen();
  dc.SetPen(*wxGREY_PEN);
  for(x=mx1; x<mx2; x++) {
    for(y=my1; y<my2; y++) {
      dc.DrawLine((x*unitLength*zoom)/100, (y*unitLength*zoom)/100, 1+(x*unitLength*zoom)/100, 1+(y*unitLength*zoom)/100);
    }
  }
  dc.SetPen(oldpen);
}

// xc,yc=room connection point. xb,yb=closest grid connection point
void MapCanvas::LineAnchor(int x, int y, int p, int *xb, int *yb, int *xc, int *yc)
{
  wxRect r=Hotspot(x, y, ROOM);
  switch(p) {
    case DIR_N:
      *xc=r.GetLeft()+r.GetWidth()/2;
      *yc=r.GetTop();
      *xb=*xc;
      *yb=y*unitLength*zoom/100;
      break;
    case DIR_S:
      *xc=r.GetLeft()+r.GetWidth()/2;
      *yc=r.GetBottom();
      *xb=*xc;
      *yb=((y+1)*unitLength*zoom)/100;
      break;
    case DIR_E:
      *xc=r.GetRight();
      *yc=r.GetTop()+r.GetHeight()/2;
      *xb=((x+1)*unitLength*zoom)/100;
      *yb=*yc;
      break;
    case DIR_W:
      *xc=r.GetLeft();
      *yc=r.GetTop()+r.GetHeight()/2;
      *xb=(x*unitLength*zoom)/100;
      *yb=*yc;
      break;
    case DIR_NE:
      *xc=r.GetRight();
      *yc=r.GetTop();
      *xb=((x+1)*unitLength*zoom)/100;
      *yb=(y*unitLength*zoom)/100;
      break;
    case DIR_NW:
      *xc=r.GetLeft();
      *yc=r.GetTop();
      *xb=(x*unitLength*zoom)/100;
      *yb=(y*unitLength*zoom)/100;
      break;
    case DIR_SE:
      *xc=r.GetRight();
      *yc=r.GetBottom();
      *xb=((x+1)*unitLength*zoom)/100;
      *yb=((y+1)*unitLength*zoom)/100;
      break;
    case DIR_SW:
      *xc=r.GetLeft();
      *yc=r.GetBottom();
      *xb=(x*unitLength*zoom)/100;
      *yb=((y+1)*unitLength*zoom)/100;
      break;
    case ROOM:
      *xc=r.GetLeft();
      *yc=r.GetTop()+unitLength/4;
      *xb=(x*unitLength*zoom)/100;
      *yb=(y*unitLength*zoom)/100;
      break;
  }
}

void MapCanvas::DrawArrowHead(wxPaintDC &dc, int x, int y, int p)
{
  switch(p) {
  case DIR_N:
    dc.DrawLine(x-4, y-4, x, y);
    dc.DrawLine(x+4, y-4, x, y);
    break;
  case DIR_NE:
    dc.DrawLine(x, y-5, x, y);
    dc.DrawLine(x+5, y, x, y);
    break;
  case DIR_NW:
    dc.DrawLine(x, y-5, x, y);
    dc.DrawLine(x-5, y, x, y);
    break;
  case DIR_S:
    dc.DrawLine(x-4, y+4, x, y);
    dc.DrawLine(x+4, y+4, x, y);
    break;
  case DIR_E:
    dc.DrawLine(x+4, y-4, x, y);
    dc.DrawLine(x+4, y+4, x, y);
    break;
  case DIR_W:
    dc.DrawLine(x-4, y-4, x, y);
    dc.DrawLine(x-4, y+4, x, y);
    break;
  case DIR_SE:
    dc.DrawLine(x, y+5, x, y);
    dc.DrawLine(x+5, y, x, y);
    break;
  case DIR_SW:
    dc.DrawLine(x, y+5, x, y);
    dc.DrawLine(x-5, y, x, y);
    break;
  }
}

void MapCanvas::DrawConnection(wxPaintDC &dc, bool isdoor, int x1, int y1, int p1,int x2, int y2, int p2, int mx, int my)
{
  // NB: use mx/my instead of mouseX/mouseY.

  wxPen doorpen=wxPen(*wxBLUE, 1, wxDOT);
  if(isdoor) dc.SetPen(doorpen);

  // === STEP 1 ===

  // We calculate three coordinates for each room:
  // xr, yr: hotspot connection point
  // xg, yg: closest grid connection point
  // xi, yi: closest grid intersection point
  int xr1, yr1, xg1, yg1, xi1, yi1;
  LineAnchor(x1, y1, p1, &xg1, &yg1, &xr1, &yr1);

  if(p2==NONE && dragMode!=ROOM) {
    // Try to find a hotspot at the destination
    ObjectData *target=frame->project.FindRoom(x2, y2);
    if(target) {
      for(int i=DIR_N; i<=ROOM && p2==NONE; i++) {
        if(Hotspot(target->mRoomX, target->mRoomY, i).Inside(mx, my)) {
          p2=i;
        }
      }
    }

    if(p2==NONE) {
      // No hotspot found. Draw a rubber band and exit
      dc.DrawLine(xr1, yr1, xg1, yg1);
      dc.DrawLine(xg1, yg1, mx, my);
      return;
    }
  }

  int xr2, yr2, xg2, yg2, xi2, yi2;
  LineAnchor(x2, y2, p2, &xg2, &yg2, &xr2, &yr2);

  xi1=xg1;
  xi2=xg2;
  yi1=yg1;
  yi2=yg2;
  if(p1==DIR_N || p1==DIR_S) {
    if(xg2>xg1) {
      xi1=(unitLength*(x1+1)*zoom)/100;
    } else {
      xi1=(unitLength*x1*zoom)/100;
    }
  } else if(p1==DIR_E || p1==DIR_W) {
    if(yg2>yg1) {
      yi1=(unitLength*(y1+1)*zoom)/100;
    } else {
      yi1=(unitLength*y1*zoom)/100;
    }
  }

  if(p2==DIR_N || p2==DIR_S) {
    if(xg1>xg2) {
      xi2=(unitLength*(x2+1)*zoom)/100;
    } else {
      xi2=(unitLength*x2*zoom)/100;
    }
  } else if(p2==DIR_E || p2==DIR_W) {
    if(yg1>yg2) {
      yi2=(unitLength*(y2+1)*zoom)/100;
    } else {
      yi2=(unitLength*y2*zoom)/100;
    }
  }

  // === STEP 2 ===
  int i;
  bool ok=false;

  dc.DrawLine(xr1, yr1, xg1, yg1);
  dc.DrawLine(xr2, yr2, xg2, yg2);
  DrawArrowHead(dc, xr2, yr2, p2);

  // Check if self reference
  if(x1==x2 && y1==y2) {
    if(isdoor) dc.SetPen(wxNullPen);
    return;
  }

  // Connect with straight lines if there are no rooms between
  if(yg1==yg2 && x1<x2 && (p1==DIR_NE || p1==DIR_E || p1==DIR_SE)) {
    for(i=x1+1, ok=true; ok && i<x2; i++) ok=(frame->project.FindRoom(i, y1)==NULL);
  } else if(yg1==yg2 && x2<x1 && (p2==DIR_NE || p2==DIR_E || p2==DIR_SE)) {
    for(i=x2+1, ok=true; ok && i<x1; i++) ok=(frame->project.FindRoom(i, y1)==NULL);
  } else if(xg1==xg2 && y1<y2 && (p1==DIR_SE || p1==DIR_S || p1==DIR_SW)) {
    for(i=y1+1, ok=true; ok && i<y2; i++) ok=(frame->project.FindRoom(x1, i)==NULL);
  } else if(xg1==xg2 && y2<y1 && (p2==DIR_SE || p2==DIR_S || p2==DIR_SW)) {
    for(i=y2+1, ok=true; ok && i<y1; i++) ok=(frame->project.FindRoom(x1, i)==NULL);
  } else if((x1-x2)==(y1-y2) && 
    ((p1==DIR_SE && p2==DIR_NW) || (p1==DIR_NW && p2==DIR_SE))) {
    if(x1<x2) {
      for(i=x1+1, ok=true; ok && i<x2; i++) ok=(frame->project.FindRoom(i, (y1-x1)+i)==NULL);
    } else {
      for(i=x2+1, ok=true; ok && i<x1; i++) ok=(frame->project.FindRoom(i, (y2-x2)+i)==NULL);
    }
  } else if((x1-x2)==-(y1-y2) && 
    ((p1==DIR_SW && p2==DIR_NE) || (p1==DIR_NE && p2==DIR_SW))) {
    if(x1<x2) {
      for(i=x1+1, ok=true; ok && i<x2; i++) ok=(frame->project.FindRoom(i, (y1-x1)-i)==NULL);
    } else {
      for(i=x2+1, ok=true; ok && i<x1; i++) ok=(frame->project.FindRoom(i, (y2-x2)-i)==NULL);
    }
  }
//  1       2         1       2
//    2       1     2       1
  if(ok) {
    dc.DrawLine(xg1, yg1, xg2, yg2);
    if(isdoor) dc.SetPen(wxNullPen);
    return;
  }

  // Connect with L-shaped line if no rooms between
  if(x1<x2 && y1<y2 && (p1==DIR_NE || p1==DIR_E || p1==DIR_SE) && p2!=DIR_S) {    
    //  1--+ 
    //   +-- 2
    for(i=x1+1, ok=(p2!=DIR_S); ok && i<x2; i++) ok=(frame->project.FindRoom(i, y1)==NULL);
    for(i=y1+1; ok && i<=y2; i++) ok=(frame->project.FindRoom(x2, i)==NULL);
    if(ok) {
      dc.DrawLine(xg1, yg1, xg2, yg1);
      dc.DrawLine(xg2, yg1, xg2, yg2);
    } else {
      for(i=x1+1, ok=(p2!=DIR_W); ok && i<x2; i++) ok=(frame->project.FindRoom(i, y2)==NULL);
      for(i=y1+1; ok && i<=y2; i++) ok=(frame->project.FindRoom(x1, i)==NULL);
      if(ok) {
        dc.DrawLine(xg1, yg1, xg1, yg2);
        dc.DrawLine(xg1, yg2, xg2, yg2);
      }
    }
  } else if(x2<x1 && y1<y2 && (p2==DIR_NE || p2==DIR_E || p2==DIR_SE)) {
    //   +-- 1
    //  2--+
    for(i=x2+1, ok=(p1!=DIR_W); ok && i<x1; i++) ok=(frame->project.FindRoom(i, y1)==NULL);
    for(i=y1+1; ok && i<=y2; i++) ok=(frame->project.FindRoom(x2, i)==NULL);
    if(ok) {
      dc.DrawLine(xg2, yg2, xg2, yg1);
      dc.DrawLine(xg2, yg1, xg1, yg1);
    } else {
      for(i=x2+1, ok=(p1!=DIR_N); ok && i<x1; i++) ok=(frame->project.FindRoom(i, y2)==NULL);
      for(i=y1+1; ok && i<=y2; i++) ok=(frame->project.FindRoom(x1, i)==NULL);
      if(ok) {
        dc.DrawLine(xg2, yg2, xg1, yg2);
        dc.DrawLine(xg1, yg2, xg1, yg1);
      }
    }
  } else if(x1<x2 && y2<y1 && (p1==DIR_NE || p1==DIR_E || p1==DIR_SE)) {
    //   +-- 2
    //  1--+
    wxLogDebug("DrawConnection %d %d - %d %d\n", x1, y1, x2, y2);
    for(i=x1+1, ok=(p2!=DIR_W); ok && i<x2; i++) {
      ok=(frame->project.FindRoom(i, y2)==NULL);
      wxLogDebug("loop 1: FindRoom(%d, %d) %d\n", i, y2, ok);
    }
    for(i=y2+1; ok && i<=y1; i++) {
      ok=(frame->project.FindRoom(x1, i)==NULL);
      wxLogDebug("loop 2: FindRoom(%d, %d) %d\n", i, x1, ok);
    }
    if(ok) {
      dc.DrawLine(xg1, yg1, xg1, yg2);
      dc.DrawLine(xg1, yg2, xg2, yg2);
    } else {
      for(i=x1+1, ok=(p2!=DIR_N); ok && i<x2; i++) {
        ok=(frame->project.FindRoom(i, y1)==NULL);
        wxLogDebug("loop 3: FindRoom(%d, %d) %d\n", i, y2, ok);
      }
      for(i=y2+1; ok && i<=y1; i++) {
        ok=(frame->project.FindRoom(x2, i)==NULL);
        wxLogDebug("loop 4: FindRoom(%d, %d) %d\n", x2, i, ok);
      }
      if(ok) {
        dc.DrawLine(xg1, yg1, xg2, yg1);
        dc.DrawLine(xg2, yg1, xg2, yg2);
      }
    }
  } else if(x2<x1 && y2<y1 && (p2==DIR_NE || p2==DIR_E || p2==DIR_SE)) {
    //  2+-- 
    //   --+ 1
    for(i=x2+1, ok=(p1!=DIR_S); ok && i<x1; i++) ok=(frame->project.FindRoom(i, y2)==NULL);
    for(i=y2+1; ok && i<=y1; i++) ok=(frame->project.FindRoom(x1, i)==NULL);
    if(ok) {
      dc.DrawLine(xg2, yg2, xg1, yg2);
      dc.DrawLine(xg1, yg2, xg1, yg1);
    } else {
      for(i=x2+1, ok=(p1!=DIR_W); ok && i<x1; i++) ok=(frame->project.FindRoom(i, y1)==NULL);
      for(i=y2+1; ok && i<=y1; i++) ok=(frame->project.FindRoom(x2, i)==NULL);
      if(ok) {
        dc.DrawLine(xg2, yg2, xg2, yg1);
        dc.DrawLine(xg2, yg1, xg1, yg1);
      }
    }
  }
  if(ok) {
    if(isdoor) dc.SetPen(wxNullPen);
    return;
  }

  // Default: follow grid
  dc.DrawLine(xg1, yg1, xi1, yi1);
  dc.DrawLine(xg2, yg2, xi2, yi2);
  // Connect the grid intersections
  if(xi1>xi2) {
    dc.DrawLine(xi1, yi1, xi2, yi1);
    dc.DrawLine(xi2, yi1, xi2, yi2);
  } else {
    dc.DrawLine(xi2, yi2, xi1, yi2);
    dc.DrawLine(xi1, yi2, xi1, yi1);
  }

  if(isdoor) dc.SetPen(wxNullPen);
}

wxRect MapCanvas::Hotspot(int x, int y, int p)
{
  wxRect r;
  switch(p) {
  case DIR_N:
    r=wxRect(x*unitLength+unitLength/2-gridOffset/2, y*unitLength+gridOffset/2, gridOffset, gridOffset);
    break;
  case DIR_S:
    r=wxRect(x*unitLength+unitLength/2-gridOffset/2, (y+1)*unitLength-3*gridOffset/2, gridOffset, gridOffset);
    break;
  case DIR_E:
    r=wxRect((x+1)*unitLength-3*gridOffset/2, y*unitLength+unitLength/2-gridOffset/2, gridOffset, gridOffset);
    break;
  case DIR_W:
    r=wxRect(x*unitLength+gridOffset/2, y*unitLength+unitLength/2-gridOffset/2, gridOffset, gridOffset);
    break;
  case DIR_NW:
    r=wxRect(x*unitLength+gridOffset/2, y*unitLength+gridOffset/2, gridOffset, gridOffset);
    break;
  case DIR_NE:
    r=wxRect((x+1)*unitLength-3*gridOffset/2, y*unitLength+gridOffset/2, gridOffset, gridOffset);
    break;
  case DIR_SW:
    r=wxRect(x*unitLength+gridOffset/2, (y+1)*unitLength-3*gridOffset/2, gridOffset, gridOffset);
    break;
  case DIR_SE:
    r=wxRect((x+1)*unitLength-3*gridOffset/2, (y+1)*unitLength-3*gridOffset/2, gridOffset, gridOffset);
    break;
  case ROOM:
    r=wxRect(gridOffset+x*unitLength, gridOffset+y*unitLength, unitLength-2*gridOffset, unitLength-2*gridOffset);
  }

  return wxRect(r.x*zoom/100, r.y*zoom/100, r.width*zoom/100, r.height*zoom/100);
}

int findOrder[]={DIR_N, DIR_NE, DIR_E, DIR_SE, DIR_S, DIR_SW, DIR_W, DIR_NW };
int MapCanvas::FindFreeHotspot(int t, ObjectData *target)
{
  int i, j;
  for(i=0; i<8; i++) {
    j=findOrder[(i+t)%8];
    if(GetRoomDir(target, j, NULL)==NULL) {
      return j;
    }
  }
  return ROOM;
}

void MapCanvas::DrawRoom(wxPaintDC &dc, ObjectData *room)
{
  int offset=gridOffset/4;
  if(offset<2) offset=2;

  wxRect r=Hotspot(room->mRoomX, room->mRoomY, ROOM);

  if(room->selected && dragging) {
    if(dragMode==ROOM) {
      dc.SetPen(*wxBLACK_DASHED_PEN);
      dc.DrawRectangle(Hotspot(room->dragX, room->dragY, ROOM));
      dc.SetPen(wxNullPen);
      return;
    }

    int tx, ty;
    dc.SetLogicalFunction(wxINVERT);
    if(lastMouseX>0) {
      CanvasToMap(lastMouseX, lastMouseY, &tx, &ty);
      DrawConnection(dc, false, room->mRoomX, room->mRoomY, dragMode, tx, ty, NONE, lastMouseX, lastMouseY);
    }
    CanvasToMap(mouseX, mouseY, &tx, &ty);
    DrawConnection(dc, false, room->mRoomX, room->mRoomY, dragMode, tx, ty, NONE, mouseX, mouseY);
    lastMouseX=mouseX;
    lastMouseY=mouseY;
    dc.SetLogicalFunction(wxCOPY);
  }


  // Draw all connections to other rooms
	int i;
  bool isdoor;
  for(i=DIR_N; i<=DIR_SW; i++) {
    ObjectData *target=GetRoomDir(room, i, &isdoor);
    if(target) {
      wxLogDebug("connection %s -> %s\n", room->GetSource(INF_ID).c_str(), target->GetSource(INF_ID).c_str());

      // Which hotspot should we connect to on the target room?
      int targetDir=ROOM;
      for(int j=DIR_N; j<=DIR_SW && targetDir==ROOM; j++) {
        if(GetRoomDir(target, j, NULL)==room) {
          targetDir=j;
        }
      }

      if(targetDir==ROOM) {
        // This is a one-way direction. Find the closest free hotspot
        // on the target
        int t=0;
        if(room->mRoomX==target->mRoomX && room->mRoomY>target->mRoomY) t=4;
        if(room->mRoomX<target->mRoomX && room->mRoomY>target->mRoomY) t=5;
        if(room->mRoomX<target->mRoomX && room->mRoomY==target->mRoomY) t=6;
        if(room->mRoomX<target->mRoomX && room->mRoomY<target->mRoomY) t=7;
        if(room->mRoomX==target->mRoomX && room->mRoomY<target->mRoomY) t=0;
        if(room->mRoomX>target->mRoomX && room->mRoomY<target->mRoomY) t=1;
        if(room->mRoomX>target->mRoomX && room->mRoomY==target->mRoomY) t=2;
        if(room->mRoomX>target->mRoomX && room->mRoomY>target->mRoomY) t=3;
        targetDir=FindFreeHotspot(t, target);
      }

      if((room->selected && room->selectedConnection==i) ||
         (target->selected && target->selectedConnection==targetDir)) {
        dc.SetPen(*wxRED_PEN);
        DrawConnection(dc, false, room->mRoomX, room->mRoomY, i, target->mRoomX, target->mRoomY, targetDir, mouseX, mouseY);
        dc.SetPen(wxNullPen);
      } else {
        DrawConnection(dc, isdoor, room->mRoomX, room->mRoomY, i, target->mRoomX, target->mRoomY, targetDir, mouseX, mouseY);
      }
    }
  }

  // Draw room
  dc.DrawRectangle(r);
  if(room->selected) {
    dc.SetBrush(*wxLIGHT_GREY_BRUSH);
    for(int i=DIR_N; i<=DIR_SW; i++) dc.DrawRectangle(Hotspot(room->mRoomX, room->mRoomY, i));
    dc.SetBrush(wxNullBrush);
  }

  dc.DrawLine(r.GetLeft(), r.GetTop(), r.GetLeft()-offset, r.GetTop()-offset);
  dc.DrawLine(r.GetRight(), r.GetTop(), r.GetRight()+offset, r.GetTop()-offset);
  dc.DrawLine(r.GetLeft(), r.GetBottom(), r.GetLeft()-offset, r.GetBottom()+offset);
  dc.DrawLine(r.GetRight(), r.GetBottom(), r.GetRight()+offset, r.GetBottom()+offset);

  dc.DrawLine(r.GetLeft()+r.GetWidth()/2, r.GetTop(), r.GetLeft()+r.GetWidth()/2, r.GetTop()-offset);
  dc.DrawLine(r.GetLeft()+r.GetWidth()/2, r.GetBottom(), r.GetLeft()+r.GetWidth()/2, r.GetBottom()+offset);
  dc.DrawLine(r.GetLeft(), r.GetTop()+r.GetHeight()/2, r.GetLeft()-offset, r.GetTop()+r.GetHeight()/2);
  dc.DrawLine(r.GetRight(), r.GetTop()+r.GetHeight()/2, r.GetRight()+offset, r.GetTop()+r.GetHeight()/2);

  wxString dir;
  int ty, tw, th;
  dc.GetTextExtent("XXX", &tw, &th);
  dc.SetClippingRegion(r);
  dc.DrawText(room->GetSource(INF_ID), r.x+5, r.y+5);
  ty=r.y+th+10;
  ObjectData *target;
  for(i=DIR_U; i<=DIR_OUT; i++) {
    target=GetRoomDir(room, i, NULL);
    if(target==NULL) continue;
    if(i==DIR_U) dir="U"; else if(i==DIR_D) dir="D";
    else if(i==DIR_IN) dir="I"; else dir="O";
    dir+=":"+target->GetSource(INF_ID);
    dc.DrawText(dir, r.x+5, ty);
    ty+=th;
  }
  dc.DestroyClippingRegion();
}
