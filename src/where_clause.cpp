
#include <sstream>
#include "where_clause.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        where_clause::where_clause()
        {
        }

        where_clause::where_clause(const string &value) : value_(value)
        {
        }

        where_clause::where_clause(const where_clause &other) : value_(other.value_), and_(other.and_), or_(other.or_)
        {
        }

        where_clause::where_clause(where_clause &&other) : value_(std::move(other.value_)), and_(std::move(other.and_)), or_(std::move(other.or_))
        {
        }

        where_clause::~where_clause()
        {
        }

        where_clause &where_clause::operator=(const where_clause &other)
        {
            value_ = other.value_;
            and_ = other.and_;
            or_ = other.or_;

            return *this;
        }

        where_clause &where_clause::operator=(where_clause &&other)
        {
            value_ = std::move(other.value_);
            and_ = std::move(other.and_);
            or_ = std::move(other.or_);

            return *this;
        }

        string where_clause::to_string() const
        {
            ostringstream buf;

            if (and_.size() > 0 && or_.size() > 0) {
                buf << "(";
            }

            buf << value_;

            if (and_.size() > 0) {
                buf << " AND ";
                auto it = and_.begin();
                while (it < and_.end() - 1) {
                    buf << it->to_string() << " AND ";
                    ++it;
                }
                if (it != and_.end()) {
                    buf << it->to_string();
                }
                if (or_.size() > 0) {
                    buf << ")";
                }
            }

            if (or_.size() > 0) {
                if (and_.size() > 0) {
                    buf << " OR (";
                } else {
                    buf << " OR ";
                }
                auto it = or_.begin();
                while (it < or_.end() - 1) {
                    buf << it->to_string() << " OR ";
                    ++it;
                }
                if (it != or_.end()) {
                    buf << it->to_string();
                }
                if (and_.size() > 0) {
                    buf << ")";
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
            if (value_.empty())
                value_ = value.to_string();
            else
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
            if (value_.empty())
                value_ = value.to_string();
            else
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

        ostream &operator<<(ostream &out, const where_clause &where)
        {
            out << where.to_string();
            return out;
        }
    }
}
