#include "column.h"

namespace arg3
{
    namespace db
    {
    	column::column() : m_p(NULL)
    	{

    	}
        column::column(sqlite3_value *pValue) : m_p(pValue)
        {

        }

        column::column(const column &other) : m_p(other.m_p) {}

        column &column::operator=(const column &other)  {
        	if(this != &other) {
        		m_p = other.m_p;
        	}
        	return *this;
        }

        bool column::is_valid() const {
        	return m_p != NULL;
        }

        void column::assert_value() const throw (no_such_column_exception)
        {
        	if(m_p == NULL)
        	{
        		throw no_such_column_exception();
        	}
        }

        const void *column::to_blob() const
        {
            assert_value();

            return sqlite3_value_blob(m_p);
        }

        int column::blob_size() const
        {
            assert_value();

            return sqlite3_value_bytes(m_p);
        }

        int column::to_blob16() const
        {
            assert_value();

            return sqlite3_value_bytes16(m_p);
        }

        double column::to_double() const
        {
            assert_value();

            return sqlite3_value_double(m_p);
        }

        int column::to_int() const
        {
            assert_value();

            return sqlite3_value_int(m_p);
        }

        sqlite3_int64 column::to_int64() const
        {
            assert_value();

            return sqlite3_value_int64(m_p);
        }

        const unsigned char *column::to_text() const
        {
            assert_value();

            return sqlite3_value_text(m_p);
        }

        string column::to_string() const
        {
            assert_value();

            const unsigned char *textValue = sqlite3_value_text(m_p);

            if(textValue == NULL)
            	return string();

            return reinterpret_cast<const char *>(textValue);
        }

        int column::type() const
        {
            assert_value();

            return sqlite3_value_type(m_p);
        }

        int column::numeric_type() const
        {
            assert_value();

            return sqlite3_value_numeric_type(m_p);
        }

        column::operator sqlite3_value *() const
        {
            return m_p;
        }
    }
}