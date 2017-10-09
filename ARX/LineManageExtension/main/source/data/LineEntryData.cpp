// ------------------------------------------------
//                  LineManagementAssistant
// Copyright 2012-2013, Chengyong Yang & Changhai Gu. 
//               All rights reserved.
// ------------------------------------------------
//	LineEntryData.cpp
//	written by Changhai Gu
// ------------------------------------------------
// $File:\\LineManageAssitant\main\source\data\LineEntryData.h $
// $Author: Changhai Gu $
// $DateTime: 2013/1/2 01:35:46 $
// $Revision: #1 $
// ------------------------------------------------

#include <LineEntryData.h>
#include <LMAUtils.h>

#include <ArxWrapper.h>
#include <acdocman.h>
#include <acutmem.h>

namespace com
{

namespace guch
{

namespace assistant
{

namespace data
{

#define POINT_START L"PS"
#define POINT_END L"PE"
#define POINTS_SEP L"&&"

///////////////////////////////////////////////////////////////////////////
// Implementation PointEntry

/**
 * 管线坐标实体
 */

PointEntry::PointEntry()
:m_PointNO(0),
m_LevelKind(L""),
m_Direction(L""),
m_pEntry(NULL)
{
	m_Point[X] = 0;
	m_Point[Y] = 0;
	m_Point[Z] = 0;
}

PointEntry::PointEntry( const UINT& pointNO, const ads_point& point, const wstring& levelKind, const wstring& direction)
:m_PointNO(pointNO),
m_LevelKind(levelKind),
m_Direction(direction),
m_pEntry(NULL)	
{
	m_Point[X] = point[X];
	m_Point[Y] = point[Y];
	m_Point[Z] = point[Z];
}

PointEntry::PointEntry( const PointEntry& pointEntry)
{
	this->m_PointNO = pointEntry.m_PointNO;
	this->m_LevelKind = pointEntry.m_LevelKind;
	this->m_Direction = pointEntry.m_Direction;

	this->m_Point[X] = pointEntry.m_Point[X];
	this->m_Point[Y] = pointEntry.m_Point[Y];
	this->m_Point[Z] = pointEntry.m_Point[Z];

	this->m_pEntry = pointEntry.m_pEntry;
}

PointEntry::PointEntry( const wstring& data )
{
	double temp;

	const static size_t start = wcslen(POINT_START);
	size_t end = data.find_first_of(POINT_END);

	int index = 0;

	wstrVector* dataColumn = vectorContructor(data,POINTS_SEP,start,end);
	wstring& column = (*dataColumn)[index++];

	acdbDisToF(column.c_str(), -1, &temp);
	this->m_PointNO = (UINT)temp;

	column = (*dataColumn)[index++];
	acdbDisToF(column.c_str(), -1, &m_Point[X]);

	column = (*dataColumn)[index++];
	acdbDisToF(column.c_str(), -1, &m_Point[Y]);

	column = (*dataColumn)[index++];
	acdbDisToF(column.c_str(), -1, &m_Point[Z]);

	m_LevelKind = (*dataColumn)[index++];

	m_Direction = (*dataColumn)[index++];

	delete dataColumn;
}

wstring PointEntry::toString() const
{
	CString temp;
	temp.Format(L"%s%d%s%0.2f%s%0.2f%s%0.2f%s%s%s%s%s%s",
				POINT_START,
				m_PointNO, POINTS_SEP,
				m_Point[X], POINTS_SEP,
				m_Point[Y], POINTS_SEP,
				m_Point[Z], POINTS_SEP,
				m_LevelKind,POINTS_SEP,
				m_Direction,POINTS_SEP,
				POINT_END);

	return temp.GetBuffer();
}

///////////////////////////////////////////////////////////////////////////
// Implementation LineEntry

const wstring LineEntry::LINE_ENTRY_LAYER = L"管线实体层";

ACRX_DXF_DEFINE_MEMBERS(LineEntry, AcDbObject, 
AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
0,
    LineEntry, LMA);

/**
 * 管线实体
 */

LineEntry::LineEntry()
	:m_LineID(0),
	m_LineNO(L"0"),
	m_LineName(L""),
	m_LineKind(L""),
	m_CurrentPointNO(0)
{
	m_PrePointList = NULL;
	m_PointList = new PointList();
}

LineEntry::LineEntry(const wstring& rLineNO,
				const wstring& rLineName,
				const wstring& rLineKind)
	:m_LineID(0),
	m_LineNO(rLineNO),
	m_LineName(rLineName),
	m_LineKind(rLineKind),
	m_CurrentPointNO(0)
{
	m_PrePointList = NULL;
	m_PointList = new PointList();
}

LineEntry::LineEntry( const wstring& data)
{
	m_PointList = new PointList();

	double temp;

	int index = 0;

	wstrVector* dataColumn = vectorContructor(data,L"\t");

	//得到线的相关属性
	wstring& column = (*dataColumn)[index++];
	acdbDisToF(column.c_str(), -1, &temp);
	this->m_LineID = (UINT)temp;

	m_LineNO = (*dataColumn)[index++];
	m_LineName = (*dataColumn)[index++];
	m_LineKind = (*dataColumn)[index++];

	column = (*dataColumn)[index++];
	acdbDisToF(column.c_str(), -1, &temp);
	m_CurrentPointNO = (UINT)temp;

	//得到每个点的属性
	int size = (int)dataColumn->size();

	while( index < size )
	{
		column = (*dataColumn)[index++];

		m_PointList->push_back(new PointEntry(column));
	}

	delete dataColumn;
}

LineEntry::~LineEntry()
{
	//ClearPoints();
}

void LineEntry::ClearPoints()
{
	ClearPoints(this->m_PrePointList);
	ClearPoints(this->m_PointList);
}

void LineEntry::ClearPoints( PointList* pPointList)
{
	if( pPointList )
	{
		for( PointIter iter = pPointList->begin();
				iter != pPointList->end();
				iter++ )
		{
			if(*iter)
				delete *iter;
		}

		delete pPointList;
		pPointList = NULL;
	}
}

PointIter LineEntry::FindPoint( const UINT& PointNO ) const
{
	for( PointIter iter = this->m_PointList->begin();
			iter != this->m_PointList->end();
			iter++)
	{
		if( (*iter)->m_PointNO == PointNO )
			return iter;
	}

	return m_PointList->end();
}

ContstPointIter LineEntry::FindConstPoint( const UINT& PointNO ) const
{
	for( ContstPointIter iter = this->m_PointList->begin();
			iter != this->m_PointList->end();
			iter++)
	{
		if( (*iter)->m_PointNO == PointNO )
			return iter;
	}

	return m_PointList->end();
}

int LineEntry::InsertPoint( const PointEntry& newPoint )
{
	pPointEntry point = new PointEntry(newPoint);

	point->m_PointNO = m_CurrentPointNO;

	m_PointList->push_back(point);

	m_CurrentPointNO++;

	return (int)m_PointList->size();
}

void LineEntry::UpdatePoint( const PointEntry& updatePoint )
{
	PointIter findPoint = this->FindPoint(updatePoint.m_PointNO);

	if( findPoint != this->m_PointList->end() )
	{
		delete *findPoint;
		*findPoint = new PointEntry(updatePoint);
	}
}

void LineEntry::DeletePoint( const UINT& PointNO )
{
	PointIter findPoint = this->FindPoint(PointNO);

	if( findPoint != this->m_PointList->end() )
	{
		m_PointList->erase(findPoint);
	}
}

void LineEntry::SetPoints( PointList* newPoints)
{
	//保存当前的节点列表，以用于删除以前的对象
	m_PrePointList = m_PointList;

	//新的列表，用于创建新的线段
	m_PointList = newPoints;

	Redraw();
}

wstring LineEntry::toString()
{
	wstring lineData;

	CString temp;
	temp.Format(L"%d\t%s\t%s\t%s\t%d",m_LineID,m_LineNO,m_LineName,m_LineKind,m_CurrentPointNO);

#ifdef DEBUG
	//acutPrintf(L"\n管线实体序列化为【%s】",temp.GetBuffer());
#endif

	lineData = temp;

	for( ContstPointIter iter = this->m_PointList->begin();
			iter != this->m_PointList->end();
			iter++)
	{
		lineData += L"\t";
		lineData += (*iter)->toString();
	}

	return lineData;
}

void LineEntry::Redraw()
{
	//删除以前的线段(从数据库中)
	ArxWrapper::eraseLMALine(*this,true);

	//删除以前的线段(从内存中)
	ClearPoints(this->m_PrePointList);

	//绘制新的线段
	ArxWrapper::createLMALine(*this);
}

// Files data in from a DWG file.
//
Acad::ErrorStatus
LineEntry::dwgInFields(AcDbDwgFiler* pFiler)
{
    assertWriteEnabled();

    AcDbObject::dwgInFields(pFiler);
    // For wblock filing we wrote out our owner as a hard
    // pointer ID so now we need to read it in to keep things
    // in sync.
    //
    if (pFiler->filerType() == AcDb::kWblockCloneFiler) {
        AcDbHardPointerId id;
        pFiler->readItem(&id);
    }

	if( !this->isErased() )
	{
		Adesk::UInt32 lineID;
		pFiler->readItem(&lineID);
		m_LineID = (UINT)lineID;

		TCHAR* tmpStr = NULL;    // must explicitly set to NULL or readItem() crashes!
		pFiler->readItem(&tmpStr);
		m_LineNO = wstring(tmpStr);
		acutDelString(tmpStr);

		tmpStr = NULL;    // must explicitly set to NULL or readItem() crashes!
		pFiler->readItem(&tmpStr);
		m_LineName = wstring(tmpStr);
		acutDelString(tmpStr);

		tmpStr = NULL;    // must explicitly set to NULL or readItem() crashes!
		pFiler->readItem(&tmpStr);
		m_LineKind = wstring(tmpStr);
		acutDelString(tmpStr);

		CString filename;
		dbToStr(this->database(),filename);

#ifdef DEBUG
		acutPrintf(L"\n从文件【%s】读出管线实体 ID【%d】编号【%s】名称【%s】类型【%s】.",
					filename.GetBuffer(),m_LineID,m_LineNO,m_LineName,m_LineKind);
#endif

		wstring fileName(filename.GetBuffer());
		LineEntryFile* entryFile = LineEntryFileManager::RegisterEntryFile(fileName);

		if( m_PointList )
			delete m_PointList;

		this->m_PointList = entryFile->TransferTempLine(m_LineID);

#ifdef DEBUG
		acutPrintf(L"\n从临时管线管理器中得到线段数据，个数为【%d】", ( m_PointList ? m_PointList->size() : 0 ) );
#endif

		//向管线文件中加入管线line
		entryFile->InsertLine(this);
	}

    return pFiler->filerStatus();
}

// Files data out to a DWG file.
//
Acad::ErrorStatus
LineEntry::dwgOutFields(AcDbDwgFiler* pFiler) const
{
    assertReadEnabled();

    AcDbObject::dwgOutFields(pFiler);
    // Since objects of this class will be in the Named
    // Objects Dictionary tree and may be hard referenced
    // by some other object, to support wblock we need to
    // file out our owner as a hard pointer ID so that it
    // will be added to the list of objects to be wblocked.
    //
    if (pFiler->filerType() == AcDb::kWblockCloneFiler)
        pFiler->writeHardPointerId((AcDbHardPointerId)ownerId());

#ifdef DEBUG
	acutPrintf(L"保存管线实体到数据库 ID【%d】编号【%s】名称【%s】类型【%s】\n",
				m_LineID,m_LineNO,m_LineName,m_LineKind);
#endif

    pFiler->writeItem(Adesk::UInt32(m_LineID));

	pFiler->writeItem(m_LineNO.c_str());
	pFiler->writeItem(m_LineName.c_str());
	pFiler->writeItem(m_LineKind.c_str());

    return pFiler->filerStatus();
}

// Files data in from a DXF file.
//
Acad::ErrorStatus
LineEntry::dxfInFields(AcDbDxfFiler* pFiler)
{
    assertWriteEnabled();

    Acad::ErrorStatus es;
    if ((es = AcDbObject::dxfInFields(pFiler))
        != Acad::eOk)
    {
        return es;
    }

    // Check if we're at the right subclass getLineID marker.
    //
    if (!pFiler->atSubclassData(_T("LineEntryData"))) {
        return Acad::eBadDxfSequence;
    }

    struct resbuf inbuf;
    while (es == Acad::eOk) {
        if ((es = pFiler->readItem(&inbuf)) == Acad::eOk) {

			switch ( inbuf.restype )
			{
				case AcDb::kDxfInt16:
					m_LineID = inbuf.resval.rint;
				//case AcDb::kDxfInt16 + 1:
					//mSequenceNO = inbuf.resval.rint;
			}
        }
    }

    return pFiler->filerStatus();
}

// Files data out to a DXF file.
//
Acad::ErrorStatus
LineEntry::dxfOutFields(AcDbDxfFiler* pFiler) const
{
    assertReadEnabled();

    AcDbObject::dxfOutFields(pFiler);
    pFiler->writeItem(AcDb::kDxfSubclass, _T("LineEntryData"));
    //pFiler->writeItem(AcDb::kDxfInt16, mLineID);
	//pFiler->writeItem(AcDb::kDxfInt16 + 1, mSequenceNO);

    return pFiler->filerStatus();
}


///////////////////////////////////////////////////////////////////////////
// Implementation LineEntryFile

/**
 * 管线实体文件
 */
LineEntryFile::LineEntryFile(const wstring& fileName, bool import)
	:m_FileName(fileName)
{
	m_LineList = new LineList();
	m_LinePoint = new LinePointMap();

	if( import )
		Import();
}

LineEntryFile::~LineEntryFile()
{
	if( m_LineList )
	{
		for( LineIterator iter = m_LineList->begin();
				iter != m_LineList->end();
				iter++ )
		{
			(*iter)->close();
		}

		delete m_LineList;
	}

	if( m_LinePoint )
		delete m_LinePoint;
}

void LineEntryFile::Import()
{
	CFile archiveFile;

	//read data from file PERSISTENT_FILE
	BOOL result = archiveFile.Open(this->m_FileName.c_str(),CFile::modeRead);
	if( !result )
	{
		acutPrintf(L"打开管线实体数据文件失败");
		return;
	}

	//得到文件内容长度
	int length = (int)archiveFile.GetLength()+1;

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

#ifdef DEBUG
		acutPrintf(L"得到一行管线实体数据【%s】\n",wLine.c_str());
#endif

		if(wLine.length() == 0)
			break;

		LineEntry *newLine = new LineEntry(wLine);
		m_LineList->push_back( newLine );

		//创建对应的图层
		ArxWrapper::createNewLayer( newLine->m_LineName );

		//创建相应的柱体
		ArxWrapper::createLMALine(*newLine );

		//从下一个字符开始查找另外一行
		lineFrom = linePos + 1;
		linePos = wContent.find_first_of(L"\n",lineFrom + 1);
	}

	//关闭文件
	archiveFile.Close();
}

void LineEntryFile::Persistent() const
{
	acutPrintf(L"开始导出管线实体数据\n");

	CString exportFile;
	exportFile.Format(L"%s导出.txt",this->m_FileName.c_str());
	CFile archiveFile(exportFile,CFile::modeCreate|CFile::modeWrite);

	//遍历所有的类型定义
	for( ConstLineIterator iter = m_LineList->begin(); 
			iter != m_LineList->end(); 
			iter++)
	{
		LineEntry* data = *iter;

		if( data )
		{
			//得到消息的宽字符序列化
			wstring wData = data->toString();

			//转为窄字符
			string dataStr = WstringToString(wData);

#ifdef DEBUG
			acutPrintf(L"\n管线实体数据【%s】.",wData.c_str());
#endif
			//使用 virtual void Write( const void* lpBuf, UINT nCount ); 将窄字符写入文件
			archiveFile.Write(dataStr.c_str(),(UINT)dataStr.size());
			archiveFile.Write("\n",(UINT)strlen("\n"));
		}
	}

	acutPrintf(L"\n管线实体数据保存完成.");
	archiveFile.Close();
}

void LineEntryFile::InsertLine(LineEntry* lineEntry)
{
	if( lineEntry )
		m_LineList->push_back(lineEntry);
}

BOOL LineEntryFile::UpdateLine(LineEntry* lineEntry)
{
	LineIterator iter = FindLinePos(lineEntry->m_LineID);

	if( iter != this->m_LineList->end())
	{
		(*iter)->m_LineName = lineEntry->m_LineName;
		(*iter)->m_LineNO = lineEntry->m_LineNO;

		return TRUE;
	}

	return FALSE;
}

BOOL LineEntryFile::DeleteLine( const UINT& lineID )
{
	LineIterator iter = FindLinePos(lineID);

	if( iter != this->m_LineList->end())
	{
		m_LineList->erase(iter);
		return TRUE;
	}
	else
		return FALSE;
}

LineIterator LineEntryFile::FindLinePos( const UINT& lineID ) const
{
	for( LineIterator iter = this->m_LineList->begin();
			iter != this->m_LineList->end();
			iter++)
	{
		if( (*iter)->m_LineID == lineID )
			return iter;
	}

	return m_LineList->end();
}

LineIterator LineEntryFile::FindLinePosByNO( const wstring& lineNO ) const
{
	for( LineIterator iter = this->m_LineList->begin();
			iter != this->m_LineList->end();
			iter++)
	{
		if( (*iter)->m_LineNO == lineNO )
			return iter;
	}

	return m_LineList->end();
}

LineIterator LineEntryFile::FindLinePosByName( const wstring& lineName ) const
{
	for( LineIterator iter = this->m_LineList->begin();
			iter != this->m_LineList->end();
			iter++)
	{
		if( (*iter)->m_LineName == lineName )
			return iter;
	}

	return m_LineList->end();
}

LineEntry* LineEntryFile::FindLine( const UINT& lineID ) const
{
	LineIterator iter = FindLinePos(lineID);

	if( iter != m_LineList->end())
		return (*iter);
	else
		return NULL;
}

LineEntry* LineEntryFile::FindLineByName( const wstring& lineName ) const
{
	LineIterator iter = FindLinePosByName(lineName);

	if( iter != m_LineList->end())
		return (*iter);
	else
		return NULL;
}

LineEntry* LineEntryFile::FindLineByNO( const wstring& lineNO ) const
{
	LineIterator iter = FindLinePosByNO(lineNO);

	if( iter != m_LineList->end())
		return (*iter);
	else
		return NULL;
}

LineEntry* LineEntryFile::HasAnotherLineByNO( const UINT& lineID, const wstring& lineNO  ) const
{
	for( LineIterator iter = this->m_LineList->begin();
			iter != this->m_LineList->end();
			iter++)
	{
		if( (*iter)->m_LineNO == lineNO && (*iter)->m_LineID != lineID)
			return (*iter);
	}

	return NULL;
}

LineEntry* LineEntryFile::HasAnotherLineByByName( const UINT& lineID, const wstring& lineName  ) const
{
	for( LineIterator iter = this->m_LineList->begin();
			iter != this->m_LineList->end();
			iter++)
	{
		if( (*iter)->m_LineName == lineName && (*iter)->m_LineID != lineID)
			return (*iter);
	}

	return NULL;
}

PointList* LineEntryFile::GetTempLine( const UINT& lineID )
{
	LinePointMap::iterator iter = m_LinePoint->find(lineID);

	if( iter == m_LinePoint->end() )
	{
		PointList* newList = new PointList();
		m_LinePoint->insert( std::pair<UINT,PointList*>(lineID,newList));

		return newList;
	}
	else
	{
		return iter->second;
	}
}

PointList* LineEntryFile::TransferTempLine( const UINT& lineID )
{
	LinePointMap::iterator iter = m_LinePoint->find(lineID);

	if( iter == m_LinePoint->end() )
	{
		return NULL;
	}
	else
	{
		PointList* findList = iter->second;
		m_LinePoint->erase(iter);

		return findList;
	}
}

/////////////////////////////////////////////////////////////////////////

EntryFileList* LineEntryFileManager::pEntryFileList = NULL;

void LineEntryFileManager::ReadFromCurrentDWG()
{
#ifdef DEBUG
	acutPrintf(L"从当前DWG文件读取数据。");
#endif

	ArxWrapper::PullFromNameObjectsDict();
}

void LineEntryFileManager::RemoveEntryFileOnDWGUnLoad()
{
#ifdef DEBUG
		acutPrintf(L"DWG文件关闭了，删除管理数据。");

		if( pEntryFileList )
		{
			for( EntryFileIter iter = pEntryFileList->begin();
			iter != pEntryFileList->end();
			iter++)
			{
				delete (*iter);
			}

			pEntryFileList->clear();
		}
#endif
}

LineEntryFile* LineEntryFileManager::GetLineEntryFile( const wstring& fileName )
{
	if( pEntryFileList == NULL )
	{
		pEntryFileList = new EntryFileList();
#ifdef DEBUG
		acutPrintf(L"\n文件实体管理器还未创建.");
#endif
		return NULL;
	}

	for( EntryFileIter iter = pEntryFileList->begin();
			iter != pEntryFileList->end();
			iter++)
	{
		if( (*iter)->m_FileName == fileName )
			return (*iter);
	}

#ifdef DEBUG
	acutPrintf(L"\n没有找到文件【%s】对应的管线实体.",fileName.c_str());
#endif

	return NULL;
}

LineEntryFile* LineEntryFileManager::RegisterEntryFile(const wstring& fileName)
{
	LineEntryFile* entryFile = GetLineEntryFile(fileName);
	if( entryFile == NULL )
	{
		acutPrintf(L"\n创建【%s】对应的管线实体.",fileName.c_str());

		entryFile = new LineEntryFile(fileName);
		pEntryFileList->push_back( entryFile );
	}

	return entryFile;
}

bool LineEntryFileManager::RegisterLineSegment( const wstring& fileName, AcDbEntity* pEntry, UINT lineID, UINT sequence, 
										const AcGePoint3d& start, const AcGePoint3d& end )
{
	//找到文件管理类
	LineEntryFile* pFileEntry = RegisterEntryFile(fileName);
	acutPrintf(L"\n添加线段时，找到管线实体文件管理器【%s】.",fileName.c_str());

	//找到实体类
	LineEntry* lineEntry = pFileEntry->FindLine(lineID);
	PointList* pPointList = NULL;

	if( lineEntry == NULL )
	{
#ifdef DEBUG
		acutPrintf(L"\n保存到临时管线管理器中.");
#endif
		pPointList = pFileEntry->GetTempLine( lineID );
	}

	if( sequence == 1 )
	{
#ifdef DEBUG
		acutPrintf(L"\n序列号为1，这是第一个线段.");
#endif
		ads_point startPoint;
		startPoint[X] = start.x;
		startPoint[Y] = start.y;
		startPoint[Z] = start.z;

		PointEntry* tempPoint = new PointEntry(0,startPoint,L"",L"");
		tempPoint->m_pEntry = pEntry;

		if( lineEntry )
		{
			lineEntry->InsertPoint( *tempPoint );
			delete tempPoint;
		}
		else
		{
			pPointList->push_back( tempPoint );
		}

		ads_point endPoint;
		endPoint[X] = end.x;
		endPoint[Y] = end.y;
		endPoint[Z] = end.z;

		tempPoint = new PointEntry(0,endPoint,L"",L"");
		tempPoint->m_pEntry = pEntry;

		if( lineEntry )
		{
			lineEntry->InsertPoint( *tempPoint );
			delete tempPoint;
		}
		else
		{
			pPointList->push_back( tempPoint );
		}
	}
	else if (  sequence > 1 )
	{
#ifdef DEBUG
		acutPrintf(L"\n普通线段.");
#endif
		ads_point endPoint;
		endPoint[X] = end.x;
		endPoint[Y] = end.y;
		endPoint[Z] = end.z;

		PointEntry* tempPoint = new PointEntry(0,endPoint,L"",L"");
		tempPoint->m_pEntry = pEntry;

		if( lineEntry )
		{
			lineEntry->InsertPoint( *tempPoint );
			delete tempPoint;
		}
		else
		{
			pPointList->push_back( tempPoint );
		}
	}
	else if ( sequence == 0)
	{
		acutPrintf(L"\n无效的线段.");
	}

	return true;
}

LineEntryFile* LineEntryFileManager::GetCurrentLineEntryFile()
{
	//Get current filename
	wstring fileName = curDoc()->fileName();

	acutPrintf(L"\n对【%s】的管线进行切图.",fileName.c_str());

	return GetLineEntryFile(fileName);
}

} // end of data

} // end of assistant

} // end of guch

} // end of com