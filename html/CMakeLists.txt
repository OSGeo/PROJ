set(HTML_DOC_FILE faq.html
                  gen_parms.html
                  index.html
                  proj_logo.png)
                  
install(FILES ${HTML_DOC_FILE} 
               DESTINATION ${DOCDIR}
               PERMISSIONS WORLD_READ WORLD_WRITE)

if( IS_DIRECTORY "${PROJECT_SOURCE_DIR}/html/man1/" )
    install(FILES man1/proj.1.html man1/cs2cs.1.html man1/geod.1.html
        DESTINATION ${DOCDIR}/man1 PERMISSIONS WORLD_READ WORLD_WRITE)
endif ( IS_DIRECTORY "${PROJECT_SOURCE_DIR}/html/man1/" )

if( IS_DIRECTORY "${PROJECT_SOURCE_DIR}/html/man3/" )
    install(FILES man3/pj_init.3.html man3/geodesic.3.html
        DESTINATION ${DOCDIR}/man3 PERMISSIONS WORLD_READ WORLD_WRITE)
endif( IS_DIRECTORY "${PROJECT_SOURCE_DIR}/html/man3/" )
