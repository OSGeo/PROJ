/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  projinfo utility
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2018, Even Rouault <even dot rouault at spatialys dot com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

//! @cond Doxygen_Suppress

#define FROM_PROJ_CPP

#include <cstdlib>
#include <iostream>

#include "projects.h"

#include <proj/coordinateoperation.hpp>
#include <proj/crs.hpp>
#include <proj/internal.hpp> // for split
#include <proj/io.hpp>
#include <proj/metadata.hpp>
#include <proj/util.hpp>

using namespace NS_PROJ::crs;
using namespace NS_PROJ::io;
using namespace NS_PROJ::metadata;
using namespace NS_PROJ::operation;
using namespace NS_PROJ::util;
using namespace NS_PROJ::internal;

// ---------------------------------------------------------------------------

struct OutputOptions {
    bool quiet = false;
    bool PROJ5 = false;
    bool PROJ4 = false;
    bool WKT2_2018 = false;
    bool WKT2_2015 = false;
    bool WKT1_GDAL = false;
};

// ---------------------------------------------------------------------------

static void usage() {
    std::cerr
        << "usage: projinfo [-o formats] [-k crs|operation] [--summary] [-q]"
        << std::endl
        << "                [--bbox min_long,min_lat,max_long,max_lat] "
        << std::endl
        << "                [--spatial-test contains|intersects]" << std::endl
        << "                [--crs-extent-use none|both|intersection|smallest]"
        << std::endl
        << "                {object_definition} | (-s {src_def} -t {src_def})"
        << std::endl;
    std::cerr << std::endl;
    std::cerr << "-o: formats is a comma separated combination of: "
                 "all,default,PROJ4,PROJ,WKT_ALL,WKT2_2015,WKT2_2018,WKT1_GDAL"
              << std::endl;
    std::cerr << "    Except 'all' and 'default', other format can be preceded "
                 "by '-' to disable them"
              << std::endl;
    std::exit(1);
}

// ---------------------------------------------------------------------------

static BaseObjectPtr buildObject(const std::string &user_string, bool kindIsCRS,
                                 const std::string &context) {
    bool isWKT = false;
    try {
        WKTNode::createFrom(user_string);
        isWKT = true;
    } catch (...) {
    }

    BaseObjectPtr obj;
    if (isWKT) {
        try {
            obj = WKTParser().createFromWKT(user_string).as_nullable();
        } catch (const std::exception &e) {
            std::cerr << context
                      << ": parsing of WKT string failed: " << e.what()
                      << std::endl;
            std::exit(1);
        }
    } else if (user_string.find("+proj=") != std::string::npos) {
        try {
            obj = PROJStringParser()
                      .createFromPROJString(user_string)
                      .as_nullable();
        } catch (const std::exception &e) {
            std::cerr << context
                      << ": parsing of PROJ string failed: " << e.what()
                      << std::endl;
            std::exit(1);
        }
    } else {
        size_t columnPos = user_string.find(':');
        if (columnPos != std::string::npos) {
            auto authority(user_string.substr(0, columnPos));
            auto code(user_string.substr(columnPos + 1));
            try {
                auto factory = AuthorityFactory::create(
                    DatabaseContext::create(), authority);
                if (kindIsCRS) {
                    obj = nn_static_pointer_cast<BaseObject>(
                              factory->createCoordinateReferenceSystem(code))
                              .as_nullable();
                } else {
                    obj = nn_static_pointer_cast<BaseObject>(
                              factory->createCoordinateOperation(code))
                              .as_nullable();
                }
            } catch (const std::exception &e) {
                std::cerr << context
                          << ": error while querying database: " << e.what()
                          << std::endl;
                std::exit(1);
            }
        }
    }

    if (!obj) {
        std::cerr << context << ": unrecognized format." << std::endl;
        std::exit(1);
    }

    return obj;
}

// ---------------------------------------------------------------------------

static void outputObject(BaseObjectPtr obj, const OutputOptions &outputOpt) {
    auto projStringExportable =
        std::dynamic_pointer_cast<IPROJStringExportable>(obj);
    bool alreadyOutputed = false;
    if (projStringExportable) {
        if (outputOpt.PROJ5) {
            try {
                if (!outputOpt.quiet) {
                    std::cout << "PROJ string: " << std::endl;
                }
                std::cout << projStringExportable->exportToPROJString(
                                 PROJStringFormatter::create(
                                     PROJStringFormatter::Convention::PROJ_5))
                          << std::endl;
                alreadyOutputed = true;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to PROJ string: " << e.what()
                          << std::endl;
            }
        }

        if (outputOpt.PROJ4) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "PROJ.4 string: " << std::endl;
                }
                std::cout << projStringExportable->exportToPROJString(
                                 PROJStringFormatter::create(
                                     PROJStringFormatter::Convention::PROJ_4))
                          << std::endl;
                alreadyOutputed = true;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to PROJ string: " << e.what()
                          << std::endl;
            }
        }
    }

    auto wktExportable = std::dynamic_pointer_cast<IWKTExportable>(obj);
    if (wktExportable) {
        if (outputOpt.WKT2_2015) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "WKT2_2015 string: " << std::endl;
                }
                std::cout << wktExportable->exportToWKT(WKTFormatter::create(
                                 WKTFormatter::Convention::WKT2_2015))
                          << std::endl;
                alreadyOutputed = true;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT2_2015: " << e.what()
                          << std::endl;
            }
        }

        if (outputOpt.WKT2_2018) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "WKT2_2018 string: " << std::endl;
                }
                std::cout << wktExportable->exportToWKT(WKTFormatter::create(
                                 WKTFormatter::Convention::WKT2_2018))
                          << std::endl;
                alreadyOutputed = true;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT2_2018: " << e.what()
                          << std::endl;
            }
        }

        if (outputOpt.WKT1_GDAL &&
            !std::dynamic_pointer_cast<Conversion>(obj)) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "WKT1_GDAL: " << std::endl;
                }
                std::cout << wktExportable->exportToWKT(WKTFormatter::create(
                                 WKTFormatter::Convention::WKT1_GDAL))
                          << std::endl;
                std::cout << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT1_GDAL: " << e.what()
                          << std::endl;
            }
        }
    }
}

// ---------------------------------------------------------------------------

static void outputOperations(
    const std::string &sourceCRSStr, const std::string &targetCRSStr,
    const ExtentPtr &bboxFilter,
    CoordinateOperationContext::SpatialCriterion spatialCriterion,
    CoordinateOperationContext::SourceTargetCRSExtentUse crsExtentUse,
    const OutputOptions &outputOpt, bool summary) {
    auto sourceObj = buildObject(sourceCRSStr, true, "source CRS");
    auto sourceCRS = std::dynamic_pointer_cast<CRS>(sourceObj);
    if (!sourceCRS) {
        std::cerr << "source CRS string is not a CRS" << std::endl;
        std::exit(1);
    }

    auto targetObj = buildObject(targetCRSStr, true, "target CRS");
    auto targetCRS = std::dynamic_pointer_cast<CRS>(targetObj);
    if (!targetCRS) {
        std::cerr << "target CRS string is not a CRS" << std::endl;
        std::exit(1);
    }

    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, bboxFilter, 0);
    ctxt->setSpatialCriterion(spatialCriterion);
    ctxt->setSourceAndTargetCRSExtentUse(crsExtentUse);
    std::vector<CoordinateOperationNNPtr> list;
    try {
        list = CoordinateOperationFactory::create()->createOperations(
            NN_CHECK_ASSERT(sourceCRS), NN_CHECK_ASSERT(targetCRS), ctxt);
    } catch (const std::exception &e) {
        std::cerr << "createOperations() failed with: " << e.what()
                  << std::endl;
        std::exit(1);
    }
    if (outputOpt.quiet && !list.empty()) {
        outputObject(nn_static_pointer_cast<BaseObject>(list[0]), outputOpt);
        return;
    }
    if (summary) {
        std::cout << "Candidate operations found: " << list.size() << std::endl;
        for (const auto &op : list) {
            auto ids = op->identifiers();
            if (!ids.empty()) {
                std::cout << *(ids[0]->codeSpace()) << ":" << ids[0]->code();
            } else {
                std::cout << "unknown id";
            }

            std::cout << ", ";

            auto name = *(op->name()->description());
            if (!name.empty()) {
                std::cout << name;
            } else {
                std::cout << "unknown name";
            }

            std::cout << ", ";

            auto accuracies = op->coordinateOperationAccuracies();
            if (!accuracies.empty()) {
                std::cout << accuracies[0]->value() << " m";
            } else {
                if (std::dynamic_pointer_cast<Conversion>(op.as_nullable())) {
                    std::cout << "0 m";
                } else {
                    std::cout << "unknown accuracy";
                }
            }

            std::cout << ", ";

            auto domains = op->domains();
            if (!domains.empty() && domains[0]->domainOfValidity() &&
                domains[0]->domainOfValidity()->description().has_value()) {
                std::cout << *(domains[0]->domainOfValidity()->description());
            } else {
                std::cout << "unknown domain of validity";
            }

            std::cout << std::endl;
        }
    } else {
        bool first = true;
        for (size_t i = 0; i < list.size(); ++i) {
            const auto &op = list[i];
            if (list.size() > 1) {
                if (!first) {
                    std::cout << std::endl;
                }
                first = false;
                std::cout << "-------------------------------------"
                          << std::endl;
                std::cout << "Operation n°" << (i + 1) << ":" << std::endl
                          << std::endl;
            }
            outputObject(nn_static_pointer_cast<BaseObject>(op), outputOpt);
        }
    }
}

// ---------------------------------------------------------------------------

int main(int argc, char **argv) {

    if (argc == 1) {
        std::cerr << pj_get_release() << std::endl;
        usage();
    }

    std::string user_string;
    std::string sourceCRSStr;
    std::string targetCRSStr;
    bool outputSwithSpecified = false;
    OutputOptions outputOpt;
    bool kindIsCRS = true;
    bool summary = false;
    ExtentPtr bboxFilter = nullptr;
    CoordinateOperationContext::SpatialCriterion spatialCriterion =
        CoordinateOperationContext::SpatialCriterion::STRICT_CONTAINMENT;
    CoordinateOperationContext::SourceTargetCRSExtentUse crsExtentUse =
        CoordinateOperationContext::SourceTargetCRSExtentUse::SMALLEST;

    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg == "-o" && i + 1 < argc) {
            outputSwithSpecified = true;
            i++;
            auto formats(split(argv[i], ','));
            for (auto format : formats) {
                if (ci_equal(format, "all")) {
                    outputOpt.PROJ5 = true;
                    outputOpt.PROJ4 = true;
                    outputOpt.WKT2_2018 = true;
                    outputOpt.WKT2_2015 = true;
                    outputOpt.WKT1_GDAL = true;
                } else if (ci_equal(format, "default")) {
                    outputOpt.PROJ5 = true;
                    outputOpt.PROJ4 = false;
                    outputOpt.WKT2_2018 = false;
                    outputOpt.WKT2_2015 = true;
                    outputOpt.WKT1_GDAL = false;
                } else if (ci_equal(format, "PROJ4") ||
                           ci_equal(format, "PROJ.4")) {
                    outputOpt.PROJ4 = true;
                } else if (ci_equal(format, "-PROJ4") ||
                           ci_equal(format, "-PROJ.4")) {
                    outputOpt.PROJ4 = false;
                } else if (ci_equal(format, "PROJ")) {
                    outputOpt.PROJ5 = true;
                } else if (ci_equal(format, "-PROJ")) {
                    outputOpt.PROJ5 = false;
                } else if (ci_equal(format, "WKT_ALL") ||
                           ci_equal(format, "WKT_ALL")) {
                    outputOpt.WKT2_2018 = true;
                    outputOpt.WKT2_2015 = true;
                    outputOpt.WKT1_GDAL = true;
                } else if (ci_equal(format, "-WKT_ALL") ||
                           ci_equal(format, "-WKT_ALL")) {
                    outputOpt.WKT2_2018 = false;
                    outputOpt.WKT2_2015 = false;
                    outputOpt.WKT1_GDAL = false;
                } else if (ci_equal(format, "WKT2_2018") ||
                           ci_equal(format, "WKT2-2018") ||
                           ci_equal(format, "WKT2:2018")) {
                    outputOpt.WKT2_2018 = true;
                } else if (ci_equal(format, "-WKT2_2018") ||
                           ci_equal(format, "-WKT2-2018") ||
                           ci_equal(format, "-WKT2:2018")) {
                    outputOpt.WKT2_2018 = false;
                } else if (ci_equal(format, "WKT2_2015") ||
                           ci_equal(format, "WKT2-2015") ||
                           ci_equal(format, "WKT2:2015")) {
                    outputOpt.WKT2_2015 = true;
                } else if (ci_equal(format, "-WKT2_2015") ||
                           ci_equal(format, "-WKT2-2015") ||
                           ci_equal(format, "-WKT2:2015")) {
                    outputOpt.WKT2_2015 = false;
                } else if (ci_equal(format, "WKT1_GDAL") ||
                           ci_equal(format, "WKT1-GDAL") ||
                           ci_equal(format, "WKT1:GDAL")) {
                    outputOpt.WKT1_GDAL = true;
                } else if (ci_equal(format, "-WKT1_GDAL") ||
                           ci_equal(format, "-WKT1-GDAL") ||
                           ci_equal(format, "-WKT1:GDAL")) {
                    outputOpt.WKT1_GDAL = false;
                } else {
                    std::cerr << "Unrecognized value for option -o: " << format
                              << std::endl;
                    usage();
                }
            }
        } else if (arg == "--bbox" && i + 1 < argc) {
            i++;
            auto bboxStr(argv[i]);
            auto bbox(split(bboxStr, ','));
            if (bbox.size() != 4) {
                std::cerr << "Incorrect number of values for option --bbox: "
                          << bboxStr << std::endl;
                usage();
            }
            try {
                bboxFilter = Extent::createFromBBOX(
                                 c_locale_stod(bbox[0]), c_locale_stod(bbox[1]),
                                 c_locale_stod(bbox[2]), c_locale_stod(bbox[3]))
                                 .as_nullable();
            } catch (const std::exception &e) {
                std::cerr << "Invalid value for option --bbox: " << bboxStr
                          << ", " << e.what() << std::endl;
                usage();
            }
        } else if (arg == "-k" && i + 1 < argc) {
            i++;
            std::string kind(argv[i]);
            if (ci_equal(kind, "crs") || ci_equal(kind, "srs")) {
                kindIsCRS = true;
            } else if (ci_equal(kind, "operation")) {
                kindIsCRS = false;
            } else {
                std::cerr << "Unrecognized value for option -k: " << kind
                          << std::endl;
                usage();
            }
        } else if ((arg == "-s" || arg == "--ssource-crs") && i + 1 < argc) {
            i++;
            sourceCRSStr = argv[i];
        } else if ((arg == "-t" || arg == "--target-crs") && i + 1 < argc) {
            i++;
            targetCRSStr = argv[i];
        } else if (arg == "-q" || arg == "--quiet") {
            outputOpt.quiet = true;
        } else if (arg == "--summary") {
            summary = true;
        } else if (arg == "--spatial-test" && i + 1 < argc) {
            i++;
            std::string value(argv[i]);
            if (ci_equal(value, "contains")) {
                spatialCriterion = CoordinateOperationContext::
                    SpatialCriterion::STRICT_CONTAINMENT;
            } else if (ci_equal(value, "intersects")) {
                spatialCriterion = CoordinateOperationContext::
                    SpatialCriterion::PARTIAL_INTERSECTION;
            } else {
                std::cerr << "Unrecognized value for option --patial-test: "
                          << value << std::endl;
                usage();
            }
        } else if (arg == "--crs-extent-use" && i + 1 < argc) {
            i++;
            std::string value(argv[i]);
            if (ci_equal(value, "NONE")) {
                crsExtentUse =
                    CoordinateOperationContext::SourceTargetCRSExtentUse::NONE;
            } else if (ci_equal(value, "BOTH")) {
                crsExtentUse =
                    CoordinateOperationContext::SourceTargetCRSExtentUse::BOTH;
            } else if (ci_equal(value, "INTERSECTION")) {
                crsExtentUse = CoordinateOperationContext::
                    SourceTargetCRSExtentUse::INTERSECTION;
            } else if (ci_equal(value, "SMALLEST")) {
                crsExtentUse = CoordinateOperationContext::
                    SourceTargetCRSExtentUse::SMALLEST;
            } else {
                std::cerr << "Unrecognized value for option --crs-extent-use: "
                          << value << std::endl;
                usage();
            }
        } else if (arg == "-?" || arg == "--help") {
            usage();
        } else if (arg[0] == '-') {
            std::cerr << "Unrecognized option: " << arg << std::endl;
            usage();
        } else {
            if (user_string.empty()) {
                user_string = arg;
            } else {
                std::cerr << "Too many parameters: " << arg << std::endl;
                usage();
            }
        }
    }
    if (!sourceCRSStr.empty() && targetCRSStr.empty()) {
        std::cerr << "Source CRS specified, but missing target CRS"
                  << std::endl;
        usage();
    } else if (sourceCRSStr.empty() && !targetCRSStr.empty()) {
        std::cerr << "Target CRS specified, but missing source CRS"
                  << std::endl;
        usage();
    } else if (!sourceCRSStr.empty() && !targetCRSStr.empty()) {
        if (!user_string.empty()) {
            std::cerr << "Unused extra value" << std::endl;
            usage();
        }
    } else if (user_string.empty()) {
        std::cerr << "Missing user string" << std::endl;
        usage();
    }

    if (!outputSwithSpecified) {
        outputOpt.PROJ5 = true;
        outputOpt.WKT2_2015 = true;
    }

    if (outputOpt.quiet &&
        (outputOpt.PROJ5 + outputOpt.PROJ4 + outputOpt.WKT2_2018 +
         outputOpt.WKT2_2015 + outputOpt.WKT1_GDAL) != 1) {
        std::cerr << "-q can only be used with a single output format"
                  << std::endl;
        usage();
    }

    if (!user_string.empty()) {
        auto obj(buildObject(user_string, kindIsCRS, "input string"));
        outputObject(obj, outputOpt);
    } else {
        outputOperations(sourceCRSStr, targetCRSStr, bboxFilter,
                         spatialCriterion, crsExtentUse, outputOpt, summary);
    }

    return 0;
}

//! @endcond
