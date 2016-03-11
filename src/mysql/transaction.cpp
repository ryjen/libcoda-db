#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include "transaction.h"
#include "../exception.h"

namespace arg3
{
    namespace db
    {
        namespace mysql
        {
            transaction::transaction(const std::shared_ptr<MYSQL> &db, const transaction::mode &mode) : db_(db), mode_(mode), active_(false)
            {
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
                    throw transaction_exception("unable to start transaction");
                }

                active_ = true;
            }

            void transaction::commit()
            {
                if (mysql_commit(db_.get())) {
                    throw transaction_exception("unable to commit transaction");
                }
                active_ = false;
            }

            void transaction::rollback()
            {
                if (mysql_rollback(db_.get())) {
                    throw transaction_exception("unable to rollback transaction");
                }
                active_ = false;
            }
        }
    }
}

#endif
