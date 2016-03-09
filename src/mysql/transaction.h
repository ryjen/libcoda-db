#ifndef ARG3_DB_MYSQL_TRANSACTION_H
#define ARG3_DB_MYSQL_TRANSACTION_H

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include "../transaction.h"

namespace arg3
{
    namespace db
    {
        namespace mysql
        {
            class transaction : public transaction_impl
            {
                friend class session;

               public:
                typedef struct {
                    arg3::db::transaction::type type;
                    isolation::level isolation;
                } mode;

                transaction(const std::shared_ptr<MYSQL> &db, const transaction::mode &mode = {});

                void start();
                void commit();
                void rollback();
                bool is_active() const;

               private:
                std::shared_ptr<MYSQL> db_;
                transaction::mode mode_;
                bool active_;
            };
        }
    }
}

#endif

#endif
