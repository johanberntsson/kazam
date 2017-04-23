#ifndef _INFORMDATA_H
#define _INFORMDATA_H

#include <wx/treectrl.h>

// These indices correspond to informKeyword in InformData.cpp
#define INF_SOURCE         0
#define INF_ID             1
#define INF_NAME           2
#define INF_CLASS          3
#define INF_PARENT         4 
#define INF_DIR_N          5
#define INF_DIR_S          6
#define INF_DIR_E          7
#define INF_DIR_W          8
#define INF_DIR_NE         9
#define INF_DIR_NW        10
#define INF_DIR_SE        11
#define INF_DIR_SW        12
#define INF_DIR_U         13
#define INF_DIR_D         14
#define INF_DIR_IN        15
#define INF_DIR_OUT       16
#define INF_STORY         17
#define INF_HEADLINE      18
#define INF_STARTPOS      19
#define INF_INITDESC      20
#define INF_DESCRIPTION   21
#define INF_A_ABSENT      22
#define INF_A_ANIMATE     23
#define INF_A_CLOTHING    24
#define INF_A_CONCEALED   25
#define INF_A_CONTAINER   26
#define INF_A_DOOR        27
#define INF_A_EDIBLE      28
#define INF_A_ENTERABLE   29
#define INF_A_FEMALE      30
#define INF_A_GENERAL     31
#define INF_A_LIGHT       32
#define INF_A_LOCKABLE    33
#define INF_A_LOCKED      34
#define INF_A_MOVED       35
#define INF_A_NEUTER      36
#define INF_A_ON          37
#define INF_A_OPEN        38
#define INF_A_OPENABLE    39
#define INF_A_PLURALNAME  40
#define INF_A_PROPER      41
#define INF_A_SCENERY     42
#define INF_A_SCORED      43
#define INF_A_STATIC      44
#define INF_A_SUPPORTER   45
#define INF_A_SWITCHABLE  46
#define INF_A_TALKABLE    47
#define INF_A_TRANSPARENT 48
#define INF_A_VISITED     49
#define INF_A_WORKFLAG    50
#define INF_A_WORN        51
#define INF_WITH          52
#define INF_HAS           53
#define INF_WORDS         54
#define INF_LEVEL         55
#define INF_KAZAM_XY      56
#define INF_FOUNDIN       57
#define INF_DOORTO        58
#define INF_DOORDIR       59
#define INF_WITHKEY       60

// Total number of segment types
#define INF_SIZE          61

// InformData subclasses
#define INF_T_ROOM         1
#define INF_T_ACTOR        2
#define INF_T_ITEM         3
#define INF_T_FUNCTION     4
#define INF_T_CLASS        5
#define INF_T_SOURCEFILE   6

class InformData
{
public:
  InformData();
  virtual ~InformData();

	virtual wxString ToString();

  // functions for commiting changes
  bool IsDirty();
  void SetDirty();
  void Update();

  void AdjustPointers(unsigned int file, unsigned int start, unsigned int offset);
  virtual wxString GetSource(int infType);
  virtual void SetSource(int infType, const wxString& newSource);

  // treeID is public for convinience, but should only be set by
  // ObjectTree::BuildTree()
  wxTreeItemId treeId;

  // These are public for convinience, but should only be set by
  // SourceFile::Parse*()
  unsigned int file;
  unsigned int b[INF_SIZE]; // begin of INF_* segment
  unsigned int e[INF_SIZE]; // end of INF_* segment
  unsigned int rb[INF_SIZE]; // begin of INF_* range (usually line(s))
  unsigned int re[INF_SIZE]; // end of INF_* range (usually line(s))

  int type; // should be INF_T_*

  static void Print();

private:
  bool dirty;
};
WX_DECLARE_OBJARRAY(InformData, InformDataArray);

#endif
