#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include "mysql_db.h"
#include "mysql_statement.h"
#include "mysql_resultset.h"

namespace arg3
{
    namespace db
    {

        mysql_db::mysql_db(const string &dbName, const string &user, const string &password, const string &host, int port) : sqldb(), db_(NULL),
            dbName_(dbName), user_(user), password_(password), host_(host), port_(port), schema_factory_(this)
        {
        }

        mysql_db::mysql_db(const mysql_db &other) : sqldb(other), db_(other.db_),
            dbName_(other.dbName_), user_(other.user_), password_(other.password_), host_(other.host_),
            port_(other.port_), schema_factory_(other.schema_factory_)
        {

        }

        mysql_db::mysql_db(mysql_db &&other) : sqldb(other), db_(other.db_),
            dbName_(std::move(other.dbName_)), user_(std::move(other.user_)), password_(std::move(other.password_)),
            host_(std::move(other.host_)), port_(other.port_), schema_factory_(std::move(other.schema_factory_))
        {
            other.db_ = NULL;
        }

        mysql_db &mysql_db::operator=(const mysql_db &other)
        {
            db_ = other.db_;
            dbName_ = other.dbName_;
            user_ = other.user_;
            password_ = other.password_;
            host_ = other.host_;
            port_ = other.port_;
            schema_factory_ = other.schema_factory_;

            return *this;
        }

        mysql_db &mysql_db::operator=(mysql_db && other)
        {
            db_ = other.db_;
            dbName_ = std::move(other.dbName_);
            user_ = std::move(other.user_);
            password_ = std::move(other.password_);
            host_ = std::move(other.host_);
            port_ = other.port_;
            schema_factory_ = std::move(other.schema_factory_);
            other.db_ = NULL;

            return *this;
        }

        mysql_db::~mysql_db()
        {
            close();
        }

        string mysql_db::connection_string() const
        {
            char buf[BUFSIZ + 1] = {0};
            snprintf(buf, BUFSIZ, "mysql://%s@%s:%d/%s", user_.c_str(), host_.c_str(), port_, dbName_.c_str());
            return buf;
        }

        schema_factory *mysql_db::schemas()
        {
            return &schema_factory_;
        }

        void mysql_db::set_connection_string(const string &value)
        {

        }

        void mysql_db::query_schema(const string &tableName, std::vector<column_definition> &columns)
        {
            auto rs = execute("show columns from " + tableName);

            for (auto & row : rs)
            {
                column_definition def;

                // column name
                def.name = row["Field"].to_string();

                // primary key check
                def.pk = row["Key"].to_string() == "PRI";

                // find type
                string type = row["Type"].to_string();

                if (type.find("int") != string::npos)
                {
                    def.type = MYSQL_TYPE_LONG;
                }
                else if (type.find("real") != string::npos)
                {
                    def.type = MYSQL_TYPE_FLOAT;
                }
                else if (type.find("blob") != string::npos)
                {
                    def.type = MYSQL_TYPE_BLOB;
                }
                else
                {
                    def.type = MYSQL_TYPE_STRING;
                }

                columns.push_back(def);
            }
        }

        void mysql_db::open()
        {
            if (db_ != NULL) return;

            db_ = mysql_init(NULL);

            if (mysql_real_connect(db_, host_.c_str(), user_.c_str(), password_.c_str(), dbName_.c_str(), port_, NULL, 0) == NULL)
            {
                mysql_close(db_);
                db_ = NULL;
                throw database_exception("No connection could be made to the database");
            }
        }

        bool mysql_db::is_open() const
        {
            return db_ != NULL;
        }

        void mysql_db::close()
        {
            if (db_ != NULL)
            {
                mysql_close(db_);
                db_ = NULL;
            }
        }

        string mysql_db::last_error() const
        {
            ostringstream buf;

            buf << mysql_errno(db_);
            buf << ": " << mysql_error(db_);

            return buf.str();
        }

        long long mysql_db::last_insert_id() const
        {
            return mysql_insert_id(db_);
        }

        int mysql_db::last_number_of_changes() const
        {
            return mysql_affected_rows(db_);
        }

        resultset mysql_db::execute(const string &sql)
        {
            MYSQL_RES *res;

            if (mysql_real_query(db_, sql.c_str(), sql.length()))
                throw database_exception(last_error());

            res = mysql_use_result(db_);

            resultset set(make_shared<mysql_resultset>(this, res));

            return set;
        }

        shared_ptr<statement> mysql_db::create_statement()
        {
            return make_shared<mysql_statement>(this);
        }
    }
}

#endif
