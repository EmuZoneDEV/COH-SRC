#ifndef _COMM_GAME_H
#define _COMM_GAME_H

#include "netio.h"

// mapserver to client
typedef enum ServerCommCmd
{
	SERVER_UPDATE = COMM_MAX_CMD,
	SERVER_ALLENTS,
	SERVER_CMDSHORTCUTS,
	SERVER_CONNECT_MSG,
	SERVER_GROUPS,
	SERVER_DYNAMIC_DEFS,
	SERVER_EDITMSG,
	SERVER_DEBUGCMD,
	SERVER_SENDENTDEBUGMENU,
	SERVER_MAP_XFER_WAIT,
	SERVER_MAP_XFER_LOADING,
	SERVER_MAP_XFER,
	SERVER_FORCE_LOGOUT,
	SERVER_GAME_CMDS,

	SERVER_RECEIVE_DIALOG,
	SERVER_RECEIVE_DIALOG_POWREQ,
	SERVER_RECEIVE_DIALOG_TEAMLEVEL,
	SERVER_RECEIVE_DIALOG_NAG,
	SERVER_RECEIVE_DIALOG_IGNORABLE,
	SERVER_RECEIVE_DIALOG_WIDTH,
	SERVER_RECEIVE_INFO_BOX,
	SERVER_DOOR_CMD,
	SERVER_SEND_CONSOLE_OUTPUT,

	SERVER_SET_CLIENT_STATE,
	SERVER_CON_PRINTF,
	SERVER_SEND_CHAT_MSG,
	SERVER_SEND_FLOATING_DAMAGE,

	SERVER_VISITED_MAP_CELLS,
	SERVER_ALL_STATIC_MAP_CELLS,
	SERVER_RESEND_WAYPOINT_REQUEST,
	SERVER_UPDATE_WAYPOINT,
	SERVER_SET_WAYPOINT,

	// team commands
	SERVER_TEAM_OFFER,

	// taskforce
	SERVER_TASKFORCE_KICK,
	SERVER_TASKFORCE_QUIT,

	// sg commands
	SERVER_SUPERGROUP_OFFER,
	SERVER_SGROUP_CREATE_REPLY,
	SERVER_SUPERGROUP_COSTUME,
	SERVER_HIDE_EMBLEM_UPDATE,

	// alliance commands
	SERVER_ALLIANCE_OFFER,

	// league commands
	SERVER_LEAGUE_OFFER,
	SERVER_LEAGUE_TEAM_KICK,
	SERVER_LEAGUE_ER_QUIT,

	// cutscene commands
	SERVER_CUTSCENE_UPDATE,

	// trade
	SERVER_TRADE_OFFER,
	SERVER_TRADE_INIT,
	SERVER_TRADE_CANCEL,
	SERVER_TRADE_UPDATE,
	SERVER_TRADE_SUCESS,

	SERVER_SEND_FRIENDS,

	SERVER_CONTROL_PLAYER,			// Tell a client that it can take control of another player.
	SERVER_CONTACT_DIALOG_OPEN,		// open the contact dialog box
	SERVER_CONTACT_DIALOG_CLOSE,	// close the contact dialog box
	SERVER_CONTACT_DIALOG_OK,		// simple ok dialog
	SERVER_CONTACT_DIALOG_YESNO,	// simple yes/no dialog
	SERVER_CONTACT_STATUS,			// Mapserver sending contact info update to the client
	SERVER_ACCESSIBLE_CONTACT_STATUS,	// Mapserver sending accessible contact info to the client
	SERVER_CONTACT_SELECT,			// Mapserver causing selection of contact
	SERVER_TASK_STATUS,				// mapserver sending task info update to the client
	SERVER_TASK_SELECT,				// mapserver asking client to select a task
	SERVER_TASK_REMOVE_TEAMMATE_TASKS,	// mapserver sending task info update to the client
	SERVER_MISSION_ENTRY,			// Mapserver is sending mission entry dialog
	SERVER_MISSION_EXIT,			// Mapserver is sending mission exit dialog
	SERVER_MISSION_KICKTIMER,		// tell client it will be kicked from mission at specified time
	SERVER_TEAMCOMPLETE_PROMPT,		// Prompt the client to chose to complete their task
	SERVER_BROKER_CONTACT,			// Alert player he can receive a new contact
	SERVER_DEAD_NOGURNEY,			// show dialog before resurrection
	SERVER_DOORANIM,				// server is requesting client animate into door
	SERVER_DOOREXIT,				// server is requesting client fade in again after door anim
	SERVER_FACE_ENTITY,
	SERVER_FACE_LOCATION,
	SERVER_SEND_FLOATING_INFO,
	SERVER_SEND_PLAY_SOUND,
	SERVER_SEND_FADE_SOUND,
	SERVER_SEND_DESATURATE_INFO,
	SERVER_SET_STANCE,
	SERVER_MAP_XFER_LIST,
	SERVER_MAP_XFER_LIST_INIT,
	SERVER_MAP_XFER_LIST_APPEND,
	SERVER_MAP_XFER_LIST_CLOSE,
	SERVER_ENABLE_CONTROL_LOG,

	SERVER_LEVEL_UP,
	SERVER_LEVEL_UP_RESPEC,
	SERVER_NEW_TITLE,

	SERVER_PERFORMANCE_UPDATE,

	SERVER_CSR_BUG_REPORT,		// allows CSR to send bug report for another player

	SERVER_SEND_EMAIL_HEADERS,
	SERVER_SEND_EMAIL_MESSAGE,
	SERVER_SEND_EMAIL_MESSAGE_STATUS,

	// Visit location task related commands
	SERVER_TASK_VISITLOCATION_UPDATE,	// Tell the client the full list of location group names they are to visit.

	SERVER_SEND_PLAYER_INFO,
	SERVER_SEND_COMBAT_STATS,
	SERVER_CLUE_UPDATE,
	SERVER_KEYCLUE_UPDATE,
	SERVER_SOUVENIRCLUE_UPDATE,
	SERVER_SOUVENIRCLUE_DETAIL,
	SERVER_MISSION_OBJECTIVE_TIMER_UPDATE,

	SERVER_REGISTER_SUPERGROUP,
	SERVER_SUPERGROUP_LIST,
	SERVER_TASK_DETAIL,

	SERVER_BROWSER_TEXT,
	SERVER_BROWSER_CLOSE,

	SERVER_STORE_OPEN,
	SERVER_STORE_CLOSE,

	SERVER_BUG_REPORT,    // server sends /bugqa report to client for local logging (QA purposes)

	SERVER_MISSION_SURVEY,
	SERVER_SEND_TRAY_ADD,

	SERVER_COMBINE_RESPONSE,
	SERVER_BOOSTER_RESPONSE,
	SERVER_CATALYST_RESPONSE,

	// Arena stuff
	SERVER_ARENA_KIOSK,
	SERVER_ARENA_EVENTS,
	SERVER_ARENA_EVENTUPDATE,
	SERVER_ARENA_UPDATE_TRAY_DISABLE,
	SERVER_ARENAKIOSK_SETOPEN,
	SERVER_ARENA_OFFER,
	SERVER_ARENA_ERROR,

	SERVER_SGRAID_COMPASS_STRING,
	SERVER_SGRAID_UPDATE,
	SERVER_SGRAID_ERROR,
	SERVER_SGRAID_OFFER,
	SERVER_SGRAID_INFO,

	SERVER_TAILOR_OPEN,
	SERVER_RESPEC_OPEN,
	SERVER_AUCTION_OPEN,
	SERVER_SEARCH_RESULTS,

	SERVER_VALIDATE_RESPEC,
	SERVER_SEND_REWARD_CHOICE,

	SERVER_SALVAGE_IMMEDIATE_USE_STATUS,

	SERVER_SEND_PACKAGEDENT,

	SERVER_LARGE_PACKET,

	SERVER_SCRIPT_DEBUGINFO,

	SERVER_SHARDCOMM,

	SERVER_ARENA_UPDATE_PLAYER,
	SERVER_ARENA_REQRESULTS,
	SERVER_ARENA_START_COUNT,
	SERVER_ARENA_COMPASS_STRING,
	SERVER_ARENA_VICTORY_INFORMATION,
	SERVER_ARENA_SCHEDULED_TELEPORT,
	SERVER_ARENA_RUN_EVENT_WINDOW,
	SERVER_ARENA_NOTIFY_FINISH,

	SERVER_SCRIPT_UI,
	SERVER_SCRIPT_UI_UPDATE,

	SERVER_MISSION_WAYPOINT,
	SERVER_MISSION_KEYDOOR,

	SERVER_PET_UI_UPDATE,

	SERVER_ARENA_MANAGEPETS,
	SERVER_UPDATE_PET_STATE,
	SERVER_BASE_CREATE,

	//map generation
	SERVER_MAP_GENERATION,

	SERVER_UPDATE_BASE_INTERACT,
	SERVER_UPDATE_BASE_MODE,
	SERVER_UPDATE_RAID_STATE,
	SERVER_UPDATE_NAME,
	SERVER_UPDATE_PRESTIGE,

	SERVER_MISSION_CONTROL,
	SERVER_DEBUG_LOCATION,
	SERVER_EDIT_VILLAIN_COSTUME,

	SERVER_PLAQUE_DIALOG,
	SERVER_RAID_RESULT,
	SERVER_AUCTION_HISTORY,
	SERVER_AUCTION_ITEMSTATUS,
	SERVER_AUCTION_BATCH_ITEMSTATUS,
	SERVER_AUCTION_LIST_ITEMSTATUS,
	SERVER_AUCTION_BANNED_UPDATE,
	SERVER_ACCOUNTSERVER_CHARCOUNT,
	SERVER_ACCOUNTSERVER_CATALOG,
	SERVER_ACCOUNTSERVER_CLIENTAUTH,
	SERVER_ACCOUNTSERVER_PLAYNC_AUTH_KEY,
	SERVER_ACCOUNTSERVER_NOTIFYREQUEST,
	SERVER_ACCOUNTSERVER_INVENTORY,

	SERVER_FLASHBACK_LIST,
	SERVER_FLASHBACK_DETAILS,
	SERVER_FLASHBACK_IS_ELIGIBLE,
	SERVER_FLASHBACK_CONFIRM,

	SERVER_TASKFORCE_TIME_LIMITS,
	SERVER_SETDEBUGPOWER,
	SERVER_AUTH_BITS,
	SERVER_SELECT_BUILD,

	SERVER_MISSIONSERVER_SEARCHPAGE,
	SERVER_MISSIONSERVER_ARCINFO,
	SERVER_MISSIONSERVER_ARCDATA,
	SERVER_ARCHITECTKIOSK_SETOPEN,

	SERVER_SG_COLOR_DATA,
	SERVER_RENAME_BUILD,
	SERVER_COSTUME_CHANGE_EMOTE_LIST,
	SERVER_CONFIRM_SG_PROMOTE,
	SERVER_LEVELINGPACT_INVITE,

	SERVER_ARCHITECT_COMPLETE,
	SERVER_ARCHITECT_SOUVENIR,
	SERVER_ARCHITECT_INVENTORY,

	SERVER_UPDATE_BADGE_MONITOR_LIST,
	SERVER_SEND_DIALOG,
	SERVER_SEND_TOKEN,
	SERVER_SG_PERMISSIONS,
	// Not yet
	//SERVER_ENABLE_ASSERT_ON_BS_ERRORS,

	SERVER_BEGIN_BYTE_ALIGNED_DATA,
	SERVER_SEND_ITEMLOCATIONS,
	SERVER_SEND_CRITTERLOCATIONS,
	SERVER_SEND_MORAL_CHOICE,
	SERVER_DETACH_SCRIPT_UI,

	SERVER_SEND_POP_HELP_TAG,
	SERVER_SEND_POP_HELP_RESET,
	SERVER_SEND_POP_HELP_N,

	SERVER_SEND_KARMA_STATS,

	// Turnstile server
	SERVER_EVENT_LIST,
	SERVER_QUEUE_STATUS,
	SERVER_EVENT_READY,
	SERVER_EVENT_READY_ACCEPT,
	SERVER_EVENT_START_STATUS,
	SERVER_EVENT_STATUS,
	SERVER_EVENT_JOIN_LEADER,

	SERVER_MISSIONSERVER_ARCDATA_OTHERUSER,
	SERVER_GOINGROGUE_NAG,
	SERVER_INCARNATE_TRIAL_STATUS,
	SERVER_POPHELP_EVENT_HAPPENED_BY_TAG,
	
	SERVER_VISITOR_SHARD_XFER_DATA,
	SERVER_VISITOR_SHARD_XFER_JUMP,

	SERVER_CHANGE_WINDOW_COLORS,

	SERVER_UPDATE_TURNSTILE_STATUS,
	SERVER_ENDGAMERAID_VOTE_KICK,

	SERVER_COMBAT_MESSAGE,

	SERVER_SHOW_CONTACT_FINDER_WINDOW,
	
	SERVER_OPEN_SALVAGE_REQUEST,
	SERVER_OPEN_SALVAGE_RESULTS,

	SERVER_WEB_STORE_OPEN_PRODUCT,

	SERVER_CONVERT_ENHANCEMENT_RESULT,

	SERVER_SUPPORT_HOME,
	SERVER_SUPPORT_KB,
	SERVER_DISPLAY_PRODUCT_PAGE,
	SERVER_FORCE_QUEUE_FOR_EVENT,

	SERVER_START_FORCED_MOVEMENT,

	// MS: ATTENTION: SERVER_CMD_COUNT is the _LAST_ thing in this list.
	//     Putting anything afterwards is an instantaneous dunce-cap offense.
	//     And I don't mean DUNC cap, because putting something below SERVER_CMD_COUNT
	//     doesn't make you cool enough to be the DUNC.
	SERVER_CMD_COUNT,
	// MS: What?  You were thinking of putting something down here?
	//     Think again, Einstein.  You might want to read the previous comment
	//     about how incredibly lame it is to put something down here.
} ServerCommCmd;
// put additions to this list at the end of this file in getServerCmdName()

typedef enum ClientCommCmd
{
	CLIENT_INPUT = COMM_MAX_CMD,
	CLIENT_REQSCENE,
	CLIENT_REQSHORTCUTS,
	CLIENT_REQALLENTS,
	CLIENT_READY,
	CLIENT_DISCONNECT,
	CLIENT_QUITSERVER,
	CLIENT_CONNECT,
	CLIENT_WORLDCMD,
	CLIENT_MAP_XFER_WAITING,
	CLIENT_MAP_XFER_ACK,
	CLIENT_BUG,
	CLIENT_ACCOUNT_SERVER_CMD,
	CLIENT_LOGPRINTF,
	CLIENT_BASEPOWER_NOT_FOUND,
	CLIENT_LWC_STAGE,
	CLIENT_STOP_FORCED_MOVEMENT,
	CLIENT_CMD_MAX,
} ClientCommCmd;
// put additions to this list in svr_tick.c - svrHandleClientMsg()

// Used by entsend and entrecv.
typedef enum
{
	AT_NO_APPEARANCE,   // No appearance is being specified.
	AT_ENTIRE_COSTUME,  // The entire costume is being sent.  This only happens for players currently.
	AT_OWNED_COSTUME,	// A costume that should be destroyed along with the entity.
	AT_NPC_COSTUME,     // The costume is specified by an npc index and a costume index.
	// JS: This may turn into a costume filename + index soon.
	AT_SEQ_NAME,        // The seq name is specified.
} AppearanceType;

typedef enum TradeFail
{
	TRADE_FAIL_OTHER_LEFT,
	TRADE_FAIL_I_LEFT,
	TRADE_FAIL_CORRUPT,
	TRADE_FAIL_INVENTORY,
	TRADE_FAIL_DISTANCE,
}TradeFail;

typedef enum HideType
{
	HIDE_SEARCH,
	HIDE_SG,
	HIDE_FRIENDS,
	HIDE_GLOBAL_FRIENDS,
	HIDE_GLOBAL_CHANNELS,
	HIDE_TELL,
	HIDE_INVITE,
	HIDE_COUNT,
}HideType;

typedef enum BuffSettings
{
	kBuff_HideAuto, // 0
	kBuff_HideToggle, // 1
	kBuff_NoBlink, //2
	kBuff_NoStack, //3
	kBuff_NumStack, //4
	kBuff_NoNumbers, //5
	kBuff_NoSend, //6
	// 7 ok
	// this is being used to cram 3 numbers into 1 int, do not exceed 8 bits 
}BuffSettings; 

typedef enum EditorMessageType
{
	EDITOR_MESSAGE_JUST_SAVE_LINK = -1,
	EDITOR_MESSAGE_LOW_PRIORITY = 0,
	EDITOR_MESSAGE_ERROR		= 1,
	EDITOR_MESSAGE_HIGH_PRIORITY = 2, //Not an error, just worth a box
	EDITOR_MESSAGE_ERROR_SERVER_WRITE = 3,

}EditorMessageType;

// for bug reporting
enum
{	BUG_REPORT_NORMAL,
BUG_REPORT_INTERNAL,
BUG_REPORT_LOCAL,
BUG_REPORT_SURVEY,
BUG_REPORT_CSR,
};

typedef enum AccountServerCmdType
{
	ACCOUNT_SERVER_CMD_CHAR_COUNT,
	ACCOUNT_SERVER_CMD_GET_CATALOG,
	ACCOUNT_SERVER_CMD_GET_INVENTORY,
	ACCOUNT_SERVER_CMD_GET_ACTIVE_ACCOUNT,
} AccountServerCmdType;

#endif // _COMM_GAME_H


#ifdef SERVERCMDNAME_FUNCTION

#include "utils.h"
const char* getServerCmdName(int cmd)
{
	static char* names[SERVER_CMD_COUNT];
	static int init = 0;

	if(!init)
	{
		int i;
		int last = -1;
		char* msg = "A SERVER cmd was listed out of order.  Keep these commands in order.";

		init = 1;

		PERFINFO_AUTO_START("getServerCmdName - init", 1);

#define SET_NAME(x) {assertmsg(x == last + 1, msg);assert(x >= 0 && x < ARRAY_SIZE(names));names[x] = #x;last = x;}

		SET_NAME(COMM_CONTROLCOMMAND);
		SET_NAME(COMM_CONNECT);
		SET_NAME(SERVER_UPDATE);
		SET_NAME(SERVER_ALLENTS);
		SET_NAME(SERVER_CMDSHORTCUTS);
		SET_NAME(SERVER_CONNECT_MSG);
		SET_NAME(SERVER_GROUPS);
		SET_NAME(SERVER_DYNAMIC_DEFS);
		SET_NAME(SERVER_EDITMSG);
		SET_NAME(SERVER_DEBUGCMD);
		SET_NAME(SERVER_SENDENTDEBUGMENU);
		SET_NAME(SERVER_MAP_XFER_WAIT);
		SET_NAME(SERVER_MAP_XFER_LOADING);
		SET_NAME(SERVER_MAP_XFER);
		SET_NAME(SERVER_FORCE_LOGOUT);
		SET_NAME(SERVER_GAME_CMDS);
		SET_NAME(SERVER_RECEIVE_DIALOG);
		SET_NAME(SERVER_RECEIVE_DIALOG_POWREQ);
		SET_NAME(SERVER_RECEIVE_DIALOG_TEAMLEVEL);
		SET_NAME(SERVER_RECEIVE_DIALOG_NAG);
		SET_NAME(SERVER_RECEIVE_DIALOG_IGNORABLE);
		SET_NAME(SERVER_RECEIVE_DIALOG_WIDTH);
		SET_NAME(SERVER_RECEIVE_INFO_BOX);
		SET_NAME(SERVER_DOOR_CMD);
		SET_NAME(SERVER_SEND_CONSOLE_OUTPUT);
		SET_NAME(SERVER_SET_CLIENT_STATE);
		SET_NAME(SERVER_CON_PRINTF);
		SET_NAME(SERVER_SEND_CHAT_MSG);
		SET_NAME(SERVER_SEND_FLOATING_DAMAGE);
		SET_NAME(SERVER_VISITED_MAP_CELLS);
		SET_NAME(SERVER_ALL_STATIC_MAP_CELLS);
		SET_NAME(SERVER_RESEND_WAYPOINT_REQUEST);
		SET_NAME(SERVER_UPDATE_WAYPOINT);
		SET_NAME(SERVER_SET_WAYPOINT);
		SET_NAME(SERVER_TEAM_OFFER);
		SET_NAME(SERVER_TASKFORCE_KICK);
		SET_NAME(SERVER_TASKFORCE_QUIT);
		SET_NAME(SERVER_SUPERGROUP_OFFER);
		SET_NAME(SERVER_SGROUP_CREATE_REPLY);
		SET_NAME(SERVER_SUPERGROUP_COSTUME);
		SET_NAME(SERVER_HIDE_EMBLEM_UPDATE);
		SET_NAME(SERVER_ALLIANCE_OFFER);
		SET_NAME(SERVER_LEAGUE_OFFER);
		SET_NAME(SERVER_LEAGUE_TEAM_KICK);
		SET_NAME(SERVER_LEAGUE_ER_QUIT);
		SET_NAME(SERVER_CUTSCENE_UPDATE);
		SET_NAME(SERVER_TRADE_OFFER);
		SET_NAME(SERVER_TRADE_INIT);
		SET_NAME(SERVER_TRADE_CANCEL);
		SET_NAME(SERVER_TRADE_UPDATE);
		SET_NAME(SERVER_TRADE_SUCESS);
		SET_NAME(SERVER_SEND_FRIENDS);
		SET_NAME(SERVER_CONTROL_PLAYER);			// Tell a client that it can take control of another player.
		SET_NAME(SERVER_CONTACT_DIALOG_OPEN);		// open the contact dialog box
		SET_NAME(SERVER_CONTACT_DIALOG_CLOSE);	// close the contact dialog box
		SET_NAME(SERVER_CONTACT_DIALOG_OK);		// simple ok dialog
		SET_NAME(SERVER_CONTACT_DIALOG_YESNO);	// simple yes/no dialog
		SET_NAME(SERVER_CONTACT_STATUS);			// Mapserver sending contact info update to the client
		SET_NAME(SERVER_ACCESSIBLE_CONTACT_STATUS);	// Mapserver sending accessible contact info to the client
		SET_NAME(SERVER_CONTACT_SELECT);			// Mapserver causing selection of contact
		SET_NAME(SERVER_TASK_STATUS);
		SET_NAME(SERVER_TASK_SELECT);
		SET_NAME(SERVER_TASK_REMOVE_TEAMMATE_TASKS);
		SET_NAME(SERVER_MISSION_ENTRY);			// Mapserver is sending mission entry dialog
		SET_NAME(SERVER_MISSION_EXIT);			// Mapserver is sending mission exit dialog
		SET_NAME(SERVER_MISSION_KICKTIMER);		// tell client it will be kicked from mission at specified time
		SET_NAME(SERVER_TEAMCOMPLETE_PROMPT);	// Prompt the client to chose to complete their task
		SET_NAME(SERVER_BROKER_CONTACT);		// Alert player he can receive a new contact
		SET_NAME(SERVER_DEAD_NOGURNEY);			// show dialog before resurrection
		SET_NAME(SERVER_DOORANIM);				// server is requesting client animate into door
		SET_NAME(SERVER_DOOREXIT);				// server is requesting client fade in again after door anim
		SET_NAME(SERVER_FACE_ENTITY);
		SET_NAME(SERVER_FACE_LOCATION);
		SET_NAME(SERVER_SEND_FLOATING_INFO);
		SET_NAME(SERVER_SEND_PLAY_SOUND);
		SET_NAME(SERVER_SEND_FADE_SOUND);
		SET_NAME(SERVER_SEND_DESATURATE_INFO);
		SET_NAME(SERVER_SET_STANCE);
		SET_NAME(SERVER_MAP_XFER_LIST);
		SET_NAME(SERVER_MAP_XFER_LIST_INIT);
		SET_NAME(SERVER_MAP_XFER_LIST_APPEND);
		SET_NAME(SERVER_MAP_XFER_LIST_CLOSE);
		SET_NAME(SERVER_ENABLE_CONTROL_LOG);
		SET_NAME(SERVER_LEVEL_UP);
		SET_NAME(SERVER_LEVEL_UP_RESPEC);
		SET_NAME(SERVER_NEW_TITLE);
		SET_NAME(SERVER_PERFORMANCE_UPDATE);
		SET_NAME(SERVER_CSR_BUG_REPORT);
		SET_NAME(SERVER_SEND_EMAIL_HEADERS);
		SET_NAME(SERVER_SEND_EMAIL_MESSAGE);
		SET_NAME(SERVER_SEND_EMAIL_MESSAGE_STATUS);
		SET_NAME(SERVER_TASK_VISITLOCATION_UPDATE);	// Tell the client the full list of location group names they are to visit.
		SET_NAME(SERVER_SEND_PLAYER_INFO);
		SET_NAME(SERVER_SEND_COMBAT_STATS);
		SET_NAME(SERVER_CLUE_UPDATE);
		SET_NAME(SERVER_KEYCLUE_UPDATE);
		SET_NAME(SERVER_SOUVENIRCLUE_UPDATE);
		SET_NAME(SERVER_SOUVENIRCLUE_DETAIL);
		SET_NAME(SERVER_MISSION_OBJECTIVE_TIMER_UPDATE);
		SET_NAME(SERVER_REGISTER_SUPERGROUP);
		SET_NAME(SERVER_SUPERGROUP_LIST);
		SET_NAME(SERVER_TASK_DETAIL);
		SET_NAME(SERVER_BROWSER_TEXT);
		SET_NAME(SERVER_BROWSER_CLOSE);
		SET_NAME(SERVER_STORE_OPEN);
		SET_NAME(SERVER_STORE_CLOSE);
		SET_NAME(SERVER_BUG_REPORT);
		SET_NAME(SERVER_MISSION_SURVEY);
		SET_NAME(SERVER_SEND_TRAY_ADD);
		SET_NAME(SERVER_COMBINE_RESPONSE);
		SET_NAME(SERVER_BOOSTER_RESPONSE);
		SET_NAME(SERVER_CATALYST_RESPONSE);
		SET_NAME(SERVER_ARENA_KIOSK);
		SET_NAME(SERVER_ARENA_EVENTS);
		SET_NAME(SERVER_ARENA_EVENTUPDATE);
		SET_NAME(SERVER_ARENA_UPDATE_TRAY_DISABLE);
		SET_NAME(SERVER_ARENAKIOSK_SETOPEN);
		SET_NAME(SERVER_ARENA_OFFER);
		SET_NAME(SERVER_ARENA_ERROR);

		SET_NAME(SERVER_SGRAID_COMPASS_STRING);
		SET_NAME(SERVER_SGRAID_UPDATE);
		SET_NAME(SERVER_SGRAID_ERROR);
		SET_NAME(SERVER_SGRAID_OFFER);
		SET_NAME(SERVER_SGRAID_INFO);

		SET_NAME(SERVER_TAILOR_OPEN);
		SET_NAME(SERVER_RESPEC_OPEN);
		SET_NAME(SERVER_AUCTION_OPEN);
		SET_NAME(SERVER_SEARCH_RESULTS);
		SET_NAME(SERVER_VALIDATE_RESPEC);
		SET_NAME(SERVER_SEND_REWARD_CHOICE);

		SET_NAME(SERVER_SALVAGE_IMMEDIATE_USE_STATUS);

		SET_NAME(SERVER_SEND_PACKAGEDENT);
		SET_NAME(SERVER_LARGE_PACKET);
		SET_NAME(SERVER_SCRIPT_DEBUGINFO);
		SET_NAME(SERVER_SHARDCOMM);

		SET_NAME(SERVER_ARENA_UPDATE_PLAYER);
		SET_NAME(SERVER_ARENA_REQRESULTS);
		SET_NAME(SERVER_ARENA_START_COUNT);
		SET_NAME(SERVER_ARENA_COMPASS_STRING);
		SET_NAME(SERVER_ARENA_VICTORY_INFORMATION);
		SET_NAME(SERVER_ARENA_SCHEDULED_TELEPORT);
		SET_NAME(SERVER_ARENA_RUN_EVENT_WINDOW);
		SET_NAME(SERVER_ARENA_NOTIFY_FINISH);

		SET_NAME(SERVER_SCRIPT_UI);
		SET_NAME(SERVER_SCRIPT_UI_UPDATE);

		SET_NAME(SERVER_MISSION_WAYPOINT);
		SET_NAME(SERVER_MISSION_KEYDOOR);

		SET_NAME(SERVER_PET_UI_UPDATE);

		SET_NAME(SERVER_ARENA_MANAGEPETS);
		SET_NAME(SERVER_UPDATE_PET_STATE);
		SET_NAME(SERVER_BASE_CREATE);
		SET_NAME(SERVER_MAP_GENERATION);
		SET_NAME(SERVER_UPDATE_BASE_INTERACT);
		SET_NAME(SERVER_UPDATE_BASE_MODE);
		SET_NAME(SERVER_UPDATE_RAID_STATE);
		SET_NAME(SERVER_UPDATE_NAME);
		SET_NAME(SERVER_UPDATE_PRESTIGE);
		SET_NAME(SERVER_MISSION_CONTROL);
		SET_NAME(SERVER_DEBUG_LOCATION);
		SET_NAME(SERVER_EDIT_VILLAIN_COSTUME);

		SET_NAME(SERVER_PLAQUE_DIALOG);
		SET_NAME(SERVER_RAID_RESULT);
		SET_NAME(SERVER_AUCTION_HISTORY);
		SET_NAME(SERVER_AUCTION_ITEMSTATUS);
		SET_NAME(SERVER_AUCTION_BATCH_ITEMSTATUS);
		SET_NAME(SERVER_AUCTION_LIST_ITEMSTATUS);
		SET_NAME(SERVER_AUCTION_BANNED_UPDATE);
		SET_NAME(SERVER_ACCOUNTSERVER_CHARCOUNT);
		SET_NAME(SERVER_ACCOUNTSERVER_CATALOG);
		SET_NAME(SERVER_ACCOUNTSERVER_CLIENTAUTH);
		SET_NAME(SERVER_ACCOUNTSERVER_PLAYNC_AUTH_KEY);		
		SET_NAME(SERVER_ACCOUNTSERVER_NOTIFYREQUEST);
		SET_NAME(SERVER_ACCOUNTSERVER_INVENTORY);

		SET_NAME(SERVER_FLASHBACK_LIST);
		SET_NAME(SERVER_FLASHBACK_DETAILS);
		SET_NAME(SERVER_FLASHBACK_IS_ELIGIBLE);
		SET_NAME(SERVER_FLASHBACK_CONFIRM);

		SET_NAME(SERVER_TASKFORCE_TIME_LIMITS);
		SET_NAME(SERVER_SETDEBUGPOWER);
		SET_NAME(SERVER_AUTH_BITS);
		SET_NAME(SERVER_SELECT_BUILD);
		SET_NAME(SERVER_MISSIONSERVER_SEARCHPAGE);
		SET_NAME(SERVER_MISSIONSERVER_ARCINFO);
		SET_NAME(SERVER_MISSIONSERVER_ARCDATA);
		SET_NAME(SERVER_ARCHITECTKIOSK_SETOPEN);
		
		SET_NAME(SERVER_SG_COLOR_DATA);
		SET_NAME(SERVER_RENAME_BUILD);
		SET_NAME(SERVER_COSTUME_CHANGE_EMOTE_LIST);

		SET_NAME(SERVER_CONFIRM_SG_PROMOTE);

		SET_NAME(SERVER_LEVELINGPACT_INVITE);
		SET_NAME(SERVER_ARCHITECT_COMPLETE);
		SET_NAME(SERVER_ARCHITECT_SOUVENIR);
		SET_NAME(SERVER_ARCHITECT_INVENTORY);

		SET_NAME(SERVER_UPDATE_BADGE_MONITOR_LIST);
		SET_NAME(SERVER_SEND_DIALOG);
		SET_NAME(SERVER_SEND_TOKEN);

		SET_NAME(SERVER_SG_PERMISSIONS);
		// Not yet
		//SET_NAME(SERVER_ENABLE_ASSERT_ON_BS_ERRORS);		

		SET_NAME(SERVER_BEGIN_BYTE_ALIGNED_DATA);
		SET_NAME(SERVER_SEND_ITEMLOCATIONS);
		SET_NAME(SERVER_SEND_CRITTERLOCATIONS);

		SET_NAME(SERVER_SEND_MORAL_CHOICE);
		SET_NAME(SERVER_DETACH_SCRIPT_UI);
		SET_NAME(SERVER_SEND_POP_HELP_TAG);
		SET_NAME(SERVER_SEND_POP_HELP_RESET);
		SET_NAME(SERVER_SEND_POP_HELP_N);
		SET_NAME(SERVER_SEND_KARMA_STATS);

		SET_NAME(SERVER_EVENT_LIST);
		SET_NAME(SERVER_QUEUE_STATUS);
		SET_NAME(SERVER_EVENT_READY);
		SET_NAME(SERVER_EVENT_READY_ACCEPT);
		SET_NAME(SERVER_EVENT_START_STATUS);
		SET_NAME(SERVER_EVENT_STATUS);
		SET_NAME(SERVER_EVENT_JOIN_LEADER);

		SET_NAME(SERVER_MISSIONSERVER_ARCDATA_OTHERUSER);
		SET_NAME(SERVER_GOINGROGUE_NAG);
		SET_NAME(SERVER_INCARNATE_TRIAL_STATUS);
		SET_NAME(SERVER_POPHELP_EVENT_HAPPENED_BY_TAG);

		SET_NAME(SERVER_VISITOR_SHARD_XFER_DATA);
		SET_NAME(SERVER_VISITOR_SHARD_XFER_JUMP);

		SET_NAME(SERVER_CHANGE_WINDOW_COLORS);
		SET_NAME(SERVER_UPDATE_TURNSTILE_STATUS);
		SET_NAME(SERVER_ENDGAMERAID_VOTE_KICK);
		SET_NAME(SERVER_COMBAT_MESSAGE);

		SET_NAME(SERVER_SHOW_CONTACT_FINDER_WINDOW);
		SET_NAME(SERVER_OPEN_SALVAGE_REQUEST);
		SET_NAME(SERVER_OPEN_SALVAGE_RESULTS);
		SET_NAME(SERVER_WEB_STORE_OPEN_PRODUCT);
		SET_NAME(SERVER_CONVERT_ENHANCEMENT_RESULT);
		SET_NAME(SERVER_SUPPORT_HOME);
		SET_NAME(SERVER_SUPPORT_KB);
		SET_NAME(SERVER_DISPLAY_PRODUCT_PAGE);
		SET_NAME(SERVER_FORCE_QUEUE_FOR_EVENT);
		SET_NAME(SERVER_START_FORCED_MOVEMENT);
#undef SET_NAME

		assertmsg(last + 1 == SERVER_CMD_COUNT, "A SERVER cmd was missing from the end of the list.");

		for(i = 0; i < ARRAY_SIZE(names); i++)
		{
			if(!names[i])
			{
				char buffer[1000];
				sprintf(buffer, "SERVER CMD NAME NOT SET: %d\n\nPlease add it in file (%s) in function %s.", i, __FILE__, __FUNCTION__);
				assertmsg(0, buffer);
				sprintf(buffer, "SERVER CMD NAME NOT SET(%d)", i);
				names[i] = strdup(buffer);

				/********************************************************************************************
				* Dear Programmers,
				*
				* If you hit this assert then there is a SERVER cmd (with value "i") that
				* is missing its SET_NAME(..) above.  Please add it.  Thanks,
				*
				* -Martin
				*
				* p.s. To figure out which one has value "i", get the value of i and then hover
				*      your mouse over the enums in comm_game.h until you find the one with that value.
				********************************************************************************************/
			}
		}

		PERFINFO_AUTO_STOP();
	}

	if(cmd >= 0 && cmd < ARRAY_SIZE(names))
		return names[cmd];
	else
		return "Invalid cmd index";
}

#endif // SERVERCMDNAME_FUNCTION
