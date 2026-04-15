# Single switch for the full proj.db consistency pipeline
# Consistency check is OFF unless data/sql/emlid_patch.sql applied.
set(PROJ_DB_ENABLE_FINAL_CONSISTENCY_CHECKS OFF)
set(PROJ_DB_EXTRA_VALIDATION ${PROJ_DB_ENABLE_FINAL_CONSISTENCY_CHECKS})
