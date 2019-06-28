// Exception
// $Revision: 70 $
// $Date: 7/25/01 5:39p $
// $Author: Miyu $
#include "GlobalAuth.h"
#include "exception.h"
#include <imagehlp.h>

// Disable warnings generated by the Windows header files.
#pragma warning(disable : 4514)
#pragma warning(disable : 4201)

const char *mailServer;
LONG	Terminating;

class CLogCritical 
{
	static CRITICAL_SECTION m_CriticalSection;
public:
	CLogCritical() { EnterCriticalSection(&m_CriticalSection); }
	~CLogCritical() { LeaveCriticalSection(&m_CriticalSection); }
	static void Initialize() { InitializeCriticalSection(&m_CriticalSection); }
	static void Deinitialize() { DeleteCriticalSection(&m_CriticalSection); }
};

CRITICAL_SECTION CLogCritical::m_CriticalSection;

class CExceptionInit
{
public:
	static time_t g_timeStart;
	CExceptionInit();
	~CExceptionInit();
};

time_t CExceptionInit::g_timeStart;
CExceptionInit theExceptionInit;

static char g_szLogPath[MAX_PATH];

static LPTOP_LEVEL_EXCEPTION_FILTER previousFilter;
#define MAIL_FROM   "server@ncsoft.co.kr"
#ifdef _DEBUG
#define RECV_TO "darkangel@ncsoft.co.kr\0"
#elif defined(NPCSVR)
#define RECV_TO "darkangel@ncsoft.co.kr\0" 
#else
#define RECV_TO "darkangel@ncsoft.co.kr\0" 
#endif
// #define USE_LAUNCHER
#define SEND_EMAIL

const int NumCodeBytes = 16;	// Number of code bytes to record.
const int MaxStackDump = 80; // 2048;	// Maximum number of DWORDS in stack dumps.
const int StackColumns = 8;		// Number of columns in stack dump.

#define	ONEK			1024
#define	SIXTYFOURK		(64*ONEK)
#define	ONEM			(ONEK*ONEK)
#define	ONEG			(ONEK*ONEK*ONEK)

// hprintf behaves similarly to printf, with a few vital differences.
// It uses wvsprintf to do the formatting, which is a system routine,
// thus avoiding C run time interactions. For similar reasons it
// uses WriteFile rather than fwrite.
// The one limitation that this imposes is that wvsprintf, and
// therefore hprintf, cannot handle floating point numbers.

static void __cdecl hprintf(HANDLE LogFile, char* Format, ...)
{
		char buffer[2000];	// wvsprintf never prints more than one K.

		va_list arglist;
		va_start( arglist, Format);
		wvsprintf(buffer, Format, arglist);
		va_end( arglist);

		DWORD NumBytes;
		WriteFile(LogFile, buffer, lstrlen(buffer), &NumBytes, 0);
}

// Print the specified FILETIME to output in a human readable format,
// without using the C run time.

static void PrintTime(char *output, FILETIME TimeToPrint)
{
		WORD Date, Time;
		if (FileTimeToLocalFileTime(&TimeToPrint, &TimeToPrint) &&
								FileTimeToDosDateTime(&TimeToPrint, &Date, &Time))
		{
				// What a silly way to print out the file date/time. Oh well,
				// it works, and I'm not aware of a cleaner way to do it.
				wsprintf(output, "%d/%d/%d %02d:%02d:%02d",
										(Date / 32) & 15, Date & 31, (Date / 512) + 1980,
										(Time / 2048), (Time / 32) & 63, (Time & 31) * 2);
		}
		else
				output[0] = 0;
}

// Print information about a code module (DLL or EXE) such as its size,
// location, time stamp, etc.

static void ShowModuleInfo(HANDLE LogFile, HINSTANCE ModuleHandle)
{
	char ModName[MAX_PATH];
	__try
	{
		if (GetModuleFileName(ModuleHandle, ModName, sizeof(ModName)) > 0)
		{
			// If GetModuleFileName returns greater than zero then this must
			// be a valid code module address. Therefore we can try to walk
			// our way through its structures to find the link time stamp.
			IMAGE_DOS_HEADER *DosHeader = (IMAGE_DOS_HEADER*)ModuleHandle;
			if (IMAGE_DOS_SIGNATURE != DosHeader->e_magic)
				return;
			IMAGE_NT_HEADERS *NTHeader = (IMAGE_NT_HEADERS*)((char *)DosHeader
										+ DosHeader->e_lfanew);
			if (IMAGE_NT_SIGNATURE != NTHeader->Signature)
				return;
			// Open the code module file so that we can get its file date
			// and size.
			HANDLE ModuleFile = CreateFile(ModName, GENERIC_READ,
									FILE_SHARE_READ, 0, OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL, 0);
			char TimeBuffer[100] = "";
			DWORD FileSize = 0;
			if (ModuleFile != INVALID_HANDLE_VALUE)
			{
				FileSize = GetFileSize(ModuleFile, 0);
				FILETIME	LastWriteTime;
				if (GetFileTime(ModuleFile, 0, 0, &LastWriteTime))
				{
					wsprintf(TimeBuffer, " - file date is ");
					PrintTime(TimeBuffer + lstrlen(TimeBuffer), LastWriteTime);
				}
				CloseHandle(ModuleFile);
			}
			hprintf(LogFile, "%s, loaded at 0x%08x - %d bytes - %08x%s\r\n",
				ModName, ModuleHandle, FileSize,
				NTHeader->FileHeader.TimeDateStamp, TimeBuffer);
		}
	}
	// Handle any exceptions by continuing from this point.
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

// Scan memory looking for code modules (DLLs or EXEs). VirtualQuery is used
// to find all the blocks of address space that were reserved or committed,
// and ShowModuleInfo will display module information if they are code
// modules.

static void RecordModuleList(HANDLE LogFile)
{
		hprintf(LogFile, "\r\n"
										 "\tModule list: names, addresses, sizes, time stamps "
						"and file times:\r\n");
		SYSTEM_INFO	SystemInfo;
		GetSystemInfo(&SystemInfo);
		const size_t PageSize = SystemInfo.dwPageSize;
		// Set NumPages to the number of pages in the 4GByte address space,
		// while being careful to avoid overflowing ints.
		const size_t NumPages = 4 * size_t(ONEG / PageSize);
		size_t pageNum = 0;
		void *LastAllocationBase = 0;
		while (pageNum < NumPages)
		{
				MEMORY_BASIC_INFORMATION	MemInfo;
				if (VirtualQuery((void *)(pageNum * PageSize), &MemInfo,
										sizeof(MemInfo)))
				{
						if (MemInfo.RegionSize > 0)
						{
								// Adjust the page number to skip over this block of memory.
								pageNum += MemInfo.RegionSize / PageSize;
								if (MemInfo.State == MEM_COMMIT && MemInfo.AllocationBase >
														LastAllocationBase)
								{
										// Look for new blocks of committed memory, and try
										// recording their module names - this will fail
										// gracefully if they aren't code modules.
										LastAllocationBase = MemInfo.AllocationBase;
										ShowModuleInfo(LogFile, (HINSTANCE)LastAllocationBase);
								}
						}
						else
								pageNum += SIXTYFOURK / PageSize;
				}
				else
						pageNum += SIXTYFOURK / PageSize;
				// If VirtualQuery fails we advance by 64K because that is the
				// granularity of address space doled out by VirtualAlloc().
		}
}

// Record information about the user's system, such as processor type, amount
// of memory, etc.

#define REG_DISPLAY "System\\CurrentControlSet\\Services\\Class\\Display\\0000"

BOOL getVideoDesc(char *descStr, int bufferSize)
{
	HKEY hKey;
	LONG result;
	DWORD dataType;
	DWORD dataSize;
	BOOL retValue = FALSE;

	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_DISPLAY,
		0, KEY_QUERY_VALUE, &hKey);
	if (result == ERROR_SUCCESS) {
		dataSize = bufferSize;
		result = RegQueryValueEx(hKey, "DriverDesc", NULL, &dataType, (BYTE *)descStr, &dataSize);
		if (result == ERROR_SUCCESS) {
			retValue = TRUE;
		}
		RegCloseKey(hKey);
	}
	return retValue;
}

static void RecordSystemInformation(HANDLE LogFile)
{
	FILETIME	CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	char TimeBuffer[100];
	PrintTime(TimeBuffer, CurrentTime);
	hprintf(LogFile, "Error occurred at %s.\r\n", TimeBuffer);
	char	ModuleName[MAX_PATH];
	if (GetModuleFileName(0, ModuleName, sizeof(ModuleName)) <= 0)
		lstrcpy(ModuleName, "Unknown");
	char	UserName[200];
	DWORD UserNameSize = sizeof(UserName);
	if (!GetUserName(UserName, &UserNameSize))
		lstrcpy(UserName, "Unknown");
	hprintf(LogFile, "%s, run by %s.\r\n", ModuleName, UserName);

	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);
	hprintf(LogFile, "%d processor(s), type %d.\r\n",
		SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);

	MEMORYSTATUS	MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);
	// Print out the amount of physical memory, rounded up.
	hprintf(LogFile, "%d MBytes physical memory.\r\n", (MemInfo.dwTotalPhys +
		ONEM - 1) / ONEM);
}

// Translate the exception code into something human readable.

static const char *GetExceptionDescription(DWORD ExceptionCode)
{
		struct ExceptionNames
		{
				DWORD	ExceptionCode;
				char*	ExceptionName;
		};

		ExceptionNames ExceptionMap[] =
		{
				{0x40010005, "a Control-C"},
				{0x40010008, "a Control-Break"},
				{0x80000002, "a Datatype Misalignment"},
				{0x80000003, "a Breakpoint"},
				{0xc0000005, "an Access Violation"},
				{0xc0000006, "an In Page Error"},
				{0xc0000017, "a No Memory"},
				{0xc000001d, "an Illegal Instruction"},
				{0xc0000025, "a Noncontinuable Exception"},
				{0xc0000026, "an Invalid Disposition"},
				{0xc000008c, "a Array Bounds Exceeded"},
				{0xc000008d, "a Float Denormal Operand"},
				{0xc000008e, "a Float Divide by Zero"},
				{0xc000008f, "a Float Inexact Result"},
				{0xc0000090, "a Float Invalid Operation"},
				{0xc0000091, "a Float Overflow"},
				{0xc0000092, "a Float Stack Check"},
				{0xc0000093, "a Float Underflow"},
				{0xc0000094, "an Integer Divide by Zero"},
				{0xc0000095, "an Integer Overflow"},
				{0xc0000096, "a Privileged Instruction"},
				{0xc00000fD, "a Stack Overflow"},
				{0xc0000142, "a DLL Initialization Failed"},
				{0xe06d7363, "a Microsoft C++ Exception"},
		};

		for (int i = 0; i < sizeof(ExceptionMap) / sizeof(ExceptionMap[0]); i++)
				if (ExceptionCode == ExceptionMap[i].ExceptionCode)
						return ExceptionMap[i].ExceptionName;

		return "Unknown exception type";
}

static char* GetFilePart(char *source)
{
		char *result = strrchr(source, '\\');
		if (result)
				result++;
		else
				result = source;
		return result;
}

#ifdef SEND_EMAIL
static char heloStr[256] = "HELO main1.ncsoft.co.kr\r\n";
static char mailStr[256] = "MAIL From: <darkangel@ncsoft.co.kr>\r\n";
static char rcptStr[256] = "RCPT To: <darkangel@ncsoft.co.kr>\r\n";
static char subjectStr[256];
static char dataStr[] = "DATA\r\n";
static char finishStr[] = "\r\n.\r\n";
static char quitStr[] = "QUIT\r\n";

static bool sendFile(SOCKET sock, const char *name)
{
	HANDLE hFile = CreateFile(name, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return true;

	char buf[1024];
	for ( ; ; ) {
		DWORD dwRead;
		if (!ReadFile(hFile, buf, sizeof(buf), &dwRead, NULL) || !dwRead)
			break;
		if (send(sock, buf, dwRead, 0) <= 0)
			return false;
	}
	CloseHandle(hFile);
	return true;
}

static BOOL sendMail(const char *host, const char *from, const char *to, const char *body)
{
	time_t currentTime;
	int packetLen;
	char packetBuffer[2048];
	SOCKET sock;

	BOOL smResult = FALSE;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0){
		return FALSE;
	}
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		WSACleanup();
		return FALSE;
	}
	int timeout = 5000;
	int result = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	if (result == SOCKET_ERROR) {
		goto exit;
	}
	timeout = 5000;
	result = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	if (result == SOCKET_ERROR) {
		goto exit;
	}
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY; // bind socket local any where
	addr.sin_port = 0;
	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
		goto exit;
	}
	addr.sin_family = AF_INET;
	if (host[0] >= '0' && host[0] <= '9') {
		addr.sin_addr.s_addr = inet_addr(host);
	} else {
		hostent *h = gethostbyname(host);
		if (h == NULL)
			goto exit;
		memcpy(&addr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
		char *t = inet_ntoa(addr.sin_addr);
	}
	addr.sin_port = htons(25);
	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
		goto exit;
	}
	packetLen = recv(sock, packetBuffer, sizeof(packetBuffer), 0);
	if (send(sock, heloStr, (int)strlen(heloStr), 0) <= 0)
		goto exit;
	packetLen = recv(sock, packetBuffer, sizeof(packetBuffer), 0);
	sprintf(mailStr, "MAIL From: <%s>\r\n", from);
	if (send(sock, mailStr, (int)strlen(mailStr), 0) <= 0)
		goto exit;
	packetLen = recv(sock, packetBuffer, sizeof(packetBuffer), 0);
	for ( ; to[0]; to += strlen(to) + 1) {
		sprintf(rcptStr, "RCPT To: <%s>\r\n", to);
		if (send(sock, rcptStr, (int)strlen(rcptStr), 0) <= 0)
			goto exit;
		packetLen = recv(sock, packetBuffer, sizeof(packetBuffer), 0);
	}
	if (send(sock, dataStr, (int)strlen(dataStr), 0) <= 0)
		goto exit;
	packetLen = recv(sock, packetBuffer, sizeof(packetBuffer), 0);
	currentTime = time(NULL);
	{
		SOCKADDR_IN sockname;
		memset(&sockname, 0, sizeof(sockname));
		int namelen = sizeof(sockname);
		getsockname(sock, (LPSOCKADDR) &sockname, &namelen);
#ifdef SERVER
		sprintf(subjectStr, "Subject: Server [%d](%d.%d.%d.%d) crashed at %s\r\n", serverId,
			sockname.sin_addr.S_un.S_un_b.s_b1, sockname.sin_addr.S_un.S_un_b.s_b2,
			sockname.sin_addr.S_un.S_un_b.s_b3, sockname.sin_addr.S_un.S_un_b.s_b4,
			ctime(&currentTime));
		log.Add(LOG_ERROR, "Server crashed at %s", ctime(&currentTime));
#elif defined(CACHED)
		sprintf(subjectStr, "Subject: CacheD [%d](%d.%d.%d.%d) crashed at %s\r\n", serverId,
			sockname.sin_addr.S_un.S_un_b.s_b1, sockname.sin_addr.S_un.S_un_b.s_b2,
			sockname.sin_addr.S_un.S_un_b.s_b3, sockname.sin_addr.S_un.S_un_b.s_b4,
			ctime(&currentTime));
#elif defined(AUTHD)
		sprintf(subjectStr, "Subject: AuthD [%d](%d.%d.%d.%d) crashed at %s\r\n", serverId,
			sockname.sin_addr.S_un.S_un_b.s_b1, sockname.sin_addr.S_un.S_un_b.s_b2,
			sockname.sin_addr.S_un.S_un_b.s_b3, sockname.sin_addr.S_un.S_un_b.s_b4,
			ctime(&currentTime));
#elif defined(NPCSVR)
		sprintf(subjectStr, "Subject: NPCSvr (%d.%d.%d.%d) crashed at %s\r\n", 
			sockname.sin_addr.S_un.S_un_b.s_b1, sockname.sin_addr.S_un.S_un_b.s_b2,
			sockname.sin_addr.S_un.S_un_b.s_b3, sockname.sin_addr.S_un.S_un_b.s_b4,
			ctime(&currentTime));
#endif
	}
	if (send(sock, subjectStr, (int)strlen(subjectStr), 0) <= 0)
		goto exit;
	if (!sendFile(sock, body)) 
		goto exit;
	if (send(sock, finishStr, (int)strlen(finishStr), 0) <= 0)
		goto exit;
	packetLen = recv(sock, packetBuffer, sizeof(packetBuffer), 0);
	if (send(sock, quitStr, (int)strlen(quitStr), 0) <= 0)
		goto exit;
	packetLen = recv(sock, packetBuffer, sizeof(packetBuffer), 0);
	smResult = TRUE;
exit:
	closesocket(sock);
	sock = 0;
	WSACleanup();
	return smResult;
}
#endif

BOOL GetLogicalAddress(
		PVOID addr, PSTR szModule, DWORD len, DWORD& section, DWORD& offset )
{
	MEMORY_BASIC_INFORMATION mbi;

	if ( !VirtualQuery( addr, &mbi, sizeof(mbi) ) )
		return FALSE;

	UINT_PTR hMod = (UINT_PTR)(mbi.AllocationBase);

	if ( !GetModuleFileName( (HMODULE)hMod, szModule, len ) )
		return FALSE;

	// Point to the DOS header in memory
	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;

	// From the DOS header, find the NT (PE) header
	PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);

	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNtHdr );

	UINT_PTR rva = (UINT_PTR)addr - hMod; // RVA is offset from module load address

	// Iterate through the section table, looking for the one that encompasses
	// the linear address.
	for (   unsigned i = 0;
			i < pNtHdr->FileHeader.NumberOfSections;
			i++, pSection++ )
	{
		DWORD sectionStart = pSection->VirtualAddress;
		DWORD sectionEnd = sectionStart
					+ max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

		// Is the address in this section???
		if ( (rva >= sectionStart) && (rva <= sectionEnd) )
		{
			// Yes, address is in the section.  Calculate section and offset,
			// and store in the "section" & "offset" params, which were
			// passed by reference.
			section = i+1;
			offset = (DWORD)rva - sectionStart;
			return TRUE;
		}
	}

	return FALSE;   // Should never get here!
}

void ImageHelpStackWalk(HANDLE LogFile, PCONTEXT ptrContext)
{
	hprintf(LogFile, "\r\nCall Stack Information\r\n");

	SymSetOptions(SYMOPT_DEFERRED_LOADS);

	STACKFRAME sf;
	memset(&sf, 0, sizeof(sf));

	sf.AddrPC.Offset       = ptrContext->Eip;
	sf.AddrPC.Mode         = AddrModeFlat;
	sf.AddrStack.Offset    = ptrContext->Esp;
	sf.AddrStack.Mode      = AddrModeFlat;
	sf.AddrFrame.Offset    = ptrContext->Ebp;
	sf.AddrFrame.Mode      = AddrModeFlat;

	while ( 1 )
	{
		if (!StackWalk(IMAGE_FILE_MACHINE_I386,
						GetCurrentProcess(),
						GetCurrentThread(),
						&sf,
						ptrContext,
						0,
						SymFunctionTableAccess,
						SymGetModuleBase,
						0))
			break;

		if (0 == sf.AddrFrame.Offset) // Bail if frame is not okay.
			break;

		hprintf(LogFile, "%08x  %08x  ", sf.AddrPC.Offset, sf.AddrFrame.Offset);

		BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + 512];
		PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
		pSymbol->SizeOfStruct = sizeof(symbolBuffer);
		pSymbol->MaxNameLength = 512;

		DWORD symDisplacement = 0;

		if (SymGetSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset,
							&symDisplacement, pSymbol)) {
			hprintf(LogFile, "%s+%x ", pSymbol->Name, symDisplacement);
		} 
		{// No symbol found.  Print out the logical address instead.
			char CrashModulePathName[MAX_PATH];
			char *CrashModuleFileName = "Unknown";
			MEMORY_BASIC_INFORMATION MemInfo;
			// VirtualQuery can be used to get the allocation base associated with a
			// code address, which is the same as the ModuleHandle. This can be used
			// to get the filename of the module that the crash happened in.
			if (VirtualQuery(IntToPtr(sf.AddrPC.Offset), &MemInfo, sizeof(MemInfo)) &&
					GetModuleFileName((HINSTANCE)MemInfo.AllocationBase,
					CrashModulePathName, sizeof(CrashModulePathName)) > 0)
				CrashModuleFileName = GetFilePart(CrashModulePathName);

			CHAR szModule[MAX_PATH] = "";
			DWORD section = 0, offset = 0;
			GetLogicalAddress(IntToPtr(sf.AddrPC.Offset),
								szModule, sizeof(szModule), section, offset);
			hprintf(LogFile, "%04x:%08x %s\r\n", section, offset, szModule);

			hprintf(LogFile, "Params: %08x %08x %08x %08x\r\n", sf.Params[0], sf.Params[1], sf.Params[2], sf.Params[3]);
			hprintf(LogFile, "[%s] Bytes at CS:EIP: ", CrashModuleFileName);
			unsigned char *code = (unsigned char*) IntToPtr(sf.AddrPC.Offset);
			for (int codebyte = 0; codebyte < NumCodeBytes; codebyte++) {
				__try {
					hprintf(LogFile, "%02x ", code[codebyte]);
				}
				__except(EXCEPTION_EXECUTE_HANDLER) {
					hprintf(LogFile, "?? ");
				}
			}
			hprintf(LogFile, "\r\n\r\n");
		}
	}
}

void	IntelStackWalk(HANDLE LogFile, PCONTEXT ptrContext)
{
	hprintf(LogFile, "\r\nIntel Call Stack Information\r\n");

	DWORD pc = ptrContext->Eip;
	PDWORD pFrame, pPrevFrame;

	pFrame = (PDWORD)IntToPtr(ptrContext->Ebp);

	for ( ; ; ) {

		CHAR szModule[MAX_PATH] = "";
		DWORD section = 0, offset = 0;
		GetLogicalAddress((PVOID)IntToPtr(pc),
							szModule, sizeof(szModule), section, offset);
		hprintf(LogFile, "%08x  %08x  %04x:%08x %s\r\n", pc, pFrame, section, offset, szModule);
		hprintf(LogFile, "Bytes at CS:EIP: ");
		unsigned char *code = (unsigned char*) IntToPtr(pc);
		for (int codebyte = 0; codebyte < NumCodeBytes; codebyte++) {
			__try {
				hprintf(LogFile, "%02x ", code[codebyte]);
			}
			__except(EXCEPTION_EXECUTE_HANDLER) {
				hprintf(LogFile, "?? ");
			}
		}
		hprintf(LogFile, "\r\n\r\n");


		pc = pFrame[1];

		pPrevFrame = pFrame;

		pFrame = (PDWORD)IntToPtr(pFrame[0]); // precede to next higher frame on stack

		if ( (DWORD)PtrToUint(pFrame) & 3 )    // Frame pointer must be aligned on a
			break;                  // DWORD boundary.  Bail if not so.

		if ( pFrame <= pPrevFrame )
			break;

		// Can two DWORDs be read from the supposed frame address?          
		if ( IsBadWritePtr(pFrame, sizeof(PVOID)*2) )
			break;

	}
}

static void GenerateExceptionReport(PEXCEPTION_POINTERS data)
{
	HANDLE LogFile = CreateFile(g_szLogPath, GENERIC_WRITE, 0, 0,
	OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);
	if (LogFile == INVALID_HANDLE_VALUE) {
		OutputDebugString("Error creating exception report");
		return;
	}
	// Append to the error log.
	SetFilePointer(LogFile, 0, 0, FILE_END);
	// Print out some blank lines to separate this error log from any previous ones.
	SYSTEMTIME st = {0};
	::GetLocalTime(&st);
	hprintf(LogFile,  "[(%d) %04d/%02d/%02d %02d:%02d:%02d]: =======================\r\n", GetCurrentThreadId(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
	PEXCEPTION_RECORD Exception = data->ExceptionRecord;
	PCONTEXT Context = data->ContextRecord;

	char CrashModulePathName[MAX_PATH];
	char *CrashModuleFileName = "Unknown";
	MEMORY_BASIC_INFORMATION MemInfo;
	// VirtualQuery can be used to get the allocation base associated with a
	// code address, which is the same as the ModuleHandle. This can be used
	// to get the filename of the module that the crash happened in.
	if (VirtualQuery((void*)IntToPtr(Context->Eip), &MemInfo, sizeof(MemInfo)) &&
			GetModuleFileName((HINSTANCE)MemInfo.AllocationBase,
			CrashModulePathName, sizeof(CrashModulePathName)) > 0)
		CrashModuleFileName = GetFilePart(CrashModulePathName);

	// Print out the beginning of the error log in a Win95 error window
	// compatible format.
	hprintf(LogFile, "%s in module %s at %04x:%08x.\r\n",
			GetExceptionDescription(Exception->ExceptionCode),
			CrashModuleFileName, Context->SegCs, Context->Eip);
	hprintf(LogFile, "Exception handler called in the GlobalAuth Server.\r\n");

	tm* tm = localtime(&CExceptionInit::g_timeStart);
	hprintf(LogFile, "start at %d/%d/%d %02d:%02d:%02d\r\n", tm->tm_year+1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
	RecordSystemInformation(LogFile);
	// If the exception was an access violation, print out some additional
	// information, to the error log and the debugger.
	if (Exception->ExceptionCode == STATUS_ACCESS_VIOLATION &&
			Exception->NumberParameters >= 2) {
		char DebugMessage[1000];
		const char* readwrite = "Read from";
		if (Exception->ExceptionInformation[0])
			readwrite = "Write to";
		wsprintf(DebugMessage, "%s location %08x caused an access violation.\r\n",
				readwrite, Exception->ExceptionInformation[1]);
#ifdef	_DEBUG
		// The VisualC++ debugger doesn't actually tell you whether a read
		// or a write caused the access violation, nor does it tell what
		// address was being read or written. So I fixed that.
		OutputDebugString("Exception handler: ");
		OutputDebugString(DebugMessage);
#endif
		hprintf(LogFile, "%s", DebugMessage);
	}
	// Print out the register values in a Win95 error window compatible format.
	hprintf(LogFile, "\r\n");
	hprintf(LogFile, "Registers:\r\n");
	hprintf(LogFile, "EAX=%08x CS=%04x EIP=%08x EFLGS=%08x\r\n",
			Context->Eax, Context->SegCs, Context->Eip, Context->EFlags);
	hprintf(LogFile, "EBX=%08x SS=%04x ESP=%08x EBP=%08x\r\n",
			Context->Ebx, Context->SegSs, Context->Esp, Context->Ebp);
	hprintf(LogFile, "ECX=%08x DS=%04x ESI=%08x FS=%04x\r\n",
			Context->Ecx, Context->SegDs, Context->Esi, Context->SegFs);
	hprintf(LogFile, "EDX=%08x ES=%04x EDI=%08x GS=%04x\r\n",
			Context->Edx, Context->SegEs, Context->Edi, Context->SegGs);
	hprintf(LogFile, "Bytes at CS:EIP:\r\n");

	// Print out the bytes of code at the instruction pointer. Since the
	// crash may have been caused by an instruction pointer that was bad,
	// this code needs to be wrapped in an exception handler, in case there
	// is no memory to read. If the dereferencing of code[] fails, the
	// exception handler will print '??'.
	unsigned char *code = (unsigned char*)IntToPtr(Context->Eip);
	for (int codebyte = 0; codebyte < NumCodeBytes; codebyte++) {
		__try {
			hprintf(LogFile, "%02x ", code[codebyte]);
		}
		__except(EXCEPTION_EXECUTE_HANDLER) {
			hprintf(LogFile, "?? ");
		}
	}

	// Time to print part or all of the stack to the error log. This allows
	// us to figure out the call stack, parameters, local variables, etc.
	hprintf(LogFile, "\r\n"
		"Stack dump:\r\n");
	__try {
		// Esp contains the bottom of the stack, or at least the bottom of
		// the currently used area.
		DWORD* pStack = (DWORD *)IntToPtr(Context->Esp);
		DWORD* pStackTop;
		__asm {
			// Load the top (highest address) of the stack from the
			// thread information block. It will be found there in
			// Win9x and Windows NT.
			mov	eax, fs:[4]
			mov pStackTop, eax
		}
		if (pStackTop > pStack + MaxStackDump)
			pStackTop = pStack + MaxStackDump;
		int Count = 0;
		// Too many calls to WriteFile can take a long time, causing
		// confusing delays when programs crash. Therefore I implemented
		// simple buffering for the stack dumping code instead of calling
		// hprintf directly.
		char buffer[1000] = "";
		const int safetyzone = 50;
		char* nearend = buffer + sizeof(buffer) - safetyzone;
		char* output = buffer;
		while (pStack + 1 <= pStackTop) {
			if ((Count % StackColumns) == 0)
				output += wsprintf(output, "%08x: ", pStack);
			char *Suffix = " ";
			if ((++Count % StackColumns) == 0 || pStack + 2 > pStackTop)
				Suffix = "\r\n";
			output += wsprintf(output, "%08x%s", *pStack, Suffix);
			pStack++;
			// Check for when the buffer is almost full, and flush it to disk.
			if (output > nearend) {
				hprintf(LogFile, "%s", buffer);
				buffer[0] = 0;
				output = buffer;
			}
		}
		// Print out any final characters from the cache.
		hprintf(LogFile, "%s", buffer);
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		hprintf(LogFile, "Exception encountered during stack dump.\r\n");
	}

	if (SymInitialize(GetCurrentProcess(), 0, TRUE)) {
		ImageHelpStackWalk(LogFile, Context);
		SymCleanup(GetCurrentProcess());
	}
#ifdef _M_IX86  // Intel Only!
	else {
		// Walk the stack using x86 specific code
		IntelStackWalk(LogFile, Context );
	}
#endif

	RecordModuleList(LogFile);
	CloseHandle(LogFile);
}

LONG WINAPI RecordExceptionInfo(PEXCEPTION_POINTERS data)
{
	static int BeenHere;
	if (BeenHere) {     // Going recursive! That must mean this routine crashed!
		if (previousFilter)
			return previousFilter(data);
		else
			return EXCEPTION_CONTINUE_SEARCH;
	}
	BeenHere = true;

	if (!Terminating && data->ExceptionRecord->ExceptionCode != 0xe0000001) {
		CLogCritical theCritical;
		if (mailServer == NULL)
			DeleteFile(g_szLogPath);
		GenerateExceptionReport(data);
		if (mailServer && sendMail(mailServer, MAIL_FROM, RECV_TO, g_szLogPath)) {
			DeleteFile(g_szLogPath);
		}
	}

	if (previousFilter)
		return previousFilter(data);
	else
		return EXCEPTION_CONTINUE_SEARCH;
}

void SetUpExceptionFilter(void)
{
	previousFilter = SetUnhandledExceptionFilter(RecordExceptionInfo);
}

void ClearExceptionFilter(void)
{
	if (previousFilter)
		SetUnhandledExceptionFilter(previousFilter);
}


CExceptionInit::CExceptionInit()
{
   CLogCritical::Initialize(); 

	char FileName[MAX_PATH] = "Unknown";
	// Create a filename to record the error information to.
	// Storing it in the executable directory works well.
	if (GetModuleFileName(0, g_szLogPath, sizeof(g_szLogPath)) <= 0)
		g_szLogPath[0] = 0;
	char *FilePart = GetFilePart(g_szLogPath);

	// Extract the file name portion and remove it's file extension. We'll
	// use that name shortly.
	lstrcpy(FileName, FilePart);
	char *lastperiod = strrchr(FileName, '.');
	if (lastperiod)
		lastperiod[0] = 0;
	// Replace the executable filename with our error log file name.
	lstrcpy(FilePart, "GlobalAuthError.txt");
	g_timeStart = time(NULL);
	SetUpExceptionFilter();
}

CExceptionInit::~CExceptionInit()
{
	ClearExceptionFilter();
	CLogCritical::Deinitialize();
}


void ELOG(LPCSTR lpszFormat, ...)
{
		if (Terminating)
			return;
		CLogCritical theCritical;
		va_list args;
		va_start(args, lpszFormat);

		char buff[1024];
		SYSTEMTIME st = {0};

		::GetLocalTime(&st);

		HANDLE hFile  = CreateFile(g_szLogPath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
				OPEN_ALWAYS,  FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
				return;

		SetFilePointer(hFile, 0, NULL, FILE_END);

		DWORD dwWritten;
		int n;
		INT_PTR start;
		n = _snprintf(buff, sizeof(buff), "[(%d) %04d/%02d/%02d %02d:%02d:%02d]: ", GetCurrentThreadId(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
		if (n < 0)
				goto cleanup;
		WriteFile(hFile, buff, n, &dwWritten, NULL);
		n = _vsnprintf(buff, sizeof(buff), lpszFormat, args);
#ifdef	_DEBUG
		OutputDebugString(buff);
#endif
		if (n < 0)
				goto cleanup;
		start = 0;
		for ( ; start < n ; ) {
				char *p = (char *)memchr(buff+start, '\n', n - start);
				if (!p)
						break;
				if (n < sizeof(buff))
						n++;
				start = p - buff;
				memmove(p+1, p, n - start-1);
				*p = '\r';
				start += 2;

		}
		WriteFile(hFile, buff, n, &dwWritten, NULL);

cleanup:
		va_end(args);

		CloseHandle(hFile);
}

#ifndef	SEH_EXCEPTION
BOOL CSystemException::GetErrorMessage(LPSTR lpszError, UINT nMaxError, PUINT)
{
		sprintf(lpszError, "System Exception %#x", m_nSE);
		return TRUE;
}
#endif


#ifdef	SEH_EXCEPTION

int exception_filter(LPEXCEPTION_POINTERS pExp)
{
	if (!Terminating && pExp->ExceptionRecord->ExceptionCode != 0xe0000001) {
		CLogCritical theCritical;
		if (mailServer == NULL)
			DeleteFile(g_szLogPath);
		GenerateExceptionReport(pExp);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

#else

void trans_func( unsigned int u, _EXCEPTION_POINTERS* pExp )
{
	if (Terminating)
		return;
	{
		CLogCritical theCritical;
		if (mailServer == NULL)
			DeleteFile(g_szLogPath);
		GenerateExceptionReport(pExp);
	}
	throw new CSystemException( u );
}
#endif

void send_exception(bool bExit)
{
	if (Terminating)
		return;
	{
		CLogCritical theCritical;
		if (mailServer && sendMail(mailServer, MAIL_FROM, RECV_TO, g_szLogPath)) {
			DeleteFile(g_szLogPath);
		}
	}
	if (bExit) {
		Terminating = TRUE;
		ExitProcess(0);
	}
}

void	exception_init()
{
	if (mailServer == NULL)
		return;
	HANDLE hFile = CreateFile(g_szLogPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		sendMail(mailServer, MAIL_FROM, RECV_TO, g_szLogPath);
		DeleteFile(g_szLogPath);
	}
}

#ifdef	IOSOCKET
extern	HANDLE *g_hThread;
extern	unsigned *g_nThreadId;
extern	int g_nThread;

void ImageHelpStackWalk(HANDLE LogFile, HANDLE hThread)
{

	SuspendThread(hThread);

	CONTEXT context;
	memset(&context, 0, sizeof(context));
	context.ContextFlags = CONTEXT_FULL;
	GetThreadContext(hThread, &context);

	// Time to print part or all of the stack to the error log. This allows
	// us to figure out the call stack, parameters, local variables, etc.
	hprintf(LogFile, "\r\nRegisters:\r\n");
	hprintf(LogFile, "EAX=%08x CS=%04x EIP=%08x EFLGS=%08x\r\n",
			context.Eax, context.SegCs, context.Eip, context.EFlags);
	hprintf(LogFile, "EBX=%08x SS=%04x ESP=%08x EBP=%08x\r\n",
			context.Ebx, context.SegSs, context.Esp, context.Ebp);
	hprintf(LogFile, "ECX=%08x DS=%04x ESI=%08x FS=%04x\r\n",
			context.Ecx, context.SegDs, context.Esi, context.SegFs);
	hprintf(LogFile, "EDX=%08x ES=%04x EDI=%08x GS=%04x\r\n",
			context.Edx, context.SegEs, context.Edi, context.SegGs);
	hprintf(LogFile, "\r\n"
		"Stack dump:\r\n");
	__try {
		// Esp contains the bottom of the stack, or at least the bottom of
		// the currently used area.
		DWORD* pStack = (DWORD *)context.Esp;
		DWORD* pStackTop;
/*	__asm {
			// Load the top (highest address) of the stack from the
			// thread information block. It will be found there in
			// Win9x and Windows NT.
			mov	eax, fs:[4]
			mov pStackTop, eax
		}
		if (pStackTop > pStack + MaxStackDump)
			pStackTop = pStack + MaxStackDump;
*/
		pStackTop = pStack + MaxStackDump;
		int Count = 0;
		// Too many calls to WriteFile can take a long time, causing
		// confusing delays when programs crash. Therefore I implemented
		// simple buffering for the stack dumping code instead of calling
		// hprintf directly.
		char buffer[1000] = "";
		const int safetyzone = 50;
		char* nearend = buffer + sizeof(buffer) - safetyzone;
		char* output = buffer;
		while (pStack + 1 <= pStackTop) {
			if ((Count % StackColumns) == 0)
				output += wsprintf(output, "%08x: ", pStack);
			char *Suffix = " ";
			if ((++Count % StackColumns) == 0 || pStack + 2 > pStackTop)
				Suffix = "\r\n";
			if (IsBadWritePtr(pStack, sizeof(DWORD)))
				break;
			output += wsprintf(output, "%08x%s", *pStack, Suffix);
			pStack++;
			// Check for when the buffer is almost full, and flush it to disk.
			if (output > nearend) {
				hprintf(LogFile, "%s", buffer);
				buffer[0] = 0;
				output = buffer;
			}
		}
		// Print out any final characters from the cache.
		hprintf(LogFile, "%s", buffer);
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		hprintf(LogFile, "Exception encountered during stack dump.\r\n");
	}

	hprintf(LogFile, "\r\n"
		"Call Stack:\r\n");

	STACKFRAME sf;
	memset(&sf, 0, sizeof(sf));

	sf.AddrPC.Offset       = context.Eip;
	sf.AddrPC.Mode         = AddrModeFlat;
	sf.AddrStack.Offset    = context.Esp;
	sf.AddrStack.Mode      = AddrModeFlat;
	sf.AddrFrame.Offset    = context.Ebp;
	sf.AddrFrame.Mode      = AddrModeFlat;

	while ( 1 )
	{
		if (!StackWalk(IMAGE_FILE_MACHINE_I386,
						GetCurrentProcess(),
						hThread,
						&sf,
						&context,
						0,
						SymFunctionTableAccess,
						SymGetModuleBase,
						0))
			break;

		if (0 == sf.AddrFrame.Offset) // Bail if frame is not okay.
			break;

		hprintf(LogFile, "%08x  %08x  ", sf.AddrPC.Offset, sf.AddrFrame.Offset);

		BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + 512];
		PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
		pSymbol->SizeOfStruct = sizeof(symbolBuffer);
		pSymbol->MaxNameLength = 512;

		DWORD symDisplacement = 0;

		if (SymGetSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset,
							&symDisplacement, pSymbol)) {
			hprintf(LogFile, "%s+%x ", pSymbol->Name, symDisplacement);
		} 
		{// No symbol found.  Print out the logical address instead.
			char CrashModulePathName[MAX_PATH];
			char *CrashModuleFileName = "Unknown";
			MEMORY_BASIC_INFORMATION MemInfo;
			// VirtualQuery can be used to get the allocation base associated with a
			// code address, which is the same as the ModuleHandle. This can be used
			// to get the filename of the module that the crash happened in.
			if (VirtualQuery((void*)sf.AddrPC.Offset, &MemInfo, sizeof(MemInfo)) &&
					GetModuleFileName((HINSTANCE)MemInfo.AllocationBase,
					CrashModulePathName, sizeof(CrashModulePathName)) > 0)
				CrashModuleFileName = GetFilePart(CrashModulePathName);

			CHAR szModule[MAX_PATH] = "";
			DWORD section = 0, offset = 0;
			GetLogicalAddress((PVOID)sf.AddrPC.Offset,
								szModule, sizeof(szModule), section, offset);
			hprintf(LogFile, "%04x:%08x %s\r\n", section, offset, szModule);
			hprintf(LogFile, "Params: %08x %08x %08x %08x\r\n", sf.Params[0], sf.Params[1], sf.Params[2], sf.Params[3]);
/*            hprintf(LogFile, "[%s] Bytes at CS:EIP:\r\n   ", CrashModuleFileName);
			unsigned char *code = (unsigned char*) sf.AddrPC.Offset;
			for (int codebyte = 0; codebyte < NumCodeBytes; codebyte++) {
				__try {
					hprintf(LogFile, "%02x ", code[codebyte]);
				}
				__except(EXCEPTION_EXECUTE_HANDLER) {
					hprintf(LogFile, "?? ");
				}
			}
			hprintf(LogFile, "\r\n\r\n");
*/        }
	}
	ResumeThread(hThread);
}

void	DumpStack()
{
	CLogCritical theCritical;
	HANDLE LogFile = CreateFile(g_szLogPath, GENERIC_WRITE, 0, 0,
	OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);
	if (LogFile == INVALID_HANDLE_VALUE) {
		OutputDebugString("Error creating exception report");
		return;
	}
	// Append to the error log.
	SetFilePointer(LogFile, 0, 0, FILE_END);
	// Print out some blank lines to separate this error log from any previous ones.
	SYSTEMTIME st = {0};
	::GetLocalTime(&st);
	hprintf(LogFile,  "[(%d) %04d/%02d/%02d %02d:%02d:%02d]: Dump Stack =======================\r\n", GetCurrentThreadId(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
	if (SymInitialize(GetCurrentProcess(), 0, TRUE)) {
		SymSetOptions(SYMOPT_DEFERRED_LOADS);
		for (int i = 0; i < g_nThread; i++) {
			hprintf(LogFile, "\r\nCall Stack Information %d (%d)\r\n", i, g_nThreadId[i]);
			ImageHelpStackWalk(LogFile, g_hThread[i]);
		}
		SymCleanup(GetCurrentProcess());
	}
	CloseHandle(LogFile);
}
#endif // IOSOCKET

static bool xfirst = true;
void violation()
{
#ifdef	_DEBUG
	DebugBreak();
#else
	if (!xfirst)
		return;
	xfirst = false;
	ELOG("Intentional Exception\n");
	BEFORE
		char *p = 0;
		*p = 0;
	AFTER
		send_exception(FALSE);
	FIN
#endif
}
