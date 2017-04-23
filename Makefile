CC = gcc

PROGRAM = kazam

SRCS = ActorNotebook.cpp ClassNotebook.cpp DlgPreferences.cpp FunctionData.cpp \
FunctionNotebook.cpp InformData.cpp InformNotebook.cpp MapWidget.cpp \
ObjectData.cpp ObjectNotebook.cpp ObjectTree.cpp Project.cpp \
RoomNotebook.cpp SettingsNotebook.cpp SourceFile.cpp SourceNotebook.cpp \
kazam.cpp BlankNotebook.cpp InformEditor.cpp InformComboBox.cpp \
MapCanvas.cpp DlgAddDoor.cpp


OBJECTS = ActorNotebook.o ClassNotebook.o DlgPreferences.o FunctionData.o \
FunctionNotebook.o InformData.o InformNotebook.o MapWidget.o ObjectData.o \
ObjectNotebook.o ObjectTree.o Project.o RoomNotebook.o \
SettingsNotebook.o SourceFile.o SourceNotebook.o kazam.o \
BlankNotebook.o InformEditor.o InformComboBox.o \
MapCanvas.o DlgAddDoor.o

# implementation

.SUFFIXES:	.o .cpp

CFLAGS = `wx-config --cflags`
.cpp.o :
	$(CC) -g -c $(CFLAGS) -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS) `wx-config --libs` -lwx_gtk_stc-2.4

clean: 
	rm -f *.o $(PROGRAM)

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

# DO NOT DELETE

ActorNotebook.o: Project.h InformData.h ActorNotebook.h InformEditor.h
ActorNotebook.o: InformNotebook.h ObjectData.h kazam.h ObjectTree.h
ClassNotebook.o: Project.h InformData.h ClassNotebook.h InformEditor.h
ClassNotebook.o: InformNotebook.h ObjectData.h kazam.h ObjectTree.h
DlgPreferences.o: DlgPreferences.h
FunctionData.o: FunctionData.h InformData.h
FunctionNotebook.o: Project.h InformData.h FunctionNotebook.h InformEditor.h
FunctionNotebook.o: InformNotebook.h FunctionData.h kazam.h ObjectTree.h
InformData.o: InformData.h SourceFile.h ObjectData.h FunctionData.h kazam.h
InformData.o: ObjectTree.h Project.h InformNotebook.h
InformNotebook.o: InformNotebook.h InformData.h kazam.h ObjectTree.h
InformNotebook.o: Project.h
MapWidget.o: MapWidget.h MapCanvas.h Project.h InformData.h
ObjectData.o: ObjectData.h InformData.h
ObjectNotebook.o: Project.h InformData.h ObjectNotebook.h InformEditor.h
ObjectNotebook.o: InformNotebook.h ObjectData.h kazam.h ObjectTree.h
ObjectTree.o: ObjectTree.h Project.h InformData.h InformNotebook.h
ObjectTree.o: ObjectData.h SourceFile.h kazam.h
Project.o: Project.h InformData.h ObjectData.h SourceFile.h kazam.h
Project.o: ObjectTree.h InformNotebook.h
RoomNotebook.o: RoomNotebook.h MapWidget.h MapCanvas.h Project.h InformData.h
RoomNotebook.o: ObjectData.h InformEditor.h InformComboBox.h InformNotebook.h
RoomNotebook.o: SourceFile.h kazam.h ObjectTree.h
SettingsNotebook.o: Project.h InformData.h SourceFile.h SettingsNotebook.h
SettingsNotebook.o: InformNotebook.h kazam.h ObjectTree.h
SourceFile.o: Project.h InformData.h SourceFile.h ObjectData.h FunctionData.h
SourceFile.o: kazam.h ObjectTree.h InformNotebook.h
SourceNotebook.o: Project.h InformData.h SourceFile.h SourceNotebook.h
SourceNotebook.o: InformEditor.h InformNotebook.h kazam.h ObjectTree.h
kazam.o: kazam.h ObjectTree.h Project.h InformData.h InformNotebook.h
kazam.o: DlgPreferences.h SourceFile.h BlankNotebook.h RoomNotebook.h
kazam.o: MapWidget.h MapCanvas.h ObjectData.h InformEditor.h InformComboBox.h
kazam.o: ClassNotebook.h ActorNotebook.h ObjectNotebook.h SourceNotebook.h
kazam.o: FunctionNotebook.h SettingsNotebook.h winres/mondrian.xpm
kazam.o: winres/help.xpm winres/compile.xpm winres/execute.xpm
kazam.o: winres/room.xpm winres/actor.xpm winres/object.xpm winres/class.xpm
kazam.o: winres/function.xpm winres/source.xpm winres/zoomin.xpm
kazam.o: winres/zoomout.xpm
BlankNotebook.o: Project.h InformData.h BlankNotebook.h InformNotebook.h
InformEditor.o: InformEditor.h
InformComboBox.o: InformComboBox.h
MapCanvas.o: MapCanvas.h Project.h InformData.h MapWidget.h DlgAddDoor.h
MapCanvas.o: ObjectData.h InformComboBox.h kazam.h ObjectTree.h
MapCanvas.o: InformNotebook.h
DlgAddDoor.o: DlgAddDoor.h ObjectData.h InformData.h InformComboBox.h kazam.h
DlgAddDoor.o: ObjectTree.h Project.h InformNotebook.h
