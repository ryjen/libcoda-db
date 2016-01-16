
#include "join_clause.h"
#include <sstream>

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

        join_clause::join_clause(const string &tableName, join::type joinType) : tableName_(tableName), type_(joinType)
        {
        }

        join_clause::join_clause(const join_clause &other) : tableName_(other.tableName_), columns_(other.columns_)
        {
        }

        join_clause::join_clause(join_clause &&other) : tableName_(std::move(other.tableName_)), columns_(std::move(other.columns_))
        {
        }

        join_clause::~join_clause()
        {
        }

        join_clause &join_clause::operator=(const join_clause &other)
        {
            tableName_ = other.tableName_;
            type_ = other.type_;
            columns_ = other.columns_;

            return *this;
        }

        join_clause &join_clause::operator=(join_clause &&other)
        {
            tableName_ = std::move(other.tableName_);
            type_ = other.type_;
            columns_ = other.columns_;

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

            buf << " JOIN " << tableName_ << " ON ";

            auto it = columns_.begin();

            buf << it->first << " = " << it->second;

            while (++it != columns_.end()) {
                buf << " AND " << it->first << " = " << it->second;
            }

            return buf.str();
        }

        join_clause &join_clause::on(const string &colA, const string &colB)
        {
            columns_[colA] = colB;
            return *this;
        }

        join_clause &join_clause::on(const std::pair<string, string> &cols)
        {
            columns_[cols.first] = cols.second;
            return *this;
        }

        bool join_clause::empty() const
        {
            return tableName_.empty() || columns_.empty();
        }

        join_clause::operator string()
        {
            return to_string();
        }

        void join_clause::reset()
        {
            tableName_.clear();
            columns_.clear();
        }

        ostream &operator<<(ostream &out, const join_clause &join)
        {
            out << join.to_string();
            return out;
        }
    }
}
