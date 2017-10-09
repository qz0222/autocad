#pragma once
#include "afxcmn.h"

#include <resource.h>

#include <dbsymtb.h>
#include <dbapserv.h>
#include <adslib.h>
#include <adui.h>
#include <acui.h>

#include "LineConfigDialog.h"

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

class LineTypeConfigPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(LineTypeConfigPropertySheet)

public:
	LineTypeConfigPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	LineTypeConfigPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	virtual ~LineTypeConfigPropertySheet();

	void ChangeControlStatus(BOOL bEnable);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	BOOL InitPages();
	BOOL InitButtons();

	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOK();

	DECLARE_MESSAGE_MAP()

private:
	static const int BTN_WIDTH;
	static const int BTN_HEIGHT;

	static const int LEFT_WIDTH;
	static const int BOTTOM_MARGIN;
	static const int BTN_MARGIN;

	LineConfigDialog m_LineConfigDlg;
	BlockConfigDialog m_BlockConfigDlg;

	CButton m_ButtonAdd;
	CButton m_ButtonMod;
	CButton m_ButtonDel;
};

} // end of config

} // end of assistant

} // end of guch

} // end of com