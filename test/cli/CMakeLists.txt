#
# test
#
set(CS2CS_BIN "cs2cs")
set(PROJ_BIN "proj")
set(PROJINFO_BIN "projinfo")
set(CCT_BIN "cct")
proj_add_test_script_sh("test27" PROJ_BIN)
proj_add_test_script_sh("test83" PROJ_BIN)
proj_add_test_script_sh("testvarious" CS2CS_BIN)
proj_add_test_script_sh("testdatumfile" CS2CS_BIN "connu")
proj_add_test_script_sh("testIGNF" CS2CS_BIN "ntf_r93.gsb")
proj_add_test_script_sh("testntv2" CS2CS_BIN "ntv2_0.gsb")
proj_add_test_script_sh("testprojinfo" PROJINFO_BIN)
proj_add_test_script_sh("testcct" CCT_BIN)
