#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include "../log.h"
#include "binding.h"
#include "resultset.h"
#include "row.h"
#include "session.h"

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


            resultset::resultset(const std::shared_ptr<mysql::session> &sess, const shared_ptr<MYSQL_RES> &res)
                : res_(res), row_(nullptr), sess_(sess)
            {
                if (sess_ == nullptr) {
                    throw database_exception("database not provided to mysql resultset");
                }
            }

            resultset::resultset(resultset &&other) : res_(std::move(other.res_)), row_(other.row_), sess_(std::move(other.sess_))
            {
                other.sess_ = nullptr;
                other.res_ = nullptr;
                other.row_ = nullptr;
            }

            resultset::~resultset()
            {
            }

            resultset &resultset::operator=(resultset &&other)
            {
                res_ = std::move(other.res_);
                sess_ = std::move(other.sess_);
                row_ = other.row_;
                other.sess_ = nullptr;
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
                if (sess_ == nullptr) {
                    log::warn("mysql resultset next: database not open");
                    return false;
                }

                if (!is_valid() || !sess_->is_open()) {
                    return false;
                }

                row_ = mysql_fetch_row(res_.get());

                return row_ != nullptr;
            }

            void resultset::reset()
            {
                if (res_ != nullptr) {
                    mysql_data_seek(res_.get(), 0);
                }
            }

            resultset::row_type resultset::current_row()
            {
                return row_type(make_shared<mysql::row>(sess_, res_, row_));
            }
            /* Statement version */

            stmt_resultset::stmt_resultset(const std::shared_ptr<mysql::session> &sess, const shared_ptr<MYSQL_STMT> &stmt)
                : stmt_(stmt), metadata_(nullptr), sess_(sess), bindings_(nullptr), status_(-1)
            {
                if (stmt_ == nullptr) {
                    throw database_exception("invalid statement provided to mysql statement resultset");
                }
                if (sess_ == nullptr) {
                    throw database_exception("invalid database provided to mysql statement resultset");
                }
            }

            stmt_resultset::stmt_resultset(stmt_resultset &&other)
                : stmt_(std::move(other.stmt_)),
                  metadata_(std::move(other.metadata_)),
                  sess_(std::move(other.sess_)),
                  bindings_(std::move(other.bindings_)),
                  status_(other.status_)
            {
                other.sess_ = nullptr;
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
                sess_ = std::move(other.sess_);
                metadata_ = std::move(other.metadata_);
                bindings_ = std::move(other.bindings_);
                status_ = other.status_;
                other.sess_ = nullptr;
                other.bindings_ = nullptr;
                other.metadata_ = nullptr;

                return *this;
            }

            void stmt_resultset::prepare_results()
            {
                if (stmt_ == nullptr || !stmt_ || status_ != INVALID) {
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

                if (status_ == INVALID) {
                    prepare_results();

                    if (status_ == INVALID) {
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
                bindings_.reset();

                if (!is_valid()) {
                    log::warn("mysql stmt resultset reset invalid");
                    return;
                }

                status_ = INVALID;

                if (mysql_stmt_reset(stmt_.get())) {
                    throw database_exception(helper::last_stmt_error(stmt_.get()));
                }
            }

            resultset::row_type stmt_resultset::current_row()
            {
                return row_type(make_shared<stmt_row>(sess_, stmt_, metadata_, bindings_));
            }
        }
    }
}

#endif
