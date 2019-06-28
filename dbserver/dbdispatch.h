#ifndef _DBDISPATCH_H
#define _DBDISPATCH_H

#include "dbinit.h"

extern int start_zone_override;

// Generated by mkproto
void sendAccountLogout(U32 auth_id,int reason,int use_time);
void sendPlayerLogout(int player_id,int reason,bool keep_account_logged_in);
void handleContainerReq(Packet *pak,NetLink *link);
void sendContainerAcks(NetLink *link,int list_id,int callback_id,int count,int *ack_list);
void handleContainerSet(Packet *pak,NetLink *link);
void handleStatusReq(Packet *pak,NetLink *link);
void handleMapXfer(Packet *pak,NetLink *link);
void handleContainerAck(Packet *pak);
void handleMapXferTest(Packet *pak_in,NetLink *link);
void handleSaveLists(Packet *pak);
void sendFail(NetLink *link,int errorlog,int fail_code,char const *fmt, ...);
void handleAddDelGroupMembers(Packet *pak,NetLink *link);
void handlePlayerDisconnect(Packet *pak,DBClientLink *client);
void handleReqNames(Packet *pak_in,NetLink *link);
void handleContainerFindByElement(Packet *pak_in,NetLink *link);
void handleLog(Packet *pak);
void handleSendToAllServers(Packet *pak);
void handleReqAllOnline(Packet *pak_in, NetLink *link);
void handlePlayerRename(Packet *pak, NetLink *link);
void handlePlayerUnlock(Packet *pak, NetLink *link);
void handlePlayerAdjustServerSlots(Packet *pak, NetLink *link);
void handlePlayerChangeType(Packet *pak, NetLink *link);
void handlePlayerChangeSubType(Packet *pak, NetLink *link);
void handlePlayerChangePraetorianProgress(Packet *pak, NetLink *link);
void handlePlayerChangeInfluenceType(Packet *pak, NetLink *link);
void handleRenameResponse(Packet *pak);
void handleCheckNameResponse(Packet *pak);
void handleCharSlotGrant(Packet *pak);
void handleUnlockCharacterResponse(Packet *pak);

int dbHandleClientMsg(Packet *pak,int cmd, NetLink *link);
F32 dbGetCurrAvgLatency(int *cmd);
F32 dbGetCurrMaxLatency(int *cmd);
void dbShutdownTick();
void dbPrepShutdown(const char *msg, int req_quit_map_id);
void dbShutdown(const char *msg, int req_quit_map_id);
void dbDelinkMapserver(int map_id);
int dbIsShuttingDown(void);

int handleCreateEntByStr(char *str);
AnyContainer* handleContainerUpdate(int list_id,int id,int notdiff,char* str);
void handleSendPetition(char *authname, char *name, U32 ss2000, const char *summary, const char *msg, int cat, const char *notes);

char *putCharacterVerifyUniqueName(char *str);
char *putSupergroupVerifyUniqueName(char *str);
int putCharacterVerifyFreeSlot(char *str);
int allContainerIds(int** ilist, int* ilist_max, int list_id, int limit);

void updateAllServerLoggingInformation();
void sendLogLevels(NetLink * link, int cmd);
// End mkproto
#endif
