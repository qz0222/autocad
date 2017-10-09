// LineConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "resource.h"

#include <GlobalDataConfig.h>
#include <LineConfigDialog.h>
#include <LineConfigDataManager.h>
#include <LMAException.h>
#include <LineTypeConfigPropertySheet.h>

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

// LineConfigDialog dialog

IMPLEMENT_DYNAMIC(LineConfigDialog, CPropertyPage)

LineConfigDialog::LineConfigDialog(CWnd* pParent /*=NULL*/)
: CPropertyPage(LineConfigDialog::IDD)
{
	//重设标题
	this->m_psp.dwFlags |= PSP_USETITLE;
	this->m_psp.pszTitle = L"管线管理";
}

LineConfigDialog::~LineConfigDialog()
{
}

void LineConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LINE_CONFIG, m_lineConfig);
}

BOOL LineConfigDialog::OnInitDialog()
{
	//和页面交互数据
	CPropertyPage::OnInitDialog();

	this->InitLineHeader();

	this->InitLineData();

	return TRUE; // return TRUE unless you set the focus to a control
}

BOOL LineConfigDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	//acutPrintf(L"收到Notify数据WPARAM[%ld] LPARAM[%ld].\n",wParam,lParam);

	return CPropertyPage::OnNotify(wParam,lParam,pResult);
}

BOOL LineConfigDialog::InitLineHeader()
{
	acutPrintf(L"初始化管线配置数据.\n");
	int index = 0;

	LVCOLUMN lvColumn;

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_CENTER;
	lvColumn.cx = 40;
	lvColumn.pszText = L"序号";
	//m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 140;
	lvColumn.pszText = L"名称";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_CENTER;
	lvColumn.cx = 80;
	lvColumn.pszText = L"类型";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = L"形状";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 60;
	lvColumn.pszText = L"大小";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 60;
	lvColumn.pszText = L"壁厚";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = L"安全范围";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 80;
	lvColumn.pszText = L"长度单位";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 150;
	lvColumn.pszText = L"描述";
	m_lineConfig.InsertColumn(index++, &lvColumn);

	m_lineConfig.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	return TRUE;
}

/**
 *
 **/
BOOL LineConfigDialog::InsertLine(LineCategoryItemData& itemData,BOOL initialize)
{
	//新线段得到其ID
	if( !initialize )
	{
		itemData.mID = LineConfigDataManager::Instance()->GetNextID();
	}

	LVITEM lvItem;

	//插入新行
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = 0;
	lvItem.iSubItem = 0;
	lvItem.pszText = const_cast<wchar_t*>(itemData.mName.c_str());
	m_lineConfig.InsertItem(&lvItem);

	//设置每一行的ID
	m_lineConfig.SetItemData(lvItem.iItem,itemData.mID);

	//更新其他列的数据
	UpdateUILineData(itemData);

	//非初始化阶段，插入新数据
	if( !initialize )
		LineConfigDataManager::Instance()->AddLine(&itemData);

	return TRUE;
}

/**
 *
 **/
BOOL LineConfigDialog::UpdateLine(const LineCategoryItemData& itemData)
{
	UpdateUILineData(itemData);

	//更新数据结构
	LineConfigDataManager::Instance()->UpdateLine(&itemData);

	return TRUE;
}

void LineConfigDialog::UpdateUILineData(const LineCategoryItemData& itemData)
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

	m_lineConfig.SetItemText(nItem, columIndex++, itemData.mComment.c_str());

	UpdateData(FALSE);
}

/**
 *
 **/
void LineConfigDialog::CheckValid( const LineCategoryItemData& item, BOOL bNew )
{
	//检验新行是否可用
	const LineCategoryVecotr* lineCategoryData
		 = LineConfigDataManager::Instance()->GetData();

	typedef vector<LineCategoryItemData*>::const_iterator DataIterator;

	for( DataIterator iter = lineCategoryData->begin(); 
			iter != lineCategoryData->end(); 
			iter++)
	{
		if( bNew && (*iter)->mName == item.mName
			|| !bNew && (*iter)->mID != item.mID && (*iter)->mName == item.mName )
		{
			CString msg;
			msg.Format(L"管线类型【%s】已存在，请换个名字吧。",item.mName);
			throw ErrorException(msg.GetBuffer());
		}
	}
}

BOOL LineConfigDialog::InitLineData()
{
	const LineCategoryVecotr* lineCategoryData
		 = LineConfigDataManager::Instance()->GetData();

	typedef vector<LineCategoryItemData*>::const_reverse_iterator DataIterator;

	for( DataIterator iter = lineCategoryData->rbegin(); 
			iter != lineCategoryData->rend(); 
			iter++)
	{
		LineCategoryItemData* data = *iter;

		if( data && data->mCategory == GlobalData::KIND_LINE )
		{
			InsertLine(*data,true);
		}
	}

	UpdateData(FALSE);
	return TRUE;
}

BEGIN_MESSAGE_MAP(LineConfigDialog, CPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LINE_CONFIG, OnItemChangedList)
	ON_LBN_DBLCLK(IDC_LIST_LINE_CONFIG,OnBnClickedButtonMod)
END_MESSAGE_MAP()

void LineConfigDialog::OnBnClickedButtonAdd()
{
	// TODO: Add your control notification handler code here
	AddLineConfigDialog dlg(this);
	
	//设置操作类型
	dlg.SetOperType(AddLineConfigDialog::OPER_ADD);

	INT_PTR nReturnValue = dlg.DoModal();
}

void LineConfigDialog::OnBnClickedButtonMod()
{
	// TODO: Add your control notification handler code here
	AddLineConfigDialog dlg(this);
	
	//设置操作类型
	dlg.SetOperType(AddLineConfigDialog::OPER_UPDATE);

	dlg.DoModal();
}

BOOL LineConfigDialog::GetSelectData( LineCategoryItemData& configData )
{
	//选择的行
	int item = m_lineConfig.GetSelectionMark();
	if( item == -1 )
		return FALSE;

	GetItemData(item,configData);

	return TRUE;
}

BOOL LineConfigDialog::GetItemData( int item, LineCategoryItemData& configData)
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

	//备注
	lvItem.iSubItem = columIndex++;
	if( m_lineConfig.GetItem(&lvItem) )
		configData.mComment = lvItem.pszText;

	return TRUE;
}

void LineConfigDialog::OnBnClickedButtonDel()
{
	// TODO: Add your control notification handler code here

	//选择的行
	int item = m_lineConfig.GetSelectionMark();
	if( item == -1 )
		return;

	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = item;
	lvItem.iSubItem = 0;

	TCHAR szBuffer[255];
	DWORD cchBuf(GlobalData::ITEM_TEXT_MAX_LENGTH);

	lvItem.pszText = szBuffer;
	lvItem.cchTextMax = cchBuf;

	if( m_lineConfig.GetItem(&lvItem) )
	{
		// Initializes the variables to pass to the MessageBox::Show method.
		CString message;
		message.Format(L"确实要删除[%s]吗?",lvItem.pszText);

		LPCTSTR caption = L"删除配置";

		// Displays the MessageBox.
		int result = MessageBoxW(message, caption, MB_OKCANCEL);
		if ( result == IDOK )
		{
			//Delete from the list
			LineConfigDataManager::Instance()->DeleteLine( (UINT)m_lineConfig.GetItemData(item) );

			// Closes the parent form. 
			m_lineConfig.DeleteItem(item);
		}
	}
}

void LineConfigDialog::OnBnClickedButtonOK()
{
	//acutPrintf(L"保存管线类型数据到文件.\n");

	/*
	for(int i = 0; i < m_lineConfig.GetItemCount(); i++)
	{
		LineConfigData configData;
		GetItemData(i,configData);

		LineCategoryItemData* newData = new LineCategoryItemData();

		newData->mID = configData.mID;
		newData->mName = configData.mName;
		newData->mKind = configData.mKind;
		newData->mCategory = configData.mCategory;
		newData->mShape = configData.mShape;
		newData->mSize = configData.mSize;
		newData->mEffectSize = configData.mSafeSize;
		newData->mUnit = configData.mUnit;
		newData->mComment = configData.mDesc;

		LineConfigDataManager::Instance()->AddLine(newData);
	}
	*/
}

int LineConfigDialog::FindItem( UINT id ) const
{
	for( int i = 0; i < this->m_lineConfig.GetItemCount(); i++ )
	{
		if( id == (UINT) m_lineConfig.GetItemData(i) )
			return i;
	}

	return -1;
}

void LineConfigDialog::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    //if (pNMListView->uChanged & LVIF_STATE)
	{
		LineTypeConfigPropertySheet* pParentSheet =
			dynamic_cast<LineTypeConfigPropertySheet*>(this->GetParent());

		UINT selectCnt = m_lineConfig.GetSelectedCount();

		if( selectCnt > 0 )
		{
			//选中则可用
			pParentSheet ? pParentSheet->ChangeControlStatus(TRUE) : NULL;
		}
		else
		{
			//没选中则不可用
			pParentSheet ? pParentSheet->ChangeControlStatus(FALSE) : NULL;
		}
		/*
        if(pNMListView->uNewState & LVNI_SELECTED))
		{
			
		}
		*/
	}
}

} // end of config

} // end of assistant

} // end of guch

} // end of com

// LineConfigDialog message handlers
