#include "stdafx.h"
#include <string>

using namespace std;

namespace com
{

namespace guch
{

namespace assistant
{

namespace data
{

class GlobalData
{
public:
	
	/**
	* 管线类型
	**/
	static const wstring KIND_LINE;
	static const wstring KIND_SEPERATOR;

	/**
	* 管道种类
	**/
	static const wstring LINE_CATEGORY_SHANGSHUI;
	static const wstring LINE_CATEGORY_XIASHUI;
	static const wstring LINE_CATEGORY_NUANQI;
	static const wstring LINE_CATEGORY_DIANLAN;
	static const wstring LINE_CATEGORY_YUSUI;
	static const wstring LINE_CATEGORY_TONGXIN;

	/**
	* 阻隔体种类
	**/
	static const wstring BLOCK_CATEGORY_WUSHUI;
	static const wstring BLOCK_CATEGORY_HANGDAO;
	static const wstring BLOCK_CATEGORY_NUANQIGUAN;
	static const wstring BLOCK_CATEGORY_GUODAO;
	static const wstring BLOCK_CATEGORY_CHEZHAN;

	/**
	* 穿越方向
	**/
	static const wstring BLOCK_CATEGORY_ABOVE;
	static const wstring BLOCK_CATEGORY_BELLOW;

	/**
	* 管道形状
	**/
	static const wstring LINE_SHAPE_CIRCLE;
	static const wstring LINE_SHAPE_SQUARE;

	/**
	* 管道单位
	**/
	static const wstring LINE_UNIT_MM;
	static const wstring LINE_UNIT_CM;
	static const wstring LINE_UNIT_M;

	/**
	* 字符长度
	**/
	static const UINT ITEM_TEXT_MAX_LENGTH;

	/**
	* 出错标题
	**/
	static const wstring ERROR_DIALOG_CAPTION;
};


} // end of data

} // end of assistant

} // end of guch

} // end of com