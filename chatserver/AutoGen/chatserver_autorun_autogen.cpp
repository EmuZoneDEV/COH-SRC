//This file contains data and prototypes for autoruns. It is autogenerated by StructParser


// autogeneratednocheckin

extern "C"
{
extern void Add_Auto_Cmds_chatserver(void);
extern void _chatserver_AutoRun_SPECIALINTERNAL(void);
void doAutoRuns_UtilitiesLib_0(void);
void doAutoRuns_UtilitiesLib_1(void);
void doAutoRuns_UtilitiesLib_2(void);
void doAutoRuns_UtilitiesLib_3(void);
void doAutoRuns_UtilitiesLib_4(void);


void doAutoRuns_chatserver_0(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
	doAutoRuns_UtilitiesLib_0();
	_chatserver_AutoRun_SPECIALINTERNAL();
}



void doAutoRuns_chatserver_1(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
	doAutoRuns_UtilitiesLib_1();
	Add_Auto_Cmds_chatserver();
}



void doAutoRuns_chatserver_2(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
	doAutoRuns_UtilitiesLib_2();
}



void doAutoRuns_chatserver_3(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
	doAutoRuns_UtilitiesLib_3();
}



void doAutoRuns_chatserver_4(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
	doAutoRuns_UtilitiesLib_4();
}

extern void utilitiesLibPreAutoRunStuff(void);
int MagicAutoRunFunc_chatserver(void)
{
	utilitiesLibPreAutoRunStuff();
	doAutoRuns_chatserver_0();
	doAutoRuns_chatserver_1();
	doAutoRuns_chatserver_2();
	doAutoRuns_chatserver_3();
	doAutoRuns_chatserver_4();
	return 0;
}

};


#if 0
PARSABLE
2
"Add_Auto_Cmds_chatserver" "autogen_magiccommands" 1 "" "" 
"_chatserver_AutoRun_SPECIALINTERNAL" "_SPECIAL_INTERNAL" 0 "" "" 
#endif
