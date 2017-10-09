#pragma once

#include <resource.h>

#include "afxcmn.h"

// AddEntryDialog dialog

namespace com
{

namespace guch
{

namespace assistant
{

namespace entry
{

class AddEntryDialog : public CDialog
{
	DECLARE_DYNAMIC(AddEntryDialog)

public:
	AddEntryDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~AddEntryDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADD_ENTRY };

	typedef enum { OPER_ADD, OPER_UPDATE} OPER_TYPE;
	void SetOperType( OPER_TYPE type ){ m_OperType = type; };
	OPER_TYPE GetOperType() const { return m_OperType;}

	void SetUpdateID( const UINT& id ) { m_UpdateLineID = id; }

	afx_msg void OnSelchangeComboCategory();
	afx_msg void OnBnClickedOk();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

	virtual BOOL InitConfigData();

	CComboBox m_LineCategory;
	CComboBox m_LineKind;

	CEdit m_LineName;
	CEdit m_LineNO;

	UINT m_UpdateLineID;

private:

	virtual void FillUpdateData();

private:
		
	OPER_TYPE m_OperType;
};

} // end of entry

} // end of assistant

} // end of guch

} // end of com
