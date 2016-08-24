/*!
 * @file alloc.h
 * @abstract Functions for dealing with C-style memory allocation
 * @discussion These functions exist because we need to be able to use realloc in some cases
 */
#ifndef RJ_DB_ALLOC_H
#define RJ_DB_ALLOC_H

namespace rj
{
    namespace db
    {
        /*!
         * allocates a number instances of type T
         * @param num the number of instances to allocate (default 1)
         */
        template <typename T>
        T *c_alloc(size_t num = 1)
        {
            void *mem = nullptr;

            if (num == 0) {
                return nullptr;
            }
            mem = calloc(num, sizeof(T));
            if (mem == nullptr) {
                throw std::bad_alloc();
            }
            return static_cast<T *>(mem);
        }

        /*!
         * allocates a number of void pointers of a size
         * @param size the size of the memory block
         * @param num the number of instances to allocated (default 1)
         */
        void *c_alloc(size_t size, size_t num = 1);

        /*!
         * reallocate a value to a new size
         * @param value the existing memory block
         * @param size the number of type T items to allocate
         */
        template <typename T>
        T *c_alloc(T *value, size_t num, size_t oldSize)
        {
            void *mem = nullptr;
            T *val = nullptr;

            if (num == 0) {
                return value;
            }
            mem = realloc(value, sizeof(T) * num + 1);

            if (mem == nullptr) {
                throw std::bad_alloc();
            }

            val = static_cast<T *>(mem);

            // make sure new values are initialized
            // will be ignored if oldSize is greater than the new size
            for (size_t i = oldSize; i < num; i++) {
                memset(&val[i], 0, sizeof(T));
            }

            return val;
        }
    }
}

#endif
