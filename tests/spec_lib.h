
#ifndef SPEC_REG
#define SPEC_REG(name) extern coda::db::test::spec::type spec_file_##name
#else
#undef SPEC_REG
#define SPEC_REG(name) bandit::detail::specs().push_back(spec_file_##name)
#endif

SPEC_REG(columns);
SPEC_REG(deletes);
SPEC_REG(joins);
SPEC_REG(modifying);
SPEC_REG(records);
SPEC_REG(resultsets);
SPEC_REG(rows);
SPEC_REG(schemas);
SPEC_REG(schema_factories);
SPEC_REG(selects);
SPEC_REG(transactions);
SPEC_REG(updates);
