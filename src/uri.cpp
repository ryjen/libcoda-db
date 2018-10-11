
#include "uri.h"
#include <algorithm>
#include <sstream>

using namespace std;

namespace coda::db {
  uri_type::uri_type(const std::string &url) { parse(url); }

  uri_type::operator std::string() const { return value; }

  void uri_type::parse(const string &url_s) {
    value = url_s;
    // do the manual implementation from stack overflow
    // with some mods for the port
    const string prot_end("://");
    string::const_iterator pos_i = search(url_s.begin(), url_s.end(), prot_end.begin(), prot_end.end());
    protocol.reserve(static_cast<unsigned long>(distance(url_s.begin(), pos_i)));
    transform(url_s.begin(), pos_i, back_inserter(protocol),
              [](int c) { return std::tolower(c); });  // protocol is icase
    if (pos_i == url_s.end()) {
      return;
    }

    advance(pos_i, prot_end.length());

    string::const_iterator user_i = find(pos_i, url_s.end(), '@');
    string::const_iterator path_i;

    if (user_i != url_s.end()) {
      string::const_iterator pwd_i = find(pos_i, user_i, ':');

      if (pwd_i != user_i) {
        password.assign(pwd_i + 1, user_i);
        user.assign(pos_i, pwd_i);
      } else {
        user.assign(pos_i, user_i);
      }

      pos_i = user_i + 1;
    }

    path_i = find(pos_i, url_s.end(), '/');
    if (path_i == url_s.end()) {
      path_i = pos_i;
    }

    string::const_iterator port_i = find(pos_i, path_i, ':');
    string::const_iterator host_end;
    if (port_i != url_s.end()) {
      port.assign(*port_i == ':' ? (port_i + 1) : port_i, path_i);
      host_end = port_i;
    } else {
      host_end = path_i;
    }
    host.reserve(static_cast<unsigned long>(distance(pos_i, host_end)));
    transform(pos_i, host_end, back_inserter(host), [](int c) { return std::tolower(c); });  // host is icase
    string::const_iterator query_i = find(path_i, url_s.end(), '?');
    path.assign(*path_i == '/' ? (path_i + 1) : path_i, query_i);
    if (query_i != url_s.end()) ++query_i;
    query.assign(query_i, url_s.end());
  }
}  // namespace coda::db