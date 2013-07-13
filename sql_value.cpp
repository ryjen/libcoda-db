#include "sql_value.h"
#include <sstream>

namespace arg3
{
    namespace db
    {
        const sql_null_t sql_null = sql_null_t();

        sql_blob::sql_blob(const void *ptr, size_t size, sql_blob::cleanup_method cleanup) : p_(ptr), s_(size), destruct_(cleanup)
        {}

        const void *sql_blob::ptr() const {
            return p_;
        }
        size_t sql_blob::size() const {
            return s_;
        }
        sql_blob::cleanup_method sql_blob::destructor() const {
            return destruct_;
        }

        class ostream_sql_value_visitor
        {
            std::ostream &out_;
        public:
            typedef void result_type;

            ostream_sql_value_visitor(std::ostream &out) : out_(out)
            {}

            void operator()(int value) const
            {
                out_ << value;
            }

            void operator()(int64_t value) const
            {
                out_ << value;
            }

            void operator()(double value) const
            {
                out_ << value;
            }

            void operator()(std::string value) const
            {
                out_ << value;
            }

            void operator()(sql_blob value) const {

            }

            void operator()(sql_null_t value) const
            {
                out_ << "NULL";
            }
        };

        std::ostream &operator<<(std::ostream &out, const sql_value &value)
        {
            apply_visitor(ostream_sql_value_visitor(out), value);

            return out;
        }

        std::ostream &operator<<(std::ostream &out, const sql_null_t &value)
        {
            out << "NULL";

            return out;
        }
    }
}

namespace std
{
    string to_string(const arg3::db::sql_value &value)
    {
        ostringstream os;
        apply_visitor(arg3::db::ostream_sql_value_visitor(os), value);
        return os.str();
    }
}