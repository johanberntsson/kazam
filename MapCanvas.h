#include <wx/wx.h>
#include <wx/image.h>

#ifndef MAPCANVAS_H
#define MAPCANVAS_H

#include "Project.h"

// TODO: remove hard coding
#define MAX_SET 100

class MapWidget;

class MapCanvas: public wxPanel {
public:
  MapCanvas(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
  ~MapCanvas();

  void Init();

private:
  MapWidget *map;

  void AutoArrange();
  void AppendRoomSet(int set, ObjectData *room);
  InformDataArray setlist[MAX_SET];
  wxPoint setTopLeft[MAX_SET];
  wxPoint setBottomRight[MAX_SET];

  bool dragging;   // room drag mode active?
  bool rubberband; // rubberband mode active?
  int dragX, dragY, dragMode; // Map coordinates for room drag mode
  int rubberX, rubberY; // start coordinates for rubber band
  int mouseX, mouseY; // current mouse coordinates (for rubber band)
  int lastMouseX, lastMouseY; // previous mouse coordinates (for rubber band)

  int unitLength;
  int gridOffset;
  int width, height, zoom, widthPixel, heightPixel; // Use Resize

  void Resize(int x, int y, int z);
  void EnsureVisible(ObjectData *room);
  void GetWindowSize(int *x1, int *y1, int *x2, int *y2, bool expand);
  void CanvasToMap(int cx, int cy, int *mx, int *my);
  void MapToCanvas(int mx, int my, int *cx, int *cy);

  bool blockInit;

  wxRect Hotspot(int x, int y, int p);
  int FindFreeHotspot(int t, ObjectData *target);
  void DrawRoom(wxPaintDC &dc, ObjectData *room);
  void DrawGrid(wxPaintDC &dc, int mx1, int my1, int mx2, int my2);
  void DrawConnection(wxPaintDC &dc, bool isdoor, int x1, int y1, int p1,int x2, int y2, int p2, int mx, int my);
  void LineAnchor(int x, int y, int p, int *xb, int *yb, int *xc, int *yc);
  void DrawArrowHead(wxPaintDC &dc, int x, int y, int p);

  void AddRoom(int x, int y);
  void FindFreeRoomPosition(int *xc, int *yc);
  void SetRoomDir(ObjectData *room, int direction, ObjectData *target);
  ObjectData *GetRoomDir(ObjectData *room, int direction, bool *isdoor);

  void OnKeyDown(wxKeyEvent &event);
  void OnPaint(wxPaintEvent &event);
  void OnLeftUp(wxMouseEvent &event);
  void OnLeftDown(wxMouseEvent &event);
  void OnLeftDClick(wxMouseEvent& event);
  void OnRightDown(wxMouseEvent &event);
  void OnMouseMove(wxMouseEvent &event);
  void OnMouseLeave(wxMouseEvent &event);
  void OnMapZoomIn(wxCommandEvent& event);
  void OnMapZoomOut(wxCommandEvent& event);
  void OnMapAdd(wxCommandEvent& event);
  void OnMapDelete(wxCommandEvent& event);
  void OnMapZoom100(wxCommandEvent& event);
  void OnMapZoom50(wxCommandEvent& event);
  void OnMapZoom10(wxCommandEvent& event);
  void OnMapAutoArrange(wxCommandEvent& event);
  void OnMapIncreaseCanvas(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()
};


#endif // MAPCANVAS_H
