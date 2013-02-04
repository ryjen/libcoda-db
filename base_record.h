#ifndef _ARG3_DB_BASE_RECORD_H_
#define _ARG3_DB_BASE_RECORD_H_

#include "defines.h"
#include "sqldb.h"

namespace arg3
{
    namespace db
    {
        class sqldb;

        class base_record
        {
        private:
            row mRow;
        public:
            base_record();

            base_record(const row &values);

            virtual columnset columns() const = 0;

            virtual sqldb db() const = 0;

            virtual string tableName() const = 0;

            void save() const;

            variant get(const string &name);

            void set(const string &name, const variant &value);

            void unset(const string &name);
        };

    }
}

#endif
