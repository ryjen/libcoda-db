#ifndef ARG3_DB_SESSION_FACTORY_H
#define ARG3_DB_SESSION_FACTORY_H

namespace arg3
{
    namespace db
    {
        class session_impl;

        class session_factory
        {
           public:
            virtual std::shared_ptr<arg3::db::session_impl> create(const uri &uri) = 0;
            inline std::shared_ptr<arg3::db::session_impl> create(const std::string &uri_s)
            {
                return create(uri(uri_s));
            }
        };
    }
}

#endif
