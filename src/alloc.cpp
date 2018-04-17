#include "alloc.h"

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
    }
}
