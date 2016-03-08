#ifndef ARG3_DB_TRANSACTION_H
#define ARG3_DB_TRANSACTION_H

#include <string>
#include <memory>

namespace arg3
{
    namespace db
    {
        class session;

        class transaction_impl
        {
           public:
            virtual void start() = 0;
            virtual void commit() = 0;
            virtual void rollback() = 0;
            virtual bool is_active() const = 0;
        };

        namespace isolation
        {
            typedef enum { none, serializable, repeatable_read, read_commited, read_uncommited } level;
        }

        class transaction
        {
            friend class sqldb;

           public:
            typedef enum { none, read_write, read_only } type;
            transaction(const std::shared_ptr<session> &session, const std::shared_ptr<transaction_impl> &impl);
            transaction(const transaction &other);
            transaction(transaction &&other);
            virtual ~transaction();
            transaction &operator=(const transaction &other);
            transaction &operator=(transaction &&other);
            virtual void start();
            void save(const std::string &name);
            void commit();
            void release(const std::string &name);
            void rollback();
            void rollback(const std::string &name);
            bool is_active() const;

            operator std::shared_ptr<session>() const;

           private:
            std::shared_ptr<session> session_;
            std::shared_ptr<transaction_impl> impl_;
        };
    }
}

#endif
