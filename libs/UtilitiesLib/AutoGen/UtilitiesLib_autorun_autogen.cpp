//This file contains data and prototypes for autoruns. It is autogenerated by StructParser


// autogeneratednocheckin

extern "C"
{
extern void Add_Auto_Cmds_UtilitiesLib(void);
extern void _UtilitiesLib_AutoRun_SPECIALINTERNAL(void);


void doAutoRuns_UtilitiesLib_0(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
	_UtilitiesLib_AutoRun_SPECIALINTERNAL();
}



void doAutoRuns_UtilitiesLib_1(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
	Add_Auto_Cmds_UtilitiesLib();
}



void doAutoRuns_UtilitiesLib_2(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
}



void doAutoRuns_UtilitiesLib_3(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
}



void doAutoRuns_UtilitiesLib_4(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
}

extern void utilitiesLibPreAutoRunStuff(void);
int MagicAutoRunFunc_UtilitiesLib(void)
{
	utilitiesLibPreAutoRunStuff();
	doAutoRuns_UtilitiesLib_0();
	doAutoRuns_UtilitiesLib_1();
	doAutoRuns_UtilitiesLib_2();
	doAutoRuns_UtilitiesLib_3();
	doAutoRuns_UtilitiesLib_4();
	return 0;
}

};


#if 0
PARSABLE
2
"Add_Auto_Cmds_UtilitiesLib" "autogen_magiccommands" 1 "" "" 
"_UtilitiesLib_AutoRun_SPECIALINTERNAL" "_SPECIAL_INTERNAL" 0 "" "" 
#endif
