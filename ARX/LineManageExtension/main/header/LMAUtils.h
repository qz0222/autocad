#pragma once
#include <afxwin.h> 

#include <string>
#include <vector>

#include <dbmain.h>
#include <acdocman.h>
#include <acestext.h>

using namespace std;

typedef vector<wstring> wstrVector;

#define LOG(log) acutPrintf(log);

//将string转换成wstring
wstring StringToWString(const string& str);

//将wstring转换成string     
string WstringToString(const wstring& wstr);

// Enables and disables an MFC control by name
void EnableControl( CWnd* pWnd, int iControl, bool enable );

void ShowControl( CWnd *pWnd, int iControl, bool show );

void MoveControl( CWnd *pWnd, int iControl, int top, int left, int sizex, int sizey );

void OverControl( CWnd *pWnd, int iControlSrc, int iControlDst );

void SetControlText( CWnd* pWnd, int iControl, LPCTSTR lpszString );

wstrVector* vectorContructor( const wstring& data, const wstring& seq, size_t start = 0, size_t end = 0 );

LPCTSTR dbToStr(AcDbDatabase* db, CString& str);

void rxErrorMsg(Acad::ErrorStatus msgId);
