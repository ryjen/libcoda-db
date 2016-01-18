#ifndef ARG3_DB_ALLOC_H
#define ARG3_DB_ALLOC_H

namespace arg3
{
    namespace db
    {
        template <typename T>
        T *c_alloc(size_t num = 1)
        {
            void *mem = calloc(num, sizeof(T));
            if (mem == NULL) {
                throw std::bad_alloc();
            }
            return static_cast<T *>(mem);
        }

        void *c_alloc(size_t size, size_t num = 1);

        template <typename T>
        T *c_alloc(T *value, size_t size, size_t oldSize)
        {
            void *mem = realloc(value, sizeof(T) * size);

            if (mem == nullptr) {
                throw std::bad_alloc();
            }

            T *val = static_cast<T *>(mem);

            // make sure new values are initialized
            for (size_t i = oldSize; i < size; i++) {
                memset(&val[i], 0, sizeof(T));
            }

            return val;
        }
    }
}

#endif
