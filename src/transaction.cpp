#include "transaction.h"
#include "exception.h"
#include "session.h"
#include "sqldb.h"

namespace coda {
  namespace db {
    transaction::transaction(const std::shared_ptr<session_type> &session,
                             const std::shared_ptr<transaction_impl> &impl)
        : successful_(false), session_(session), impl_(impl) {}

    transaction::transaction(const transaction &other)
        : successful_(other.successful_), session_(other.session_),
          impl_(other.impl_) {}

    transaction::transaction(transaction &&other)
        : successful_(other.successful_), session_(std::move(other.session_)),
          impl_(std::move(other.impl_)) {
      other.impl_ = nullptr;
      other.session_ = nullptr;
    }

    transaction::~transaction() {
      if (is_active()) {
        if (is_successful()) {
          commit();
        } else {
          rollback();
        }
      }
    }

    transaction &transaction::operator=(const transaction &other) {
      successful_ = other.successful_;
      session_ = other.session_;
      impl_ = other.impl_;
      return *this;
    }

    transaction &transaction::operator=(transaction &&other) {
      successful_ = other.successful_;
      session_ = std::move(other.session_);
      impl_ = std::move(other.impl_);
      other.impl_ = nullptr;
      other.session_ = nullptr;
      return *this;
    }

    void transaction::start() { impl_->start(); }

    void transaction::commit() {
      if (!session_->impl()->execute("COMMIT;")) {
        throw transaction_exception("unable to commit transaction: " +
                                    session_->last_error());
      }
    }

    void transaction::rollback() {
      if (!session_->impl()->execute("ROLLBACK;")) {
        throw transaction_exception("unable to rollback transaction: " +
                                    session_->last_error());
      }
    }

    void transaction::save(const std::string &name) {
      if (name.empty()) {
        return;
      }

      if (!session_->impl()->execute("SAVEPOINT " + name + ";")) {
        throw transaction_exception("unable to save point " + name + ": " +
                                    session_->last_error());
      }
    }

    void transaction::release(const std::string &name) {
      if (name.empty()) {
        return;
      }

      if (!session_->impl()->execute("RELEASE SAVEPOINT " + name + ";")) {
        throw transaction_exception("unable to release save point " + name +
                                    ": " + session_->last_error());
      }
    }

    void transaction::rollback(const std::string &name) {
      if (name.empty()) {
        return;
      }

      if (!session_->impl()->execute("ROLLBACK TO SAVEPOINT " + name + ";")) {
        throw transaction_exception("unable to rollback save point " + name +
                                    ": " + session_->last_error());
      }
    }

    bool transaction::is_active() const noexcept {
      return impl_ != nullptr && impl_->is_active();
    }

    std::shared_ptr<transaction::session_type>
    transaction::get_session() const {
      return session_;
    }

    void transaction::set_successful(bool value) { successful_ = value; }

    bool transaction::is_successful() const noexcept { return successful_; }

    std::shared_ptr<transaction_impl> transaction::impl() const {
      return impl_;
    }
  } // namespace db
} // namespace coda