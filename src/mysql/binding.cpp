#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include "binding.h"
#include "../exception.h"
#include "../log.h"
#include "../alloc.h"
#include <memory>
#include <cstdlib>
#include <time.h>
#include <cassert>

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
                    value->buffer_type = field->type;
                    value->is_null = c_alloc<my_bool>();
                    value->is_unsigned = 0;
                    value->error = 0;
                    value->buffer_length = field->length;
                    value->length = c_alloc<unsigned long>();
                    *value->length = 0;
                    value->buffer = c_alloc(field->length);
                }

                void bind_value_copy(MYSQL_BIND *value, const MYSQL_BIND *other)
                {
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
                    if (binding == nullptr) {
                        return 0;
                    }

                    MYSQL_TIME *db_tm = (MYSQL_TIME *)binding->buffer;
                    struct tm sys;

                    sys.tm_year = db_tm->year;
                    sys.tm_mon = db_tm->month;
                    sys.tm_mday = db_tm->day;
                    sys.tm_hour = db_tm->hour;
                    sys.tm_min = db_tm->minute;
                    sys.tm_sec = db_tm->second;

                    return mktime(&sys);
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
                        case MYSQL_TYPE_DATETIME:
                        case MYSQL_TYPE_TIMESTAMP:
                        case MYSQL_TYPE_YEAR:
                        case MYSQL_TYPE_TIME: {
                            struct tm *tp;

                            if ((tp = getdate(value))) {
                                return mktime(tp);
                            } else {
                                log::error("unable to get date of %s (%d)", value, getdate_err);
                                return sql_value();
                            }
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

            binding::binding(const binding &other) : value_(nullptr)
            {
                copy_value(other.value_, other.size_);
            }
            binding::binding(binding &&other)
            {
                value_ = other.value_;
                size_ = other.size_;
                other.value_ = nullptr;
                other.size_ = 0;
            }

            binding &binding::operator=(const binding &other)
            {
                copy_value(other.value_, other.size_);
                return *this;
            }

            binding &binding::operator=(binding &&other)
            {
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
                if (index >= size_) {
                    throw binding_error("invalid index in mysql binding get");
                }

                return &value_[index];
            }

            void binding::bind_result(MYSQL_STMT *stmt) const
            {
                if (mysql_stmt_bind_result(stmt, value_) != 0) {
                    throw binding_error(helper::last_stmt_error(stmt));
                }
            }

            sql_value binding::to_value(size_t index) const
            {
                if (index >= size_ || value_[index].buffer == nullptr) {
                    return sql_null;
                }

                return data_mapper::to_value(&value_[index]);
            }

            int binding::sql_type(size_t index) const
            {
                if (index >= size_) {
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

            /**
             * binding methods ensure the dynamic array is sized properly and store the value as a memory pointer
             */

            binding &binding::bind(size_t index, int value)
            {
                if (reallocate_value(index)) {
                    value_[index - 1].buffer_type = MYSQL_TYPE_LONG;
                    value_[index - 1].buffer = helper::to_ptr(value);
                    value_[index - 1].buffer_length = sizeof(value);
                } else {
                    log::error("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, unsigned value)
            {
                if (reallocate_value(index)) {
                    value_[index - 1].buffer_type = MYSQL_TYPE_LONG;
                    value_[index - 1].buffer = helper::to_ptr(value);
                    value_[index - 1].buffer_length = sizeof(value);
                    value_[index - 1].is_unsigned = 1;
                } else {
                    log::error("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, long long value)
            {
                if (reallocate_value(index)) {
                    value_[index - 1].buffer_type = MYSQL_TYPE_LONGLONG;
                    value_[index - 1].buffer = helper::to_ptr(value);
                    value_[index - 1].buffer_length = sizeof(value);
                } else {
                    log::error("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, unsigned long long value)
            {
                if (reallocate_value(index)) {
                    value_[index - 1].buffer_type = MYSQL_TYPE_LONGLONG;
                    value_[index - 1].buffer = helper::to_ptr(value);
                    value_[index - 1].buffer_length = sizeof(value);
                    value_[index - 1].is_unsigned = 1;
                } else {
                    log::error("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, float value)
            {
                if (reallocate_value(index)) {
                    value_[index - 1].buffer_type = MYSQL_TYPE_FLOAT;
                    value_[index - 1].buffer = helper::to_ptr(value);
                    value_[index - 1].buffer_length = sizeof(value);
                } else {
                    log::error("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, double value)
            {
                if (reallocate_value(index)) {
                    value_[index - 1].buffer_type = MYSQL_TYPE_DOUBLE;
                    value_[index - 1].buffer = helper::to_ptr(value);
                    value_[index - 1].buffer_length = sizeof(value);
                } else {
                    log::error("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, const std::string &value, int len)
            {
                if (reallocate_value(index)) {
                    value_[index - 1].buffer_type = MYSQL_TYPE_STRING;
                    auto size = len == -1 ? value.size() : len;
                    value_[index - 1].buffer = strdup(value.c_str());
                    value_[index - 1].buffer_length = size;
                    if (!value_[index - 1].length) {
                        value_[index - 1].length = c_alloc<unsigned long>();
                    }
                    *value_[index - 1].length = size;
                } else {
                    log::error("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, const std::wstring &value, int len)
            {
                if (reallocate_value(index)) {
                    value_[index - 1].buffer_type = MYSQL_TYPE_STRING;
                    auto size = len == -1 ? value.size() : len;
                    value_[index - 1].buffer = wcsdup(value.c_str());
                    value_[index - 1].buffer_length = size;
                    if (!value_[index - 1].length) {
                        value_[index - 1].length = c_alloc<unsigned long>();
                    }
                    *value_[index - 1].length = size;
                } else {
                    log::error("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, const sql_blob &value)
            {
                if (reallocate_value(index)) {
                    value_[index - 1].buffer_type = MYSQL_TYPE_BLOB;
                    void *ptr = c_alloc(value.size());
                    memcpy(ptr, value.value(), value.size());
                    value_[index - 1].buffer = ptr;
                    value_[index - 1].buffer_length = value.size();
                    if (!value_[index - 1].length) {
                        value_[index - 1].length = c_alloc<unsigned long>();
                    }
                    *value_[index - 1].length = value.size();
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }

            binding &binding::bind(size_t index, const sql_null_type &value)
            {
                if (reallocate_value(index)) {
                    value_[index - 1].buffer_type = MYSQL_TYPE_NULL;
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }
                return *this;
            }

            binding &binding::bind(size_t index, const sql_time &value)
            {
                if (reallocate_value(index)) {
                    data_mapper::set_time(&value_[index - 1], value);
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }

            void binding::bind_params(MYSQL_STMT *stmt) const
            {
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
                clear_value();
            }
        }
    }
}

#endif