
#include "transaction.h"
#include "../exception.h"

namespace rj
{
    namespace db
    {
        namespace postgres
        {
            transaction::transaction(const std::shared_ptr<PGconn> &db, const transaction::mode &mode)
                : db_(db), mode_(mode)
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
                    case db::transaction::isolation::none:
                        break;
                    case db::transaction::isolation::serializable:
                        buf += " ISOLATION LEVEL SERIALIZABLE";
                        break;
                    case db::transaction::isolation::repeatable_read:
                        buf += " ISOLATION LEVEL REPEATABLE READ";
                        break;
                    case db::transaction::isolation::read_commited:
                        buf += " ISOLATION LEVEL READ COMMITTED";
                        break;
                    case db::transaction::isolation::read_uncommited:
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
                if (mode_.deferrable && mode_.isolation == db::transaction::isolation::serializable &&
                    mode_.type == db::transaction::read_only) {
                    if (mode_.deferrable == 1) {
                        buf += " DEFERABLE";
                    } else {
                        buf += " NOT DEFERABLE";
                    }
                }

                buf += ";";

                PGresult *res = PQexec(db_.get(), buf.c_str());

                bool error = PQresultStatus(res) != PGRES_COMMAND_OK;

                PQclear(res);

                if (error) {
                    throw transaction_exception(std::string("unable to start transaction: ") +
                                                PQerrorMessage(db_.get()));
                }
            }
        }
    }
}
