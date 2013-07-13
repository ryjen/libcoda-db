#ifndef ARG3_DB_TYPEDEF_H_
#define ARG3_DB_TYPEDEF_H_

#include <boost/variant.hpp>

namespace arg3
{
	namespace db
	{
        class sql_blob
        {
        private:
            void *p_;
            size_t s_;
            void (*destruct_)(void*);
        public:
            sql_blob(void *ptr, size_t size, void (*destruct)(void*) = NULL) : p_(ptr), s_(size), destruct_(destruct) 
            {}
            ~sql_blob() {
            	if(destruct_ != NULL) {
            		destruct_(p_);
            		p_ = NULL;
            	}
            }

            void *ptr() const {
            	return p_;
            }
            size_t size() const {
            	return s_;
            }
        };

        typedef boost::variant<long, int64_t, std::string, sql_blob> sql_value;

	}
}

#endif
