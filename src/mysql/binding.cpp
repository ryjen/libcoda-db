#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include <memory>
#include <cstdlib>
#include <time.h>
#include <cassert>
#include <regex>

#include "../exception.h"
#include "../log.h"
#include "../alloc.h"

#include "binding.h"

namespace arg3
{
    namespace db
    {
        namespace mysql
        {
            namespace helper
            {
                // small util method to make a dynamic c pointer for a type
                template <typename T>
                void *to_ptr(const T &value)
                {
                    T *ptr = c_alloc<T>(sizeof(T));
                    *ptr = value;
                    return ptr;
                }

                void bind_value_from_field(MYSQL_BIND *value, MYSQL_FIELD *field)
                {
                    if (value == nullptr || field == nullptr) {
                        return;
                    }
                    value->buffer_type = field->type;
                    value->is_null = c_alloc<my_bool>();
                    value->is_unsigned = 0;
                    value->error = 0;
                    value->length = c_alloc<unsigned long>();
                    switch (field->type) {
                        default:
                            value->buffer_length = field->length;
                            break;
                        case MYSQL_TYPE_DATETIME:
                        case MYSQL_TYPE_DATE:
                        case MYSQL_TYPE_TIME:
                        case MYSQL_TYPE_TIMESTAMP:
                            value->buffer_length = sizeof(MYSQL_TIME);
                            break;
                    }
                    value->buffer = c_alloc(value->buffer_length);
                }

                void bind_value_copy(MYSQL_BIND *value, const MYSQL_BIND *other)
                {
                    if (value == nullptr || other == nullptr) {
                        return;
                    }

                    if (other->length) {
                        value->length = c_alloc<unsigned long>();
                        memmove(value->length, other->length, sizeof(unsigned long));
                    }

                    if (other->buffer_length > 0 && other->buffer) {
                        value->buffer = c_alloc(other->buffer_length);
                        memmove(value->buffer, other->buffer, other->buffer_length);
                    }

                    if (other->is_null) {
                        value->is_null = c_alloc<my_bool>();
                        memmove(value->is_null, other->is_null, sizeof(my_bool));
                    }

                    if (other->error) {
                        value->error = c_alloc<my_bool>();
                        memmove(value->error, other->error, sizeof(my_bool));
                    }

                    value->buffer_type = other->buffer_type;
                    value->buffer_length = other->buffer_length;
                    value->is_unsigned = other->is_unsigned;
                }

                time_t parse_time(MYSQL_BIND *binding)
                {
                    struct tm sys;
                    MYSQL_TIME *db_tm;

                    if (binding == nullptr) {
                        return 0;
                    }

                    db_tm = (MYSQL_TIME *)binding->buffer;

                    if (db_tm == nullptr) {
                        return 0;
                    }

                    memset(&sys, 0, sizeof(sys));

                    sys.tm_year = db_tm->year - 1900;
                    sys.tm_mon = db_tm->month;
                    sys.tm_mday = db_tm->day;
                    sys.tm_hour = db_tm->hour;
                    sys.tm_min = db_tm->minute;
                    sys.tm_sec = db_tm->second;

                    return timegm(&sys);
                }

                time_t parse_time(const char *value)
                {
                    struct tm *tp;

                    if (value == nullptr) {
                        return 0;
                    }

                    if ((tp = getdate(value))) {
                        return timegm(tp);
                    } else {
                        log::error("unable to get date of %s (%d)", value, getdate_err);
                        return 0;
                    }
                }

                extern std::string last_stmt_error(MYSQL_STMT *stmt);
            }
            namespace data_mapper
            {
                /*
                 * Key method here, handles conversion from MYSQL_BIND to sql_value
                 * TODO: test this more
                 */
                sql_value to_value(MYSQL_BIND *binding)
                {
                    if (binding == nullptr) {
                        return sql_null;
                    }

                    switch (binding->buffer_type) {
                        case MYSQL_TYPE_BIT:
                        case MYSQL_TYPE_TINY:
                        case MYSQL_TYPE_SHORT:
                        case MYSQL_TYPE_LONG:
                        case MYSQL_TYPE_INT24: {
                            if (binding->is_unsigned) {
                                unsigned *p = static_cast<unsigned *>(binding->buffer);
                                return *p;
                            } else {
                                int *p = static_cast<int *>(binding->buffer);
                                return *p;
                            }
                        }
                        case MYSQL_TYPE_LONGLONG: {
                            if (binding->is_unsigned) {
                                unsigned long long *p = static_cast<unsigned long long *>(binding->buffer);
                                return *p;
                            } else {
                                long long *p = static_cast<long long *>(binding->buffer);
                                return *p;
                            }
                        }
                        case MYSQL_TYPE_NULL:
                            return sql_null;
                        case MYSQL_TYPE_TIME:
                            return sql_time(helper::parse_time(binding), sql_time::TIME);
                        case MYSQL_TYPE_DATE:
                            return sql_time(helper::parse_time(binding), sql_time::DATE);
                        case MYSQL_TYPE_TIMESTAMP:
                            return sql_time(helper::parse_time(binding), sql_time::TIMESTAMP);
                        case MYSQL_TYPE_DATETIME:
                            return sql_time(helper::parse_time(binding), sql_time::DATETIME);
                        case MYSQL_TYPE_VAR_STRING:
                        case MYSQL_TYPE_VARCHAR:
                        case MYSQL_TYPE_DECIMAL:
                        case MYSQL_TYPE_SET:
                        case MYSQL_TYPE_ENUM:
                        case MYSQL_TYPE_GEOMETRY:
                        case MYSQL_TYPE_NEWDECIMAL:
                        case MYSQL_TYPE_STRING:
                        default: {
                            return static_cast<const char *>(binding->buffer);
                        }
                        case MYSQL_TYPE_FLOAT: {
                            float *p = static_cast<float *>(binding->buffer);
                            return *p;
                        }
                        case MYSQL_TYPE_DOUBLE: {
                            double *p = static_cast<double *>(binding->buffer);
                            return *p;
                        }
                        case MYSQL_TYPE_TINY_BLOB:
                        case MYSQL_TYPE_MEDIUM_BLOB:
                        case MYSQL_TYPE_LONG_BLOB:
                        case MYSQL_TYPE_BLOB: {
                            if (binding->length) {
                                return sql_blob(binding->buffer, *binding->length);
                            }

                            return sql_blob();
                        }
                    }
                }

                /*
                 * Key method here, handles conversion from a field to sql_value
                 * TODO: test this more with different types
                 * Doesn't throw, prefers to return a null value
                 */
                sql_value to_value(int type, const char *value, size_t length)
                {
                    if (value == nullptr) {
                        throw sql_null;
                    }

                    switch (type) {
                        case MYSQL_TYPE_BIT:
                        case MYSQL_TYPE_TINY:
                        case MYSQL_TYPE_SHORT:
                        case MYSQL_TYPE_LONG: {
                            try {
                                return std::stoi(value);
                            } catch (const std::exception &e) {
                                log::error("unable to get integer from %s", value);
                                return sql_value();
                            }
                        }
                        case MYSQL_TYPE_INT24:
                        case MYSQL_TYPE_LONGLONG: {
                            try {
                                return std::stoll(value);
                            } catch (const std::exception &e) {
                                log::error("unable to get long from %s", value);
                                return sql_value();
                            }
                        }
                        case MYSQL_TYPE_DECIMAL:
                        case MYSQL_TYPE_VARCHAR:
                        case MYSQL_TYPE_VAR_STRING:
                        case MYSQL_TYPE_NEWDECIMAL:
                        case MYSQL_TYPE_GEOMETRY:
                        case MYSQL_TYPE_ENUM:
                        case MYSQL_TYPE_SET:
                        case MYSQL_TYPE_STRING:
                        default:
                            return value;
                        case MYSQL_TYPE_NEWDATE:
                        case MYSQL_TYPE_DATE:
                            return sql_time(helper::parse_time(value), sql_time::DATE);
                        case MYSQL_TYPE_DATETIME:
                            return sql_time(helper::parse_time(value), sql_time::DATETIME);
                        case MYSQL_TYPE_TIMESTAMP:
                            return sql_time(helper::parse_time(value), sql_time::TIMESTAMP);
                        case MYSQL_TYPE_TIME: {
                            return sql_time(helper::parse_time(value), sql_time::TIME);
                        }
                        case MYSQL_TYPE_FLOAT: {
                            try {
                                return std::stof(value);
                            } catch (const std::exception &e) {
                                log::error("unable to get float of %s", value);
                                return sql_value();
                            }
                        }
                        case MYSQL_TYPE_DOUBLE: {
                            try {
                                return std::stod(value);
                            } catch (const std::exception &e) {
                                log::error("unable to get double of %s", value);
                                return sql_value();
                            }
                        }
                        case MYSQL_TYPE_TINY_BLOB:
                        case MYSQL_TYPE_MEDIUM_BLOB:
                        case MYSQL_TYPE_LONG_BLOB:
                        case MYSQL_TYPE_BLOB: {
                            return sql_blob(value, length);
                        }
                        case MYSQL_TYPE_NULL:
                            return sql_null;
                    }
                }

                /*!
                 * TODO: test this more
                 */
                void set_time(MYSQL_BIND *binding, const sql_time &value)
                {
                    MYSQL_TIME *tm = nullptr;

                    if (binding == nullptr) {
                        return;
                    }
                    switch (value.format()) {
                        case sql_time::DATE:
                            binding->buffer_type = MYSQL_TYPE_DATE;
                            break;
                        case sql_time::TIME:
                            binding->buffer_type = MYSQL_TYPE_TIME;
                            break;
                        case sql_time::DATETIME:
                            binding->buffer_type = MYSQL_TYPE_DATETIME;
                            break;
                        case sql_time::TIMESTAMP:
                            binding->buffer_type = MYSQL_TYPE_TIMESTAMP;
                            break;
                    }
                    tm = c_alloc<MYSQL_TIME>();
                    auto gmt = value.to_gmtime();
                    tm->year = gmt->tm_year + 1900;
                    tm->month = gmt->tm_mon;
                    tm->day = gmt->tm_mday;
                    tm->hour = gmt->tm_hour;
                    tm->minute = gmt->tm_min;
                    tm->second = gmt->tm_sec;
                    binding->buffer = tm;
                    binding->buffer_length = sizeof(MYSQL_TIME);
                }
            }


            binding::binding() : value_(nullptr), size_(0)
            {
            }

            binding::binding(size_t size) : value_(nullptr), size_(size)
            {
                value_ = c_alloc<MYSQL_BIND>(size);
            }

            binding::binding(const MYSQL_BIND &value) : value_(nullptr), size_(1)
            {
                copy_value(&value, size_);
            }
            binding::binding(MYSQL_BIND *values, size_t size) : value_(nullptr), size_(size)
            {
                copy_value(values, size);
            }

            binding::binding(MYSQL_FIELD *fields, size_t size) : size_(size)
            {
                value_ = c_alloc<MYSQL_BIND>(size);

                for (size_t i = 0; i < size; i++) {
                    helper::bind_value_from_field(&value_[i], &fields[i]);
                }
            }

            void binding::clear_value(size_t i)
            {
                if (i >= size_) {
                    throw binding_error("invalid index in mysql binding clear");
                }

                if (value_[i].buffer) {
                    free(value_[i].buffer);
                }
                if (value_[i].length) {
                    free(value_[i].length);
                }
                if (value_[i].is_null) {
                    free(value_[i].is_null);
                }
                if (value_[i].error) {
                    free(value_[i].error);
                }
                memset(&value_[i], 0, sizeof(value_[i]));
            }

            void binding::clear_value()
            {
                if (value_) {
                    for (size_t i = 0; i < size_; i++) {
                        clear_value(i);
                    }
                    free(value_);
                    value_ = nullptr;
                }
                size_ = 0;
            }

            void binding::copy_value(const MYSQL_BIND *others, size_t size)
            {
                clear_value();

                value_ = c_alloc<MYSQL_BIND>(size);

                if (value_ == nullptr) {
                    throw std::bad_alloc();
                }

                for (size_t i = 0; i < size; i++) {
                    const MYSQL_BIND *other = &others[i];
                    MYSQL_BIND *value = &value_[i];

                    helper::bind_value_copy(value, other);
                }

                size_ = size;
            }

            binding::binding(const binding &other) : bind_mapping(other), value_(nullptr), size_(0)
            {
                copy_value(other.value_, other.size_);
            }
            binding::binding(binding &&other) : bind_mapping(std::move(other))
            {
                value_ = other.value_;
                size_ = other.size_;
                other.value_ = nullptr;
                other.size_ = 0;
            }

            binding &binding::operator=(const binding &other)
            {
                bind_mapping::operator=(other);
                copy_value(other.value_, other.size_);
                return *this;
            }

            binding &binding::operator=(binding &&other)
            {
                bind_mapping::operator=(std::move(other));
                value_ = other.value_;
                size_ = other.size_;
                other.value_ = nullptr;
                other.size_ = 0;
                return *this;
            }
            binding::~binding()
            {
                clear_value();
            }

            MYSQL_BIND *binding::get(size_t index) const
            {
                if (index >= size_ || value_ == nullptr) {
                    throw binding_error("invalid index in mysql binding get");
                }

                return &value_[index];
            }

            void binding::bind_result(MYSQL_STMT *stmt) const
            {
                if (stmt == nullptr || value_ == nullptr) {
                    return;
                }
                if (mysql_stmt_bind_result(stmt, value_) != 0) {
                    throw binding_error(helper::last_stmt_error(stmt));
                }
            }

            sql_value binding::to_value(size_t index) const
            {
                if (index >= size_ || value_ == nullptr || value_[index].buffer == nullptr) {
                    return sql_null;
                }

                return data_mapper::to_value(&value_[index]);
            }

            int binding::sql_type(size_t index) const
            {
                if (index >= size_ || value_ == nullptr) {
                    return MYSQL_TYPE_NULL;
                }

                return value_[index].buffer_type;
            }

            bool binding::reallocate_value(size_t index)
            {
                // asset non-zero-indexed
                if (index == 0) {
                    return false;
                }

                if (index <= size_) {
                    clear_value(index - 1);
                    return true;
                }

                // dynamic array of parameter values
                if (value_ == nullptr) {
                    value_ = c_alloc<MYSQL_BIND>(index);
                } else {
                    value_ = c_alloc<MYSQL_BIND>(value_, index, size_);
                }

                size_ = index;

                return true;
            }

            std::set<size_t> &binding::get_indexes(size_t index)
            {
                auto &indexes = indexes_[index];

                if (indexes.size() == 0) {
                    indexes.insert(index);
                }
                return indexes;
            }

            /**
             * binding methods ensure the dynamic array is sized properly and store the value as a memory pointer
             */
            binding &binding::bind(size_t index, int value)
            {
                for (size_t i : get_indexes(index)) {
                    if (reallocate_value(i)) {
                        value_[i - 1].buffer_type = MYSQL_TYPE_LONG;
                        value_[i - 1].buffer = helper::to_ptr(value);
                        value_[i - 1].buffer_length = sizeof(value);
                    } else {
                        log::error("unable to reallocate bindings for index %ld", index);
                        break;
                    }
                }

                return *this;
            }
            binding &binding::bind(size_t index, unsigned value)
            {
                for (size_t i : get_indexes(index)) {
                    if (reallocate_value(i)) {
                        value_[i - 1].buffer_type = MYSQL_TYPE_LONG;
                        value_[i - 1].buffer = helper::to_ptr(value);
                        value_[i - 1].buffer_length = sizeof(value);
                        value_[i - 1].is_unsigned = 1;
                    } else {
                        log::error("unable to reallocate bindings for index %ld", index);
                        break;
                    }
                }

                return *this;
            }
            binding &binding::bind(size_t index, long long value)
            {
                for (size_t i : get_indexes(index)) {
                    if (reallocate_value(i)) {
                        value_[i - 1].buffer_type = MYSQL_TYPE_LONGLONG;
                        value_[i - 1].buffer = helper::to_ptr(value);
                        value_[i - 1].buffer_length = sizeof(value);
                    } else {
                        log::error("unable to reallocate bindings for index %ld", index);
                        break;
                    }
                }

                return *this;
            }
            binding &binding::bind(size_t index, unsigned long long value)
            {
                for (size_t i : get_indexes(index)) {
                    if (reallocate_value(i)) {
                        value_[i - 1].buffer_type = MYSQL_TYPE_LONGLONG;
                        value_[i - 1].buffer = helper::to_ptr(value);
                        value_[i - 1].buffer_length = sizeof(value);
                        value_[i - 1].is_unsigned = 1;
                    } else {
                        log::error("unable to reallocate bindings for index %ld", index);
                        break;
                    }
                }

                return *this;
            }
            binding &binding::bind(size_t index, float value)
            {
                for (size_t i : get_indexes(index)) {
                    if (reallocate_value(i)) {
                        value_[i - 1].buffer_type = MYSQL_TYPE_FLOAT;
                        value_[i - 1].buffer = helper::to_ptr(value);
                        value_[i - 1].buffer_length = sizeof(value);
                    } else {
                        log::error("unable to reallocate bindings for index %ld", index);
                        break;
                    }
                }

                return *this;
            }
            binding &binding::bind(size_t index, double value)
            {
                for (size_t i : get_indexes(index)) {
                    if (reallocate_value(i)) {
                        value_[i - 1].buffer_type = MYSQL_TYPE_DOUBLE;
                        value_[i - 1].buffer = helper::to_ptr(value);
                        value_[i - 1].buffer_length = sizeof(value);
                    } else {
                        log::error("unable to reallocate bindings for index %ld", index);
                        break;
                    }
                }

                return *this;
            }
            binding &binding::bind(size_t index, const std::string &value, int len)
            {
                for (size_t i : get_indexes(index)) {
                    if (reallocate_value(i)) {
                        value_[i - 1].buffer_type = MYSQL_TYPE_STRING;
                        auto size = len == -1 ? value.size() : len;
                        value_[i - 1].buffer = strdup(value.c_str());
                        value_[i - 1].buffer_length = size;
                        if (!value_[i - 1].length) {
                            value_[i - 1].length = c_alloc<unsigned long>();
                        }
                        *value_[i - 1].length = size;
                    } else {
                        log::error("unable to reallocate bindings for index %ld", index);
                        break;
                    }
                }

                return *this;
            }
            binding &binding::bind(size_t index, const std::wstring &value, int len)
            {
                for (size_t i : get_indexes(index)) {
                    if (reallocate_value(i)) {
                        value_[i - 1].buffer_type = MYSQL_TYPE_STRING;
                        auto size = len == -1 ? value.size() : len;
                        value_[i - 1].buffer = wcsdup(value.c_str());
                        value_[i - 1].buffer_length = size;
                        if (!value_[i - 1].length) {
                            value_[i - 1].length = c_alloc<unsigned long>();
                        }
                        *value_[i - 1].length = size;
                    } else {
                        log::error("unable to reallocate bindings for index %ld", index);
                        break;
                    }
                }

                return *this;
            }
            binding &binding::bind(size_t index, const sql_blob &value)
            {
                for (size_t i : get_indexes(index)) {
                    if (reallocate_value(i)) {
                        value_[i - 1].buffer_type = MYSQL_TYPE_BLOB;
                        void *ptr = c_alloc(value.size());
                        memcpy(ptr, value.value(), value.size());
                        value_[i - 1].buffer = ptr;
                        value_[i - 1].buffer_length = value.size();
                        if (!value_[i - 1].length) {
                            value_[i - 1].length = c_alloc<unsigned long>();
                        }
                        *value_[i - 1].length = value.size();
                    } else {
                        log::warn("unable to reallocate bindings for index %ld", index);
                        break;
                    }
                }
                return *this;
            }

            binding &binding::bind(size_t index, const sql_null_type &value)
            {
                for (size_t i : get_indexes(index)) {
                    if (reallocate_value(i)) {
                        value_[i - 1].buffer_type = MYSQL_TYPE_NULL;
                    } else {
                        log::warn("unable to reallocate bindings for index %ld", index);
                        break;
                    }
                }
                return *this;
            }

            binding &binding::bind(size_t index, const sql_time &value)
            {
                for (size_t i : get_indexes(index)) {
                    if (reallocate_value(i)) {
                        data_mapper::set_time(&value_[i - 1], value);
                    } else {
                        log::warn("unable to reallocate bindings for index %ld", index);
                        break;
                    }
                }

                return *this;
            }

            binding &binding::bind(const std::string &name, const sql_value &value)
            {
                bind_mapping::bind(name, value);
                return *this;
            }

            void binding::bind_params(MYSQL_STMT *stmt) const
            {
                if (value_ == nullptr || stmt == nullptr) {
                    return;
                }

                if (mysql_stmt_bind_param(stmt, value_)) {
                    throw binding_error(helper::last_stmt_error(stmt));
                }
            }

            size_t binding::size() const
            {
                return size_;
            }

            void binding::reset()
            {
                bind_mapping::reset();

                clear_value();
            }

            std::string binding::prepare(const std::string &sql)
            {
                // now build the list of indexes
                auto match_begin = std::sregex_iterator(sql.begin(), sql.end(), bindable::index_regex);
                auto match_end = std::sregex_iterator();

                indexes_.clear();

                unsigned index = 0;
                char param_type = '\0';
                for (auto i = match_begin; i != match_end; ++i) {
                    auto str = i->str();
                    if (param_type == '\0') {
                        param_type = str[0];
                    } else if (param_type != str[0]) {
                        throw binding_error("mixed $ and ? parameters are not allowed.");
                    }
                    switch (param_type) {
                        // if its a ? parameter...
                        case '?': {
                            // then go to the next index
                            ++index;
                            break;
                        }
                        case '$': {
                            auto sub = *i;
                            // if its a $ parameter, mapp the real index
                            auto pos = std::stol(sub[1].str());
                            indexes_[pos].insert(++index);
                            break;
                        }
                    }
                }

                // setup the named parameters
                bind_mapping::prepare(sql, index);

                // replace all parameters to the mysql ? parameters
                return regex_replace(sql, bindable::param_regex, std::string("?"));
            }
        }
    }
}

#endif
