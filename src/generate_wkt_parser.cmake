message("Generating ${OUT_FILE}")

execute_process(COMMAND "bison" "-v" "--no-lines" "-d" "-p" "${PREFIX}" "-o${OUT_FILE}" "${IN_FILE}"
                RESULT_VARIABLE STATUS)

if(STATUS AND NOT STATUS EQUAL 0)
  message(FATAL_ERROR "bison failed")
endif()

# Post processing of the generated file
# All those replacements are to please MSVC
file(READ ${OUT_FILE} CONTENTS)
string(REPLACE "yyerrorlab:" "#if 0\nyyerrorlab:" CONTENTS "${CONTENTS}")
string(REPLACE "yyerrlab1:" "#endif\nyyerrlab1:" CONTENTS "${CONTENTS}")
string(REPLACE "for (yylen = 0; yystr[yylen]; yylen++)" "for (yylen = 0; yystr && yystr[yylen]; yylen++)" CONTENTS "${CONTENTS}")
string(REPLACE "return yystpcpy (yyres, yystr) - yyres;" "return (YYPTRDIFF_T)(yystpcpy (yyres, yystr) - yyres);" CONTENTS "${CONTENTS}")
string(REPLACE "YYPTRDIFF_T yysize = yyssp - yyss + 1;" "YYPTRDIFF_T yysize = (YYPTRDIFF_T)(yyssp - yyss + 1);" CONTENTS "${CONTENTS}")
string(REPLACE "#define yynerrs         pj_wkt1_nerrs" "/* #define yynerrs         pj_wkt1_nerrs */" CONTENTS "${CONTENTS}")
string(REPLACE "int yynerrs;" "/* int yynerrs; */" CONTENTS "${CONTENTS}")
string(REPLACE "yynerrs = 0;" "/* yynerrs = 0; */" CONTENTS "${CONTENTS}")
string(REPLACE "++yynerrs;" "/* ++yynerrs; */" CONTENTS "${CONTENTS}")
file(WRITE "${OUT_FILE}" "${CONTENTS}")
