/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_COLUMN_VALUE_H_
#define _ARG3_DB_COLUMN_VALUE_H_

#include <sqlite3.h>
#include <string>
#include <cassert>
#include "exception.h"
#include "sql_value.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        class column
        {
        protected:
            sqlite3_value *value_;

            void assert_value() const throw (no_such_column_exception);
        public:
            column();

            virtual ~column();

            column(sqlite3_value *pValue);

            column(const column &other);

            column(column &&other);

            column &operator=(const column &other);

            column &operator=(column &&other);

            bool is_valid() const;

            sql_blob to_blob() const;

            double to_double() const;

            int to_int() const;

            bool to_bool() const;

            sqlite3_int64 to_int64() const;

            const unsigned char *to_text() const;

            string to_string() const;

            const wchar_t *to_text16() const;

            sql_value to_value() const;

            int type() const;

            int numeric_type() const;

            operator sqlite3_value *() const;

            operator string() const;

            operator int() const;

            operator sqlite3_int64() const;

            operator double() const;
        };

    }
}

#endif
