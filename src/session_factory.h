#ifndef ARG3_DB_SESSION_FACTORY_H
#define ARG3_DB_SESSION_FACTORY_H

namespace arg3
{
    namespace db
    {
        class session;

        class session_factory
        {
           public:
            virtual arg3::db::session *create(const uri &uri) = 0;
            inline arg3::db::session *create(const std::string &uri_s)
            {
                return create(uri(uri_s));
            }
        };
    }
}

#endif
