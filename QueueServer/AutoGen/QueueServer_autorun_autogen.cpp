//This file contains data and prototypes for autoruns. It is autogenerated by StructParser


// autogeneratednocheckin

extern "C"
{
extern void Add_Auto_Cmds_QueueServer(void);
extern void _QueueServer_AutoRun_SPECIALINTERNAL(void);
void doAutoRuns_ServerLib_0(void);
void doAutoRuns_ServerLib_1(void);
void doAutoRuns_ServerLib_2(void);
void doAutoRuns_ServerLib_3(void);
void doAutoRuns_ServerLib_4(void);
void doAutoRuns_UtilitiesLib_0(void);
void doAutoRuns_UtilitiesLib_1(void);
void doAutoRuns_UtilitiesLib_2(void);
void doAutoRuns_UtilitiesLib_3(void);
void doAutoRuns_UtilitiesLib_4(void);


void doAutoRuns_QueueServer_0(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
	doAutoRuns_ServerLib_0();
	doAutoRuns_UtilitiesLib_0();
	_QueueServer_AutoRun_SPECIALINTERNAL();
}



void doAutoRuns_QueueServer_1(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
	doAutoRuns_ServerLib_1();
	doAutoRuns_UtilitiesLib_1();
	Add_Auto_Cmds_QueueServer();
}



void doAutoRuns_QueueServer_2(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
	doAutoRuns_ServerLib_2();
	doAutoRuns_UtilitiesLib_2();
}



void doAutoRuns_QueueServer_3(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
	doAutoRuns_ServerLib_3();
	doAutoRuns_UtilitiesLib_3();
}



void doAutoRuns_QueueServer_4(void)
{
	static int once = 0;
	if (once) return;
	once = 1;
	doAutoRuns_ServerLib_4();
	doAutoRuns_UtilitiesLib_4();
}

extern void utilitiesLibPreAutoRunStuff(void);
int MagicAutoRunFunc_QueueServer(void)
{
	utilitiesLibPreAutoRunStuff();
	doAutoRuns_QueueServer_0();
	doAutoRuns_QueueServer_1();
	doAutoRuns_QueueServer_2();
	doAutoRuns_QueueServer_3();
	doAutoRuns_QueueServer_4();
	return 0;
}

};


#if 0
PARSABLE
2
"Add_Auto_Cmds_QueueServer" "autogen_magiccommands" 1 "" "" 
"_QueueServer_AutoRun_SPECIALINTERNAL" "_SPECIAL_INTERNAL" 0 "" "" 
#endif
