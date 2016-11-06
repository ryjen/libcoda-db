#include "row.h"
#include "column.h"
#include "session.h"

using namespace std;

namespace rj
{
    namespace db
    {
        namespace postgres
        {
            row::row(const std::shared_ptr<postgres::session> &sess, const shared_ptr<PGresult> &stmt, int row)
                : row_impl(), stmt_(stmt), sess_(sess), row_(row)
            {
                if (sess_ == NULL) {
                    throw database_exception("no database provided to postgres row");
                }

                if (stmt_ == nullptr) {
                    throw database_exception("no statement provided to postgres row");
                }

                size_ = PQnfields(stmt_.get());
            }

            row::row(row &&other)
                : row_impl(std::move(other)), stmt_(std::move(other.stmt_)), sess_(std::move(other.sess_)), size_(other.size_), row_(other.row_)
            {
                other.stmt_ = nullptr;
                other.sess_ = NULL;
            }

            row::~row()
            {
            }


            row &row::operator=(row &&other)
            {
                stmt_ = std::move(other.stmt_);
                sess_ = std::move(other.sess_);
                size_ = other.size_;
                row_ = other.row_;
                other.stmt_ = nullptr;
                other.sess_ = NULL;

                return *this;
            }

            row::column_type row::column(size_t nPosition) const
            {
                if (nPosition >= size() || row_ == -1) {
                    throw no_such_column_exception();
                }

                return column_type(make_shared<postgres::column>(stmt_, row_, nPosition));
            }

            row::column_type row::column(const string &name) const
            {
                if (name.empty()) {
                    throw no_such_column_exception();
                }

                for (size_t i = 0; i < size_; i++) {
                    const char *col_name = PQfname(stmt_.get(), i);

                    if (name == col_name) {
                        return column(i);
                    }
                }
                throw no_such_column_exception(name);
            }

            string row::column_name(size_t nPosition) const
            {
                if (nPosition >= size()) {
                    throw no_such_column_exception();
                }

                return PQfname(stmt_.get(), nPosition);
            }

            size_t row::size() const
            {
                return size_;
            }

            bool row::is_valid() const
            {
                return stmt_ != nullptr && row_ >= 0;
            }
        }
    }
}
