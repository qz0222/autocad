// BlockConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "resource.h"

#include <GlobalDataConfig.h>
#include <LineConfigDialog.h>
#include <LineConfigDataManager.h>
#include <LMAException.h>

using namespace com::guch::assistant::data;
using namespace com::guch::assistant::exception;

namespace com
{

namespace guch
{

namespace assistant
{

namespace config
{

// BlockConfigDialog dialog

IMPLEMENT_DYNAMIC(BlockConfigDialog, LineConfigDialog)

BlockConfigDialog::BlockConfigDialog(CWnd* pParent /*=NULL*/)
: LineConfigDialog(pParent)
{	
	//重设标题
	this->m_psp.dwFlags |= PSP_USETITLE;
	this->m_psp.pszTitle = L"阻隔体管理";
}

BlockConfigDialog::~BlockConfigDialog()
{
}

BOOL BlockConfigDialog::OnInitDialog()
{
	//和页面交互数据
	CPropertyPage::OnInitDialog();

	this->InitLineHeader();

	this->InitLineData();

	return TRUE; // return TRUE unless you set the focus to a control
}

BOOL BlockConfigDialog::InitLineHeader()
{
	acutPrintf(L"初始化阻隔体配置数据.\n");
	int index = 0;

	LVCOLUMN lvColumn;

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_CENTER;
	lvColumn.cx = 40;
	lvColumn.pszText = L"序号";
	//m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 120;
	lvColumn.pszText = L"名称";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_CENTER;
	lvColumn.cx = 60;
	lvColumn.pszText = L"类型";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 60;
	lvColumn.pszText = L"形状";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 60;
	lvColumn.pszText = L"大小";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 50;
	lvColumn.pszText = L"壁厚";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 60;
	lvColumn.pszText = L"安全范围";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 60;
	lvColumn.pszText = L"长度单位";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = L"是否可穿越";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 60;
	lvColumn.pszText = L"穿越方向";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 120;
	lvColumn.pszText = L"描述";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	m_lineConfig.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	return TRUE;
}

void BlockConfigDialog::UpdateUILineData(const LineCategoryItemData& itemData)
{
	//根据ID更新
	int nItem = FindItem(itemData.mID);
	if( nItem == -1 )
		return;

	//更新页面数据
	int columIndex = 0;
	m_lineConfig.SetItemText(nItem, columIndex++, itemData.mName.c_str());

	m_lineConfig.SetItemText(nItem, columIndex++, itemData.mKind.c_str());
	m_lineConfig.SetItemText(nItem, columIndex++, itemData.mShape.c_str());

	wstring size;
	
	if( itemData.mShape == GlobalData::LINE_SHAPE_CIRCLE )
	{
		size = itemData.mRadius;
	}
	else if( itemData.mShape == GlobalData::LINE_SHAPE_SQUARE )
	{
		size = itemData.mLength + L"x" + itemData.mWidth;
	}

	m_lineConfig.SetItemText(nItem, columIndex++, size.c_str());
	m_lineConfig.SetItemText(nItem, columIndex++, itemData.mWallSize.c_str());
	m_lineConfig.SetItemText(nItem, columIndex++, itemData.mSafeSize.c_str());
	m_lineConfig.SetItemText(nItem, columIndex++, itemData.mUnit.c_str());

	m_lineConfig.SetItemText(nItem, columIndex++, itemData.mCanThrough.c_str());
	m_lineConfig.SetItemText(nItem, columIndex++, itemData.mThroughDirection.c_str());

	m_lineConfig.SetItemText(nItem, columIndex++, itemData.mComment.c_str());

	UpdateData(FALSE);
}

BOOL BlockConfigDialog::InitLineData()
{
	const LineCategoryVecotr* lineCategoryData
		 = LineConfigDataManager::Instance()->GetData();

	typedef vector<LineCategoryItemData*>::const_reverse_iterator DataIterator;

	for( DataIterator iter = lineCategoryData->rbegin(); 
			iter != lineCategoryData->rend(); 
			iter++)
	{
		LineCategoryItemData* data = *iter;

		if( data && data->mCategory == GlobalData::KIND_SEPERATOR )
		{
			InsertLine(*data,true);
		}
	}

	UpdateData(FALSE);
	return TRUE;
}

BEGIN_MESSAGE_MAP(BlockConfigDialog, CPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LINE_CONFIG, OnItemChangedList)
	ON_LBN_DBLCLK(IDC_LIST_LINE_CONFIG,OnBnClickedButtonMod)
END_MESSAGE_MAP()

void BlockConfigDialog::OnBnClickedButtonAdd()
{
	// TODO: Add your control notification handler code here
	AddBlockConfigDialog dlg(this);
	
	//设置操作类型
	dlg.SetOperType(AddBlockConfigDialog::OPER_ADD);

	INT_PTR nReturnValue = dlg.DoModal();
}

void BlockConfigDialog::OnBnClickedButtonMod()
{
	// TODO: Add your control notification handler code here
	AddBlockConfigDialog dlg(this);
	
	//设置操作类型
	dlg.SetOperType(AddBlockConfigDialog::OPER_UPDATE);

	dlg.DoModal();
}

BOOL BlockConfigDialog::GetItemData( int item, LineCategoryItemData& configData)
{
	//得到要填充的数据
	configData.mIndex = item;

	//得到行的ID
	DWORD_PTR itemData = m_lineConfig.GetItemData(item);
	configData.mID = (UINT)itemData;

	//为管线类型
	configData.mCategory = GlobalData::KIND_LINE;

	//得到每一列的数据
	int columIndex = 0;

	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = item;
	lvItem.iSubItem = columIndex++;

	TCHAR szBuffer[255];
	DWORD cchBuf(GlobalData::ITEM_TEXT_MAX_LENGTH);

	lvItem.pszText = szBuffer;
	lvItem.cchTextMax = cchBuf;

	//名字
	if( m_lineConfig.GetItem(&lvItem) )
		configData.mName = lvItem.pszText;

	//类型
	lvItem.iSubItem = columIndex++;
	if( m_lineConfig.GetItem(&lvItem) )
		configData.mKind = lvItem.pszText;

	//形状
	lvItem.iSubItem = columIndex++;
	if( m_lineConfig.GetItem(&lvItem) )
		configData.mShape = lvItem.pszText;

	//大小
	lvItem.iSubItem = columIndex++;
	if( m_lineConfig.GetItem(&lvItem) )
	{
		if( configData.mShape == GlobalData::LINE_SHAPE_CIRCLE )
		{
			configData.mRadius = lvItem.pszText;
		}
		else if( configData.mShape == GlobalData::LINE_SHAPE_SQUARE )
		{
			wstring size = lvItem.pszText;
			size_t xPos = size.find_first_of(L"x");

			configData.mLength = size.substr(0,xPos);
			configData.mWidth = size.substr(xPos+1);
		}
	}
	
	//壁厚
	lvItem.iSubItem = columIndex++;
	if( m_lineConfig.GetItem(&lvItem) )
		configData.mWallSize = lvItem.pszText;

	//安全范围
	lvItem.iSubItem = columIndex++;
	if( m_lineConfig.GetItem(&lvItem) )
		configData.mSafeSize = lvItem.pszText;

	//长度单位
	lvItem.iSubItem = columIndex++;
	if( m_lineConfig.GetItem(&lvItem) )
		configData.mUnit = lvItem.pszText;

	//是否可穿越
	lvItem.iSubItem = columIndex++;
	if( m_lineConfig.GetItem(&lvItem) )
		configData.mCanThrough = lvItem.pszText;

	//穿越方向
	lvItem.iSubItem = columIndex++;
	if( m_lineConfig.GetItem(&lvItem) )
		configData.mThroughDirection = lvItem.pszText;

	//备注
	lvItem.iSubItem = columIndex++;
	if( m_lineConfig.GetItem(&lvItem) )
		configData.mComment = lvItem.pszText;

	return TRUE;
}

} // end of config

} // end of assistant

} // end of guch

} // end of com

// BlockConfigDialog message handlers
