#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#undef PACKAGE_NAME
#undef PACKAGE_VERSION
#include <postgres.h>
#include <libpq-fe.h>
#include <catalog/pg_type.h>
#include <memory>
#include <cstdlib>
#include <time.h>
#include <cassert>
#include <locale>
#include <codecvt>
#include <regex>

#include "../exception.h"
#include "../log.h"
#include "../alloc.h"

#include "binding.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        namespace postgres
        {
            namespace data_mapper
            {
                // Key function here. Handles conversion from Oid to sql_value
                // TODO: test more
                sql_value to_value(Oid type, const char *value, int len)
                {
                    if (value == nullptr) {
                        return sql_null;
                    }

                    switch (type) {
                        case BYTEAOID: {
                            size_t blen = 0;
                            unsigned char *b = PQunescapeBytea(reinterpret_cast<const unsigned char *>(value), &blen);
                            return binary(b, blen);
                        }
                        case BOOLOID:
                        case CHAROID:
                        case INT8OID:
                        case INT2OID:
                        case INT4OID:
                        case TIMESTAMPOID:
                        case BITOID:
                            try {
                                return stol(value);
                            } catch (const std::exception &e) {
                                return sql_null;
                            }
                        case FLOAT4OID:
                            try {
                                return stod(value);
                            } catch (const std::exception &e) {
                                return sql_null;
                            }
                        case FLOAT8OID:
                            try {
                                return stold(value);
                            } catch (const std::exception &e) {
                                return sql_null;
                            }
                        case UNKNOWNOID:
                            return nullptr;
                        case VARCHAROID:
                        case TEXTOID:
                        case DATEOID:
                        case TIMEOID:
                        case UUIDOID:
                            return value;
                        default:
                            return value;
                    }
                }
            }

            binding::binding() : values_(nullptr), types_(nullptr), lengths_(nullptr), formats_(nullptr), size_(0)
            {
            }

            binding::binding(size_t size) : values_(nullptr), types_(nullptr), lengths_(nullptr), formats_(nullptr), size_(size)
            {
                values_ = c_alloc<char *>(size);
                types_ = c_alloc<Oid>(size);
                lengths_ = c_alloc<int>(size);
                formats_ = c_alloc<int>(size);
            }

            void binding::clear_value(size_t i)
            {
                if (i >= size_) {
                    throw binding_error("invalid index in postgres binding clear");
                }

                if (values_[i]) {
                    free(values_[i]);
                    values_[i] = nullptr;
                }
                types_[i] = 0;
                lengths_[i] = 0;
                formats_[i] = 0;
            }

            void binding::clear_value()
            {
                if (values_) {
                    for (size_t i = 0; i < size_; i++) {
                        clear_value(i);
                    }
                    free(values_);
                    values_ = nullptr;
                }
                if (types_) {
                    free(types_);
                    types_ = nullptr;
                }
                if (lengths_) {
                    free(lengths_);
                    lengths_ = nullptr;
                }
                if (formats_) {
                    free(formats_);
                    formats_ = nullptr;
                }
                size_ = 0;
            }

            void binding::copy_value(const binding &value)
            {
                clear_value();

                size_ = value.size_;
                values_ = c_alloc<char *>(size_);
                types_ = c_alloc<Oid>(size_);
                lengths_ = c_alloc<int>(size_);
                formats_ = c_alloc<int>(size_);

                for (size_t i = 0; i < size_; i++) {
                    types_[i] = value.types_[i];
                    lengths_[i] = value.lengths_[i];
                    formats_[i] = value.formats_[i];
                    if (value.values_[i]) {
                        values_[i] = strdup(value.values_[i]);
                    }
                }
            }

            binding::binding(const binding &other)
                : bind_mapping(other), values_(nullptr), types_(nullptr), lengths_(nullptr), formats_(nullptr), size_(0)
            {
                copy_value(other);
            }
            binding::binding(binding &&other)
                : bind_mapping(std::move(other)),
                  values_(other.values_),
                  types_(other.types_),
                  lengths_(other.lengths_),
                  formats_(other.formats_),
                  size_(other.size_)
            {
                other.values_ = nullptr;
                other.types_ = nullptr;
                other.lengths_ = nullptr;
                other.formats_ = nullptr;
                other.size_ = 0;
            }

            binding &binding::operator=(const binding &other)
            {
                bind_mapping::operator=(other);
                copy_value(other);
                return *this;
            }

            binding &binding::operator=(binding &&other)
            {
                bind_mapping::operator=(std::move(other));
                values_ = other.values_;
                types_ = other.types_;
                lengths_ = other.lengths_;
                formats_ = other.formats_;
                size_ = other.size_;
                other.values_ = nullptr;
                other.types_ = nullptr;
                other.lengths_ = nullptr;
                other.formats_ = nullptr;
                other.size_ = 0;
                return *this;
            }
            binding::~binding()
            {
                clear_value();
            }

            sql_value binding::to_value(size_t index) const
            {
                if (index >= size_ || values_ == nullptr || values_[index] == nullptr) {
                    return sql_null;
                }
                return data_mapper::to_value(types_[index], values_[index], lengths_[index]);
            }

            int binding::sql_type(size_t index) const
            {
                if (index >= size_) {
                    return 0;
                }

                return types_[index];
            }

            bool binding::reallocate_value(size_t index)
            {
                // assert non-zero-indexed
                if (index == 0) {
                    return false;
                }

                if (index <= size_) {
                    clear_value(index - 1);
                    return true;
                }

                // dynamic array of parameter values

                values_ = c_alloc<char *>(values_, index, size_);
                types_ = c_alloc<Oid>(types_, index, size_);
                lengths_ = c_alloc<int>(lengths_, index, size_);
                formats_ = c_alloc<int>(formats_, index, size_);

                size_ = index;

                return true;
            }

            /**
             * binding methods ensure the dynamic array is sized properly and store the value as a memory pointer
             */

            binding &binding::bind(size_t index, int value)
            {
                if (reallocate_value(index)) {
                    values_[index - 1] = strdup(std::to_string(value).c_str());
                    types_[index - 1] = sizeof(int) == 2 ? INT2OID : sizeof(int) == 4 ? INT4OID : INT8OID;
                    lengths_[index - 1] = sizeof(int);
                    formats_[index - 1] = 0;
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, unsigned value)
            {
                if (reallocate_value(index)) {
                    values_[index - 1] = strdup(std::to_string(value).c_str());
                    types_[index - 1] = sizeof(unsigned) <= 2 ? INT2OID : sizeof(unsigned) <= 4 ? INT4OID : INT8OID;
                    lengths_[index - 1] = sizeof(unsigned);
                    formats_[index - 1] = 0;
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, long long value)
            {
                if (reallocate_value(index)) {
                    values_[index - 1] = strdup(std::to_string(value).c_str());
                    types_[index - 1] = sizeof(long long) == 2 ? INT2OID : sizeof(long long) == 4 ? INT4OID : INT8OID;
                    lengths_[index - 1] = sizeof(long long);
                    formats_[index - 1] = 0;
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, unsigned long long value)
            {
                if (reallocate_value(index)) {
                    values_[index - 1] = strdup(std::to_string(value).c_str());
                    types_[index - 1] = sizeof(unsigned long long) <= 2 ? INT2OID : sizeof(unsigned long long) <= 4 ? INT4OID : INT8OID;
                    lengths_[index - 1] = sizeof(unsigned long long);
                    formats_[index - 1] = 0;
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, float value)
            {
                if (reallocate_value(index)) {
                    values_[index - 1] = strdup(std::to_string(value).c_str());
                    types_[index - 1] = FLOAT4OID;
                    lengths_[index - 1] = sizeof(float);
                    formats_[index - 1] = 0;
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, double value)
            {
                if (reallocate_value(index)) {
                    values_[index - 1] = strdup(std::to_string(value).c_str());
                    types_[index - 1] = sizeof(double) <= 4 ? FLOAT4OID : FLOAT8OID;
                    lengths_[index - 1] = sizeof(double);
                    formats_[index - 1] = 0;
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, const std::string &value, int len)
            {
                if (reallocate_value(index)) {
                    values_[index - 1] = strdup(value.c_str());
                    types_[index - 1] = TEXTOID;
                    lengths_[index - 1] = value.size();
                    formats_[index - 1] = 0;
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, const std::wstring &value, int len)
            {
                if (reallocate_value(index)) {
                    typedef std::codecvt_utf8<wchar_t> convert_type;
                    std::wstring_convert<convert_type, wchar_t> converter;
                    std::string converted_str = converter.to_bytes(value);
                    values_[index - 1] = strdup(converted_str.c_str());
                    types_[index - 1] = TEXTOID;
                    lengths_[index - 1] = value.size();
                    formats_[index - 1] = 0;
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, const sql_blob &value)
            {
                if (reallocate_value(index)) {
                    values_[index - 1] = strdup(value.to_string().c_str());
                    types_[index - 1] = BYTEAOID;
                    lengths_[index - 1] = value.size();
                    formats_[index - 1] = 1;
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }
            binding &binding::bind(size_t index, const sql_null_type &value)
            {
                if (reallocate_value(index)) {
                    if (values_[index - 1]) {
                        free(values_[index - 1]);
                    }
                    values_[index - 1] = nullptr;
                    lengths_[index - 1] = 0;
                    types_[index - 1] = UNKNOWNOID;
                    formats_[index - 1] = 0;
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }
                return *this;
            }
            binding &binding::bind(size_t index, const sql_time &value)
            {
                if (reallocate_value(index)) {
                    values_[index - 1] = strdup(value.to_string().c_str());
                    switch (value.format()) {
                        case sql_time::DATE:
                            types_[index - 1] = DATEOID;
                            break;
                        case sql_time::TIME:
                            types_[index - 1] = TIMEOID;
                            break;
                        case sql_time::DATETIME:
                        case sql_time::TIMESTAMP:
                            types_[index - 1] = TIMESTAMPOID;
                            break;
                    }
                    lengths_[index - 1] = value.size();
                    formats_[index - 1] = 0;
                } else {
                    log::warn("unable to reallocate bindings for index %ld", index);
                }

                return *this;
            }

            binding &binding::bind(const string &name, const sql_value &value)
            {
                bind_mapping::bind(name, value);
                return *this;
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

            std::string binding::prepare(const string &sql)
            {
                auto match_begin = std::sregex_iterator(sql.begin(), sql.end(), bindable::index_regex);
                auto match_end = std::sregex_iterator();

                std::vector<size_t> indexes;

                // determine max index, to add named params at end
                // postgres can reuse indexes so this is necessary over just counting
                size_t max_index = 0;
                for (auto match = match_begin; match != match_end; ++match) {
                    auto str = match->str();
                    if (str[0] == '$') {
                        auto sub = *match;
                        auto pos = std::stol(sub[2].str());
                        // don't increment max index if not needed
                        if (pos < max_index) {
                            continue;
                        }
                    } else {
                        indexes.push_back(indexes.size() + 1);
                    }
                    ++max_index;
                }

                // map the named parameters
                bind_mapping::prepare(sql, max_index);

                // do replacing while searching.
                // TODO: this is ineffecient, find a better way
                std::string formatted = sql;

                match_begin = std::sregex_iterator(sql.begin(), sql.end(), bindable::param_regex);
                match_end = std::sregex_iterator();

                unsigned index = 0;
                for (auto match = match_begin; match != match_end; ++match) {
                    auto str = match->str();
                    // if it is a named parameter...
                    if (str[0] == '@' || str[0] == ':') {
                        // get the determined positions for the named parameter
                        auto mapped = get_named_param_indexes(str);
                        if (!mapped.empty()) {
                            // postgres only needs the first determined index as it can re-use parameter indexes
                            auto pos = *mapped.begin();
                            // replace the sql with the positional parameter
                            formatted.replace(formatted.find(str), str.length(), "$" + std::to_string(pos));
                        }
                        continue;
                    }

                    // if its not a postgres parameter, replace it with one
                    if (str[0] == '?') {
                        // otherwise, replace the matched paramter with the current index
                        auto pos = indexes.back();
                        formatted.replace(formatted.find(str), str.length(), "$" + std::to_string(pos));
                        indexes.pop_back();
                    }
                }

                return formatted;
            }
        }
    }
}

#endif
