set(GIE_BIN "gie")

# Regression tests
proj_add_gie_test("Builtins" "gie/builtins.gie")
proj_add_gie_test("Builtins2" "gie/more_builtins.gie")
proj_add_gie_test("Axisswap" "gie/axisswap.gie")
proj_add_gie_test("Deformation" "gie/deformation.gie")
proj_add_gie_test("Ellipsoid" "gie/ellipsoid.gie")
proj_add_gie_test("GDA" "gie/GDA.gie")
