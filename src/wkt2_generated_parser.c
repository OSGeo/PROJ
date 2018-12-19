/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         pj_wkt2_parse
#define yylex           pj_wkt2_lex
#define yyerror         pj_wkt2_error
#define yydebug         pj_wkt2_debug
#define yynerrs         pj_wkt2_nerrs


/* Copy the first part of user declarations.  */


/******************************************************************************
 * Project:  PROJ
 * Purpose:  WKT2 parser grammar
 * Author:   Even Rouault, <even.rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2018 Even Rouault, <even.rouault at spatialys.com>
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

#include "wkt2_parser.h"




# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "wkt2_generated_parser.h".  */
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

/* Copy the second part of user declarations.  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  99
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3313

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  163
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  324
/* YYNRULES -- Number of rules.  */
#define YYNRULES  651
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1343

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   402

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     159,   161,     2,   153,   162,   154,   148,     2,     2,   150,
     151,   152,     2,     2,     2,     2,     2,     2,   155,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   149,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   156,     2,     2,     2,     2,     2,
     157,   158,     2,   160,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   206,   206,   206,   206,   206,   206,   206,   207,   207,
     207,   208,   211,   211,   212,   212,   212,   213,   216,   216,
     216,   216,   217,   217,   217,   217,   218,   218,   218,   219,
     219,   219,   223,   227,   227,   229,   231,   233,   233,   235,
     235,   237,   239,   241,   243,   245,   245,   247,   247,   249,
     249,   249,   249,   251,   251,   255,   257,   261,   262,   263,
     265,   265,   267,   269,   271,   273,   277,   278,   281,   282,
     284,   286,   288,   291,   292,   293,   295,   297,   299,   299,
     301,   304,   305,   307,   307,   312,   312,   314,   314,   316,
     318,   320,   324,   325,   328,   329,   330,   332,   332,   333,
     336,   337,   341,   342,   343,   347,   348,   349,   350,   352,
     356,   358,   361,   363,   366,   367,   368,   369,   370,   371,
     372,   373,   374,   375,   376,   377,   378,   379,   380,   383,
     384,   385,   386,   387,   388,   389,   390,   391,   392,   393,
     394,   395,   396,   397,   401,   403,   405,   409,   414,   416,
     418,   420,   422,   426,   431,   432,   434,   436,   438,   442,
     446,   448,   448,   450,   450,   455,   460,   461,   462,   463,
     464,   465,   466,   468,   470,   472,   472,   474,   474,   476,
     478,   480,   482,   484,   486,   490,   492,   496,   496,   499,
     502,   507,   507,   507,   507,   507,   510,   515,   515,   515,
     515,   518,   522,   523,   525,   541,   545,   546,   548,   548,
     550,   550,   556,   556,   558,   560,   567,   567,   567,   569,
     576,   577,   578,   579,   581,   588,   595,   596,   597,   599,
     601,   601,   601,   601,   601,   601,   601,   601,   601,   604,
     604,   604,   606,   606,   608,   608,   608,   610,   615,   621,
     626,   629,   632,   633,   634,   635,   636,   637,   638,   639,
     640,   643,   644,   645,   646,   647,   648,   649,   650,   653,
     654,   655,   656,   657,   658,   659,   660,   663,   664,   667,
     668,   669,   670,   675,   676,   677,   678,   679,   680,   681,
     682,   683,   686,   687,   688,   689,   692,   693,   694,   695,
     698,   699,   702,   703,   708,   709,   712,   713,   714,   715,
     718,   719,   720,   721,   722,   723,   724,   725,   726,   727,
     728,   729,   730,   731,   732,   733,   734,   735,   736,   737,
     738,   739,   740,   741,   742,   743,   744,   745,   746,   747,
     748,   749,   750,   751,   752,   753,   755,   758,   760,   762,
     764,   766,   768,   784,   784,   786,   794,   795,   797,   798,
     800,   808,   809,   811,   813,   815,   820,   821,   823,   825,
     827,   829,   831,   833,   835,   840,   844,   846,   849,   852,
     853,   854,   856,   857,   859,   864,   865,   867,   867,   869,
     873,   873,   873,   873,   875,   883,   891,   900,   910,   911,
     913,   915,   915,   917,   917,   920,   921,   925,   931,   932,
     933,   935,   935,   937,   939,   941,   945,   950,   950,   952,
     955,   956,   960,   965,   965,   965,   967,   969,   970,   971,
     972,   974,   977,   979,   983,   989,   989,   993,   993,   995,
    1000,  1001,  1002,  1003,  1005,  1011,  1011,  1013,  1015,  1019,
    1027,  1028,  1030,  1032,  1034,  1038,  1038,  1040,  1042,  1047,
    1048,  1050,  1052,  1054,  1056,  1060,  1060,  1062,  1068,  1075,
    1075,  1078,  1085,  1086,  1087,  1088,  1089,  1091,  1095,  1097,
    1099,  1099,  1103,  1108,  1108,  1108,  1112,  1117,  1117,  1119,
    1123,  1123,  1127,  1132,  1134,  1138,  1138,  1142,  1147,  1149,
    1153,  1154,  1155,  1156,  1157,  1159,  1159,  1161,  1164,  1166,
    1166,  1168,  1170,  1172,  1176,  1183,  1183,  1185,  1186,  1187,
    1188,  1190,  1192,  1196,  1201,  1203,  1206,  1211,  1215,  1221,
    1221,  1221,  1221,  1221,  1221,  1225,  1230,  1232,  1238,  1245,
    1255,  1261,  1263,  1265,  1270,  1275,  1281,  1281,  1283,  1286,
    1290,  1295,  1301,  1304,  1309,  1315,  1318,  1323,  1329,  1332,
    1337,  1343,  1344,  1345,  1346,  1347,  1349,  1351,  1353,  1353,
    1353,  1355,  1355,  1360,  1363,  1363,  1366,  1367,  1368,  1370,
    1374,  1375,  1377,  1379,  1379,  1380,  1380,  1381,  1381,  1381,
    1382,  1382,  1383,  1383,  1384,  1384,  1385,  1385,  1387,  1387,
    1388,  1388,  1389,  1389,  1390,  1390,  1394,  1401,  1402,  1403,
    1404,  1405,  1406,  1407,  1409,  1411,  1413,  1415,  1417,  1419,
    1421,  1423,  1425,  1427,  1432,  1439,  1440,  1441,  1442,  1443,
    1445,  1450,  1458,  1458,  1458,  1460,  1461,  1462,  1463,  1465,
    1467,  1472,  1478,  1480,  1487,  1487,  1489,  1490,  1491,  1492,
    1494,  1496
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of string\"", "error", "$undefined", "\"PROJECTION\"",
  "\"DATUM\"", "\"SPHEROID\"", "\"PRIMEM\"", "\"UNIT\"", "\"AXIS\"",
  "\"PARAMETER\"", "\"GEODCRS\"", "\"LENGTHUNIT\"", "\"ANGLEUNIT\"",
  "\"SCALEUNIT\"", "\"TIMEUNIT\"", "\"ELLIPSOID\"", "\"CS\"", "\"ID\"",
  "\"PROJCRS\"", "\"BASEGEODCRS\"", "\"MERIDIAN\"", "\"BEARING\"",
  "\"ORDER\"", "\"ANCHOR\"", "\"CONVERSION\"", "\"METHOD\"", "\"REMARK\"",
  "\"GEOGCRS\"", "\"BASEGEOGCRS\"", "\"SCOPE\"", "\"AREA\"", "\"BBOX\"",
  "\"CITATION\"", "\"URI\"", "\"VERTCRS\"", "\"VDATUM\"", "\"GEOIDMODEL\"",
  "\"COMPOUNDCRS\"", "\"PARAMETERFILE\"", "\"COORDINATEOPERATION\"",
  "\"SOURCECRS\"", "\"TARGETCRS\"", "\"INTERPOLATIONCRS\"",
  "\"OPERATIONACCURACY\"", "\"CONCATENATEDOPERATION\"", "\"STEP\"",
  "\"BOUNDCRS\"", "\"ABRIDGEDTRANSFORMATION\"", "\"DERIVINGCONVERSION\"",
  "\"TDATUM\"", "\"CALENDAR\"", "\"TIMEORIGIN\"", "\"TIMECRS\"",
  "\"VERTICALEXTENT\"", "\"TIMEEXTENT\"", "\"USAGE\"", "\"DYNAMIC\"",
  "\"FRAMEEPOCH\"", "\"MODEL\"", "\"VELOCITYGRID\"", "\"ENSEMBLE\"",
  "\"MEMBER\"", "\"ENSEMBLEACCURACY\"", "\"DERIVEDPROJCRS\"",
  "\"BASEPROJCRS\"", "\"EDATUM\"", "\"ENGCRS\"", "\"PDATUM\"",
  "\"PARAMETRICCRS\"", "\"PARAMETRICUNIT\"", "\"BASEVERTCRS\"",
  "\"BASEENGCRS\"", "\"BASEPARAMCRS\"", "\"BASETIMECRS\"", "\"EPOCH\"",
  "\"COORDEPOCH\"", "\"COORDINATEMETADATA\"", "\"POINTMOTIONOPERATION\"",
  "\"GEODETICCRS\"", "\"GEODETICDATUM\"", "\"PROJECTEDCRS\"",
  "\"PRIMEMERIDIAN\"", "\"GEOGRAPHICCRS\"", "\"TRF\"", "\"VERTICALCRS\"",
  "\"VERTICALDATUM\"", "\"VRF\"", "\"TIMEDATUM\"", "\"TEMPORALQUANTITY\"",
  "\"ENGINEERINGDATUM\"", "\"ENGINEERINGCRS\"", "\"PARAMETRICDATUM\"",
  "\"affine\"", "\"Cartesian\"", "\"cylindrical\"", "\"ellipsoidal\"",
  "\"linear\"", "\"parametric\"", "\"polar\"", "\"spherical\"",
  "\"vertical\"", "\"temporal\"", "\"temporalCount\"",
  "\"temporalMeasure\"", "\"ordinal\"", "\"temporalDateTime\"",
  "\"north\"", "\"northNorthEast\"", "\"northEast\"", "\"eastNorthEast\"",
  "\"east\"", "\"eastSouthEast\"", "\"southEast\"", "\"southSouthEast\"",
  "\"south\"", "\"southSouthWest\"", "\"southWest\"", "\"westSouthWest\"",
  "\"west\"", "\"westNorthWest\"", "\"northWest\"", "\"northNorthWest\"",
  "\"up\"", "\"down\"", "\"geocentricX\"", "\"geocentricY\"",
  "\"geocentricZ\"", "\"columnPositive\"", "\"columnNegative\"",
  "\"rowPositive\"", "\"rowNegative\"", "\"displayRight\"",
  "\"displayLeft\"", "\"displayUp\"", "\"displayDown\"", "\"forward\"",
  "\"aft\"", "\"port\"", "\"starboard\"", "\"clockwise\"",
  "\"counterClockwise\"", "\"towards\"", "\"awayFrom\"", "\"future\"",
  "\"part\"", "\"unspecified\"", "\"string\"", "\"unsigned integer\"",
  "'.'", "'E'", "'1'", "'2'", "'3'", "'+'", "'-'", "':'", "'T'", "'Z'",
  "'['", "'('", "']'", "')'", "','", "$accept", "input", "datum", "crs",
  "period", "number", "signed_numeric_literal_with_sign",
  "signed_numeric_literal", "unsigned_numeric_literal", "opt_sign",
  "approximate_numeric_literal", "mantissa", "exponent", "signed_integer",
  "exact_numeric_literal", "opt_period_unsigned_integer",
  "unsigned_integer", "sign", "colon", "hyphen", "datetime",
  "opt_24_hour_clock", "year", "month", "day", "_24_hour_clock",
  "opt_colon_minute_colon_second_time_zone_designator",
  "opt_colon_second_time_zone_designator", "time_designator", "hour",
  "minute", "second_time_zone_designator", "seconds_integer",
  "seconds_fraction", "time_zone_designator", "utc_designator",
  "local_time_zone_designator", "opt_colon_minute", "left_delimiter",
  "right_delimiter", "wkt_separator", "quoted_latin_text",
  "quoted_unicode_text", "opt_separator_scope_extent_identifier_remark",
  "no_opt_separator_scope_extent_identifier_remark",
  "opt_identifier_list_remark",
  "scope_extent_opt_identifier_list_opt_remark",
  "scope_extent_opt_identifier_list_remark",
  "usage_list_opt_identifier_list_remark", "usage", "usage_keyword",
  "scope", "scope_keyword", "scope_text_description", "extent",
  "extent_opt_identifier_list_remark", "area_description",
  "area_description_keyword", "area_text_description",
  "geographic_bounding_box", "geographic_bounding_box_keyword",
  "lower_left_latitude", "lower_left_longitude", "upper_right_latitude",
  "upper_right_longitude", "vertical_extent", "opt_separator_length_unit",
  "vertical_extent_keyword", "vertical_extent_minimum_height",
  "vertical_extent_maximum_height", "temporal_extent",
  "temporal_extent_keyword", "temporal_extent_start",
  "temporal_extent_end", "identifier",
  "opt_version_authority_citation_uri", "identifier_keyword",
  "authority_name", "authority_unique_identifier", "version",
  "authority_citation", "citation_keyword", "citation", "id_uri",
  "uri_keyword", "uri", "remark", "remark_keyword", "unit", "spatial_unit",
  "angle_or_length_or_parametric_or_scale_unit",
  "angle_or_length_or_parametric_or_scale_unit_keyword",
  "angle_or_length_or_scale_unit", "angle_or_length_or_scale_unit_keyword",
  "angle_unit", "opt_separator_identifier_list", "length_unit",
  "time_unit", "opt_separator_conversion_factor_identifier_list",
  "angle_unit_keyword", "length_unit_keyword", "time_unit_keyword",
  "unit_name", "conversion_factor",
  "coordinate_system_scope_extent_identifier_remark",
  "spatial_cs_scope_extent_identifier_remark",
  "opt_separator_spatial_axis_list_opt_separator_cs_unit_scope_extent_identifier_remark",
  "temporalcountmeasure_cs_scope_extent_identifier_remark",
  "ordinaldatetime_cs_scope_extent_identifier_remark",
  "opt_separator_ordinaldatetime_axis_list_scope_extent_identifier_remark",
  "cs_keyword", "spatial_cs_type", "temporalcountmeasure_cs_type",
  "ordinaldatetime_cs_type", "dimension", "spatial_axis",
  "temporalcountmeasure_axis", "ordinaldatetime_axis", "axis_keyword",
  "axis_name_abbrev",
  "axis_direction_opt_axis_order_spatial_unit_identifier_list",
  "north_south_options_spatial_unit",
  "clockwise_counter_clockwise_options_spatial_unit",
  "axis_direction_except_n_s_cw_ccw_opt_axis_spatial_unit_identifier_list",
  "axis_direction_except_n_s_cw_ccw_opt_axis_spatial_unit_identifier_list_options",
  "axis_direction_opt_axis_order_identifier_list", "north_south_options",
  "clockwise_counter_clockwise_options",
  "axis_direction_except_n_s_cw_ccw_opt_axis_identifier_list",
  "axis_direction_except_n_s_cw_ccw_opt_axis_identifier_list_options",
  "opt_separator_axis_time_unit_identifier_list",
  "axis_direction_except_n_s_cw_ccw_opt_axis_time_unit_identifier_list_options",
  "axis_direction_except_n_s_cw_ccw", "meridian", "meridian_keyword",
  "bearing", "bearing_keyword", "axis_order", "axis_order_keyword",
  "cs_unit", "datum_ensemble", "geodetic_datum_ensemble_without_pm",
  "datum_ensemble_member_list_ellipsoid_accuracy_identifier_list",
  "opt_separator_datum_ensemble_identifier_list",
  "vertical_datum_ensemble",
  "datum_ensemble_member_list_accuracy_identifier_list",
  "datum_ensemble_keyword", "datum_ensemble_name", "datum_ensemble_member",
  "opt_datum_ensemble_member_identifier_list",
  "datum_ensemble_member_keyword", "datum_ensemble_member_name",
  "datum_ensemble_member_identifier", "datum_ensemble_accuracy",
  "datum_ensemble_accuracy_keyword", "accuracy",
  "datum_ensemble_identifier", "dynamic_crs", "dynamic_crs_keyword",
  "frame_reference_epoch", "frame_reference_epoch_keyword",
  "reference_epoch", "opt_separator_deformation_model_id",
  "deformation_model_id", "opt_separator_identifier",
  "deformation_model_id_keyword", "deformation_model_name", "geodetic_crs",
  "static_geodetic_crs", "static_geographic_crs", "dynamic_geodetic_crs",
  "dynamic_geographic_crs",
  "opt_prime_meridian_coordinate_system_scope_extent_identifier_remark",
  "crs_name", "geodetic_crs_keyword", "geographic_crs_keyword",
  "geodetic_reference_frame_or_geodetic_datum_ensemble_without_pm",
  "ellipsoid", "opt_separator_length_unit_identifier_list",
  "ellipsoid_keyword", "ellipsoid_name", "semi_major_axis",
  "inverse_flattening", "prime_meridian", "prime_meridian_keyword",
  "prime_meridian_name", "irm_longitude_opt_separator_identifier_list",
  "geodetic_reference_frame_without_pm",
  "geodetic_reference_frame_keyword", "datum_name",
  "opt_separator_datum_anchor_identifier_list", "datum_anchor",
  "datum_anchor_keyword", "datum_anchor_description", "projected_crs",
  "projected_crs_keyword", "base_geodetic_crs", "base_static_geodetic_crs",
  "opt_separator_pm_ellipsoidal_cs_unit", "base_dynamic_geodetic_crs",
  "base_geodetic_crs_keyword", "base_crs_name", "ellipsoidal_cs_unit",
  "map_projection", "parameter_list_identifier_list",
  "map_projection_keyword", "map_projection_name", "map_projection_method",
  "map_projection_method_keyword", "map_projection_method_name",
  "map_projection_parameter", "opt_separator_param_unit_identifier_list",
  "parameter_keyword", "parameter_name", "parameter_value",
  "map_projection_parameter_unit", "vertical_crs", "static_vertical_crs",
  "dynamic_vertical_crs",
  "vertical_reference_frame_or_vertical_datum_ensemble",
  "vertical_cs_opt_geoid_model_id_scope_extent_identifier_remark",
  "opt_separator_cs_unit_opt_geoid_model_id_scope_extent_identifier_remark",
  "geoid_model_id", "geoid_model_keyword", "geoid_model_name",
  "vertical_crs_keyword", "vertical_reference_frame",
  "vertical_reference_frame_keyword", "engineering_crs",
  "engineering_crs_keyword", "engineering_datum",
  "engineering_datum_keyword", "parametric_crs", "parametric_crs_keyword",
  "parametric_datum", "parametric_datum_keyword", "temporal_crs",
  "temporal_crs_keyword", "temporal_datum",
  "opt_separator_temporal_datum_end", "temporal_datum_keyword",
  "temporal_origin", "temporal_origin_keyword",
  "temporal_origin_description", "calendar", "calendar_keyword",
  "calendar_identifier", "deriving_conversion",
  "parameter_or_parameter_file", "opt_separator_deriving_conversion_end",
  "deriving_conversion_keyword", "deriving_conversion_name",
  "operation_method", "operation_method_keyword", "operation_method_name",
  "operation_parameter", "parameter_unit",
  "length_or_angle_or_scale_or_time_or_parametric_unit",
  "length_or_angle_or_scale_or_time_or_parametric_unit_keyword",
  "operation_parameter_file", "parameter_file_keyword",
  "parameter_file_name", "derived_geodetic_crs", "derived_geographic_crs",
  "derived_projected_crs", "derived_projected_crs_keyword",
  "derived_crs_name", "base_projected_crs", "base_projected_crs_keyword",
  "derived_vertical_crs", "base_vertical_crs", "base_static_vertical_crs",
  "base_dynamic_vertical_crs", "base_vertical_crs_keyword",
  "derived_engineering_crs", "base_engineering_crs",
  "base_engineering_crs_keyword", "derived_parametric_crs",
  "base_parametric_crs", "base_parametric_crs_keyword",
  "derived_temporal_crs", "base_temporal_crs", "base_temporal_crs_keyword",
  "compound_crs", "compound_crs_other_components", "compound_crs_keyword",
  "compound_crs_name", "horizontal_crs", "geographic2D_crs",
  "metadata_coordinate_epoch", "coordinate_epoch_keyword",
  "coordinate_epoch", "coordinate_metadata", "coordinate_metadata_crs",
  "coordinate_metadata_keyword", "static_crs",
  "dynamic_crs_coordinate_metadata", "coordinate_operation",
  "opt_coordinate_operation_end", "operation_keyword", "operation_name",
  "source_crs", "source_crs_keyword", "target_crs", "target_crs_keyword",
  "interpolation_crs", "interpolation_crs_keyword", "operation_accuracy",
  "operation_accuracy_keyword", "point_motion_operation",
  "opt_point_motion_operation_end", "point_motion_keyword",
  "concatenated_operation", "step", "opt_concatenated_operation_end",
  "concatenated_operation_keyword", "step_keyword", "bound_crs",
  "bound_crs_keyword", "abridged_coordinate_transformation",
  "abridged_parameter_or_parameter_file",
  "opt_end_abridged_coordinate_transformation",
  "abridged_transformation_keyword", "abridged_transformation_parameter", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,    46,    69,
      49,    50,    51,    43,    45,    58,    84,    90,    91,    40,
      93,    41,    44
};
# endif

#define YYPACT_NINF -1109

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-1109)))

#define YYTABLE_NINF -606

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     782, -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109,
   -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109,
   -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109,
   -1109, -1109, -1109, -1109, -1109, -1109, -1109,    77, -1109, -1109,
   -1109,   367, -1109, -1109, -1109,   367, -1109, -1109, -1109, -1109,
   -1109,   367,   367, -1109,   367, -1109,   367, -1109,   367, -1109,
     367, -1109, -1109, -1109,   367, -1109,   367, -1109,   367, -1109,
     367, -1109,   367, -1109,   367, -1109,   367, -1109,   367, -1109,
   -1109, -1109,   367, -1109, -1109, -1109, -1109, -1109,   367, -1109,
     367, -1109,   367, -1109,   367, -1109,   367, -1109,   367, -1109,
   -1109, -1109,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,
     -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,
     681,   -44,   -44,   -44,   239, -1109, -1109,   175, -1109,   175,
   -1109,   175,   175, -1109,   175, -1109,   175,   175, -1109,   175,
     175,   175,   175,   175,   175,   175,   175,   175, -1109,   175,
   -1109,   175, -1109, -1109, -1109, -1109,   190, -1109, -1109, -1109,
   -1109, -1109,   207,   218,   223,   249, -1109, -1109, -1109, -1109,
     409, -1109,   175, -1109,   175,   175,   175, -1109,   175,   367,
   -1109,   933,   234,   467,   467,   788,   269,   209,   132,   654,
     295,   409,    56,   409,   391,   409,   299,   312,   409,   354,
     324, -1109, -1109, -1109,   509,   239,   239,   239,   406,   681,
   -1109, -1109, -1109, -1109, -1109, -1109, -1109,   644, -1109, -1109,
   -1109, -1109,   274,   283,   279,   788, -1109,   175, -1109,   175,
     367, -1109, -1109, -1109, -1109,   367,   175,   367,   175, -1109,
     175, -1109, -1109,   367,   175,   175,   175, -1109,   175,   175,
     175, -1109, -1109,   175,   367, -1109, -1109,   367,   175,   175,
   -1109,   175, -1109, -1109,   367, -1109,   175,   175,   367, -1109,
   -1109,   175,   175,   367, -1109, -1109,   175,   175,   367, -1109,
   -1109,   175,   175,   367, -1109, -1109,   175,   175,   367,   175,
     367, -1109, -1109,   175,   367, -1109, -1109,   367, -1109, -1109,
     367,   175, -1109, -1109, -1109, -1109,   367,   175,   175,   175,
   -1109,   175,   367,   409, -1109,   444,   644, -1109, -1109,   298,
     409,   155,   409,   409,   -44,   -44,   119,   400,   102,   414,
     -44,   119,   102,   414,   788,   409,   452,   483,   -44,   -44,
      99,   497,   414,   -44,   499, -1109,   499,   -44,   497,   414,
     -44,   497,   414,   -44,   497,   414,   -44, -1109, -1109,   591,
     129, -1109,   -44,   414,   -44,   -44,   -44,   289,   644,   406,
     505,   406,   493,   681, -1109,   644, -1109, -1109, -1109, -1109,
   -1109, -1109, -1109, -1109,   175,   175,   367, -1109,   367, -1109,
   -1109,   175,   175,   367,   175, -1109, -1109, -1109,   175,   175,
     175, -1109,   175,   367, -1109, -1109, -1109, -1109, -1109, -1109,
   -1109,   367,   409,   175,   367, -1109,   175,   367, -1109,   175,
     175,   409,   175, -1109,   175, -1109,   175, -1109,   409,   175,
     367, -1109,   175,   175,   175,   367,   409,   175,   175,   175,
     175, -1109,   409,   409,   175,   175,   409,   175,   175,   409,
     175,   175, -1109, -1109,   331, -1109,   409,   175, -1109,   409,
     175,   175,   175,   175,   175,   367,   175,   367,   175,   367,
     409,   279,   409,   175, -1109,   175,   367,   175, -1109,   175,
     367,   409, -1109,   515,   527,   -44,   -44, -1109, -1109,   499,
   -1109,  1202,   526,   499,   409,   234,   102,   587,   409,   644,
    1171, -1109,   497,   -44,   497,   -44,   222,   102, -1109,   497,
     448,   409,   497, -1109,   225, -1109,   -44,   409,   234,   497,
    1364, -1109,   497,   323, -1109, -1109, -1109, -1109,   497,   275,
   -1109,   497,   345, -1109,   497,    95, -1109, -1109,   644, -1109,
   -1109,   644, -1109, -1109, -1109,   497,   209,    83,   275,   960,
   -1109,   -44,   960, -1109,   -44,   826, -1109,   -44, -1109,   644,
   -1109,   505,    91,   -44,   520,   409,   -44, -1109,   175, -1109,
   -1109,   409, -1109,   409, -1109,   175, -1109,   409,   175, -1109,
     175, -1109,   175,   409, -1109, -1109, -1109,   367, -1109,   279,
     409, -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109,
   -1109, -1109, -1109, -1109, -1109,   175,   175,   175, -1109, -1109,
     175,   409, -1109,   175,   175,   175,   409,   409, -1109, -1109,
     175,   175,   367, -1109,   409, -1109, -1109,   175, -1109,   175,
     409,   175,   409,   175,   409,   409,   409,   409,   409,   409,
     409,   402,   439, -1109,   958,   409,   175, -1109, -1109, -1109,
   -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109,   175,   367,
     175,   367, -1109,   175,   367,   175,   367,   175,   367,   175,
     367,   175, -1109,   367,   175,   175,   175,   175,   175, -1109,
     175, -1109,   367,   175, -1109, -1109,   367, -1109,   175, -1109,
     367, -1109,   175,   527, -1109, -1109, -1109, -1109, -1109, -1109,
     278, -1109,   -44,   644, -1109,   386,   386,   386,   444, -1109,
     505,    70,   126,   409, -1109, -1109, -1109, -1109,   -44, -1109,
     444,   605, -1109,   386, -1109,   213, -1109, -1109, -1109, -1109,
   -1109, -1109, -1109, -1109,   644, -1109, -1109,   644,   644, -1109,
     449, -1109, -1109, -1109, -1109,   452,    93,   597,   453, -1109,
     -44,   549, -1109,   -44,   300, -1109,  1202,   377, -1109,  1202,
     390, -1109,   591, -1109,   471, -1109,   382, -1109,   345,    95,
      91,   -44,   888,   409,   -44,   499,   409,    92,   505, -1109,
     175, -1109,   175, -1109, -1109, -1109, -1109,   175,   175,   175,
     175,   788,   409,   175,   175, -1109, -1109, -1109,   367,   175,
   -1109, -1109, -1109,   175, -1109,   175,   175,   175,   409, -1109,
     464,   449, -1109,   958,   644, -1109,   409, -1109,   175, -1109,
     175, -1109,   175, -1109, -1109,   409,   175,   175,   175, -1109,
     409,   175,   175, -1109,   175,   175, -1109,   175, -1109, -1109,
     175, -1109,   409,   175,   175, -1109, -1109,   175,   175,   175,
     367, -1109,   175, -1109, -1109, -1109, -1109, -1109,   409,   175,
     409,   409,   409,   409,   135, -1109, -1109, -1109,    91,   409,
     -44,   229,   788,   621,   409,   409, -1109, -1109, -1109,   644,
   -1109, -1109, -1109, -1109, -1109,   443, -1109, -1109,   300, -1109,
     377, -1109, -1109, -1109,   377, -1109, -1109,  1202, -1109,  1202,
     591, -1109,   869,   409,   444, -1109, -1109, -1109,  1202,   -44,
     175,    91, -1109,   175,   175,   175,   175,   175, -1109,   175,
   -1109, -1109,   175, -1109, -1109, -1109, -1109, -1109,   367,   175,
   -1109,   175, -1109, -1109,  1154,   409,   175,   175,   175, -1109,
     175,   175,   175,   175, -1109,   175, -1109,   175, -1109, -1109,
     409, -1109, -1109,   175,   175,   175,   367,   175, -1109,   175,
     409, -1109,   175,   520,   367, -1109,   175, -1109,   632,   632,
     632, -1109,   327,   409,   788,   409,   -44, -1109,   632,   627,
   -1109, -1109,   261,   619,   602,   377, -1109, -1109, -1109, -1109,
    1202,   348,   409, -1109, -1109, -1109,  1035, -1109,   681, -1109,
     550, -1109,   409,   367,   -44,   920,   409, -1109,   175,   367,
     175,   367,   175,   367,   175,   175,   175, -1109,   175, -1109,
     175,   175,   503,   627, -1109,   175,   175, -1109,   175, -1109,
   -1109,   175, -1109,   175, -1109, -1109,   175,   409, -1109, -1109,
   -1109, -1109, -1109, -1109,   175, -1109,   367, -1109,    92,   175,
   -1109,   175,   175, -1109,   560, -1109,   -44, -1109,   -44,   675,
   -1109,   -44, -1109, -1109, -1109,   409,   788,   624, -1109, -1109,
     619,   602,   602, -1109,  1202, -1109, -1109,   409,   -44,   409,
     444, -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109,
   -1109, -1109, -1109, -1109,   367, -1109,   367,   175,   175, -1109,
     175,   175, -1109,   175,   175, -1109,   175, -1109, -1109,   175,
     175,   367,   175, -1109, -1109, -1109, -1109,   409, -1109,   175,
     175,   175,   -44,   -44, -1109, -1109,  1117,  1310, -1109,  1271,
     409,  1065, -1109, -1109,   -44,   602, -1109,   788,   409,   784,
     409,   409,   175,   175,   175, -1109, -1109, -1109, -1109, -1109,
   -1109, -1109,   175, -1109, -1109, -1109, -1109, -1109, -1109, -1109,
   -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109, -1109,
   -1109, -1109, -1109, -1109, -1109, -1109, -1109,   175,   175, -1109,
   -1109, -1109, -1109, -1109,   409, -1109,   175,   175,   175,   175,
     175,   175,   409, -1109,   175, -1109,   175, -1109,   175, -1109,
     175, -1109, -1109,   175,   367, -1109, -1109,   788,   409,   408,
     408,   507,   507, -1109,   371,   141,   409,   565,   408,   500,
     500, -1109,   353, -1109,   409, -1109, -1109,    92,   175, -1109,
   -1109, -1109,   175,   175, -1109,   175,   367,   175,   367, -1109,
   -1109,   175,   175, -1109,   175,   367,   175, -1109,   175,   175,
   -1109,   175,   175,   175, -1109,   175, -1109,   175, -1109,   175,
     175, -1109,   175, -1109,   175,   175, -1109,   175, -1109,   175,
   -1109,   409,   409, -1109, -1109,   371, -1109,  1202,   588, -1109,
     644, -1109, -1109,   371, -1109,  1202,   588, -1109, -1109, -1109,
     588, -1109, -1109, -1109,   103, -1109, -1109,   353, -1109, -1109,
   -1109,   353, -1109, -1109, -1109, -1109,   175, -1109,   175,   175,
     175,   175,   409,   175,   175,   409,   175,   175,   175,   175,
     175, -1109, -1109,   588, -1109,   229, -1109, -1109, -1109,   588,
   -1109, -1109, -1109, -1109, -1109, -1109, -1109,   175,   409,   175,
   -1109, -1109, -1109
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   423,   412,   401,   411,   173,   435,   452,   403,   480,
     483,   566,   614,   639,   642,   505,   498,   363,   541,   490,
     487,   495,   493,   582,   630,   402,   425,   436,   404,   424,
     481,   485,   484,   506,   491,   488,   496,     0,     4,     5,
       2,     0,    13,   353,   354,     0,    18,   390,   391,   392,
     393,     0,     0,     3,     0,    12,     0,    19,     0,    11,
       0,    20,   465,   466,     0,    14,     0,    21,     0,    15,
       0,    22,     0,    16,     0,    23,     0,    17,     0,    24,
      25,    26,     0,    27,    28,    29,    30,    31,     0,     7,
       0,     8,     0,     9,     0,    10,     0,     6,     0,     1,
      85,    86,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    90,   174,     0,   364,     0,
     400,     0,     0,   413,     0,   426,     0,     0,   453,     0,
       0,   427,     0,   427,     0,   427,     0,   500,   542,     0,
     567,     0,   583,   584,   598,   599,   585,   586,   601,   587,
     588,   589,   590,   591,   592,   593,   594,   595,   596,   597,
       0,   580,     0,   615,     0,     0,     0,   617,     0,     0,
      89,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    87,    88,   579,     0,     0,     0,     0,     0,     0,
      52,    32,    49,    50,    51,    53,    54,     0,   175,    33,
      34,    38,     0,    37,    47,     0,   176,   166,   368,     0,
       0,   445,   446,   376,   406,     0,     0,     0,     0,   405,
       0,   437,   438,     0,     0,     0,     0,   414,     0,   427,
       0,   456,   455,     0,     0,   550,   470,     0,     0,     0,
     469,     0,   546,   547,     0,   432,   202,   428,     0,   482,
     553,     0,     0,     0,   489,   556,     0,     0,     0,   494,
     559,     0,     0,     0,   512,   508,   202,   202,     0,   202,
       0,   499,   544,     0,     0,   571,   572,     0,   569,   570,
       0,     0,   568,   574,   575,   581,     0,     0,     0,     0,
     619,     0,     0,     0,    46,    39,     0,    45,    35,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   429,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   504,   503,     0,
       0,   501,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   616,     0,    41,    43,    40,    48,
     180,   183,   177,   178,   167,   170,     0,   172,     0,   165,
     372,     0,   358,     0,     0,   355,   360,   369,   366,     0,
       0,   378,   382,     0,   417,   229,   418,   399,   216,   217,
     218,     0,     0,     0,     0,   521,     0,     0,   447,     0,
       0,     0,     0,   415,   408,   422,     0,   461,     0,   202,
       0,   457,   202,     0,     0,     0,     0,     0,     0,   202,
     202,   433,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    62,   509,    60,   510,     0,   202,   513,     0,
       0,     0,     0,     0,    92,     0,    92,     0,    92,     0,
       0,   576,     0,     0,   524,     0,     0,     0,   650,    92,
       0,     0,    44,     0,     0,     0,     0,   357,   362,     0,
     361,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   394,     0,     0,     0,     0,     0,     0,   395,     0,
       0,     0,     0,   449,     0,   450,     0,     0,     0,     0,
       0,   467,     0,     0,   203,   430,   431,   486,     0,     0,
     492,     0,     0,   497,     0,     0,    56,    70,     0,    57,
      61,     0,   507,   502,   511,     0,     0,     0,     0,     0,
     561,     0,     0,   562,     0,     0,   563,     0,   560,   577,
     573,     0,     0,     0,     0,     0,     0,   618,   168,   171,
     181,     0,   184,     0,   374,   358,   373,     0,   358,   370,
     366,   365,     0,     0,   387,   388,   383,     0,   375,   379,
       0,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,     0,     0,     0,   398,   419,
       0,     0,   522,     0,     0,   440,     0,     0,   211,   210,
     202,   202,     0,   407,     0,   451,   462,     0,   454,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    60,     0,    71,     0,     0,     0,   186,   112,   145,
     148,   156,   160,   110,    93,    94,   100,   101,   105,     0,
      97,     0,   104,    97,     0,    97,     0,    97,     0,    97,
       0,    97,    96,     0,    92,     0,    92,     0,     0,   578,
       0,   536,     0,   625,   515,   516,     0,   525,   385,   640,
       0,   641,     0,     0,   179,   182,   359,   371,   356,   367,
       0,   396,     0,   380,   377,     0,     0,     0,    39,   538,
       0,     0,     0,     0,   397,   539,   410,   409,     0,   434,
      39,     0,   468,     0,   545,     0,   548,   551,   552,   554,
     555,   557,   558,    58,     0,    55,    80,     0,     0,    65,
      83,    67,    78,    79,   540,     0,     0,     0,     0,   103,
       0,     0,   129,     0,     0,   130,     0,     0,   131,     0,
       0,   132,     0,    95,     0,   564,     0,   565,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   169,
       0,   389,   385,   381,   244,   245,   246,   202,   202,   202,
     202,     0,     0,     0,   440,   209,   208,   448,     0,   441,
     443,   439,   214,     0,   463,   459,     0,   202,     0,    64,
      60,    83,    72,     0,     0,    82,     0,   108,    97,   106,
       0,   102,    97,    99,   113,     0,    97,    97,    97,   146,
       0,    97,    97,   149,     0,    97,   157,     0,   161,   162,
       0,    91,     0,   607,     0,   623,   629,   625,   625,    92,
       0,   624,     0,   386,   523,   634,   632,   633,     0,     0,
       0,     0,     0,     0,     0,   421,    36,   416,     0,     0,
       0,     0,     0,     0,     0,     0,   549,    59,    81,     0,
      66,    69,    84,   543,   107,     0,    98,   111,     0,   133,
       0,   134,   135,   144,     0,   136,   137,     0,   138,     0,
       0,   185,     0,     0,    39,   626,   627,   628,     0,     0,
       0,     0,   384,     0,     0,     0,   202,   517,   444,     0,
     442,   215,   202,   200,   198,   197,   199,   464,     0,   202,
     458,     0,    76,    68,     0,     0,   114,   115,   116,   117,
      97,    97,    97,    97,   150,     0,   158,   154,   163,   164,
       0,   621,   613,   607,   607,    92,     0,    92,   606,     0,
       0,   537,   385,     0,     0,   645,   646,   644,     0,     0,
       0,   420,     0,     0,     0,     0,     0,   460,     0,     0,
      75,   109,     0,     0,     0,     0,   139,   140,   141,   142,
       0,     0,     0,   159,   608,   609,     0,   610,     0,   612,
       0,   622,     0,     0,     0,     0,     0,   250,   220,     0,
      92,     0,   226,     0,   385,   517,   517,   514,   202,   204,
       0,   472,    77,     0,    73,   118,   119,   120,   121,   122,
     123,    97,   151,     0,   155,   153,    92,     0,   534,   529,
     530,   531,   532,   533,   385,   527,     0,   535,     0,     0,
     649,   646,   646,   643,     0,   219,     0,   224,     0,     0,
     225,     0,   520,   518,   519,     0,     0,     0,   471,    74,
       0,     0,     0,   143,     0,   611,   620,     0,     0,     0,
      39,   648,   647,   195,   192,   191,   194,   212,   193,   213,
     223,   352,   187,   189,     0,   188,     0,   220,    92,   251,
       0,     0,   228,   226,     0,   201,   202,   478,   476,    92,
      92,     0,   124,   125,   126,   127,   152,     0,   526,   206,
     635,   202,     0,     0,   222,   221,     0,     0,   227,     0,
       0,     0,   473,   475,     0,     0,   147,     0,     0,     0,
       0,     0,     0,   206,   253,   310,   311,   312,   313,   314,
     315,   316,   255,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   339,   340,   257,   259,   341,
     342,   343,   344,   345,     0,   252,   277,   304,   284,   286,
     288,   290,     0,   283,   300,   196,    92,   479,   385,   128,
     202,   528,   638,    92,     0,   631,   651,     0,     0,     0,
       0,     0,     0,   247,     0,     0,     0,     0,     0,     0,
       0,   249,     0,   474,     0,   207,   637,     0,   202,   205,
     347,   351,   202,   202,   254,   202,     0,   202,     0,   256,
     349,   202,   202,   258,   202,     0,   202,   260,   202,   202,
     278,   202,   202,   202,   305,   202,   248,   202,   285,   202,
     202,   287,   202,   289,   202,   202,   291,   202,   301,   202,
     477,     0,     0,   261,   268,     0,   265,     0,     0,   267,
       0,   269,   276,     0,   273,     0,     0,   275,   279,   282,
       0,   280,   306,   309,     0,   307,   292,     0,   294,   295,
     296,     0,   298,   299,   302,   303,   635,   190,   202,   202,
       0,   202,     0,   202,   202,     0,   202,   202,   202,   202,
     202,   636,   264,     0,   262,     0,   266,   350,   272,     0,
     270,   348,   274,   281,   308,   293,   297,   202,     0,   202,
     263,   346,   271
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1109, -1109, -1109,  -205,  -212,  -178, -1109,   -57,  -184,   344,
   -1109, -1109, -1109, -1109, -1109, -1109,  -203,  -310,  -558,    28,
    -673,  -542, -1109, -1109, -1109, -1109, -1109, -1109, -1109,  -493,
    -153, -1109, -1109, -1109,  -757, -1109, -1109,  -136,   -43,  1622,
     897,  2179, -1109,  -414,  -678,  -541, -1109, -1109,   -59, -1109,
   -1109,   -53, -1109, -1109, -1109,   -58,  -187, -1109, -1109,  -707,
   -1109, -1109, -1109, -1109, -1109,  -679, -1109, -1109, -1109, -1109,
    -675, -1109, -1109, -1109,     0, -1109, -1109, -1109, -1109, -1109,
     217, -1109, -1109,  -421, -1109, -1109,  -282, -1109, -1109,  -962,
   -1109, -1109, -1109, -1109,  -844,  1726,  -289, -1090,  -436, -1109,
   -1109, -1109,  -790,  -905,   140, -1109,  -388, -1109, -1109,  -392,
    -280,   196, -1109, -1109,  -208,  -880, -1109,  -339,  -854,  -838,
   -1109,  -709,  -489, -1109, -1109, -1109, -1109,  -488, -1109, -1109,
   -1109, -1109,  -847,  -490, -1109,  -494, -1109, -1108, -1109,  -333,
   -1109,   736,  -351,  -253,   745,  -363,     7,  -188,  -287,   170,
   -1109, -1109, -1109,   264, -1109,  -150, -1109,  -157, -1109, -1109,
   -1109, -1109, -1109, -1109,  -744, -1109, -1109, -1109,   647,   -36,
     649,   -29,  -231,   -46, -1109,   583,   -93,    57, -1109, -1109,
   -1109, -1109, -1109,    73, -1109, -1109, -1109,     6, -1109,   651,
     -48, -1109, -1109, -1109,   -27, -1109,  -141, -1109,    -6, -1109,
   -1109,   189,   -73,  -306,   294, -1109, -1109, -1109, -1109, -1109,
   -1109, -1109,  -319,  -693,  -819, -1109,   451,   691,   704,   485,
     311, -1109,  -299, -1109, -1109,   466,    10, -1109,   -24,   635,
      19, -1109,   -78,   469,    22, -1109,   -96,  -296,    16, -1109,
   -1109,   480, -1109, -1109, -1109, -1109, -1109,   655,  -687,  -263,
   -1109, -1109,  -511, -1109, -1109,  -714, -1109, -1109, -1109,  -699,
   -1109, -1109,   721,   724,   726, -1109, -1109, -1109, -1109,   731,
   -1109, -1109, -1109, -1109,   733, -1109, -1109,   734, -1109, -1109,
     743, -1109, -1109,   750, -1109, -1109, -1109, -1109, -1109, -1109,
   -1109, -1109, -1109, -1109, -1109, -1109, -1109,   874,  -151, -1109,
     -86,   475, -1109,   133, -1109, -1109, -1109,  -842, -1109, -1109,
     -25,   875, -1109,  -981,  -430, -1109,  -875, -1109, -1109, -1109,
   -1109,  -247, -1109,  -128
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   217,   576,   219,   804,   220,   791,
     221,   222,   376,   377,   223,   317,   224,   225,   814,   538,
     453,   539,   454,   642,   810,   540,   739,   880,   541,   740,
     813,   933,   934,  1023,   741,   742,   743,   815,   102,   203,
     344,   130,   842,   550,   654,   749,   655,   656,   657,   658,
     659,   660,   661,   825,   935,   662,   663,   664,   830,   665,
     666,   834,   945,  1033,  1117,   667,   992,   668,   837,   947,
     669,   670,   840,   950,   439,   320,    41,   127,   227,   384,
     385,   386,   571,   387,   388,   573,   672,   673,  1091,  1233,
    1093,  1094,   927,   928,   797,   345,   621,  1095,  1138,   798,
     622,  1096,   803,   922,   407,   408,  1055,   409,   410,  1060,
     411,   605,   606,   607,   787,  1008,  1010,  1012,  1009,  1100,
    1184,  1234,  1243,  1185,  1250,  1192,  1258,  1263,  1193,  1268,
    1216,  1254,  1186,  1235,  1236,  1244,  1245,  1237,  1238,  1098,
      42,   234,   322,   490,   256,   323,   235,   129,   229,   494,
     230,   398,   580,   392,   393,   577,   575,   244,   237,   402,
     403,   590,   498,   586,   776,   587,   782,    46,    47,    48,
      49,    50,   412,   131,    51,    52,   245,   394,   511,    54,
     134,   248,   424,   413,   414,   610,   792,   239,    56,   136,
     191,   267,   268,   442,    57,    58,   240,   241,   713,   242,
     243,   419,   800,    59,   428,    60,   139,   253,   254,   432,
     429,   874,   682,   627,   805,   929,    61,    62,    63,   259,
     436,  1068,  1110,  1111,  1198,    64,   260,    66,    67,    68,
     271,    70,    71,    72,   276,    74,    75,    76,   281,   198,
      78,   287,   288,   456,   289,   290,   459,   855,   683,   973,
     417,   613,   475,   476,   688,   684,  1044,  1045,  1046,   685,
     686,   962,    79,    80,    81,    82,   149,   293,   294,    83,
     261,   262,   263,   264,    84,   272,   273,    85,   277,   278,
      86,   282,   283,    87,   470,    88,   151,   301,   302,   305,
     306,   472,    89,   170,    90,   171,   172,   856,   903,    92,
     174,   178,   179,   311,   312,   955,   956,   849,   850,    93,
     773,   857,    95,   858,  1140,    96,   690,    97,    98,   479,
     966,  1006,   480,   967
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      40,   247,   103,   218,   313,   378,    55,    45,   104,   105,
      65,   106,   316,   107,   314,   108,    77,   109,   430,    69,
     916,   110,    73,   111,   161,   112,   236,   113,   488,   114,
     426,   115,   258,   116,   391,   117,   175,   176,   860,   118,
     487,   318,   160,   246,   826,   119,   250,   120,   644,   121,
     680,   122,   553,   123,   556,   124,   881,    53,   847,   132,
     957,   435,   137,   569,   140,   565,   142,  1079,   144,  1018,
     146,   469,   827,   848,     1,   831,   828,    99,   844,   832,
     919,   852,   835,   843,   153,   959,   738,     1,  1003,   838,
     238,   155,  1092,   156,   846,   193,   159,   195,  1021,   733,
     427,   421,   125,  1246,  1246,  1092,  1251,  1255,   404,  1260,
       5,  1265,  1265,   379,  1269,  1011,  1013,  1087,   405,   647,
       5,    19,   752,     1,   755,  1253,   758,   270,   761,   681,
     763,    12,   404,   795,    10,   251,   209,   399,   796,   233,
     415,   382,   795,    17,    15,    34,     5,   796,   653,    26,
     423,   433,     5,    29,  1036,  1087,   452,   252,     5,    17,
       2,  1106,    26,  1231,   295,   471,    29,  1309,   481,    24,
       4,   296,   482,   298,  1097,  1314,   299,   980,   937,    17,
     285,   917,    33,   406,    31,    32,  1020,   324,   953,  1319,
     266,  1089,   325,  1320,   327,   430,   257,   286,    26,   793,
     330,   335,    29,   954,  1318,  1013,   938,   406,   582,   940,
     939,   338,   965,   941,   339,   942,   228,   390,  1002,   943,
    1101,   343,  1024,  1104,   952,   347,     1,   948,   231,  1089,
     350,   629,  1200,   400,   427,   353,   795,   232,   420,   435,
     356,   796,     5,   249,   811,   359,  1271,   362,    10,  1242,
    1242,   364,  1249,   469,   365,   879,   469,   366,  1015,   559,
     765,  1121,   767,   368,  1204,   583,  1069,   859,   877,   373,
    1062,   468,   779,  1016,     2,  1025,   616,   884,   233,   177,
    1187,   886,  1194,     2,     4,   889,   891,   892,  1119,   466,
     895,   896,   650,     4,   898,   228,   589,  1203,    31,    32,
    1077,    26,  1228,  1026,  1028,    29,  1051,  1027,  1029,  1030,
    1031,  1049,     5,  1308,   651,   652,  1311,     5,   265,   462,
     463,  1313,   696,     9,  1316,   698,   647,  1050,  1317,     5,
     380,   381,  1142,  1143,   737,   641,   427,   180,   643,   228,
      19,    16,     6,   485,     5,   486,   440,   257,    15,   614,
     491,     8,  -600,   651,   652,   618,   679,    22,    10,   619,
     499,  1337,   284,   285,    34,   681,   633,  1339,   500,  -602,
       5,   503,   280,    30,   505,  1231,  1090,   703,  1083,   233,
    -603,  1102,  1084,  1085,  1086,  -604,    33,   516,     5,  1108,
      20,  1112,   520,  1231,     5,  1113,  1114,  1115,   378,   986,
     987,   988,   989,   647,    27,   646,    28,     5,    31,    32,
     378,  -605,    21,   780,    35,  1083,   647,    10,   292,  1084,
    1085,  1086,   551,   315,   554,     5,   557,   211,  1230,   487,
    1231,   652,   -42,   563,   806,   907,    36,   566,   233,   816,
    1088,   643,    17,   488,   125,   210,   211,   310,   212,   213,
     214,   215,   216,   674,  1224,   618,   676,   401,    21,   619,
    1199,  1202,   415,   275,   819,     5,   823,    31,    32,   823,
       5,     1,   823,   649,   650,   823,     7,  1088,   823,   647,
     692,  1338,    36,   649,   650,   536,   231,   537,   443,   574,
    1073,   446,   427,   579,   449,   232,   651,   652,   788,   789,
     783,  1239,   473,   737,   477,   675,   651,   652,   677,  1261,
     620,   678,   615,   405,  1083,   807,     5,     5,  1084,  1085,
    1086,  1240,  1231,   233,     5,   100,   101,    17,  1240,  1231,
     474,   809,   438,   634,   643,   812,   784,   785,   786,   445,
     478,   997,   448,   999,   702,   451,    26,   380,   636,   671,
      29,   640,   671,   461,   638,   671,   -63,  1038,   537,   452,
     381,  1039,  1040,  1041,  1042,   689,     5,  1083,  1007,   201,
     202,  1084,  1085,  1086,  1087,   647,  1088,     5,   833,   718,
     650,   836,     5,   303,   304,  1230,   647,  1231,   390,   648,
     649,   650,   964,   536,   378,  1083,  1057,   215,   216,  1084,
    1085,  1086,   651,   652,   735,     5,   823,   866,   823,   258,
     643,   812,   823,   651,   652,   653,   747,   841,   750,  1043,
     537,   753,  1075,   756,   737,   759,   648,   762,   923,  1088,
     764,  1083,   924,   925,   926,  1084,  1085,  1086,  1087,   771,
    1007,     5,   608,   774,   611,   584,   585,   777,  1089,   617,
     647,   790,   624,   648,   649,   650,   652,  1088,   -71,   375,
    1107,   882,   632,   -71,   -71,   -71,   228,   390,   635,   737,
     734,   637,   651,   652,   639,   878,   932,   651,   652,   653,
     307,   308,   309,  1007,  1125,   645,   964,   817,   921,    10,
     821,     3,     5,  1088,   820,  1132,  1133,   452,   936,     6,
     568,   647,  1034,   823,   648,   649,   650,  1208,     8,  1124,
     233,  1128,  1089,   737,    17,     9,   631,   794,    11,   944,
    1103,   946,   979,  1247,   255,  1264,  1264,  1259,   651,   652,
     653,   643,  1266,    16,  1109,   808,    43,   125,   210,    31,
      32,   212,   213,   214,    18,    44,   818,    20,   822,    22,
     699,   822,  1063,  1064,   822,   870,   578,   822,   960,    25,
     822,    27,   141,    28,   143,    30,   145,   152,   147,   154,
     378,    35,   671,   257,   210,   853,  1022,   212,   213,   214,
     215,   216,  1223,   297,   736,   799,     1,     2,   869,  1226,
     921,   210,     3,  1037,   212,   213,   214,     4,   920,     5,
       6,     5,   994,   995,  1081,  1082,     7,   908,   625,     8,
     647,   157,  1032,   648,   649,   650,     9,    10,   464,    11,
     643,    12,   905,   906,   158,   434,    13,   845,    14,   689,
     630,    15,  1196,   465,    16,   300,   467,   651,   652,   653,
     457,   162,    17,     5,   163,    18,   164,    19,    20,    21,
      22,   165,   647,   166,   167,   648,   649,   650,    23,    24,
      25,    26,    27,   168,    28,    29,    30,    31,    32,    33,
     169,    34,    35,    36,    91,    94,  1321,  1052,   427,   651,
     652,   653,   921,     0,     0,   976,     5,     0,   822,     0,
     822,     0,     0,     0,   822,   647,  1116,   427,   648,   649,
     650,     0,   671,     0,     0,     5,     0,   681,     0,     0,
       0,   951,   845,   998,   647,     0,     0,   648,   649,   650,
       0,  1004,   651,   652,   653,     0,   681,     0,     0,   427,
       0,   845,     0,     0,     0,   210,   211,     5,   212,   213,
     214,   651,   652,   653,     0,     0,   647,     0,     0,   648,
     649,   650,     0,   921,     0,     0,     0,     0,   681,     0,
    1048,     0,     0,     0,     0,     0,  1056,     0,  1058,     0,
    1061,     0,  1014,   651,   652,   653,     0,     5,     0,     0,
       0,     0,     0,     0,   416,   822,   647,     0,   422,   648,
     649,   650,     0,     0,     0,     0,   671,   437,     0,     0,
       0,     0,     0,  1078,   444,   671,     0,   447,     0,     0,
     450,     0,    16,   651,   652,   653,     0,     0,   460,     0,
       0,     0,     0,   921,   181,     0,   182,     0,   183,   184,
       0,   185,     0,   186,   187,     0,   188,   189,   190,   192,
     190,   194,   190,   196,   197,     0,   199,     0,   200,     0,
       0,  1122,     5,  1123,   671,     0,     0,     0,     0,   671,
       0,   647,     0,     0,   648,   649,   650,   671,  1134,   204,
       0,   205,   206,   207,     0,   208,     0,  1312,   845,   125,
     210,   211,     5,   212,   213,   214,   215,   216,   651,   652,
     653,   647,     0,     0,   648,   649,   650,     0,     0,  1310,
       0,  1107,     0,     0,     0,   210,     0,  1315,   212,   213,
     214,   215,   216,   735,     0,   736,     0,     0,   651,   652,
     653,     0,     0,     0,   319,     0,   321,     0,     0,     0,
       0,   671,     0,   326,     0,   328,     0,   329,     0,   671,
       0,   331,   332,   333,     0,   334,   190,   336,     0,     0,
     337,     0,     0,     0,     0,   340,   341,     0,   342,     0,
       0,  1227,     0,     0,   346,     0,     0,     0,   348,   349,
       0,     0,     0,   351,   352,     0,     0,     0,   354,   355,
       0,     0,     0,     0,     0,     0,   360,     0,     0,     0,
     363,     0,     0,  1277,     0,  1280,     0,     0,   367,     0,
       0,     0,  1285,     0,   369,   370,   371,     0,   372,  1232,
    1232,  1241,  1241,     0,  1248,  1252,     0,  1257,  1232,  1262,
    1262,     0,  1267,  1144,  1145,  1146,  1147,  1148,  1149,  1150,
    1151,  1152,  1153,  1154,  1155,  1156,  1157,  1158,  1159,  1160,
    1161,  1162,  1163,  1164,  1165,  1166,  1167,  1168,  1169,  1170,
    1171,  1172,  1173,  1174,  1175,  1176,  1177,  1178,  1179,  1180,
    1181,  1182,  1183,   591,   592,   593,   594,   595,   596,   597,
     598,   599,   600,   601,   602,   603,   604,     0,     0,     0,
       0,   483,   484,     0,     0,     0,     0,     0,   321,   489,
       0,   492,     0,     0,     0,   493,   495,   496,     0,   497,
       0,   210,   211,     0,   212,   213,   214,   215,   216,     0,
     502,   736,     0,   504,     0,     0,   506,   507,     0,   509,
       0,   510,     0,   512,     0,     0,   514,     0,     0,     0,
     518,   519,     0,     0,   522,   523,     0,     0,     0,     0,
       0,   528,   529,     0,   531,   532,     0,   534,   535,   210,
     211,     0,   212,   213,   214,   215,   216,   545,   546,   547,
     548,   549,     0,   552,     0,   555,     0,     0,     0,     0,
     561,     0,   562,     0,   564,     0,   555,  1188,  1145,  1146,
    1147,  1148,  1149,  1150,  1151,  1189,  1153,  1154,  1155,  1156,
    1157,  1158,  1159,  1160,  1161,  1162,  1163,  1164,  1165,  1166,
    1167,  1168,  1169,  1170,  1171,  1172,  1173,  1174,  1175,  1176,
    1190,  1191,  1179,  1180,  1181,  1182,  1183,  1145,  1146,  1147,
    1148,  1149,  1150,  1151,     0,  1153,  1154,  1155,  1156,  1157,
    1158,  1159,  1160,  1161,  1162,  1163,  1164,  1165,  1166,  1167,
    1168,  1169,  1170,  1171,  1172,  1173,  1174,  1175,  1176,     0,
       0,  1179,  1180,  1181,  1182,  1183,   591,   592,   593,   594,
     595,   596,   597,   598,   599,   693,     0,     0,     0,     0,
       0,     0,   489,     0,     0,   489,     0,   493,     0,   700,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   705,   706,   707,     0,     0,   708,     0,     0,
     710,   711,   712,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   720,     0,   721,     0,   723,     0,
     725,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   745,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   746,     0,   748,     0,     0,
     751,     0,   754,     0,   757,     0,   760,     0,   760,     0,
       0,   555,   766,   555,   768,   769,     0,   770,     0,     0,
     772,     0,     0,     0,     0,   775,     0,     0,     0,   778,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   700,     0,   775,
       0,     0,     0,     0,     0,     0,     0,   864,     0,     0,
     868,   712,     0,     0,     0,     0,   871,     0,     0,     0,
     872,     0,   873,   721,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   760,     0,   885,     0,   760,
       0,     0,     0,   888,   890,   760,     0,     0,   894,   760,
       0,   897,   760,     0,   899,     0,     0,   900,     0,     0,
     902,   904,     0,     0,   772,   772,   555,     0,     0,   909,
       0,     0,     0,     0,     0,     0,   911,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   963,     0,     0,
     968,   969,   970,   269,   972,   274,   974,   279,     0,     0,
     291,     0,     0,     0,     0,     0,     0,     0,   978,     0,
       0,     0,     0,   982,   983,   984,     0,   985,   760,   760,
     760,     0,   990,     0,   991,     0,     0,     0,     0,     0,
     902,   902,   996,     0,   555,     0,  1000,     0,     0,   775,
       0,     0,     0,  1005,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1054,     0,   555,     0,  1059,
       0,   775,   972,   972,     0,     0,     0,  1066,  1067,     0,
       0,     0,  1070,  1071,     0,  1072,     0,     0,   760,     0,
    1074,     0,     0,   555,     0,   374,     0,     0,     0,     0,
       0,   775,   389,     0,   395,   396,  1080,     0,  1005,  1005,
       0,     0,     0,     0,     0,     0,     0,   425,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1054,   555,     0,  1126,  1127,     0,
    1059,  1129,     0,     0,     0,     0,  1131,   555,     0,  1135,
       0,     0,   357,   358,     0,   361,  1137,  1139,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   501,     0,     0,     0,     0,  1207,
    1137,  1209,     0,   508,     0,     0,     0,     0,     0,  1210,
     513,     0,     0,     0,     0,     0,     0,     0,   521,     0,
       0,     0,     0,     0,   526,   527,     0,     0,   530,     0,
       0,   533,     0,     0,  1211,  1212,     0,     0,   542,     0,
       0,   544,     0,  1214,  1215,  1217,  1218,  1219,  1220,     0,
       0,  1222,   558,   555,   560,   775,     0,     0,     0,     0,
     555,     0,     0,   567,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   581,     0,     0,     0,
     588,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1275,   623,  1278,     0,     0,     0,     0,   628,
       0,  1283,     0,  1286,     0,     0,     0,     0,  1290,     0,
       0,     0,  1294,     0,     0,   515,  1297,     0,   517,     0,
       0,  1301,     0,     0,     0,   524,   525,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   543,     0,     0,     0,   691,     0,     0,
       0,     0,     0,   694,     0,   695,     0,     0,     0,   697,
       0,     0,     0,  1139,     0,   701,  1323,  1325,     0,     0,
       0,  1329,   704,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   709,     0,     0,     0,     0,   714,   715,
       0,     0,     0,     0,     0,     0,   719,     0,     0,     0,
       0,     0,   722,     0,   724,     0,   726,   727,   728,   729,
     730,   731,   732,     0,     0,     0,     0,   744,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   126,   128,     0,     0,   133,   135,     0,   138,     0,
     135,     0,   135,     0,   135,     0,   135,   148,   150,     0,
     173,   173,   173,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   801,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   716,   717,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     226,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   851,     0,     0,   854,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   867,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     876,     0,     0,     0,     0,     0,     0,     0,   883,     0,
       0,     0,     0,     0,     0,     0,     0,   887,     0,     0,
       0,     0,   893,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   901,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     910,     0,   912,   913,   914,   915,     0,     0,     0,     0,
       0,   918,     0,     0,     0,     0,   930,   931,   383,     0,
       0,     0,     0,   397,   128,     0,     0,     0,     0,   418,
       0,     0,     0,   861,   862,   863,   865,   431,   128,     0,
       0,     0,   418,     0,     0,   958,   441,     0,     0,   418,
       0,     0,   418,   875,     0,   418,     0,     0,   455,     0,
       0,   458,     0,   418,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   981,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   993,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1001,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1017,     0,  1019,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1035,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1047,     0,     0,     0,  1053,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   971,     0,     0,     0,     0,     0,   975,     0,
       0,     0,     0,     0,     0,   977,     0,     0,     0,  1076,
       0,     0,     0,     0,   570,   572,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   609,     0,   612,     0,     0,  1105,     0,     0,
       0,     0,     0,     0,     0,   626,     0,     0,     0,  1118,
       0,  1120,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1136,
       0,     0,   687,     0,  1065,   173,     0,     0,     0,     0,
       0,     0,  1195,     0,     0,     0,     0,     0,     0,     0,
    1201,     0,  1205,  1206,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1213,     0,     0,     0,
       0,     0,     0,     0,  1221,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1229,     0,  1130,     0,     0,     0,     0,     0,  1256,     0,
       0,     0,     0,     0,     0,     0,  1270,  1141,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   781,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1306,  1307,     0,     0,   802,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1225,     0,     0,   824,
       0,     0,   829,     0,  1327,     0,     0,  1331,     0,     0,
       0,   839,     0,     0,     0,     0,     0,     0,     0,     0,
     626,     0,     0,   626,  1272,     0,     0,     0,  1273,  1274,
    1341,  1276,     0,  1279,     0,     0,     0,  1281,  1282,     0,
    1284,     0,  1287,     0,  1288,  1289,     0,  1291,  1292,  1293,
       0,  1295,     0,  1296,     0,  1298,  1299,     0,  1300,     0,
    1302,  1303,     0,  1304,     0,  1305,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1322,  1324,     0,  1326,     0,  1328,
    1330,     0,  1332,  1333,  1334,  1335,  1336,     0,     0,   802,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1340,     0,  1342,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   949,
       0,     0,     0,     0,     0,     0,     0,     0,   961,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   802,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   626,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1099,     0,  1099,     0,     0,
    1099,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   802,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   802,   802,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1197
};

static const yytype_int16 yycheck[] =
{
       0,   185,    45,   181,   209,   315,     0,     0,    51,    52,
       0,    54,   224,    56,   217,    58,     0,    60,   337,     0,
     864,    64,     0,    66,   120,    68,   183,    70,   391,    72,
     336,    74,   189,    76,   321,    78,   122,   123,   782,    82,
     391,   225,   120,   184,   751,    88,   187,    90,   541,    92,
     561,    94,   466,    96,   468,    98,   813,     0,   772,   105,
     902,   341,   108,   484,   110,   479,   112,  1048,   114,   974,
     116,   367,   751,   772,     4,   754,   751,     0,   771,   754,
     870,   774,   757,   770,   120,   904,   644,     4,   963,   762,
     183,   120,  1054,   120,   772,   143,   120,   145,   978,   641,
       9,   332,   146,  1211,  1212,  1067,  1214,  1215,     6,  1217,
      17,  1219,  1220,   316,  1222,   969,   970,    14,    16,    26,
      17,    65,   663,     4,   665,  1215,   667,    71,   669,    38,
     671,    39,     6,     7,    35,     3,   179,   325,    12,    56,
      48,   319,     7,    60,    49,    89,    17,    12,    55,    79,
     334,   339,    17,    83,   996,    14,   359,    25,    17,    60,
       5,  1066,    79,    22,   200,   368,    83,  1275,   373,    77,
      15,   200,   375,   200,  1054,  1283,   200,   934,   885,    60,
      51,   868,    87,    81,    85,    86,   976,   230,   902,  1297,
     190,    88,   235,  1301,   237,   514,   189,   197,    79,   710,
     243,   249,    83,   902,  1294,  1059,   885,    81,   495,   888,
     885,   254,   911,   888,   257,   890,    61,    62,   962,   894,
    1058,   264,   979,  1061,   902,   268,     4,   900,    19,    88,
     273,   518,  1137,   326,     9,   278,     7,    28,   331,   519,
     283,    12,    17,   186,   737,   288,  1227,   290,    35,  1211,
    1212,   294,  1214,   549,   297,   813,   552,   300,   972,   471,
     674,  1080,   676,   306,  1139,   496,  1023,   778,   810,   312,
    1014,   367,   693,   972,     5,   982,   507,   818,    56,    40,
    1127,   822,  1129,     5,    15,   826,   827,   828,  1078,   367,
     831,   832,    31,    15,   835,    61,   499,  1139,    85,    86,
    1044,    79,  1207,   982,   983,    83,  1005,   982,   983,   984,
     985,  1004,    17,  1275,    53,    54,  1278,    17,    23,   365,
     366,  1283,   575,    34,  1286,   578,    26,  1005,  1290,    17,
      32,    33,  1122,  1123,   644,   538,     9,   162,   541,    61,
      65,    52,    18,   386,    17,   388,   346,   340,    49,   506,
     393,    27,   162,    53,    54,     7,   559,    68,    35,    11,
     403,  1323,    50,    51,    89,    38,   523,  1329,   411,   162,
      17,   414,    73,    84,   417,    22,  1054,   589,     7,    56,
     162,  1059,    11,    12,    13,   162,    87,   430,    17,  1067,
      66,  1070,   435,    22,    17,  1070,  1071,  1072,   708,   940,
     941,   942,   943,    26,    80,   546,    82,    17,    85,    86,
     720,   162,    67,   700,    90,     7,    26,    35,    64,    11,
      12,    13,   465,   149,   467,    17,   469,   148,    20,   780,
      22,    54,   149,   476,   721,   849,    91,   480,    56,   745,
      69,   644,    60,   806,   146,   147,   148,    41,   150,   151,
     152,   153,   154,   549,  1198,     7,   552,    57,    67,    11,
    1135,  1139,    48,    72,   746,    17,   748,    85,    86,   751,
      17,     4,   754,    30,    31,   757,    24,    69,   760,    26,
     566,  1325,    91,    30,    31,   154,    19,   156,   348,   489,
    1031,   351,     9,   493,   354,    28,    53,    54,   706,   707,
     703,  1210,   369,   813,   371,   551,    53,    54,   554,  1218,
     510,   557,   506,    16,     7,   723,    17,    17,    11,    12,
      13,    21,    22,    56,    17,   158,   159,    60,    21,    22,
      25,   734,   343,   523,   737,   738,   150,   151,   152,   350,
      47,   955,   353,   957,   587,   356,    79,    32,   529,   549,
      83,   535,   552,   364,   532,   555,   154,     7,   156,   762,
      33,    11,    12,    13,    14,    45,    17,     7,     8,   160,
     161,    11,    12,    13,    14,    26,    69,    17,   756,   622,
      31,   759,    17,    74,    75,    20,    26,    22,    62,    29,
      30,    31,   911,   154,   904,     7,  1010,   153,   154,    11,
      12,    13,    53,    54,   155,    17,   888,   791,   890,   766,
     813,   814,   894,    53,    54,    55,   659,   146,   661,    69,
     156,   664,  1036,   666,   934,   668,    29,   670,     7,    69,
     673,     7,    11,    12,    13,    11,    12,    13,    14,   682,
       8,    17,   502,   686,   504,    58,    59,   690,    88,   509,
      26,   708,   512,    29,    30,    31,    54,    69,   155,   315,
      36,   814,   522,   160,   161,   162,    61,    62,   528,   979,
     642,   531,    53,    54,   534,   811,   879,    53,    54,    55,
     205,   206,   207,     8,  1098,   545,  1005,   746,   872,    35,
     748,    10,    17,    69,   747,  1109,  1110,   900,   885,    18,
     483,    26,   991,   985,    29,    30,    31,  1143,    27,  1097,
      56,  1103,    88,  1023,    60,    34,   520,   711,    37,   897,
    1059,   899,   934,  1212,    70,  1219,  1220,  1217,    53,    54,
      55,   934,  1220,    52,  1067,   725,     0,   146,   147,    85,
      86,   150,   151,   152,    63,     0,   746,    66,   748,    68,
     580,   751,  1015,  1016,   754,   798,   492,   757,   908,    78,
     760,    80,   111,    82,   113,    84,   115,   120,   117,   120,
    1080,    90,   772,   766,   147,   775,   979,   150,   151,   152,
     153,   154,  1196,   200,   157,   712,     4,     5,   794,  1203,
     974,   147,    10,   998,   150,   151,   152,    15,   871,    17,
      18,    17,   953,   954,  1051,  1052,    24,   850,   514,    27,
      26,   120,   990,    29,    30,    31,    34,    35,   367,    37,
    1023,    39,   847,   848,   120,   340,    44,    43,    46,    45,
     519,    49,  1131,   367,    52,   200,   367,    53,    54,    55,
     360,   120,    60,    17,   120,    63,   120,    65,    66,    67,
      68,   120,    26,   120,   120,    29,    30,    31,    76,    77,
      78,    79,    80,   120,    82,    83,    84,    85,    86,    87,
     120,    89,    90,    91,     0,     0,  1306,  1005,     9,    53,
      54,    55,  1066,    -1,    -1,   928,    17,    -1,   888,    -1,
     890,    -1,    -1,    -1,   894,    26,  1074,     9,    29,    30,
      31,    -1,   902,    -1,    -1,    17,    -1,    38,    -1,    -1,
      -1,    42,    43,   956,    26,    -1,    -1,    29,    30,    31,
      -1,   964,    53,    54,    55,    -1,    38,    -1,    -1,     9,
      -1,    43,    -1,    -1,    -1,   147,   148,    17,   150,   151,
     152,    53,    54,    55,    -1,    -1,    26,    -1,    -1,    29,
      30,    31,    -1,  1137,    -1,    -1,    -1,    -1,    38,    -1,
    1003,    -1,    -1,    -1,    -1,    -1,  1009,    -1,  1011,    -1,
    1013,    -1,   972,    53,    54,    55,    -1,    17,    -1,    -1,
      -1,    -1,    -1,    -1,   329,   985,    26,    -1,   333,    29,
      30,    31,    -1,    -1,    -1,    -1,   996,   342,    -1,    -1,
      -1,    -1,    -1,  1046,   349,  1005,    -1,   352,    -1,    -1,
     355,    -1,    52,    53,    54,    55,    -1,    -1,   363,    -1,
      -1,    -1,    -1,  1207,   127,    -1,   129,    -1,   131,   132,
      -1,   134,    -1,   136,   137,    -1,   139,   140,   141,   142,
     143,   144,   145,   146,   147,    -1,   149,    -1,   151,    -1,
      -1,  1094,    17,  1096,  1054,    -1,    -1,    -1,    -1,  1059,
      -1,    26,    -1,    -1,    29,    30,    31,  1067,  1111,   172,
      -1,   174,   175,   176,    -1,   178,    -1,  1280,    43,   146,
     147,   148,    17,   150,   151,   152,   153,   154,    53,    54,
      55,    26,    -1,    -1,    29,    30,    31,    -1,    -1,  1277,
      -1,    36,    -1,    -1,    -1,   147,    -1,  1285,   150,   151,
     152,   153,   154,   155,    -1,   157,    -1,    -1,    53,    54,
      55,    -1,    -1,    -1,   227,    -1,   229,    -1,    -1,    -1,
      -1,  1131,    -1,   236,    -1,   238,    -1,   240,    -1,  1139,
      -1,   244,   245,   246,    -1,   248,   249,   250,    -1,    -1,
     253,    -1,    -1,    -1,    -1,   258,   259,    -1,   261,    -1,
      -1,  1204,    -1,    -1,   267,    -1,    -1,    -1,   271,   272,
      -1,    -1,    -1,   276,   277,    -1,    -1,    -1,   281,   282,
      -1,    -1,    -1,    -1,    -1,    -1,   289,    -1,    -1,    -1,
     293,    -1,    -1,  1236,    -1,  1238,    -1,    -1,   301,    -1,
      -1,    -1,  1245,    -1,   307,   308,   309,    -1,   311,  1209,
    1210,  1211,  1212,    -1,  1214,  1215,    -1,  1217,  1218,  1219,
    1220,    -1,  1222,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,    -1,    -1,    -1,
      -1,   384,   385,    -1,    -1,    -1,    -1,    -1,   391,   392,
      -1,   394,    -1,    -1,    -1,   398,   399,   400,    -1,   402,
      -1,   147,   148,    -1,   150,   151,   152,   153,   154,    -1,
     413,   157,    -1,   416,    -1,    -1,   419,   420,    -1,   422,
      -1,   424,    -1,   426,    -1,    -1,   429,    -1,    -1,    -1,
     433,   434,    -1,    -1,   437,   438,    -1,    -1,    -1,    -1,
      -1,   444,   445,    -1,   447,   448,    -1,   450,   451,   147,
     148,    -1,   150,   151,   152,   153,   154,   460,   461,   462,
     463,   464,    -1,   466,    -1,   468,    -1,    -1,    -1,    -1,
     473,    -1,   475,    -1,   477,    -1,   479,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   107,   108,   109,
     110,   111,   112,   113,    -1,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,    -1,
      -1,   141,   142,   143,   144,   145,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   568,    -1,    -1,    -1,    -1,
      -1,    -1,   575,    -1,    -1,   578,    -1,   580,    -1,   582,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   605,   606,   607,    -1,    -1,   610,    -1,    -1,
     613,   614,   615,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   627,    -1,   629,    -1,   631,    -1,
     633,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   646,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   658,    -1,   660,    -1,    -1,
     663,    -1,   665,    -1,   667,    -1,   669,    -1,   671,    -1,
      -1,   674,   675,   676,   677,   678,    -1,   680,    -1,    -1,
     683,    -1,    -1,    -1,    -1,   688,    -1,    -1,    -1,   692,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   780,    -1,   782,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   790,    -1,    -1,
     793,   794,    -1,    -1,    -1,    -1,   799,    -1,    -1,    -1,
     803,    -1,   805,   806,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   818,    -1,   820,    -1,   822,
      -1,    -1,    -1,   826,   827,   828,    -1,    -1,   831,   832,
      -1,   834,   835,    -1,   837,    -1,    -1,   840,    -1,    -1,
     843,   844,    -1,    -1,   847,   848,   849,    -1,    -1,   852,
      -1,    -1,    -1,    -1,    -1,    -1,   859,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   910,    -1,    -1,
     913,   914,   915,   191,   917,   193,   919,   195,    -1,    -1,
     198,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   931,    -1,
      -1,    -1,    -1,   936,   937,   938,    -1,   940,   941,   942,
     943,    -1,   945,    -1,   947,    -1,    -1,    -1,    -1,    -1,
     953,   954,   955,    -1,   957,    -1,   959,    -1,    -1,   962,
      -1,    -1,    -1,   966,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1008,    -1,  1010,    -1,  1012,
      -1,  1014,  1015,  1016,    -1,    -1,    -1,  1020,  1021,    -1,
      -1,    -1,  1025,  1026,    -1,  1028,    -1,    -1,  1031,    -1,
    1033,    -1,    -1,  1036,    -1,   313,    -1,    -1,    -1,    -1,
      -1,  1044,   320,    -1,   322,   323,  1049,    -1,  1051,  1052,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   335,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1097,  1098,    -1,  1100,  1101,    -1,
    1103,  1104,    -1,    -1,    -1,    -1,  1109,  1110,    -1,  1112,
      -1,    -1,   286,   287,    -1,   289,  1119,  1120,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   412,    -1,    -1,    -1,    -1,  1142,
    1143,  1144,    -1,   421,    -1,    -1,    -1,    -1,    -1,  1152,
     428,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   436,    -1,
      -1,    -1,    -1,    -1,   442,   443,    -1,    -1,   446,    -1,
      -1,   449,    -1,    -1,  1177,  1178,    -1,    -1,   456,    -1,
      -1,   459,    -1,  1186,  1187,  1188,  1189,  1190,  1191,    -1,
      -1,  1194,   470,  1196,   472,  1198,    -1,    -1,    -1,    -1,
    1203,    -1,    -1,   481,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   494,    -1,    -1,    -1,
     498,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1235,   511,  1237,    -1,    -1,    -1,    -1,   517,
      -1,  1244,    -1,  1246,    -1,    -1,    -1,    -1,  1251,    -1,
      -1,    -1,  1255,    -1,    -1,   429,  1259,    -1,   432,    -1,
      -1,  1264,    -1,    -1,    -1,   439,   440,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   457,    -1,    -1,    -1,   565,    -1,    -1,
      -1,    -1,    -1,   571,    -1,   573,    -1,    -1,    -1,   577,
      -1,    -1,    -1,  1306,    -1,   583,  1309,  1310,    -1,    -1,
      -1,  1314,   590,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   611,    -1,    -1,    -1,    -1,   616,   617,
      -1,    -1,    -1,    -1,    -1,    -1,   624,    -1,    -1,    -1,
      -1,    -1,   630,    -1,   632,    -1,   634,   635,   636,   637,
     638,   639,   640,    -1,    -1,    -1,    -1,   645,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   102,   103,    -1,    -1,   106,   107,    -1,   109,    -1,
     111,    -1,   113,    -1,   115,    -1,   117,   118,   119,    -1,
     121,   122,   123,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   713,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   620,   621,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     181,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   773,    -1,    -1,   776,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   792,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     808,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   816,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   825,    -1,    -1,
      -1,    -1,   830,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   842,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     858,    -1,   860,   861,   862,   863,    -1,    -1,    -1,    -1,
      -1,   869,    -1,    -1,    -1,    -1,   874,   875,   319,    -1,
      -1,    -1,    -1,   324,   325,    -1,    -1,    -1,    -1,   330,
      -1,    -1,    -1,   787,   788,   789,   790,   338,   339,    -1,
      -1,    -1,   343,    -1,    -1,   903,   347,    -1,    -1,   350,
      -1,    -1,   353,   807,    -1,   356,    -1,    -1,   359,    -1,
      -1,   362,    -1,   364,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   935,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   950,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   960,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   973,    -1,   975,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   992,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1002,    -1,    -1,    -1,  1006,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   916,    -1,    -1,    -1,    -1,    -1,   922,    -1,
      -1,    -1,    -1,    -1,    -1,   929,    -1,    -1,    -1,  1037,
      -1,    -1,    -1,    -1,   485,   486,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   503,    -1,   505,    -1,    -1,  1065,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   516,    -1,    -1,    -1,  1077,
      -1,  1079,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1117,
      -1,    -1,   563,    -1,  1018,   566,    -1,    -1,    -1,    -1,
      -1,    -1,  1130,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1138,    -1,  1140,  1141,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1184,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1192,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1208,    -1,  1106,    -1,    -1,    -1,    -1,    -1,  1216,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1224,  1121,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   702,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1271,  1272,    -1,    -1,   718,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1200,    -1,    -1,   750,
      -1,    -1,   753,    -1,  1312,    -1,    -1,  1315,    -1,    -1,
      -1,   762,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     771,    -1,    -1,   774,  1228,    -1,    -1,    -1,  1232,  1233,
    1338,  1235,    -1,  1237,    -1,    -1,    -1,  1241,  1242,    -1,
    1244,    -1,  1246,    -1,  1248,  1249,    -1,  1251,  1252,  1253,
      -1,  1255,    -1,  1257,    -1,  1259,  1260,    -1,  1262,    -1,
    1264,  1265,    -1,  1267,    -1,  1269,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1308,  1309,    -1,  1311,    -1,  1313,
    1314,    -1,  1316,  1317,  1318,  1319,  1320,    -1,    -1,   870,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1337,    -1,  1339,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   900,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   909,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   976,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1004,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1056,    -1,  1058,    -1,    -1,
    1061,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1078,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1122,  1123,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1134
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     4,     5,    10,    15,    17,    18,    24,    27,    34,
      35,    37,    39,    44,    46,    49,    52,    60,    63,    65,
      66,    67,    68,    76,    77,    78,    79,    80,    82,    83,
      84,    85,    86,    87,    89,    90,    91,   164,   165,   166,
     237,   239,   303,   304,   307,   309,   330,   331,   332,   333,
     334,   337,   338,   340,   342,   350,   351,   357,   358,   366,
     368,   379,   380,   381,   388,   389,   390,   391,   392,   393,
     394,   395,   396,   397,   398,   399,   400,   401,   403,   425,
     426,   427,   428,   432,   437,   440,   443,   446,   448,   455,
     457,   460,   462,   472,   474,   475,   478,   480,   481,     0,
     158,   159,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   146,   204,   240,   204,   310,
     204,   336,   336,   204,   343,   204,   352,   336,   204,   369,
     336,   352,   336,   352,   336,   352,   336,   352,   204,   429,
     204,   449,   331,   332,   333,   334,   357,   380,   381,   391,
     395,   399,   425,   426,   427,   432,   437,   440,   443,   446,
     456,   458,   459,   204,   463,   463,   463,    40,   464,   465,
     162,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   353,   203,   353,   203,   353,   203,   203,   402,   203,
     203,   160,   161,   202,   203,   203,   203,   203,   203,   201,
     147,   148,   150,   151,   152,   153,   154,   167,   168,   169,
     171,   173,   174,   177,   179,   180,   204,   241,    61,   311,
     313,    19,    28,    56,   304,   309,   320,   321,   339,   350,
     359,   360,   362,   363,   320,   339,   359,   171,   344,   340,
     359,     3,    25,   370,   371,    70,   307,   309,   320,   382,
     389,   433,   434,   435,   436,    23,   237,   354,   355,   202,
      71,   393,   438,   439,   202,    72,   397,   441,   442,   202,
      73,   401,   444,   445,    50,    51,   237,   404,   405,   407,
     408,   202,    64,   430,   431,   332,   334,   338,   357,   391,
     392,   450,   451,    74,    75,   452,   453,   464,   464,   464,
      41,   466,   467,   166,   179,   149,   167,   178,   171,   203,
     238,   203,   305,   308,   201,   201,   203,   201,   203,   203,
     201,   203,   203,   203,   203,   353,   203,   203,   201,   201,
     203,   203,   203,   201,   203,   258,   203,   201,   203,   203,
     201,   203,   203,   201,   203,   203,   201,   258,   258,   201,
     203,   258,   201,   203,   201,   201,   201,   203,   201,   203,
     203,   203,   203,   201,   202,   172,   175,   176,   180,   179,
      32,    33,   168,   204,   242,   243,   244,   246,   247,   202,
      62,   311,   316,   317,   340,   202,   202,   204,   314,   310,
     339,    57,   322,   323,     6,    16,    81,   267,   268,   270,
     271,   273,   335,   346,   347,    48,   410,   413,   204,   364,
     339,   335,   410,   171,   345,   202,   366,     9,   367,   373,
     375,   204,   372,   310,   382,   273,   383,   410,   364,   237,
     237,   204,   356,   267,   410,   364,   267,   410,   364,   267,
     410,   364,   179,   183,   185,   204,   406,   404,   204,   409,
     410,   364,   336,   336,   379,   388,   395,   396,   399,   400,
     447,   179,   454,   466,    25,   415,   416,   466,    47,   482,
     485,   166,   179,   203,   203,   201,   201,   305,   308,   203,
     306,   201,   203,   203,   312,   203,   203,   203,   325,   201,
     201,   202,   203,   201,   203,   201,   203,   203,   202,   203,
     203,   341,   203,   202,   203,   258,   201,   258,   203,   203,
     201,   202,   203,   203,   258,   258,   202,   202,   203,   203,
     202,   203,   203,   202,   203,   203,   154,   156,   182,   184,
     188,   191,   202,   258,   202,   203,   203,   203,   203,   203,
     206,   201,   203,   206,   201,   203,   206,   201,   202,   167,
     202,   203,   203,   201,   203,   206,   201,   202,   243,   246,
     204,   245,   204,   248,   237,   319,   168,   318,   316,   237,
     315,   202,   311,   335,    58,    59,   326,   328,   202,   179,
     324,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   274,   275,   276,   267,   204,
     348,   267,   204,   414,   320,   350,   335,   267,     7,    11,
     237,   259,   263,   202,   267,   367,   204,   376,   202,   311,
     383,   274,   267,   320,   389,   267,   393,   267,   397,   267,
     401,   179,   186,   179,   192,   267,   359,    26,    29,    30,
      31,    53,    54,    55,   207,   209,   210,   211,   212,   213,
     214,   215,   218,   219,   220,   222,   223,   228,   230,   233,
     234,   237,   249,   250,   399,   336,   399,   336,   336,   179,
     415,    38,   375,   411,   418,   422,   423,   204,   417,    45,
     479,   202,   463,   203,   202,   202,   306,   202,   306,   312,
     203,   202,   201,   167,   202,   203,   203,   203,   203,   202,
     203,   203,   203,   361,   202,   202,   258,   258,   201,   202,
     203,   203,   202,   203,   202,   203,   202,   202,   202,   202,
     202,   202,   202,   184,   182,   155,   157,   180,   181,   189,
     192,   197,   198,   199,   202,   203,   203,   201,   203,   208,
     201,   203,   208,   201,   203,   208,   201,   203,   208,   201,
     203,   208,   201,   208,   201,   206,   203,   206,   203,   203,
     203,   201,   203,   473,   201,   203,   327,   201,   203,   246,
     311,   204,   329,   179,   150,   151,   152,   277,   277,   277,
     170,   172,   349,   415,   350,     7,    12,   257,   262,   346,
     365,   202,   204,   265,   170,   377,   311,   277,   389,   179,
     187,   192,   179,   193,   181,   200,   366,   211,   237,   249,
     214,   218,   237,   249,   204,   216,   222,   228,   233,   204,
     221,   228,   233,   168,   224,   233,   168,   231,   183,   204,
     235,   146,   205,   411,   376,    43,   207,   418,   422,   470,
     471,   202,   376,   237,   202,   410,   460,   474,   476,   415,
     327,   258,   258,   258,   203,   258,   171,   202,   203,   361,
     201,   203,   203,   203,   374,   258,   202,   184,   200,   181,
     190,   197,   193,   202,   208,   203,   208,   202,   203,   208,
     203,   208,   208,   202,   203,   208,   208,   203,   208,   203,
     203,   202,   203,   461,   203,   473,   473,   206,   201,   203,
     202,   203,   202,   202,   202,   202,   257,   411,   202,   265,
     365,   171,   266,     7,    11,    12,    13,   255,   256,   378,
     202,   202,   179,   194,   195,   217,   219,   222,   228,   233,
     228,   233,   233,   233,   168,   225,   168,   232,   183,   204,
     236,    42,   207,   418,   422,   468,   469,   470,   202,   377,
     318,   204,   424,   203,   375,   422,   483,   486,   203,   203,
     203,   258,   203,   412,   203,   258,   201,   258,   203,   167,
     197,   202,   203,   203,   203,   203,   208,   208,   208,   208,
     203,   203,   229,   202,   461,   461,   203,   206,   201,   206,
     203,   202,   327,   479,   201,   203,   484,     8,   278,   281,
     279,   281,   280,   281,   237,   418,   422,   202,   266,   202,
     265,   278,   179,   196,   197,   222,   228,   233,   228,   233,
     233,   233,   168,   226,   259,   202,   470,   166,     7,    11,
      12,    13,    14,    69,   419,   420,   421,   202,   201,   376,
     207,   422,   486,   202,   203,   269,   201,   206,   201,   203,
     272,   201,   327,   412,   412,   258,   203,   203,   384,   197,
     203,   203,   203,   208,   203,   206,   202,   327,   201,   476,
     203,   484,   484,     7,    11,    12,    13,    14,    69,    88,
     207,   251,   252,   253,   254,   260,   264,   278,   302,   204,
     282,   282,   207,   280,   282,   202,   266,    36,   207,   302,
     385,   386,   228,   233,   233,   233,   168,   227,   202,   265,
     202,   377,   201,   201,   269,   206,   203,   203,   272,   203,
     258,   203,   206,   206,   201,   203,   202,   203,   261,   203,
     477,   258,   265,   265,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   283,   286,   295,   295,   106,   114,
     139,   140,   288,   291,   295,   202,   385,   204,   387,   233,
     266,   202,   207,   470,   479,   202,   202,   203,   261,   203,
     203,   203,   203,   202,   203,   203,   293,   203,   203,   203,
     203,   202,   203,   206,   327,   258,   206,   201,   266,   202,
      20,    22,   237,   252,   284,   296,   297,   300,   301,   284,
      21,   237,   252,   285,   298,   299,   300,   285,   237,   252,
     287,   300,   237,   260,   294,   300,   202,   237,   289,   296,
     300,   284,   237,   290,   298,   300,   290,   237,   292,   300,
     202,   476,   258,   258,   258,   203,   258,   201,   203,   258,
     201,   258,   258,   203,   258,   201,   203,   258,   258,   258,
     203,   258,   258,   258,   203,   258,   258,   203,   258,   258,
     258,   203,   258,   258,   258,   258,   202,   202,   252,   300,
     168,   252,   179,   252,   300,   168,   252,   252,   260,   300,
     300,   477,   258,   203,   258,   203,   258,   202,   258,   203,
     258,   202,   258,   258,   258,   258,   258,   252,   257,   252,
     258,   202,   258
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   163,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   165,   165,   165,   165,   165,   165,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   167,   168,   168,   169,   170,   171,   171,   172,
     172,   173,   174,   175,   176,   177,   177,   178,   178,   179,
     179,   179,   179,   180,   180,   181,   182,   183,   183,   183,
     184,   184,   185,   186,   187,   188,   189,   189,   190,   190,
     191,   192,   193,   194,   194,   194,   195,   196,   197,   197,
     198,   199,   199,   200,   200,   201,   201,   202,   202,   203,
     204,   205,   206,   206,   207,   207,   207,   208,   208,   208,
     209,   209,   210,   210,   210,   211,   211,   211,   211,   212,
     213,   214,   215,   216,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   229,   230,   231,   232,   233,
     234,   235,   235,   236,   236,   237,   238,   238,   238,   238,
     238,   238,   238,   239,   240,   241,   241,   242,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   251,   252,
     253,   254,   254,   254,   254,   254,   255,   256,   256,   256,
     256,   257,   258,   258,   259,   260,   261,   261,   262,   262,
     263,   263,   264,   264,   265,   266,   267,   267,   267,   268,
     269,   269,   269,   269,   270,   271,   272,   272,   272,   273,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   275,
     275,   275,   276,   276,   277,   277,   277,   278,   279,   280,
     281,   282,   283,   283,   283,   283,   283,   283,   283,   283,
     283,   284,   284,   284,   284,   284,   284,   284,   284,   285,
     285,   285,   285,   285,   285,   285,   285,   286,   286,   287,
     287,   287,   287,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   289,   289,   289,   289,   290,   290,   290,   290,
     291,   291,   292,   292,   293,   293,   294,   294,   294,   294,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   303,   304,   305,   305,   306,   306,
     307,   308,   308,   309,   310,   311,   312,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     324,   324,   325,   325,   326,   327,   327,   328,   328,   329,
     330,   330,   330,   330,   331,   332,   333,   334,   335,   335,
     336,   337,   337,   338,   338,   339,   339,   340,   341,   341,
     341,   342,   342,   343,   344,   345,   346,   347,   347,   348,
     349,   349,   350,   351,   351,   351,   352,   353,   353,   353,
     353,   354,   355,   356,   357,   358,   358,   359,   359,   360,
     361,   361,   361,   361,   362,   363,   363,   364,   365,   366,
     367,   367,   368,   369,   370,   371,   371,   372,   373,   374,
     374,   375,   376,   377,   378,   379,   379,   380,   381,   382,
     382,   383,   384,   384,   384,   384,   384,   385,   386,   387,
     388,   388,   389,   390,   390,   390,   391,   392,   392,   393,
     394,   394,   395,   396,   397,   398,   398,   399,   400,   401,
     402,   402,   402,   402,   402,   403,   403,   404,   405,   406,
     406,   407,   408,   409,   410,   411,   411,   412,   412,   412,
     412,   413,   414,   415,   416,   417,   418,   419,   420,   421,
     421,   421,   421,   421,   421,   422,   423,   424,   425,   426,
     427,   428,   429,   430,   431,   432,   433,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   447,   447,   447,   447,   448,   449,   450,   450,
     450,   451,   451,   452,   453,   453,   454,   454,   454,   455,
     456,   456,   457,   458,   458,   458,   458,   458,   458,   458,
     458,   458,   458,   458,   458,   458,   458,   458,   459,   459,
     459,   459,   459,   459,   459,   459,   460,   461,   461,   461,
     461,   461,   461,   461,   462,   463,   464,   465,   466,   467,
     468,   469,   470,   471,   472,   473,   473,   473,   473,   473,
     474,   475,   476,   476,   476,   477,   477,   477,   477,   478,
     479,   480,   481,   482,   483,   483,   484,   484,   484,   484,
     485,   486
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     1,     1,     0,
       1,     3,     1,     1,     2,     2,     2,     0,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     4,     6,
       0,     1,     1,     1,     1,     3,     3,     1,     2,     1,
       1,     1,     1,     3,     4,     2,     1,     1,     1,     1,
       1,     3,     2,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     0,     2,     1,     2,     1,     0,     3,     2,
       1,     1,     3,     2,     1,     1,     3,     4,     3,     6,
       1,     4,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     5,     5,     5,     5,     7,     2,
       2,     2,     2,     4,     4,     4,     4,     4,     4,     6,
       6,     6,     6,     8,     4,     1,     1,    10,     1,     1,
       1,     1,     1,     7,     0,     2,     1,     1,     1,     6,
       1,     1,     1,     1,     1,     7,     0,     2,     4,     6,
       2,     4,     2,     1,     1,     1,     1,     1,     1,     4,
       1,     1,     4,     1,     1,     4,     1,     1,     1,     1,
       7,     1,     1,     1,     1,     1,     7,     1,     1,     1,
       1,     7,     0,     3,     7,     5,     0,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,    10,
       0,     3,     3,     2,    10,    10,     0,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     6,     7,     6,
       1,     1,     1,     1,     3,     1,     3,     1,     3,     1,
       3,     2,     4,     6,     4,     2,     4,     2,     2,     2,
       4,     6,     4,     2,     4,     2,     2,     1,     3,     2,
       2,     4,     2,     1,     1,     3,     1,     3,     1,     3,
       1,     3,     2,     4,     2,     2,     2,     4,     2,     2,
       1,     3,     2,     2,     0,     2,     2,     2,     4,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     6,     1,     4,     1,
       4,     1,     1,     1,     1,     7,     5,     3,     0,     3,
       7,     3,     3,     1,     1,     5,     0,     3,     1,     1,
       1,     4,     1,     1,     1,     5,     1,     4,     1,     1,
       2,     3,     0,     2,     5,     0,     2,     1,     1,     1,
       1,     1,     1,     1,     8,     8,    10,    10,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     9,     0,     3,
       3,     1,     1,     1,     1,     1,     6,     1,     1,     1,
       4,     2,     7,     1,     1,     1,     1,     0,     2,     3,
       5,     4,     1,     1,    10,     1,     1,     1,     1,     7,
       0,     2,     4,     2,     9,     1,     1,     1,     1,     8,
       2,     3,     1,     1,     5,     1,     1,     1,     7,     0,
       3,     1,     1,     1,     1,     1,     1,     8,    10,     1,
       1,    10,     0,     3,     5,     3,     2,     5,     1,     1,
       1,     1,     5,     1,     1,     1,     8,     1,     1,     5,
       1,     1,     8,     1,     5,     1,     1,     8,     1,     5,
       0,     3,     5,     3,     3,     1,     1,     4,     1,     1,
       1,     4,     1,     1,     9,     1,     1,     0,     3,     3,
       3,     1,     1,     5,     1,     1,     9,     1,     5,     1,
       1,     1,     1,     1,     1,     7,     1,     1,    10,    10,
      10,     1,     1,     8,     1,    10,     1,     1,     6,     8,
       1,    10,     6,     1,    10,     6,     1,    10,     6,     1,
       8,     2,     2,     2,     4,     4,     1,     1,     1,     1,
       1,     1,     1,     4,     1,     1,     1,     2,     3,     4,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,    13,     0,     3,     3,
       3,     5,     3,     2,     1,     1,     4,     1,     4,     1,
       4,     1,     4,     1,    11,     0,     3,     3,     3,     2,
       1,    19,     1,     1,     1,     0,     6,     3,     2,     1,
       1,     9,     1,     9,     1,     1,     0,     3,     3,     2,
       1,     7
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (context, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, context); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, pj_wkt2_parse_context *context)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (context);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, pj_wkt2_parse_context *context)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, context);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, pj_wkt2_parse_context *context)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , context);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, context); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr && yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, pj_wkt2_parse_context *context)
{
  YYUSE (yyvaluep);
  YYUSE (context);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (pj_wkt2_parse_context *context)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = (yytype_int16)yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, context);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
      

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (context, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (context, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, context);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
#if 0
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
#endif
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, context);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (context, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, context);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, context);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
