#include "bindable.h"
#include "exception.h"
#include "sql_value.h"

namespace coda {
  namespace db {
    const int bindable::prealloc_size = 10;
    const int bindable::prealloc_increment = 5;

#ifdef ENABLE_PARAMETER_MAPPING
    // these ugly beasts will find parameters not in quotes
    const std::regex bindable::param_regex(
        "([@:]\\w+|\\$([0-9]+)|\\?)(?=(?:[^\"']|[\"'][^\"']*[\"'])*$)");
    const std::regex bindable::index_regex(
        "(\\$([0-9]+)|\\?)(?=(?:[^\"']|[\"'][^\"']*[\"'])*$)");
    const std::regex
        bindable::named_regex("([@:]\\w+)(?=(?:[^\"']|[\"'][^\"']*[\"'])*$)");
#endif

    bindable &bindable::bind(const std::vector<sql_value> &values,
                             size_t start_index) {
      size_t index = start_index;
      for (auto &value : values) {
        bind(index++, value);
      }
      return *this;
    }

    bindable &
    bindable::bind(const std::unordered_map<std::string, sql_value> &values) {
      for (auto &pair : values) {
        bind(pair.first, pair.second);
      }
      return *this;
    }
  } // namespace db
} // namespace coda
