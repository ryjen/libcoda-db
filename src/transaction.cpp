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
            session_ = other.session_;
            impl_ = other.impl_;
            return *this;
        }

        transaction &transaction::operator=(transaction &&other)
        {
            session_ = std::move(other.session_);
            impl_ = std::move(other.impl_);
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
            impl_->commit();
        }

        void transaction::rollback()
        {
            log::trace("ROLLBACK TRANSACTION");
            impl_->rollback();
        }

        void transaction::save(const std::string &name)
        {
            log::trace("SAVEPOINT");

            if (!session_->execute("SAVEPOINT " + name + ";")) {
                throw transaction_exception("unable to save point " + name + ": " + session_->last_error());
            }
        }

        void transaction::release(const std::string &name)
        {
            log::trace("RELEASE SAVEPOINT");

            if (!session_->execute("RELEASE TO " + name + ";")) {
                throw transaction_exception("unable to release save point " + name + ": " + session_->last_error());
            }
        }

        void transaction::rollback(const std::string &name)
        {
            log::trace("ROLLBACK SAVEPOINT");

            if (!session_->execute("ROLLBACK TO " + name + ";")) {
                throw transaction_exception("unable to rollback to save point " + name + ": " + session_->last_error());
            }
        }

        bool transaction::is_active() const
        {
            return impl_->is_active();
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
    }
}