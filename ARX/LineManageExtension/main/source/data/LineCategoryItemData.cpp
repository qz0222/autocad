#include "stdafx.h"
#include "LineCategoryItemData.h"
#include <LMAUtils.h>

#pragma warning (disable : 4996 )

namespace com
{

namespace guch
{

namespace assistant
{

namespace config
{

LineCategoryItemData::LineCategoryItemData(void)
:mIndex(0),
mID(0),
mCategory(L"0"),
mName(L"0"),
mKind(L"0"),
mUnit(L"0"),
mShape(L"0"),
mRadius(L"0"),
mLength(L"0"),
mWidth(L"0"),
mWallSize(L"0"),
mSafeSize(L"0"),
mCanThrough(L"0"),
mThroughDirection(L"0"),
mComment(L"0")
{
}

LineCategoryItemData::LineCategoryItemData( int index,
											const UINT& rID,
											const wstring rCategory,
											const wstring& rName,
											const wstring& rKind,
											const wstring& rUnit,
											const wstring& rShape,
											const wstring& rRadius,
											const wstring& rLength,
											const wstring& rWidth,
											const wstring& rWallSize,
											const wstring& rSafeSize,
											const wstring& rCanThrough,
											const wstring& rThroughDirection,
											const wstring& rComment)
:mIndex(index),
mID(rID),
mCategory(rCategory),
mName(rName),
mKind(rKind),
mUnit(rUnit),
mShape(rShape),
mRadius(rRadius),
mLength(rLength),
mWidth(rWidth),
mWallSize(rWallSize),
mSafeSize(rSafeSize),
mCanThrough(rCanThrough),
mThroughDirection(rThroughDirection),
mComment(rComment)
{}

LineCategoryItemData::LineCategoryItemData( const LineCategoryItemData& rData)
:mIndex(rData.mIndex),
mID(rData.mID),
mCategory(rData.mCategory),
mName(rData.mName),
mKind(rData.mKind),
mUnit(rData.mUnit),
mShape(rData.mShape),
mRadius(rData.mRadius),
mLength(rData.mLength),
mWidth(rData.mWidth),
mWallSize(rData.mWallSize),
mSafeSize(rData.mSafeSize),
mCanThrough(rData.mCanThrough),
mThroughDirection(rData.mThroughDirection),
mComment(rData.mComment)
{
}

wstring LineCategoryItemData::toString() const
{
	CString ID;
	ID.Format(L"%d",mID);

	return wstring(ID.GetBuffer()) + L"\t" 
			+ mCategory + L"\t"
			+ mName + L"\t"
			+ mKind + L"\t"
			+ mUnit + L"\t"
			+ mShape + L"\t"
			+ mRadius + L"\t"
			+ mLength + L"\t"
			+ mWidth + L"\t"
			+ mWallSize + L"\t"
			+ mSafeSize + L"\t"
			+ mCanThrough + L"\t"
			+ mThroughDirection + L"\t"
			+ mComment + L"\n";
}

LineCategoryItemData::~LineCategoryItemData(void){}

/*
std::ostream & operator<<(std::ostream &os, const LineCategoryItemData &itemData)
{
	return os << itemData.mID << itemData.mName << itemData.mKind;
}
*/

} // end of data

} // end of assistant

} // end of guch

} // end of com