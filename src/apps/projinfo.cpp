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

#include "proj.h"
#include "proj_internal.h"

#include <proj/common.hpp>
#include <proj/coordinateoperation.hpp>
#include <proj/crs.hpp>
#include <proj/io.hpp>
#include <proj/metadata.hpp>
#include <proj/util.hpp>

#include "proj/internal/internal.hpp" // for split

using namespace NS_PROJ::common;
using namespace NS_PROJ::crs;
using namespace NS_PROJ::io;
using namespace NS_PROJ::metadata;
using namespace NS_PROJ::operation;
using namespace NS_PROJ::util;
using namespace NS_PROJ::internal;

// ---------------------------------------------------------------------------

namespace { // anonymous namespace
struct OutputOptions {
    bool quiet = false;
    bool PROJ5 = false;
    bool WKT2_2019 = false;
    bool WKT2_2019_SIMPLIFIED = false;
    bool WKT2_2015 = false;
    bool WKT2_2015_SIMPLIFIED = false;
    bool WKT1_GDAL = false;
    bool WKT1_ESRI = false;
    bool PROJJSON = false;
    bool c_ify = false;
    bool singleLine = false;
    bool strict = true;
    bool ballparkAllowed = true;
};
} // anonymous namespace

// ---------------------------------------------------------------------------

static void usage() {
    std::cerr
        << "usage: projinfo [-o formats] "
           "[-k crs|operation|datum|ensemble|ellipsoid] "
           "[--summary] [-q]"
        << std::endl
        << "                ([--area name_or_code] | "
           "[--bbox west_long,south_lat,east_long,north_lat]) "
        << std::endl
        << "                [--spatial-test contains|intersects]" << std::endl
        << "                [--crs-extent-use none|both|intersection|smallest]"
        << std::endl
        << "                [--grid-check "
           "none|discard_missing|sort|known_available] "
        << std::endl
        << "                [--pivot-crs always|if_no_direct_transformation|"
        << "never|{auth:code[,auth:code]*}]" << std::endl
        << "                [--show-superseded] [--hide-ballpark]" << std::endl
        << "                [--boundcrs-to-wgs84]" << std::endl
        << "                [--main-db-path path] [--aux-db-path path]*"
        << std::endl
        << "                [--identify] [--3d]" << std::endl
        << "                [--c-ify] [--single-line]" << std::endl
        << "                --searchpaths | --remote-data |" << std::endl
        << "                {object_definition} | (-s {srs_def} -t {srs_def})"
        << std::endl;
    std::cerr << std::endl;
    std::cerr << "-o: formats is a comma separated combination of: "
                 "all,default,PROJ,WKT_ALL,WKT2:2015,WKT2:2019,WKT1:GDAL,"
                 "WKT1:ESRI,PROJJSON"
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

static BaseObjectNNPtr buildObject(
    DatabaseContextPtr dbContext, const std::string &user_string,
    const std::string &kind, const std::string &context,
    bool buildBoundCRSToWGS84,
    CoordinateOperationContext::IntermediateCRSUse allowUseIntermediateCRS,
    bool promoteTo3D, bool quiet) {
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
        if (kind == "operation" && tokens.size() == 2) {
            auto urn = "urn:ogc:def:coordinateOperation:" + tokens[0] + "::" +
                       tokens[1];
            obj = createFromUserInput(urn, dbContext).as_nullable();
        } else if ((kind == "ellipsoid" || kind == "datum" ||
                    kind == "ensemble") &&
                   tokens.size() == 2) {
            auto urn =
                "urn:ogc:def:" + kind + ":" + tokens[0] + "::" + tokens[1];
            obj = createFromUserInput(urn, dbContext).as_nullable();
        } else {
            // Convenience to be able to use C escaped strings...
            if (l_user_string.size() > 2 && l_user_string[0] == '"' &&
                l_user_string.back() == '"' &&
                l_user_string.find("\\\"") != std::string::npos) {
                l_user_string = un_c_ify_string(l_user_string);
            }
            WKTParser wktParser;
            if (wktParser.guessDialect(l_user_string) !=
                WKTParser::WKTGuessedDialect::NOT_WKT) {
                wktParser.setStrict(false);
                wktParser.attachDatabaseContext(dbContext);
                obj = wktParser.createFromWKT(l_user_string).as_nullable();
                if (!quiet) {
                    auto warnings = wktParser.warningList();
                    if (!warnings.empty()) {
                        for (const auto &str : warnings) {
                            std::cerr << "Warning: " << str << std::endl;
                        }
                    }
                }
            } else if (dbContext && !kind.empty() && kind != "crs" &&
                       l_user_string.find(':') == std::string::npos) {
                std::vector<AuthorityFactory::ObjectType> allowedTypes;
                if (kind == "operation")
                    allowedTypes.push_back(
                        AuthorityFactory::ObjectType::COORDINATE_OPERATION);
                else if (kind == "ellipsoid")
                    allowedTypes.push_back(
                        AuthorityFactory::ObjectType::ELLIPSOID);
                else if (kind == "datum")
                    allowedTypes.push_back(AuthorityFactory::ObjectType::DATUM);
                else if (kind == "ensemble")
                    allowedTypes.push_back(
                        AuthorityFactory::ObjectType::DATUM_ENSEMBLE);
                constexpr size_t limitResultCount = 10;
                auto factory = AuthorityFactory::create(NN_NO_CHECK(dbContext),
                                                        std::string());
                for (int pass = 0; pass <= 1; ++pass) {
                    const bool approximateMatch = (pass == 1);
                    auto res = factory->createObjectsFromName(
                        l_user_string, allowedTypes, approximateMatch,
                        limitResultCount);
                    if (res.size() == 1) {
                        obj = res.front().as_nullable();
                    } else {
                        for (const auto &l_obj : res) {
                            if (Identifier::isEquivalentName(
                                    l_obj->nameStr().c_str(),
                                    l_user_string.c_str())) {
                                obj = l_obj.as_nullable();
                                break;
                            }
                        }
                        if (obj) {
                            break;
                        }
                    }
                    if (res.size() > 1) {
                        std::string msg("several objects matching this name: ");
                        bool first = true;
                        for (const auto &l_obj : res) {
                            if (msg.size() > 200) {
                                msg += ", ...";
                                break;
                            }
                            if (!first) {
                                msg += ", ";
                            }
                            first = false;
                            msg += l_obj->nameStr();
                        }
                        std::cerr << context << ": " << msg << std::endl;
                        std::exit(1);
                    }
                }

            } else {
                obj =
                    createFromUserInput(l_user_string, dbContext).as_nullable();
            }
        }
    } catch (const std::exception &e) {
        std::cerr << context << ": parsing of user string failed: " << e.what()
                  << std::endl;
        std::exit(1);
    }

    if (buildBoundCRSToWGS84) {
        auto crs = std::dynamic_pointer_cast<CRS>(obj);
        if (crs) {
            obj = crs->createBoundCRSToWGS84IfPossible(dbContext,
                                                       allowUseIntermediateCRS)
                      .as_nullable();
        }
    }

    if (promoteTo3D) {
        auto crs = std::dynamic_pointer_cast<CRS>(obj);
        if (crs) {
            obj = crs->promoteTo3D(std::string(), dbContext).as_nullable();
        }
    }

    return NN_NO_CHECK(obj);
}

// ---------------------------------------------------------------------------

static void outputObject(
    DatabaseContextPtr dbContext, BaseObjectNNPtr obj,
    CoordinateOperationContext::IntermediateCRSUse allowUseIntermediateCRS,
    const OutputOptions &outputOpt) {

    auto identified = dynamic_cast<const IdentifiedObject *>(obj.get());
    if (!outputOpt.quiet && identified && identified->isDeprecated()) {
        std::cout << "Warning: object is deprecated" << std::endl;
        auto crs = dynamic_cast<const CRS *>(obj.get());
        if (crs && dbContext) {
            try {
                auto list = crs->getNonDeprecated(NN_NO_CHECK(dbContext));
                if (!list.empty()) {
                    std::cout << "Alternative non-deprecated CRS:" << std::endl;
                }
                for (const auto &altCRS : list) {
                    const auto &ids = altCRS->identifiers();
                    if (!ids.empty()) {
                        std::cout << "  " << *(ids[0]->codeSpace()) << ":"
                                  << ids[0]->code() << std::endl;
                    }
                }
            } catch (const std::exception &) {
            }
        }
        std::cout << std::endl;
    }

    auto projStringExportable =
        nn_dynamic_pointer_cast<IPROJStringExportable>(obj);
    bool alreadyOutputed = false;
    if (projStringExportable) {
        if (outputOpt.PROJ5) {
            try {
                auto crs = nn_dynamic_pointer_cast<CRS>(obj);
                if (!outputOpt.quiet) {
                    if (crs) {
                        std::cout << "PROJ.4 string:" << std::endl;
                    } else {
                        std::cout << "PROJ string:" << std::endl;
                    }
                }

                std::shared_ptr<IPROJStringExportable> objToExport;
                if (crs) {
                    objToExport =
                        nn_dynamic_pointer_cast<IPROJStringExportable>(
                            crs->createBoundCRSToWGS84IfPossible(
                                dbContext, allowUseIntermediateCRS));
                }
                if (!objToExport) {
                    objToExport = projStringExportable;
                }

                auto formatter = PROJStringFormatter::create(
                    PROJStringFormatter::Convention::PROJ_5, dbContext);
                formatter->setMultiLine(!outputOpt.singleLine &&
                                        crs == nullptr);
                std::cout << objToExport->exportToPROJString(formatter.get())
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
                    std::cout << "WKT2:2015 string:" << std::endl;
                }
                auto formatter =
                    WKTFormatter::create(WKTFormatter::Convention::WKT2_2015);
                formatter->setMultiLine(!outputOpt.singleLine);
                formatter->setStrict(outputOpt.strict);
                auto wkt = wktExportable->exportToWKT(formatter.get());
                if (outputOpt.c_ify) {
                    wkt = c_ify_string(wkt);
                }
                std::cout << wkt << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT2:2015: " << e.what()
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
                    std::cout << "WKT2:2015_SIMPLIFIED string:" << std::endl;
                }
                auto formatter = WKTFormatter::create(
                    WKTFormatter::Convention::WKT2_2015_SIMPLIFIED);
                if (outputOpt.singleLine) {
                    formatter->setMultiLine(false);
                }
                formatter->setStrict(outputOpt.strict);
                auto wkt = wktExportable->exportToWKT(formatter.get());
                if (outputOpt.c_ify) {
                    wkt = c_ify_string(wkt);
                }
                std::cout << wkt << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT2:2015_SIMPLIFIED: "
                          << e.what() << std::endl;
            }
            alreadyOutputed = true;
        }

        if (outputOpt.WKT2_2019) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "WKT2:2019 string:" << std::endl;
                }
                auto formatter =
                    WKTFormatter::create(WKTFormatter::Convention::WKT2_2019);
                if (outputOpt.singleLine) {
                    formatter->setMultiLine(false);
                }
                formatter->setStrict(outputOpt.strict);
                auto wkt = wktExportable->exportToWKT(formatter.get());
                if (outputOpt.c_ify) {
                    wkt = c_ify_string(wkt);
                }
                std::cout << wkt << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT2:2019: " << e.what()
                          << std::endl;
            }
            alreadyOutputed = true;
        }

        if (outputOpt.WKT2_2019_SIMPLIFIED) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "WKT2:2019_SIMPLIFIED string:" << std::endl;
                }
                auto formatter = WKTFormatter::create(
                    WKTFormatter::Convention::WKT2_2019_SIMPLIFIED);
                if (outputOpt.singleLine) {
                    formatter->setMultiLine(false);
                }
                formatter->setStrict(outputOpt.strict);
                auto wkt = wktExportable->exportToWKT(formatter.get());
                if (outputOpt.c_ify) {
                    wkt = c_ify_string(wkt);
                }
                std::cout << wkt << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT2:2019_SIMPLIFIED: "
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
                    std::cout << "WKT1:GDAL string:" << std::endl;
                }

                auto formatter = WKTFormatter::create(
                    WKTFormatter::Convention::WKT1_GDAL, dbContext);
                if (outputOpt.singleLine) {
                    formatter->setMultiLine(false);
                }
                formatter->setStrict(outputOpt.strict);
                auto wkt = wktExportable->exportToWKT(formatter.get());
                if (outputOpt.c_ify) {
                    wkt = c_ify_string(wkt);
                }
                std::cout << wkt << std::endl;
                std::cout << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT1:GDAL: " << e.what()
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
                    std::cout << "WKT1:ESRI string:" << std::endl;
                }

                auto formatter = WKTFormatter::create(
                    WKTFormatter::Convention::WKT1_ESRI, dbContext);
                formatter->setStrict(outputOpt.strict);
                auto wkt = wktExportable->exportToWKT(formatter.get());
                if (outputOpt.c_ify) {
                    wkt = c_ify_string(wkt);
                }
                std::cout << wkt << std::endl;
                std::cout << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to WKT1:ESRI: " << e.what()
                          << std::endl;
            }
            alreadyOutputed = true;
        }
    }

    auto JSONExportable = nn_dynamic_pointer_cast<IJSONExportable>(obj);
    if (JSONExportable) {
        if (outputOpt.PROJJSON) {
            try {
                if (alreadyOutputed) {
                    std::cout << std::endl;
                }
                if (!outputOpt.quiet) {
                    std::cout << "PROJJSON:" << std::endl;
                }
                auto formatter(JSONFormatter::create(dbContext));
                if (outputOpt.singleLine) {
                    formatter->setMultiLine(false);
                }
                auto jsonString(JSONExportable->exportToJSON(formatter.get()));
                if (outputOpt.c_ify) {
                    jsonString = c_ify_string(jsonString);
                }
                std::cout << jsonString << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "Error when exporting to PROJJSON: " << e.what()
                          << std::endl;
            }
            // alreadyOutputed = true;
        }
    }

    auto op = dynamic_cast<CoordinateOperation *>(obj.get());
    if (!outputOpt.quiet && op && dbContext &&
        getenv("PROJINFO_NO_GRID_CHECK") == nullptr) {
        try {
            auto setGrids = op->gridsNeeded(dbContext, false);
            bool firstWarning = true;
            for (const auto &grid : setGrids) {
                if (!grid.available) {
                    if (firstWarning) {
                        std::cout << std::endl;
                        firstWarning = false;
                    }
                    std::cout << "Grid " << grid.shortName
                              << " needed but not found on the system.";
                    if (!grid.packageName.empty()) {
                        std::cout << " Can be obtained from the "
                                  << grid.packageName << " package";
                        if (!grid.url.empty()) {
                            std::cout << " at " << grid.url;
                        }
                        std::cout << ", or on CDN";
                    } else if (!grid.url.empty()) {
                        std::cout << " Can be obtained at " << grid.url;
                    }
                    std::cout << std::endl;
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "Error in gridsNeeded(): " << e.what() << std::endl;
        }
    }
}

// ---------------------------------------------------------------------------

static void outputOperationSummary(
    const CoordinateOperationNNPtr &op, const DatabaseContextPtr &dbContext,
    CoordinateOperationContext::GridAvailabilityUse gridAvailabilityUse) {
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

    if (op->hasBallparkTransformation()) {
        std::cout << ", has ballpark transformation";
    }

    if (dbContext && getenv("PROJINFO_NO_GRID_CHECK") == nullptr) {
        try {
            auto setGrids = op->gridsNeeded(dbContext, false);
            for (const auto &grid : setGrids) {
                if (!grid.available) {
                    std::cout << ", at least one grid missing";
                    if (gridAvailabilityUse ==
                            CoordinateOperationContext::GridAvailabilityUse::
                                KNOWN_AVAILABLE &&
                        !grid.packageName.empty()) {
                        std::cout << " on the system, but available on CDN";
                    }
                    break;
                }
            }
        } catch (const std::exception &) {
        }
    }
    std::cout << std::endl;
}

// ---------------------------------------------------------------------------

static void outputOperations(
    DatabaseContextPtr dbContext, const std::string &sourceCRSStr,
    const std::string &targetCRSStr, const ExtentPtr &bboxFilter,
    CoordinateOperationContext::SpatialCriterion spatialCriterion,
    bool spatialCriterionExplicitlySpecified,
    CoordinateOperationContext::SourceTargetCRSExtentUse crsExtentUse,
    CoordinateOperationContext::GridAvailabilityUse gridAvailabilityUse,
    CoordinateOperationContext::IntermediateCRSUse allowUseIntermediateCRS,
    const std::vector<std::pair<std::string, std::string>> &pivots,
    const std::string &authority, bool usePROJGridAlternatives,
    bool showSuperseded, bool promoteTo3D, const OutputOptions &outputOpt,
    bool summary) {
    auto sourceObj =
        buildObject(dbContext, sourceCRSStr, "crs", "source CRS", false,
                    CoordinateOperationContext::IntermediateCRSUse::NEVER,
                    promoteTo3D, outputOpt.quiet);
    auto sourceCRS = nn_dynamic_pointer_cast<CRS>(sourceObj);
    if (!sourceCRS) {
        std::cerr << "source CRS string is not a CRS" << std::endl;
        std::exit(1);
    }
    auto nnSourceCRS = NN_NO_CHECK(sourceCRS);

    auto targetObj =
        buildObject(dbContext, targetCRSStr, "crs", "target CRS", false,
                    CoordinateOperationContext::IntermediateCRSUse::NEVER,
                    promoteTo3D, outputOpt.quiet);
    auto targetCRS = nn_dynamic_pointer_cast<CRS>(targetObj);
    if (!targetCRS) {
        std::cerr << "target CRS string is not a CRS" << std::endl;
        std::exit(1);
    }
    auto nnTargetCRS = NN_NO_CHECK(targetCRS);

    std::vector<CoordinateOperationNNPtr> list;
    size_t spatialCriterionPartialIntersectionResultCount = 0;
    bool spatialCriterionPartialIntersectionMoreRelevant = false;
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
        ctxt->setAllowUseIntermediateCRS(allowUseIntermediateCRS);
        ctxt->setIntermediateCRS(pivots);
        ctxt->setUsePROJAlternativeGridNames(usePROJGridAlternatives);
        ctxt->setDiscardSuperseded(!showSuperseded);
        ctxt->setAllowBallparkTransformations(outputOpt.ballparkAllowed);
        list = CoordinateOperationFactory::create()->createOperations(
            nnSourceCRS, nnTargetCRS, ctxt);
        if (!spatialCriterionExplicitlySpecified &&
            spatialCriterion == CoordinateOperationContext::SpatialCriterion::
                                    STRICT_CONTAINMENT) {
            try {
                ctxt->setSpatialCriterion(
                    CoordinateOperationContext::SpatialCriterion::
                        PARTIAL_INTERSECTION);
                auto list2 =
                    CoordinateOperationFactory::create()->createOperations(
                        nnSourceCRS, nnTargetCRS, ctxt);
                spatialCriterionPartialIntersectionResultCount = list2.size();
                if (spatialCriterionPartialIntersectionResultCount == 1 &&
                    list.size() == 1 &&
                    list2[0]->nameStr() != list[0]->nameStr()) {
                    spatialCriterionPartialIntersectionMoreRelevant = true;
                }
            } catch (const std::exception &) {
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "createOperations() failed with: " << e.what()
                  << std::endl;
        std::exit(1);
    }
    if (outputOpt.quiet && !list.empty()) {
        outputObject(dbContext, list[0], allowUseIntermediateCRS, outputOpt);
        return;
    }
    std::cout << "Candidate operations found: " << list.size() << std::endl;
    if (spatialCriterionPartialIntersectionResultCount > list.size()) {
        std::cout << "Note: using '--spatial-test intersects' would bring "
                     "more results ("
                  << spatialCriterionPartialIntersectionResultCount << ")"
                  << std::endl;
    } else if (spatialCriterionPartialIntersectionMoreRelevant) {
        std::cout << "Note: using '--spatial-test intersects' would bring "
                     "more relevant results."
                  << std::endl;
    }
    if (summary) {
        for (const auto &op : list) {
            outputOperationSummary(op, dbContext, gridAvailabilityUse);
        }
    } else {
        bool first = true;
        for (size_t i = 0; i < list.size(); ++i) {
            const auto &op = list[i];
            if (!first) {
                std::cout << std::endl;
            }
            first = false;
            std::cout << "-------------------------------------" << std::endl;
            std::cout << "Operation No. " << (i + 1) << ":" << std::endl
                      << std::endl;
            outputOperationSummary(op, dbContext, gridAvailabilityUse);
            std::cout << std::endl;
            outputObject(dbContext, op, allowUseIntermediateCRS, outputOpt);
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
    std::string objectKind;
    bool summary = false;
    ExtentPtr bboxFilter = nullptr;
    std::string area;
    bool spatialCriterionExplicitlySpecified = false;
    CoordinateOperationContext::SpatialCriterion spatialCriterion =
        CoordinateOperationContext::SpatialCriterion::STRICT_CONTAINMENT;
    CoordinateOperationContext::SourceTargetCRSExtentUse crsExtentUse =
        CoordinateOperationContext::SourceTargetCRSExtentUse::SMALLEST;
    bool buildBoundCRSToWGS84 = false;
    CoordinateOperationContext::GridAvailabilityUse gridAvailabilityUse =
        proj_context_is_network_enabled(nullptr)
            ? CoordinateOperationContext::GridAvailabilityUse::KNOWN_AVAILABLE
            : CoordinateOperationContext::GridAvailabilityUse::USE_FOR_SORTING;
    CoordinateOperationContext::IntermediateCRSUse allowUseIntermediateCRS =
        CoordinateOperationContext::IntermediateCRSUse::
            IF_NO_DIRECT_TRANSFORMATION;
    std::vector<std::pair<std::string, std::string>> pivots;
    bool usePROJGridAlternatives = true;
    std::string mainDBPath;
    std::vector<std::string> auxDBPath;
    bool guessDialect = false;
    std::string authority;
    bool identify = false;
    bool showSuperseded = false;
    bool promoteTo3D = false;

    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg == "-o" && i + 1 < argc) {
            outputSwithSpecified = true;
            i++;
            auto formats(split(argv[i], ','));
            for (auto format : formats) {
                if (ci_equal(format, "all")) {
                    outputOpt.PROJ5 = true;
                    outputOpt.WKT2_2019 = true;
                    outputOpt.WKT2_2015 = true;
                    outputOpt.WKT1_GDAL = true;
                    outputOpt.WKT1_ESRI = true;
                    outputOpt.PROJJSON = true;
                } else if (ci_equal(format, "default")) {
                    outputOpt.PROJ5 = true;
                    outputOpt.WKT2_2019 = true;
                    outputOpt.WKT2_2015 = false;
                    outputOpt.WKT1_GDAL = false;
                } else if (ci_equal(format, "PROJ")) {
                    outputOpt.PROJ5 = true;
                } else if (ci_equal(format, "-PROJ")) {
                    outputOpt.PROJ5 = false;
                } else if (ci_equal(format, "WKT_ALL") ||
                           ci_equal(format, "WKT-ALL")) {
                    outputOpt.WKT2_2019 = true;
                    outputOpt.WKT2_2015 = true;
                    outputOpt.WKT1_GDAL = true;
                } else if (ci_equal(format, "-WKT_ALL") ||
                           ci_equal(format, "-WKT-ALL")) {
                    outputOpt.WKT2_2019 = false;
                    outputOpt.WKT2_2015 = false;
                    outputOpt.WKT1_GDAL = false;
                } else if (ci_equal(format, "WKT2_2019") ||
                           ci_equal(format, "WKT2-2019") ||
                           ci_equal(format, "WKT2:2019") ||
                           /* legacy: undocumented */
                           ci_equal(format, "WKT2_2018") ||
                           ci_equal(format, "WKT2-2018") ||
                           ci_equal(format, "WKT2:2018")) {
                    outputOpt.WKT2_2019 = true;
                } else if (ci_equal(format, "WKT2_2019_SIMPLIFIED") ||
                           ci_equal(format, "WKT2-2019_SIMPLIFIED") ||
                           ci_equal(format, "WKT2:2019_SIMPLIFIED") ||
                           /* legacy: undocumented */
                           ci_equal(format, "WKT2_2018_SIMPLIFIED") ||
                           ci_equal(format, "WKT2-2018_SIMPLIFIED") ||
                           ci_equal(format, "WKT2:2018_SIMPLIFIED")) {
                    outputOpt.WKT2_2019_SIMPLIFIED = true;
                } else if (ci_equal(format, "-WKT2_2019") ||
                           ci_equal(format, "-WKT2-2019") ||
                           ci_equal(format, "-WKT2:2019") ||
                           /* legacy: undocumented */
                           ci_equal(format, "-WKT2_2018") ||
                           ci_equal(format, "-WKT2-2018") ||
                           ci_equal(format, "-WKT2:2018")) {
                    outputOpt.WKT2_2019 = false;
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
                } else if (ci_equal(format, "PROJJSON")) {
                    outputOpt.PROJJSON = true;
                } else if (ci_equal(format, "-PROJJSON")) {
                    outputOpt.PROJJSON = false;
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
        } else if (arg == "--area" && i + 1 < argc) {
            i++;
            area = argv[i];
        } else if (arg == "-k" && i + 1 < argc) {
            i++;
            std::string kind(argv[i]);
            if (ci_equal(kind, "crs") || ci_equal(kind, "srs")) {
                objectKind = "crs";
            } else if (ci_equal(kind, "operation")) {
                objectKind = "operation";
            } else if (ci_equal(kind, "ellipsoid")) {
                objectKind = "ellipsoid";
            } else if (ci_equal(kind, "datum")) {
                objectKind = "datum";
            } else if (ci_equal(kind, "ensemble")) {
                objectKind = "ensemble";
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
        } else if (arg == "--single-line") {
            outputOpt.singleLine = true;
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
            spatialCriterionExplicitlySpecified = true;
            if (ci_equal(value, "contains")) {
                spatialCriterion = CoordinateOperationContext::
                    SpatialCriterion::STRICT_CONTAINMENT;
            } else if (ci_equal(value, "intersects")) {
                spatialCriterion = CoordinateOperationContext::
                    SpatialCriterion::PARTIAL_INTERSECTION;
            } else {
                std::cerr << "Unrecognized value for option --spatial-test: "
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
            } else if (ci_equal(value, "known_available")) {
                gridAvailabilityUse = CoordinateOperationContext::
                    GridAvailabilityUse::KNOWN_AVAILABLE;
            } else {
                std::cerr << "Unrecognized value for option --grid-check: "
                          << value << std::endl;
                usage();
            }
        } else if (arg == "--pivot-crs" && i + 1 < argc) {
            i++;
            auto value(argv[i]);
            if (ci_equal(std::string(value), "always")) {
                allowUseIntermediateCRS =
                    CoordinateOperationContext::IntermediateCRSUse::ALWAYS;
            } else if (ci_equal(std::string(value),
                                "if_no_direct_transformation")) {
                allowUseIntermediateCRS = CoordinateOperationContext::
                    IntermediateCRSUse::IF_NO_DIRECT_TRANSFORMATION;
            } else if (ci_equal(std::string(value), "never")) {
                allowUseIntermediateCRS =
                    CoordinateOperationContext::IntermediateCRSUse::NEVER;
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
        } else if (arg == "--show-superseded") {
            showSuperseded = true;
        } else if (arg == "--lax") {
            outputOpt.strict = false;
        } else if (arg == "--hide-ballpark") {
            outputOpt.ballparkAllowed = false;
        } else if (ci_equal(arg, "--3d")) {
            promoteTo3D = true;
        } else if (ci_equal(arg, "--searchpaths")) {
#ifdef _WIN32
            constexpr char delim = ';';
#else
            constexpr char delim = ':';
#endif
            const auto paths = split(proj_info().searchpath, delim);
            for (const auto &path : paths) {
                std::cout << path << std::endl;
            }
            std::exit(0);
        } else if (ci_equal(arg, "--remote-data")) {
#ifdef CURL_ENABLED
            if (proj_context_is_network_enabled(nullptr)) {
                std::cout << "Status: enabled" << std::endl;
                std::cout << "URL: " << proj_context_get_url_endpoint(nullptr)
                          << std::endl;
            } else {
                std::cout << "Status: disabled" << std::endl;
                std::cout << "Reason: not enabled in proj.ini or "
                             "PROJ_NETWORK=ON not specified"
                          << std::endl;
            }
#else
            std::cout << "Status: disabled" << std::endl;
            std::cout << "Reason: build without Curl support" << std::endl;
#endif
            std::exit(0);
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

    if (bboxFilter && !area.empty()) {
        std::cerr << "ERROR: --bbox and --area are exclusive" << std::endl;
        std::exit(1);
    }

    DatabaseContextPtr dbContext;
    try {
        dbContext =
            DatabaseContext::create(mainDBPath, auxDBPath).as_nullable();
    } catch (const std::exception &e) {
        if (!mainDBPath.empty() || !auxDBPath.empty() || !area.empty()) {
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
        outputOpt.WKT2_2019 = true;
    }

    if (outputOpt.quiet &&
        (outputOpt.PROJ5 + outputOpt.WKT2_2019 +
         outputOpt.WKT2_2019_SIMPLIFIED + outputOpt.WKT2_2015 +
         outputOpt.WKT2_2015_SIMPLIFIED + outputOpt.WKT1_GDAL +
         outputOpt.WKT1_ESRI + outputOpt.PROJJSON) != 1) {
        std::cerr << "-q can only be used with a single output format"
                  << std::endl;
        usage();
    }

    if (!user_string.empty()) {
        try {
            auto obj(buildObject(dbContext, user_string, objectKind,
                                 "input string", buildBoundCRSToWGS84,
                                 allowUseIntermediateCRS, promoteTo3D,
                                 outputOpt.quiet));
            if (guessDialect) {
                auto dialect = WKTParser().guessDialect(user_string);
                std::cout << "Guessed WKT dialect: ";
                if (dialect == WKTParser::WKTGuessedDialect::WKT2_2019) {
                    std::cout << "WKT2_2019";
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
            outputObject(dbContext, obj, allowUseIntermediateCRS, outputOpt);
            if (identify) {
                auto crs = dynamic_cast<CRS *>(obj.get());
                if (crs) {
                    try {
                        auto res = crs->identify(
                            dbContext
                                ? AuthorityFactory::create(
                                      NN_NO_CHECK(dbContext), authority)
                                      .as_nullable()
                                : nullptr);
                        std::cout << std::endl;
                        std::cout
                            << "Identification match count: " << res.size()
                            << std::endl;
                        for (const auto &pair : res) {
                            const auto &identifiedCRS = pair.first;
                            const auto &ids = identifiedCRS->identifiers();
                            if (!ids.empty()) {
                                std::cout << *ids[0]->codeSpace() << ":"
                                          << ids[0]->code() << ": "
                                          << pair.second << " %" << std::endl;
                                continue;
                            }

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
                                continue;
                            }

                            auto compoundCRS = dynamic_cast<CompoundCRS *>(
                                identifiedCRS.get());
                            if (compoundCRS) {
                                const auto &components =
                                    compoundCRS->componentReferenceSystems();
                                if (components.size() == 2 &&
                                    !components[0]->identifiers().empty() &&
                                    !components[1]->identifiers().empty()) {
                                    const auto &idH =
                                        components[0]->identifiers().front();
                                    const auto &idV =
                                        components[1]->identifiers().front();
                                    if (*idH->codeSpace() ==
                                        *idV->codeSpace()) {
                                        std::cout << *idH->codeSpace() << ":"
                                                  << idH->code() << '+'
                                                  << idV->code() << ": "
                                                  << pair.second << " %"
                                                  << std::endl;
                                        continue;
                                    }
                                }
                            }

                            std::cout << "un-identified CRS: " << pair.second
                                      << " %" << std::endl;
                        }
                    } catch (const std::exception &e) {
                        std::cerr << "Identification failed: " << e.what()
                                  << std::endl;
                    }
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "buildObject failed: " << e.what() << std::endl;
            std::exit(1);
        }
    } else {

        if (!area.empty()) {
            assert(dbContext);
            try {
                if (area.find(' ') == std::string::npos &&
                    area.find(':') != std::string::npos) {
                    auto tokens = split(area, ':');
                    if (tokens.size() == 2) {
                        const std::string &areaAuth = tokens[0];
                        const std::string &areaCode = tokens[1];
                        bboxFilter = AuthorityFactory::create(
                                         NN_NO_CHECK(dbContext), areaAuth)
                                         ->createExtent(areaCode)
                                         .as_nullable();
                    }
                }
                if (!bboxFilter) {
                    auto authFactory = AuthorityFactory::create(
                        NN_NO_CHECK(dbContext), std::string());
                    auto res = authFactory->listAreaOfUseFromName(area, false);
                    if (res.size() == 1) {
                        bboxFilter =
                            AuthorityFactory::create(NN_NO_CHECK(dbContext),
                                                     res.front().first)
                                ->createExtent(res.front().second)
                                .as_nullable();
                    } else {
                        res = authFactory->listAreaOfUseFromName(area, true);
                        if (res.size() == 1) {
                            bboxFilter =
                                AuthorityFactory::create(NN_NO_CHECK(dbContext),
                                                         res.front().first)
                                    ->createExtent(res.front().second)
                                    .as_nullable();
                        } else if (res.empty()) {
                            std::cerr << "No area of use matching provided name"
                                      << std::endl;
                            std::exit(1);
                        } else {
                            std::cerr << "Several candidates area of use "
                                         "matching provided name :"
                                      << std::endl;
                            for (const auto &candidate : res) {
                                auto obj =
                                    AuthorityFactory::create(
                                        NN_NO_CHECK(dbContext), candidate.first)
                                        ->createExtent(candidate.second);
                                std::cerr << "  " << candidate.first << ":"
                                          << candidate.second << " : "
                                          << *obj->description() << std::endl;
                            }
                            std::exit(1);
                        }
                    }
                }
            } catch (const std::exception &e) {
                std::cerr << "Area of use retrieval failed: " << e.what()
                          << std::endl;
                std::exit(1);
            }
        }

        try {
            outputOperations(dbContext, sourceCRSStr, targetCRSStr, bboxFilter,
                             spatialCriterion,
                             spatialCriterionExplicitlySpecified, crsExtentUse,
                             gridAvailabilityUse, allowUseIntermediateCRS,
                             pivots, authority, usePROJGridAlternatives,
                             showSuperseded, promoteTo3D, outputOpt, summary);
        } catch (const std::exception &e) {
            std::cerr << "outputOperations() failed with: " << e.what()
                      << std::endl;
            std::exit(1);
        }
    }

    return 0;
}

//! @endcond
