#pragma once
#include "afxcmn.h"

#include <resource.h>

#include <dbsymtb.h>
#include <dbapserv.h>
#include <adslib.h>
#include <adui.h>
#include <acui.h>

#include <string>
using namespace std;

#include <LineCategoryItemData.h>

namespace com
{

namespace guch
{

namespace assistant
{

namespace config
{

// 增加管线配置类型
class AddLineConfigDialog : public CDialog
{
	DECLARE_DYNAMIC(AddLineConfigDialog)

public:
	AddLineConfigDialog(CWnd* pParent = NULL);   // standard constructor

	AddLineConfigDialog(UINT dlgID,CWnd* pParent = NULL);   // standard constructor

	virtual ~AddLineConfigDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_LINE_CONFIG_ADD };

	typedef enum { OPER_ADD, OPER_UPDATE} OPER_TYPE;

	void SetOperType( OPER_TYPE type ){ m_OperType = type; };
	OPER_TYPE GetOperType() const { return m_OperType;}

	virtual void FillUpdateData();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	virtual BOOL InitConfigData();

	DECLARE_MESSAGE_MAP()

protected:

	CComboBox m_LineKind;
	CComboBox m_LineShape;
	CComboBox m_LineUnit;

	CEdit m_LineName;
	
	CStatic m_StaticRadius;
	CEdit m_LineRadius;

	CStatic m_StaticLength;
	CEdit m_LineLength;

	CStatic m_StaticWidth;
	CEdit m_LineWidth;

	CEdit m_LineWallSize;
	CEdit m_LineSafeSize;
	
	CEdit m_LineDesc;

	OPER_TYPE m_OperType;

	int m_lineIndex;
	UINT m_lineID;

	virtual afx_msg void OnBnClickedOk();
	virtual afx_msg void OnCbnShapeChange();

	virtual void ShowControlDynamic();
};

//增加阻隔体配置

class AddBlockConfigDialog : public AddLineConfigDialog
{
	DECLARE_DYNAMIC(AddBlockConfigDialog)

public:
	AddBlockConfigDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~AddBlockConfigDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_BLOCK_CONFIG_ADD };

	void FillUpdateData();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	BOOL InitConfigData();

	DECLARE_MESSAGE_MAP()

	virtual afx_msg void OnBnClickedOk();
	virtual afx_msg void OnChkCanThroughChange();

	virtual void ShowControlDynamic();

private:

	CButton m_CanThr;
	CComboBox m_ThroughDirection;
};

} // end of config

} // end of assistant

} // end of guch

} // end of com
