#ifndef _EDIT_CMD_SELECT_H
#define _EDIT_CMD_SELECT_H


// Generated by mkproto
void editSnapToTrackers(DefTracker** trackers);
void editCmdSnapDist();
void editUnhide(DefTracker *tracker,U32 trick_flags);
void editCmdSetPivot();
void editCmdSearch();
void editCmdInvertSelection();
void editCmdHide();
void editCmdHideOthers();
void editCmdFreeze();
void editCmdFreezeOthers();
void editCmdUnfreeze();
void editCmdLasso();
void editCmdUnhide();
void editCmdGroundSnap();
void editCmdSetQuickPlacementObject();
void editCmdSelect(DefTracker *tracker);
void editCmdSelectAll();
void editCmdSelectAllInstancesOfThisObject(void);
void editCmdCutCopy();
void editCmdPaste();
void editCmdDelete();
void editCmdLibDelete();
void editCmdCopyProperties();
void editCmdPasteProperties();
void editCmdEditTrick(void);
// End mkproto
#endif
