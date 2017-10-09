// EntryManageDialog.cpp : implementation file
//

#include "stdafx.h"
#include "afxdialogex.h"

#include <GlobalDataConfig.h>
#include <LineTypeConfigPropertySheet.h>
#include <EntryManageDialog.h>
#include <LMAException.h>
#include <LMAUtils.h>
#include <LineConfigDataManager.h>
#include <AddEntryDialog.h>
#include <ArxWrapper.h>

using namespace com::guch::assistant::data;
using namespace com::guch::assistant::exception;
using namespace com::guch::assistant::config;

namespace com
{

namespace guch
{

namespace assistant
{

namespace entry
{

// EntryManageDialog dialog

IMPLEMENT_DYNAMIC(EntryManageDialog, CDialog)

EntryManageDialog::EntryManageDialog(CWnd* pParent /*=NULL*/)
	: CDialog(EntryManageDialog::IDD, pParent)
{
	//得到当前管理的文档
	m_fileName = curDoc()->fileName();

	//得到实体数据文件中的数据
	m_EntryFile = LineEntryFileManager::RegisterEntryFile(m_fileName);
}

BOOL EntryManageDialog::OnInitDialog()
{
	//和页面交互数据
	CDialog::OnInitDialog();

	this->InitEntryList();

	this->InitEntryDetailInfo();

	this->InitEntryDetailHeader();

	return TRUE;
}

BOOL EntryManageDialog::InitEntryList()
{
	acutPrintf(L"初始化管线实例数据.\n");

#ifdef _DEMO_DATA
	HTREEITEM hKindItem ,hCatogreyItem,kLineItem;

	//在根结点上添加"管线"
	hKindItem = m_LinesTree.InsertItem(L"管线",TVI_ROOT);

	//在“管线”下面插入分类数据
	hCatogreyItem = m_LinesTree.InsertItem(L"水管",hKindItem);

	//插入具体管线
	kLineItem = m_LinesTree.InsertItem(L"水管#1",hCatogreyItem);
	kLineItem = m_LinesTree.InsertItem(L"水管#2",hCatogreyItem,kLineItem);

	//插入其他种类
	hCatogreyItem = m_LinesTree.InsertItem(L"暖气",hKindItem,hCatogreyItem);//在Parent1上添加一个子结点，排在Child1_1后面
	hCatogreyItem = m_LinesTree.InsertItem(L"电线",hKindItem,hCatogreyItem);

	hKindItem = m_LinesTree.InsertItem(L"阻隔体",TVI_ROOT,hKindItem);   

	//在“阻隔体”下面插入分类数据
	hCatogreyItem = m_LinesTree.InsertItem(L"巷道",hKindItem);

	//插入具体管线
	kLineItem = m_LinesTree.InsertItem(L"巷道#1",hCatogreyItem);
	kLineItem = m_LinesTree.InsertItem(L"巷道#2",hCatogreyItem,kLineItem);

#else

	//在根结点上添加"管线"
	m_lineRoot = m_LinesTree.InsertItem(L"管线",TVI_ROOT);

	//在根结点上添加"阻隔体"
	m_blockRoot = m_LinesTree.InsertItem(L"阻隔体",TVI_ROOT, m_lineRoot);   

	LineList* lineList =  m_EntryFile->GetList();

	//初始化左边栏树形数据
	for( LineIterator iter = lineList->begin();
			iter != lineList->end();
			iter++)
	{
		InsertLine((*iter),TRUE);
	}

	//默认展开根节点
	m_LinesTree.Expand(m_lineRoot, TVE_EXPAND);
	m_LinesTree.Expand(m_blockRoot, TVE_EXPAND);

#endif
	return TRUE;
}

BOOL EntryManageDialog::InitEntryDetailInfo()
{
	//控件不可编辑
	m_LineNO.EnableWindow(FALSE);
	m_LineName.EnableWindow(FALSE);
	m_LineKind.EnableWindow(FALSE);

	return TRUE;
}

HTREEITEM EntryManageDialog::GetKindNode( const wstring& category, const wstring& kind)
{
	TVITEM item;

	HTREEITEM hCurrent,hParent;

	if( category == GlobalData::KIND_LINE )
	{
		hParent = m_lineRoot;
	}
	else if ( category == GlobalData::KIND_SEPERATOR )
	{
		hParent = m_blockRoot;
	}

	hCurrent = m_LinesTree.GetNextItem(hParent, TVGN_CHILD);
	while (hCurrent != NULL) 
	{
	   // Get the text for the item. Notice we use TVIF_TEXT because
	   // we want to retrieve only the text, but also specify TVIF_HANDLE
	   // because we're getting the item by its handle.
	   TCHAR szText[1024];
	   item.hItem = hCurrent;
	   item.mask = TVIF_TEXT | TVIF_HANDLE;
	   item.pszText = szText;
	   item.cchTextMax = 1024;

	   BOOL bWorked = m_LinesTree.GetItem(&item);

	   // Try to get the next item
	   hCurrent = m_LinesTree.GetNextItem(hCurrent, TVGN_CHILD);

	   // If we successfuly retrieved an item, and the item's text
	   // is the line kind
	   if (bWorked && wstring(item.pszText) == kind)
	   {
#ifdef DEBUG
		   acutPrintf(L"在树上找到了这种类型【%s】\n",kind);
#endif
		  //m_LinesTree.DeleteItem(item.hItem);
		  return item.hItem;
	   }
	}

	//not find the kind, creat a new kind
#ifdef DEBUG
	acutPrintf(L"在树上没有找到了这种类型【%s】，得创建\n",kind);
#endif

	hCurrent = m_LinesTree.InsertItem(kind.c_str(),hParent);
	return hCurrent;
}

HTREEITEM EntryManageDialog::FindKindNode( const UINT& lineID)
{
	HTREEITEM hCurrent = m_LinesTree.GetRootItem();
	while (hCurrent != NULL) 
	{
	   // Get the ID for the item.
	   UINT currentID = (UINT)m_LinesTree.GetItemData(hCurrent);

	   // Check the ID
	   if( currentID == lineID)
	   {
#ifdef DEBUG
		   acutPrintf(L"在树上找到了这种ID【%d】\n",lineID);
#endif
		  //m_LinesTree.DeleteItem(item.hItem);
		  return hCurrent;
	   }

	   // Try to get the next visible item
	   hCurrent = m_LinesTree.GetNextItem(hCurrent, TVGN_NEXTVISIBLE);
	}

	return hCurrent;
}

BOOL EntryManageDialog::InsertLine( LineEntry* lineEntry, BOOL bInitialize )
{
	//判断其合法性
	try
	{
		if( !lineEntry )
		{
			CString errorMsg;
			errorMsg.Format(L"数据为空");

			throw ErrorException(errorMsg.GetBuffer());
		}

		LineEntry* pLine = NULL;
		if( !bInitialize )
		{
			if( pLine = m_EntryFile->FindLineByNO( lineEntry->m_LineNO ) )
			{
				CString errorMsg;
				errorMsg.Format(L"管线【%s】也是这个编号，换一个吧",pLine->m_LineName.c_str());

				throw ErrorException(errorMsg.GetBuffer());
			}

			if( pLine = m_EntryFile->FindLineByName( lineEntry->m_LineName ) )
			{
				CString errorMsg;
				errorMsg.Format(L"管线【%s】也是这个名字，换一个吧",pLine->m_LineNO.c_str());

				throw ErrorException(errorMsg.GetBuffer());
			}
		}
	}
	catch(const ErrorException& e)
	{
		MessageBoxW(e.errMsg.c_str(), GlobalData::ERROR_DIALOG_CAPTION.c_str(), MB_OK);
		return FALSE;
	}

	//得到其种类
	wstring category = LineConfigDataManager::Instance()->FindCategory(lineEntry->m_LineKind);

	HTREEITEM hCatogery;

	if( category == GlobalData::KIND_LINE )
	{
		hCatogery = m_lineRoot;
	}
	else if ( category == GlobalData::KIND_SEPERATOR )
	{
		hCatogery = m_blockRoot;
	}

	//得到其节点
	HTREEITEM kindNode = GetKindNode(category, lineEntry->m_LineKind);

	//插入该实体名称
	HTREEITEM newItem = m_LinesTree.InsertItem(lineEntry->m_LineName.c_str(), kindNode);

	//默认插入之后，默认打开该节点的父节点
	m_LinesTree.Expand(kindNode, TVE_EXPAND);
	
	//保持本节点可见
	m_LinesTree.EnsureVisible(newItem);

	//从数据文件初始化
	if( !bInitialize )
	{		
		//生成该项的ID
		lineEntry->m_LineID = (UINT)GetTickCount();

		//保存数据到管理器
		m_EntryFile->InsertLine(lineEntry);

		//保存到数据库
		ArxWrapper::PostToNameObjectsDict(lineEntry,LineEntry::LINE_ENTRY_LAYER);

		//保存到导出文件
		m_EntryFile->Persistent();
	}

	//设置改项的ID
	m_LinesTree.SetItemData(newItem,(DWORD_PTR)lineEntry->m_LineID);

	return TRUE;
}

BOOL EntryManageDialog::UpdateLine( LineEntry* lineEntry )
{
	//判断其合法性
	try
	{
		if( !lineEntry )
		{
			CString errorMsg;
			errorMsg.Format(L"数据为空");

			throw ErrorException(errorMsg.GetBuffer());
		}

		LineEntry* pAnotherLine = NULL;

		if( pAnotherLine = m_EntryFile->HasAnotherLineByNO( lineEntry->m_LineID, lineEntry->m_LineNO ) )
		{
			CString errorMsg;
			errorMsg.Format(L"管线【%s】也是这个编号，换一个吧",pAnotherLine->m_LineName.c_str());

			throw ErrorException(errorMsg.GetBuffer());
		}

		if( pAnotherLine = m_EntryFile->HasAnotherLineByByName( lineEntry->m_LineID, lineEntry->m_LineName ) )
		{
			CString errorMsg;
			errorMsg.Format(L"管线【%s】也是这个名字，换一个吧",pAnotherLine->m_LineNO.c_str());

			throw ErrorException(errorMsg.GetBuffer());
		}
	}
	catch(const ErrorException& e)
	{
		MessageBoxW(e.errMsg.c_str(), GlobalData::ERROR_DIALOG_CAPTION.c_str(), MB_OK);
		return FALSE;
	}

	//得到其种类
	wstring category = LineConfigDataManager::Instance()->FindCategory(lineEntry->m_LineKind);

	//得到其节点
	HTREEITEM hItem = FindKindNode(lineEntry->m_LineID);

	//打开父节点
	HTREEITEM hParent = m_LinesTree.GetParentItem(hItem);
	if (hParent != NULL)
		m_LinesTree.Expand(hParent, TVE_EXPAND);
	
	//保持本节点可见
	m_LinesTree.EnsureVisible(hItem);

	//设置新名称
	if( hItem )
	{
		m_LinesTree.SetItemText(hItem, lineEntry->m_LineName.c_str());

		//保存数据
		m_EntryFile->UpdateLine(lineEntry);

		//保存到数据库
		ArxWrapper::PostToNameObjectsDict(lineEntry,lineEntry->LINE_ENTRY_LAYER);

		//保存到导出文件
		m_EntryFile->Persistent();

		//更新详细信息
		InitEntryPointsData();
	}

	return TRUE;
}

BOOL EntryManageDialog::InitEntryDetailHeader()
{
#ifdef DEBUG
	acutPrintf(L"初始化管线中点坐标的数据.\n");
#endif

	int index = 0;

	//插入各列
	m_LineDetailList.InsertColumn(0, L"编号", LVCFMT_LEFT, 80);
	m_LineDetailList.InsertColumn(1, L"X坐标", LVCFMT_LEFT, 100);
	m_LineDetailList.InsertColumn(2, L"Y坐标", LVCFMT_LEFT, 100);
	m_LineDetailList.InsertColumn(3, L"高程", LVCFMT_LEFT, 100);

	//编号栏只读
	m_LineDetailList.SetColumnReadOnly(0);

	//默认编辑框编辑
	m_LineDetailList.SetDefaultEditor(NULL, NULL, &m_PointEdit);

	/*
	LVCOLUMN lvColumn;

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_CENTER;
	lvColumn.cx = 80;
	lvColumn.pszText = L"编号";
	this->m_LineDetailList.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 100;
	lvColumn.pszText = L"X坐标";
	m_LineDetailList.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 100;
	lvColumn.pszText = L"Y坐标";
	m_LineDetailList.InsertColumn(index++, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 100;
	lvColumn.pszText = L"高程";
	m_LineDetailList.InsertColumn(index++, &lvColumn);
	*/

	m_LineDetailList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	return TRUE;
}

BOOL EntryManageDialog::InitEntryPointsData(LineEntry* lineEntry)
{
	m_LineDetailList.DeleteAllItems();

	acutPrintf(L"初始化坐标信息.\n");

	if( lineEntry && lineEntry->m_PointList )
	{
		int index = 0;
		for( PointIter iter = lineEntry->m_PointList->begin();
			iter != lineEntry->m_PointList->end();
			iter++)
		{
			if(*iter)
			{
				CString temp;

				//编号
				temp.Format(L"%d",index+1);
				m_LineDetailList.InsertItem(index,temp);

				//x
				temp.Format(L"%0.2f",(*iter)->m_Point[X]);
				m_LineDetailList.SetItemText(index, 1, temp);

				//y
				temp.Format(L"%0.2f",(*iter)->m_Point[Y]);
				m_LineDetailList.SetItemText(index, 2, temp);
				
				//z
				temp.Format(L"%0.2f",(*iter)->m_Point[Z]);
				m_LineDetailList.SetItemText(index, 3, temp);

				index++;
			}
		}
	}

	UpdateData(FALSE);

	return TRUE;
}

EntryManageDialog::~EntryManageDialog()
{
#ifdef DEBUG
	if( m_EntryFile )
		acutPrintf(L"数据文件【%s】实体管理对话框关闭了.\n",m_EntryFile->m_FileName.c_str());
#endif
}

void EntryManageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_LINES, m_LinesTree);

	DDX_Control(pDX, IDC_EDIT_NO, m_LineNO);
	DDX_Control(pDX, IDC_EDIT_NAME, m_LineName);
	DDX_Control(pDX, IDC_EDIT_KIND, m_LineKind);

	DDX_Control(pDX, IDC_EDIT_POINT, m_PointEdit);

	DDX_Control(pDX, IDC_LIST_LINE_DETAIL, m_LineDetailList);
}

LineEntry* EntryManageDialog::GetLineEntry( const UINT& ID )
{
	return m_EntryFile->FindLine(ID);
}

BEGIN_MESSAGE_MAP(EntryManageDialog, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedButtonOK)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnBnClickedButtonMod)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_LINES, OnTreeSelChanged)
END_MESSAGE_MAP()

void EntryManageDialog::OnBnClickedButtonOK()
{
	PointList* newPoints = new PointList();

	CString temp;
	for( int i = 0; i < m_LineDetailList.GetItemCount(); i++ )
	{
		PointEntry* point = new PointEntry();

		//得到当前编号（及其在列表中的序列号）
		point->m_PointNO = (UINT)i;

		temp = m_LineDetailList.GetItemText(i,1);
		acdbDisToF(temp.GetBuffer(), -1, &((point->m_Point)[X]));

		temp = m_LineDetailList.GetItemText(i,2);
		acdbDisToF(temp.GetBuffer(), -1, &((point->m_Point)[Y]));

		temp = m_LineDetailList.GetItemText(i,3);
		acdbDisToF(temp.GetBuffer(), -1, &((point->m_Point)[Z]));

		//加入到队列中
		newPoints->push_back(point);
	}

	//得到当前编辑的直线
	LineEntry* selectLine = GetSelectLine();

	//设置新的数据
	if( selectLine )
	{
		selectLine->SetPoints(newPoints);
	}

	//默认进入XY视图
	acedCommand(RTSTR, _T("._-VIEW"), RTSTR, L"TOP", 0);

	//保存到临时文件
	m_EntryFile->Persistent();
}

LineEntry* EntryManageDialog::GetSelectLine()
{
	HTREEITEM selectedItem = m_LinesTree.GetSelectedItem();

	if( selectedItem == NULL)
		 return NULL;

	UINT selectedID = (UINT)m_LinesTree.GetItemData(selectedItem);

	return GetLineEntry(selectedID);
}

void EntryManageDialog::OnBnClickedButtonAdd()
{
	AddEntryDialog dlg(this);
	
	//设置操作类型
	dlg.SetOperType(AddEntryDialog::OPER_ADD);

	INT_PTR nReturnValue = dlg.DoModal();
}

void EntryManageDialog::OnBnClickedButtonMod()
{
	HTREEITEM selectedItem = m_LinesTree.GetSelectedItem();

	if( selectedItem == NULL)
		 return;

	UINT selectedID = (UINT)m_LinesTree.GetItemData(selectedItem);

	AddEntryDialog dlg(this);
	
	//设置操作类型
	dlg.SetOperType(AddEntryDialog::OPER_UPDATE);
	dlg.SetUpdateID(selectedID);

	INT_PTR nReturnValue = dlg.DoModal();
}

void EntryManageDialog::OnBnClickedButtonDel()
{
	//选择的行
	HTREEITEM selectedItem = m_LinesTree.GetSelectedItem();

	if( selectedItem == NULL)
		 return;

	UINT selectedID = (UINT)m_LinesTree.GetItemData(selectedItem);

	//得到选择的数据
	LineEntry* pEntry = m_EntryFile->FindLine(selectedID);

	if( pEntry )
	{
		// Initializes the variables to pass to the MessageBox::Show method.
		CString message;
		message.Format(L"确实要删除[%s]吗?",pEntry->m_LineName.c_str());

		LPCTSTR caption = L"删除实体";

		// Displays the MessageBox.
		int result = MessageBoxW(message, caption, MB_OKCANCEL);
		if ( result == IDOK )
		{
			//从数据库删除管线本身
			ArxWrapper::PostToNameObjectsDict(pEntry,pEntry->LINE_ENTRY_LAYER,true);

			//从数据库删除管线所有的线段
			ArxWrapper::eraseLMALine(*pEntry);

			//删除所有的内存节点
			pEntry->ClearPoints();

			//Delete from the list
			m_EntryFile->DeleteLine(selectedID);

			// delete the selected item. 
			m_LinesTree.DeleteItem(selectedItem);

			//保存到导出文件
			m_EntryFile->Persistent();
		}
	}
}

BOOL EntryManageDialog::InitEntryPointsData()
{
	//选择的行
	HTREEITEM selectedItem = m_LinesTree.GetSelectedItem();

	UINT selectedID = (UINT)m_LinesTree.GetItemData(selectedItem);

	//得到选择的数据
	LineEntry* pEntry = m_EntryFile->FindLine(selectedID);

	FillLineData(pEntry);

	return TRUE;
}

void EntryManageDialog::OnTreeSelChanged(LPNMHDR pnmhdr, LRESULT *pLResult)
{
	//如果有某一项被选中
	if( pnmhdr->code == TVN_SELCHANGED )
	{
		InitEntryPointsData();
	}
}

void EntryManageDialog::FillLineData( LineEntry* lineEntry )
{
	if( lineEntry )
	{
		//可以删除、更新
		m_LineDelete.EnableWindow(TRUE);
		m_LineUpdate.EnableWindow(TRUE);

		//设置详细数据
		m_LineNO.SetWindowText(lineEntry->m_LineNO.c_str());
		m_LineName.SetWindowText(lineEntry->m_LineName.c_str());
		m_LineKind.SetWindowText(lineEntry->m_LineKind.c_str());

		//设置坐标信息
		InitEntryPointsData(lineEntry);
	}
	else
	{
		//清空详细数据
		m_LineNO.SetWindowText(L"");
		m_LineName.SetWindowText(L"");
		m_LineKind.SetWindowText(L"");

		//可删除、更新
		m_LineDelete.EnableWindow(FALSE);
		m_LineUpdate.EnableWindow(FALSE);

		//清空数据
		m_LineDetailList.DeleteAllItems();
	}

	UpdateData(FALSE);
}

// EntryManageDialog message handlers

} // end of entry

} // end of assistant

} // end of guch

} // end of com
