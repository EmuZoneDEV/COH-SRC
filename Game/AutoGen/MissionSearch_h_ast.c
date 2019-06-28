#include "textparser.h"

//#ifed-out include to fool incredibuild dependencies
#if 0
#include "c:\game\code\CoH\Game\..\Common\MissionSearch.h"
#endif



//auto-generated staticdefine for enum AuthRegion
//autogeneratednocheckin
StaticDefineInt AuthRegionEnum[] =
{
	DEFINE_INT
	{ "DEV", AUTHREGION_DEV},
	{ "WW", AUTHREGION_WW},
	{ "COUNT", AUTHREGION_COUNT},
	DEFINE_END
};



//auto-generated staticdefine for enum MissionRating
//autogeneratednocheckin
StaticDefineInt MissionRatingEnum[] =
{
	DEFINE_INT
	{ "0_STARS", MISSIONRATING_0_STARS},
	{ "1_STAR", MISSIONRATING_1_STAR},
	{ "1_STARS", MISSIONRATING_1_STARS},
	{ "2_STARS", MISSIONRATING_2_STARS},
	{ "3_STARS", MISSIONRATING_3_STARS},
	{ "4_STARS", MISSIONRATING_4_STARS},
	{ "5_STARS", MISSIONRATING_5_STARS},
	{ "MAX_STARS_PROMOTED", MISSIONRATING_MAX_STARS_PROMOTED},
	{ "UNRATED", MISSIONRATING_UNRATED},
	{ "RATINGTYPES", MISSIONRATING_RATINGTYPES},
	{ "VOTETYPES", MISSIONRATING_VOTETYPES},
	{ "MAX_STARS", MISSIONRATING_MAX_STARS},
	DEFINE_END
};



//auto-generated staticdefine for enum MissionHonors
//autogeneratednocheckin
StaticDefineInt MissionHonorsEnum[] =
{
	DEFINE_INT
	{ "NONE", MISSIONHONORS_NONE},
	{ "FORMER_DC", MISSIONHONORS_FORMER_DC},
	{ "FORMER_DC_HOF", MISSIONHONORS_FORMER_DC_HOF},
	{ "HALLOFFAME", MISSIONHONORS_HALLOFFAME},
	{ "DEVCHOICE", MISSIONHONORS_DEVCHOICE},
	{ "DC_AND_HOF", MISSIONHONORS_DC_AND_HOF},
	{ "CELEBRITY", MISSIONHONORS_CELEBRITY},
	{ "TYPES", MISSIONHONORS_TYPES},
	DEFINE_END
};



//auto-generated staticdefine for enum MissionBan
//autogeneratednocheckin
StaticDefineInt MissionBanEnum[] =
{
	DEFINE_INT
	{ "NONE", MISSIONBAN_NONE},
	{ "PULLED", MISSIONBAN_PULLED},
	{ "SELFREVIEWED", MISSIONBAN_SELFREVIEWED},
	{ "SELFREVIEWED_PULLED", MISSIONBAN_SELFREVIEWED_PULLED},
	{ "SELFREVIEWED_SUBMITTED", MISSIONBAN_SELFREVIEWED_SUBMITTED},
	{ "CSREVIEWED_BANNED", MISSIONBAN_CSREVIEWED_BANNED},
	{ "CSREVIEWED_APPROVED", MISSIONBAN_CSREVIEWED_APPROVED},
	{ "CSREVIEWED_OK", MISSIONBAN_CSREVIEWED_APPROVED},
	{ "TYPES", MISSIONBAN_TYPES},
	DEFINE_END
};

//autogeneratednocheckin
//Structparser.exe autogenerated ParseTable for struct MissionSearchParams
ParseTable parse_MissionSearchParams[] =
{
	{ "MissionSearchParams", 	TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MissionSearchParams), 0, NULL, 0 },
	{ "{",						TOK_START, 0 },
	{ "text",					TOK_STRING(MissionSearchParams, text, 0), NULL },
	{ "author",					TOK_STRING(MissionSearchParams, author, 0), NULL },
	{ "author_id",				TOK_AUTOINT(MissionSearchParams, author_id, 0), NULL },
	{ "villainGroups",			TOK_RAW(MissionSearchParams, villainGroups), NULL },
	{ "arcid",					TOK_AUTOINT(MissionSearchParams, arcid, 0), NULL },
	{ "sort",					TOK_AUTOINT(MissionSearchParams, sort, 0), NULL },
	{ "rating",					TOK_AUTOINT(MissionSearchParams, rating, 0), NULL },
	{ "honors",					TOK_AUTOINT(MissionSearchParams, honors, 0), NULL },
	{ "ban",					TOK_AUTOINT(MissionSearchParams, ban, 0), NULL },
	{ "morality",				TOK_AUTOINT(MissionSearchParams, morality, 0), NULL },
	{ "arcLength",				TOK_AUTOINT(MissionSearchParams, arcLength, 0), NULL },
	{ "missionTypes",			TOK_AUTOINT(MissionSearchParams, missionTypes, 0), NULL },
	{ "locale_id",				TOK_AUTOINT(MissionSearchParams, locale_id, 0), NULL },
	{ "status",					TOK_AUTOINT(MissionSearchParams, status, 0), NULL },
	{ "keywords",				TOK_AUTOINT(MissionSearchParams, keywords, 0), NULL },
	{ "}",						TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MissionSearchParams()
{
	int iSize = sizeof(MissionSearchParams);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MissionSearchParams, iSize, "MissionSearchParams", NULL, "MissionSearch.h");
	return 0;
};

//autogeneratednocheckin
//Structparser.exe autogenerated ParseTable for struct MissionHeaderDescription
ParseTable parse_MissionHeaderDescription[] =
{
	{ "MissionHeaderDescription", 	TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MissionHeaderDescription), 0, NULL, 0 },
	{ "{",							TOK_START, 0 },
	{ "detailTypes",				TOK_AUTOINT(MissionHeaderDescription, detailTypes, 0), NULL },
	{ "min",						TOK_AUTOINT(MissionHeaderDescription, min, 0), NULL },
	{ "max",						TOK_AUTOINT(MissionHeaderDescription, max, 0), NULL },
	{ "size",						TOK_AUTOINT(MissionHeaderDescription, size, 0), NULL },
	{ "}",							TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MissionHeaderDescription()
{
	int iSize = sizeof(MissionHeaderDescription);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MissionHeaderDescription, iSize, "MissionHeaderDescription", NULL, "MissionSearch.h");
	return 0;
};

//autogeneratednocheckin
extern ParseTable parse_MissionHeaderDescription[];
//Structparser.exe autogenerated ParseTable for struct MissionSearchHeader
ParseTable parse_MissionSearchHeader[] =
{
	{ "MissionSearchHeader", 	TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MissionSearchHeader), 0, NULL, 0 },
	{ "author",					TOK_FIXEDSTR(MissionSearchHeader, author), NULL },
	{ "author_shard",			TOK_FIXEDSTR(MissionSearchHeader, author_shard), NULL },
	{ "title",					TOK_FIXEDSTR(MissionSearchHeader, title), NULL },
	{ "summary",				TOK_FIXEDSTR(MissionSearchHeader, summary), NULL },
	{ "guestAuthorBio",			TOK_STRING(MissionSearchHeader, guestAuthorBio, 0), NULL },
	{ "morality",				TOK_AUTOINT(MissionSearchHeader, morality, 0), NULL },
	{ "arcLength",				TOK_AUTOINT(MissionSearchHeader, arcLength, 0), NULL },
	{ "date_created",			TOK_AUTOINT(MissionSearchHeader, date_created, 0), NULL },
	{ "missionType",			TOK_AUTOINT(MissionSearchHeader, missionType, 0), NULL },
	{ "villaingroups",			TOK_FIXED_ARRAY | TOK_AUTOINTARRAY(MissionSearchHeader, villaingroups,  MISSIONSEARCH_VG_BITFIELDSIZE), NULL },
	{ "locale_id",				TOK_AUTOINT(MissionSearchHeader, locale_id, 0), NULL },
	{ "missionCount",			TOK_AUTOINT(MissionSearchHeader, missionCount, 0), NULL },
	{ "hazards",				TOK_AUTOINT(MissionSearchHeader, hazards, 0), NULL },
	{ "arcStatus",				TOK_AUTOINT(MissionSearchHeader, arcStatus, 0), NULL },
	{ "missionDesc",			TOK_STRUCT(MissionSearchHeader, missionDesc, parse_MissionHeaderDescription) },
	{ "minlevel",				TOK_IGNORE, 0 },
	{ "maxlevel",				TOK_IGNORE, 0 },
	{ "levelRangeLow",			TOK_IGNORE, 0 },
	{ "levelRangeHigh",			TOK_IGNORE, 0 },
	{ "End",					TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MissionSearchHeader()
{
	int iSize = sizeof(MissionSearchHeader);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MissionSearchHeader, iSize, "MissionSearchHeader", NULL, "MissionSearch.h");
	return 0;
};

//autogeneratednocheckin
//Structparser.exe autogenerated ParseTable for struct MissionSearchGuestBio
ParseTable parse_MissionSearchGuestBio[] =
{
	{ "MissionSearchGuestBio", 	TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MissionSearchGuestBio), 0, NULL, 0 },
	{ "imageName",				TOK_STRING(MissionSearchGuestBio, imageName, 0), NULL },
	{ "bio",					TOK_STRING(MissionSearchGuestBio, bio, 0), NULL },
	{ "End",					TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MissionSearchGuestBio()
{
	int iSize = sizeof(MissionSearchGuestBio);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MissionSearchGuestBio, iSize, "MissionSearchGuestBio", NULL, "MissionSearch.h");
	return 0;
};

