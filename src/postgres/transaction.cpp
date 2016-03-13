#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include "transaction.h"
#include "../exception.h"

namespace arg3
{
    namespace db
    {
        namespace postgres
        {
            transaction::transaction(const std::shared_ptr<PGconn> &db, const transaction::mode &mode) : db_(db), mode_(mode)
            {
            }

            bool transaction::is_active() const
            {
                PGTransactionStatusType type = PQtransactionStatus(db_.get());

                return type == PQTRANS_ACTIVE || type == PQTRANS_INTRANS;
            }
            void transaction::start()
            {
                std::string buf = "START TRANSACTION";

                switch (mode_.isolation) {
                    default:
                    case isolation::none:
                        break;
                    case isolation::serializable:
                        buf += " ISOLATION LEVEL SERIALIZABLE";
                        break;
                    case isolation::repeatable_read:
                        buf += " ISOLATION LEVEL REPEATABLE READ";
                        break;
                    case isolation::read_commited:
                        buf += " ISOLATION LEVEL READ COMMITTED";
                        break;
                    case isolation::read_uncommited:
                        buf += " ISOLATION LEVEL READ UNCOMMITTED";
                        break;
                }
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
                if (mode_.deferrable && mode_.isolation == isolation::serializable && mode_.type == db::transaction::read_only) {
                    if (mode_.deferrable == 1) {
                        buf += " DEFERABLE";
                    } else {
                        buf += " NOT DEFERABLE";
                    }
                }

                buf += ";";

                PGresult *res = PQexec(db_.get(), buf.c_str());

                if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                    throw transaction_exception(std::string("unable to start transaction: ") + PQerrorMessage(db_.get()));
                }
            }

            void transaction::commit()
            {
                PGresult *res = PQexec(db_.get(), "COMMIT;");

                if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                    throw transaction_exception(std::string("unable to commit transaction: ") + PQerrorMessage(db_.get()));
                }
            }

            void transaction::rollback()
            {
                PGresult *res = PQexec(db_.get(), "ROLLBACK;");

                if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                    throw transaction_exception(std::string("unable to rollback transaction: ") + PQerrorMessage(db_.get()));
                }
            }
        }
    }
}

#endif
