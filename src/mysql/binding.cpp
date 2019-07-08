
#include <time.h>
#include <cassert>
#include <codecvt>
#include <cstdlib>
#include <locale>
#include <memory>
#include <regex>

#include "../alloc.h"
#include "../exception.h"
#include "../sql_value.h"
#include "binding.h"

namespace coda
{
    namespace db
    {
        namespace mysql
        {
            namespace helper
            {
                // small util method to make a c pointer for a type
                template <typename T, typename = std::enable_if<std::is_arithmetic<T>::value>>
                void *to_cptr(const T &value)
                {
                    T *ptr = c_alloc<T>();
                    *ptr = value;
                    return ptr;
                }

                /**
                * function to assign a mysql field to a mysql binding value
                * @param value the binding value
                * @param field the field
                */
                void prepare_binding_from_field(MYSQL_BIND *value, MYSQL_FIELD *field)
                {
                    // sanity check
                    if (value == nullptr || field == nullptr) {
                        return;
                    }
                    value->buffer_type = field->type;
                    value->is_null = (field->flags & NOT_NULL_FLAG) ? 0 : c_alloc<bool>();
                    value->is_unsigned = field->flags & UNSIGNED_FLAG;
                    value->error = 0;
                    value->length = 0;

                    // so now we have to prepare the buffer size for storing a value
                    switch (field->type) {
                        default:
                            value->buffer_length = field->length;
                            break;
                        case MYSQL_TYPE_FLOAT:
                            value->buffer_length = std::max(field->length, sizeof(float));
                            break;
                        case MYSQL_TYPE_DOUBLE:
                            value->buffer_length = std::max(field->length, sizeof(double));
                            break;
                        case MYSQL_TYPE_TINY:
                        case MYSQL_TYPE_SHORT:
                        case MYSQL_TYPE_LONG:
                            value->buffer_length = std::max(field->length, sizeof(long));
                            break;
                        case MYSQL_TYPE_LONGLONG:
                            value->buffer_length = std::max(field->length, sizeof(long long));
                            break;
                        case MYSQL_TYPE_STRING:
                        case MYSQL_TYPE_VAR_STRING:
                        case MYSQL_TYPE_VARCHAR:
                        case MYSQL_TYPE_BLOB:
                        case MYSQL_TYPE_TINY_BLOB:
                        case MYSQL_TYPE_MEDIUM_BLOB:
                        case MYSQL_TYPE_LONG_BLOB:
                            value->length = c_alloc<unsigned long>();
                            value->buffer_length = field->length;
                            break;
                        case MYSQL_TYPE_DATETIME:
                        case MYSQL_TYPE_DATE:
                        case MYSQL_TYPE_TIME:
                        case MYSQL_TYPE_TIMESTAMP:
                            value->buffer_length = sizeof(MYSQL_TIME);
                            break;
                    }
                    if (value->buffer_length > 0) {
                        value->buffer = c_alloc(value->buffer_length);
                    } else {
                        value->buffer = nullptr;
                    }
                }

                /**
                 * function to copy a mysql bind structure
                 * NOTE: will copy pointers by value
                 * @param value the value to copy to
                 * @param other the value to copy from
                 */
                void bind_value_copy(MYSQL_BIND *value, const MYSQL_BIND *other)
                {
                    // sanity check
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
                        value->is_null = c_alloc<bool>();
                        memmove(value->is_null, other->is_null, sizeof(bool));
                    }

                    if (other->error) {
                        value->error = c_alloc<bool>();
                        memmove(value->error, other->error, sizeof(bool));
                    }

                    value->buffer_type = other->buffer_type;
                    value->buffer_length = other->buffer_length;
                    value->is_unsigned = other->is_unsigned;
                }

                /**
                 * parses a sql time from a binding
                 * @param  binding the binding value
                 * @param  format  the format of the time value
                 * @return         the parsed sql_time value or sql_null
                 */
                sql_value parse_time(MYSQL_BIND *binding, sql_time::formats format)
                {
                    struct tm sys;
                    MYSQL_TIME *db_tm;

                    // sanity check
                    if (binding == nullptr || (binding->is_null && *binding->is_null)) {
                        return sql_null;
                    }

                    db_tm = (MYSQL_TIME *)binding->buffer;

                    if (db_tm == nullptr) {
                        return sql_null;
                    }

                    memset(&sys, 0, sizeof(sys));

                    sys.tm_year = db_tm->year - 1900;
                    sys.tm_mon = db_tm->month - 1;
                    sys.tm_mday = db_tm->day;
                    sys.tm_hour = db_tm->hour;
                    sys.tm_min = db_tm->minute;
                    sys.tm_sec = db_tm->second;

                    return sql_time(timegm(&sys), format);
                }

                extern std::string last_stmt_error(MYSQL_STMT *stmt);
            }
            // namespace for converting data
            namespace data_mapper
            {
                /**
                 * handle unsigned/signed flag in binding when converting a number
                 */
                template <typename T>
                typename std::enable_if<std::is_integral<T>::value, sql_value>::type to_number(MYSQL_BIND *binding)
                {
                    if (binding->is_unsigned) {
                        typedef typename std::make_unsigned<T>::type U;
                        U *p = static_cast<U *>(binding->buffer);
                        if (p == nullptr) {
                            return sql_number(sql_null);
                        }
                        return sql_number(*p);
                    } else {
                        T *p = static_cast<T *>(binding->buffer);
                        if (p == nullptr) {
                            return sql_number(sql_null);
                        }
                        return sql_number(*p);
                    }
                }
                /**
                 * Key method here, handles conversion from MYSQL_BIND to sql_value
                 * TODO: test this more
                 * @param binding the binding value
                 * @return the sql_value
                 */
                sql_value to_value(MYSQL_BIND *binding)
                {
                    // sanity check
                    if (binding == nullptr || (binding->is_null && *binding->is_null)) {
                        return sql_null;
                    }

                    switch (binding->buffer_type) {
                        case MYSQL_TYPE_BIT:
                        case MYSQL_TYPE_TINY:
                        case MYSQL_TYPE_SHORT:
                        case MYSQL_TYPE_INT24:
                        case MYSQL_TYPE_LONG:
                            switch (binding->buffer_length) {
                                case sizeof(char):
                                    return to_number<char>(binding);
                                case sizeof(short):
                                    return to_number<short>(binding);
                                case sizeof(int):
                                default:
                                    return to_number<int>(binding);
                                case sizeof(long):
                                    return to_number<long>(binding);
                            }
                        case MYSQL_TYPE_LONGLONG:
                            return to_number<long long>(binding);
                        case MYSQL_TYPE_NULL:
                            return sql_null;
                        case MYSQL_TYPE_TIME:
                            return helper::parse_time(binding, sql_time::TIME);
                        case MYSQL_TYPE_DATE:
                            return helper::parse_time(binding, sql_time::DATE);
                        case MYSQL_TYPE_TIMESTAMP:
                            return helper::parse_time(binding, sql_time::TIMESTAMP);
                        case MYSQL_TYPE_DATETIME:
                            return helper::parse_time(binding, sql_time::DATETIME);
                        case MYSQL_TYPE_VAR_STRING:
                        case MYSQL_TYPE_VARCHAR:
                        case MYSQL_TYPE_DECIMAL:
                        case MYSQL_TYPE_SET:
                        case MYSQL_TYPE_ENUM:
                        case MYSQL_TYPE_GEOMETRY:
                        case MYSQL_TYPE_NEWDECIMAL:
                        case MYSQL_TYPE_STRING:
                        default: {
                            return sql_string(static_cast<const char *>(binding->buffer));
                        }
                        case MYSQL_TYPE_FLOAT: {
                            float *p = static_cast<float *>(binding->buffer);
                            return sql_number(*p);
                        }
                        case MYSQL_TYPE_DOUBLE: {
                            double *p = static_cast<double *>(binding->buffer);
                            return sql_number(*p);
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

                /**
                 * Key method here, handles conversion from a field to sql_value
                 * TODO: test this more with different types
                 * Doesn't throw, prefers to return a null value
                 * @param type the type of field
                 * @param value the value
                 * @param length the length of the value
                 * @return the sql_value
                 */
                sql_value to_value(int type, const char *value, size_t length)
                {
                    if (value == nullptr) {
                        return sql_null;
                    }

                    switch (type) {
                        case MYSQL_TYPE_BIT:
                        case MYSQL_TYPE_TINY:
                        case MYSQL_TYPE_SHORT:
                        case MYSQL_TYPE_INT24:
                        case MYSQL_TYPE_LONG: {
                            try {
                                return sql_number(std::stoi(std::string(value, 0, length)));
                            } catch (const std::exception &e) {
                                throw value_conversion_error("unable to get integer from value");
                            }
                        }
                        case MYSQL_TYPE_LONGLONG: {
                            try {
                                return sql_number(std::stoll(std::string(value, 0, length)));
                            } catch (const std::exception &e) {
                                throw value_conversion_error("unable to get longlong from value");
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
                            return sql_string(value, 0, length);
                        case MYSQL_TYPE_NEWDATE:
                        case MYSQL_TYPE_DATE:
                        case MYSQL_TYPE_DATETIME:
                        case MYSQL_TYPE_TIMESTAMP:
                        case MYSQL_TYPE_TIME: {
                            try {
                                return sql_time(std::string(value, 0, length));
                            } catch (const value_conversion_error &e) {
                                throw value_conversion_error("unable to get time format from value");
                            }
                        }
                        case MYSQL_TYPE_FLOAT: {
                            try {
                                return sql_number(std::stof(std::string(value, 0, length)));
                            } catch (const std::exception &e) {
                                throw value_conversion_error("unable to get float from value");
                            }
                        }
                        case MYSQL_TYPE_DOUBLE: {
                            try {
                                return sql_number(std::stod(std::string(value, 0, length)));
                            } catch (const std::exception &e) {
                                throw value_conversion_error("unable to get double from value");
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

                /**
                 * set a time value to a binding
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
                    tm->month = gmt->tm_mon + 1;
                    tm->day = gmt->tm_mday;
                    tm->hour = gmt->tm_hour;
                    tm->minute = gmt->tm_min;
                    tm->second = gmt->tm_sec;
                    binding->buffer = tm;
                    binding->buffer_length = sizeof(MYSQL_TIME);
                }

                /**
                 * a value visitor to apply to a mysql binding
                 */
                class from_number : public boost::static_visitor<void>
                {
                   public:
                    from_number(MYSQL_BIND *bind) : bind_(bind)
                    {
                    }
                    void operator()(const sql_null_type &null) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_NULL;
                    }
                    void operator()(char value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_TINY;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                    }
                    void operator()(unsigned char value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_TINY;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                        bind_->is_unsigned = 1;
                    }
                    void operator()(wchar_t value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_TINY;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                    }
                    void operator()(long double value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_DOUBLE;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                    }
                    void operator()(double value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_DOUBLE;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                    }
                    void operator()(float value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_FLOAT;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                    }
                    void operator()(unsigned long long value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_LONGLONG;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                        bind_->is_unsigned = 1;
                    }
                    void operator()(long long value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_LONGLONG;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                    }
                    void operator()(unsigned int value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_LONG;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                        bind_->is_unsigned = 1;
                    }
                    void operator()(unsigned long value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_LONG;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                        bind_->is_unsigned = 1;
                    }
                    void operator()(int value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_LONG;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                    }
                    void operator()(long value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_LONG;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                    }
                    void operator()(short value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_SHORT;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                    }
                    void operator()(unsigned short value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_SHORT;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = sizeof(value);
                        bind_->is_unsigned = 1;
                    }
                    void operator()(bool value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_TINY;
                        bind_->buffer = helper::to_cptr(value);
                        bind_->buffer_length = 1;
                    }

                   private:
                    MYSQL_BIND *bind_;
                };

                /**
                 * a visitor to apply a value to a mysql binding
                 */
                class from_value : public boost::static_visitor<void>
                {
                   public:
                    from_value(MYSQL_BIND *bind) : bind_(bind)
                    {
                    }
                    void operator()(const sql_time &value) const
                    {
                        data_mapper::set_time(bind_, value);
                    }
                    void operator()(const sql_null_type &value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_NULL;
                    }
                    void operator()(const sql_blob &value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_BLOB;
                        bind_->buffer = c_copy(value.get(), value.size());
                        bind_->buffer_length = value.size();
                        if (!bind_->length) {
                            bind_->length = c_alloc<unsigned long>();
                        }
                        *bind_->length = value.size();
                    }
                    void operator()(const sql_number &value) const
                    {
                        value.apply_visitor(data_mapper::from_number(bind_));
                    }
                    void operator()(const sql_string &value) const
                    {
                        bind_->buffer_type = MYSQL_TYPE_STRING;
                        bind_->buffer = strdup(value.c_str());
                        bind_->buffer_length = value.size();
                        if (!bind_->length) {
                            bind_->length = c_alloc<unsigned long>();
                        }
                        *bind_->length = value.size();
                    }
                    void operator()(const sql_wstring &value) const
                    {
                        typedef std::codecvt_utf8<wchar_t> convert_type;
                        std::wstring_convert<convert_type, wchar_t> converter;
                        std::string converted_str = converter.to_bytes(value);
                        bind_->buffer_type = MYSQL_TYPE_STRING;
                        bind_->buffer = strdup(converted_str.c_str());
                        bind_->buffer_length = value.size();
                        if (!bind_->length) {
                            bind_->length = c_alloc<unsigned long>();
                        }
                        *bind_->length = value.size();
                    }

                   private:
                    MYSQL_BIND *bind_;
                };
            }

            binding::binding() : binding(prealloc_size)
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
                    helper::prepare_binding_from_field(&value_[i], &fields[i]);
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
                clear_value();
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
                if (stmt == nullptr || value_ == nullptr || size_ == 0) {
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

                index += prealloc_increment;

                if (index < size_) {
                    throw std::bad_alloc();
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
             * Mysql does not support using an index more than once in a query
             * So this method was added to implement it
             * @param index the parameter index
             * @return the set of "real" indexes for binding
             */
            std::set<size_t> &binding::get_indexes(size_t index)
            {
                auto &indexes = indexes_[index];

                if (indexes.size() == 0) {
                    indexes.insert(index);
                }
                return indexes;
            }

            binding &binding::bind(size_t index, const sql_value &value)
            {
                for (size_t i : get_indexes(index)) {
                    if (reallocate_value(i)) {
                        value.apply_visitor(data_mapper::from_value(&value_[i - 1]));
                    } else {
                        //unable to reallocate bindings for index
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
                if (value_ == nullptr || stmt == nullptr || size_ == 0) {
                    return;
                }

                if (mysql_stmt_bind_param(stmt, value_)) {
                    throw binding_error(helper::last_stmt_error(stmt));
                }
            }

            size_t binding::num_of_bindings() const noexcept
            {
                size_t count = 0;
                for (size_t i = 0; i < size_; i++) {
                    if (value_[i].buffer != nullptr) {
                        count++;
                    }
                }
                return count;
            }

            size_t binding::capacity() const
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
#ifdef ENABLE_PARAMETER_MAPPING
                // now build the list of indexes
                auto match_begin = std::sregex_iterator(sql.begin(), sql.end(), bindable::index_regex);
                auto match_end = std::sregex_iterator();

                indexes_.clear();

                unsigned index = 0;
                for (auto i = match_begin; i != match_end; ++i) {
                    auto str = i->str();
                    switch (str[0]) {
                        // if its a ? parameter...
                        case '?': {
                            // then go to the next index
                            ++index;
                            break;
                        }
                        case '$': {
                            auto sub = *i;
                            // if its a $ parameter, mapp the real index
                            auto pos = std::stol(sub[2].str());
                            indexes_[pos].insert(++index);
                            break;
                        }
                    }
                }

                // setup the named parameters
                bind_mapping::prepare(sql, index);

                // replace all parameters to the mysql ? parameters
                return regex_replace(sql, bindable::param_regex, std::string("?"));
#else
                return sql;
#endif
            }
        }
    }
}
