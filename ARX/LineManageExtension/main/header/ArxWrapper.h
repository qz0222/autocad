// ------------------------------------------------
//                  LineManagementAssistant
// Copyright 2012-2013, Chengyong Yang & Changhai Gu. 
//               All rights reserved.
// ------------------------------------------------
//	ArxWrapper.h
//	written by Changhai Gu
// ------------------------------------------------
// $File:\\LineManageAssitant\main\source\ARX\ArxWrapper.h $
// $Author: Changhai Gu $
// $DateTime: 2013/1/2 01:35:46 $
// $Revision: #1 $
// ------------------------------------------------

#include "afxcmn.h"

#include <string.h>
#include <aced.h>
#include <dbents.h>
#include <dbsymtb.h>
#include <dbgroup.h>
#include <dbapserv.h>
#include "tchar.h"

#include <string>
#include <vector>

#include <LineEntryData.h>

using namespace com::guch::assistant::data;
using namespace std;

typedef vector<AcGePoint3d*> Point3dVector;
typedef Point3dVector::const_iterator Point3dIter;


class ArxWrapper
{
public:

	static const double kPi;
	static const double kHalfPi;
	static const double kTwoPi;
	static const double kRad45;
	static const double kRad90;
	static const double kRad135;
	static const double kRad180;
	static const double kRad270;
	static const double kRad360;

	static bool createNewLayer(const wstring& layerName);

	static bool ShowLayer(const wstring& theOnly);

	static bool DeleteLayer(const wstring& layerName);

	static AcDbObjectId createLine( const AcGePoint3d& start,
							const AcGePoint3d& end,
							const wstring& layerName );

	static void createLine( const Point3dVector& points3d,
							const wstring& layerName );

	static void createLine ( const PointList& points,
							const wstring& layerName);

	//Add entry to model
	static AcDbObjectId PostToModelSpace(AcDbEntity* pEnt,const wstring& layerName );

	//Remove entry from model
	static Acad::ErrorStatus RemoveFromModelSpace(AcDbEntity* pEnt,const wstring& layerName );

	static Acad::ErrorStatus RemoveFromModelSpace(const AcDbHandle& handle,const wstring& layerName );

	//Add object to name dictionary
	static AcDbObjectId PostToNameObjectsDict(AcDbObject* pObj,const wstring& key, bool toDelete = false );

	//Read object from name dictionary
	static void PullFromNameObjectsDict();

	//move offset
	static AcDbEntity* MoveToBottom(AcDbEntity* pEntry);

	//Create LMA Line
	static void createLMALine( const LineEntry& entry );

	//Polygon cylinder
	static void DrawPolyCylinder( const LineEntry& lineEntry );

	//cylinder
	static AcDb3dSolid* DrawCylinder( const UINT& lineID,
										const UINT& sequenceID,
										const AcGePoint3d& start,
										const AcGePoint3d& end,
										const wstring& layerName,
										LineEntry& entry);

	static void eraseLMALine(const LineEntry& lineEntry, bool old = false);

	//Change view
	static void ChangeView(int viewDirection);

	//…Ë÷√ÃÓ≥‰
	static AcDbObjectId CreateHatch(AcDbEntity* entity,const wstring& patName, bool bAssociative, const wstring& layerName, const AcGePlane& plane, const double& distance );

	static AcDbObjectId CreateHatch(AcDbObjectIdArray objIds,const wstring& patName, bool bAssociative, const wstring& layerName, const AcGeVector3d& normal, const double& elevation);

	//ÃÌº”◊¢ Õ
	static AcDbObjectId CreateMLeader(const AcGePoint3d& center, const int& offset, const int& direction,
											const wstring& content, const wstring& layerName);

	//≤‚ ‘∫Ø ˝
	static void TestFunction();
};
