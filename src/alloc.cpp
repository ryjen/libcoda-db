#include <stdlib.h>
#include <new>

namespace rj
{
    namespace db
    {
        void *c_alloc(size_t size, size_t num = 1)
        {
            if (size == 0) {
                return nullptr;
            }
            void *mem = calloc(num, size);

            if (mem == nullptr) {
                throw std::bad_alloc();
            }
            return mem;
        }
    }
}