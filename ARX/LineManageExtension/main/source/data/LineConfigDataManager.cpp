#include "stdafx.h"

#include <LineConfigDataManager.h>
#include <GlobalDataConfig.h>
#include <LMAUtils.h>

#include <iostream>
#include <fstream>

/*
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
*/

#include <assert.h>
#include <acutads.h>

using namespace com::guch::assistant::data;

#pragma warning(disable : 4267)
#pragma warning(disable : 4244)

namespace com
{

namespace guch
{

namespace assistant
{

namespace config
{

LineConfigDataManager* LineConfigDataManager::instance = NULL;
const LPCWSTR LineConfigDataManager::PERSISTENT_FILE = L"C:\\Users\\chgu\\Documents\\LineConfig.xml";

LineConfigDataManager* LineConfigDataManager::Instance()
{
	if( instance == NULL )
	{
		instance = new LineConfigDataManager();
	}

	return instance;
}

LineConfigDataManager::LineConfigDataManager(void)
{
	mLineConfigData = new vector<LineCategoryItemData*>();

#ifdef _DEMO_DATA
	const int MAX_ITEM = 10;

	for( int i = 0; i < MAX_ITEM; i++)
	{
		CString ID;
		ID.Format(L"%d",i);

		LineCategoryItemData* item = new LineCategoryItemData(wstring(ID.GetBuffer()), 
						L"测试管道",
						GlobalData::KIND_LINE,
						GlobalData::LINE_CATEGORY_SHANGSHUI,
						GlobalData::LINE_SHAPE_CIRCLE,
						L"15",
						L"5",
						GlobalData::LINE_UNIT_CM,
						L"测试数据");

		mLineConfigData->push_back(item);
	}
#else
	CFile archiveFile;

	try
	{
		//read data from file PERSISTENT_FILE
		BOOL result = archiveFile.Open(PERSISTENT_FILE,CFile::modeRead);
		if( !result )
		{
			acutPrintf(L"打开管线类型配置文件失败");
			return;
		}

		//得到文件内容长度
		int length = (ULONGLONG)archiveFile.GetLength()+1;

		//得到文件的窄字符内容
		char* content = new char[length];
		memset(content,0,length);
		archiveFile.Read(content,length);

		//将其转换为宽字符
		string strCnt(content,length);
		wstring wContent = StringToWString( strCnt );

		//查找回车以决定行
		size_t lineFrom = 0;
		size_t linePos = wContent.find_first_of(L"\n",lineFrom);

		while( linePos != wstring::npos )
		{
			//得到一行数据
			wstring& wLine = wContent.substr(lineFrom, linePos-lineFrom);

			//将此行拆分
			size_t columnFrom = 0;
			size_t columnPos = wLine.find_first_of(L"\t",columnFrom);

			LineCategoryItemData* newItem = new LineCategoryItemData();
			newItem->mIndex = 0;

			int indexCol = 0;
			while( columnPos != wstring::npos )
			{
				//得到一个分段
				wstring& rColumn = wLine.substr(columnFrom,columnPos-columnFrom);

				//决定其属性
				if( indexCol == 0 )
					newItem->mID = _wtoi(rColumn.c_str());
				else if( indexCol == 1 )
					newItem->mCategory = rColumn;
				else if( indexCol == 2 )
					newItem->mName = rColumn;
				else if( indexCol == 3 )
					newItem->mKind = rColumn;
				else if( indexCol == 4 )
					newItem->mUnit = rColumn;
				else if( indexCol == 5 )
					newItem->mShape = rColumn;
				else if( indexCol == 6 )
					newItem->mRadius = rColumn;
				else if( indexCol == 7 )
					newItem->mLength = rColumn;
				else if( indexCol == 8 )
					newItem->mWidth = rColumn;
				else if( indexCol == 9 )
					newItem->mWallSize = rColumn;
				else if( indexCol == 10 )
					newItem->mSafeSize = rColumn;
				else if( indexCol == 11 )
					newItem->mCanThrough = rColumn;
				else if( indexCol == 12 )
					newItem->mThroughDirection = rColumn;

				indexCol++;

				//继续下一个column
				columnFrom = columnPos + 1;
				columnPos =  wLine.find_first_of(L'\t',columnFrom);
			}

			if( indexCol == 13 )
			{
				wstring& rColumn = wLine.substr(columnFrom);
				newItem->mComment = rColumn;

				mLineConfigData->push_back(newItem);
			}

			//从下一个字符开始查找另外一行
			lineFrom = linePos + 1;
			linePos = wContent.find_first_of(L"\n",lineFrom + 1);
		}

		//关闭文件
		archiveFile.Close();
	}
	catch(exception& e)
	{
		acutPrintf(L"打开管线类型配置文件异常[%s]",e.what());
	}

#endif
}

LineConfigDataManager::~LineConfigDataManager(void)
{
	ClearAllLines();
}

const LineCategoryVecotr* LineConfigDataManager::GetData() const
{
	return mLineConfigData;
}

bool LineConfigDataManager::Persistent()
{
	//std::ofstream ofs(PERSISTENT_FILE);
    //assert(ofs.good());

    //boost::archive::xml_oarchive oa(ofs);
	//boost::archive::text_oarchive oa(ofs);

	acutPrintf(L"开始保存管线类型\n");

	CFile archiveFile(PERSISTENT_FILE,CFile::modeCreate|CFile::modeWrite);

	typedef vector<LineCategoryItemData*>::const_iterator DataIterator;

	//遍历所有的类型定义
	for( DataIterator iter = mLineConfigData->begin(); 
			iter != mLineConfigData->end(); 
			iter++)
	{
		LineCategoryItemData* data = *iter;

		if( data )
		{
			//得到消息的宽字符序列化
			wstring wData = data->toString();

			//转为窄字符
			string dataStr = WstringToString(wData);

			//acutPrintf(L"管线类型数据 [%s] [%d]\n",wData.c_str(),(UINT)dataStr.length());

			//使用 virtual void Write( const void* lpBuf, UINT nCount ); 将窄字符写入文件
			archiveFile.Write(dataStr.c_str(),dataStr.size());

			//oa << BOOST_SERIALIZATION_NVP(*data);
			//oa << *data;
		}
	}

	acutPrintf(L"管线类型保存完成\n");
	archiveFile.Close();

	return true;
}

bool LineConfigDataManager::ClearAllLines()
{	
	for( CategoryIterator iter = mLineConfigData->begin();
			iter != mLineConfigData->end();
			iter++)
	{
		delete *iter;
	}

	mLineConfigData->clear();

	delete mLineConfigData;
	mLineConfigData = NULL;

	return true;
}

void LineConfigDataManager::AddLine(const LineCategoryItemData* newLine)
{
	if( mLineConfigData == NULL )
		mLineConfigData = new vector<LineCategoryItemData*>();

	mLineConfigData->push_back(new LineCategoryItemData(*newLine));
	this->Persistent();
}

BOOL LineConfigDataManager::UpdateLine(const LineCategoryItemData* updateLine)
{
	for( CategoryIterator iter = mLineConfigData->begin();
		iter != mLineConfigData->end();
		iter++)
	{
		if( (*iter)->mID == updateLine->mID )
		{
			delete (*iter);

			LineCategoryItemData *newLine = new LineCategoryItemData(*updateLine);
			mLineConfigData->erase(iter);
			mLineConfigData->push_back(newLine);

			this->Persistent();
			return TRUE;
		}
	}

	return FALSE;
}

const wstring LineConfigDataManager::FindCategory( const wstring& kind )
{
	for( CategoryIterator iter = mLineConfigData->begin();
		iter != mLineConfigData->end();
		iter++)
	{
		//不是按管线配置的种类查找，而是按用户新建管线的类型查找
		if( (*iter)->mName == kind )
		{
			return (*iter)->mCategory;
		}
	}

	return L"";
}

UINT LineConfigDataManager::GetNextID()
{
	UINT newID = (UINT)GetTickCount();

	return newID;
}

LineCategoryItemData* LineConfigDataManager::FindLine( const wstring& lineName ) const
{
	for( CategoryIterator iter = mLineConfigData->begin();
		iter != mLineConfigData->end();
		iter++)
	{
		if( (*iter)->mName == lineName )
		{
			return *iter;
		}
	}

	return NULL;
}

LineCategoryItemData* LineConfigDataManager::FindByKind( const wstring& lineKind ) const
{
	for( CategoryIterator iter = mLineConfigData->begin();
		iter != mLineConfigData->end();
		iter++)
	{
		if( (*iter)->mName == lineKind )
		{
			return *iter;
		}
	}

	return NULL;
}

LineCategoryItemData* LineConfigDataManager::FindLine( UINT ID ) const
{
	for( CategoryIterator iter = mLineConfigData->begin();
		iter != mLineConfigData->end();
		iter++)
	{
		if( (*iter)->mID = ID )
		{
			return *iter;
		}
	}

	return NULL;
}

BOOL LineConfigDataManager::DeleteLine( UINT ID )
{
	for( CategoryIterator iter = mLineConfigData->begin();
		iter != mLineConfigData->end();
		iter++)
	{
		if( (*iter)->mID = ID )
		{
			delete (*iter);
			mLineConfigData->erase(iter);

			this->Persistent();

			return TRUE;
		}
	}

	return FALSE;
}

} // end of data

} // end of assistant

} // end of guch

} // end of com
