set(GIE_BIN "gie")

# Regression tests
proj_add_gie_test("Builtins" "gie/builtins.gie")
proj_add_gie_test("Builtins2" "gie/more_builtins.gie")
proj_add_gie_test("Axisswap" "gie/axisswap.gie")
proj_add_gie_test("Deformation" "gie/deformation.gie")
proj_add_gie_test("Ellipsoid" "gie/ellipsoid.gie")
proj_add_gie_test("GDA" "gie/GDA.gie")
proj_add_gie_test("4D-API-cs2cs-style" "gie/4D-API_cs2cs-style.gie")
proj_add_gie_test("DHDN_ETRS89" "gie/DHDN_ETRS89.gie")

# GIGS tests. Uncommented tests are expected to fail due to issues with
# various projections. Should be investigated further and fixed.
proj_add_gie_test("GIGS-5101.1-jhs" "gigs/5101.1-jhs.gie")
proj_add_gie_test("GIGS-5101.2-jhs" "gigs/5101.2-jhs.gie")
proj_add_gie_test("GIGS-5101.3-jhs" "gigs/5101.3-jhs.gie")
proj_add_gie_test("GIGS-5101.4-jhs-etmerc" "gigs/5101.4-jhs-etmerc.gie")
#proj_add_gie_test("GIGS-5101.4-jhs" "gigs/5101.4-jhs.gie") # Same as above, but using etmerc instead of tmerc
proj_add_gie_test("GIGS-5102.1" "gigs/5102.1.gie")
#proj_add_gie_test("GIGS-5102.2" "gigs/5102.2.gie")
proj_add_gie_test("GIGS-5103.1" "gigs/5103.1.gie")
proj_add_gie_test("GIGS-5103.2" "gigs/5103.2.gie")
proj_add_gie_test("GIGS-5103.3" "gigs/5103.3.gie")
#proj_add_gie_test("GIGS-5104" "gigs/5104.gie")
#proj_add_gie_test("GIGS-5105.1" "gigs/5105.1.gie")
proj_add_gie_test("GIGS-5105.2" "gigs/5105.2.gie")
proj_add_gie_test("GIGS-5106" "gigs/5106.gie")
proj_add_gie_test("GIGS-5107" "gigs/5107.gie")
#proj_add_gie_test("GIGS-5108" "gigs/5108.gie")
proj_add_gie_test("GIGS-5109" "gigs/5109.gie")
#proj_add_gie_test("GIGS-5110" "gigs/5110.gie")
proj_add_gie_test("GIGS-5111.1" "gigs/5111.1.gie")
#proj_add_gie_test("GIGS-5111.2" "gigs/5111.2.gie")
proj_add_gie_test("GIGS-5112" "gigs/5112.gie")
proj_add_gie_test("GIGS-5113" "gigs/5113.gie")
proj_add_gie_test("GIGS-5201" "gigs/5201.gie")
#proj_add_gie_test("GIGS-5203" "gigs/5203.1.gie")
#proj_add_gie_test("GIGS-5204.1" "gigs/5204.1.gie")
#proj_add_gie_test("GIGS-5205.1" "gigs/5205.1.gie")
#proj_add_gie_test("GIGS-5206" "gigs/5206.gie")
#proj_add_gie_test("GIGS-5207.1" "gigs/5207.1.gie")
#proj_add_gie_test("GIGS-5207.2" "gigs/5207.2.gie")
proj_add_gie_test("GIGS-5208" "gigs/5208.gie")



