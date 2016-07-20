#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include "../exception.h"
#include "transaction.h"

namespace arg3
{
    namespace db
    {
        namespace mysql
        {
            transaction::transaction(const std::shared_ptr<MYSQL> &db, const transaction::mode &mode) : db_(db), mode_(mode), active_(false)
            {
            }

            transaction::transaction(const transaction &other) : db_(other.db_), mode_(other.mode_), active_(other.active_)
            {
            }

            transaction::transaction(transaction &&other)
                : db_(std::move(other.db_)), mode_(std::move(other.mode_)), active_(std::move(other.active_))
            {
                other.db_ = nullptr;
            }
            transaction::~transaction()
            {
            }
            transaction &transaction::operator=(const transaction &other)
            {
                db_ = other.db_;
                mode_ = other.mode_;
                active_ = other.active_;
                return *this;
            }
            transaction &transaction::operator=(transaction &&other)
            {
                db_ = std::move(other.db_);
                mode_ = std::move(other.mode_);
                active_ = std::move(other.active_);
                return *this;
            }

            bool transaction::is_active() const
            {
                return active_;
            }

            void transaction::start()
            {
                std::string buf = "START TRANSACTION";

                switch (mode_.type) {
                    default:
                    case db::transaction::none:
                        break;
                    case db::transaction::read_write:
                        buf += " READ WRITE";
                        break;
                    case db::transaction::read_only:
                        buf += " READ ONLY";
                        break;
                }

                buf += ";";

                if (mysql_query(db_.get(), buf.c_str())) {
                    throw transaction_exception(std::string("unable to start transaction: ") + mysql_error(db_.get()));
                }

                active_ = true;
            }
        }
    }
}

#endif
