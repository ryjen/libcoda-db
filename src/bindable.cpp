#include <regex>
#include "bindable.h"
#include "exception.h"
#include "log.h"

namespace arg3
{
    namespace db
    {
        bindable &bindable::bind_value(size_t index, const sql_value &value)
        {
            switch (value.type()) {
                case variant::NULLTYPE:
                    bind(index, sql_null);
                    break;
                case variant::CHAR:
                case variant::WCHAR:
                case variant::BOOL:
                case variant::NUMBER:
                    if (value.size() <= sizeof(int)) {
                        bind(index, value.to_int());
                    } else {
                        bind(index, value.to_llong());
                    }
                    break;
                case variant::UNUMBER:
                    bind(index, value.to_llong());
                    break;
                case variant::REAL:
                    if (value.size() <= sizeof(float)) {
                        bind(index, value.to_float());
                    } else {
                        bind(index, value.to_double());
                    }
                    break;
                case variant::STRING:
                    bind(index, value.to_string());
                    break;
                case variant::WSTRING:
                    bind(index, value.to_wstring());
                    break;
                case variant::BINARY:
                    bind(index, value.to_binary());
                    break;
                case variant::COMPLEX:
                    if (value.is_time()) {
                        bind(index, value.to_time());
                    } else {
                        throw binding_error("unknown custom type in binding");
                    }
                    break;
            }
            return *this;
        }

        bind_mapping::bind_mapping()
        {
        }

        bind_mapping::bind_mapping(const bind_mapping &other) : mappings_(other.mappings_)
        {
        }

        bind_mapping::bind_mapping(bind_mapping &&other) : mappings_(std::move(other.mappings_))
        {
        }

        bind_mapping::~bind_mapping()
        {
        }

        bind_mapping &bind_mapping::operator=(const bind_mapping &other)
        {
            mappings_ = other.mappings_;
            return *this;
        }

        bind_mapping &bind_mapping::operator=(bind_mapping &&other)
        {
            mappings_ = std::move(other.mappings_);
            return *this;
        }
        bind_mapping &bind_mapping::bind(const std::string &name, const sql_value &value)
        {
            if (mappings_.count(name) == 0) {
                throw binding_error("No parameter named '" + name + "' found");
            }

            auto indexes = mappings_[name];

            for (auto &index : indexes) {
                bind_value(index, value);
            }
            return *this;
        }

        void bind_mapping::add_named_param(const std::string &name, unsigned index)
        {
            if (name.empty()) {
                return;
            }

            mappings_[name].insert(index);
        }

        void bind_mapping::rem_named_param(const std::string &name, unsigned index)
        {
            if (name.empty()) {
                return;
            }

            mappings_[name].erase(index);
        }

        std::string bind_mapping::prepare(const std::string &sql)
        {
            static std::regex named_regex("([@:]\\w+)");
            std::smatch matches;

            auto match_begin = std::sregex_iterator(sql.begin(), sql.end(), named_regex);
            auto match_end = std::sregex_iterator();

            unsigned index = 0;
            for (auto match = match_begin; match != match_end; ++match) {
                add_named_param(match->str(), ++index);
            }

            return sql;
        }

        bool bind_mapping::is_named() const
        {
            return mappings_.size() > 0;
        }

        void bind_mapping::reset()
        {
            mappings_.clear();
        }
    }
}
