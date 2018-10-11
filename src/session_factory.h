#ifndef CODA_DB_SESSION_FACTORY_H
#define CODA_DB_SESSION_FACTORY_H

#include <memory>

namespace coda::db {
  class session_impl;
  using uri = struct uri_type;

  class session_factory {
   public:
    session_factory() = default;
    session_factory(const session_factory &other) = default;
    session_factory(session_factory &&other) = default;
    ~session_factory() = default;
    session_factory &operator=(const session_factory &other) = default;
    session_factory &operator=(session_factory &&other) = default;

    virtual std::shared_ptr<coda::db::session_impl> create(const uri &uri) = 0;

    inline std::shared_ptr<coda::db::session_impl> create(const std::string &uri_s);
  };
}  // namespace coda::db

#endif
