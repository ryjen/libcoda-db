#include "session.h"
#include "sqldb.h"
#include "transaction.h"
#include "exception.h"
#include "log.h"

namespace arg3
{
    namespace db
    {
        transaction::transaction(const std::shared_ptr<session_type> &session, const std::shared_ptr<transaction_impl> &impl)
            : successful_(false), session_(session), impl_(impl)
        {
        }

        transaction::transaction(const transaction &other) : successful_(other.successful_), session_(other.session_), impl_(other.impl_)
        {
        }

        transaction::transaction(transaction &&other)
            : successful_(other.successful_), session_(std::move(other.session_)), impl_(std::move(other.impl_))
        {
            other.impl_ = nullptr;
            other.session_ = nullptr;
        }

        transaction::~transaction()
        {
            if (is_active()) {
                if (is_successful()) {
                    commit();
                } else {
                    rollback();
                }
            }
        }

        transaction &transaction::operator=(const transaction &other)
        {
            successful_ = other.successful_;
            session_ = other.session_;
            impl_ = other.impl_;
            return *this;
        }

        transaction &transaction::operator=(transaction &&other)
        {
            successful_ = other.successful_;
            session_ = std::move(other.session_);
            impl_ = std::move(other.impl_);
            other.impl_ = nullptr;
            other.session_ = nullptr;
            return *this;
        }

        void transaction::start()
        {
            log::trace("START TRANSACTION");
            impl_->start();
        }

        void transaction::commit()
        {
            log::trace("COMMIT TRANSACTION");
            if (!session_->execute("COMMIT;")) {
                throw transaction_exception("unable to commit transaction: " + session_->last_error());
            }
        }

        void transaction::rollback()
        {
            log::trace("ROLLBACK TRANSACTION");
            if (!session_->execute("ROLLBACK;")) {
                throw transaction_exception("unable to rollback transaction: " + session_->last_error());
            }
        }

        void transaction::save(const std::string &name)
        {
            if (name.empty()) {
                return;
            }
            log::trace("SAVEPOINT");

            if (!session_->execute("SAVEPOINT " + name + ";")) {
                throw transaction_exception("unable to save point " + name + ": " + session_->last_error());
            }
        }

        void transaction::release(const std::string &name)
        {
            if (name.empty()) {
                return;
            }
            log::trace("RELEASE SAVEPOINT");

            if (!session_->execute("RELEASE SAVEPOINT " + name + ";")) {
                throw transaction_exception("unable to release save point " + name + ": " + session_->last_error());
            }
        }

        void transaction::rollback(const std::string &name)
        {
            if (name.empty()) {
                return;
            }
            log::trace("ROLLBACK SAVEPOINT");

            if (!session_->execute("ROLLBACK TO SAVEPOINT " + name + ";")) {
                throw transaction_exception("unable to rollback save point " + name + ": " + session_->last_error());
            }
        }

        bool transaction::is_active() const
        {
            return impl_ != nullptr && impl_->is_active();
        }

        std::shared_ptr<transaction::session_type> transaction::get_session() const
        {
            return session_;
        }

        void transaction::set_successful(bool value)
        {
            successful_ = value;
        }

        bool transaction::is_successful() const
        {
            return successful_;
        }

        std::shared_ptr<transaction_impl> transaction::impl() const
        {
            return impl_;
        }
    }
}