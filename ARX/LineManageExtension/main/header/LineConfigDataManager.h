#pragma once

#include <vector>
#include <LineCategoryItemData.h>

using namespace std;

namespace com
{

namespace guch
{

namespace assistant
{

namespace config
{

typedef vector<LineCategoryItemData*> LineCategoryVecotr;
typedef LineCategoryVecotr::iterator CategoryIterator;

class LineConfigDataManager
{

public:

	static LineConfigDataManager* Instance();

	LineConfigDataManager(void);
	~LineConfigDataManager(void);

	UINT GetNextID();

	LineCategoryItemData* FindLine( const wstring& lineName ) const;

	LineCategoryItemData* FindByKind( const wstring& lineKind ) const;

	LineCategoryItemData* FindLine( UINT ID ) const;

	void AddLine(const LineCategoryItemData* newLine);

	BOOL UpdateLine(const LineCategoryItemData* updateLine);

	BOOL DeleteLine( UINT ID );

	const wstring FindCategory( const wstring& kind );

	bool ClearAllLines();

	/**
	 * Save the configuration data to file
	 **/
	bool Persistent();

	/**
	 * Save the configuration data to file
	 **/
	const LineCategoryVecotr* GetData() const;

protected:

	bool initialize();

private:

	static LineConfigDataManager* instance;
	static const LPCWSTR PERSISTENT_FILE;

	LineCategoryVecotr* mLineConfigData;
};

} // end of data

} // end of assistant

} // end of guch

} // end of com
