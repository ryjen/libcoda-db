
#include "transaction.h"
#include "../exception.h"

namespace rj
{
    namespace db
    {
        namespace sqlite
        {
            transaction::transaction(const std::shared_ptr<sqlite3> &db, transaction::type type) : db_(db), type_(type)
            {
            }
            bool transaction::is_active() const
            {
                return !sqlite3_get_autocommit(db_.get());
            }

            void transaction::start()
            {
                std::string buf = "BEGIN";

                switch (type_) {
                    default:
                    case none:
                        break;
                    case deferred:
                        buf += " DEFERRED";
                        break;
                    case immediate:
                        buf += " IMMEDIATE";
                        break;
                    case exclusive:
                        buf += " EXCLUSIVE";
                        break;
                }

                buf += " TRANSACTION;";

                if (sqlite3_exec(db_.get(), buf.c_str(), NULL, NULL, NULL) != SQLITE_OK) {
                    throw transaction_exception(std::string("unable to start transaction: ") + sqlite3_errmsg(db_.get()));
                }
            }
        }
    }
}
