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
            ostringstream os;
            os << p_;
            return os.str();
        }

        sql_blob::cleanup_method sql_blob::destructor() const
        {
            return destruct_;
        }

        sql_binding_visitor::sql_binding_visitor(bindable *obj, int index) : obj_(obj), index_(index)
        {
            assert(obj_ != NULL);
        }

        void sql_binding_visitor::operator()(int value) const
        {
            obj_->bind(index_, value);
        }
        void sql_binding_visitor::operator()(int64_t value) const
        {
            obj_->bind(index_, value);
        }
        void sql_binding_visitor::operator()(double value) const
        {
            obj_->bind(index_, value);
        }
        void sql_binding_visitor::operator()(const std::string &value) const
        {
            obj_->bind(index_, value);
        }
        void sql_binding_visitor::operator()(const sql_blob &value) const
        {
            obj_->bind(index_, value);
        }
        void sql_binding_visitor::operator()(const sql_null_type &value) const
        {
            obj_->bind(index_, value);
        }

        sql_value::sql_value()  : value_(sql_null)
        {
        }

        sql_value::sql_value(const sql_value &other) : value_(other.value_)
        {
        }

        sql_value::sql_value( sql_value &&other) : value_(std::move(other.value_))
        {
            other.value_ = sql_null;
        }

        sql_value &sql_value::operator=(const sql_value &other)
        {
            value_ = other.value_;
            return *this;
        }

        sql_value &sql_value::operator=(sql_value && other)
        {
            value_ = std::move(other.value_);
            other.value_ = sql_null;
            return *this;
        }

        sql_value::~sql_value()
        {
        }

        string sql_value::to_string() const
        {
            ostringstream os;
            apply_visitor(arg3::db::ostream_sql_value_visitor(os), value_);
            return os.str();
        }

        void sql_value::bind_to(bindable *obj, int index) const
        {
            apply_visitor(sql_binding_visitor(obj, index), value_);
        }
        bool sql_value::operator==(const sql_null_type &other) const
        {
            return apply_visitor(sql_exists_visitor<sql_null_type>(), value_);
        }
        bool sql_value::operator==(const sql_value &other) const
        {
            return other.to_string() == to_string();
        }
        bool sql_value::operator!=(const sql_null_type &other) const
        {
            return !operator==(other);
        }
        bool sql_value::operator!=(const sql_value &other) const
        {
            return !operator==(other);
        }

        sql_value::operator std::string() const
        {
            return to_string();
        }

        sql_value::operator bool() const
        {
            try
            {
                return std::stoi(to_string()) != 0;
            }
            catch (const std::exception &e)
            {

                string data = to_string();

                std::transform(data.begin(), data.end(), data.begin(), std::ptr_fun<int, int>(std::tolower));

                if (data == "true")
                    return true;

                if (data == "yes")
                    return true;

                return false;
            }
        }

        sql_value::operator int() const
        {
            try
            {
                return std::stoi(to_string());
            }
            catch (const std::exception &e)
            {
                return 0;
            }
        }

        sql_value::operator int64_t() const
        {
            try
            {
                return std::stoll(to_string());
            }
            catch (const std::exception &e)
            {
                return 0;
            }
        }

        sql_value::operator double() const
        {
            try
            {
                return std::stod(to_string());
            }
            catch (const std::exception &e)
            {
                return 0;
            }
        }

        sql_value::operator sql_blob() const
        {
            if (!apply_visitor(sql_exists_visitor<sql_blob>(), value_))
                return sql_blob(NULL, 0, NULL);

            return Juice::get<sql_blob>(value_);
        }

        std::ostream &operator<<(std::ostream &out, const sql_value &value)
        {
            apply_visitor(ostream_sql_value_visitor(out), value.value_);

            return out;
        }

        std::ostream &operator<<(std::ostream &out, const sql_null_type &value)
        {
            out << "NULL";

            return out;
        }
        std::ostream &operator<<(std::ostream &out, const sql_blob &value)
        {
            out << value.p_;

            return out;
        }
    }
}
