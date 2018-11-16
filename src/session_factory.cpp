#include "session_factory.h"
#include "uri.h"

namespace coda::db {
  
  std::shared_ptr<coda::db::session_impl> session_factory::create(const std::string &uri_s) { 
    return create(uri(uri_s)); 
  }
  
}