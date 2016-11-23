
#undef SPEC_REG
#ifdef SPEC_EXTERN
#define SPEC_REG(name) extern rj::db::test::spec_type name##_test_specs
#else
#define SPEC_REG(name) bandit::detail::specs().push_back(name##_test_specs)
#endif

SPEC_REG(column);
SPEC_REG(delete_query);
SPEC_REG(join);
SPEC_REG(modify);
SPEC_REG(record);
SPEC_REG(resultset);
SPEC_REG(row);
SPEC_REG(schema);
SPEC_REG(schema_factory);
SPEC_REG(select_query);
SPEC_REG(transaction);
SPEC_REG(update_query);
