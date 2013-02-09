#ifndef _ARG3_DB_COLUMN_VALUE_H_
#define _ARG3_DB_COLUMN_VALUE_H_

#include <sqlite3.h>
#include <string>
#include <cassert>

using namespace std;

namespace arg3
{
    namespace db
    {
        class column_value //Encapsulates a sqlite3_value*
        {
        public:
            //Constructors / Destructors
            column_value() : m_p(NULL)
            {
            }

            column_value(const column_value &value) : m_p(value.m_p)
            {
            }

            explicit column_value(sqlite3_value *pValue) : m_p(pValue)
            {
            }

            ~column_value()
            {
                if (m_p != NULL)
                    m_p = NULL;
            }

            column_value &operator=(const column_value &value)
            {
                if (this != &value)
                {
                    detach();
                    attach(value.m_p);
                }

                return *this;
            }

            //Methods
            const void *to_blob() const
            {
                //Validate our parameters
                assert(m_p != NULL);

                return sqlite3_value_blob(m_p);
            }

            int blob_size() const
            {
                //Validate our parameters
                assert(m_p != NULL);

                return sqlite3_value_bytes(m_p);
            }

            int to_blob16() const
            {
                //Validate our parameters
                assert(m_p != NULL);

                return sqlite3_value_bytes16(m_p);
            }

            double to_double() const
            {
                //Validate our parameters
                assert(m_p != NULL);

                return sqlite3_value_double(m_p);
            }

            int to_int() const
            {
                //Validate our parameters
                assert(m_p != NULL);

                return sqlite3_value_int(m_p);
            }

            sqlite3_int64 to_int64() const
            {
                //Validate our parameters
                assert(m_p != NULL);

                return sqlite3_value_int64(m_p);
            }

            const unsigned char *to_text() const
            {
                //Validate our parameters
                assert(m_p != NULL);

                return sqlite3_value_text(m_p);
            }

            string to_string() const
            {
                assert(m_p != NULL);

                return reinterpret_cast<const char *>(sqlite3_value_text(m_p));
            }

            const wchar_t *to_text16() const
            {
                //Validate our parameters
                assert(m_p != NULL);

                return reinterpret_cast<const wchar_t *>(sqlite3_value_text16(m_p));
            }

            const void *to_text16le() const
            {
                //Validate our parameters
                assert(m_p != NULL);

                return sqlite3_value_text16le(m_p);
            }

            const void *to_text16be() const
            {
                //Validate our parameters
                assert(m_p != NULL);

                return sqlite3_value_text16be(m_p);
            }

            int type() const
            {
                //Validate our parameters
                assert(m_p != NULL);

                return sqlite3_value_type(m_p);
            }

            int numeric_type() const
            {
                //Validate our parameters
                assert(m_p != NULL);

                return sqlite3_value_numeric_type(m_p);
            }

            operator sqlite3_value *() const
            {
                return m_p;
            }

            void attach(sqlite3_value *pValue)
            {
                //Validate our parameters
                assert(m_p == NULL);

                m_p = pValue;
            }

            sqlite3_value *detach()
            {
                sqlite3_value *pValue = m_p;
                m_p = NULL;
                return pValue;
            }

        protected:
            //Member variables
            sqlite3_value *m_p;
        };

    }
}

#endif
