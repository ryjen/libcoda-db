
#include "exception.h"

namespace coda::db {
    CODA_IMPLEMENT_EXCEPTION(database_exception, std::exception);

    CODA_IMPLEMENT_EXCEPTION(no_such_column_exception, database_exception);

    CODA_IMPLEMENT_EXCEPTION(record_not_found_exception, database_exception);

    CODA_IMPLEMENT_EXCEPTION(binding_error, database_exception);

    CODA_IMPLEMENT_EXCEPTION(transaction_exception, database_exception);

    CODA_IMPLEMENT_EXCEPTION(no_primary_key_exception, database_exception);

    CODA_IMPLEMENT_EXCEPTION(value_conversion_error, database_exception);
}  // namespace coda::db