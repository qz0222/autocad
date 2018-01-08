//
//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2012 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
//
// AsdkAcUiSample.cpp : Defines the initialization routines for the DLL.
//
#include "StdAfx.h"

#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif

#include "LineManageAssitant.h"

#include "AsdkAcUiDialogSample.h"
#include "AcExtensionModule.h"

#include "MenuManager.h"

#include "CommandManager.h"

#include <ArxCustomObject.h>

using namespace com::guch::assistant::arx;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern "C" HWND adsw_acadMainWnd();

/////////////////////////////////////////////////////////////////////////////
// Define the sole extension module object.
AC_IMPLEMENT_EXTENSION_MODULE(theArxDLL);

static void initApp(void* appId)
{
  // 注册菜单
  MenuManager::CreateMenu(appId);

  // 注册命令
  CommandManager::instance()->RegisterCommand();

  // 注册自定义类
  LMADbObjectManager::RegisterClass();
}

static void unloadApp()
{
  // 移除默认上下文菜单
  MenuManager::unRegister();

  //删除命令
  CommandManager::instance()->UnRegisterCommand();

  // 注销自定义类
  LMADbObjectManager::RegisterClass();
}

static void dwgLoaded()
{
	//从当前DWG文件中读取管线信息
	//LineEntryFileManager::ReadFromCurrentDWG();
}

static void dwgUnLoaded()
{
	//有DWG文件卸载，删除配置数据
	LineEntryFileManager::RemoveEntryFileOnDWGUnLoad();
}

//////////////////////////////////////////////////////////////
//
// Entry points
//
//////////////////////////////////////////////////////////////

static AFX_EXTENSION_MODULE MyAsdkMfcComSampDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    // Remove this if you use lpReserved
    UNREFERENCED_PARAMETER(lpReserved);
    
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        theArxDLL.AttachInstance(hInstance);

		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(MyAsdkMfcComSampDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(MyAsdkMfcComSampDLL);

    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        theArxDLL.DetachInstance();  
		
		TRACE0("MyAsdkMfcComSamp.DLL Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(MyAsdkMfcComSampDLL);
    }
    return 1;   // ok
}



extern "C" AcRx::AppRetCode acrxEntryPoint( AcRx::AppMsgCode msg, void* appId)
{
    switch( msg ) 
    {
    case AcRx::kInitAppMsg: 

        acrxDynamicLinker->unlockApplication(appId);
        acrxDynamicLinker->registerAppMDIAware(appId);
        
		initApp(appId); 
        break;

    case AcRx::kUnloadAppMsg: 

        unloadApp(); 
        break;

	case AcRx::kLoadDwgMsg:

		dwgLoaded();
        break;

    case AcRx::kUnloadDwgMsg:

		dwgUnLoaded();
        break;

    case AcRx::kInitDialogMsg:
        
        break;
    default:
        break;
    }
    return AcRx::kRetOK;
}

