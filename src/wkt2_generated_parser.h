/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_PJ_WKT2_WKT2_GENERATED_PARSER_H_INCLUDED
# define YY_PJ_WKT2_WKT2_GENERATED_PARSER_H_INCLUDED
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
    T_ANCHOREPOCH = 279,
    T_CONVERSION = 280,
    T_METHOD = 281,
    T_REMARK = 282,
    T_GEOGCRS = 283,
    T_BASEGEOGCRS = 284,
    T_SCOPE = 285,
    T_AREA = 286,
    T_BBOX = 287,
    T_CITATION = 288,
    T_URI = 289,
    T_VERTCRS = 290,
    T_VDATUM = 291,
    T_GEOIDMODEL = 292,
    T_COMPOUNDCRS = 293,
    T_PARAMETERFILE = 294,
    T_COORDINATEOPERATION = 295,
    T_SOURCECRS = 296,
    T_TARGETCRS = 297,
    T_INTERPOLATIONCRS = 298,
    T_OPERATIONACCURACY = 299,
    T_CONCATENATEDOPERATION = 300,
    T_STEP = 301,
    T_BOUNDCRS = 302,
    T_ABRIDGEDTRANSFORMATION = 303,
    T_DERIVINGCONVERSION = 304,
    T_TDATUM = 305,
    T_CALENDAR = 306,
    T_TIMEORIGIN = 307,
    T_TIMECRS = 308,
    T_VERTICALEXTENT = 309,
    T_TIMEEXTENT = 310,
    T_USAGE = 311,
    T_DYNAMIC = 312,
    T_FRAMEEPOCH = 313,
    T_MODEL = 314,
    T_VELOCITYGRID = 315,
    T_ENSEMBLE = 316,
    T_MEMBER = 317,
    T_ENSEMBLEACCURACY = 318,
    T_DERIVEDPROJCRS = 319,
    T_BASEPROJCRS = 320,
    T_EDATUM = 321,
    T_ENGCRS = 322,
    T_PDATUM = 323,
    T_PARAMETRICCRS = 324,
    T_PARAMETRICUNIT = 325,
    T_BASEVERTCRS = 326,
    T_BASEENGCRS = 327,
    T_BASEPARAMCRS = 328,
    T_BASETIMECRS = 329,
    T_EPOCH = 330,
    T_COORDEPOCH = 331,
    T_COORDINATEMETADATA = 332,
    T_POINTMOTIONOPERATION = 333,
    T_VERSION = 334,
    T_GEODETICCRS = 335,
    T_GEODETICDATUM = 336,
    T_PROJECTEDCRS = 337,
    T_PRIMEMERIDIAN = 338,
    T_GEOGRAPHICCRS = 339,
    T_TRF = 340,
    T_VERTICALCRS = 341,
    T_VERTICALDATUM = 342,
    T_VRF = 343,
    T_TIMEDATUM = 344,
    T_TEMPORALQUANTITY = 345,
    T_ENGINEERINGDATUM = 346,
    T_ENGINEERINGCRS = 347,
    T_PARAMETRICDATUM = 348,
    T_AFFINE = 349,
    T_CARTESIAN = 350,
    T_CYLINDRICAL = 351,
    T_ELLIPSOIDAL = 352,
    T_LINEAR = 353,
    T_PARAMETRIC = 354,
    T_POLAR = 355,
    T_SPHERICAL = 356,
    T_VERTICAL = 357,
    T_TEMPORAL = 358,
    T_TEMPORALCOUNT = 359,
    T_TEMPORALMEASURE = 360,
    T_ORDINAL = 361,
    T_TEMPORALDATETIME = 362,
    T_NORTH = 363,
    T_NORTHNORTHEAST = 364,
    T_NORTHEAST = 365,
    T_EASTNORTHEAST = 366,
    T_EAST = 367,
    T_EASTSOUTHEAST = 368,
    T_SOUTHEAST = 369,
    T_SOUTHSOUTHEAST = 370,
    T_SOUTH = 371,
    T_SOUTHSOUTHWEST = 372,
    T_SOUTHWEST = 373,
    T_WESTSOUTHWEST = 374,
    T_WEST = 375,
    T_WESTNORTHWEST = 376,
    T_NORTHWEST = 377,
    T_NORTHNORTHWEST = 378,
    T_UP = 379,
    T_DOWN = 380,
    T_GEOCENTRICX = 381,
    T_GEOCENTRICY = 382,
    T_GEOCENTRICZ = 383,
    T_COLUMNPOSITIVE = 384,
    T_COLUMNNEGATIVE = 385,
    T_ROWPOSITIVE = 386,
    T_ROWNEGATIVE = 387,
    T_DISPLAYRIGHT = 388,
    T_DISPLAYLEFT = 389,
    T_DISPLAYUP = 390,
    T_DISPLAYDOWN = 391,
    T_FORWARD = 392,
    T_AFT = 393,
    T_PORT = 394,
    T_STARBOARD = 395,
    T_CLOCKWISE = 396,
    T_COUNTERCLOCKWISE = 397,
    T_TOWARDS = 398,
    T_AWAYFROM = 399,
    T_FUTURE = 400,
    T_PAST = 401,
    T_UNSPECIFIED = 402,
    T_STRING = 403,
    T_UNSIGNED_INTEGER_DIFFERENT_ONE_TWO_THREE = 404
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int pj_wkt2_parse (pj_wkt2_parse_context *context);

#endif /* !YY_PJ_WKT2_WKT2_GENERATED_PARSER_H_INCLUDED  */
