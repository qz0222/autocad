// AddEntryDialog.cpp : implementation file
//

#include "stdafx.h"
#include "afxdialogex.h"

#include <AddEntryDialog.h>
#include <LineConfigDataManager.h>
#include <GlobalDataConfig.h>
#include <EntryManageDialog.h>

using namespace std;
using namespace com::guch::assistant::config;
using namespace com::guch::assistant::data;

// AddEntryDialog dialog

namespace com
{

namespace guch
{

namespace assistant
{

namespace entry
{

IMPLEMENT_DYNAMIC(AddEntryDialog, CDialog)

AddEntryDialog::AddEntryDialog(CWnd* pParent /*=NULL*/)
	: CDialog(AddEntryDialog::IDD, pParent)
{
}

AddEntryDialog::~AddEntryDialog()
{
}

BOOL AddEntryDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitConfigData();

	return TRUE;
}

BOOL AddEntryDialog::InitConfigData()
{
	//增加类别
	m_LineCategory.AddString(GlobalData::KIND_LINE.c_str());
	m_LineCategory.AddString(GlobalData::KIND_SEPERATOR.c_str());

	//初始化管线种类
	m_LineCategory.SetCurSel(0);
	OnSelchangeComboCategory();

	if( m_OperType == OPER_UPDATE )
	{
		//填充数据
		FillUpdateData();
	}

	return TRUE;
}

void AddEntryDialog::FillUpdateData()
{
		//更改控件的标题
	this->SetWindowTextW(L"管线实体更新");

	//Get the parent windows
	EntryManageDialog* entryConfig 
		= dynamic_cast<EntryManageDialog*>(this->m_pParentWnd);

	if( !entryConfig )
		return;

	//得到选择的数据
	LineEntry* lineEntry = entryConfig->GetLineEntry(this->m_UpdateLineID);
	if( !lineEntry )
		return;

	//填充数据
	wstring category = LineConfigDataManager::Instance()->FindCategory(lineEntry->m_LineKind);

	m_LineCategory.SelectString(0,category.c_str());
	m_LineCategory.EnableWindow(FALSE);
	OnSelchangeComboCategory();

	m_LineKind.SelectString(0,lineEntry->m_LineKind.c_str());
	m_LineKind.EnableWindow(FALSE);

	m_LineName.SetWindowTextW(lineEntry->m_LineName.c_str());
	m_LineNO.SetWindowTextW(lineEntry->m_LineNO.c_str());

	UpdateData(FALSE);
}

void AddEntryDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_CATEGORY, m_LineCategory);
	DDX_Control(pDX, IDC_COMBO_KIND, m_LineKind);

	DDX_Control(pDX, IDC_EDIT_NO, m_LineNO);
	DDX_Control(pDX, IDC_EDIT_NAME, m_LineName);
}

BEGIN_MESSAGE_MAP(AddEntryDialog, CDialog)
	ON_BN_CLICKED(IDOK, &AddEntryDialog::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_CATEGORY, &AddEntryDialog::OnSelchangeComboCategory)
END_MESSAGE_MAP()

void AddEntryDialog::OnSelchangeComboCategory()
{
	//得到用户的选择
	UpdateData(TRUE);

	CString category;
	int selected = m_LineCategory.GetCurSel();

	int length = m_LineCategory.GetLBTextLen(selected);
    m_LineCategory.GetLBText(selected, category.GetBuffer(length) );

	//清空当前列表
	for (int i = m_LineKind.GetCount()-1; i >= 0; i--)
	{
	   m_LineKind.DeleteString(i);
	}

	LineCategoryVecotr* configList = const_cast<LineCategoryVecotr*>(LineConfigDataManager::Instance()->GetData());
	for( CategoryIterator iter = configList->begin();
			iter != configList->end();
			iter++)
	{
		if( (*iter)->mCategory == category.GetBuffer() )
		{
			m_LineKind.AddString((*iter)->mName.c_str());
		}
	}

	m_LineKind.SetCurSel(0);
}

void AddEntryDialog::OnBnClickedOk()
{
	//Get the parent windows
	EntryManageDialog* entryConfig 
		= dynamic_cast<EntryManageDialog*>(this->m_pParentWnd);

	if( !entryConfig )
		return;

	UpdateData(TRUE);

	CString lineCategory,lineKind,lineName,lineNO;

	//m_LineCategory.GetWindowTextW(lineCategory);
	m_LineKind.GetWindowTextW(lineKind);

	m_LineName.GetWindowTextW(lineName);
	m_LineNO.GetWindowTextW(lineNO);

	LineEntry* newLine = new LineEntry(lineNO.GetBuffer(),lineName.GetBuffer(),lineKind.GetBuffer());

	if( this->m_OperType == OPER_ADD )
	{
		if( !entryConfig->InsertLine(newLine,FALSE) )
			return;
	}
	else if ( this->m_OperType == OPER_UPDATE )
	{
		newLine->m_LineID = this->m_UpdateLineID;
		if( !entryConfig->UpdateLine(newLine) )
			return;
	}

	//成功加载，则关闭窗口
	CDialog::OnOK();
}

// AddEntryDialog message handlers

} // end of entry

} // end of assistant

} // end of guch

} // end of com