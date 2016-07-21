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
            virtual std::shared_ptr<rj::db::session_impl> create(const uri &uri) = 0;
            inline std::shared_ptr<rj::db::session_impl> create(const std::string &uri_s)
            {
                return create(uri(uri_s));
            }
        };
    }
}

#endif
