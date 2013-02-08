#ifndef _ARG3_DB_EXCEPTION_H_
#define _ARG3_DB_EXCEPTION_H_

#include <exception>

namespace arg3
{
	namespace db 
	{
		class database_exception : public std::exception
		{

		};
	}
}

#endif
