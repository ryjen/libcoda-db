
#include "row.h"
#include "../exception.h"
#include "binding.h"
#include "column.h"
#include "session.h"

using namespace std;

namespace coda::db::mysql {
      row::row(const std::shared_ptr<mysql::session> &sess, const shared_ptr<MYSQL_RES> &res, MYSQL_ROW row)
          : row_impl(), row_(row), res_(res), sess_(sess) {
        if (sess_ == nullptr) {
          throw database_exception("no database provided for mysql row");
        }

        if (row_ == nullptr) {
          throw database_exception("no raw data for mysql row");
        }

        if (res_ == nullptr) {
          throw database_exception("no mysql result provided for row");
        }

        size_ = mysql_num_fields(res.get());
      }

      row::column_type row::column(size_t position) const {
        if (!is_valid()) {
          throw database_exception("invalid mysql row");
        }

        if (position >= size()) {
          throw no_such_column_exception();
        }

        return row_impl::column_type(make_shared<mysql::column>(res_, row_, position));
      }

      row::column_type row::column(const string &name) const {
        if (!is_valid()) {
          throw database_exception("invalid mysql row");
        }

        if (name.empty()) {
          throw no_such_column_exception();
        }

        for (size_t i = 0; i < size_; i++) {
          auto field = mysql_fetch_field_direct(res_.get(), i);

          if (field != nullptr && field->name != nullptr && name == field->name) {
            return column(i);
          }
        }
        throw no_such_column_exception(name);
      }

      string row::column_name(size_t position) const {
        if (!is_valid()) {
          throw database_exception("invalid mysql row");
        }

        if (position >= size()) {
          throw no_such_column_exception();
        }

        auto field = mysql_fetch_field_direct(res_.get(), static_cast<unsigned int>(position));

        if (field == nullptr || field->name == nullptr) {
          throw no_such_column_exception();
        }

        return field->name;
      }

      size_t row::size() const noexcept { return size_; }

      bool row::is_valid() const noexcept { return res_ != nullptr && res_ && row_ != nullptr; }

      /* statement version */

      stmt_row::stmt_row(const std::shared_ptr<mysql::session> &sess, const shared_ptr<MYSQL_STMT> &stmt,
                         const shared_ptr<MYSQL_RES> &metadata, const shared_ptr<mysql::binding> &fields)
          : row_impl(), fields_(fields), metadata_(metadata), stmt_(stmt), sess_(sess), size_(0) {
        if (sess_ == nullptr) {
          throw database_exception("No database provided for mysql row");
        }

        if (metadata_ != nullptr) {
          size_ = mysql_num_fields(metadata_.get());
        }
      }

      stmt_row::stmt_row(stmt_row &&other) noexcept
          : row_impl(std::move(other)),
            fields_(std::move(other.fields_)),
            metadata_(std::move(other.metadata_)),
            stmt_(std::move(other.stmt_)),
            sess_(std::move(other.sess_)),
            size_(other.size_) {
        other.sess_ = nullptr;
        other.fields_ = nullptr;
        other.metadata_ = nullptr;
        other.stmt_ = nullptr;
      }

      stmt_row &stmt_row::operator=(stmt_row &&other) noexcept {
        fields_ = std::move(other.fields_);
        metadata_ = std::move(other.metadata_);
        sess_ = std::move(other.sess_);
        size_ = other.size_;
        stmt_ = std::move(other.stmt_);
        other.sess_ = nullptr;
        other.fields_ = nullptr;
        other.metadata_ = nullptr;
        other.stmt_ = nullptr;

        return *this;
      }

      stmt_row::column_type stmt_row::column(size_t position) const {
        if (!is_valid()) {
          throw database_exception("invalid mysql row");
        }

        if (position >= size()) {
          throw no_such_column_exception();
        }

        return column_type(make_shared<stmt_column>(column_name(position), fields_, position));
      }

      stmt_row::column_type stmt_row::column(const string &name) const {
        if (!is_valid()) {
          throw database_exception("invalid mysql row");
        }

        if (name.empty()) {
          throw no_such_column_exception();
        }

        for (size_t i = 0; i < size(); i++) {
          auto field = mysql_fetch_field_direct(metadata_.get(), i);

          if (field != nullptr && field->name != nullptr && name == field->name) {
            return column(i);
          }
        }
        throw no_such_column_exception(name);
      }

      string stmt_row::column_name(size_t position) const {
        if (!is_valid()) {
          throw database_exception("invalid mysql row");
        }
        if (position >= size()) {
          throw no_such_column_exception();
        }

        auto field = mysql_fetch_field_direct(metadata_.get(), static_cast<unsigned int>(position));

        if (field == nullptr || field->name == nullptr) return string();

        return field->name;
      }

      size_t stmt_row::size() const noexcept { return size_; }

      bool stmt_row::is_valid() const noexcept { return fields_ != nullptr && metadata_ != nullptr; }
}  // namespace coda::db::mysql
