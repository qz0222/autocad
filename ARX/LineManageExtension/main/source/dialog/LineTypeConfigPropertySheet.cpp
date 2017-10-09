// LineConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "afxdialogex.h"

#include "resource.h"

#include <LineConfigDataManager.h>
#include <LineTypeConfigPropertySheet.h>
#include <LMAUtils.h>

#include <acutads.h>

namespace com
{

namespace guch
{

namespace assistant
{

namespace config
{

// Settings for Buttons at the bottom of Property Sheet
const int LineTypeConfigPropertySheet::BTN_WIDTH = 75;
const int LineTypeConfigPropertySheet::BTN_HEIGHT = 23;
const int LineTypeConfigPropertySheet::LEFT_WIDTH = 5;
const int LineTypeConfigPropertySheet::BOTTOM_MARGIN = 10;
const int LineTypeConfigPropertySheet::BTN_MARGIN = 10;

// LineConfigDialog dialog

IMPLEMENT_DYNAMIC(LineTypeConfigPropertySheet, CPropertySheet)

LineTypeConfigPropertySheet::LineTypeConfigPropertySheet(UINT nIDCaption, CWnd* pParentWnd /* = NULL */, UINT iSelectPage /* = 0 */)
:CPropertySheet(nIDCaption,pParentWnd,iSelectPage)
{
	//初始化各页面
	InitPages();
}

LineTypeConfigPropertySheet::LineTypeConfigPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd /* = NULL */, UINT iSelectPage /* = 0 */)
:CPropertySheet(pszCaption,pParentWnd,iSelectPage)
{
	//初始化各页面
	InitPages();
}

LineTypeConfigPropertySheet::~LineTypeConfigPropertySheet()
{
}

void LineTypeConfigPropertySheet::DoDataExchange(CDataExchange* pDX)
{
	CPropertySheet::DoDataExchange(pDX);
}

BOOL LineTypeConfigPropertySheet::OnInitDialog()
{
	CPropertySheet::OnInitDialog();

	//初始化窗口底部的新增、修改、删除、应用按钮
	this->InitButtons();

	//和页面交互数据
	return TRUE;
}

BOOL LineTypeConfigPropertySheet::InitPages()
{	
	m_psh.dwFlags |= PSH_NOAPPLYNOW;    // Lose the Apply Now button
	m_psh.dwFlags &= ~PSH_HASHELP;  // Lose the Help button

	this->AddPage(&this->m_LineConfigDlg);
	this->AddPage(&this->m_BlockConfigDlg);

	return TRUE;
}

BOOL LineTypeConfigPropertySheet::InitButtons()
{ 
	RECT rc;
	GetWindowRect (&rc);

	// Convert to client coordinates
	ScreenToClient (&rc);

	// index of the button from left
	int index = 0;

	//Create new "Add" button and set standard font
	m_ButtonAdd.Create (L"新增", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
						CRect (LEFT_WIDTH + index * ( BTN_MARGIN + BTN_WIDTH),
								rc.bottom - BTN_HEIGHT - BOTTOM_MARGIN, 
								LEFT_WIDTH + index * ( BTN_MARGIN + BTN_WIDTH) + BTN_WIDTH, 
								rc.bottom-BOTTOM_MARGIN),
						this, IDC_BUTTON_LINETYPE_ADD);

	//和对话框中的字体保持一致，否则会出现按钮上的粗细不一致
	m_ButtonAdd.SetFont(GetFont());
	index++;

	// Create new "Modify" button and set standard font
	m_ButtonMod.Create (L"修改", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
						CRect (LEFT_WIDTH + index * ( BTN_MARGIN + BTN_WIDTH),
								rc.bottom - BTN_HEIGHT - BOTTOM_MARGIN, 
								LEFT_WIDTH + index * ( BTN_MARGIN + BTN_WIDTH) + BTN_WIDTH, 
								rc.bottom - BOTTOM_MARGIN),
						this, IDC_BUTTON_LINETYPE_MOD);

	m_ButtonMod.SetFont(GetFont());
	index++;

	// Create new "Delete" button and set standard font
	m_ButtonDel.Create (L"删除", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
						CRect (LEFT_WIDTH + index * ( BTN_MARGIN + BTN_WIDTH),
								rc.bottom - BTN_HEIGHT - BOTTOM_MARGIN, 
								LEFT_WIDTH + index * ( BTN_MARGIN + BTN_WIDTH) + BTN_WIDTH, 
								rc.bottom - BOTTOM_MARGIN),
						this, IDC_BUTTON_LINETYPE_DEL);

	m_ButtonDel.SetFont(GetFont());
	index++;

	//修改确定按钮
	SetControlText(this,IDOK,L"关闭");

	SetControlText(this,IDHELP,L"帮助");

	//覆盖按钮
	//OverControl(this,IDOK,IDCANCEL);

	//删除取消
	ShowControl(this,IDCANCEL,false);

	//默认删除、修改不可用
	ChangeControlStatus(FALSE);

	return TRUE;
}

BEGIN_MESSAGE_MAP(LineTypeConfigPropertySheet, CPropertySheet)
	ON_BN_CLICKED(IDC_BUTTON_LINETYPE_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_LINETYPE_MOD, OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_LINETYPE_DEL, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_LINETYPE_APL, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedButtonOK)
END_MESSAGE_MAP()

void LineTypeConfigPropertySheet::OnBnClickedButtonAdd()
{
	acutPrintf(L"增加配置.\n");

	// TODO: Add your control notification handler code here
	CPropertyPage* pConfigPage = GetActivePage();
	if( pConfigPage )
	{
		LineConfigDialog* pLineConfigPage = dynamic_cast<LineConfigDialog*>(pConfigPage);

		if( pLineConfigPage )
		{
			acutPrintf(L"管线类型.\n");
			pLineConfigPage->OnBnClickedButtonAdd();
		}

		//pAddLinePage->SendNotifyMessageW(0,1,2);
		//SendMessageA(pLineConfigPage->m_hWnd,0,1,2);
	}
}

void LineTypeConfigPropertySheet::OnBnClickedButtonModify()
{
	// TODO: Add your control notification handler code here
	acutPrintf(L"更改配置.\n");

	// TODO: Add your control notification handler code here
	CPropertyPage* pConfigPage = GetActivePage();
	if( pConfigPage )
	{
		LineConfigDialog* pLineConfigPage = dynamic_cast<LineConfigDialog*>(pConfigPage);

		if( pLineConfigPage )
		{
			acutPrintf(L"管线类型.\n");
			pLineConfigPage->OnBnClickedButtonMod();
		}
	}
}

void LineTypeConfigPropertySheet::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
	acutPrintf(L"删除配置.\n");

	// TODO: Add your control notification handler code here
	CPropertyPage* pConfigPage = GetActivePage();
	if( pConfigPage )
	{
		LineConfigDialog* pLineConfigPage = dynamic_cast<LineConfigDialog*>(pConfigPage);

		if( pLineConfigPage )
		{
			acutPrintf(L"管线类型.\n");
			pLineConfigPage->OnBnClickedButtonDel();
		}
	}
}

void LineTypeConfigPropertySheet::OnBnClickedButtonApply()
{
	// TODO: Add your control notification handler code here
}

void LineTypeConfigPropertySheet::OnBnClickedButtonOK()
{
	// TODO: Add your control notification handler code here

	//clear all the lines
	//LineConfigDataManager::Instance()->ClearAllLines();

	//m_LineConfigDlg.OnBnClickedButtonOK();

	//acutPrintf(L"保存数据到文件.\n");
	//LineConfigDataManager::Instance()->Persistent();

	CPropertySheet::OnClose();
}

void LineTypeConfigPropertySheet::ChangeControlStatus(BOOL bEnable)
{
	m_ButtonMod.EnableWindow(bEnable);
	m_ButtonDel.EnableWindow(bEnable);
}

} // end of config

} // end of assistant

} // end of guch

} // end of com

// LineConfigDialog message handlers
