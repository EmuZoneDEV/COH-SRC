#include "textparser.h"

//#ifed-out include to fool incredibuild dependencies
#if 0
#include "c:\game\code\CoH\MissionServer\.\MissionServer.h"
#endif



//auto-generated staticdefine for enum MissionComplaintType
//autogeneratednocheckin
StaticDefineInt MissionComplaintTypeEnum[] =
{
	DEFINE_INT
	{ "Complaint", kMissionComplaint_Complaint},
	{ "Comment", kMissionComplaint_Comment},
	{ "BanMessage", kMissionComplaint_BanMessage},
	{ "Count", kMissionComplaint_Count},
	DEFINE_END
};

//autogeneratednocheckin
//Structparser.exe autogenerated ParseTable for struct MissionComplaint
ParseTable parse_MissionComplaint[] =
{
	{ "MissionComplaint", 	TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MissionComplaint), 0, NULL, 0 },
	{ "complainer_id",		TOK_STRUCTPARAM | TOK_AUTOINT(MissionComplaint, complainer_id, 0), NULL },
	{ "complainer_region",	TOK_STRUCTPARAM | TOK_AUTOINT(MissionComplaint, complainer_region, 0), AuthRegionEnum },
	{ "comment",			TOK_STRUCTPARAM | TOK_STRING(MissionComplaint, comment, 0), NULL },
	{ "type",				TOK_STRUCTPARAM | TOK_AUTOINT(MissionComplaint, type, 0), MissionComplaintTypeEnum },
	{ "Sent",				TOK_STRUCTPARAM | TOK_AUTOINT(MissionComplaint, bSent, 0), NULL },
	{ "globalName",			TOK_STRUCTPARAM | TOK_STRING(MissionComplaint, globalName, 0), NULL },
	{ "\n",					TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MissionComplaint()
{
	int iSize = sizeof(MissionComplaint);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MissionComplaint, iSize, "MissionComplaint", NULL, "MissionServer.h");
	return 0;
};

//autogeneratednocheckin
//Structparser.exe autogenerated ParseTable for struct MissionAuth
ParseTable parse_MissionAuth[] =
{
	{ "MissionAuth", 	TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MissionAuth), 0, NULL, 0 },
	{ "id",				TOK_STRUCTPARAM | TOK_AUTOINT(MissionAuth, id, 0), NULL },
	{ "region",			TOK_STRUCTPARAM | TOK_AUTOINT(MissionAuth, region, 0), AuthRegionEnum },
	{ "\n",				TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MissionAuth()
{
	int iSize = sizeof(MissionAuth);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MissionAuth, iSize, "MissionAuth", NULL, "MissionServer.h");
	return 0;
};

//autogeneratednocheckin
extern ParseTable parse_MissionAuth[];
extern ParseTable parse_MissionComplaint[];
extern ParseTable parse_MissionSearchHeader[];
//Structparser.exe autogenerated ParseTable for struct MissionServerArc
ParseTable parse_MissionServerArc[] =
{
	{ "MissionServerArc", 	TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MissionServerArc), 0, NULL, 0 },
	{ "id",					TOK_STRUCTPARAM | TOK_PRIMARY_KEY | TOK_AUTO_INCREMENT | TOK_AUTOINT(MissionServerArc, id, 0), NULL },
	{ "Creator",			TOK_AUTOINT(MissionServerArc, creator_id, 0), NULL },
	{ "CreatorServer",		TOK_AUTOINT(MissionServerArc, creator_region, 0), AuthRegionEnum },
	{ "Created",			TOK_AUTOINT(MissionServerArc, created, 0), NULL  ,  TOK_FORMAT_DATESS2000},
	{ "Updated",			TOK_AUTOINT(MissionServerArc, updated, 0), NULL  ,  TOK_FORMAT_DATESS2000},
	{ "Pulled",				TOK_AUTOINT(MissionServerArc, pulled, 0), NULL  ,  TOK_FORMAT_DATESS2000},
	{ "Unpublished",		TOK_AUTOINT(MissionServerArc, unpublished, 0), NULL  ,  TOK_FORMAT_DATESS2000},
	{ "Invalidated",		TOK_AUTOINT(MissionServerArc, invalidated, 0), NULL  ,  TOK_FORMAT_DATESS2000},
	{ "PlayableErrors",		TOK_AUTOINT(MissionServerArc, playable_errors, 0), NULL  ,  TOK_FORMAT_DATESS2000},
	{ "Uncirculated",		TOK_AUTOINT(MissionServerArc, uncirculated, 0), NULL  ,  TOK_FORMAT_DATESS2000},
	{ "DataSize",			TOK_AUTOINT(MissionServerArc, size, 0), NULL },
	{ "DataZippedSize",		TOK_AUTOINT(MissionServerArc, zsize, 0), NULL },
	{ "Votes",				TOK_FIXED_ARRAY | TOK_AUTOINTARRAY(MissionServerArc, votes,  MISSIONRATING_VOTETYPES), NULL },
	{ "FirstToComplete",	TOK_STRUCT(MissionServerArc, first_to_complete, parse_MissionAuth) },
	{ "TotalCompletions",	TOK_AUTOINT(MissionServerArc, total_completions, 0), NULL },
	{ "TotalStarts",		TOK_AUTOINT(MissionServerArc, total_starts, 0), NULL },
	{ "Rating",				TOK_AUTOINT(MissionServerArc, rating, MISSIONRATING_UNRATED), MissionRatingEnum },
	{ "Honors",				TOK_AUTOINT(MissionServerArc, honors, 0), MissionHonorsEnum },
	{ "Permanent",			TOK_BOOLFLAG(MissionServerArc, permanent, 0), NULL },
	{ "BanStatus",			TOK_AUTOINT(MissionServerArc, ban, 0), MissionBanEnum },
	{ "Complaint",			TOK_STRUCT(MissionServerArc, complaints, parse_MissionComplaint) },
	{ "BanComment",			TOK_REDUNDANTNAME | TOK_STRUCT(MissionServerArc, complaints, parse_legacy_MissionComplaint) },
	{ "NewComplaints",		TOK_AUTOINT(MissionServerArc, new_complaints, 0), NULL },
	{ "MetaData",			TOK_EMBEDDEDSTRUCT(MissionServerArc, header, parse_MissionSearchHeader)},
	{ "KeyWordVotes",		TOK_FIXED_ARRAY | TOK_AUTOINTARRAY(MissionServerArc, keyword_votes,  MISSIONSERVER_MAX_KEYWORDS), NULL },
	{ "InitialKeywords",	TOK_AUTOINT(MissionServerArc, initial_keywords, 0), NULL },
	{ "TotalStars",			TOK_IGNORE, 0 },
	{ "totalVotes",			TOK_IGNORE, 0 },
	{ "TopKeywords",		TOK_IGNORE, 0 },
	{ "End",				TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MissionServerArc()
{
	int iSize = sizeof(MissionServerArc);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MissionServerArc, iSize, "MissionServerArc", NULL, "MissionServer.h");
	return 0;
};

//autogeneratednocheckin
//Structparser.exe autogenerated ParseTable for struct MissionServerShard
ParseTable parse_MissionServerShard[] =
{
	{ "MissionServerShard", 	TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MissionServerShard), 0, NULL, 0 },
	{ "name",				TOK_STRUCTPARAM | TOK_STRING(MissionServerShard, name, 0), NULL },
	{ "ip",					TOK_STRUCTPARAM | TOK_STRING(MissionServerShard, ip, 0), NULL },
	{ "auth_region",		TOK_STRUCTPARAM | TOK_AUTOINT(MissionServerShard, auth_region, 0), AuthRegionEnum },
	{ "\n",					TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MissionServerShard()
{
	int iSize = sizeof(MissionServerShard);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MissionServerShard, iSize, "MissionServerShard", NULL, "MissionServer.h");
	return 0;
};

//autogeneratednocheckin
extern ParseTable parse_MissionServerShard[];
extern ParseTable parse_PersistConfig[];
extern ParseTable parse_PersistConfig[];
extern ParseTable parse_PersistConfig[];
extern ParseTable parse_PersistConfig[];
extern ParseTable parse_PersistConfig[];
extern ParseTable parse_PersistConfig[];
extern ParseTable parse_PersistConfig[];
//Structparser.exe autogenerated ParseTable for struct MissionServerCfg
ParseTable parse_MissionServerCfg[] =
{
	{ "MissionServerCfg", 	TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MissionServerCfg), 0, NULL, 0 },
	{ "Shard",				TOK_STRUCT(MissionServerCfg, shards, parse_MissionServerShard) },
	{ "ArcDb",				TOK_OPTIONALSTRUCT(MissionServerCfg, arcpersist, parse_PersistConfig), },
	{ "UserDb",				TOK_OPTIONALSTRUCT(MissionServerCfg, userpersist, parse_PersistConfig), },
	{ "SearchDb",			TOK_OPTIONALSTRUCT(MissionServerCfg, searchpersist, parse_PersistConfig), },
	{ "ComposedDb",			TOK_OPTIONALSTRUCT(MissionServerCfg, composedpersist, parse_PersistConfig), },
	{ "SubstringDb",		TOK_OPTIONALSTRUCT(MissionServerCfg, substringpersist, parse_PersistConfig), },
	{ "QuotedDb",			TOK_OPTIONALSTRUCT(MissionServerCfg, quotedpersist, parse_PersistConfig), },
	{ "VarsDb",				TOK_OPTIONALSTRUCT(MissionServerCfg, varspersist, parse_PersistConfig), },
	{ "ReserveMemory",		TOK_AUTOINT(MissionServerCfg, memory_reserved, ((sizeof(void*)==8?2<<30:0))), NULL },
	{ "MaxComposedCaches",	TOK_AUTOINT(MissionServerCfg, maxComposedCaches, 333), NULL },
	{ "", 0, 0 }
};

int autoStruct_fixup_MissionServerCfg()
{
	int iSize = sizeof(MissionServerCfg);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MissionServerCfg, iSize, "MissionServerCfg", NULL, "MissionServer.h");
	return 0;
};

//autogeneratednocheckin
//Structparser.exe autogenerated ParseTable for struct MissionServerVars
ParseTable parse_MissionServerVars[] =
{
	{ "MissionServerVars", 		TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MissionServerVars), 0, NULL, 0 },
	{ "rating_threshold",		TOK_AUTOINT(MissionServerVars, rating_threshold, 1), NULL },
	{ "instaban_grace_min",		TOK_AUTOINT(MissionServerVars, instaban_grace_min, 5), NULL },
	{ "instaban_threshold",		TOK_REDUNDANTNAME | TOK_AUTOINT(MissionServerVars, instaban_grace_min, 5), NULL },
	{ "instaban_grace_speed",	TOK_F32(MissionServerVars, instaban_grace_speed, 5), NULL },
	{ "instaban_grace_base",	TOK_F32(MissionServerVars, instaban_grace_base, 10), NULL },
	{ "autobans_till_userban",	TOK_AUTOINT(MissionServerVars, autobans_till_userban, 5), NULL },
	{ "halloffame_votes",		TOK_AUTOINT(MissionServerVars, halloffame_votes, 1000), NULL },
	{ "halloffame_threshold",	TOK_REDUNDANTNAME | TOK_AUTOINT(MissionServerVars, halloffame_votes, 1000), NULL },
	{ "halloffame_rating",		TOK_F32(MissionServerVars, halloffame_rating, 0), NULL },
	{ "days_till_unpublish",	TOK_AUTOINT(MissionServerVars, days_till_unpublish, 60), NULL },
	{ "days_till_delete",		TOK_AUTOINT(MissionServerVars, days_till_delete, 90), NULL },
	{ "days_till_revote",		TOK_AUTOINT(MissionServerVars, days_till_revote, 1), NULL },
	{ "ticket_arcs_per_auth",	TOK_AUTOINT(MissionServerVars, ticket_arcs_per_auth, 2), NULL },
	{ "tickets_0_stars",		TOK_AUTOINT(MissionServerVars, tickets_0_stars, 0), NULL },
	{ "tickets_1_star",			TOK_AUTOINT(MissionServerVars, tickets_1_star, 0), NULL },
	{ "tickets_1_stars",		TOK_REDUNDANTNAME | TOK_AUTOINT(MissionServerVars, tickets_1_star, 0), NULL },
	{ "tickets_2_stars",		TOK_AUTOINT(MissionServerVars, tickets_2_stars, 0), NULL },
	{ "tickets_3_stars",		TOK_AUTOINT(MissionServerVars, tickets_3_stars, 3), NULL },
	{ "tickets_4_stars",		TOK_AUTOINT(MissionServerVars, tickets_4_stars, 4), NULL },
	{ "tickets_5_stars",		TOK_AUTOINT(MissionServerVars, tickets_5_stars, 5), NULL },
	{ "End",					TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MissionServerVars()
{
	int iSize = sizeof(MissionServerVars);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MissionServerVars, iSize, "MissionServerVars", NULL, "MissionServer.h");
	return 0;
};

