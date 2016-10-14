
#include "where_clause.h"
#include <sstream>

using namespace std;

namespace rj
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
            string buf;

            if (and_.size() > 0 && or_.size() > 0) {
                buf += "(";
            }

            buf += value_;

            if (and_.size() > 0) {
                buf += " AND ";
                auto it = and_.begin();
                while (it < and_.end() - 1) {
                    buf += it->to_string();
                    buf += " AND ";
                    ++it;
                }
                if (it != and_.end()) {
                    buf += it->to_string();
                }
                if (or_.size() > 0) {
                    buf += ")";
                }
            }

            if (or_.size() > 0) {
                if (and_.size() > 0) {
                    buf += " OR (";
                } else {
                    buf += " OR ";
                }
                auto it = or_.begin();
                while (it < or_.end() - 1) {
                    buf += it->to_string();
                    buf += " OR ";
                    ++it;
                }
                if (it != or_.end()) {
                    buf += it->to_string();
                }
                if (and_.size() > 0) {
                    buf += ")";
                }
            }
            return buf;
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

        void where_clause::reset(const std::string &value)
        {
            reset();
            value_ = value;
        }
        void where_clause::reset(const where_clause &other)
        {
            value_ = other.value_;
            and_ = other.and_;
            or_ = other.or_;
        }

        ostream &operator<<(ostream &out, const where_clause &where)
        {
            out << where.to_string();
            return out;
        }


        where_builder::where_builder(const std::shared_ptr<session_impl> &session, bindable *binder) : session_(session), binder_(binder)
        {
            assert(binder != nullptr);
        }

        where_builder::where_builder(const where_builder &other) : where_clause(other), session_(other.session_), binder_(other.binder_)
        {
        }

        where_builder::where_builder(where_builder &&other)
            : where_clause(std::move(other)), session_(std::move(other.session_)), binder_(other.binder_)
        {
        }

        where_builder &where_builder::operator=(const where_builder &other)
        {
            where_clause::operator=(other);
            session_ = other.session_;
            binder_ = other.binder_;
            return *this;
        }

        where_builder &where_builder::operator=(where_builder &&other)
        {
            where_clause::operator=(std::move(other));
            session_ = std::move(other.session_);
            binder_ = other.binder_;
            return *this;
        }

        where_builder::~where_builder()
        {
        }

        where_builder &where_builder::equals(const std::string &column, const sql_value &value)
        {
            size_t index = binder_->num_of_bindings() + 1;
            reset(column + " " + op::EQ + " " + session_->bind_param(index));
            binder_->bind(index, value);
            return *this;
        }

        where_builder &where_builder::and_equals(const std::string &column, const sql_value &value)
        {
            size_t index = binder_->num_of_bindings() + 1;
            operator&&(column + " " + op::EQ + " " + session_->bind_param(index));
            binder_->bind(index, value);
            return *this;
        }

        where_builder &where_builder::or_equals(const std::string &column, const sql_value &value)
        {
            size_t index = binder_->num_of_bindings() + 1;
            operator||(column + " " + op::EQ + " " + session_->bind_param(index));
            binder_->bind(index, value);
            return *this;
        }

        where_builder &where_builder::nequals(const std::string &column, const sql_value &value)
        {
            size_t index = binder_->num_of_bindings() + 1;
            reset(column + " " + op::NEQ + " " + session_->bind_param(index));
            binder_->bind(index, value);
            return *this;
        }
        where_builder &where_builder::and_nequals(const std::string &column, const sql_value &value)
        {
            size_t index = binder_->num_of_bindings() + 1;
            operator&&(column + " " + op::NEQ + " " + session_->bind_param(index));
            binder_->bind(index, value);
            return *this;
        }
        where_builder &where_builder::or_nequals(const std::string &column, const sql_value &value)
        {
            size_t index = binder_->num_of_bindings() + 1;
            operator&&(column + " " + op::NEQ + " " + session_->bind_param(index));
            binder_->bind(index, value);
            return *this;
        }

        where_builder &where_builder::like(const std::string &column, const std::string &pattern)
        {
            reset(column + " " + op::LIKE + " " + pattern);
            return *this;
        }
        where_builder &where_builder::and_like(const std::string &column, const std::string &pattern)
        {
            operator&&(column + " " + op::LIKE + " " + pattern);
            return *this;
        }
        where_builder &where_builder::or_like(const std::string &column, const std::string &pattern)
        {
            operator||(column + " " + op::LIKE + " " + pattern);
            return *this;
        }

        where_builder &where_builder::in(const std::string &column, const std::vector<sql_value> &values)
        {
            reset(column + " " + op::IN + " (" + helper::join_csv(values) + ")");
            binder_->bind(values, binder_->num_of_bindings() + 1);
            return *this;
        }
        where_builder &where_builder::and_in(const std::string &column, const std::vector<sql_value> &values)
        {
            operator&&(column + " " + op::IN + " (" + helper::join_csv(values) + ")");
            binder_->bind(values, binder_->num_of_bindings() + 1);
            return *this;
        }
        where_builder &where_builder::or_in(const std::string &column, const std::vector<sql_value> &values)
        {
            operator||(column + " " + op::IN + " (" + helper::join_csv(values) + ")");
            binder_->bind(values, binder_->num_of_bindings() + 1);
            return *this;
        }

        where_builder &where_builder::between(const std::string &column, const sql_value &value1, const sql_value &value2)
        {
            size_t index = binder_->num_of_bindings() + 1;
            reset(column + " " + op::BETWEEN + " " + session_->bind_param(index));
            binder_->bind(index, value1);
            binder_->bind(index + 1, value2);
            return *this;
        }
        where_builder &where_builder::and_between(const std::string &column, const sql_value &value1, const sql_value &value2)
        {
            size_t index = binder_->num_of_bindings() + 1;
            operator&&(column + " " + op::BETWEEN + " " + session_->bind_param(index));
            binder_->bind(index, value1);
            binder_->bind(index + 1, value2);
            return *this;
        }
        where_builder &where_builder::or_between(const std::string &column, const sql_value &value1, const sql_value &value2)
        {
            size_t index = binder_->num_of_bindings() + 1;
            operator||(column + " " + op::BETWEEN + " " + session_->bind_param(index));
            binder_->bind(index, value1);
            binder_->bind(index + 1, value2);
            return *this;
        }

        where_builder &where_builder::bind(size_t index, const sql_value &value)
        {
            binder_->bind(index, value);
            return *this;
        }
        where_builder &where_builder::bind(const std::string &name, const sql_value &value)
        {
            binder_->bind(name, value);
            return *this;
        }

        size_t where_builder::num_of_bindings() const
        {
            return binder_->num_of_bindings();
        }

        /*!
         * Appends and AND part to this where clause
         * @param value   the sql to append
         */
        where_builder &where_builder::operator&&(const std::string &value)
        {
            where_clause::operator&&(value);
            return *this;
        }
        /*!
         * Appends and AND part to this where clause
         * @param value   the sql to append
         */
        where_builder &where_builder::operator&&(const where_clause &value)
        {
            where_clause::operator&&(value);
            return *this;
        }

        /*!
         * Appends and OR part to this where clause
         * @param value   the sql to append
         */
        where_builder &where_builder::operator||(const where_clause &value)
        {
            where_clause::operator||(value);
            return *this;
        }

        /*!
         * Appends and OR part to this where clause
         * @param value   the sql to append
         */
        where_builder &where_builder::operator||(const std::string &value)
        {
            where_clause::operator||(value);
            return *this;
        }
    }
}
