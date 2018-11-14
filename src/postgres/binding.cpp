
#include <cassert>
#include <cstdlib>
#include <postgres.h>
#include <catalog/pg_type.h>
#include <libpq-fe.h>
#include <memory>
#include <regex>
#include <time.h>

#include "../alloc.h"
#include "../exception.h"
#include "../sql_value.h"
#include "binding.h"

using namespace std;

namespace coda {
  namespace db {
    namespace postgres {
      namespace data_mapper {
        // Key function here. Handles conversion from Oid to sql_value
        // TODO: test more
        sql_value to_value(Oid type, const char *value, int len) {
          if (value == nullptr) {
            return sql_null;
          }

          switch (type) {
          case BYTEAOID: {
            size_t blen = 0;
            unsigned char *b = PQunescapeBytea(
                reinterpret_cast<const unsigned char *>(value), &blen);
            sql_blob bin(b, blen);
            free(b);
            return bin;
          }
          case BOOLOID:
          case CHAROID:
          case INT8OID:
          case INT2OID:
          case INT4OID:
          case TIMESTAMPOID:
          case BITOID:
            try {
              return sql_number(stol(value));
            } catch (const std::exception &e) {
              return sql_null;
            }
          case FLOAT4OID:
            try {
              return sql_number(stod(value));
            } catch (const std::exception &e) {
              return sql_null;
            }
          case FLOAT8OID:
            try {
              return sql_number(stold(value));
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
            return sql_string(value);
          default:
            return sql_string(value);
          }
        }

        /**
         * a visitor to apply a number to a postgres binding
         */
        class from_number {
          public:
          from_number(binding &bind, size_t index)
              : bind_(bind), index_(index) {}
          void operator()(wchar_t value) const {
            wchar_t buf[10] = {0};
            swprintf(buf, 10, L"%c", value);
            string temp = helper::convert_string(buf);
            bind_.values_[index_] = strdup(temp.c_str());
            bind_.types_[index_] = INT2OID;
            bind_.lengths_[index_] = sizeof(wchar_t);
            bind_.formats_[index_] = 0;
          }
          void operator()(unsigned char value) const {
            char buf[10] = {0};
            sprintf(buf, "%uc", value);
            bind_.values_[index_] = strdup(buf);
            bind_.types_[index_] = CHAROID;
            bind_.lengths_[index_] = sizeof(unsigned char);
            bind_.formats_[index_] = 0;
          }
          void operator()(char value) const {
            char buf[10] = {0};
            sprintf(buf, "%c", value);
            bind_.values_[index_] = strdup(buf);
            bind_.types_[index_] = CHAROID;
            bind_.lengths_[index_] = sizeof(char);
            bind_.formats_[index_] = 0;
          }
          void operator()(bool value) const {
            bind_.values_[index_] = strdup(std::to_string(value).c_str());
            bind_.types_[index_] = BOOLOID;
            bind_.lengths_[index_] = sizeof(double);
            bind_.formats_[index_] = 0;
          }
          void operator()(double value) const {
            bind_.values_[index_] = strdup(std::to_string(value).c_str());
            bind_.types_[index_] = FLOAT4OID;
            bind_.lengths_[index_] = sizeof(double);
            bind_.formats_[index_] = 0;
          }
          void operator()(float value) const {
            bind_.values_[index_] = strdup(std::to_string(value).c_str());
            bind_.types_[index_] = FLOAT4OID;
            bind_.lengths_[index_] = sizeof(float);
            bind_.formats_[index_] = 0;
          }
          void operator()(long double value) const {
            bind_.values_[index_] = strdup(std::to_string(value).c_str());
            bind_.types_[index_] = FLOAT8OID;
            bind_.lengths_[index_] = sizeof(long double);
            bind_.formats_[index_] = 0;
          }
          void operator()(unsigned long long value) const {
            bind_.values_[index_] = strdup(std::to_string(value).c_str());
            bind_.types_[index_] = INT8OID;
            bind_.lengths_[index_] = sizeof(unsigned long long);
            bind_.formats_[index_] = 0;
          }

          void operator()(long long value) const {
            bind_.values_[index_] = strdup(std::to_string(value).c_str());
            bind_.types_[index_] = INT8OID;
            bind_.lengths_[index_] = sizeof(long long);
            bind_.formats_[index_] = 0;
          }

          void operator()(unsigned int value) const {
            bind_.values_[index_] = strdup(std::to_string(value).c_str());
            bind_.types_[index_] = INT4OID;
            bind_.lengths_[index_] = sizeof(unsigned);
            bind_.formats_[index_] = 0;
          }

          void operator()(int value) const {
            bind_.values_[index_] = strdup(std::to_string(value).c_str());
            bind_.types_[index_] = INT4OID;
            bind_.lengths_[index_] = sizeof(int);
            bind_.formats_[index_] = 0;
          }
          void operator()(unsigned long value) const {
            bind_.values_[index_] = strdup(std::to_string(value).c_str());
            bind_.types_[index_] = INT4OID;
            bind_.lengths_[index_] = sizeof(unsigned long);
            bind_.formats_[index_] = 0;
          }

          void operator()(long value) const {
            bind_.values_[index_] = strdup(std::to_string(value).c_str());
            bind_.types_[index_] = INT4OID;
            bind_.lengths_[index_] = sizeof(long);
            bind_.formats_[index_] = 0;
          }
          void operator()(short value) const {
            bind_.values_[index_] = strdup(std::to_string(value).c_str());
            bind_.types_[index_] = INT2OID;
            bind_.lengths_[index_] = sizeof(short);
            bind_.formats_[index_] = 0;
          }
          void operator()(unsigned short value) const {
            bind_.values_[index_] = strdup(std::to_string(value).c_str());
            bind_.types_[index_] = INT2OID;
            bind_.lengths_[index_] = sizeof(unsigned short);
            bind_.formats_[index_] = 0;
          }
          void operator()(const sql_null_type &value) const {
            bind_.values_[index_] = 0;
            bind_.types_[index_] = UNKNOWNOID;
            bind_.lengths_[index_] = 0;
            bind_.formats_[index_] = 0;
          }

          private:
          binding &bind_;
          size_t index_;
        };

        /**
         * a visitor to apply a value to a postgres binding
         */
        class from_value {
          public:
          from_value(binding &bind, size_t index)
              : bind_(bind), index_(index) {}
          void operator()(const sql_time &value) const {
            bind_.values_[index_] = strdup(value.to_string().c_str());
            switch (value.format()) {
            case sql_time::DATE:
              bind_.types_[index_] = DATEOID;
              break;
            case sql_time::TIME:
              bind_.types_[index_] = TIMEOID;
              break;
            case sql_time::DATETIME:
            case sql_time::TIMESTAMP:
              bind_.types_[index_] = TIMESTAMPOID;
              break;
            }
            bind_.lengths_[index_] = sizeof(time_t);
            bind_.formats_[index_] = 0;
          }
          void operator()(const sql_null_type &value) const {
            bind_.values_[index_] = nullptr;
            bind_.lengths_[index_] = 0;
            bind_.types_[index_] = UNKNOWNOID;
            bind_.formats_[index_] = 0;
          }
          void operator()(const sql_blob &value) const {

            bind_.values_[index_] =
                static_cast<char *>(c_copy(value.get(), value.size()));
            bind_.types_[index_] = BYTEAOID;
            bind_.lengths_[index_] = value.size();
            bind_.formats_[index_] = 1;
          }
          void operator()(const sql_wstring &value) const {
            string temp = helper::convert_string(value);
            bind_.values_[index_] = strdup(temp.c_str());
            bind_.types_[index_] = TEXTOID;
            bind_.lengths_[index_] = temp.size();
            bind_.formats_[index_] = 0;
          }
          void operator()(const sql_string &value) const {
            bind_.values_[index_] = strdup(value.c_str());
            bind_.types_[index_] = TEXTOID;
            bind_.lengths_[index_] = value.size();
            bind_.formats_[index_] = 0;
          }
          void operator()(const sql_number &value) const {
            value.apply_visitor(from_number(bind_, index_));
          }

          private:
          binding &bind_;
          size_t index_;
        };
      } // namespace data_mapper

      binding::binding() : binding(prealloc_size) {}

      binding::binding(size_t size)
          : values_(nullptr), types_(nullptr), lengths_(nullptr),
            formats_(nullptr), size_(size) {
        values_ = c_alloc<char *>(size);
        types_ = c_alloc<Oid>(size);
        lengths_ = c_alloc<int>(size);
        formats_ = c_alloc<int>(size);
      }

      void binding::clear_value(size_t i) {
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

      void binding::clear_value() {
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

      void binding::copy_value(const binding &value) {
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
          : bind_mapping(other), values_(nullptr), types_(nullptr),
            lengths_(nullptr), formats_(nullptr), size_(0) {
        copy_value(other);
      }
      binding::binding(binding &&other)
          : bind_mapping(std::move(other)), values_(other.values_),
            types_(other.types_), lengths_(other.lengths_),
            formats_(other.formats_), size_(other.size_) {
        other.values_ = nullptr;
        other.types_ = nullptr;
        other.lengths_ = nullptr;
        other.formats_ = nullptr;
        other.size_ = 0;
      }

      binding &binding::operator=(const binding &other) {
        bind_mapping::operator=(other);
        copy_value(other);
        return *this;
      }

      binding &binding::operator=(binding &&other) {
        bind_mapping::operator=(std::move(other));
        clear_value();
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
      binding::~binding() { clear_value(); }

      sql_value binding::to_value(size_t index) const {
        if (index >= size_ || values_ == nullptr || values_[index] == nullptr) {
          return sql_null;
        }
        return data_mapper::to_value(types_[index], values_[index],
                                     lengths_[index]);
      }

      int binding::sql_type(size_t index) const {
        if (index >= size_) {
          return 0;
        }

        return types_[index];
      }

      bool binding::reallocate_value(size_t index) {
        // assert non-zero-indexed
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

        values_ = c_alloc<char *>(values_, index, size_);
        types_ = c_alloc<Oid>(types_, index, size_);
        lengths_ = c_alloc<int>(lengths_, index, size_);
        formats_ = c_alloc<int>(formats_, index, size_);

        size_ = index;

        return true;
      }

      /**
       * binding methods ensure the dynamic array is sized properly and store
       * the value as a memory pointer
       */
      binding &binding::bind(size_t index, const sql_value &value) {
        if (reallocate_value(index)) {
          value.apply_visitor(data_mapper::from_value(*this, index - 1));
        }
        return *this;
      }
      binding &binding::bind(const string &name, const sql_value &value) {
        bind_mapping::bind(name, value);
        return *this;
      }

      size_t binding::num_of_bindings() const noexcept {
        size_t count = 0;
        for (size_t i = 0; i < size_; i++) {
          if (types_[i] != 0) {
            count++;
          }
        }
        return count;
      }

      size_t binding::capacity() const { return size_; }

      void binding::reset() {
        bind_mapping::reset();
        clear_value();
      }

      std::string binding::prepare(const string &sql) {
#ifdef ENABLE_PARAMETER_MAPPING
        auto match_begin =
            std::sregex_iterator(sql.begin(), sql.end(), bindable::index_regex);
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

        match_begin =
            std::sregex_iterator(sql.begin(), sql.end(), bindable::param_regex);
        match_end = std::sregex_iterator();

        for (auto match = match_begin; match != match_end; ++match) {
          auto str = match->str();
          // if it is a named parameter...
          if (str[0] == '@' || str[0] == ':') {
            // get the determined positions for the named parameter
            auto mapped = get_named_param_indexes(str);
            if (!mapped.empty()) {
              // postgres only needs the first determined index as it can re-use
              // parameter indexes
              auto pos = *mapped.begin();
              // replace the sql with the positional parameter
              formatted.replace(formatted.find(str), str.length(),
                                "$" + std::to_string(pos));
            }
            continue;
          }

          // if its not a postgres parameter, replace it with one
          if (str[0] == '?') {
            // otherwise, replace the matched paramter with the current index
            auto pos = indexes.back();
            formatted.replace(formatted.find(str), str.length(),
                              "$" + std::to_string(pos));
            indexes.pop_back();
          }
        }

        return formatted;
#else
        return sql;
#endif
      }
    } // namespace postgres
  }   // namespace db
} // namespace coda
