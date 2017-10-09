#pragma once

#include "afxcmn.h"
#include <resource.h>

#include <dbsymtb.h>
#include <dbapserv.h>
#include <adslib.h>
#include <adui.h>
#include <acui.h>

#include <string>
#include <LineEntryData.h>
#include <ListCtrlEx.h>

using namespace std;

using namespace com::guch::assistant::data;

namespace com
{

namespace guch
{

namespace assistant
{

namespace entry
{

//实体管理窗口
class EntryManageDialog : public CDialog
{
	DECLARE_DYNAMIC(EntryManageDialog)

public:
	EntryManageDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~EntryManageDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_ENTRY_MANAGE };

	LineEntry* GetSelectLine();
	LineEntry* GetLineEntry( const UINT& ID );
	BOOL InsertLine( LineEntry* lineEntry, BOOL bInitialize = FALSE );
	BOOL UpdateLine( LineEntry* lineEntry );

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	BOOL InitEntryList();
	BOOL InitEntryDetailInfo();
	BOOL InitEntryDetailHeader();
	BOOL InitEntryPointsData();
	BOOL InitEntryPointsData(LineEntry* lineEntry);

	HTREEITEM GetKindNode( const wstring& category, const wstring& kind);
	HTREEITEM FindKindNode( const UINT& lineID);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonOK();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonMod();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnTreeSelChanged(LPNMHDR pnmhdr, LRESULT *pLResult);

	void FillLineData( LineEntry* lineEntry );

private:

	CTreeCtrl m_LinesTree;
	CListCtrlEx m_LineDetailList;

	CEdit m_LineNO;
	CEdit m_LineName;
	CEdit m_LineKind;

	CAcUiNumericEdit m_PointEdit;

	CButton m_FlatView;
	CButton m_LevelView;

	CButton m_LineAdd;
	CButton m_LineUpdate;
	CButton m_LineDelete;

	wstring m_fileName;

	LineEntryFile* m_EntryFile;

	HTREEITEM m_lineRoot;
	HTREEITEM m_blockRoot;
};

} // end of config

} // end of assistant

} // end of guch

} // end of com
