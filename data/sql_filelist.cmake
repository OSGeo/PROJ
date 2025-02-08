set(SQL_DIR "${CMAKE_CURRENT_LIST_DIR}/sql")

set(SQL_FILES_CONSISTENCY_CHECKS_TRIGGERS
  "${SQL_DIR}/consistency_checks_triggers.sql"
  "${SQL_DIR}/conversion_triggers.sql"
  "${SQL_DIR}/conversion_triggers_hand_written.sql"
)

# When setting PROJ_DB_EXTRA_VALIDATION=OFF, we defer the insertion of check triggers
# until the very end to save build time. We also entirely skip running
# final_consistency_checks.sql.
# Typical build time with PROJ_DB_EXTRA_VALIDATION=ON: 60 seconds
# Typical build time with PROJ_DB_EXTRA_VALIDATION=OFF: 3.7 seconds

set(SQL_FILES
  "${SQL_DIR}/begin.sql"
  "${SQL_DIR}/proj_db_table_defs.sql"
)
if (PROJ_DB_EXTRA_VALIDATION)
  list(APPEND SQL_FILES ${SQL_FILES_CONSISTENCY_CHECKS_TRIGGERS})
endif()
list(APPEND SQL_FILES
  "${SQL_DIR}/customizations_early.sql"
  "${SQL_DIR}/metadata.sql"
  "${SQL_DIR}/unit_of_measure.sql"
  "${SQL_DIR}/extent.sql"
  "${SQL_DIR}/scope.sql"
  "${SQL_DIR}/coordinate_system.sql"
  "${SQL_DIR}/axis.sql"
  "${SQL_DIR}/ellipsoid.sql"
  "${SQL_DIR}/prime_meridian.sql"
  "${SQL_DIR}/geodetic_datum.sql"
  "${SQL_DIR}/geodetic_datum_ensemble_member.sql"
  "${SQL_DIR}/vertical_datum.sql"
  "${SQL_DIR}/vertical_datum_ensemble_member.sql"
  "${SQL_DIR}/engineering_datum.sql"
  "${SQL_DIR}/conversion.sql"
  "${SQL_DIR}/geodetic_crs.sql"
  "${SQL_DIR}/projected_crs.sql"
  "${SQL_DIR}/vertical_crs.sql"
  "${SQL_DIR}/compound_crs.sql"
  "${SQL_DIR}/engineering_crs.sql"
  "${SQL_DIR}/helmert_transformation.sql"
  "${SQL_DIR}/grid_transformation.sql"
  "${SQL_DIR}/grid_transformation_custom.sql"
  "${SQL_DIR}/other_transformation.sql"
  "${SQL_DIR}/other_transformation_custom.sql"
  "${SQL_DIR}/concatenated_operation.sql"
  "${SQL_DIR}/concatenated_operation_step.sql"
  "${SQL_DIR}/alias_name.sql"
  "${SQL_DIR}/supersession.sql"
  "${SQL_DIR}/deprecation.sql"
  "${SQL_DIR}/esri.sql"
  "${SQL_DIR}/ignf.sql"
  "${SQL_DIR}/nkg.sql"
  "${SQL_DIR}/iau.sql"
  "${SQL_DIR}/nrcan.sql"
  # Below file not yet integrated to unclear licensing of referenced grid file
  # "${SQL_DIR}/transformations_czechia_extra.sql"
  "${SQL_DIR}/grid_alternatives.sql"
  "${SQL_DIR}/grid_alternatives_generated_noaa.sql"
  "${SQL_DIR}/nadcon5_concatenated_operations.sql"
  "${SQL_DIR}/wgs84_realizations_concatenated_operations.sql"
  "${SQL_DIR}/customizations.sql"
  "${SQL_DIR}/nkg_post_customizations.sql"
  "${SQL_DIR}/commit.sql"
)
if (NOT PROJ_DB_EXTRA_VALIDATION)
  list(APPEND SQL_FILES ${SQL_FILES_CONSISTENCY_CHECKS_TRIGGERS})
else()
  list(APPEND SQL_FILES "${SQL_DIR}/final_consistency_checks.sql")
endif()
list(APPEND SQL_FILES "${SQL_DIR}/analyze_vacuum.sql")
