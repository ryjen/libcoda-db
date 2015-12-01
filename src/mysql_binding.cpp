#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include "mysql_binding.h"
#include "exception.h"
#include <memory>
#include <cstdlib>
#include <time.h>
#include <cassert>

namespace arg3
{
    namespace db
    {
        namespace inner
        {
            // small util method to make a c pointer for a type
            template <typename T>
            void *to_ptr(T value)
            {
                T *ptr = (T *)calloc(1, sizeof(T));

                *ptr = value;

                return ptr;
            }

            bool create_bind_value_from_field(MYSQL_BIND *value, MYSQL_FIELD *field)
            {
                value->buffer_type = field->type;
                value->is_null = static_cast<my_bool *>(calloc(1, sizeof(my_bool)));
                if (value->is_null == nullptr) {
                    return false;
                }
                value->is_unsigned = 0;
                value->error = 0;
                value->buffer_length = field->length;
                value->length = static_cast<unsigned long *>(calloc(1, sizeof(unsigned long)));
                if (value->length == nullptr) {
                    return false;
                }
                *value->length = field->length;
                value->buffer = calloc(1, field->length);
                if (value->buffer == nullptr) {
                    return false;
                }
                return true;
            }

            bool copy_bind_value(MYSQL_BIND *value, const MYSQL_BIND *other)
            {
                if (other->length) {
                    value->length = static_cast<unsigned long *>(calloc(1, sizeof(unsigned long)));

                    if (value->length == nullptr) {
                        return false;
                    }
                    memmove(value->length, other->length, sizeof(unsigned long));
                }

                if (other->buffer_length > 0 && other->buffer) {
                    value->buffer = calloc(1, other->buffer_length);
                    if (value->buffer == nullptr) {
                        return false;
                    }
                    memmove(value->buffer, other->buffer, other->buffer_length);
                }

                if (other->is_null) {
                    value->is_null = static_cast<my_bool *>(calloc(1, sizeof(my_bool)));
                    if (value->is_null == nullptr) {
                        return false;
                    }
                    memmove(value->is_null, other->is_null, sizeof(my_bool));
                }

                if (other->error) {
                    value->error = static_cast<my_bool *>(calloc(1, sizeof(my_bool)));
                    if (value->error == nullptr) {
                        return false;
                    }
                    memmove(value->error, other->error, sizeof(my_bool));
                }

                value->buffer_type = other->buffer_type;
                value->buffer_length = other->buffer_length;
                value->is_unsigned = other->is_unsigned;

                return true;
            }
        }

        extern string last_stmt_error(MYSQL_STMT *stmt);

        mysql_binding::mysql_binding() : value_(nullptr), size_(0)
        {
        }

        mysql_binding::mysql_binding(size_t size) : value_(nullptr), size_(size)
        {
            value_ = static_cast<MYSQL_BIND *>(calloc(size, sizeof(MYSQL_BIND)));

            if (value_ == nullptr) {
                throw std::bad_alloc();
            }
        }

        mysql_binding::mysql_binding(const MYSQL_BIND &value) : value_(nullptr), size_(1)
        {
            copy_value(&value, size_);
        }
        mysql_binding::mysql_binding(MYSQL_BIND *values, size_t size) : value_(nullptr), size_(size)
        {
            copy_value(values, size);
        }

        mysql_binding::mysql_binding(MYSQL_FIELD *fields, size_t size) : size_(size)
        {
            value_ = static_cast<MYSQL_BIND *>(calloc(size, sizeof(MYSQL_BIND)));

            if (value_ == nullptr) {
                throw std::bad_alloc();
            }

            for (size_t i = 0; i < size; i++) {
                if (!inner::create_bind_value_from_field(&value_[i], &fields[i])) {
                    throw std::bad_alloc();
                }
            }
        }

        void mysql_binding::clear_value(size_t i)
        {
            assert(i < size_);

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

        void mysql_binding::clear_value()
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

        void mysql_binding::copy_value(const MYSQL_BIND *others, size_t size)
        {
            clear_value();

            value_ = static_cast<MYSQL_BIND *>(calloc(size, sizeof(MYSQL_BIND)));

            if (value_ == nullptr) {
                throw std::bad_alloc();
            }

            for (size_t i = 0; i < size; i++) {
                const MYSQL_BIND *other = &others[i];
                MYSQL_BIND *value = &value_[i];

                if (!inner::copy_bind_value(value, other)) {
                    throw std::bad_alloc();
                }
            }

            size_ = size;
        }

        mysql_binding::mysql_binding(const mysql_binding &other) : value_(nullptr)
        {
            copy_value(other.value_, other.size_);
        }
        mysql_binding::mysql_binding(mysql_binding &&other)
        {
            value_ = other.value_;
            size_ = other.size_;
            other.value_ = nullptr;
            other.size_ = 0;
        }

        mysql_binding &mysql_binding::operator=(const mysql_binding &other)
        {
            copy_value(other.value_, other.size_);
            return *this;
        }

        mysql_binding &mysql_binding::operator=(mysql_binding &&other)
        {
            value_ = other.value_;
            size_ = other.size_;
            other.value_ = nullptr;
            other.size_ = 0;
            return *this;
        }
        mysql_binding::~mysql_binding()
        {
            clear_value();
        }

        MYSQL_BIND *mysql_binding::get(size_t index) const
        {
            if (index >= size_) {
                return nullptr;
            }

            return &value_[index];
        }

        void mysql_binding::bind_result(MYSQL_STMT *stmt) const
        {
            if (mysql_stmt_bind_result(stmt, value_) != 0) {
                throw binding_error(last_stmt_error(stmt));
            }
        }

        sql_value mysql_binding::to_value(size_t index) const
        {
            if (index >= size_ || value_[index].buffer == nullptr) return sql_null;

            switch (value_[index].buffer_type) {
                case MYSQL_TYPE_BIT:
                case MYSQL_TYPE_TINY:
                case MYSQL_TYPE_SHORT:
                case MYSQL_TYPE_LONG:
                case MYSQL_TYPE_INT24: {
                    int *p = static_cast<int *>(value_[index].buffer);
                    if (p == nullptr) return sql_null;
                    return *p;
                }
                case MYSQL_TYPE_LONGLONG: {
                    long long *p = static_cast<long long *>(value_[index].buffer);

                    if (p == nullptr) return sql_null;

                    return *p;
                }
                case MYSQL_TYPE_NULL:
                    return sql_null;
                case MYSQL_TYPE_TIME:
                case MYSQL_TYPE_DATE:
                case MYSQL_TYPE_TIME2:
                case MYSQL_TYPE_YEAR:
                case MYSQL_TYPE_NEWDATE:
                case MYSQL_TYPE_TIMESTAMP:
                case MYSQL_TYPE_DATETIME:
                case MYSQL_TYPE_TIMESTAMP2:
                case MYSQL_TYPE_DATETIME2: {
                    struct tm *tp;

                    if ((tp = getdate(static_cast<char *>(value_[index].buffer)))) {
                        return (long long)mktime(tp);
                    } else {
                        int *p = static_cast<int *>(value_[index].buffer);
                        if (p == nullptr) return sql_null;
                        return *p;
                    }
                }
                case MYSQL_TYPE_VAR_STRING:
                case MYSQL_TYPE_VARCHAR:
                case MYSQL_TYPE_DECIMAL:
                case MYSQL_TYPE_JSON:
                case MYSQL_TYPE_SET:
                case MYSQL_TYPE_ENUM:
                case MYSQL_TYPE_GEOMETRY:
                case MYSQL_TYPE_NEWDECIMAL:
                case MYSQL_TYPE_STRING: {
                    return static_cast<const char *>(value_[index].buffer);
                }
                case MYSQL_TYPE_FLOAT:
                case MYSQL_TYPE_DOUBLE: {
                    double *p = static_cast<double *>(value_[index].buffer);

                    return *p;
                }
                case MYSQL_TYPE_TINY_BLOB:
                case MYSQL_TYPE_MEDIUM_BLOB:
                case MYSQL_TYPE_LONG_BLOB:
                case MYSQL_TYPE_BLOB: {
                    if (value_[index].length) {
                        return sql_blob(value_[index].buffer, *value_[index].length);
                    }

                    return sql_blob();
                }
            }
        }

        int mysql_binding::sql_type(size_t index) const
        {
            if (index >= size_) {
                return MYSQL_TYPE_NULL;
            }

            return value_[index].buffer_type;
        }

        bool mysql_binding::reallocate_value(size_t index)
        {
            if (index == 0) {
                return false;
            }

            if (index <= size_) {
                clear_value(index - 1);
                return true;
            }

            // dynamic array of parameter values
            if (value_ == nullptr) {
                value_ = static_cast<MYSQL_BIND *>(calloc(index, sizeof(MYSQL_BIND)));

                if (value_ == nullptr) {
                    throw std::bad_alloc();
                }
            } else {
                value_ = static_cast<MYSQL_BIND *>(realloc(value_, sizeof(MYSQL_BIND) * (index)));

                if (value_ == nullptr) {
                    throw std::bad_alloc();
                }
                // make sure new values are initialized
                for (size_t i = size_; i < index; i++) {
                    memset(&value_[i], 0, sizeof(MYSQL_BIND));
                }
            }

            size_ = index;

            return true;
        }

        /**
         * binding methods ensure the dynamic array is sized properly and store the value as a memory pointer
         */

        mysql_binding &mysql_binding::bind(size_t index, int value)
        {
            if (reallocate_value(index)) {
                value_[index - 1].buffer_type = MYSQL_TYPE_LONG;
                value_[index - 1].buffer = inner::to_ptr(value);
                value_[index - 1].buffer_length = sizeof(value);
            }

            return *this;
        }
        mysql_binding &mysql_binding::bind(size_t index, long long value)
        {
            if (reallocate_value(index)) {
                value_[index - 1].buffer_type = MYSQL_TYPE_LONGLONG;
                value_[index - 1].buffer = inner::to_ptr(value);
                value_[index - 1].buffer_length = sizeof(value);
            }

            return *this;
        }
        mysql_binding &mysql_binding::bind(size_t index, double value)
        {
            if (reallocate_value(index)) {
                value_[index - 1].buffer_type = MYSQL_TYPE_DOUBLE;
                value_[index - 1].buffer = inner::to_ptr(value);
                value_[index - 1].buffer_length = sizeof(value);
            }

            return *this;
        }
        mysql_binding &mysql_binding::bind(size_t index, const std::string &value, int len)
        {
            if (reallocate_value(index)) {
                value_[index - 1].buffer_type = MYSQL_TYPE_STRING;
                auto size = len == -1 ? value.size() : len;
                value_[index - 1].buffer = strdup(value.c_str());
                value_[index - 1].buffer_length = size;
                value_[index - 1].length = static_cast<unsigned long *>(calloc(1, sizeof(unsigned long)));
                if (value_[index - 1].length == nullptr) {
                    throw std::bad_alloc();
                }
                *value_[index - 1].length = size;
            }

            return *this;
        }
        mysql_binding &mysql_binding::bind(size_t index, const std::wstring &value, int len)
        {
            if (reallocate_value(index)) {
                value_[index - 1].buffer_type = MYSQL_TYPE_STRING;
                auto size = len == -1 ? value.size() : len;
                value_[index - 1].buffer = wcsdup(value.c_str());
                value_[index - 1].buffer_length = size;
                value_[index - 1].length = static_cast<unsigned long *>(calloc(1, sizeof(unsigned long)));
                if (value_[index - 1].length == nullptr) {
                    throw std::bad_alloc();
                }
                *value_[index - 1].length = size;
            }

            return *this;
        }
        mysql_binding &mysql_binding::bind(size_t index, const sql_blob &value)
        {
            if (reallocate_value(index)) {
                value_[index - 1].buffer_type = MYSQL_TYPE_BLOB;
                void *ptr = calloc(1, value.size());
                memcpy(ptr, value.value(), value.size());
                value_[index - 1].buffer = ptr;
                value_[index - 1].buffer_length = value.size();
                value_[index - 1].length = static_cast<unsigned long *>(calloc(1, sizeof(unsigned long)));
                if (value_[index - 1].length == nullptr) {
                    throw std::bad_alloc();
                }
                *value_[index - 1].length = value.size();
            }

            return *this;
        }
        mysql_binding &mysql_binding::bind(size_t index, const sql_null_t &value)
        {
            if (reallocate_value(index)) {
                value_[index - 1].buffer_type = MYSQL_TYPE_NULL;
            }
            return *this;
        }

        void mysql_binding::bind_params(MYSQL_STMT *stmt) const
        {
            if (mysql_stmt_bind_param(stmt, value_)) throw binding_error(last_stmt_error(stmt));
        }

        size_t mysql_binding::size() const
        {
            return size_;
        }

        void mysql_binding::reset()
        {
            clear_value();
        }
    }
}

#endif
