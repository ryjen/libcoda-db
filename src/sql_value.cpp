#include "sql_value.h"
#include "base_query.h"
#include "sqldb.h"
#include "exception.h"
#include <sstream>

namespace std
{
    string to_string(const arg3::db::sql_value &value)
    {
        return value.to_string();
    }

    string to_string(const arg3::db::sql_null_type &value)
    {
        return "NULL";
    }

    string to_string(const arg3::db::sql_blob &value)
    {
        return value.to_string();
    }

    string to_string(const std::string &value)
    {
        return value;
    }
}

namespace arg3
{
    namespace db
    {
        const sql_null_type sql_null_type::instance;

        const sql_null_type sql_null = sql_null_type::instance;

        sql_blob::sql_blob(const void *ptr, size_t size, sql_blob::cleanup_method cleanup) : p_(ptr), s_(size), destruct_(cleanup)
        {}

        sql_blob::sql_blob(const void *ptr, size_t size) : p_(ptr), s_(size), destruct_(NULL)
        {}

        sql_blob::sql_blob(const sql_blob &other) : p_(NULL), destruct_(NULL)
        {
            copy(other);
        }

        sql_blob::sql_blob(sql_blob &&other) : p_(other.p_), s_(other.s_), destruct_(other.destruct_)
        {
            other.p_ = NULL;
            other.s_ = 0;
            other.destruct_ = NULL;
        }
        sql_blob &sql_blob::operator=(const sql_blob &other)
        {
            copy(other);
            return *this;
        }

        sql_blob &sql_blob::operator=(sql_blob && other)
        {
            p_ = other.p_;
            s_ = other.s_;
            destruct_ = other.destruct_;

            other.p_ = NULL;
            other.s_ = 0;
            other.destruct_ = NULL;

            return *this;
        }

        void sql_blob::clear()
        {
            if (p_ != NULL && destruct_ != NULL)
            {
                destruct_(const_cast<void *>(p_));
                p_ = NULL;
            }
        }

        sql_blob::~sql_blob()
        {
            clear();
        }

        void sql_blob::copy(const sql_blob &other)
        {
            clear();
            if (other.s_ > 0)
            {
                void *buf = calloc(1, other.s_);
                memcpy(buf, other.p_, other.s_);
                p_ = buf;
            }
            s_ = other.s_;
            destruct_ = other.destruct_;
        }

        const void *sql_blob::ptr() const
        {
            return p_;
        }
        size_t sql_blob::size() const
        {
            return s_;
        }

        string sql_blob::to_string() const
        {
	    if(p_ == NULL) return "0x0";

            ostringstream os;
	    os << hex;
            os << p_;
            return os.str();
        }

        sql_blob::cleanup_method sql_blob::destructor() const
        {
            return destruct_;
        }

        sql_value::sql_value()  : value_(nullptr)
        {
        }

        sql_value::sql_value(const sql_value &other) : value_(other.value_)
        {
        }

        sql_value::sql_value( sql_value &&other) : value_(std::move(other.value_))
        {
            other.value_ = nullptr;
        }

        sql_value &sql_value::operator=(const sql_value &other)
        {
            value_ = other.value_;
            return *this;
        }

        sql_value &sql_value::operator=(sql_value && other)
        {
            value_ = std::move(other.value_);
            other.value_ = nullptr;
            return *this;
        }

        sql_value::~sql_value()
        {
        }

        string sql_value::to_string() const
        {
            return value_.to_string();
        }

        void sql_value::bind_to(bindable *obj, int index) const
        {
            if (value_.is_null())
            {
                obj->bind(index, sql_null);
            }
            else if (value_.is_numeric())
            {
                if (value_.is_real())
                {
                    obj->bind(index, value_.to_double(DOUBLE_DEFAULT));
                }
                else
                {
                    obj->bind(index, value_.to_llong(INT_DEFAULT));
                }
            }
            else if (value_.is_string())
            {
                obj->bind(index, value_.to_string());
            }
            else
            {
                obj->bind(index, value_.to_pointer(), value_.size(), NULL);
            }
        }

        bool sql_value::operator==(const sql_null_type &other) const
        {
            return value_.is_null();
        }
        bool sql_value::operator==(const sql_value &other) const
        {
            return other.value_ == value_;
        }
        bool sql_value::operator==(const int &other) const
        {
            return other == value_;
        }
        bool sql_value::operator==(const long long &other) const
        {
            return other == value_;
        }
        bool sql_value::operator==(const double &other) const
        {
            return other == value_;
        }
        bool sql_value::operator==(const std::string &other) const
        {
            return other == value_;
        }
        bool sql_value::operator!=(const sql_null_type &other) const
        {
            return !operator==(other);
        }
        bool sql_value::operator!=(const sql_value &other) const
        {
            return !operator==(other);
        }
        bool sql_value::operator!=(const int &other) const
        {
            return !operator==(other);
        }
        bool sql_value::operator!=(const long long &other) const
        {
            return !operator==(other);
        }
        bool sql_value::operator!=(const double &other) const
        {
            return !operator==(other);
        }
        bool sql_value::operator!=(const std::string &other) const
        {
            return !operator==(other);
        }

        sql_value::operator std::string() const
        {
            return to_string();
        }

        sql_value::operator bool() const
        {
            return to_bool();
        }

        bool sql_value::to_bool(const bool def) const
        {
            return value_.to_bool();
        }

        sql_value::operator int() const
        {
            return to_int();
        }

        int sql_value::to_int(const int def) const
        {
            return value_.to_int(def);
        }

        sql_value::operator long long() const
        {
            return to_llong();
        }

        long long sql_value::to_llong(const long long def) const
        {
            return value_.to_llong(def);
        }

        sql_value::operator double() const
        {
            return to_double();
        }

        double sql_value::to_double(const double def) const
        {
            return value_.to_double(def);
        }

        sql_value::operator sql_blob() const
        {
            return to_blob();
        }

        sql_blob sql_value::to_blob() const
        {
            return sql_blob(value_.to_pointer(), value_.size(), NULL);
        }

        std::ostream &operator<<(std::ostream &out, const sql_value &value)
        {
            out << value.value_;

            return out;
        }

        std::ostream &operator<<(std::ostream &out, const sql_null_type &value)
        {
            out << "NULL";

            return out;
        }
        std::ostream &operator<<(std::ostream &out, const sql_blob &value)
        {
            out << hex << value.p_;

            return out;
        }


    }
}
