
#include "where_clause.h"
#include <sstream>

namespace arg3
{
    namespace db
    {
        where_clause::where_clause() {}

        where_clause::where_clause(const string &value) : value_(value) {}

        where_clause::where_clause(const where_clause &other) : value_(other.value_) {}

        where_clause::where_clause(where_clause &&other) : value_(std::move(other.value_)) {}

        where_clause::~where_clause() {}

        where_clause &where_clause::operator=(const where_clause &other)
        {
            value_ = other.value_;

            return *this;
        }

        where_clause &where_clause::operator=(where_clause && other)
        {
            value_ = std::move( other.value_ );
            return *this;
        }

        string where_clause::to_string() const
        {
            ostringstream buf;

            buf << value_;

            if (and_.size() > 0)
            {
                buf << " AND ";
                for (auto & w : and_)
                {
                    buf << w.to_string();
                }
            }

            if (or_.size() > 0)
            {
                buf << " OR ";
                for (auto & w : or_)
                {
                    buf << w.to_string();
                }
            }
            return buf.str();
        }

        bool where_clause::empty() const
        {
            return value_.empty() && and_.empty() && or_.empty();
        }

        where_clause::operator string()
        {
            return to_string();
        }

        where_clause &where_clause::operator&&(const where_clause &value)
        {
            and_.push_back(value);
            return *this;
        }
        where_clause &where_clause::operator&&(const string &value)
        {
            if (value_.empty())
                value_ = value;
            else
                and_.push_back(where_clause(value));
            return *this;
        }
        where_clause &where_clause::operator||(const where_clause &value)
        {
            or_.push_back(value);
            return *this;
        }
        where_clause &where_clause::operator||(const string &value)
        {
            if (value_.empty())
                value_ = value;
            else
                or_.push_back(where_clause(value));
            return *this;
        }

        void where_clause::reset()
        {
            value_.clear();
            and_.clear();
            or_.clear();
        }
    }
}