// AddLineDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AddLineDialog.h"
#include "afxdialogex.h"

#include "GlobalDataConfig.h"
#include "LineTypeConfigPropertySheet.h"
#include <LMAException.h>
#include <LMAUtils.h>

using namespace com::guch::assistant::data;
using namespace com::guch::assistant::exception;

// AddBlockConfigDialog dialog
namespace com
{

namespace guch
{

namespace assistant
{

namespace config
{

IMPLEMENT_DYNAMIC(AddBlockConfigDialog, CDialog)

AddBlockConfigDialog::AddBlockConfigDialog(CWnd* pParent /*=NULL*/)
	: AddLineConfigDialog(AddBlockConfigDialog::IDD, pParent)
{
}

void AddBlockConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	AddLineConfigDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK_CAN_THROUGH, m_CanThr);
	DDX_Control(pDX, IDC_COMBO_THROUGH_DIRECTION, m_ThroughDirection);
}

BEGIN_MESSAGE_MAP(AddBlockConfigDialog, CDialog)
	ON_BN_CLICKED(IDOK, &AddBlockConfigDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_CAN_THROUGH, &AddBlockConfigDialog::OnChkCanThroughChange)
	ON_CBN_SELCHANGE(IDC_COMBO_SHAPE, &AddLineConfigDialog::OnCbnShapeChange)
END_MESSAGE_MAP()

BOOL AddBlockConfigDialog::InitConfigData()
{
	//类型
	m_LineKind.AddString(GlobalData::BLOCK_CATEGORY_WUSHUI.c_str());
	m_LineKind.AddString(GlobalData::BLOCK_CATEGORY_HANGDAO.c_str());
	m_LineKind.AddString(GlobalData::BLOCK_CATEGORY_NUANQIGUAN.c_str());
	m_LineKind.AddString(GlobalData::BLOCK_CATEGORY_GUODAO.c_str());
	m_LineKind.AddString(GlobalData::BLOCK_CATEGORY_CHEZHAN.c_str());
	m_LineKind.SetCurSel(0);

	//断面形状
	m_LineShape.InsertString(0,GlobalData::LINE_SHAPE_CIRCLE.c_str());
	m_LineShape.InsertString(1,GlobalData::LINE_SHAPE_SQUARE.c_str());
	m_LineShape.SetCurSel(0);

	//单位
	m_LineUnit.AddString(GlobalData::LINE_UNIT_MM.c_str());
	m_LineUnit.AddString(GlobalData::LINE_UNIT_CM.c_str());
	m_LineUnit.AddString(GlobalData::LINE_UNIT_M.c_str());
	m_LineUnit.SetCurSel(0);

	//名称
	m_LineName.SetWindowTextW(L"");

	//大小
	m_LineRadius.SetWindowTextW(L"0");
	m_LineLength.SetWindowTextW(L"0");
	m_LineWidth.SetWindowTextW(L"0");

	//壁厚
	m_LineWallSize.SetWindowTextW(L"0");

	//安全范围
	m_LineSafeSize.SetWindowTextW(L"0");

	//可否穿越
	m_CanThr.SetCheck( BST_UNCHECKED );

	//穿越方向
	m_ThroughDirection.InsertString(0,GlobalData::BLOCK_CATEGORY_ABOVE.c_str());
	m_ThroughDirection.InsertString(1,GlobalData::BLOCK_CATEGORY_BELLOW.c_str());
	m_ThroughDirection.SetCurSel(-1);

	//描述
	m_LineDesc.SetWindowText(L"");

	if( m_OperType == OPER_UPDATE )
	{
		//填充数据
		FillUpdateData();
	}

	//动态控制控件
	ShowControlDynamic();

	return TRUE;
}

void AddBlockConfigDialog::FillUpdateData()
{
	//更改控件的标题
	this->SetWindowTextW(L"阻隔体更新");

	//Get the parent windows
	LineConfigDialog* lineConfig 
		= dynamic_cast<LineConfigDialog*>(this->m_pParentWnd);

	if( !lineConfig )
		return;

	LineCategoryItemData configData;
	
	//得到选择的数据
	lineConfig->GetSelectData(configData);

	//填充数据
	m_lineIndex = configData.mIndex;
	m_lineID = configData.mID;

	m_LineName.SetWindowTextW(configData.mName.c_str());
	m_LineKind.SelectString(0,configData.mKind.c_str());

	m_LineShape.SelectString(0,configData.mShape.c_str());
	m_LineUnit.SelectString(0,configData.mUnit.c_str());

	m_LineRadius.SetWindowTextW(configData.mRadius.c_str());
	m_LineLength.SetWindowTextW(configData.mLength.c_str());
	m_LineWidth.SetWindowTextW(configData.mWidth.c_str());

	m_LineWallSize.SetWindowTextW(configData.mWallSize.c_str());
	m_LineSafeSize.SetWindowTextW(configData.mSafeSize.c_str());

	if( configData.mCanThrough == L"是" )
	{
		m_CanThr.SetCheck( BST_CHECKED );
		m_ThroughDirection.SelectString(0,configData.mThroughDirection.c_str());
	}
	else if( configData.mCanThrough == L"否" )
	{
		m_CanThr.SetCheck( BST_UNCHECKED );
		m_ThroughDirection.SetCurSel(-1);
	}

	m_LineDesc.SetWindowTextW(configData.mComment.c_str());

	UpdateData(FALSE);
}

AddBlockConfigDialog::~AddBlockConfigDialog()
{
}

void AddBlockConfigDialog::OnBnClickedOk()
{
	//得到用户输入的数据
	UpdateData(TRUE);

	CString lineKind,lineName,lineShape,lineRadius,lineLength,lineWidth,lineUnit,lineSafeSize,lineWallSize,blockCanThr,blockThrDirection,lineDesc;

	m_LineName.GetWindowTextW(lineName);
	m_LineKind.GetWindowTextW(lineKind);

	m_LineShape.GetWindowTextW(lineShape);
	m_LineUnit.GetWindowTextW(lineUnit);

	m_LineRadius.GetWindowTextW(lineRadius);
	m_LineLength.GetWindowTextW(lineLength);
	m_LineWidth.GetWindowTextW(lineWidth);

	m_LineWallSize.GetWindowTextW(lineWallSize);
	m_LineSafeSize.GetWindowTextW(lineSafeSize);

	if( m_CanThr.GetCheck() == BST_CHECKED )
	{
		blockCanThr = L"是";
	}
	else if(  m_CanThr.GetCheck() == BST_UNCHECKED)
	{
		blockCanThr = L"否";
	}

	m_ThroughDirection.GetWindowTextW(blockThrDirection);

	m_LineDesc.GetWindowTextW(lineDesc);

	//准备配置数据结构体
	LineCategoryItemData categoryData;

	categoryData.mIndex = m_lineIndex;
	categoryData.mID = m_lineID;

	categoryData.mCategory = GlobalData::KIND_SEPERATOR;

	categoryData.mName = lineName;
	categoryData.mKind = lineKind;

	categoryData.mUnit = lineUnit;
	categoryData.mShape = lineShape;

	categoryData.mRadius = lineRadius;
	categoryData.mLength = lineLength;
	categoryData.mWidth = lineWidth;

	categoryData.mWallSize = lineWallSize;
	categoryData.mSafeSize = lineSafeSize;

	categoryData.mCanThrough = blockCanThr;
	categoryData.mThroughDirection = blockThrDirection;

	categoryData.mComment = lineDesc;

	try
	{
		//调用父窗口进行操作
		LineConfigDialog* lineConfig 
				= dynamic_cast<LineConfigDialog*>(this->m_pParentWnd);

		if( lineConfig == NULL )
		{
			acutPrintf(L"!!不能找到配置主窗口.\n");
		}

		//增加或更新管线配置
		if( m_OperType == OPER_ADD )
		{
			acutPrintf(L"增加管线类型.\n");

			lineConfig->CheckValid(categoryData,true);
			lineConfig->InsertLine(categoryData,false);
		}
		else if( m_OperType == OPER_UPDATE )
		{
			acutPrintf(L"更新管线类型.\n");

			lineConfig->CheckValid(categoryData,false);
			lineConfig->UpdateLine(categoryData);
		}

		CDialog::OnOK();
	}
	catch(ErrorException& e)
	{
		MessageBoxW(e.errMsg.c_str(), GlobalData::ERROR_DIALOG_CAPTION.c_str(), MB_OKCANCEL);
	}
}

afx_msg void AddBlockConfigDialog::OnChkCanThroughChange()
{
	ShowControlDynamic();
}

void AddBlockConfigDialog::ShowControlDynamic()
{
	AddLineConfigDialog::ShowControlDynamic();

	if( m_CanThr.GetCheck() == BST_CHECKED )
	{
		m_ThroughDirection.EnableWindow(TRUE);
	}
	else if(  m_CanThr.GetCheck() == BST_UNCHECKED)
	{
		m_ThroughDirection.EnableWindow(FALSE);
	}
}

} // end of config

} // end of assistant

} // end of guch

} // end of com
