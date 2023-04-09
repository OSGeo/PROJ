/*******************************************************************************
    Example code that illustrates how to convert between two CRS

    Note: This file is in-lined in the documentation. Any changes must be
    reflected in docs/source/development/quickstart.rst

*******************************************************************************/

#include <cassert>
#include <cmath>   // for HUGE_VAL
#include <iomanip> // for std::setprecision()

#include <iostream>

#include "proj/coordinateoperation.hpp"
#include "proj/crs.hpp"
#include "proj/io.hpp"
#include "proj/util.hpp" // for nn_dynamic_pointer_cast

using namespace NS_PROJ::crs;
using namespace NS_PROJ::io;
using namespace NS_PROJ::operation;
using namespace NS_PROJ::util;

int main(void) {
    auto dbContext = DatabaseContext::create();

    // Instantiate a generic authority factory, that is not tied to a particular
    // authority, to be able to get transformations registered by different
    // authorities. This can only be used for CoordinateOperationContext.
    auto authFactory = AuthorityFactory::create(dbContext, std::string());

    // Create a coordinate operation context, that can be customized to ammend
    // the way coordinate operations are computed. Here we ask for default
    // settings.
    auto coord_op_ctxt =
        CoordinateOperationContext::create(authFactory, nullptr, 0.0);

    // Instantiate a authority factory for EPSG related objects.
    auto authFactoryEPSG = AuthorityFactory::create(dbContext, "EPSG");

    // Instantiate source CRS from EPSG code
    auto sourceCRS = authFactoryEPSG->createCoordinateReferenceSystem("4326");

    // Instantiate target CRS from PROJ.4 string (commented out, the equivalent
    // from the EPSG code)
    // auto targetCRS =
    // authFactoryEPSG->createCoordinateReferenceSystem("32631");
    auto targetCRS =
        NN_CHECK_THROW(nn_dynamic_pointer_cast<CRS>(createFromUserInput(
            "+proj=utm +zone=31 +datum=WGS84 +type=crs", dbContext)));

    // List operations available to transform from EPSG:4326
    // (WGS 84 latitude/longitude) to EPSG:32631 (WGS 84 / UTM zone 31N).
    auto list = CoordinateOperationFactory::create()->createOperations(
        sourceCRS, targetCRS, coord_op_ctxt);

    // Check that we got a non-empty list of operations
    // The list is sorted from the most relevant to the less relevant one.
    // Cf
    // https://proj.org/operations/operations_computation.html#filtering-and-sorting-of-coordinate-operations
    // for more details on the sorting of those operations.
    // For a transformation between a projected CRS and its base CRS, like
    // we do here, there will be only one operation.
    assert(!list.empty());

    // Create an execution context (must only be used by one thread at a time)
    PJ_CONTEXT *ctx = proj_context_create();

    // Create a coordinate transformer from the first operation of the list
    auto transformer = list[0]->coordinateTransformer(ctx);

    // Perform the coordinate transformation.
    PJ_COORD c = {{
        49.0,    // latitude in degree
        2.0,     // longitude in degree
        0.0,     // z ordinate. unused
        HUGE_VAL // time ordinate. unused
    }};
    c = transformer->transform(c);

    // Display result
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Easting: " << c.v[0] << std::endl;  // should be 426857.988
    std::cout << "Northing: " << c.v[1] << std::endl; // should be 5427937.523

    // Destroy execution context
    proj_context_destroy(ctx);

    return 0;
}
