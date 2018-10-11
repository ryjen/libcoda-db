
#include "bind_mapping.h"
#ifndef ENABLE_PARAMETER_MAPPING
#include "exception.h"
#endif

namespace coda::db {
#ifdef ENABLE_PARAMETER_MAPPING

  bind_mapping::bind_mapping() {}

  bind_mapping::bind_mapping(const bind_mapping &other) : mappings_(other.mappings_) {}

  bind_mapping::bind_mapping(bind_mapping &&other) : mappings_(std::move(other.mappings_)) {}

  bind_mapping::~bind_mapping() {}

  bind_mapping &bind_mapping::operator=(const bind_mapping &other) {
    mappings_ = other.mappings_;
    return *this;
  }

  bind_mapping &bind_mapping::operator=(bind_mapping &&other) {
    mappings_ = std::move(other.mappings_);
    return *this;
  }
  bind_mapping &bind_mapping::bind(const std::string &name, const sql_value &value) {
    if (mappings_.count(name) == 0) {
      throw binding_error("No parameter named '" + name + "' found");
    }

    auto indexes = mappings_[name];

    for (auto &index : indexes) {
      bind(index, value);
    }
    return *this;
  }

  void bind_mapping::add_named_param(const std::string &name, size_t index) {
    if (name.empty()) {
      return;
    }

    mappings_[name].insert(index);
  }

  void bind_mapping::rem_named_param(const std::string &name, size_t index) {
    if (name.empty()) {
      return;
    }

    mappings_[name].erase(index);
  }

  std::set<size_t> bind_mapping::get_named_param_indexes(const std::string &name) {
    if (name.empty()) {
      return std::set<size_t>();
    }

    return mappings_[name];
  }

  std::string bind_mapping::prepare(const std::string &sql, size_t max_index) {
    mappings_.clear();

    auto match_begin = std::sregex_iterator(sql.begin(), sql.end(), named_regex);
    auto match_end = std::sregex_iterator();

    for (auto match = match_begin; match != match_end; ++match) {
      add_named_param(match->str(), ++max_index);
    }

    return sql;
  }

  bool bind_mapping::is_named() const { return mappings_.size() > 0; }

  void bind_mapping::reset() { mappings_.clear(); }
#else

  bind_mapping &bind_mapping::bind(const std::string &name, const sql_value &value) {
    throw database_exception("not implemented");
  }
  std::string bind_mapping::prepare(const std::string &sql, size_t max_index) { return sql; }
  bool bind_mapping::is_named() const { return false; }

  void bind_mapping::reset() {}

#endif
}  // namespace coda::db
