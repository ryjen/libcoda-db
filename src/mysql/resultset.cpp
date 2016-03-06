#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include "resultset.h"
#include "db.h"
#include "row.h"
#include "binding.h"
#include "../log.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        namespace mysql
        {
            namespace helper
            {
                extern string last_stmt_error(MYSQL_STMT *stmt);

                void res_delete::operator()(MYSQL_RES *p) const
                {
                    if (p != nullptr) {
                        mysql_free_result(p);
                    }
                }
            }


            resultset::resultset(mysql::db *db, const shared_ptr<MYSQL_RES> &res) : res_(res), row_(nullptr), db_(db)
            {
                if (db_ == nullptr) {
                    throw database_exception("database not provided to mysql resultset");
                }
            }

            resultset::resultset(resultset &&other) : res_(std::move(other.res_)), row_(other.row_), db_(other.db_)
            {
                other.db_ = nullptr;
                other.res_ = nullptr;
                other.row_ = nullptr;
            }

            resultset::~resultset()
            {
            }

            resultset &resultset::operator=(resultset &&other)
            {
                res_ = std::move(other.res_);
                db_ = other.db_;
                row_ = other.row_;
                other.db_ = nullptr;
                other.res_ = nullptr;
                other.row_ = nullptr;

                return *this;
            }

            bool resultset::is_valid() const
            {
                return res_ != nullptr;
            }

            bool resultset::next()
            {
                if (db_ == nullptr) {
                    log::warn("mysql resultset next: database not open");
                    return false;
                }

                if (!is_valid() || !db_->is_open()) {
                    return false;
                }

                row_ = mysql_fetch_row(res_.get());

                bool value = row_ != nullptr;

                if (!value && !mysql_more_results(db_->db_.get())) {
                    MYSQL_RES *temp = mysql_use_result(db_->db_.get());
                    if (temp != nullptr) {
                        res_ = shared_ptr<MYSQL_RES>(temp, helper::res_delete());
                        row_ = mysql_fetch_row(temp);
                        value = row_ != nullptr;
                    }
                }

                return value;
            }

            void resultset::reset()
            {
                if (res_ != nullptr) {
                    mysql_data_seek(res_.get(), 0);
                }
            }

            resultset::row_type resultset::current_row()
            {
                return row_type(make_shared<mysql::row>(db_, res_, row_));
            }
            /* Statement version */

            stmt_resultset::stmt_resultset(mysql::db *db, const shared_ptr<MYSQL_STMT> &stmt)
                : stmt_(stmt), metadata_(nullptr), db_(db), bindings_(nullptr), status_(-1)
            {
                if (stmt_ == nullptr) {
                    throw database_exception("invalid statement provided to mysql statement resultset");
                }
                if (db_ == nullptr) {
                    throw database_exception("invalid database provided to mysql statement resultset");
                }
            }

            stmt_resultset::stmt_resultset(stmt_resultset &&other)
                : stmt_(std::move(other.stmt_)),
                  metadata_(std::move(other.metadata_)),
                  db_(other.db_),
                  bindings_(std::move(other.bindings_)),
                  status_(other.status_)
            {
                other.db_ = nullptr;
                other.stmt_ = nullptr;
                other.bindings_ = nullptr;
                other.metadata_ = nullptr;
            }

            stmt_resultset::~stmt_resultset()
            {
            }

            stmt_resultset &stmt_resultset::operator=(stmt_resultset &&other)
            {
                stmt_ = std::move(other.stmt_);
                db_ = other.db_;
                metadata_ = std::move(other.metadata_);
                bindings_ = std::move(other.bindings_);
                status_ = other.status_;
                other.db_ = nullptr;
                other.bindings_ = nullptr;
                other.metadata_ = nullptr;

                return *this;
            }

            void stmt_resultset::prepare_results()
            {
                if (stmt_ == nullptr || !stmt_ || status_ != -1) {
                    return;
                }

                if ((status_ = mysql_stmt_execute(stmt_.get()))) {
                    throw database_exception(helper::last_stmt_error(stmt_.get()));
                }

                // get information about the results
                MYSQL_RES *temp = mysql_stmt_result_metadata(stmt_.get());

                if (temp == nullptr) {
                    throw database_exception("No result data found.");
                }

                metadata_ = shared_ptr<MYSQL_RES>(temp, helper::res_delete());

                int size = mysql_num_fields(temp);

                auto fields = mysql_fetch_fields(temp);

                bindings_ = make_shared<mysql::binding>(fields, size);

                bindings_->bind_result(stmt_.get());

                if (mysql_stmt_store_result(stmt_.get())) {
                    throw database_exception(helper::last_stmt_error(stmt_.get()));
                }
            }

            bool stmt_resultset::is_valid() const
            {
                return stmt_ != nullptr && stmt_;
            }

            bool stmt_resultset::next()
            {
                if (!is_valid()) {
                    log::warn("mysql resultset next invalid");
                    return false;
                }

                if (status_ == -1) {
                    prepare_results();

                    if (status_ == -1) {
                        return false;
                    }
                }

                int res = mysql_stmt_fetch(stmt_.get());

                if (res == 1 || res == MYSQL_DATA_TRUNCATED) {
                    throw database_exception(helper::last_stmt_error(stmt_.get()));
                }

                if (res == MYSQL_NO_DATA) {
                    return false;
                }

                return true;
            }

            void stmt_resultset::reset()
            {
                if (!is_valid()) {
                    log::warn("mysql stmt resultset reset invalid");
                    return;
                }

                if (mysql_stmt_reset(stmt_.get())) {
                    throw database_exception(helper::last_stmt_error(stmt_.get()));
                }

                status_ = -1;
            }

            resultset::row_type stmt_resultset::current_row()
            {
                return row_type(make_shared<stmt_row>(db_, stmt_, metadata_, bindings_));
            }
        }
    }
}

#endif
