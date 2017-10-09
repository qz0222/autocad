#include <ArxWrapper.h>
#include <GlobalDataConfig.h>
#include <LineCategoryItemData.h>
#include <LineConfigDataManager.h>
#include <LineEntryData.h>

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

// Hatch
#include <dbhatch.h>

// Leader
#include <dblead.h>
#include <dbmleader.h>

// Custom object
#include <ArxCustomObject.h>

#include <LMAException.h>

#include <LMAUtils.h>

void TestHatch();
void TestText();
void TestLeader();
void TestMLeader();

using namespace com::guch::assistant::arx;
using namespace com::guch::assistant::data;
using namespace com::guch::assistant::config;
using namespace com::guch::assistant::exception;

const double ArxWrapper::kPi       = 3.14159265358979323846;
const double ArxWrapper::kHalfPi   = 3.14159265358979323846 / 2.0;
const double ArxWrapper::kTwoPi	  = 3.14159265358979323846 * 2.0;
const double ArxWrapper::kRad45    = 3.14159265358979323846 / 4.0;
const double ArxWrapper::kRad90    = 3.14159265358979323846 / 2.0;
const double ArxWrapper::kRad135   = (3.14159265358979323846 * 3.0) / 4.0;
const double ArxWrapper::kRad180   = 3.14159265358979323846;
const double ArxWrapper::kRad270   = 3.14159265358979323846 * 1.5;
const double ArxWrapper::kRad360   = 3.14159265358979323846 * 2.0;

/**
 * 将对象放置在命名词典中
 **/
AcDbObjectId ArxWrapper::PostToNameObjectsDict(AcDbObject* pObj,const wstring& key, bool toDelete )
{
	AcDbObjectId id;

	try
	{
		AcDbDictionary *pNamedobj;
		acdbHostApplicationServices()->workingDatabase()->
			getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);

		// Check to see if the dictionary we want to create is
		// already present. If not, create it and add
		// it to the named object dictionary.
		//
		AcDbDictionary *pDict;
		if (pNamedobj->getAt(key.c_str(), (AcDbObject*&) pDict,
			AcDb::kForWrite) == Acad::eKeyNotFound)
		{
			pDict = new AcDbDictionary;
			AcDbObjectId DictId;
			pNamedobj->setAt(key.c_str(), pDict, DictId);
		}
		pNamedobj->close();

		if (pDict) 
		{
			// New objects to add to the new dictionary, then close them.
			LineEntry* pLineEntry = LineEntry::cast(pObj);

			if( pLineEntry )
			{
				if( toDelete )
				{
					acutPrintf(L"\n从命名词典删除管线【%s】",pLineEntry->m_LineName.c_str());
					
					//首先关闭对象，为了下面以可写方式打开，进行删除
					pLineEntry->close();

					//以可写方式打开
					AcDbObject* pObjToDel = NULL;
					Acad::ErrorStatus es = pDict->getAt(pLineEntry->m_LineName.c_str(),pObjToDel,AcDb::kForWrite);

					if( es == Acad::eOk )
					{
						//对象自身的erased flag被设置，这样在保存的时候会被过滤掉
						//奇怪的是对象自身设置标志位后，并没有通知数据库（文件）更新，也就没有保存
						Acad::ErrorStatus es = pObjToDel->erase();
						pObjToDel->close();

						if (es != Acad::eOk)
						{
							acutPrintf(L"\n删除失败！");
							rxErrorMsg(es);
						}

						//从命名字典中删除关键字
						pDict->remove(pLineEntry->m_LineName.c_str());
					}
					else
					{
						acutPrintf(L"\n打开被删除的对象失败了！");
						rxErrorMsg(es);
					}
				}
				else
				{
					acutPrintf(L"\n添加管线【%s】到命名词典",pLineEntry->m_LineName.c_str());
					Acad::ErrorStatus es = pDict->setAt(pLineEntry->m_LineName.c_str(), pObj, id);

					if (es != Acad::eOk)
					{
						acutPrintf(L"\n添加失败！");
						rxErrorMsg(es);
					}

					//pObj->close();
				}
			}

			pDict->close();
		}
	}
	catch(const Acad::ErrorStatus es)
	{
		acutPrintf(L"\n操作词典发生异常！");
		rxErrorMsg(es);
	}

	return id;
}

/**

/**
 * 将对象放置从命名词典中读出来
 **/
void ArxWrapper::PullFromNameObjectsDict()
{
	AcDbDictionaryPointer pNamedobj;
	// use a smart pointer to access the objects, the destructor will close them automatically
	pNamedobj.open(acdbHostApplicationServices()->workingDatabase()->namedObjectsDictionaryId(), AcDb::kForRead);
	// if ok
	if (pNamedobj.openStatus() == Acad::eOk)
	{
		AcDbObjectId dictId;
		// get at the dictionary entry itself
		Acad::ErrorStatus es = pNamedobj->getAt(LineEntry::LINE_ENTRY_LAYER.c_str(), dictId);
		// if ok
		if (es == Acad::eOk)
		{
			// now open it for read
			AcDbDictionaryPointer pDict(dictId, AcDb::kForRead);
			// if ok
			if (pDict.openStatus() == Acad::eOk)
			{
				// Get an iterator for the ASDK_DICT dictionary.
				AcDbDictionaryIterator* pDictIter= pDict->newIterator();

				LineEntry *pLineEntry = NULL;
				for (; !pDictIter->done(); pDictIter->next()) 
				{
					// Get the current record, open it for read, and
					es = pDictIter->getObject((AcDbObject*&)pLineEntry, AcDb::kForRead);
					// if ok
					if (es == Acad::eOk)
					{
						pLineEntry->close();
					}
				}
				delete pDictIter;
			}

			// no need to close the dicts as we used smart pointers
		}
	}
}

/**
 * 将对象放置在模型空间中的某一层上
 **/
AcDbObjectId ArxWrapper::PostToModelSpace(AcDbEntity* pEnt,const wstring& layerName )
{
	AcDbObjectId entId;
	Acad::ErrorStatus es;

	if( !pEnt )
		return 0;

#ifdef DEBUG
	acutPrintf(L"\n加入实体【%p】到图层【%s】",pEnt,layerName.c_str());
#endif 

	AcDbBlockTable *pBlockTable(NULL);
	AcDbBlockTableRecord *pBlockTableRecord(NULL);

	try
	{
		//打开块表数据库
		es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForWrite);
		if( es != Acad::eOk )
			throw ErrorException(L"打开块表记录失败");

		//得到模型空间
		es = pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
		if( es != Acad::eOk )
			throw ErrorException(L"打开模块空间记录失败");

		//加入实体
		es = pBlockTableRecord->appendAcDbEntity(entId, pEnt);
		if( es != Acad::eOk )
			throw ErrorException(L"添加实体记录失败");

		es = pEnt->setLayer(layerName.c_str());
		if( es != Acad::eOk )
			throw ErrorException(L"设置实体所在的图层失败");

		//关闭实体
		//pEnt->close();
	}
	catch(const Acad::ErrorStatus es)
	{
		acutPrintf(L"\n操作数据库发生异常！");
		rxErrorMsg(es);
	}
	catch( const ErrorException& e)
	{
		acutPrintf(L"\n创建新的实体记录失败【%s】",e.errMsg.c_str());
		rxErrorMsg(es);
	}

	if( pBlockTableRecord != NULL )
		pBlockTableRecord->close();

	if( pBlockTable != NULL )
		pBlockTable->close();

	return entId;
}


/**
 * 将对象从模型空间中的某一层上删除
 **/
Acad::ErrorStatus ArxWrapper::RemoveFromModelSpace(const AcDbHandle& handle,const wstring& layerName )
{
	AcDbObjectId entId;

	try
	{
		if( handle.isNull() )
			return Acad::eOk;

		//打开块表数据库
		AcDbBlockTable *pBlockTable;
		acdbHostApplicationServices()->workingDatabase()
			->getBlockTable(pBlockTable, AcDb::kForWrite);

		//得到模型空间
		AcDbBlockTableRecord *pBlockTableRecord;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord,
		AcDb::kForWrite);

		//遍历数据库
		AcDbBlockTableRecordIterator* iter;
		Acad::ErrorStatus es = pBlockTableRecord->newIterator(iter);
        if (es != Acad::eOk) 
		{
			acutPrintf(L"\n打开数据库失败了");
            rxErrorMsg(es);
            pBlockTableRecord->close();
        }
        else 
		{
            AcDbEntity* ent;
            for (; !iter->done(); iter->step()) 
			{
                if (iter->getEntity(ent, AcDb::kForWrite) == Acad::eOk) 
				{
					AcDbHandle entHandle;
					ent->getAcDbHandle(entHandle);

					if( !entHandle.isNull() )
					{
						if( entHandle == handle )
						{
#ifdef DEBUG
							acutPrintf(L"\n对象找到了，删除并关闭掉");
#endif						
							ent->erase();
							ent->close();
							break;
						}
					}
					else
					{
						acutPrintf(L"\n根据Handle删除对象时出错！");
					}
                }
            }

			//把迭代器删了，防内存泄露
            delete iter;
        }

		//关闭实体
		pBlockTable->close();
		pBlockTableRecord->close();

		return Acad::eOk;
	}
	catch(const Acad::ErrorStatus es)
	{
		acutPrintf(L"\n操作数据库发生异常！");
		rxErrorMsg(es);

		return Acad::eOutOfMemory;
	}
}

/**
 * 将对象从模型空间中的某一层上删除
 **/
Acad::ErrorStatus ArxWrapper::RemoveFromModelSpace(AcDbEntity* pEnt,const wstring& layerName )
{
	AcDbObjectId entId;

	try
	{
		if( !pEnt )
			return Acad::eOk;

		//打开块表数据库
		AcDbBlockTable *pBlockTable;
		acdbHostApplicationServices()->workingDatabase()
			->getBlockTable(pBlockTable, AcDb::kForWrite);

		//得到模型空间
		AcDbBlockTableRecord *pBlockTableRecord;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord,
		AcDb::kForWrite);

		//遍历数据库
		AcDbBlockTableRecordIterator* iter;
		Acad::ErrorStatus es = pBlockTableRecord->newIterator(iter);
        if (es != Acad::eOk) 
		{
			acutPrintf(L"\n打开数据库失败了");
            rxErrorMsg(es);
            pBlockTableRecord->close();
        }
        else 
		{
            AcDbEntity* ent;
            for (; !iter->done(); iter->step()) 
			{
                if (iter->getEntity(ent, AcDb::kForWrite) == Acad::eOk) 
				{
					if( ent == pEnt )
					{
#ifdef DEBUG
						acutPrintf(L"\n对象找到了，删除并关闭掉");
#endif						
						ent->erase();
						ent->close();
						break;
					}
                }
            }

			//把迭代器删了，防内存泄露
            delete iter;
        }

		//关闭实体
		pBlockTable->close();
		pBlockTableRecord->close();

		return Acad::eOk;
	}
	catch(const Acad::ErrorStatus es)
	{
		acutPrintf(L"\n操作数据库发生异常！");
		rxErrorMsg(es);

		return Acad::eOutOfMemory;
	}
}

/**
 * 创建特定名称的层
 **/
bool ArxWrapper::createNewLayer(const wstring& layerName)
{
	bool result = false;
	Acad::ErrorStatus es;
		
	AcDbLayerTable *pLayerTable;
	AcDbLayerTableRecord *pLayerTableRecord = NULL;

	try
	{
		//打开层表数据库
		es = acdbHostApplicationServices()->workingDatabase()
			->getSymbolTable(pLayerTable, AcDb::kForWrite);

		if( es != Acad::eOk )
			throw ErrorException(L"打开层表记录失败");

		//层表不存在，则创建
		if( pLayerTable->has(layerName.c_str()) )
		{
			acutPrintf(L"\n图层【%s】已存在,无需创建",layerName.c_str());
			pLayerTable->close();

			return true;
		}

#ifdef DEBUG
		acutPrintf(L"\n创建图层【%s】",layerName.c_str());
#endif

		//构建新的层表记录
		AcDbLayerTableRecord *pLayerTableRecord = new AcDbLayerTableRecord;
		es = pLayerTableRecord->setName(layerName.c_str());

		if( es != Acad::eOk )
			throw ErrorException(L"设置新层表记录的名字失败");

		// Defaults are used for other properties of 
		// the layer if they are not otherwise specified.
		es = pLayerTable->add(pLayerTableRecord);
		if( es != Acad::eOk )
			throw ErrorException(L"加入新的层表记录失败");

		result = true; 
	}
	catch( const ErrorException& e)
	{
		acutPrintf(L"\n创建新的层表记录失败【%s】",e.errMsg.c_str());
		rxErrorMsg(es);
	}

	if( pLayerTable != NULL )
		pLayerTable->close();

	if( pLayerTableRecord != NULL )
		pLayerTableRecord->close();

	return result;
}

/**
 * 删除特定图层
 **/
bool ArxWrapper::DeleteLayer(const wstring& layerName)
{
	bool result = false;

	//打开层表数据库
	AcDbLayerTable *pLayerTable;
	Acad::ErrorStatus es = acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pLayerTable, AcDb::kForWrite);

	if( pLayerTable )
	{
		AcDbLayerTableRecord *pLayerTableRecord = NULL;

		//层表不存在，则创建
		if(pLayerTable->has(layerName.c_str()))
		{
			acutPrintf(L"\n删除图层【%s】",layerName.c_str());

			//构建新的层表记录
			AcDbLayerTableRecord *pLayerRecord;

			// Defaults are used for other properties of 
			// the layer if they are not otherwise specified.
			Acad::ErrorStatus es = pLayerTable->getAt(layerName.c_str(),pLayerRecord,AcDb::kForWrite);

			if( pLayerRecord )
			{
				pLayerRecord->erase();
				pLayerRecord->close();

				result = true;
			}
			else
			{
				acutPrintf(L"\n打开图层失败");
				rxErrorMsg(es);
			}
		}
		else
		{
			acutPrintf(L"\n图层【%s】不存在，无法删除",layerName.c_str());
		}

		pLayerTable->close();

		if( pLayerTableRecord )
			pLayerTableRecord->close();
	}
	else
	{
		acutPrintf(L"\n打开层表记录失败！");
		rxErrorMsg(es);
	}

	return result;
}

/**
 * 隐藏其他图层
 **/
bool ArxWrapper::ShowLayer(const wstring& layerName)
{
	bool result = true;

	//打开层表数据库
	AcDbLayerTable *pLayerTable;
    acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pLayerTable, AcDb::kForWrite);

	if( pLayerTable )
	{
		AcDbLayerTableIterator *pLayerIter = NULL;
		Acad::ErrorStatus es = pLayerTable->newIterator(pLayerIter);

		if( es == Acad::eOk )
		{
			for( ;!pLayerIter->done();pLayerIter->step() )
			{
				AcDbLayerTableRecord* pLayerTableRecord = NULL;

				pLayerIter->getRecord(pLayerTableRecord,AcDb::kForWrite);

				if( pLayerTableRecord )
				{
					const TCHAR *pLayerName; 
					pLayerTableRecord->getName(pLayerName);
#ifdef DEBUG
					acutPrintf(L"\n找到层【%s】",pLayerName);

					if( wstring(pLayerName) != wstring(L"0") )
					{
#endif
						if( layerName.length() == 0
							|| wstring(pLayerName) == layerName 
						)
						{
							pLayerTableRecord->setIsOff(false);
#ifdef DEBUG
							acutPrintf(L"，显示该层",pLayerName);
#endif					
						}
						else
						{
							pLayerTableRecord->setIsOff(true);
#ifdef DEBUG
							acutPrintf(L"，隐藏该层",pLayerName);
#endif					
						}
					}
					else
					{
						acutPrintf(L"\n0号图层不处理");
					}

					pLayerTableRecord->close();
				}
			}

			delete pLayerIter;
		}
		else
		{
			acutPrintf(L"\n打开图层表失败");
			rxErrorMsg(es);

			result = false;
		}

		pLayerTable->close();
	}

	return result;
}

/**
 * 根据起始点创建线段，并放置在特定的层上
 **/
AcDbObjectId ArxWrapper::createLine( const AcGePoint3d& start,
							const AcGePoint3d& end,
							const wstring& layerName )
{
    AcDbLine *pLine = new AcDbLine(start, end);
    return ArxWrapper::PostToModelSpace(pLine,layerName);
}

/**
 * 根据起始点队列（向量列表），并放置在特定的层上
 **/
void ArxWrapper::createLine( const Point3dVector& points3d,
							const wstring& layerName )
{
	if( points3d.size() < 2 )
		return;

	AcGePoint3d *pStart = NULL;

	for( Point3dIter iter = points3d.begin();
		iter != points3d.end();
		iter++)
	{
		if( pStart == NULL )
		{
			pStart = *iter;
			continue;
		}
		else
		{
			createLine( *pStart, *(*iter), layerName );
			pStart = *iter;
		}
	}
}

/**
 * 根据管线实体起始点队列（向量列表），并放置在特定的层上
 **/
void ArxWrapper::createLine( const PointList& points,
							const wstring& layerName )
{
	if( points.size() < 2 )
		return;

	AcGePoint3d *pStart = NULL;

	for( ContstPointIter iter = points.begin();
		iter != points.end();
		iter++)
	{
		if( pStart == NULL )
		{
			pStart = new AcGePoint3d((*iter)->m_Point[X],
										(*iter)->m_Point[Y],
										(*iter)->m_Point[Z]);
			continue;
		}
		else
		{
			AcGePoint3d *pEnd = new AcGePoint3d((*iter)->m_Point[X],
										(*iter)->m_Point[Y],
										(*iter)->m_Point[Z]);

			createLine( *pStart, *pEnd, layerName );

			delete pStart;

			pStart = pEnd;
		}
	}

	if( pStart != NULL )
		delete pStart;
}

/**
 * 根据起始点队列（向量列表），并放置在特定的层上
 **/
AcDbEntity* ArxWrapper::MoveToBottom(AcDbEntity* pEntry)
{
	if( !pEntry )
		return NULL;

	AcGeVector3d vec(-8,10,0);

	AcGeMatrix3d moveMatrix;
	moveMatrix.setToTranslation(vec);

	pEntry->transformBy(moveMatrix);

	return pEntry;
}

/**
 * 根据起始点队列（向量列表），并放置在特定的层上
 **/
AcDb3dSolid* ArxWrapper::DrawCylinder(const UINT& lineID,
										const UINT& sequenceID,
										const AcGePoint3d& start,
										const AcGePoint3d& end,
										const wstring& layerName,
										LineEntry& entry )
{
#ifdef DEBUG
	acutPrintf(L"\n绘制柱体实例，加入到图层空间\n");
#endif

	LMALineDbObject* lmaLineObj = new LMALineDbObject(Adesk::Int32(lineID),
															Adesk::Int32(sequenceID),
															start,end,&entry);
	PostToModelSpace(lmaLineObj,layerName);

	return lmaLineObj;
}

/**
 * 根据导入线段配置，创建多线段3D折线
 **/
void ArxWrapper::createLMALine( const LineEntry& lineEntry )
{
#ifdef DEBUG
	acutPrintf(L"\n在图层【%s】绘制管线实体，端点个数【%d】\n",lineEntry.m_LineName.c_str(),lineEntry.m_PointList->size());
#endif

	try
	{
		//首先创建图层
		createNewLayer(lineEntry.m_LineName);

		//绘制3D模型
		DrawPolyCylinder(lineEntry);
	}
	catch(const Acad::ErrorStatus es)
	{
		acutPrintf(L"\n绘制线段发生异常！");
		rxErrorMsg(es);
	}
}

/**
 * 创建多线段3D折线
 **/
void ArxWrapper::DrawPolyCylinder( const LineEntry& lineEntry )
{
	const PointList& points = *(lineEntry.m_PointList);
	const wstring& layerName = lineEntry.m_LineName;

	if( points.size() < 2 )
		return;

	AcGePoint3d *pStart = NULL;

	for( ContstPointIter iter = points.begin();
		iter != points.end();
		iter++)
	{
		if( pStart == NULL )
		{
			//多线段的第一个起点
			pStart = new AcGePoint3d((*iter)->m_Point[X],
										(*iter)->m_Point[Y],
										(*iter)->m_Point[Z]);
			continue;
		}
		else
		{
			AcGePoint3d *pEnd = new AcGePoint3d((*iter)->m_Point[X],
										(*iter)->m_Point[Y],
										(*iter)->m_Point[Z]);

			//创建3D柱体代表直线
			AcDb3dSolid* pNewLine = DrawCylinder( lineEntry.m_LineID, (*iter)->m_PointNO, *pStart, *pEnd, layerName, const_cast<LineEntry&>(lineEntry) );

			//保存实例的ObjectID
			//(*iter)->m_EntryId = pNewLine->objectId();
			(*iter)->m_pEntry = pNewLine;

			//删除临时对象
			delete pStart;

			//继续下一个线段
			pStart = pEnd;
		}
	}

	if( pStart != NULL )
		delete pStart;
}

/**
 * 根据多线段的配置，删除3D管线
 **/
void ArxWrapper::eraseLMALine(const LineEntry& lineEntry, bool old)
{
	PointList* pPointList = old ? lineEntry.m_PrePointList : lineEntry.m_PointList;

	if( pPointList == NULL )
	{
#ifdef DEBUG
		acutPrintf(L"\n管线没有【%s】的线段",(old ? L"无效" : L"当前"));
#endif
		return;
	}

	const PointList& points = old ? *(lineEntry.m_PrePointList) : *(lineEntry.m_PointList);
	const wstring& layerName = lineEntry.m_LineName;

#ifdef DEBUG
	acutPrintf(L"\n删除管线【%s】所有【%s】的线段共【%d】条",lineEntry.m_LineName.c_str(),(old ? L"无效" : L"当前"),points.size());
#endif

	if( points.size() < 2 )
		return;

	AcGePoint3d *pStart = NULL;

	for( ContstPointIter iter = points.begin();
		iter != points.end();
		iter++)
	{
		if( iter == points.begin() )
		{
			continue;
		}
		else
		{
			//得到线段的数据库对象ID
			//AcDbObjectId objId = (*iter)->m_EntryId;
			if( (*iter)->m_pEntry )
			{
#ifdef DEBUG
				acutPrintf(L"\n线段终点 序号【%d】 坐标 x:【%lf】y:【%lf】z:【%lf】被删除",(*iter)->m_PointNO,(*iter)->m_Point[X],(*iter)->m_Point[Y],(*iter)->m_Point[Z]);
#endif

				LMALineDbObject* pLineObject = dynamic_cast<LMALineDbObject*>((*iter)->m_pEntry);

				if( pLineObject )
				{
					RemoveFromModelSpace(pLineObject->mHandleDim,lineEntry.m_LineName);
					
					RemoveFromModelSpace(pLineObject->mHandleText,lineEntry.m_LineName);

					RemoveFromModelSpace(pLineObject,lineEntry.m_LineName);
				}
			}
		}
	}
}


/**
 * 切换当前视图到与WCS的viewDirection相一致的视图
 **/
void ArxWrapper::ChangeView(int viewDirection)
{
	AcDbViewTableRecord view;

    // get desired view direction
    AcGeVector3d viewDir = -AcGeVector3d::kYAxis;
	/*
    if (prViewDir.isKeyWordPicked(_T("Top")))
        viewDir = AcGeVector3d::kZAxis;
    else if (prViewDir.isKeyWordPicked(_T("BOttom")))
        viewDir = -AcGeVector3d::kZAxis;
    else if (prViewDir.isKeyWordPicked(_T("BAck")))
        viewDir = AcGeVector3d::kYAxis;
    else if (prViewDir.isKeyWordPicked(_T("Front")))
        viewDir = -AcGeVector3d::kYAxis;
    else if (prViewDir.isKeyWordPicked(_T("Right")))
        viewDir = AcGeVector3d::kXAxis;
    else if (prViewDir.isKeyWordPicked(_T("Left")))
        viewDir = -AcGeVector3d::kXAxis;
    else {
        ASSERT(0);
    }
	*/

#ifdef DEBUG
	acutPrintf(L"\n设置当前视图为前视,中心点在50");
#endif

	wstring cmdViewDir;

	if( viewDirection == 1 )
		cmdViewDir = L"RIGHT";
	else if ( viewDirection == 2 )
		cmdViewDir =  L"FRONT";
	else if  ( viewDirection == 3 )
		cmdViewDir =  L"TOP";

#ifdef DEBUG
	acutPrintf(L"\n设置当前视图为前视为【%s】",cmdViewDir.c_str());
#endif

	acedCommand(RTSTR, _T("._-VIEW"), RTSTR, cmdViewDir.c_str(), 0);

    //view.setViewDirection(viewDir);
	//view.setCenterPoint(AcGePoint2d(50, 50));

	// 设置视图的中心点
	//view.setCenterPoint(AcGePoint2d((xMin + xMax) / 2,  (yMin + yMax) / 2));

	// 设置视图的高度和宽度
	//view.setHeight(fabs(yMax - yMin));
	//view.setWidth(fabs(xMax - xMin));

	// 将视图对象设置为当前视图
	//Acad::ErrorStatus es = acedSetCurrentView(&view, NULL);
}

AcDbObjectId ArxWrapper::CreateHatch(AcDbObjectIdArray objIds,const wstring& patName, bool bAssociative, const wstring& layerName, const AcGeVector3d& normal, const double& elevation)
{
	Acad::ErrorStatus es;
	AcDbHatch *pHatch = new AcDbHatch();
	
	// 设置填充平面
	pHatch->setNormal(normal);
	pHatch->setElevation(elevation);
	
	// 设置关联性
	pHatch->setAssociative(bAssociative);
	
	// 设置填充图案
#ifdef DEBUG
	acutPrintf(L"\n设置填充图案为【%s】",patName.c_str());
#endif

	pHatch->setPattern(AcDbHatch::kPreDefined, patName.c_str());
	
	// 添加填充边界
	es = pHatch->appendLoop(AcDbHatch::kExternal, objIds);
	
	// 显示填充对象
	es = pHatch->evaluateHatch();
	
	// 添加到模型空间
	AcDbObjectId hatchId;
	hatchId = PostToModelSpace(pHatch,layerName.c_str());
	
	// 如果是关联性的填充，将填充对象与边界绑定，以便使其能获得边界对象修改的通知
	if (bAssociative)
	{
		AcDbEntity *pEnt;
		for (int i = 0; i < objIds.length(); i++)
		{
			es = acdbOpenAcDbEntity(pEnt, objIds[i], AcDb::kForWrite);
			if (es == Acad::eOk)
			{
				// 添加一个永久反应器
				pEnt->addPersistentReactor(hatchId);
				pEnt->close();
			}
		}
	}
	
	return hatchId;
}

AcDbObjectId ArxWrapper::CreateHatch(AcDbEntity* entity,const wstring& patName, bool bAssociative, const wstring& layerName, const AcGePlane& plane, const double& distance  )
{
	if( entity == NULL )
		return 0;

	AcDbObjectId objId = entity->objectId();
			
	AcGeVector3d normal = plane.normal();
	//double distance = plane.distanceTo(AcGePoint3d::kOrigin);

#ifdef DEBUG
		acutPrintf(L"\n该平面到原点的距离是【%lf】",distance);
#endif

	AcDbObjectIdArray objIds;
	objIds.append(objId);

	return ArxWrapper::CreateHatch(objIds,patName,bAssociative,layerName,normal,distance);
}

AcDbObjectId ArxWrapper::CreateMLeader(const AcGePoint3d& start, const int& offset, const int& direction,
											const wstring& content, const wstring& layerName)
{
	static int leaderOffset = 6;

	//标注的起点
	AcGePoint3d startPoint(start);

		//进行相应的转换
	{
		if( direction == 1 )
		{
#ifdef DEBUG
			acutPrintf(L"\n切面与X轴垂直,把Z轴位置转换为Y，把Y轴位置转换为X");
#endif
			startPoint.y = start.z;
			startPoint.x = start.y;
		}
		else if ( direction == 2 )
		{
#ifdef DEBUG
			acutPrintf(L"\n切面与Y轴垂直，把Z轴位置转换为Y轴位置");
#endif		
			startPoint.y = start.z;
			startPoint.z = 0;
		} 
	}

	//折点为编译6个单位，且位置在左下方
	AcGePoint3d endPoint(start.x - leaderOffset, start.y - leaderOffset, start.z);

    AcDbMLeader* leader = new AcDbMLeader;

	//设置标注的内容
	{
		int index = 0;
		leader->addLeaderLine(startPoint,index);

		leader->addLastVertex(index,endPoint);
		leader->setLastVertex(index,endPoint);

		leader->setContentType(AcDbMLeaderStyle::kMTextContent);

		//设置标注的文字
		AcDbMText* mtext = new AcDbMText;
		mtext->setContents(content.c_str());

		mtext->setTextHeight(mtext->textHeight()/2);

		leader->setMText(mtext);
	}

	//进行相应的转换
	{
		if( direction == 1 )
		{
#ifdef DEBUG
			acutPrintf(L"\n切面与X轴垂直,先沿X轴翻转到XZ平面，然后绕Z轴翻转到ZY平面，最后沿X轴平移");
#endif
			//进行翻转到XZ平面
			AcGeMatrix3d rotateMatrix = AcGeMatrix3d::rotation( ArxWrapper::kRad90, AcGeVector3d::kYAxis, AcGePoint3d::kOrigin);
			leader->transformBy(rotateMatrix);

			//进行翻转到YZ平面
			rotateMatrix = AcGeMatrix3d::rotation( ArxWrapper::kRad90, AcGeVector3d::kXAxis, AcGePoint3d::kOrigin);
			leader->transformBy(rotateMatrix);

			//进行偏移
			AcGeMatrix3d moveMatrix;
			moveMatrix.setToTranslation(AcGeVector3d(offset,0,0));

			leader->transformBy(moveMatrix);
		}
		else if ( direction == 2 )
		{
#ifdef DEBUG
			acutPrintf(L"\n切面与Y轴垂直,先沿X轴翻转到XZ平面，然后沿Y轴平移");
#endif		
			//进行翻转
			AcGeMatrix3d rotateMatrix = AcGeMatrix3d::rotation( ArxWrapper::kRad90, AcGeVector3d::kXAxis, AcGePoint3d::kOrigin);
			leader->transformBy(rotateMatrix);

			//进行偏移
			AcGeMatrix3d moveMatrix;
			moveMatrix.setToTranslation(AcGeVector3d(0,offset,0));

			leader->transformBy(moveMatrix);
		} 
		else if ( direction == 3 )
		{
#ifdef DEBUG
			acutPrintf(L"\n切面与Z轴垂直，因此标注进行偏移即可");
#endif		
			//进行偏移
			AcGeMatrix3d moveMatrix;
			moveMatrix.setToTranslation(AcGeVector3d(0,0,offset));

			leader->transformBy(moveMatrix);
		}
	}

	//添加到模型空间中
	ArxWrapper::PostToModelSpace(leader,layerName);

	return leader->objectId();
}

void ArxWrapper::TestFunction()
{
	//TestHatch();

	//TestText();

	//TestLeader();

	TestMLeader();
}

void TestHatch()
{
	// 提示用户选择填充边界
	ads_name ss;
	int rt = acedSSGet(NULL, NULL, NULL, NULL, ss);

	AcDbObjectIdArray objIds;
	// 初始化填充边界的ID数组
	if (rt == RTNORM)
	{
		long length;
		acedSSLength(ss, &length);
		for (int i = 0; i < length; i++)
		{
			ads_name ent;
			acedSSName(ss, i, ent);
			AcDbObjectId objId;
			acdbGetObjectId(objId, ent);
			objIds.append(objId);
		}
	}

	acedSSFree(ss); // 释放选择集

	ArxWrapper::CreateHatch(objIds, L"JIS_LC_20", true, L"0", AcGeVector3d(0,0,1), 0);
}

static AcDbObjectId CreateText(const AcGePoint3d& ptInsert,
		const wchar_t* text, AcDbObjectId style = AcDbObjectId::kNull,
			double height = 2.5, double rotation = 0);

// 实现部分
AcDbObjectId CreateText(const AcGePoint3d& ptInsert,
						const wchar_t* text, AcDbObjectId style,
						double height, double rotation)
{
	AcDbText *pText = new AcDbText(ptInsert, text, style, height, rotation);
	return ArxWrapper::PostToModelSpace(pText,L"0");
}

void TestText()
{
	// Hide the dialog and give control to the editor
    ads_point pt;

    // Get a point
    if (acedGetPoint(NULL, _T("\n选取输入点: "), pt) == RTNORM) {
		// 创建单行文字
		AcGePoint3d ptInsert(pt[X], pt[Y], pt[Z]);
		CreateText(ptInsert, L"测试文字");
	}
}

void TestMLeader()
{
	AcGePoint3dArray vertices;
	AcGePoint3d startPoint,endPoint;

	{
		ads_point pt;

		if (acedGetPoint(NULL, _T("\n选取MLeader开始点: "), pt) != RTNORM) 
			return;

		startPoint.x = pt[X];
		startPoint.y = pt[Y];
		startPoint.z = pt[Z];

		//if (acedGetPoint(NULL, _T("\n选取MLeader结束点: "), pt) != RTNORM) 
		//	return;

		endPoint.x = pt[X] - 9;
		endPoint.y = pt[Y] - 9;
		endPoint.z = pt[Z];
	}

    AcDbMLeader* leader = new AcDbMLeader;

	int index = 0;
	leader->addLeaderLine(startPoint,index);

	leader->addLastVertex(index,endPoint);
	leader->setLastVertex(index,endPoint);

	leader->setContentType(AcDbMLeaderStyle::kMTextContent);

	AcDbMText* mtext = new AcDbMText;
	mtext->setContents(L"动力线1#【半径10】");

	leader->setMText(mtext);
	ArxWrapper::PostToModelSpace(leader,L"0");
}

void TestLeader()
{
    AcGePoint3dArray vertices;
	AcGePoint3d startPoint;

	{
		ads_point pt;

		if (acedGetPoint(NULL, _T("\n选取输入点: "), pt) != RTNORM) 
			return;

		startPoint.x = pt[X];
		startPoint.y = pt[Y];
		startPoint.z = pt[Z];
	}
		
	AcGePoint3d endPoint(startPoint.x + 1,startPoint.y-2,0);
	AcGePoint3d lastPoint(startPoint.x + 5,startPoint.y-2,0);

	vertices.append(startPoint);
	vertices.append(endPoint);
	vertices.append(lastPoint);

    AcDbLeader* leader = new AcDbLeader;

    for (int i=0;i<3;i++)
        leader->appendVertex(vertices[i]);

    leader->setToSplineLeader();    // set to spline just for fun
    leader->setDatabaseDefaults();    // pick up default dimstyle, etc.

	AcDbMText* mtext = new AcDbMText;
    //mtext->setLocation(prInsertPt.value());
    //mtext->setWidth(fabs(prInsertPt.value().x - prCorner.value().x));

	mtext->setContents(L"1#动力线，半径10");
	mtext->setDirection(AcGeVector3d(1,0,0));
	mtext->setLocation(endPoint);
	mtext->setHeight(0.5);

	ArxWrapper::PostToModelSpace(mtext,L"0");

	leader->setAnnotationObjId(mtext->objectId());
	ArxWrapper::PostToModelSpace(leader,L"0");
}
