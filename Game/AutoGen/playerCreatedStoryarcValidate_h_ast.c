#include "textparser.h"

//#ifed-out include to fool incredibuild dependencies
#if 0
#include "c:\game\code\CoH\Game\..\Common\storyarc\playerCreatedStoryarcValidate.h"
#endif



//auto-generated staticdefine for enum SpecialAction
//autogeneratednocheckin
StaticDefineInt SpecialActionEnum[] =
{
	DEFINE_INT
	{ "None", kAction_None},
	{ "EnumMorality", kAction_EnumMorality},
	{ "EnumAlignment", kAction_EnumAlignment},
	{ "EnumDifficulty", kAction_EnumDifficulty},
	{ "EnumDifficultyWithSingle", kAction_EnumDifficultyWithSingle},
	{ "EnumPacing", kAction_EnumPacing},
	{ "EnumPlacement", kAction_EnumPlacement},
	{ "EnumDetail", kAction_EnumDetail},
	{ "EnumPersonCombat", kAction_EnumPersonCombat},
	{ "EnumPersonBehavior", kAction_EnumPersonBehavior},
	{ "EnumMapLength", kAction_EnumMapLength},
	{ "EnumContactType", kAction_EnumContactType},
	{ "EnumRumbleType", kAction_EnumRumbleType},
	{ "EnumArcStatus", kAction_EnumArcStatus},
	{ "BuildVillainGroupList", kAction_BuildVillainGroupList},
	{ "BuildEntityList", kAction_BuildEntityList},
	{ "BuildSupportEntityList", kAction_BuildSupportEntityList},
	{ "BuildBossEntityList", kAction_BuildBossEntityList},
	{ "BuildObjectEntityList", kAction_BuildObjectEntityList},
	{ "BuildAnimList", kAction_BuildAnimList},
	{ "BuildModelList", kAction_BuildModelList},
	{ "BuildMapList", kAction_BuildMapList},
	{ "BuildContactList", kAction_BuildContactList},
	{ "BuildModelListContact", kAction_BuildModelListContact},
	{ "BuildEntityListContact", kAction_BuildEntityListContact},
	{ "BuildObjectEntityListContact", kAction_BuildObjectEntityListContact},
	{ "BuildLevelList", kAction_BuildLevelList},
	{ "BuildCustomVillainGroupList", kAction_BuildCustomVillainGroupList},
	{ "BuildCustomCritterList", kAction_BuildCustomCritterList},
	{ "BuildCustomCritterAndContactList", kAction_BuildCustomCritterAndContactList},
	{ "BuildAmbushTrigger", kAction_BuildAmbushTrigger},
	{ "BuildDestinationList", kAction_BuildDestinationList},
	{ "BuildGiantMonsterEntityList", kAction_BuildGiantMonsterEntityList},
	{ "BuildDoppelEntityList", kAction_BuildDoppelEntityList},
	DEFINE_END
};



//auto-generated staticdefine for enum PlayerCreatedDetailType
//autogeneratednocheckin
StaticDefineInt PlayerCreatedDetailTypeEnum[] =
{
	DEFINE_INT
	{ "Ambush", kDetail_Ambush},
	{ "Boss", kDetail_Boss},
	{ "Collection", kDetail_Collection},
	{ "DestructObject", kDetail_DestructObject},
	{ "DefendObject", kDetail_DefendObject},
	{ "Patrol", kDetail_Patrol},
	{ "Rescue", kDetail_Rescue},
	{ "Escort", kDetail_Escort},
	{ "Ally", kDetail_Ally},
	{ "Rumble", kDetail_Rumble},
	{ "DefeatAll", kDetail_DefeatAll},
	{ "GiantMonster", kDetail_GiantMonster},
	{ "Count", kDetail_Count},
	DEFINE_END
};

//autogeneratednocheckin
//Structparser.exe autogenerated ParseTable for struct MapLimit
ParseTable parse_MapLimit[] =
{
	{ "MapLimit", 			TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MapLimit), 0, NULL, 0 },
	{ "{",					TOK_START, 0 },
	{ "regular_only",		TOK_FIXED_ARRAY | TOK_AUTOINTARRAY(MapLimit, regular_only,  4), NULL },
	{ "around_only",		TOK_FIXED_ARRAY | TOK_AUTOINTARRAY(MapLimit, around_only,  4), NULL },
	{ "around_or_regular",	TOK_FIXED_ARRAY | TOK_AUTOINTARRAY(MapLimit, around_or_regular,  4), NULL },
	{ "giant_monster",		TOK_AUTOINT(MapLimit, giant_monster, 0), NULL },
	{ "wall",				TOK_FIXED_ARRAY | TOK_AUTOINTARRAY(MapLimit, wall,  4), NULL },
	{ "floor",				TOK_FIXED_ARRAY | TOK_AUTOINTARRAY(MapLimit, floor,  4), NULL },
	{ "defeat_all",			TOK_AUTOINT(MapLimit, defeat_all, 0), NULL },
	{ "}",					TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MapLimit()
{
	int iSize = sizeof(MapLimit);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MapLimit, iSize, "MapLimit", NULL, "playerCreatedStoryarcValidate.h");
	return 0;
};

//autogeneratednocheckin
extern ParseTable parse_MMElementList[];
extern ParseTable parse_MapLimit[];
extern ParseTable parse_PCC_Critter[];
//Structparser.exe autogenerated ParseTable for struct MMElement
ParseTable parse_MMElement[] =
{
	{ "MMElement", 			TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MMElement), 0, NULL, 0 },
	{ "{",					TOK_START, 0 },
	{ "DisplayName",		TOK_STRING(MMElement, pchDisplayName, 0), NULL },
	{ "Text",				TOK_STRING(MMElement, pchText, 0), NULL },
	{ "ToolTip",			TOK_STRING(MMElement, pchToolTip, 0), NULL },
	{ "Active",				TOK_AUTOINT(MMElement, bActive, 0), NULL },
	{ "minLevel",			TOK_AUTOINT(MMElement, minLevel, 0), NULL },
	{ "maxLevel",			TOK_AUTOINT(MMElement, maxLevel, 0), NULL },
	{ "ElementList",		TOK_STRUCT(MMElement, ppElementList, parse_MMElementList) },
	{ "Description",		TOK_STRING(MMElement, pchDescription, 0), NULL },
	{ "VGDescription",		TOK_STRING(MMElement, pchVGDescription, 0), NULL },
	{ "PowerDescription",	TOK_STRING(MMElement, pchPowerDescription, 0), NULL },
	{ "Alias",				TOK_STRING(MMElement, pchAlias, 0), NULL },
	{ "Seq",				TOK_STRING(MMElement, pchSeq, 0), NULL },
	{ "npcIndex",			TOK_AUTOINT(MMElement, npcIndex, 0), NULL },
	{ "npcIndexList",		TOK_INTARRAY(MMElement, npcIndexList),  NULL},
	{ "animationUpdate",	TOK_AUTOINT(MMElement, animationUpdate, 0), NULL },
	{ "setnum",				TOK_AUTOINT(MMElement, setnum, 0), NULL },
	{ "Colorable",			TOK_AUTOINT(MMElement, bColorable, 0), NULL },
	{ "mapImageName",		TOK_STRING(MMElement, mapImageName, 0), NULL },
	{ "maplimit",			TOK_EMBEDDEDSTRUCT(MMElement, maplimit, parse_MapLimit)},
	{ "Critter",			TOK_OPTIONALSTRUCT(MMElement, pCritter, parse_PCC_Critter), },
	{ "doppelFlags",		TOK_AUTOINT(MMElement, doppelFlags, 0), NULL },
	{ "}",					TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MMElement()
{
	int iSize = sizeof(MMElement);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MMElement, iSize, "MMElement", NULL, "playerCreatedStoryarcValidate.h");
	return 0;
};

//autogeneratednocheckin
extern ParseTable parse_MMElement[];
extern ParseTable parse_MMElementList[];
extern ParseTable parse_PCC_Critter[];
//Structparser.exe autogenerated ParseTable for struct MMElementList
ParseTable parse_MMElementList[] =
{
	{ "MMElementList", 					TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MMElementList), 0, NULL, 0 },
	{ "{",								TOK_START, 0 },
	{ "Name",							TOK_STRING(MMElementList, pchName, 0), NULL },
	{ "Template",						TOK_STRING(MMElementList, pchTemplate, 0), NULL },
	{ "SpecialAction",					TOK_AUTOINT(MMElementList, eSpecialAction, 0), SpecialActionEnum },
	{ "Element",						TOK_STRUCT(MMElementList, ppElement, parse_MMElement) },
	{ "current_element",				TOK_AUTOINT(MMElementList, current_element, 0), NULL },
	{ "updated",						TOK_AUTOINT(MMElementList, updated, 1), NULL },
	{ "ToolTip",						TOK_STRING(MMElementList, pchToolTip, 0), NULL },
	{ "Default",						TOK_STRING(MMElementList, pchDefault, 0), NULL },
	{ "Field",							TOK_STRING(MMElementList, pchField, 0), NULL },
	{ "AmbushLoad",						TOK_STRING(MMElementList, pchAmbushLoad, 0), NULL },
	{ "DestinationLoad",				TOK_STRING(MMElementList, pchDestinationLoad, 0), NULL },
	{ "ReplaceNoneWith",				TOK_STRING(MMElementList, pchReplaceNoneWith, 0), NULL },
	{ "CheckBoxList",					TOK_AUTOINT(MMElementList, bCheckBoxList, 0), NULL },
	{ "Button",							TOK_AUTOINT(MMElementList, bButton, 0), NULL },
	{ "CopyrightCheck",					TOK_AUTOINT(MMElementList, bCopyrightCheck, 0), NULL },
	{ "TabbedSelection",				TOK_AUTOINT(MMElementList, bTabbedSelection, 0), NULL },
	{ "KeywordSelector",				TOK_AUTOINT(MMElementList, bKeywordSelector, 0), NULL },
	{ "Required",						TOK_AUTOINT(MMElementList, bRequired, 0), NULL },
	{ "FieldRestrictionAlreadyDefined",	TOK_AUTOINT(MMElementList, bFieldRestrictionAlreadyDefined, 0), NULL },
	{ "DontUnlockCheck",				TOK_AUTOINT(MMElementList, bDontUnlockCheck, 0), NULL },
	{ "AllowHTML",						TOK_AUTOINT(MMElementList, AllowHTML, 0), NULL },
	{ "CharLimit",						TOK_AUTOINT(MMElementList, CharLimit, 0), NULL },
	{ "ValMin",							TOK_AUTOINT(MMElementList, ValMin, 0), NULL },
	{ "ValMax",							TOK_AUTOINT(MMElementList, ValMax, 0), NULL },
	{ "MultiList",						TOK_STRUCT(MMElementList, ppMultiList, parse_MMElementList) },
	{ "current_list",					TOK_AUTOINT(MMElementList, current_list, 0), NULL },
	{ "previous_list",					TOK_AUTOINT(MMElementList, previous_list, 0), NULL },
	{ "SelectedCritter",				TOK_OPTIONALSTRUCT(MMElementList, SelectedCritter, parse_PCC_Critter), },
	{ "reasonForInvalid",				TOK_AUTOINT(MMElementList, reasonForInvalid, 0), NULL },
	{ "doneCritterValidation",			TOK_AUTOINT(MMElementList, doneCritterValidation, 0), NULL },
	{ "}",								TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MMElementList()
{
	int iSize = sizeof(MMElementList);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MMElementList, iSize, "MMElementList", NULL, "playerCreatedStoryarcValidate.h");
	return 0;
};

//autogeneratednocheckin
extern ParseTable parse_MMElementList[];
//Structparser.exe autogenerated ParseTable for struct MMRegionButton
ParseTable parse_MMRegionButton[] =
{
	{ "MMRegionButton", 	TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MMRegionButton), 0, NULL, 0 },
	{ "{",				TOK_START, 0 },
	{ "Name",			TOK_STRING(MMRegionButton, pchName, 0), NULL },
	{ "SpecialAction",	TOK_STRING(MMRegionButton, pchSpecialAction, 0), NULL },
	{ "ElementList",	TOK_STRUCT(MMRegionButton, ppElementList, parse_MMElementList) },
	{ "PageName",		TOK_STRING(MMRegionButton, pchPageName, 0), NULL },
	{ "ToolTip",		TOK_STRING(MMRegionButton, pchToolTip, 0), NULL },
	{ "isOpen",			TOK_AUTOINT(MMRegionButton, isOpen, 0), NULL },
	{ "Optional",		TOK_AUTOINT(MMRegionButton, bOptional, 0), NULL },
	{ "FirstUse",		TOK_AUTOINT(MMRegionButton, bFirstUse, 0), NULL },
	{ "wd",				TOK_F32(MMRegionButton, wd, 0), NULL },
	{ "ht",				TOK_F32(MMRegionButton, ht, 0), NULL },
	{ "}",				TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MMRegionButton()
{
	int iSize = sizeof(MMRegionButton);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MMRegionButton, iSize, "MMRegionButton", NULL, "playerCreatedStoryarcValidate.h");
	return 0;
};

//autogeneratednocheckin
extern ParseTable parse_MMElementList[];
extern ParseTable parse_MMRegionButton[];
//Structparser.exe autogenerated ParseTable for struct MMRegion
ParseTable parse_MMRegion[] =
{
	{ "MMRegion", 		TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MMRegion), 0, NULL, 0 },
	{ "{",				TOK_START, 0 },
	{ "Name",			TOK_STRING(MMRegion, pchName, 0), NULL },
	{ "DisplayName",	TOK_STRING(MMRegion, pchDisplayName, 0), NULL },
	{ "UserName",		TOK_STRING(MMRegion, pchUserName, 0), NULL },
	{ "ActualDisplay",	TOK_STRING(MMRegion, pchActualDisplay, 0), NULL },
	{ "ElementList",	TOK_STRUCT(MMRegion, ppElementList, parse_MMElementList) },
	{ "Button",			TOK_STRUCT(MMRegion, ppButton, parse_MMRegionButton) },
	{ "Struct",			TOK_STRING(MMRegion, pchStruct, 0), NULL },
	{ "ToolTip",		TOK_STRING(MMRegion, pchToolTip, 0), NULL },
	{ "Icon",			TOK_STRING(MMRegion, pchIcon, 0), NULL },
	{ "IconGlow",		TOK_STRING(MMRegion, pchIconGlow, 0), NULL },
	{ "outer_ht",		TOK_F32(MMRegion, outer_ht, 0), NULL },
	{ "inner_ht",		TOK_F32(MMRegion, inner_ht, 0), NULL },
	{ "inner_wd",		TOK_F32(MMRegion, inner_wd, 0), NULL },
	{ "isOpen",			TOK_AUTOINT(MMRegion, isOpen, 0), NULL },
	{ "Undeleteable",	TOK_AUTOINT(MMRegion, bUndeleteable, 0), NULL },
	{ "detailType",		TOK_AUTOINT(MMRegion, detailType, 0), PlayerCreatedDetailTypeEnum },
	{ "limit",			TOK_AUTOINT(MMRegion, limit, 0), NULL },
	{ "}",				TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MMRegion()
{
	int iSize = sizeof(MMRegion);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MMRegion, iSize, "MMRegion", NULL, "playerCreatedStoryarcValidate.h");
	return 0;
};

//autogeneratednocheckin
extern ParseTable parse_MMRegion[];
extern ParseTable parse_MMRegionButton[];
//Structparser.exe autogenerated ParseTable for struct MMScrollSet_Loader
ParseTable parse_MMScrollSet_Loader[] =
{
	{ "MMScrollSet_Loader", 	TOK_IGNORE | TOK_PARSETABLE_INFO, sizeof(MMScrollSet_Loader), 0, NULL, 0 },
	{ "{",					TOK_START, 0 },
	{ "Region",				TOK_STRUCT(MMScrollSet_Loader, ppRegion, parse_MMRegion) },
	{ "Button",				TOK_STRUCT(MMScrollSet_Loader, ppButton, parse_MMRegionButton) },
	{ "}",					TOK_END, 0 },
	{ "", 0, 0 }
};

int autoStruct_fixup_MMScrollSet_Loader()
{
	int iSize = sizeof(MMScrollSet_Loader);
	static char once = 0;
	if (once) return 0;
	once = 1;
	ParserSetTableInfoExplicitly(parse_MMScrollSet_Loader, iSize, "MMScrollSet_Loader", NULL, "playerCreatedStoryarcValidate.h");
	return 0;
};

