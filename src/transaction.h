#ifndef ARG3_DB_TRANSACTION_H
#define ARG3_DB_TRANSACTION_H

#include <memory>
#include <string>

namespace arg3
{
    namespace db
    {
        class session;

        class transaction_impl
        {
           public:
            virtual void start() = 0;
            virtual bool is_active() const = 0;
        };

        namespace isolation
        {
            typedef enum { none, serializable, repeatable_read, read_commited, read_uncommited } level;
        }

        class transaction
        {
            friend class session;

           public:
            typedef session session_type;
            typedef enum { none, read_write, read_only } type;
            transaction(const std::shared_ptr<session_type> &session, const std::shared_ptr<transaction_impl> &impl);
            transaction(const transaction &other);
            transaction(transaction &&other);
            virtual ~transaction();
            transaction &operator=(const transaction &other);
            transaction &operator=(transaction &&other);
            void save(const std::string &name);
            void release(const std::string &name);
            void rollback(const std::string &name);
            void start();
            void commit();
            void rollback();
            bool is_active() const;
            void set_successful(bool value);
            bool is_successful() const;
            std::shared_ptr<transaction_impl> impl() const;

            std::shared_ptr<session_type> get_session() const;

           private:
            bool successful_;
            std::shared_ptr<session_type> session_;
            std::shared_ptr<transaction_impl> impl_;
        };
    }
}

#endif
