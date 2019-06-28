///////////////////////////////////////////////////////////////////////////////
//
//  Module: excprpt.cpp
//
//    Desc: See excprpt.h
//
// Copyright (c) 2003 Michael Carruth
//
///////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "excprpt.h"
#include "utility.h"

CString CExceptionReport::m_sModule = _T("");
CString CExceptionReport::m_sException = _T("");
CString CExceptionReport::m_sAddress = _T("");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
// CExceptionReport::CExceptionReport
//
// 
//
CExceptionReport::CExceptionReport(PEXCEPTION_POINTERS ExceptionInfo, DWORD dwThreadID)
{
   m_excpInfo = ExceptionInfo;
   m_dwThreadID = dwThreadID;
   m_sModuleOfCrash[0] = 0;
}

// Taken from superassert.c in UtilitiesLib
typedef BOOL (__stdcall *MiniDumpWriter)(
	HANDLE hProcess,
	DWORD ProcessId,
	HANDLE hFile,
	MINIDUMP_TYPE DumpType,
	PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	PMINIDUMP_CALLBACK_INFORMATION CallbackParam
	);
typedef struct DumpThreadParams
{
	char*					filename;
	DWORD					threadId;
	PEXCEPTION_POINTERS		exceptInfo;
	BOOL					error;
} DumpThreadParams;
static MiniDumpWriter pMiniDumpWriteDump = NULL;
unsigned WINAPI MiniDumpThread(void* pData);
// The default set of flags to pass to the minidump writer
MINIDUMP_TYPE minidump_flags = (MINIDUMP_TYPE)(MiniDumpNormal |  MiniDumpWithProcessThreadData | MiniDumpWithDataSegs);
// The set of flags that work on older versions of dbghelp.dll
MINIDUMP_TYPE minidump_old_flags = (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithDataSegs);

static void assertWriteMiniDump(char* filename, PEXCEPTION_POINTERS info, DWORD dwThreadID)
{
	HMODULE debugDll;
	HANDLE hThread;
	unsigned int dumpThreadId;
	DumpThreadParams params;

	// Try to load the debug help dll or imagehlp.dll
	debugDll = LoadLibrary( "dbghelp.dll" );
	if(debugDll == NULL)
	{
		debugDll = LoadLibrary( "imagehlp.dll" );
		if(debugDll == NULL)
			return;
	}
	pMiniDumpWriteDump = (MiniDumpWriter) GetProcAddress(debugDll, "MiniDumpWriteDump");
	if(!pMiniDumpWriteDump)
		return;

	// Use the other thread to write with (some versions of MiniDumpWriteDump need to pause this thread)
	params.exceptInfo = info;
	params.filename = filename;
	params.threadId = dwThreadID?dwThreadID:GetCurrentThreadId();
	hThread = (HANDLE)_beginthreadex(NULL, 0, MiniDumpThread, &params, 0, &dumpThreadId);
	if (INVALID_HANDLE_VALUE != hThread)
	{
		WaitForSingleObject(hThread, 1000*60*10);
		CloseHandle(hThread);
	}

	pMiniDumpWriteDump = NULL;
	FreeLibrary(debugDll);
}

static unsigned WINAPI MiniDumpThread(void* pData)
{
	HANDLE hFile;
	DumpThreadParams* pParams = (DumpThreadParams*)pData;

	pParams->error = 0;

	// Create the file first.
	hFile = CreateFile(pParams->filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;

		mdei.ThreadId = pParams->threadId;
		mdei.ExceptionPointers = pParams->exceptInfo;
		mdei.ClientPointers = TRUE;

		MINIDUMP_CALLBACK_INFORMATION cbMiniDump;
		cbMiniDump.CallbackRoutine = CExceptionReport::miniDumpCallback;
		cbMiniDump.CallbackParam = 0;

		pParams->error = !pMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, 
			minidump_flags,
			pParams->exceptInfo? &mdei: NULL, NULL, &cbMiniDump);
		if (pParams->error) // try again with simpler options
			pParams->error = !pMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, 
			minidump_old_flags,
			pParams->exceptInfo? &mdei: NULL, NULL, &cbMiniDump);

		CloseHandle(hFile);
	}
	else
	{
		// Could not open the file!
		pParams->error = 1;
	}
	return !pParams->error;
}




//-----------------------------------------------------------------------------
// CExceptionReport::getCrashFile
//
// Creates the dump file returning the file name
//
CString CExceptionReport::getCrashFile()
{
   CString sFile;

   // Create the dump file name
   TCHAR tempPath[MAX_PATH];
   GetTempPath(sizeof(tempPath) / sizeof(tempPath[0]), tempPath);
   sFile.Format(_T("%s%s.mdmp"), tempPath, CUtility::getAppName());

   //
   // Write the minidump to the file via another thread
   //

	assertWriteMiniDump(sFile.LockBuffer(), m_excpInfo, m_dwThreadID);
	sFile.UnlockBuffer();
	/*
	old stuff:
	MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithDataSegs),
        m_excpInfo ? &eInfo : NULL,
        NULL,
        &cbMiniDump);*/

   return sFile;
}


//-----------------------------------------------------------------------------
// CExceptionReport::getCrashLog
//
// Creates the XML log file returning the name
//
CString CExceptionReport::getCrashLog(const char *szAuth, const char *szEntity, const char *szShard, const char *szShardTime, const char *szVersion, const char *szMessage)
{
   CString sFile;
   MSXML::IXMLDOMDocument *pDoc  = NULL;
   MSXML::IXMLDOMNode *root      = NULL;
   MSXML::IXMLDOMNode *node      = NULL;
   MSXML::IXMLDOMNode *newNode   = NULL;
   BSTR rootName = ::SysAllocString(L"CrashLog");
   VARIANT v;

   CoInitialize(NULL);

   // Create an empty XML document
   CHECKHR(CoCreateInstance(
      MSXML::CLSID_DOMDocument, 
      NULL, 
      CLSCTX_INPROC_SERVER,
      MSXML::IID_IXMLDOMDocument, 
      (void**)&pDoc));

   // Create root node
   root = CreateDOMNode(pDoc, MSXML::NODE_ELEMENT, rootName);

   //
   // Add exception record node
   //
   if (m_excpInfo)
   {
      node = CreateExceptionRecordNode(pDoc, m_excpInfo->ExceptionRecord);
      CHECKHR(root->appendChild(node, &newNode));
      // The XML Document should now own the node.
      SAFERELEASE(node);
      SAFERELEASE(newNode);
   }

   //
   // Add game-specific node
   //
   node = CreateGameNode(pDoc, szAuth, szEntity, szShard, szShardTime);
   CHECKHR(root->appendChild(node, &newNode));
   // The XML Document should now own the node.
   SAFERELEASE(node);
   SAFERELEASE(newNode);

   //
   // Add program-specific node
   //
   node = CreateProgramNode(pDoc, szVersion, szMessage);
   CHECKHR(root->appendChild(node, &newNode));
   // The XML Document should now own the node.
   SAFERELEASE(node);
   SAFERELEASE(newNode);

   //
   // Add processor node
   //
   node = CreateProcessorNode(pDoc);
   CHECKHR(root->appendChild(node, &newNode));
   // The XML Document should now own the node.
   SAFERELEASE(node);
   SAFERELEASE(newNode);

   //
   // Add OS node
   //
   node = CreateOSNode(pDoc);
   CHECKHR(root->appendChild(node, &newNode));
   // The XML Document should now own the node.
   SAFERELEASE(node);
   SAFERELEASE(newNode);

   //
   // Add modules node
   //
   node = CreateModulesNode(pDoc, m_excpInfo->ExceptionRecord);
   CHECKHR(root->appendChild(node, &newNode));
   // The XML Document should now own the node.
   SAFERELEASE(node);
   SAFERELEASE(newNode);

   // Add the root to the doc
   CHECKHR(pDoc->appendChild(root, NULL));

   //
   // Create dat file name and save
   //
   TCHAR tempPath[MAX_PATH];
   GetTempPath(sizeof(tempPath) / sizeof(tempPath[0]), tempPath);
   sFile.Format(_T("%s%s.xml"), tempPath, CUtility::getAppName());
   V_VT(&v) = VT_BSTR;
   V_BSTR(&v) = sFile.AllocSysString();
   pDoc->save(v);
   SysFreeString(V_BSTR(&v));

CleanUp:
   SAFERELEASE(pDoc);
   SAFERELEASE(root);
   SAFERELEASE(node);
   SAFERELEASE(newNode);
   SysFreeString(rootName);

   CoUninitialize();

   return sFile;
}


//-----------------------------------------------------------------------------
// CExceptionReport::getNumSymbolFiles
//
// Returns the number of symbols files found
//
int CExceptionReport::getNumSymbolFiles()
{
   return m_symFiles.GetSize();
}


//-----------------------------------------------------------------------------
// CExceptionReport::getSymbolFile
//
// Returns the symbol file name given an index
//
CString CExceptionReport::getSymbolFile(int index)
{
   CString ret;

   if (0 < index && index < m_symFiles.GetSize())
      ret = m_symFiles[index];

   return ret;
}

//-----------------------------------------------------------------------------
// CExceptionReport::CreateDOMNode
//
// Helper function 
//
MSXML::IXMLDOMNode*
CExceptionReport::CreateDOMNode(MSXML::IXMLDOMDocument* pDoc, 
                                int type, 
                                BSTR bstrName)
{
    MSXML::IXMLDOMNode * node;
    VARIANT vtype;

    vtype.vt = VT_I4;
    V_I4(&vtype) = (int)type;

    pDoc->createNode(vtype, bstrName, NULL, &node);
    return node;
}

//-----------------------------------------------------------------------------
// CExceptionReport::CreateExceptionRecordNode
//
//
//
MSXML::IXMLDOMNode*
CExceptionReport::CreateExceptionRecordNode(MSXML::IXMLDOMDocument* pDoc, 
                                            EXCEPTION_RECORD* pExceptionRecord)
{
   MSXML::IXMLDOMNode*     pNode    = NULL;
   MSXML::IXMLDOMElement*  pElement = NULL;
   BSTR nodeName                    = ::SysAllocString(L"ExceptionRecord");
   BSTR modName                     = ::SysAllocString(L"ModuleName");
   BSTR codeName                    = ::SysAllocString(L"ExceptionCode");
   BSTR descName                    = ::SysAllocString(L"ExceptionDescription");
   BSTR addrName                    = ::SysAllocString(L"ExceptionAddress");
   VARIANT v;
   CString sAddr;

   // Create exception record node
   pNode = CreateDOMNode(pDoc, MSXML::NODE_ELEMENT, nodeName);

   // Get element interface
   CHECKHR(pNode->QueryInterface(MSXML::IID_IXMLDOMElement, (void**)&pElement));

   //
   // Set module name attribute
   //
   TCHAR szModName[_MAX_FNAME + 1];
   GetModuleFileName(NULL, szModName, _MAX_FNAME);
   m_sModule = szModName;
   V_VT(&v)    = VT_BSTR;
   V_BSTR(&v)  = A2BSTR(szModName);
   pElement->setAttribute(modName, v);
   // Recycle variant
   SysFreeString(V_BSTR(&v));

   //
   // Set exception code
   //
   sAddr.Format(_T("%#x"), pExceptionRecord->ExceptionCode);
   m_sException = sAddr;
   V_VT(&v)    = VT_BSTR;
   V_BSTR(&v)  = sAddr.AllocSysString();
   pElement->setAttribute(codeName, v);
   // Recycle variant
   SysFreeString(V_BSTR(&v));

   //
   // Set exception description
   //
   V_VT(&v)    = VT_BSTR;
   switch (pExceptionRecord->ExceptionCode)
   {
   case EXCEPTION_ACCESS_VIOLATION:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_ACCESS_VIOLATION");
      break;
   case EXCEPTION_DATATYPE_MISALIGNMENT:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_DATATYPE_MISALIGNMENT");
      break;
   case EXCEPTION_BREAKPOINT:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_BREAKPOINT");
      break;
   case EXCEPTION_SINGLE_STEP:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_SINGLE_STEP");
      break;
   case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
      break;
   case EXCEPTION_FLT_DENORMAL_OPERAND:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_FLT_DENORMAL_OPERAND");
      break;
   case EXCEPTION_FLT_DIVIDE_BY_ZERO:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_FLT_DIVIDE_BY_ZERO");
      break;
   case EXCEPTION_FLT_INEXACT_RESULT:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_FLT_INEXACT_RESULT");
      break;
   case EXCEPTION_FLT_INVALID_OPERATION:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_FLT_INVALID_OPERATION");
      break;
   case EXCEPTION_FLT_OVERFLOW:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_FLT_OVERFLOW");
      break;
   case EXCEPTION_FLT_STACK_CHECK:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_FLT_STACK_CHECK");
      break;
   case EXCEPTION_FLT_UNDERFLOW:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_FLT_UNDERFLOW");
      break;
   case EXCEPTION_INT_DIVIDE_BY_ZERO:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_INT_DIVIDE_BY_ZERO");
      break;
   case EXCEPTION_INT_OVERFLOW:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_INT_OVERFLOW");
      break;
   case EXCEPTION_PRIV_INSTRUCTION:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_PRIV_INSTRUCTION");
      break;
   case EXCEPTION_IN_PAGE_ERROR:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_IN_PAGE_ERROR");
      break;
   case EXCEPTION_ILLEGAL_INSTRUCTION:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_ILLEGAL_INSTRUCTION");
      break;
   case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_NONCONTINUABLE_EXCEPTION");
      break;
   case EXCEPTION_STACK_OVERFLOW:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_STACK_OVERFLOW");
      break;
   case EXCEPTION_INVALID_DISPOSITION:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_INVALID_DISPOSITION");
      break;
   case EXCEPTION_GUARD_PAGE:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_GUARD_PAGE");
      break;
   case EXCEPTION_INVALID_HANDLE:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_INVALID_HANDLE");
      break;
   default:
      V_BSTR(&v) = ::SysAllocString(L"EXCEPTION_UNKNOWN");
      break;
   }
   pElement->setAttribute(descName, v);
   // Recycle variant
   SysFreeString(V_BSTR(&v));

   //
   // Set exception address
   //
   sAddr.Format(_T("%#x"), pExceptionRecord->ExceptionAddress);
   m_sAddress = sAddr;
   V_VT(&v)    = VT_BSTR;
   V_BSTR(&v)  = sAddr.AllocSysString();
   pElement->setAttribute(addrName, v);
   // Recycle variant
   SysFreeString(V_BSTR(&v));

CleanUp:
   ::SysFreeString(nodeName);
   ::SysFreeString(modName);
   ::SysFreeString(codeName);
   ::SysFreeString(addrName);
   SAFERELEASE(pElement);

   return pNode;
}

//-----------------------------------------------------------------------------
// CExceptionReport::CreateProcessorNode
//
//
//
MSXML::IXMLDOMNode*
CExceptionReport::CreateProcessorNode(MSXML::IXMLDOMDocument* pDoc)
{
   MSXML::IXMLDOMNode*     pNode    = NULL;
   MSXML::IXMLDOMElement*  pElement = NULL;
   BSTR nodeName                    = ::SysAllocString(L"Processor");
   BSTR archName                    = ::SysAllocString(L"Architecture");
   BSTR levelName                   = ::SysAllocString(L"Level");
   BSTR numberName                  = ::SysAllocString(L"NumberOfProcessors");
   SYSTEM_INFO si;
   VARIANT v;

   // Create exception record node
   pNode = CreateDOMNode(pDoc, MSXML::NODE_ELEMENT, nodeName);

   // Get element interface
   CHECKHR(pNode->QueryInterface(MSXML::IID_IXMLDOMElement, (void**)&pElement));

   //
   // Get processor info
   //
   GetSystemInfo(&si);

   //
   // Set architecture
   //
   V_VT(&v) = VT_BSTR;
   switch (si.wProcessorArchitecture)
   {
   case PROCESSOR_ARCHITECTURE_INTEL:
      V_BSTR(&v) = ::SysAllocString(L"PROCESSOR_ARCHITECTURE_INTEL");
      break;
   case PROCESSOR_ARCHITECTURE_MIPS:
      V_BSTR(&v) = ::SysAllocString(L"PROCESSOR_ARCHITECTURE_MIPS");
      break;
   case PROCESSOR_ARCHITECTURE_ALPHA:
      V_BSTR(&v) = ::SysAllocString(L"PROCESSOR_ARCHITECTURE_ALPHA");
      break;
   case PROCESSOR_ARCHITECTURE_PPC:
      V_BSTR(&v) = ::SysAllocString(L"PROCESSOR_ARCHITECTURE_PPC");
      break;
   case PROCESSOR_ARCHITECTURE_SHX:
      V_BSTR(&v) = ::SysAllocString(L"PROCESSOR_ARCHITECTURE_SHX");
      break;
   case PROCESSOR_ARCHITECTURE_ARM:
      V_BSTR(&v) = ::SysAllocString(L"PROCESSOR_ARCHITECTURE_ARM");
      break;
   case PROCESSOR_ARCHITECTURE_IA64:
      V_BSTR(&v) = ::SysAllocString(L"PROCESSOR_ARCHITECTURE_IA64");
      break;
   case PROCESSOR_ARCHITECTURE_ALPHA64:
      V_BSTR(&v) = ::SysAllocString(L"PROCESSOR_ARCHITECTURE_ALPHA64");
      break;
   case PROCESSOR_ARCHITECTURE_UNKNOWN:
      V_BSTR(&v) = ::SysAllocString(L"PROCESSOR_ARCHITECTURE_UNKNOWN");
      break;
   default:
      V_BSTR(&v) = ::SysAllocString(L"Unknown");
   }
   pElement->setAttribute(archName, v);
   // Recycle variant
   SysFreeString(V_BSTR(&v));

   //
   // Set level
   //
   V_VT(&v) = VT_BSTR;
   if (PROCESSOR_ARCHITECTURE_INTEL == si.wProcessorArchitecture)
   {
      switch (si.wProcessorLevel)
      {
      case 3:
         V_BSTR(&v) = ::SysAllocString(L"Intel 30386");
         break;
      case 4:
         V_BSTR(&v) = ::SysAllocString(L"Intel 80486");
         break;
      case 5:
         V_BSTR(&v) = ::SysAllocString(L"Intel Pentium");
         break;
      case 6:
         V_BSTR(&v) = ::SysAllocString(L"Intel Pentium Pro or Pentium II");
         break;
      default:
         V_BSTR(&v) = ::SysAllocString(L"Unknown");
      }
   }
   pElement->setAttribute(levelName, v);
   // Recycle variant
   SysFreeString(V_BSTR(&v));

   //
   // Set num of processors
   //
   V_VT(&v) = VT_I4;
   V_I4(&v) = si.dwNumberOfProcessors;
   pElement->setAttribute(numberName, v);

CleanUp:
   ::SysFreeString(nodeName);
   ::SysFreeString(archName);
   ::SysFreeString(levelName);
   ::SysFreeString(numberName);
   SAFERELEASE(pElement);

   return pNode;
}

static const char *escapeString(const char *instring)
{
	static char *ret=NULL;
	static int retlen=0;
	const char *c;
	char *out;
	int deslen;

	if(!instring)
		return NULL;
	deslen = ((int)strlen(instring))*2+1;
	if (retlen < deslen) { // We need a bigger buffer to return the data
		if (ret)
			free(ret);
		if (deslen<256) deslen = 256; // Allocate at least 256 bytes the first time
		ret = (char*)malloc(deslen);
		retlen = deslen;
	}
	for (c=instring, out=ret; ; c++) {
		switch (*c) {
			case '\\':
				*out++='\\';
				*out++='\\';
				break;
			case '\n':
				*out++='\\';
				*out++='n';
				break;
			case '\r':
				*out++='\\';
				*out++='r';
				break;
			case '\t':
				*out++='\\';
				*out++='t';
				break;
			default:
				*out++=*c;
		}
		if (!*c) break;
	}
	return ret;
}

//-----------------------------------------------------------------------------
// CExceptionReport::CreateGameNode
//
//
//
MSXML::IXMLDOMNode* 
CExceptionReport::CreateGameNode(MSXML::IXMLDOMDocument* pDoc, const char *szAuth, const char *szEntity, const char *szShard, const char *szShardTime)
{
	MSXML::IXMLDOMNode*     pNode    = NULL;
	MSXML::IXMLDOMElement*  pElement = NULL;
	BSTR nodeName                    = ::SysAllocString(L"GameInfo");
	BSTR authName                    = ::SysAllocString(L"AuthName");
	BSTR entityName                  = ::SysAllocString(L"EntityName");
	BSTR shardName                   = ::SysAllocString(L"ShardName");
	BSTR shardTimeName               = ::SysAllocString(L"MapTimeSince2000");
	VARIANT v;

	// Create exception record node
	pNode = CreateDOMNode(pDoc, MSXML::NODE_ELEMENT, nodeName);

	// Get element interface
	CHECKHR(pNode->QueryInterface(MSXML::IID_IXMLDOMElement, (void**)&pElement));

	//
	// Set auth name
	//
	V_VT(&v) = VT_BSTR;
	V_BSTR(&v) = A2BSTR(escapeString(szAuth ? szAuth : "UnknownAuth"));
	pElement->setAttribute(authName, v);
	::SysFreeString(V_BSTR(&v));

	//
	// Set entity name
	//
	V_VT(&v) = VT_BSTR;
	V_BSTR(&v) = A2BSTR(escapeString(szEntity ? szEntity : "UnknownEntity"));
	pElement->setAttribute(entityName, v);
	::SysFreeString(V_BSTR(&v));

	//
	// Set shard name
	//
	V_VT(&v) = VT_BSTR;
	V_BSTR(&v) = A2BSTR(escapeString(szShard ? szShard : "UnknownShard"));
	pElement->setAttribute(shardName, v);
	::SysFreeString(V_BSTR(&v));

	//
	// Set shard time
	//
	V_VT(&v) = VT_BSTR;
	V_BSTR(&v) = A2BSTR(escapeString(szShardTime ? szShardTime : "UnknownShardTime"));
	pElement->setAttribute(shardTimeName, v);
	::SysFreeString(V_BSTR(&v));

CleanUp:
	::SysFreeString(nodeName);
	::SysFreeString(authName);
	::SysFreeString(entityName);
	::SysFreeString(shardName);
	::SysFreeString(shardTimeName);
	SAFERELEASE(pElement);

	return pNode;
}

//-----------------------------------------------------------------------------
// CExceptionReport::CreateProgramNode
//
//
//
MSXML::IXMLDOMNode* 
CExceptionReport::CreateProgramNode(MSXML::IXMLDOMDocument* pDoc, const char *szVersion, const char *szMessage)
{
	MSXML::IXMLDOMNode*     pNode    = NULL;
	MSXML::IXMLDOMElement*  pElement = NULL;
	BSTR nodeName                    = ::SysAllocString(L"ProgramData");
	BSTR patchVersionName            = ::SysAllocString(L"PatchVersion");
	BSTR messageName                 = ::SysAllocString(L"CrashMessage");
	VARIANT v;

	// Create exception record node
	pNode = CreateDOMNode(pDoc, MSXML::NODE_ELEMENT, nodeName);

	// Get element interface
	CHECKHR(pNode->QueryInterface(MSXML::IID_IXMLDOMElement, (void**)&pElement));

	//
	// Set Patch version
	//
	V_VT(&v) = VT_BSTR;
	V_BSTR(&v) = A2BSTR(escapeString(szVersion));
	pElement->setAttribute(patchVersionName, v);
	::SysFreeString(V_BSTR(&v));

	//
	// Set assert message
	//
	V_VT(&v) = VT_BSTR;
	V_BSTR(&v) = A2BSTR(escapeString(szMessage));
	pElement->setAttribute(messageName, v);
	::SysFreeString(V_BSTR(&v));

CleanUp:
	::SysFreeString(nodeName);
	::SysFreeString(patchVersionName);
	::SysFreeString(messageName);
	SAFERELEASE(pElement);

	return pNode;
}

//-----------------------------------------------------------------------------
// CExceptionReport::CreateOSNode
//
//
//
MSXML::IXMLDOMNode* 
CExceptionReport::CreateOSNode(MSXML::IXMLDOMDocument* pDoc)
{
   MSXML::IXMLDOMNode*     pNode    = NULL;
   MSXML::IXMLDOMElement*  pElement = NULL;
   BSTR nodeName                    = ::SysAllocString(L"OperatingSystem");
   BSTR majorName                   = ::SysAllocString(L"MajorVersion");
   BSTR minorName                   = ::SysAllocString(L"MinorVersion");
   BSTR buildName                   = ::SysAllocString(L"BuildNumber");
   BSTR csdName                     = ::SysAllocString(L"CSDVersion");
   OSVERSIONINFO oi;
   VARIANT v;

   // Create exception record node
   pNode = CreateDOMNode(pDoc, MSXML::NODE_ELEMENT, nodeName);

   // Get element interface
   CHECKHR(pNode->QueryInterface(MSXML::IID_IXMLDOMElement, (void**)&pElement));

   //
   // Get OS info
   //
   oi.dwOSVersionInfoSize = sizeof(oi);
   GetVersionEx(&oi);

   //
   // Set major version
   //
   V_VT(&v) = VT_I4;
   V_I4(&v) = oi.dwMajorVersion;
   pElement->setAttribute(majorName, v);

   //
   // Set minor version
   //
   V_VT(&v) = VT_I4;
   V_I4(&v) = oi.dwMinorVersion;
   pElement->setAttribute(minorName, v);

   //
   // Set build version
   //
   V_VT(&v) = VT_I4;
   V_I4(&v) = oi.dwBuildNumber;
   pElement->setAttribute(buildName, v);

   //
   // Set CSD version
   //
   V_VT(&v) = VT_BSTR;
   V_BSTR(&v) = A2BSTR(oi.szCSDVersion);
   pElement->setAttribute(csdName, v);
   ::SysFreeString(V_BSTR(&v));

CleanUp:
   ::SysFreeString(nodeName);
   ::SysFreeString(majorName);
   ::SysFreeString(minorName);
   ::SysFreeString(buildName);
   ::SysFreeString(csdName);
   SAFERELEASE(pElement);

   return pNode;
}

//-----------------------------------------------------------------------------
// CExceptionReport::CreateModulesNode
//
//
//
MSXML::IXMLDOMNode* 
CExceptionReport::CreateModulesNode(MSXML::IXMLDOMDocument* pDoc, 
									EXCEPTION_RECORD* pExceptionRecord) // For determining the module of the crash
{
   USES_CONVERSION;
   MSXML::IXMLDOMNode*     pNode    = NULL;
   MSXML::IXMLDOMNode*     pNode2   = NULL;
   MSXML::IXMLDOMNode*     pNewNode = NULL;
   MSXML::IXMLDOMElement*  pElement = NULL;
   MSXML::IXMLDOMElement*  pElement2= NULL;
   BSTR nodeName                    = ::SysAllocString(L"Modules");
   BSTR nodeName2                   = ::SysAllocString(L"Module");
   BSTR fullPath                    = ::SysAllocString(L"FullPath");
   BSTR baseAddrName                = ::SysAllocString(L"BaseAddress");
   BSTR sizeName                    = ::SysAllocString(L"Size");
   BSTR timeStampName               = ::SysAllocString(L"TimeStamp");
   BSTR fileVerName                 = ::SysAllocString(L"FileVersion");
   BSTR prodVerName                 = ::SysAllocString(L"ProductVersion");

   CString sAddr;
   VARIANT v;


   // Create modules node
   pNode = CreateDOMNode(pDoc, MSXML::NODE_ELEMENT, nodeName);

   //
   // Add module information
   // 
   node = start.next;
   while (node)
   {
      // Create module node
      pNode2 = CreateDOMNode(pDoc, MSXML::NODE_ELEMENT, nodeName2);

      // Get element interface
      CHECKHR(pNode2->QueryInterface(MSXML::IID_IXMLDOMElement, (void**)&pElement));

	  // Check to see if this is the module that crashed
	  if ((ULONG64)pExceptionRecord->ExceptionAddress >= node->item.BaseOfImage &&
		  (ULONG64)pExceptionRecord->ExceptionAddress < node->item.BaseOfImage + node->item.SizeOfImage)
	  {
		  char *temp = W2A(node->item.FullPath);
		  if (temp) {
			  char *fileName = strrchr(temp, '\\');
			  if (!fileName) fileName = temp; else fileName++;
			  strcpy_s(m_sModuleOfCrash, sizeof(m_sModuleOfCrash), fileName);
		  } else {
			  strcpy_s(m_sModuleOfCrash, sizeof(m_sModuleOfCrash), "Unknown");
		  }
	  }

      //
      // Set full path
      //
      V_VT(&v) = VT_BSTR;
      V_BSTR(&v) = W2BSTR(node->item.FullPath);
      pElement->setAttribute(fullPath, v);
      // Recycle variant
      SysFreeString(V_BSTR(&v));

      //
      // Set base address
      //
      sAddr.Format(_T("0x%08x"), node->item.BaseOfImage);
      V_VT(&v) = VT_BSTR;
      V_BSTR(&v) = sAddr.AllocSysString();
      pElement->setAttribute(baseAddrName, v);
      // Recycle variant
      SysFreeString(V_BSTR(&v));

      //
      // Set module size
      //
      sAddr.Format(_T("0x%08x"), node->item.SizeOfImage);
      V_VT(&v) = VT_BSTR;
      V_BSTR(&v) = sAddr.AllocSysString();
      pElement->setAttribute(sizeName, v);
      // Recycle variant
      SysFreeString(V_BSTR(&v));

      //
      // Set timestamp
      //
      FILETIME    ft = CUtility::getLastWriteFileTime(node->item.FullPath);
      SYSTEMTIME  st = {0};

      FileTimeToSystemTime(&ft, &st);

      sAddr.Format(
         _T("%02u/%02u/%04u %02u:%02u:%02u"), 
         st.wMonth, 
         st.wDay, 
         st.wYear, 
         st.wHour, 
         st.wMinute, 
         st.wSecond);

      V_VT(&v) = VT_BSTR;
      V_BSTR(&v) = sAddr.AllocSysString();
      pElement->setAttribute(timeStampName, v);
      // Recycle variant
      SysFreeString(V_BSTR(&v));

      //
      // Set file version
      //
      sAddr.Format("%d.%d.%d.%d", 
                   HIWORD(node->item.VersionInfo.dwFileVersionMS),
                   LOWORD(node->item.VersionInfo.dwFileVersionMS),
                   HIWORD(node->item.VersionInfo.dwFileVersionLS),
                   LOWORD(node->item.VersionInfo.dwFileVersionLS));
      V_VT(&v) = VT_BSTR;
      V_BSTR(&v) = sAddr.AllocSysString();
      pElement->setAttribute(fileVerName, v);
      // Recycle variant
      SysFreeString(V_BSTR(&v));

      //
      // Set product version
      //
      sAddr.Format("%d.%d.%d.%d", 
                   HIWORD(node->item.VersionInfo.dwProductVersionMS),
                   LOWORD(node->item.VersionInfo.dwProductVersionMS),
                   HIWORD(node->item.VersionInfo.dwProductVersionLS),
                   LOWORD(node->item.VersionInfo.dwProductVersionLS));
      V_VT(&v) = VT_BSTR;
      V_BSTR(&v) = sAddr.AllocSysString();
      pElement->setAttribute(prodVerName, v);
      // Recycle variant
      SysFreeString(V_BSTR(&v));

      //
      // Append module to modules
      //
      pNode->appendChild(pNode2, &pNewNode);
      // The XML Document should now own the node.
      SAFERELEASE(pNode2);
      SAFERELEASE(pElement2);
      SAFERELEASE(pNewNode);

      node = node->next;
   }

CleanUp:

   ::SysFreeString(nodeName);
   ::SysFreeString(nodeName2);
   ::SysFreeString(fullPath);
   ::SysFreeString(baseAddrName);
   ::SysFreeString(sizeName);
   ::SysFreeString(timeStampName);
   ::SysFreeString(fileVerName);
   ::SysFreeString(prodVerName);
   SAFERELEASE(pNode2);
   SAFERELEASE(pNewNode);
   SAFERELEASE(pElement);
   SAFERELEASE(pElement2);

   return pNode;
}

//-----------------------------------------------------------------------------
// CExceptionReport::miniDumpCallback
//
// Mini dump module callback.  Hit once for each module processed by
// MiniDumpWriteDump.  Builds a linked list of all module names which is
// eventually used to create the <modules> node in the XML log file.
//
BOOL CALLBACK 
CExceptionReport::miniDumpCallback(PVOID,
                                   CONST PMINIDUMP_CALLBACK_INPUT CallbackInput,
                                   PMINIDUMP_CALLBACK_OUTPUT)
{
   if (ModuleCallback == CallbackInput->CallbackType && node)
   {
      node->next = new ModuleListEntry;
      node = node->next;
      node->item = CallbackInput->Module;
      node->item.FullPath = _wcsdup(CallbackInput->Module.FullPath);
      node->next = NULL;
   }

   return TRUE;
}