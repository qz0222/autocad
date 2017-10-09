// LineCutPosDialog.cpp : implementation file
//

#include "stdafx.h"
#include "LineCutPosDialog.h"
#include "afxdialogex.h"

#include "acedads.h"
#include "accmd.h"
#include <adscodes.h>

#include <adsdlg.h>

#include <dbapserv.h>

#include <dbregion.h>

#include <gepnt3d.h>

//symbol table
#include <dbsymtb.h>

//3D Object
#include <dbsol3d.h>

#include <ArxWrapper.h>


void Transform(AcDbEntity* pEntry, int xoffset = 0, int yoffset = 0, int zoffset = 0)
{
	AcGeVector3d vec(xoffset,yoffset,zoffset);

	AcGeMatrix3d moveMatrix;
	moveMatrix.setToTranslation(vec);

	pEntry->transformBy(moveMatrix);
}

void generateCutRegion(AcDb3dSolid* pSolid, int offset, int direction)
{
	//创建切面
	AcGePlane plane;

	if( direction == 1)
		plane.set(AcGePoint3d(offset,0,0),AcGeVector3d(1,0,0));
	else if( direction == 2)
		plane.set(AcGePoint3d(0,offset,0),AcGeVector3d(0,1,0));
	else if( direction == 3)
		plane.set(AcGePoint3d(0,0,offset),AcGeVector3d(0,0,1));

	//得到实体与切面相切的截面
	AcDbRegion *pSelectionRegion = NULL;
	pSolid->getSection(plane, pSelectionRegion);

	//将其移动到YZ平面
	//moveToBottom(pSelectionRegion);

	//将截面加入到模型空间
	PostToModelSpace(pSelectionRegion);
}

// LineCutPosDialog dialog

IMPLEMENT_DYNAMIC(LineCutPosDialog, CAcUiDialog)

AcDb3dSolid* LineCutPosDialog::m3dSolid = NULL;

LineCutPosDialog::LineCutPosDialog(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(LineCutPosDialog::IDD, pParent),
	m_Direction(0)
{
}

LineCutPosDialog::~LineCutPosDialog()
{
}

BOOL LineCutPosDialog::OnInitDialog()
{
	//和页面交互数据
	CAcUiDialog::OnInitDialog();

	edit_m_A00.SetWindowTextW(L"0.1");
	edit_m_A10.SetWindowTextW(L"1");
	edit_m_A20.SetWindowTextW(L"0");

	edit_m_A01.SetWindowTextW(L"20");
	edit_m_A11.SetWindowTextW(L"0");
	edit_m_A21.SetWindowTextW(L"0");

	edit_m_A02.SetWindowTextW(L"0");
	edit_m_A12.SetWindowTextW(L"0");
	edit_m_A22.SetWindowTextW(L"0");

	edit_m_T0.SetWindowTextW(L"0");
	edit_m_T1.SetWindowTextW(L"0");
	edit_m_T2.SetWindowTextW(L"0");

	return TRUE;
}

void LineCutPosDialog::DoDataExchange(CDataExchange* pDX)
{
	CAcUiDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OFFSET, mOffset);

	DDX_Control(pDX, IDC_X, mDirectionX);
	DDX_Control(pDX, IDC_Y, m_DirectionY);
	DDX_Control(pDX, IDC_Z, m_DirectionZ);

	DDX_Control(pDX, IDC_EDIT_A00, edit_m_A00);
	DDX_Control(pDX, IDC_EDIT_A10, edit_m_A10);
	DDX_Control(pDX, IDC_EDIT_A20, edit_m_A20);

	DDX_Control(pDX, IDC_EDIT_A01, edit_m_A01);
	DDX_Control(pDX, IDC_EDIT_A11, edit_m_A11);
	DDX_Control(pDX, IDC_EDIT_A21, edit_m_A21);

	DDX_Control(pDX, IDC_EDIT_A02, edit_m_A02);
	DDX_Control(pDX, IDC_EDIT_A12, edit_m_A12);
	DDX_Control(pDX, IDC_EDIT_A22, edit_m_A22);

	DDX_Control(pDX, IDC_EDIT_T0, edit_m_T0);
	DDX_Control(pDX, IDC_EDIT_T1, edit_m_T1);
	DDX_Control(pDX, IDC_EDIT_T2, edit_m_T2);
}

void CreatePipe( const AcGePoint3d& start, const AcGePoint3d& end, const double& radius)
{
	acutPrintf(L"开始绘制管体\n");

	//得到线段的长度
	double length = start.distanceTo(end);
	if( length < 0.1 )
		return;

	acutPrintf(L"得到管体高度%lf\n",length);

	//绘制圆柱体
	AcDb3dSolid* p3dPipe = CreateCylinder(radius,length);
	if( p3dPipe == NULL )
		return;

	//得到线段与Z轴的垂直向量
	AcGeVector3d line3dVector(end.x - start.x,end.y - start.y, end.z-start.z);
	AcGeVector3d rotateVctor = line3dVector.crossProduct(AcGeVector3d::kZAxis);

	//得到旋转的角度
	double angle = -line3dVector.angleTo(AcGeVector3d::kZAxis);
	acutPrintf(L"得到旋转角度%lf\n",angle);

	//进行旋转
	AcGeMatrix3d rotateMatrix = AcGeMatrix3d::rotation( angle, rotateVctor, AcGePoint3d::kOrigin);
	p3dPipe->transformBy(rotateMatrix);
	
	//得到线段的中心点
	AcGePoint3d center(start.x + end.x, start.y + end.y, start.z + end.z); 
	center /= 2;
	acutPrintf(L"得到中心点[%lf][%lf][%lf]\n",center.x,center.y,center.z);

	//进行偏移
	AcGeMatrix3d moveMatrix;
	moveMatrix.setToTranslation(AcGeVector3d(center.x,center.y,center.z));

	p3dPipe->transformBy(moveMatrix);

	//加入到3D模型中
	PostToModelSpace(p3dPipe);

#ifdef DEBUG
	acutPrintf(L"插入中心线，用于矫正");

	AcDbLine *pLine = new AcDbLine(start, end);
    PostToModelSpace(pLine);
#endif
}

void LineCutPosDialog::OnBnClickedOk()
{
	GetTransformData();

	/*
	//绘制圆柱
	{
		//得到半径和高度
		double radius = _wtof(m_A00.GetBuffer());
		double height = _wtof(m_A01.GetBuffer());

		m3dSolid = CreateCylinder(radius,height);
		if( m3dSolid == NULL )
			return;
	}
	*/

	//绘制管道

	double radius = _wtof(m_A00.GetBuffer());

	double startX = _wtof(m_A10.GetBuffer());
	double startY = _wtof(m_A11.GetBuffer());
	double startZ = _wtof(m_A12.GetBuffer());

	double endX = _wtof(m_A20.GetBuffer());
	double endY = _wtof(m_A21.GetBuffer());
	double endZ = _wtof(m_A22.GetBuffer());

	CreatePipe(AcGePoint3d(startX,startY,startZ),AcGePoint3d(endX,endY,endZ),radius);

	/*
	{
		acutPrintf(L"首先绘制线段\n");
		point = AcGePoint3d(x,y,z);
		ArxWrapper::createLine(AcGePoint3d(),point,L"测试矩阵旋转");
	}

	//首先进行旋转
	{
		//AcGeMatrix3d rotateMatrix = AcGeMatrix3d::alignCoordSys(AcGePoint3d::kOrigin,AcGeVector3d::kXAxis,AcGeVector3d::kYAxis,AcGeVector3d::kZAxis,
		//	AcGePoint3d::kOrigin,AcGeVector3d::kXAxis,AcGeVector3d::kYAxis,AcGeVector3d(point.x,point.y,point.z));
		
		//得到垂直向量
		AcGeVector3d line3dVector(x,y,z);
		AcGeVector3d rotateVctor = line3dVector.crossProduct(AcGeVector3d::kZAxis);

		//得到角度
		double angle = line3dVector.angleTo(AcGeVector3d::kZAxis)/-2;

		//double angle = _wtof(m_A10.GetBuffer());
		acutPrintf(L"向量角度,除以-2,进行旋转[%lf]\n",angle);

		//AcGeMatrix3d rotateMatrix = AcGeMatrix3d::rotation( (3.14159265358979323846 * angle ) / 360,AcGeVector3d(point.x,point.y,point.z),AcGePoint3d::kOrigin);
		AcGeMatrix3d rotateMatrix = AcGeMatrix3d::rotation( angle, rotateVctor, AcGePoint3d::kOrigin);
		m3dSolid->transformBy(rotateMatrix);

		//X轴旋转
		//AcGeVector3d vecX(_wtof(m_A20.GetBuffer()),_wtof(m_A21.GetBuffer()),_wtof(m_A22.GetBuffer()));

		//得到

		//rotateMatrix = AcGeMatrix3d::rotation( ( 3.14159265358979323846 * _wtof(m_A10.GetBuffer()) ) / 180 , vecX );
		m3dSolid->transformBy(rotateMatrix);

		//Y轴旋转
		//AcGeVector3d vecY(0,1,0);
		//rotateMatrix = AcGeMatrix3d::rotation( ( 3.14159265358979323846 * _wtof(m_A11.GetBuffer()) ) / 180, vecY );
		//m3dSolid->transformBy(rotateMatrix);

		//Z轴旋转
		//AcGeVector3d vecZ(0,0,1);
		//rotateMatrix = AcGeMatrix3d::rotation( ( 3.14159265358979323846 * _wtof(m_A12.GetBuffer()) ) / 180, vecZ );
		//m3dSolid->transformBy(rotateMatrix);
	}

	//然后进行相应的位移
	{
		AcGeVector3d moveVec(_wtof(m_T0.GetBuffer()), _wtof(m_T1.GetBuffer()), _wtof(m_T2.GetBuffer()));

		AcGeMatrix3d moveMatrix;
		moveMatrix.setToTranslation(moveVec);

		m3dSolid->transformBy(moveMatrix);
	}

	// 将圆锥体添加到模型空间
	PostToModelSpace(m3dSolid);
	*/

	/*
	//AcGePoint3d orignal(_wtof(m_T0.GetBuffer()), _wtof(m_T1.GetBuffer()), _wtof(m_T2.GetBuffer()));
		
	int offset = 0;
	
	if( m_strOffset.GetLength())
		offset = _wtoi(m_strOffset);

	//generateCutRegion(pSolid,offset,m_Direction);
	*/

	CAcUiDialog::OnOK();
}

void LineCutPosDialog::GetTransformData()
{
	UpdateData(FALSE);

	edit_m_A00.Convert();
	edit_m_A00.GetWindowText(m_A00);

	edit_m_A10.Convert();
	edit_m_A10.GetWindowText(m_A10);

	edit_m_A20.Convert();
	edit_m_A20.GetWindowText(m_A20);

	//-------------------------//

	edit_m_A01.Convert();
	edit_m_A01.GetWindowText(m_A01);

	edit_m_A11.Convert();
	edit_m_A11.GetWindowText(m_A11);

	edit_m_A21.Convert();
	edit_m_A21.GetWindowText(m_A21);

	//-------------------------//

	edit_m_A02.Convert();
	edit_m_A02.GetWindowText(m_A02);

	edit_m_A12.Convert();
	edit_m_A12.GetWindowText(m_A12);

	edit_m_A22.Convert();
	edit_m_A22.GetWindowText(m_A22);

	//-------------------------//

	edit_m_T0.Convert();
	edit_m_T0.GetWindowText(m_T0);

	edit_m_T1.Convert();
	edit_m_T1.GetWindowText(m_T1);

	edit_m_T2.Convert();
	edit_m_T2.GetWindowText(m_T2);
}

void LineCutPosDialog::OnBnClickedX()
{
	m_Direction = 1;
}

void LineCutPosDialog::OnBnClickedY()
{
	m_Direction = 2;
}

void LineCutPosDialog::OnBnClickedZ()
{
	m_Direction = 3;
}

BEGIN_MESSAGE_MAP(LineCutPosDialog, CAcUiDialog)
	ON_BN_CLICKED(IDOK, &LineCutPosDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_X, &LineCutPosDialog::OnBnClickedX)
	ON_BN_CLICKED(IDC_Y, &LineCutPosDialog::OnBnClickedY)
	ON_BN_CLICKED(IDC_Z, &LineCutPosDialog::OnBnClickedZ)
END_MESSAGE_MAP()

// LineCutPosDialog message handlers
