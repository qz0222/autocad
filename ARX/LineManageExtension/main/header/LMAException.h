#pragma once

#include <string>
#include <exception>

using namespace std;

namespace com
{

namespace guch
{

namespace assistant
{

namespace exception
{

class ErrorException : public exception
{
public:

	ErrorException(const wstring& rError);

	wstring errMsg;
};

} // end of exception

} // end of assistant

} // end of guch

} // end of com