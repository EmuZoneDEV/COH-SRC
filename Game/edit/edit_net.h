#ifndef _EDIT_NET_H
#define _EDIT_NET_H

#include "network\netio.h"
#include "grouptrack.h"

// Generated by mkproto
void sendTracker(Packet *pak,DefTracker *tracker);
void editUngroup(int edit_open);
void editGroup(int attach);
void editDetach();
void editDelete();
void editPaste();
void editNew();
void saveOnExit();
void editSave(char *fname);
void editSaveSelection(char *fname);
void editSaveLibs();
void editLoad(char *fname,int import);
void editUpdateTracker(DefTracker *tracker);
void editUpdateTrackers(DefTracker ** tracker);
void editUpdateDef(GroupDef *def,char *fullpath,char *objname);
void editUndo();
void editRedo();
void editSendSceneFile(char *scenefile);
void editSendLoadScreen(char *loadscreen);
void editNetUpdate();
void editSendPlayerPos();
void editDefLoad(char *name);
void editTraySwap(char *fname);
void editCmd(char *cmd);
// End mkproto
#endif
