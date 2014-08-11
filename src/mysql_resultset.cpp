#include "config.h"


#ifdef HAVE_LIBMYSQLCLIENT

#include "mysql_resultset.h"
#include "mysql_db.h"
#include "mysql_row.h"
#include "exception.h"
#include "mysql_binding.h"

namespace arg3
{
    namespace db
    {
        extern string last_stmt_error(MYSQL_STMT *stmt);

        void mysql_res_delete::operator()(MYSQL_RES *p) const
        {
            mysql_free_result(p);
        }


        mysql_resultset::mysql_resultset(mysql_db *db, shared_ptr<MYSQL_RES> res) : res_(res), row_(NULL), db_(db)
        {
        }

        mysql_resultset::mysql_resultset(mysql_resultset &&other) : res_(other.res_), row_(other.row_), db_(other.db_)
        {
            other.db_ = NULL;
            other.res_ = nullptr;
            other.row_ = NULL;
        }

        mysql_resultset::~mysql_resultset()
        {
            res_ = nullptr;
        }

        mysql_resultset &mysql_resultset::operator=(mysql_resultset && other)
        {
            res_ = other.res_;
            db_ = other.db_;
            row_ = other.row_;
            other.db_ = NULL;
            other.res_ = nullptr;
            other.row_ = NULL;

            return *this;
        }

        bool mysql_resultset::is_valid() const
        {
            return res_ != nullptr;
        }

        bool mysql_resultset::next()
        {
            assert(db_ != NULL);

            if (!is_valid() || !db_->is_open())
                return false;

            bool value = (row_ = mysql_fetch_row(res_.get())) != NULL;

            if (!value && !mysql_more_results(db_->db_))
            {
                MYSQL_RES *temp;
                if ((temp = mysql_use_result(db_->db_)) != NULL)
                {
                    res_ = shared_ptr<MYSQL_RES>(temp, mysql_res_delete());
                    value = (row_ = mysql_fetch_row(temp)) != NULL;
                }
                else
                {
                    res_ = nullptr;
                }
            }

            return value;
        }

        void mysql_resultset::reset()
        {
            mysql_data_seek(res_.get(), 0);
        }

        row mysql_resultset::current_row()
        {
            if (db_->cache_level() == sqldb::CACHE_ROW)
                return row(make_shared<mysql_cached_row>(db_, res_, row_));
            else
                return row(make_shared<mysql_row>(db_, res_, row_));
        }

        size_t mysql_resultset::size() const
        {
            return mysql_num_fields(res_.get());
        }


        /* Statement version */

        mysql_stmt_resultset::mysql_stmt_resultset(mysql_db *db, shared_ptr<MYSQL_STMT> stmt) : stmt_(stmt), metadata_(NULL), db_(db),
            bindings_(nullptr), status_(-1)
        {
            assert(stmt_ != nullptr);
            assert(db_ != NULL);
        }

        mysql_stmt_resultset::mysql_stmt_resultset(mysql_stmt_resultset &&other) : stmt_(other.stmt_), metadata_(other.metadata_),
            db_(other.db_),
            bindings_(other.bindings_), status_(other.status_)
        {
            other.db_ = NULL;
            other.stmt_ = nullptr;
            other.bindings_ = nullptr;
            other.metadata_ = NULL;
        }

        mysql_stmt_resultset::~mysql_stmt_resultset()
        {
            metadata_ = nullptr;
        }

        mysql_stmt_resultset &mysql_stmt_resultset::operator=(mysql_stmt_resultset && other)
        {
            stmt_ = other.stmt_;
            db_ = other.db_;
            metadata_ = other.metadata_;
            bindings_ = other.bindings_;
            status_ = other.status_;
            other.db_ = NULL;
            other.bindings_ = NULL;
            other.metadata_ = NULL;

            return *this;
        }

        void mysql_stmt_resultset::prepare_results()
        {
            if (stmt_ == nullptr || !stmt_ || bindings_ != NULL || status_ != -1)
                return;

            if ((status_ = mysql_stmt_execute(stmt_.get())))
            {
                throw database_exception(last_stmt_error(stmt_.get()));
            }

            // get information about the results
            MYSQL_RES *temp = mysql_stmt_result_metadata(stmt_.get());

            if (temp == NULL)
                throw database_exception("No result data found.");

            metadata_ = shared_ptr<MYSQL_RES>(temp, mysql_res_delete());

            int size = mysql_num_fields(temp);

            auto fields = mysql_fetch_fields(temp);

            bindings_ = make_shared<mysql_binding>(fields, size);

            bindings_->bind_result(stmt_.get());
        }

        bool mysql_stmt_resultset::is_valid() const
        {
            return stmt_ != nullptr;
        }

        bool mysql_stmt_resultset::next()
        {
            if (!is_valid())
                return false;

            if (status_ == -1)
            {
                prepare_results();

                if (status_ == -1)
                    return false;
            }

            int res = mysql_stmt_fetch(stmt_.get());

            if (res == 1 || res == MYSQL_DATA_TRUNCATED)
                throw database_exception(last_stmt_error(stmt_.get()));

            if (res == MYSQL_NO_DATA)
            {
                return false;
            }

            return true;

        }

        void mysql_stmt_resultset::reset()
        {
            assert(is_valid());

            if (mysql_stmt_reset(stmt_.get()))
                throw database_exception(last_stmt_error(stmt_.get()));

            status_ = -1;
        }

        row mysql_stmt_resultset::current_row()
        {
            assert(db_ != NULL);

            if (db_->cache_level() == sqldb::CACHE_ROW)
                return row(make_shared<mysql_cached_row>(db_, metadata_, *bindings_.get()));
            else
                return row(make_shared<mysql_stmt_row>(db_, metadata_, bindings_ ));
        }

        size_t mysql_stmt_resultset::size() const
        {
            if (!is_valid()) return 0;

            return mysql_stmt_field_count(stmt_.get());
        }


        /* cached version */

        mysql_cached_resultset::mysql_cached_resultset(sqldb *db, shared_ptr<MYSQL_STMT> stmt) : currentRow_(-1)
        {
            assert(is_valid());

            if (mysql_stmt_execute(stmt.get()))
            {
                throw database_exception(last_stmt_error(stmt.get()));
            }

            // get information about the results
            MYSQL_RES *temp = mysql_stmt_result_metadata(stmt.get());

            if (temp == NULL)
                throw database_exception("No result data found.");

            auto metadata = shared_ptr<MYSQL_RES>(temp, mysql_res_delete());

            int size = mysql_num_fields(temp);

            auto fields = mysql_fetch_fields(temp);

            mysql_binding bindings(fields, size);

            bindings.bind_result(stmt.get());

            int res = mysql_stmt_fetch(stmt.get());

            while (res != 1 && res != MYSQL_DATA_TRUNCATED)
            {
                if (res == MYSQL_NO_DATA)
                    break;

                rows_.push_back(make_shared<mysql_cached_row>(db, metadata, bindings));

                res = mysql_stmt_fetch(stmt.get());
            }
        }

        mysql_cached_resultset::mysql_cached_resultset(mysql_db *db, shared_ptr<MYSQL_RES> res) : currentRow_(0)
        {
            MYSQL_ROW row = mysql_fetch_row(res.get());

            if (row != NULL)
            {
                rows_.push_back(make_shared<mysql_cached_row>(db, res, row));

                while (mysql_more_results(db->db_) && (row = mysql_fetch_row(res.get())) != NULL)
                {
                    rows_.push_back(make_shared<mysql_cached_row>(db, res, row));
                }
            }
            else
            {
                MYSQL_RES *temp;

                while ((temp = mysql_use_result(db->db_)) != NULL && (row = mysql_fetch_row(temp)))
                {
                    rows_.push_back(make_shared<mysql_cached_row>(db, shared_ptr<MYSQL_RES>(temp, mysql_res_delete()), row));
                }
            }
        }
        mysql_cached_resultset::mysql_cached_resultset(mysql_cached_resultset &&other) : rows_(std::move(other.rows_))
        {
            other.rows_.clear();
        }

        mysql_cached_resultset::~mysql_cached_resultset()
        {
        }

        mysql_cached_resultset &mysql_cached_resultset::operator=(mysql_cached_resultset && other)
        {
            rows_ = std::move(other.rows_);

            return *this;
        }

        bool mysql_cached_resultset::is_valid() const
        {
            return true;
        }

        bool mysql_cached_resultset::next()
        {
            if (rows_.empty())
            {
                return false;
            }

            return ++currentRow_ < rows_.size();
        }

        void mysql_cached_resultset::reset()
        {
            currentRow_ = -1;
        }

        row mysql_cached_resultset::current_row()
        {
            assert (currentRow_ >= 0 && currentRow_ < rows_.size());

            return row(rows_[currentRow_]);
        }

        size_t mysql_cached_resultset::size() const
        {
            return rows_.size();
        }
    }
}

#endif
