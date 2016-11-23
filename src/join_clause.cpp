
#include "join_clause.h"
#include <sstream>

using namespace std;

namespace rj
{
    namespace db
    {
        join_clause::join_clause()
        {
        }

        join_clause::join_clause(const string &tableName, join::type type) : tableName_(tableName), type_(type)
        {
        }
        join_clause::join_clause(const string &tableName, const string &alias, join::type type) : tableName_(tableName + " " + alias), type_(type)
        {
        }
        join_clause::~join_clause()
        {
        }

        join_clause::join_clause(const join_clause &other) : tableName_(other.tableName_), type_(other.type_), on_(other.on_)
        {
        }

        join_clause::join_clause(join_clause &&other) : tableName_(std::move(other.tableName_)), type_(other.type_), on_(std::move(other.on_))
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
            string buf;

            switch (type_) {
                default:
                case join::none:
                    break;
                case join::natural:
                    buf = " NATURAL";
                    break;
                case join::inner:
                    buf = " INNER";
                    break;
                case join::left:
                    buf = " LEFT";
                    break;
                case join::right:
                    buf = " RIGHT";
                    break;
                case join::full:
                    buf = " FULL OUTER";
                    break;
                case join::cross:
                    buf = " CROSS";
                    break;
            }

            buf += " JOIN ";
            buf += tableName_;

            if (type_ != join::cross && type_ != join::natural) {
                buf += " ON ";
                buf += on_.to_string();
            }

            return buf;
        }

        join_clause &join_clause::table(const string &value)
        {
            tableName_ = value;
            return *this;
        }
        join_clause &join_clause::table(const string &value, const string &alias)
        {
            tableName_ = value + " " + alias;
            return *this;
        }
        std::string join_clause::table() const
        {
            return tableName_;
        }

        join_clause &join_clause::type(join::type value)
        {
            type_ = value;
            return *this;
        }

        join::type join_clause::type() const
        {
            return type_;
        }

        where_clause &join_clause::on(const string &value)
        {
            on_ = where_clause(value);
            return on_;
        }

        join_clause &join_clause::on(const where_clause &value)
        {
            on_ = value;
            return *this;
        }

        const where_clause &join_clause::on() const
        {
            return on_;
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
