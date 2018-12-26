/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_PJ_WKT2_SRC_WKT2_GENERATED_PARSER_H_INCLUDED
# define YY_PJ_WKT2_SRC_WKT2_GENERATED_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int pj_wkt2_debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    END = 0,
    T_PROJECTION = 258,
    T_DATUM = 259,
    T_SPHEROID = 260,
    T_PRIMEM = 261,
    T_UNIT = 262,
    T_AXIS = 263,
    T_PARAMETER = 264,
    T_GEODCRS = 265,
    T_LENGTHUNIT = 266,
    T_ANGLEUNIT = 267,
    T_SCALEUNIT = 268,
    T_TIMEUNIT = 269,
    T_ELLIPSOID = 270,
    T_CS = 271,
    T_ID = 272,
    T_PROJCRS = 273,
    T_BASEGEODCRS = 274,
    T_MERIDIAN = 275,
    T_BEARING = 276,
    T_ORDER = 277,
    T_ANCHOR = 278,
    T_CONVERSION = 279,
    T_METHOD = 280,
    T_REMARK = 281,
    T_GEOGCRS = 282,
    T_BASEGEOGCRS = 283,
    T_SCOPE = 284,
    T_AREA = 285,
    T_BBOX = 286,
    T_CITATION = 287,
    T_URI = 288,
    T_VERTCRS = 289,
    T_VDATUM = 290,
    T_GEOIDMODEL = 291,
    T_COMPOUNDCRS = 292,
    T_PARAMETERFILE = 293,
    T_COORDINATEOPERATION = 294,
    T_SOURCECRS = 295,
    T_TARGETCRS = 296,
    T_INTERPOLATIONCRS = 297,
    T_OPERATIONACCURACY = 298,
    T_CONCATENATEDOPERATION = 299,
    T_STEP = 300,
    T_BOUNDCRS = 301,
    T_ABRIDGEDTRANSFORMATION = 302,
    T_DERIVINGCONVERSION = 303,
    T_TDATUM = 304,
    T_CALENDAR = 305,
    T_TIMEORIGIN = 306,
    T_TIMECRS = 307,
    T_VERTICALEXTENT = 308,
    T_TIMEEXTENT = 309,
    T_USAGE = 310,
    T_DYNAMIC = 311,
    T_FRAMEEPOCH = 312,
    T_MODEL = 313,
    T_VELOCITYGRID = 314,
    T_ENSEMBLE = 315,
    T_MEMBER = 316,
    T_ENSEMBLEACCURACY = 317,
    T_DERIVEDPROJCRS = 318,
    T_BASEPROJCRS = 319,
    T_EDATUM = 320,
    T_ENGCRS = 321,
    T_PDATUM = 322,
    T_PARAMETRICCRS = 323,
    T_PARAMETRICUNIT = 324,
    T_BASEVERTCRS = 325,
    T_BASEENGCRS = 326,
    T_BASEPARAMCRS = 327,
    T_BASETIMECRS = 328,
    T_EPOCH = 329,
    T_COORDEPOCH = 330,
    T_COORDINATEMETADATA = 331,
    T_POINTMOTIONOPERATION = 332,
    T_GEODETICCRS = 333,
    T_GEODETICDATUM = 334,
    T_PROJECTEDCRS = 335,
    T_PRIMEMERIDIAN = 336,
    T_GEOGRAPHICCRS = 337,
    T_TRF = 338,
    T_VERTICALCRS = 339,
    T_VERTICALDATUM = 340,
    T_VRF = 341,
    T_TIMEDATUM = 342,
    T_TEMPORALQUANTITY = 343,
    T_ENGINEERINGDATUM = 344,
    T_ENGINEERINGCRS = 345,
    T_PARAMETRICDATUM = 346,
    T_AFFINE = 347,
    T_CARTESIAN = 348,
    T_CYLINDRICAL = 349,
    T_ELLIPSOIDAL = 350,
    T_LINEAR = 351,
    T_PARAMETRIC = 352,
    T_POLAR = 353,
    T_SPHERICAL = 354,
    T_VERTICAL = 355,
    T_TEMPORAL = 356,
    T_TEMPORALCOUNT = 357,
    T_TEMPORALMEASURE = 358,
    T_ORDINAL = 359,
    T_TEMPORALDATETIME = 360,
    T_NORTH = 361,
    T_NORTHNORTHEAST = 362,
    T_NORTHEAST = 363,
    T_EASTNORTHEAST = 364,
    T_EAST = 365,
    T_EASTSOUTHEAST = 366,
    T_SOUTHEAST = 367,
    T_SOUTHSOUTHEAST = 368,
    T_SOUTH = 369,
    T_SOUTHSOUTHWEST = 370,
    T_SOUTHWEST = 371,
    T_WESTSOUTHWEST = 372,
    T_WEST = 373,
    T_WESTNORTHWEST = 374,
    T_NORTHWEST = 375,
    T_NORTHNORTHWEST = 376,
    T_UP = 377,
    T_DOWN = 378,
    T_GEOCENTRICX = 379,
    T_GEOCENTRICY = 380,
    T_GEOCENTRICZ = 381,
    T_COLUMNPOSITIVE = 382,
    T_COLUMNNEGATIVE = 383,
    T_ROWPOSITIVE = 384,
    T_ROWNEGATIVE = 385,
    T_DISPLAYRIGHT = 386,
    T_DISPLAYLEFT = 387,
    T_DISPLAYUP = 388,
    T_DISPLAYDOWN = 389,
    T_FORWARD = 390,
    T_AFT = 391,
    T_PORT = 392,
    T_STARBOARD = 393,
    T_CLOCKWISE = 394,
    T_COUNTERCLOCKWISE = 395,
    T_TOWARDS = 396,
    T_AWAYFROM = 397,
    T_FUTURE = 398,
    T_PAST = 399,
    T_UNSPECIFIED = 400,
    T_STRING = 401,
    T_UNSIGNED_INTEGER_DIFFERENT_ONE_TWO_THREE = 402
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int pj_wkt2_parse (pj_wkt2_parse_context *context);

#endif /* !YY_PJ_WKT2_SRC_WKT2_GENERATED_PARSER_H_INCLUDED  */
