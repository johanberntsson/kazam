#ifndef _OBJECTDATA_H
#define _OBJECTDATA_H

#include "InformData.h"

class ObjectData : public InformData
{
public:
  ObjectData();

	wxString ToString();

  // Some information used in the MapWidget for INF_T_ROOM
  int mRoomX;
  int mRoomY;
  int dragX, dragY;
  bool selected;
  int selectedConnection;
  struct DirCache {
    bool ok;
    ObjectData *target;
    bool isdoor;
  } dirCache[12];
};

#endif
