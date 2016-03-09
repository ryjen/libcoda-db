#include "session.h"
#include "sqldb.h"
#include "transaction.h"
#include "log.h"

namespace arg3
{
    namespace db
    {
        transaction::transaction(const std::shared_ptr<session> &session, const std::shared_ptr<transaction_impl> &impl)
            : session_(session), impl_(impl)
        {
        }

        transaction::transaction(const transaction &other) : session_(other.session_), impl_(other.impl_)
        {
        }

        transaction::transaction(transaction &&other) : session_(std::move(other.session_)), impl_(std::move(other.impl_))
        {
        }

        transaction::~transaction()
        {
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
            if (!session_->execute("SAVEPOINT " + name + ";")) {
                throw transaction_exception("unable to save point " + name + ": " + session_->last_error());
            }
        }

        void transaction::release(const std::string &name)
        {
            if (!session_->execute("RELEASE TO " + name + ";")) {
                throw transaction_exception("unable to release save point " + name + ": " + session_->last_error());
            }
        }

        void transaction::rollback(const std::string &name)
        {
            if (!session_->execute("ROLLBACK TO " + name + ";")) {
                throw transaction_exception("unable to rollback to save point " + name + ": " + session_->last_error());
            }
        }

        transaction::operator std::shared_ptr<session>() const
        {
            return session_;
        }
    }
}