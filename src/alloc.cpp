#include "alloc.h"
#include <cstring>

namespace coda {
  namespace db {
    void *c_alloc(size_t size, size_t num) {
      void *mem = nullptr;

      if (size == 0 || num == 0) {
        return nullptr;
      }

      mem = calloc(num, size);

      if (mem == nullptr) {
        throw std::bad_alloc();
      }
      return mem;
    }

    void *c_copy(const void *data, size_t sz) {
      if (sz == 0 || data == nullptr) {
        return nullptr;
      }

      void *mem = c_alloc(sz);

      std::memmove(mem, data, sz);
      return mem;
    }
  }  // namespace db
}  // namespace coda
