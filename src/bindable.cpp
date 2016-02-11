#include "bindable.h"
#include "exception.h"

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
                case variant::CUSTOM:
                    if (value.is_time()) {
                        bind(index, value.to_time());
                    } else {
                        throw binding_error("unknown custom type in binding");
                    }
                    break;
            }
            return *this;
        }

        bindable &bindable::bind_all(size_t index, const sql_value &value)
        {
            bind_value(index, value);
            return *this;
        }
    }
}