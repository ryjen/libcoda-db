#include <stdlib.h>
#include <new>

namespace arg3
{
    namespace db
    {
        void *c_alloc(size_t size, size_t num = 1)
        {
            void *mem = calloc(num, size);

            if (mem == NULL) {
                throw std::bad_alloc();
            }
            return mem;
        }
    }
}