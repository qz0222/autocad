#include "stdafx.h"
#include "CommandManager.h"

#include "LineManageAssitant.h"

#include "AsdkAcUiDialogSample.h"
#include "AcExtensionModule.h"

#include "LineTypeConfigPropertySheet.h"
#include "LineCutPosDialog.h"
#include <EntryManageDialog.h>
#include <ArxWrapper.h>

using namespace com::guch::assistant::config;
using namespace com::guch::assistant::entry;

typedef map<wstring,AcRxFunctionPtr>::const_iterator CommandIterator;

CommandManager* CommandManager::gCmdManager = NULL;

const WCHAR* CommandManager::CMD_GROUP = L"LMA_CMD_GROUP";
const WCHAR* CommandManager::CMD_LINE_CONFIG = L"LMA_CONFIG";
const WCHAR* CommandManager::CMD_LINE_INPUT = L"LMA_INPUT";
const WCHAR* CommandManager::CMD_LIEN_CUT = L"LMA_CUT";
const WCHAR* CommandManager::CMD_LINE_CUT_BACK = L"LMA_CUT_BACK";
const WCHAR* CommandManager::CMD_LINE_TEST = L"LMA_TESTFUN";

CommandManager* CommandManager::instance()
{
	if( gCmdManager == NULL )
	{
		gCmdManager = new CommandManager();
	}

	return gCmdManager;
}

void CommandManager::Release()
{
	if( gCmdManager )
	{
		delete gCmdManager;
		gCmdManager = NULL;
	}
}

CommandManager::CommandManager(void)
{
	//注册主功能
	mSupportCommands[CMD_LINE_CONFIG] = ShowConfigDialog;
	mSupportCommands[CMD_LINE_INPUT] = ShowAddLineDialog;
	mSupportCommands[CMD_LIEN_CUT] = GenerateCut;
	mSupportCommands[CMD_LINE_CUT_BACK] = GenerateCutBack;
	mSupportCommands[CMD_LINE_TEST] = TestFunction;
}

CommandManager::~CommandManager(void)
{
}

void CommandManager::RegisterCommand() const
{
	for( CommandIterator iter = this->mSupportCommands.begin();
		iter != this->mSupportCommands.end();
		iter++)
	{
		CAcModuleResourceOverride resOverride;

		CString globalCmd;
		globalCmd.Format(L"G_%s",iter->first.c_str());

		acedRegCmds->addCommand(CMD_GROUP,globalCmd,
			iter->first.c_str(),
			ACRX_CMD_MODAL,
			iter->second);
	}
}

void CommandManager::UnRegisterCommand() const
{
	acedRegCmds->removeGroup(CMD_GROUP);

	CommandManager::Release();
}

void CommandManager::ShowConfigDialog()
{
	// Modal
	LineTypeConfigPropertySheet dlg(L"管线类型配置",CWnd::FromHandle(adsw_acadMainWnd()));
	INT_PTR nReturnValue = dlg.DoModal();
}

void CommandManager::ShowAddLineDialog()
{
	EntryManageDialog dlg(CWnd::FromHandle(adsw_acadMainWnd()));
	INT_PTR nReturnValue = dlg.DoModal();
}

void CommandManager::GenerateCut()
{
	//AsdkAcUiDialogSample dlg(CWnd::FromHandle(adsw_acadMainWnd()));
	LineCutPosDialog dlg(CWnd::FromHandle(adsw_acadMainWnd()));
	INT_PTR nReturnValue = dlg.DoModal();
}

void CommandManager::GenerateCutBack()
{
#ifdef DEBUG
	acutPrintf(L"\n通过点击菜单恢复视窗");
#endif
	LineCutPosDialog::CutBack();
}

void CommandManager::TestFunction()
{
#ifdef DEBUG
	acutPrintf(L"\n测试AutoCAD的功能");
#endif
	ArxWrapper::TestFunction();
}
