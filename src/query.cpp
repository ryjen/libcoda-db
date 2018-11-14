/*!
 * implementation of a query
 * @copyright ryan jennings (coda.life), 2013
 */
#include "query.h"
#include "statement.h"
#include "where_clause.h"

using namespace std;

namespace coda {
  namespace db {
    query::query(const std::shared_ptr<coda::db::session> &session)
        : dirty_(false), session_(session), stmt_(nullptr), params_(),
          named_params_() {
      if (session_ == nullptr) {
        throw database_exception("No database provided for query");
      }
    }

    query::query(const query &other) noexcept
        : dirty_(false), session_(other.session_), stmt_(other.stmt_),
          params_(other.params_), named_params_(other.named_params_) {}

    query::query(query &&other) noexcept
        : dirty_(false), session_(std::move(other.session_)),
          stmt_(std::move(other.stmt_)), params_(std::move(other.params_)),
          named_params_(std::move(other.named_params_)) {
      other.session_ = nullptr;
      other.stmt_ = nullptr;
    }

    query::~query() {}

    query &query::operator=(const query &other) {
      dirty_ = other.dirty_;
      session_ = other.session_;
      stmt_ = other.stmt_;
      params_ = other.params_;
      named_params_ = other.named_params_;
      return *this;
    }

    query &query::operator=(query &&other) {
      dirty_ = other.dirty_;
      session_ = std::move(other.session_);
      stmt_ = std::move(other.stmt_);
      params_ = std::move(other.params_);
      named_params_ = std::move(other.named_params_);
      other.session_ = nullptr;
      other.stmt_ = nullptr;

      return *this;
    }

    std::shared_ptr<query::session_type> query::get_session() const {
      return session_;
    }

    void query::prepare(const string &sql) {

      if (stmt_ == nullptr || dirty_) {
        stmt_ = session_->create_statement();

        stmt_->prepare(sql);
      } else if (!dirty_) {
        return;
      }

      for (size_t i = 1; i <= params_.size(); i++) {
        auto &value = params_[i - 1];

        stmt_->bind(i, value);
      }

      for (auto &it : named_params_) {
        stmt_->bind(it.first, it.second);
      }

      dirty_ = false;
    }

    size_t query::assert_binding_index(size_t index) {
      if (index == 0) {
        throw binding_error("parameter index must be greater than zero");
      }

      if (index > params_.size()) {
        params_.resize(index);
        dirty_ = true;
      }

      return index - 1;
    }

    void query::set_modified() {
      dirty_ = true;
      sql_generator::reset();
    }

    bindable &query::bind(size_t index, const sql_value &value) {
      params_[assert_binding_index(index)] = value;
      set_modified();
      return *this;
    }

    bindable &query::bind(const string &name, const sql_value &value) {
      named_params_[name] = value;
      set_modified();
      return *this;
    }

    string query::last_error() {
      if (stmt_ == nullptr) {
        return string();
      }

      return stmt_->last_error();
    }

    size_t query::num_of_bindings() const noexcept { return params_.size(); }

    bool query::is_valid() const noexcept { return session_ != nullptr; }

    void query::reset() {
      params_.clear();
      named_params_.clear();
      dirty_ = false;
      stmt_->reset();
    }
  } // namespace db
} // namespace coda
