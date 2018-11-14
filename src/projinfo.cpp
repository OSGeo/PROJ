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
#include <fstream> // std::ifstream
#include <iostream>
#include <utility>

#include "projects.h"

#include <proj/coordinateoperation.hpp>
#include <proj/crs.hpp>
#include <proj/io.hpp>
#include <proj/metadata.hpp>
#include <proj/util.hpp>

#include "proj/internal/internal.hpp" // for split

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
    bool WKT2_2018_SIMPLIFIED = false;
    bool WKT2_2015 = false;
    bool WKT2_2015_SIMPLIFIED = false;
    bool WKT1_GDAL = false;
    bool WKT1_ESRI = false;
    bool c_ify = false;
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
        << "                [--grid-check none|discard_missing|sort]"
        << std::endl
        << "                [--pivot-crs none|{auth:code[,auth:code]*}]"
        << std::endl
        << "                [--boundcrs-to-wgs84]" << std::endl
        << "                [--main-db-path path] [--aux-db-path path]*"
        << std::endl
        << "                [--identify]" << std::endl
        << "                {object_definition} | (-s {srs_def} -t {srs_def})"
        << std::endl;
    std::cerr << std::endl;
    std::cerr << "-o: formats is a comma separated combination of: "
                 "all,default,PROJ4,PROJ,WKT_ALL,WKT2_2015,WKT2_2018,WKT1_GDAL,"
                 "WKT1_ESRI"
              << std::endl;
    std::cerr << "    Except 'all' and 'default', other format can be preceded "
                 "by '-' to disable them"
              << std::endl;
    std::cerr << std::endl;
    std::cerr << "{object_definition} might be a PROJ string, a WKT string, "
                 " a AUTHORITY:CODE, or urn:ogc:def:OBJECT_TYPE:AUTHORITY::CODE"
              << std::endl;
    std::exit(1);
}

// ---------------------------------------------------------------------------

static std::string un_c_ify_string(const std::string &str) {
    std::string out(str);
    out = out.substr(1, out.size() - 2);
    out = replaceAll(out, "\\\"", "{ESCAPED_DOUBLE_QUOTE}");
    out = replaceAll(out, "\\n\"", "");
    out = replaceAll(out, "\"", "");
    out = replaceAll(out, "{ESCAPED_DOUBLE_QUOTE}", "\"");
    return out;
}

// ---------------------------------------------------------------------------

static std::string c_ify_string(const std::string &str) {
    std::string out(str);
    out = replaceAll(out, "\"", "{DOUBLE_QUOTE}");
    out = replaceAll(out, "\n", "\\n\"\n\"");
    out = replaceAll(out, "{DOUBLE_QUOTE}", "\\\"");
    return "\"" + out + "\"";
}

// ---------------------------------------------------------------------------

static BaseObjectNNPtr buildObject(DatabaseContextPtr dbContext,
                                   const std::string &user_string,
                                   bool kindIsCRS, const std::string &context,
                                   bool buildBoundCRSToWGS84) {
    BaseObjectPtr obj;

    std::string l_user_string(user_string);
    if (!user_string.empty() && user_string[0] == '@') {
        std::ifstream fs;
        auto filename = user_string.substr(1);
        fs.open(filename, std::fstream::in | std::fstream::binary);
        if (!fs.is_open()) {
            std::cerr << context << ": cannot open " << filename << std::endl;
            std::exit(1);
        }
        l_user_string.clear();
        while (!fs.eof()) {
            char buffer[256];
            fs.read(buffer, sizeof(buffer));
            l_user_string.append(buffer, static_cast<size_t>(fs.gcount()));
            if (l_user_string.size() > 100 * 1000) {
                fs.close();
                std::cerr << context << ": too big file" << std::endl;
                std::exit(1);
            }
        }
        fs.close();
    }
    if (!l_user_string.empty() && l_user_string.back() == '\n') {
        l_user_string.resize(l_user_string.size() - 1);
    }
    if (!l_user_string.empty() && l_user_string.back() == '\r') {
        l_user_string.resize(l_user_string.size() - 1);
    }

    try {
        auto tokens = split(l_user_string, ':');
        if (!kindIsCRS && tokens.size() == 2) {
            auto urn = "urn:ogc:def:coordinateOperation:" + tokens[0] + "::" +
                       tokens[1];
            obj = createFromUserInput(urn, dbContext).as_nullable();
        } else {
            // Convenience to be able to use C escaped strings...
            if (l_user_string.size() > 2 && l_user_string[0] == '"' &&
                l_user_string.back() == '"' &&
                l_user_string.find("\\\"") != std::string::npos) {
                l_user_string = un_c_ify_string(l_user_string);
            }
            obj = createFromUserInput(l_user_string, dbContext).as_nullable();
        }
    } catch (const std::exception &e) {
        std::cerr << context << ": parsing of user string failed: " << e.what()
                  << std::endl;
        std::exit(1);
    }

    if (buildBoundCRSToWGS84) {
        auto crs = std::dynamic_pointer_cast<CRS>(obj);
        if (crs) {
            obj = crs->createBoundCRSToWGS84IfPossible(dbContext).as_nullable();
        }
    }

    return NN_NO_CHECK(obj);
}

// ---------------------------------------------------------------------------

static void outputObject(DatabaseContextPtr dbContext, BaseObjectNNPtr obj,
                         const OutputOptions &outputOpt) {
    auto projStringExportable =
        nn_dynamic_pointer_cast<IPROJStringExportable>(obj);
    bool alreadyOutputed = false;
    if (projStringExportable) {
        if (outputOpt.PROJ5) {
            try {
                if (!outputOpt.quiet) {
                    std::cout << "PROJ string: " << std::endl;
                }
                std::cout << projStringExportable->exportToPROJString(
                                 PROJStringFormatter::create(
                                     PROJStringFormatter::Convention::PROJ_5,
                                     dbContext)
                                     .get())
                          << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to PROJ string: " << e.what()
                          << std::endl;
            }
            alreadyOutputed = true;
        }

        if (outputOpt.PROJ4) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "PROJ.4 string: " << std::endl;
                }

                auto crs = nn_dynamic_pointer_cast<CRS>(obj);
                std::shared_ptr<IPROJStringExportable> objToExport;
                if (crs) {
                    objToExport =
                        nn_dynamic_pointer_cast<IPROJStringExportable>(
                            crs->createBoundCRSToWGS84IfPossible(dbContext));
                }
                if (!objToExport) {
                    objToExport = projStringExportable;
                }

                std::cout << objToExport->exportToPROJString(
                                 PROJStringFormatter::create(
                                     PROJStringFormatter::Convention::PROJ_4,
                                     dbContext)
                                     .get())
                          << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to PROJ string: " << e.what()
                          << std::endl;
            }
            alreadyOutputed = true;
        }
    }

    auto wktExportable = nn_dynamic_pointer_cast<IWKTExportable>(obj);
    if (wktExportable) {
        if (outputOpt.WKT2_2015) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "WKT2_2015 string: " << std::endl;
                }
                auto wkt = wktExportable->exportToWKT(
                    WKTFormatter::create(WKTFormatter::Convention::WKT2_2015)
                        .get());
                if (outputOpt.c_ify) {
                    wkt = c_ify_string(wkt);
                }
                std::cout << wkt << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT2_2015: " << e.what()
                          << std::endl;
            }
            alreadyOutputed = true;
        }

        if (outputOpt.WKT2_2015_SIMPLIFIED) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "WKT2_2015_SIMPLIFIED string: " << std::endl;
                }
                auto wkt = wktExportable->exportToWKT(
                    WKTFormatter::create(
                        WKTFormatter::Convention::WKT2_2015_SIMPLIFIED)
                        .get());
                if (outputOpt.c_ify) {
                    wkt = c_ify_string(wkt);
                }
                std::cout << wkt << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT2_2015_SIMPLIFIED: "
                          << e.what() << std::endl;
            }
            alreadyOutputed = true;
        }

        if (outputOpt.WKT2_2018) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "WKT2_2018 string: " << std::endl;
                }
                auto wkt = wktExportable->exportToWKT(
                    WKTFormatter::create(WKTFormatter::Convention::WKT2_2018)
                        .get());
                if (outputOpt.c_ify) {
                    wkt = c_ify_string(wkt);
                }
                std::cout << wkt << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT2_2018: " << e.what()
                          << std::endl;
            }
            alreadyOutputed = true;
        }

        if (outputOpt.WKT2_2018_SIMPLIFIED) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "WKT2_2018_SIMPLIFIED string: " << std::endl;
                }
                auto wkt = wktExportable->exportToWKT(
                    WKTFormatter::create(
                        WKTFormatter::Convention::WKT2_2018_SIMPLIFIED)
                        .get());
                if (outputOpt.c_ify) {
                    wkt = c_ify_string(wkt);
                }
                std::cout << wkt << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT2_2018_SIMPLIFIED: "
                          << e.what() << std::endl;
            }
            alreadyOutputed = true;
        }

        if (outputOpt.WKT1_GDAL && !nn_dynamic_pointer_cast<Conversion>(obj)) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "WKT1_GDAL: " << std::endl;
                }

                auto crs = nn_dynamic_pointer_cast<CRS>(obj);
                std::shared_ptr<IWKTExportable> objToExport;
                if (crs) {
                    objToExport = nn_dynamic_pointer_cast<IWKTExportable>(
                        crs->createBoundCRSToWGS84IfPossible(dbContext));
                }
                if (!objToExport) {
                    objToExport = wktExportable;
                }

                auto wkt = objToExport->exportToWKT(
                    WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)
                        .get());
                if (outputOpt.c_ify) {
                    wkt = c_ify_string(wkt);
                }
                std::cout << wkt << std::endl;
                std::cout << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT1_GDAL: " << e.what()
                          << std::endl;
            }
            alreadyOutputed = true;
        }

        if (outputOpt.WKT1_ESRI && !nn_dynamic_pointer_cast<Conversion>(obj)) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "WKT1_ESRI: " << std::endl;
                }

                auto wkt = wktExportable->exportToWKT(
                    WKTFormatter::create(WKTFormatter::Convention::WKT1_ESRI,
                                         dbContext)
                        .get());
                if (outputOpt.c_ify) {
                    wkt = c_ify_string(wkt);
                }
                std::cout << wkt << std::endl;
                std::cout << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT1_ESRI: " << e.what()
                          << std::endl;
            }
        }
    }
}

// ---------------------------------------------------------------------------

static void outputOperations(
    DatabaseContextPtr dbContext, const std::string &sourceCRSStr,
    const std::string &targetCRSStr, const ExtentPtr &bboxFilter,
    CoordinateOperationContext::SpatialCriterion spatialCriterion,
    CoordinateOperationContext::SourceTargetCRSExtentUse crsExtentUse,
    CoordinateOperationContext::GridAvailabilityUse gridAvailabilityUse,
    bool allowPivots,
    const std::vector<std::pair<std::string, std::string>> &pivots,
    const std::string &authority, bool usePROJGridAlternatives,
    const OutputOptions &outputOpt, bool summary) {
    auto sourceObj =
        buildObject(dbContext, sourceCRSStr, true, "source CRS", false);
    auto sourceCRS = nn_dynamic_pointer_cast<CRS>(sourceObj);
    if (!sourceCRS) {
        std::cerr << "source CRS string is not a CRS" << std::endl;
        std::exit(1);
    }

    auto targetObj =
        buildObject(dbContext, targetCRSStr, true, "target CRS", false);
    auto targetCRS = nn_dynamic_pointer_cast<CRS>(targetObj);
    if (!targetCRS) {
        std::cerr << "target CRS string is not a CRS" << std::endl;
        std::exit(1);
    }

    std::vector<CoordinateOperationNNPtr> list;
    try {
        auto authFactory =
            dbContext
                ? AuthorityFactory::create(NN_NO_CHECK(dbContext), authority)
                      .as_nullable()
                : nullptr;
        auto ctxt =
            CoordinateOperationContext::create(authFactory, bboxFilter, 0);
        ctxt->setSpatialCriterion(spatialCriterion);
        ctxt->setSourceAndTargetCRSExtentUse(crsExtentUse);
        ctxt->setGridAvailabilityUse(gridAvailabilityUse);
        ctxt->setAllowUseIntermediateCRS(allowPivots);
        ctxt->setIntermediateCRS(pivots);
        ctxt->setUsePROJAlternativeGridNames(usePROJGridAlternatives);
        list = CoordinateOperationFactory::create()->createOperations(
            NN_NO_CHECK(sourceCRS), NN_NO_CHECK(targetCRS), ctxt);
    } catch (const std::exception &e) {
        std::cerr << "createOperations() failed with: " << e.what()
                  << std::endl;
        std::exit(1);
    }
    if (outputOpt.quiet && !list.empty()) {
        outputObject(dbContext, list[0], outputOpt);
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

            auto name = op->nameStr();
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
                std::cout << "Operation n"
                             "\xC2\xB0"
                          << (i + 1) << ":" << std::endl
                          << std::endl;
            }
            outputObject(dbContext, op, outputOpt);
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
    bool user_string_specified = false;
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
    bool buildBoundCRSToWGS84 = false;
    CoordinateOperationContext::GridAvailabilityUse gridAvailabilityUse =
        CoordinateOperationContext::GridAvailabilityUse::USE_FOR_SORTING;
    bool allowPivots = true;
    std::vector<std::pair<std::string, std::string>> pivots;
    bool usePROJGridAlternatives = true;
    std::string mainDBPath;
    std::vector<std::string> auxDBPath;
    bool guessDialect = false;
    std::string authority;
    bool identify = false;

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
                } else if (ci_equal(format, "WKT2_2018_SIMPLIFIED") ||
                           ci_equal(format, "WKT2-2018_SIMPLIFIED") ||
                           ci_equal(format, "WKT2:2018_SIMPLIFIED")) {
                    outputOpt.WKT2_2018_SIMPLIFIED = true;
                } else if (ci_equal(format, "-WKT2_2018") ||
                           ci_equal(format, "-WKT2-2018") ||
                           ci_equal(format, "-WKT2:2018")) {
                    outputOpt.WKT2_2018 = false;
                } else if (ci_equal(format, "WKT2_2015") ||
                           ci_equal(format, "WKT2-2015") ||
                           ci_equal(format, "WKT2:2015")) {
                    outputOpt.WKT2_2015 = true;
                } else if (ci_equal(format, "WKT2_2015_SIMPLIFIED") ||
                           ci_equal(format, "WKT2-2015_SIMPLIFIED") ||
                           ci_equal(format, "WKT2:2015_SIMPLIFIED")) {
                    outputOpt.WKT2_2015_SIMPLIFIED = true;
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
                } else if (ci_equal(format, "WKT1_ESRI") ||
                           ci_equal(format, "WKT1-ESRI") ||
                           ci_equal(format, "WKT1:ESRI")) {
                    outputOpt.WKT1_ESRI = true;
                } else if (ci_equal(format, "-WKT1_ESRI") ||
                           ci_equal(format, "-WKT1-ESRI") ||
                           ci_equal(format, "-WKT1:ESRI")) {
                    outputOpt.WKT1_ESRI = false;
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
        } else if (arg == "--c-ify") {
            outputOpt.c_ify = true;
        } else if (arg == "--summary") {
            summary = true;
        } else if (ci_equal(arg, "--boundcrs-to-wgs84")) {
            buildBoundCRSToWGS84 = true;

            // undocumented: only for debugging purposes
        } else if (ci_equal(arg, "--no-proj-grid-alternatives")) {
            usePROJGridAlternatives = false;

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
        } else if (arg == "--grid-check" && i + 1 < argc) {
            i++;
            std::string value(argv[i]);
            if (ci_equal(value, "none")) {
                gridAvailabilityUse = CoordinateOperationContext::
                    GridAvailabilityUse::IGNORE_GRID_AVAILABILITY;
            } else if (ci_equal(value, "discard_missing")) {
                gridAvailabilityUse = CoordinateOperationContext::
                    GridAvailabilityUse::DISCARD_OPERATION_IF_MISSING_GRID;
            } else if (ci_equal(value, "sort")) {
                gridAvailabilityUse = CoordinateOperationContext::
                    GridAvailabilityUse::USE_FOR_SORTING;
            } else {
                std::cerr << "Unrecognized value for option --grid-check: "
                          << value << std::endl;
                usage();
            }
        } else if (arg == "--pivot-crs" && i + 1 < argc) {
            i++;
            auto value(argv[i]);
            if (ci_equal(std::string(value), "none")) {
                allowPivots = false;
            } else {
                auto splitValue(split(value, ','));
                for (const auto &v : splitValue) {
                    auto auth_code = split(v, ':');
                    if (auth_code.size() != 2) {
                        std::cerr
                            << "Unrecognized value for option --grid-check: "
                            << value << std::endl;
                        usage();
                    }
                    pivots.emplace_back(
                        std::make_pair(auth_code[0], auth_code[1]));
                }
            }
        } else if (arg == "--main-db-path" && i + 1 < argc) {
            i++;
            mainDBPath = argv[i];
        } else if (arg == "--aux-db-path" && i + 1 < argc) {
            i++;
            auxDBPath.push_back(argv[i]);
        } else if (arg == "--guess-dialect") {
            guessDialect = true;
        } else if (arg == "--authority" && i + 1 < argc) {
            i++;
            authority = argv[i];
        } else if (arg == "--identify") {
            identify = true;
        } else if (arg == "-?" || arg == "--help") {
            usage();
        } else if (arg[0] == '-') {
            std::cerr << "Unrecognized option: " << arg << std::endl;
            usage();
        } else {
            if (!user_string_specified) {
                user_string_specified = true;
                user_string = arg;
            } else {
                std::cerr << "Too many parameters: " << arg << std::endl;
                usage();
            }
        }
    }

    DatabaseContextPtr dbContext;
    try {
        dbContext =
            DatabaseContext::create(mainDBPath, auxDBPath).as_nullable();
    } catch (const std::exception &e) {
        if (!mainDBPath.empty() || !auxDBPath.empty()) {
            std::cerr << "ERROR: Cannot create database connection: "
                      << e.what() << std::endl;
            std::exit(1);
        }
        std::cerr << "WARNING: Cannot create database connection: " << e.what()
                  << std::endl;
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
        if (user_string_specified) {
            std::cerr << "Unused extra value" << std::endl;
            usage();
        }
    } else if (!user_string_specified) {
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
        auto obj(buildObject(dbContext, user_string, kindIsCRS, "input string",
                             buildBoundCRSToWGS84));
        if (guessDialect) {
            auto dialect = WKTParser().guessDialect(user_string);
            std::cout << "Guessed WKT dialect: ";
            if (dialect == WKTParser::WKTGuessedDialect::WKT2_2018) {
                std::cout << "WKT2_2018";
            } else if (dialect == WKTParser::WKTGuessedDialect::WKT2_2015) {
                std::cout << "WKT2_2015";
            } else if (dialect == WKTParser::WKTGuessedDialect::WKT1_GDAL) {
                std::cout << "WKT1_GDAL";
            } else if (dialect == WKTParser::WKTGuessedDialect::WKT1_ESRI) {
                std::cout << "WKT1_ESRI";
            } else {
                std::cout << "Not WKT / unknown";
            }
            std::cout << std::endl;
        }
        outputObject(dbContext, obj, outputOpt);
        if (identify) {
            auto crs = dynamic_cast<CRS *>(obj.get());
            if (crs) {
                try {
                    auto res = crs->identify(
                        dbContext
                            ? AuthorityFactory::create(NN_NO_CHECK(dbContext),
                                                       authority)
                                  .as_nullable()
                            : nullptr);
                    std::cout << std::endl;
                    std::cout << "Identification match count: " << res.size()
                              << std::endl;
                    for (const auto &pair : res) {
                        const auto &identifiedCRS = pair.first;
                        const auto &ids = identifiedCRS->identifiers();
                        if (!ids.empty()) {
                            std::cout << *ids[0]->codeSpace() << ":"
                                      << ids[0]->code() << ": " << pair.second
                                      << " %" << std::endl;
                        } else {
                            auto boundCRS =
                                dynamic_cast<BoundCRS *>(identifiedCRS.get());
                            if (boundCRS &&
                                !boundCRS->baseCRS()->identifiers().empty()) {
                                const auto &idsBase =
                                    boundCRS->baseCRS()->identifiers();
                                std::cout << "BoundCRS of "
                                          << *idsBase[0]->codeSpace() << ":"
                                          << idsBase[0]->code() << ": "
                                          << pair.second << " %" << std::endl;
                            } else {
                                std::cout
                                    << "un-identifier CRS: " << pair.second
                                    << " %" << std::endl;
                            }
                        }
                    }
                } catch (const std::exception &e) {
                    std::cerr << "Identification failed: " << e.what()
                              << std::endl;
                }
            }
        }
    } else {
        outputOperations(dbContext, sourceCRSStr, targetCRSStr, bboxFilter,
                         spatialCriterion, crsExtentUse, gridAvailabilityUse,
                         allowPivots, pivots, authority,
                         usePROJGridAlternatives, outputOpt, summary);
    }

    return 0;
}

//! @endcond
