#pragma once

#include "MenuLMAMain.h"

class MenuManager
{
public:

	static void CreateMenu(void* appId);

	static MenuManager* instance();

	static void unRegister();

private:

	MenuManager(const void* appId);

	~MenuManager(void);

	static MenuManager* gMenuManager;

	const void* mAppId;

	MenuLMAMain* mpMainMenu;

	/**
	* 注册上下文菜单
	**/
	void RegisterMenu();

	/**
	* 注册主下拉菜单
	**/
	void AddDropdownMenu();

	/**
	* 注销系统菜单
	**/
	void UnRegisterMenu();

	/**
	* 注销主下拉菜单
	**/
	void RemoveDropdownMenu();
};

