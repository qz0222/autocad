#pragma once
#include "afxcmn.h"

#include <resource.h>

#include <dbsymtb.h>
#include <dbapserv.h>
#include <adslib.h>
#include <adui.h>
#include <acui.h>

#include <AddLineDialog.h>
#include <LMAException.h>

// LineConfigDialog dialog

using namespace std;

namespace com
{

namespace guch
{

namespace assistant
{

namespace config
{

//管线数据

class LineConfigDialog : public CPropertyPage
{
	DECLARE_DYNAMIC(LineConfigDialog)

public:
	LineConfigDialog( CWnd* pParentWnd = NULL);

	virtual ~LineConfigDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_LINE_CONFIG };

	virtual afx_msg void OnBnClickedButtonAdd();
	virtual afx_msg void OnBnClickedButtonMod();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonOK();

	virtual BOOL InsertLine(LineCategoryItemData& itemData, BOOL initialize = false);
	virtual BOOL UpdateLine(const LineCategoryItemData& itemData);

	virtual BOOL GetSelectData(LineCategoryItemData& configData );
	virtual void CheckValid( const LineCategoryItemData& item, BOOL bNew );

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	virtual BOOL InitLineHeader();
	virtual BOOL InitLineData();

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

	virtual BOOL GetItemData( int item, LineCategoryItemData& configData);

	virtual int FindItem( UINT id ) const;

private:
	
	virtual void UpdateUILineData(const LineCategoryItemData& itemData);

protected:

	CAcUiListCtrl m_lineConfig;
};

// 阻隔体数据

class BlockConfigDialog : public LineConfigDialog
{
	DECLARE_DYNAMIC(BlockConfigDialog)

public:
	BlockConfigDialog( CWnd* pParentWnd = NULL );

	virtual ~BlockConfigDialog();

// Dialog Data

	virtual afx_msg void OnBnClickedButtonAdd();
	virtual afx_msg void OnBnClickedButtonMod();

protected:

	virtual BOOL OnInitDialog();

	virtual BOOL InitLineHeader();
	virtual BOOL InitLineData();

	DECLARE_MESSAGE_MAP()

	virtual BOOL GetItemData( int item, LineCategoryItemData& configData);

private:
	
	void UpdateUILineData(const LineCategoryItemData& itemData);
};

} // end of config

} // end of assistant

} // end of guch

} // end of com