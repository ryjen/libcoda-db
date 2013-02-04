#include "base_record.h"
#include "modify_query.h"

namespace arg3
{
	namespace db
	{

        base_record::base_record() {}

        base_record::base_record(const row &values) : mRow(values) {}

        void base_record::save() const
        {
            modify_query query(*this);

            query.execute();
        }

        variant base_record::get(const string &name) {
        	return mRow[name];
        }

        void base_record::set(const string &name, const variant &value) {
        	if(!name.empty())
	        	mRow[name] = value;
        }

        void base_record::unset(const string &name) {
        	mRow.erase(name);
        }

	}
}