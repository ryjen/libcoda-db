
#include "join_clause.h"
#include <sstream>

using namespace std;

namespace arg3
{
    namespace db
    {
        join_clause operator"" _join(const char *cstr, size_t len)
        {
            return join_clause(string(cstr, len));
        }

        join_clause::join_clause()
        {
        }

        join_clause::join_clause(const string &tableName, join::type type) : tableName_(tableName), type_(type)
        {
        }

        join_clause::~join_clause()
        {
        }

        join_clause &join_clause::operator=(const join_clause &other)
        {
            tableName_ = other.tableName_;
            type_ = other.type_;
            on_ = other.on_;
            return *this;
        }

        join_clause &join_clause::operator=(join_clause &&other)
        {
            tableName_ = std::move(other.tableName_);
            type_ = other.type_;
            on_ = std::move(other.on_);
            return *this;
        }

        string join_clause::to_string() const
        {
            ostringstream buf;

            switch (type_) {
                default:
                case join::inner:
                    buf << "INNER";
                    break;
                case join::left:
                    buf << "LEFT";
                    break;
                case join::right:
                    buf << "RIGHT";
                    break;
                case join::outer:
                    buf << "OUTER";
                    break;
            }

            buf << " JOIN " << tableName_ << " ON " << on_;

            return buf.str();
        }

        join_clause &join_clause::set_table_name(const string &value)
        {
            tableName_ = value;
            return *this;
        }

        join_clause &join_clause::set_type(join::type value)
        {
            type_ = value;
            return *this;
        }

        where_clause &join_clause::where(const string &value)
        {
            on_ = where_clause(value);
            return on_;
        }

        join_clause &join_clause::where(const where_clause &value)
        {
            on_ = value;
            return *this;
        }

        bool join_clause::empty() const
        {
            return tableName_.empty() || on_.empty();
        }

        join_clause::operator string()
        {
            return to_string();
        }

        void join_clause::reset()
        {
            tableName_.clear();
            on_.reset();
        }

        ostream &operator<<(ostream &out, const join_clause &join)
        {
            out << join.to_string();
            return out;
        }
    }
}
