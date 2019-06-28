#ifndef _CSDBCLIENT_H
#define _CSDBCLIENT_H
 
#include "netio.h"
 

// Generated by mkproto
void csdbSendPlayerValid(EntCon *ent_con);
void sendLoginFailure(NetLink *link,int cookie,char *errmsg);
void sendPlayerLogout(int player_id,int unload);
void sendLoginInfoToGameClient(EntCon *ent_con,int login_cookie);
MapCon *setNewMap(EntCon *ent_con,int map_id,NetLink *link,int cookie,char *errmsg,int is_map_xfer,int write_sql);
void handlePlayerLogin(Packet *pak,NetLink *link);
void handlePlayerDelete(Packet *pak);
void handleReqPlayers(Packet *pak_in,NetLink *link);
void handleForceLogout(Packet *pak);
int tryConnectConnServer(char *ip_str,int dbserver_id,char *shard_name);
void dbCsDisconnect();
int connConnected();
// End mkproto
#endif