#ifndef RJ_DB_SESSION_FACTORY_H
#define RJ_DB_SESSION_FACTORY_H

namespace rj
{
    namespace db
    {
        class session_impl;

        class session_factory
        {
           public:
            session_factory() = default;
            session_factory(const session_factory &other) = default;
            session_factory(session_factory &&other) = default;
            virtual ~session_factory() = default;
            session_factory &operator=(const session_factory &other) = default;
            session_factory &operator=(session_factory &&other) = default;

            virtual std::shared_ptr<rj::db::session_impl> create(const uri &uri) = 0;

            inline std::shared_ptr<rj::db::session_impl> create(const std::string &uri_s)
            {
                return create(uri(uri_s));
            }
        };
    }
}

#endif
