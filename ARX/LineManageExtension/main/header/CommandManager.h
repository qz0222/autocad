#pragma once

#include <map>
#include <string>

#include <accmd.h>

using namespace std;

#define MAIN_MENU_NAME L"管线辅助系统"

#define MAIN_MENU_LINE_CONFIG L"类型定义"

#define MAIN_MENU_LINE_INPUT L"位置录入"

#define MAIN_MENU_LINE_CUT L"生成切图"

#define MAIN_MENU_LINE_CUT_BACK L"恢复视窗"

#define MAIN_MENU_LINE_TEST L"功能测试"

class CommandManager
{
public:

	static CommandManager* gCmdManager;

	static CommandManager* instance();

	static void Release();

	void RegisterCommand() const;

	void UnRegisterCommand() const;

	static void ShowConfigDialog();

	static void ShowAddLineDialog();

	static void GenerateCut();

	static void GenerateCutBack();

	static void TestFunction();

	static const WCHAR* CMD_GROUP;

	static const WCHAR* CMD_LINE_CONFIG; 
	static const WCHAR* CMD_LINE_INPUT;
	static const WCHAR* CMD_LIEN_CUT;
	static const WCHAR* CMD_LINE_CUT_BACK;
	static const WCHAR* CMD_LINE_TEST;

private:

	CommandManager(void);
	~CommandManager(void);

	//list to contains the commands supported
	map<wstring,AcRxFunctionPtr> mSupportCommands;
};
