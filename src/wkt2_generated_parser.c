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
#define YYFINAL  105
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3221

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  163
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  339
/* YYNRULES -- Number of rules.  */
#define YYNRULES  670
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1371

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
     207,   208,   211,   211,   212,   212,   212,   213,   215,   215,
     219,   223,   223,   225,   227,   229,   229,   231,   231,   233,
     235,   237,   239,   241,   241,   243,   243,   245,   245,   245,
     245,   247,   247,   251,   253,   257,   258,   259,   261,   261,
     263,   265,   267,   269,   273,   274,   277,   278,   280,   282,
     284,   287,   288,   289,   291,   293,   295,   295,   297,   300,
     301,   303,   303,   308,   308,   310,   310,   312,   314,   316,
     320,   321,   324,   325,   326,   328,   328,   329,   332,   333,
     337,   338,   339,   343,   344,   345,   346,   348,   352,   354,
     357,   359,   362,   363,   364,   365,   366,   367,   368,   369,
     370,   371,   372,   373,   374,   375,   376,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,   389,   390,   391,
     392,   393,   397,   399,   401,   405,   410,   412,   414,   416,
     418,   422,   427,   428,   430,   432,   434,   438,   442,   444,
     444,   446,   446,   451,   456,   457,   458,   459,   460,   461,
     462,   464,   466,   468,   468,   470,   470,   472,   474,   476,
     478,   480,   482,   486,   488,   492,   492,   495,   498,   503,
     503,   503,   503,   503,   506,   511,   511,   511,   511,   514,
     518,   519,   521,   537,   541,   542,   544,   544,   546,   546,
     552,   552,   554,   556,   563,   563,   563,   565,   572,   573,
     574,   575,   577,   584,   591,   592,   593,   595,   597,   597,
     597,   597,   597,   597,   597,   597,   597,   600,   600,   600,
     602,   602,   604,   604,   604,   606,   611,   617,   622,   625,
     628,   629,   630,   631,   632,   633,   634,   635,   636,   639,
     640,   641,   642,   643,   644,   645,   646,   649,   650,   651,
     652,   653,   654,   655,   656,   659,   660,   663,   664,   665,
     666,   671,   672,   673,   674,   675,   676,   677,   678,   679,
     682,   683,   684,   685,   688,   689,   690,   691,   694,   695,
     698,   699,   704,   705,   708,   709,   710,   711,   714,   715,
     716,   717,   718,   719,   720,   721,   722,   723,   724,   725,
     726,   727,   728,   729,   730,   731,   732,   733,   734,   735,
     736,   737,   738,   739,   740,   741,   742,   743,   744,   745,
     746,   747,   748,   749,   751,   754,   756,   758,   760,   762,
     764,   780,   780,   782,   790,   791,   793,   794,   796,   804,
     805,   807,   809,   811,   816,   817,   819,   821,   823,   825,
     827,   829,   831,   836,   840,   842,   845,   848,   849,   850,
     852,   853,   855,   860,   861,   863,   863,   865,   869,   869,
     869,   871,   871,   873,   881,   890,   898,   908,   909,   911,
     913,   913,   915,   915,   918,   919,   923,   929,   930,   931,
     933,   933,   935,   937,   939,   943,   948,   948,   950,   953,
     954,   958,   963,   963,   963,   965,   967,   968,   969,   970,
     972,   975,   977,   981,   987,   987,   991,   991,   992,   992,
     994,   999,  1000,  1001,  1002,  1004,  1010,  1015,  1021,  1023,
    1025,  1027,  1031,  1037,  1038,  1039,  1041,  1043,  1045,  1049,
    1049,  1051,  1053,  1058,  1059,  1061,  1063,  1065,  1067,  1071,
    1071,  1073,  1079,  1086,  1086,  1089,  1096,  1097,  1098,  1099,
    1100,  1102,  1106,  1108,  1110,  1110,  1114,  1119,  1119,  1119,
    1123,  1128,  1128,  1130,  1134,  1134,  1138,  1143,  1145,  1149,
    1149,  1153,  1158,  1160,  1164,  1165,  1166,  1167,  1168,  1170,
    1170,  1172,  1175,  1177,  1177,  1179,  1181,  1183,  1187,  1193,
    1194,  1195,  1196,  1198,  1200,  1204,  1209,  1211,  1214,  1219,
    1223,  1229,  1229,  1229,  1229,  1229,  1229,  1233,  1238,  1240,
    1245,  1245,  1246,  1248,  1248,  1250,  1257,  1257,  1259,  1266,
    1266,  1268,  1275,  1282,  1287,  1288,  1290,  1296,  1301,  1309,
    1315,  1317,  1319,  1324,  1326,  1326,  1327,  1327,  1331,  1337,
    1337,  1339,  1342,  1346,  1351,  1357,  1360,  1365,  1371,  1374,
    1379,  1385,  1388,  1393,  1399,  1399,  1400,  1400,  1401,  1401,
    1402,  1402,  1403,  1403,  1404,  1404,  1407,  1407,  1409,  1410,
    1411,  1413,  1415,  1419,  1422,  1422,  1425,  1426,  1427,  1429,
    1433,  1434,  1436,  1438,  1438,  1439,  1439,  1440,  1440,  1440,
    1441,  1442,  1442,  1443,  1443,  1444,  1444,  1446,  1446,  1447,
    1447,  1448,  1449,  1449,  1453,  1459,  1460,  1461,  1462,  1463,
    1464,  1465,  1467,  1469,  1471,  1473,  1475,  1477,  1479,  1481,
    1483,  1485,  1490,  1496,  1497,  1498,  1499,  1500,  1502,  1507,
    1515,  1515,  1515,  1515,  1517,  1518,  1519,  1520,  1522,  1524,
    1529,  1535,  1537,  1544,  1544,  1546,  1547,  1548,  1549,  1551,
    1553
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
  "geographic_crs", "static_geodetic_crs", "dynamic_geodetic_crs",
  "static_geographic_crs", "dynamic_geographic_crs",
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
  "base_static_geographic_crs", "base_dynamic_geographic_crs",
  "base_geodetic_crs_keyword", "base_geographic_crs_keyword",
  "base_crs_name", "ellipsoidal_cs_unit", "map_projection",
  "opt_separator_parameter_list_identifier_list", "map_projection_keyword",
  "map_projection_name", "map_projection_method",
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
  "opt_separator_parameter_or_parameter_file_identifier_list",
  "deriving_conversion_keyword", "deriving_conversion_name",
  "operation_method", "operation_method_keyword", "operation_method_name",
  "operation_parameter", "parameter_unit",
  "length_or_angle_or_scale_or_time_or_parametric_unit",
  "length_or_angle_or_scale_or_time_or_parametric_unit_keyword",
  "operation_parameter_file", "parameter_file_keyword",
  "parameter_file_name", "derived_geodetic_crs", "derived_geographic_crs",
  "derived_static_geod_crs",
  "base_static_geod_crs_or_base_static_geog_crs",
  "derived_dynamic_geod_crs",
  "base_dynamic_geod_crs_or_base_dynamic_geog_crs",
  "derived_static_geog_crs", "derived_dynamic_geog_crs",
  "base_static_geod_crs", "opt_separator_pm", "base_dynamic_geod_crs",
  "base_static_geog_crs", "base_dynamic_geog_crs", "derived_projected_crs",
  "derived_projected_crs_keyword", "derived_crs_name",
  "base_projected_crs", "base_projected_crs_keyword",
  "base_geodetic_geographic_crs", "derived_vertical_crs",
  "base_vertical_crs", "base_static_vertical_crs",
  "base_dynamic_vertical_crs", "base_vertical_crs_keyword",
  "derived_engineering_crs", "base_engineering_crs",
  "base_engineering_crs_keyword", "derived_parametric_crs",
  "base_parametric_crs", "base_parametric_crs_keyword",
  "derived_temporal_crs", "base_temporal_crs", "base_temporal_crs_keyword",
  "compound_crs", "single_crs", "single_crs_or_bound_crs",
  "opt_wkt_separator_single_crs_list_opt_separator_scope_extent_identifier_remark",
  "compound_crs_keyword", "compound_crs_name", "metadata_coordinate_epoch",
  "coordinate_epoch_keyword", "coordinate_epoch", "coordinate_metadata",
  "coordinate_metadata_crs", "coordinate_metadata_keyword",
  "static_crs_coordinate_metadata", "dynamic_crs_coordinate_metadata",
  "coordinate_operation",
  "opt_parameter_or_parameter_file_list_opt_interpolation_crs_opt_operation_accuracy_opt_separator_scope_extent_identifier_remark",
  "operation_keyword", "operation_name", "source_crs",
  "source_crs_keyword", "target_crs", "target_crs_keyword",
  "interpolation_crs", "interpolation_crs_keyword", "operation_accuracy",
  "operation_accuracy_keyword", "point_motion_operation",
  "opt_parameter_or_parameter_file_list_opt_operation_accuracy_opt_separator_scope_extent_identifier_remark",
  "point_motion_keyword", "concatenated_operation", "step",
  "opt_concatenated_operation_end", "concatenated_operation_keyword",
  "step_keyword", "bound_crs", "bound_crs_keyword",
  "abridged_coordinate_transformation",
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

#define YYPACT_NINF -1145

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-1145)))

#define YYTABLE_NINF -624

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     799, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145,
   -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145,
   -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145,
   -1145, -1145, -1145, -1145, -1145, -1145, -1145,   132, -1145, -1145,
   -1145,   304, -1145, -1145, -1145,   304, -1145, -1145, -1145, -1145,
   -1145, -1145,   304,   304, -1145,   304, -1145,   304, -1145,   304,
   -1145,   304, -1145, -1145, -1145,   304, -1145,   304, -1145,   304,
   -1145,   304, -1145,   304, -1145,   304, -1145,   304, -1145,   304,
   -1145, -1145, -1145, -1145, -1145, -1145, -1145,   304, -1145, -1145,
   -1145, -1145, -1145, -1145,   304, -1145,   304, -1145,   304, -1145,
     304, -1145,   304, -1145,   304, -1145, -1145, -1145,    30,    30,
      30,    30,    30,    30,    30,    30,    30,    30,    30,    30,
      30,    30,    30,    30,    30,    30,  1044,    30,    30,    30,
     170, -1145, -1145,    61, -1145,    61, -1145,    61,    61, -1145,
      61, -1145,    61,    61, -1145,    61,    61,    61,    61,    61,
      61,    61,    61,    61, -1145,    61, -1145,    61, -1145, -1145,
   -1145, -1145,    78, -1145, -1145, -1145, -1145, -1145,   148,   229,
     241, -1145, -1145, -1145, -1145,   315, -1145,    61, -1145,    61,
      61,    61, -1145,    61,   304, -1145,   922,   284,   445,   445,
     577,   345,   279,   321,   412,   342,   315,   250,   315,   393,
     315,    67,   280,   315,   351,  1285, -1145, -1145, -1145,   446,
     170,   170,   170,   381,  1044, -1145, -1145, -1145, -1145, -1145,
   -1145, -1145,   624, -1145, -1145, -1145, -1145,   299,   302,   307,
     577, -1145,    61, -1145,    61,   304, -1145, -1145, -1145, -1145,
     304,    61,   304,    61, -1145,   304,   304,    61,    61, -1145,
   -1145, -1145, -1145,    61,    61,    61,    61, -1145,    61,    61,
      61, -1145, -1145, -1145, -1145,   304,   304, -1145, -1145,    61,
     304, -1145, -1145,   304,    61,    61, -1145,    61, -1145, -1145,
     304, -1145,    61,    61,   304, -1145, -1145,    61,    61,   304,
   -1145, -1145,    61,    61,   304, -1145, -1145,    61,    61,   304,
   -1145, -1145,    61,    61,   304,    61,   304, -1145, -1145,    61,
     304, -1145,    61, -1145, -1145, -1145, -1145,   304,    61,    61,
      61, -1145,    61,   304,   315, -1145,   400,   624, -1145, -1145,
     365,   315,   122,   315,   315,    30,    30,   115,   402,   127,
      30,    30,   419,   419,   115,   127,   419,   419,   577,   315,
     456,    30,    30,   423,   315,    30,    30,   189,   480,   419,
      30,   490, -1145,   490,    30,   480,   419,    30,   480,   419,
      30,   480,   419,    30, -1145, -1145,   506,   146, -1145,    30,
     419,    30,  1285,   624,   381,   485,   381,   487,  1044, -1145,
     624, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145,    61,
      61,   304, -1145,   304, -1145, -1145,    61,    61,   304,    61,
   -1145, -1145, -1145,    61,    61,    61, -1145,    61,   304, -1145,
   -1145, -1145, -1145, -1145, -1145, -1145,   304,   315,    61,   304,
   -1145,    61,    61, -1145,    61,   304,    61,    61,   315,    61,
      61, -1145,    61, -1145,    61,    61,    61, -1145,    61,    61,
     304, -1145, -1145,    61,    61,    61,   304,   315,    61,    61,
      61,    61, -1145,   315,   315,    61,    61,   315,    61,    61,
     315,    61,    61, -1145, -1145,   376, -1145,   315,    61, -1145,
     315,    61,    61,    61,   307,   315,    61, -1145,    61,   304,
      61, -1145,    61,   304,   315, -1145,   510,   505,    30,    30,
   -1145, -1145,   490, -1145,   752,   515,   490,   315,   284,   127,
     532,   315,   624,  1049, -1145,   480,    30,   272,   272,   480,
      30,   480,   127, -1145,   480,   480,   470,   315,   480,   272,
     272, -1145, -1145,    30,   315,   284,   480,  1313, -1145,   480,
     106, -1145, -1145, -1145, -1145,   480,   100, -1145,   480,   152,
   -1145,   480,    74, -1145, -1145,   624, -1145, -1145,   624, -1145,
   -1145, -1145,   480,   279,   953,   315,   624, -1145,   485,  1361,
     315,    30,   502,   843,   315,    30, -1145,    61, -1145, -1145,
     315, -1145,   315, -1145,    61, -1145,   315,    61, -1145,    61,
   -1145,    61,   315, -1145, -1145, -1145,   304, -1145,   307,   315,
   -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145,
   -1145, -1145, -1145, -1145,    61,    61,    61, -1145, -1145,    61,
      61,    61,    61,    61,   315, -1145,    61,   315,   315,   315,
     315, -1145, -1145,    61,    61,   304, -1145,   315,    61,    61,
      61,    61, -1145,    61, -1145,    61,   315,    61,   315,    61,
     315,   315,   315,   315,   315,   315,   315,   428,   418, -1145,
     534,   315, -1145, -1145, -1145, -1145,    61, -1145, -1145, -1145,
   -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145,    61,   304,
      61,   304, -1145,    61,   304,    61,   304,    61,   304,    61,
     304,    61, -1145,   304,    61, -1145, -1145,    61, -1145, -1145,
   -1145,   304,    61,    61,   304,    61,   304, -1145, -1145,    61,
   -1145,   304, -1145, -1145,    61,   505, -1145, -1145, -1145, -1145,
   -1145, -1145,   110, -1145,    30,   624, -1145,   409,   409,   409,
     400,   115,    89,   315,   115,   315, -1145,   485, -1145, -1145,
   -1145, -1145, -1145, -1145,    30, -1145,   115,   105,   315,   115,
     315,   400,   569, -1145,   409, -1145,   189, -1145, -1145, -1145,
   -1145, -1145, -1145, -1145, -1145,   624, -1145, -1145,   624,   624,
   -1145,   431, -1145, -1145, -1145, -1145,   456,   130,   566,   469,
   -1145,    30,   340, -1145,    30,   183, -1145,   752,   282, -1145,
     752,   335, -1145,   506, -1145,   453, -1145,  1330,   315,    30,
   -1145, -1145,    30, -1145,   752,   490,   315,    90,   485, -1145,
      61, -1145,    61, -1145, -1145, -1145, -1145,    61,    61,    61,
      61,   577,   315,    61, -1145, -1145,    61, -1145,    61, -1145,
      61,    61, -1145, -1145, -1145,   304,    61, -1145, -1145,    61,
   -1145, -1145,    61,    61,    61,   315, -1145,   449,   431, -1145,
     534,   624, -1145,   315, -1145,    61, -1145,    61, -1145,    61,
   -1145, -1145,   315,    61,    61,    61, -1145,   315,    61,    61,
   -1145,    61,    61, -1145,    61, -1145, -1145,    61, -1145,   315,
   -1145, -1145,    61,    61,    61,   304,    61, -1145,    61,    61,
     315, -1145, -1145, -1145, -1145, -1145, -1145,   315,    61,   315,
     315,   315,   315,   497, -1145, -1145, -1145,   315,   315,   339,
     315,   577,   315,    30,   157,   315,   616,   315,   315, -1145,
   -1145, -1145,   624, -1145, -1145, -1145, -1145, -1145,   273, -1145,
   -1145,   183, -1145,   282, -1145, -1145, -1145,   282, -1145, -1145,
     752, -1145,   752,   506, -1145, -1145, -1145,   801, -1145,  1044,
   -1145,   400,    30, -1145,    61,   221, -1145,    61,    61,    61,
      61, -1145, -1145,    61,    61,    61, -1145, -1145,    61, -1145,
      61, -1145, -1145, -1145, -1145, -1145, -1145, -1145,   304,    61,
   -1145,    61, -1145, -1145,   802,   315,    61,    61,    61, -1145,
      61,    61,    61,    61, -1145,    61, -1145,    61, -1145, -1145,
     315,    61,   315,    61, -1145,    61,   502,   304, -1145,    61,
   -1145,   599,   599,   599, -1145, -1145, -1145, -1145,   315,   577,
      30, -1145,   599,   761, -1145, -1145,   347,   583,   559,   282,
   -1145, -1145, -1145, -1145,   752,   395,   315, -1145, -1145, -1145,
     305,   315,   304,    30,  1085,   315, -1145,    61,   304,    61,
     304,    61,   304, -1145,    61,    61,    61,   328,   761, -1145,
      61,    61, -1145,    61, -1145, -1145,    61, -1145,    61, -1145,
   -1145, -1145, -1145, -1145, -1145, -1145, -1145,    61, -1145,   304,
   -1145,    90,    61, -1145,    61,    61, -1145,   666, -1145,    30,
   -1145,    30,   715, -1145,    30,   315,   577,   912, -1145, -1145,
     583,   559,   559, -1145,   752,   315,    30,   315,   400, -1145,
   -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145,
   -1145, -1145,   304, -1145,   304,    61,    61, -1145,    61,    61,
   -1145,    61,    61, -1145,    61, -1145, -1145,    61,    61,   304,
      61, -1145, -1145, -1145, -1145,   315, -1145,    61,    61,    61,
      30,    30, -1145, -1145,  1410,  1663, -1145,  1450,   315,  1160,
   -1145, -1145,    30,   559, -1145,   577,   315,   943,   315,   315,
      61,    61,    61, -1145, -1145, -1145, -1145, -1145, -1145, -1145,
      61, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145,
   -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145,
   -1145, -1145, -1145, -1145, -1145,    61,    61, -1145, -1145, -1145,
   -1145, -1145,   315, -1145,    61,    61,    61,    61,    61,    61,
     315, -1145,    61, -1145,    61, -1145,    61, -1145,    61, -1145,
   -1145,    61,   304, -1145, -1145,   577,   315,   524,   524,   696,
     696, -1145,   597,   117,   315,   523,   524,   545,   545, -1145,
     358, -1145,   315, -1145, -1145,    90,    61, -1145, -1145, -1145,
      61,    61, -1145,    61,   304,    61,   304, -1145, -1145,    61,
      61, -1145,    61,   304,    61, -1145,    61,    61, -1145,    61,
      61,    61, -1145,    61, -1145,    61, -1145,    61,    61, -1145,
      61, -1145,    61,    61, -1145,    61, -1145,    61, -1145,   315,
     315, -1145, -1145,   597, -1145,   752,   397, -1145,   624, -1145,
   -1145,   597, -1145,   752,   397, -1145, -1145, -1145,   397, -1145,
   -1145, -1145,   134, -1145, -1145,   358, -1145, -1145, -1145,   358,
   -1145, -1145, -1145, -1145,    61, -1145,    61,    61,    61,    61,
     315,    61,    61,   315,    61,    61,    61,    61,    61, -1145,
   -1145,   397, -1145,   157, -1145, -1145, -1145,   397, -1145, -1145,
   -1145, -1145, -1145, -1145, -1145,    61,   315,    61, -1145, -1145,
   -1145
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   412,   401,   390,   400,   161,   424,   446,   392,   474,
     477,   591,   632,   658,   661,   499,   492,   351,   550,   484,
     481,   489,   487,   602,   648,   391,   414,   425,   393,   413,
     475,   479,   478,   500,   485,   482,   490,     0,     4,     5,
       2,     0,    13,   341,   342,     0,   574,   380,   378,   379,
     381,   382,     0,     0,     3,     0,    12,     0,   576,     0,
      11,     0,   578,   459,   460,     0,    14,     0,   580,     0,
      15,     0,   582,     0,    16,     0,   584,     0,    17,     0,
     575,   532,   530,   531,   533,   534,   577,     0,   579,   581,
     583,   585,    19,    18,     0,     7,     0,     8,     0,     9,
       0,    10,     0,     6,     0,     1,    73,    74,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    78,   162,     0,   352,     0,   389,     0,     0,   402,
       0,   415,     0,     0,   447,     0,     0,   416,     0,   416,
       0,   416,     0,   494,   551,     0,   592,     0,   603,   617,
     604,   618,   605,   606,   620,   607,   608,   609,   610,   611,
     612,   613,   614,   615,   616,     0,   600,     0,   633,     0,
       0,     0,   635,     0,     0,    77,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    75,    76,   599,     0,
       0,     0,     0,     0,     0,    40,    20,    37,    38,    39,
      41,    42,     0,   163,    21,    22,    26,     0,    25,    35,
       0,   164,   154,   356,     0,     0,   438,   439,   364,   395,
       0,     0,     0,     0,   394,     0,     0,     0,     0,   536,
     539,   537,   540,     0,     0,     0,     0,   403,     0,   416,
       0,   426,   427,   428,   429,     0,     0,   450,   449,   443,
       0,   563,   464,     0,     0,     0,   463,     0,   559,   560,
       0,   421,   190,   417,     0,   476,   566,     0,     0,     0,
     483,   569,     0,     0,     0,   488,   572,     0,     0,     0,
     506,   502,   190,   190,     0,   190,     0,   493,   553,     0,
       0,   586,     0,   587,   594,   595,   601,     0,     0,     0,
       0,   637,     0,     0,     0,    34,    27,     0,    33,    23,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   418,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   498,   497,     0,     0,   495,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   634,
       0,    29,    31,    28,    36,   168,   171,   165,   166,   155,
     158,     0,   160,     0,   153,   360,     0,   346,     0,     0,
     343,   348,   357,   354,     0,     0,   366,   370,     0,   406,
     217,   407,   388,   204,   205,   206,     0,     0,     0,     0,
     440,     0,     0,   513,     0,     0,     0,     0,     0,     0,
       0,   404,   397,   411,     0,     0,     0,   455,   190,   443,
       0,   442,   451,   190,     0,     0,     0,     0,     0,     0,
     190,   190,   422,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    50,   503,    48,   504,     0,   190,   507,
       0,     0,     0,   588,   596,     0,     0,   516,   643,     0,
       0,   669,    80,     0,     0,    32,     0,     0,     0,     0,
     345,   350,     0,   349,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   383,     0,     0,     0,     0,     0,
       0,     0,     0,   385,     0,     0,     0,     0,     0,     0,
       0,   444,   445,     0,     0,     0,     0,     0,   461,     0,
       0,   191,   419,   420,   480,     0,     0,   486,     0,     0,
     491,     0,     0,    44,    58,     0,    45,    49,     0,   501,
     496,   505,     0,     0,     0,     0,   597,   593,     0,     0,
       0,     0,     0,     0,     0,     0,   636,   156,   159,   169,
       0,   172,     0,   362,   346,   361,     0,   346,   358,   354,
     353,     0,     0,   375,   376,   371,     0,   363,   367,     0,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,     0,     0,     0,   387,   408,     0,
       0,   544,     0,   544,     0,   514,     0,     0,     0,     0,
       0,   199,   198,   190,   190,     0,   396,     0,     0,   431,
       0,   431,   456,     0,   448,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,     0,    59,
       0,     0,   554,   555,   556,   557,     0,   174,   100,   133,
     136,   144,   148,    98,   590,    82,    88,    89,    93,     0,
      85,     0,    92,    85,     0,    85,     0,    85,     0,    85,
       0,    85,    84,     0,   588,   573,   598,   625,   528,   641,
     647,     0,   643,   643,     0,    80,     0,   642,   517,   373,
     659,     0,    81,   660,     0,     0,   167,   170,   347,   359,
     344,   355,     0,   384,     0,   368,   365,     0,     0,     0,
      27,     0,     0,     0,     0,     0,   535,     0,   538,   386,
     541,   542,   399,   398,     0,   423,     0,     0,     0,     0,
       0,    27,     0,   462,     0,   558,     0,   561,   564,   565,
     567,   568,   570,   571,    46,     0,    43,    68,     0,     0,
      53,    71,    55,    66,    67,   549,     0,     0,     0,     0,
      91,     0,     0,   117,     0,     0,   118,     0,     0,   119,
       0,     0,   120,     0,    83,     0,   589,     0,     0,     0,
     644,   645,     0,   646,     0,     0,     0,     0,     0,   157,
       0,   377,   373,   369,   232,   233,   234,   190,   190,   190,
     190,     0,     0,   544,   545,   543,   544,   547,   509,   202,
       0,   431,   197,   196,   441,     0,   432,   434,   430,   431,
     436,   457,   453,     0,   190,     0,    52,    48,    71,    60,
       0,     0,    70,     0,    96,    85,    94,     0,    90,    85,
      87,   101,     0,    85,    85,    85,   134,     0,    85,    85,
     137,     0,    85,   145,     0,   149,   150,     0,    79,     0,
     639,   631,   625,   625,    80,     0,    80,   624,     0,     0,
       0,   374,   515,   652,   653,   650,   651,     0,     0,     0,
       0,     0,     0,     0,   410,    24,   405,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   562,
      47,    69,     0,    54,    57,    72,   552,    95,     0,    86,
      99,     0,   121,     0,   122,   123,   132,     0,   124,   125,
       0,   126,     0,     0,   173,   626,   627,     0,   628,     0,
     630,    27,     0,   640,     0,     0,   372,     0,     0,     0,
     190,   546,   548,   190,   509,   509,   508,   203,   190,   435,
       0,   433,   437,   188,   186,   185,   187,   458,     0,   190,
     452,     0,    64,    56,     0,     0,   102,   103,   104,   105,
      85,    85,    85,    85,   138,     0,   146,   142,   151,   152,
       0,    80,     0,     0,   529,   373,     0,     0,   664,   665,
     663,     0,     0,     0,   409,   512,   510,   511,     0,     0,
       0,   454,     0,     0,    63,    97,     0,     0,     0,     0,
     127,   128,   129,   130,     0,     0,     0,   147,   629,   638,
       0,     0,     0,     0,     0,     0,   238,   208,     0,    80,
       0,   214,     0,   192,   190,     0,   466,    65,     0,    61,
     106,   107,   108,   109,   110,   111,    85,   139,     0,   143,
     141,   526,   521,   522,   523,   524,   525,   373,   519,     0,
     527,     0,     0,   668,   665,   665,   662,     0,   207,     0,
     212,     0,     0,   213,     0,     0,     0,     0,   465,    62,
       0,     0,     0,   131,     0,     0,     0,     0,    27,   667,
     666,   183,   180,   179,   182,   200,   181,   201,   211,   340,
     175,   177,     0,   176,     0,   208,    80,   239,     0,     0,
     216,   214,     0,   189,   190,   472,   470,    80,    80,     0,
     112,   113,   114,   115,   140,     0,   518,   194,   654,   190,
       0,     0,   210,   209,     0,     0,   215,     0,     0,     0,
     467,   469,     0,     0,   135,     0,     0,     0,     0,     0,
       0,   194,   241,   298,   299,   300,   301,   302,   303,   304,
     243,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   245,   247,   329,   330,   331,
     332,   333,     0,   240,   265,   292,   272,   274,   276,   278,
       0,   271,   288,   184,    80,   473,   373,   116,   190,   520,
     657,    80,     0,   649,   670,     0,     0,     0,     0,     0,
       0,   235,     0,     0,     0,     0,     0,     0,     0,   237,
       0,   468,     0,   195,   656,     0,   190,   193,   335,   339,
     190,   190,   242,   190,     0,   190,     0,   244,   337,   190,
     190,   246,   190,     0,   190,   248,   190,   190,   266,   190,
     190,   190,   293,   190,   236,   190,   273,   190,   190,   275,
     190,   277,   190,   190,   279,   190,   289,   190,   471,     0,
       0,   249,   256,     0,   253,     0,     0,   255,     0,   257,
     264,     0,   261,     0,     0,   263,   267,   270,     0,   268,
     294,   297,     0,   295,   280,     0,   282,   283,   284,     0,
     286,   287,   290,   291,   654,   178,   190,   190,     0,   190,
       0,   190,   190,     0,   190,   190,   190,   190,   190,   655,
     252,     0,   250,     0,   254,   338,   260,     0,   258,   336,
     262,   269,   296,   281,   285,   190,     0,   190,   251,   334,
     259
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1145, -1145, -1145,  -206,  -222,  -184, -1145,   -98,  -180,   320,
   -1145, -1145, -1145, -1145, -1145, -1145,  -217,  -325,  -571,   -33,
    -675,  -564, -1145, -1145, -1145, -1145, -1145, -1145, -1145,  -507,
    -203, -1145, -1145, -1145,  -794, -1145, -1145,  -193,   -39,  1855,
    1023,   -42, -1145,  -668,  -517,  -563, -1145, -1145,  -107, -1145,
   -1145,  -103, -1145, -1145, -1145,  -108,  -252, -1145, -1145,  -741,
   -1145, -1145, -1145, -1145, -1145,  -684, -1145, -1145, -1145, -1145,
    -608, -1145, -1145, -1145,     0, -1145, -1145, -1145, -1145, -1145,
     194, -1145, -1145,  -452, -1145, -1145,  -685, -1145, -1145,  -872,
   -1145, -1145, -1145, -1145,  -892,  1299,  -342, -1144,  -477, -1145,
   -1145, -1145,  -878,  -966,    73, -1145,  -426, -1145, -1145,  -430,
    -270,   167, -1145, -1145,  -496,  -915, -1145,  -386,  -904,  -702,
   -1145,  -793,  -529, -1145, -1145, -1145, -1145,  -533, -1145, -1145,
   -1145, -1145,  -555,  -523, -1145,  -609, -1145, -1090, -1145,  -367,
   -1145,   736,  -387,  -151,   544,  -391,    62,   -45,  -315,   151,
   -1145, -1145, -1145,   242, -1145,   -56, -1145,  -145, -1145, -1145,
   -1145, -1145, -1145, -1145,  -791, -1145, -1145, -1145, -1145,   627,
     629,   633,   635,  -232,   467, -1145, -1145,  -105,    58, -1145,
   -1145, -1145, -1145, -1145,  -611, -1145, -1145, -1145,     3, -1145,
     593,   -47, -1145, -1145, -1145,   637, -1145, -1145, -1145,  -550,
   -1145, -1145, -1145,   558,   572,   198,  -148,     4,   323, -1145,
   -1145, -1145, -1145, -1145, -1145, -1145,  -344,  -756,  -855, -1145,
   -1145,   647,   652, -1145,   245, -1145,  -375, -1145, -1145, -1145,
    -182, -1145,   660, -1145,  -158, -1145,   661, -1145,  -166, -1145,
     663, -1145,  -170, -1145, -1145,   413, -1145, -1145, -1145, -1145,
   -1145,   852,  -321, -1145, -1145,  -526, -1145, -1145,  -692, -1145,
   -1145, -1145,  -774, -1145, -1145,   667, -1145, -1145,   603, -1145,
     605, -1145, -1145,   232,  -569,   235,   237,   238,   673, -1145,
   -1145, -1145, -1145, -1145,   676, -1145, -1145, -1145, -1145,   681,
   -1145, -1145,   682, -1145, -1145,   684, -1145, -1145,   685,  -178,
    -322,   118, -1145, -1145, -1145, -1145, -1145, -1145, -1145, -1145,
   -1145, -1145,   813,  -219, -1145,  -104,   405, -1145,   236, -1145,
   -1145, -1145,  -768, -1145, -1145,   -34,   815, -1145, -1017,  -515,
   -1145,  -916,   820, -1145, -1145, -1145,  -424, -1145,  -223
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   222,   585,   224,   841,   225,   821,
     226,   227,   391,   392,   228,   328,   229,   230,   851,   555,
     474,   556,   475,   658,   847,   557,   770,   923,   558,   771,
     850,   983,   984,  1058,   772,   773,   774,   852,   108,   208,
     361,   430,   879,   574,   712,   780,   675,   676,   677,   678,
     679,   680,   681,   862,   985,   682,   683,   684,   867,   685,
     686,   871,   995,  1068,  1145,   687,  1036,   688,   874,   997,
     689,   690,   877,  1000,   460,   331,    41,   133,   232,   399,
     400,   401,   580,   402,   403,   582,   692,   693,  1119,  1261,
    1121,  1122,   977,   978,   834,   362,   634,  1123,  1166,   835,
     635,  1124,   830,   968,   422,   423,  1088,   424,   425,  1093,
     426,   614,   615,   616,   817,  1047,  1049,  1051,  1048,  1128,
    1212,  1262,  1271,  1213,  1278,  1220,  1286,  1291,  1221,  1296,
    1244,  1282,  1214,  1263,  1264,  1272,  1273,  1265,  1266,  1126,
      42,   239,   333,   503,    44,   334,   240,   135,   234,   507,
     235,   413,   589,   407,   408,   586,   584,   241,   242,   417,
     418,   599,   511,   595,   806,   596,   812,    46,    47,    48,
      49,    50,    51,   427,   137,    52,    53,   243,   409,   527,
      55,   140,   258,   442,   428,   429,   619,   822,   244,    57,
     142,   196,   283,   284,   463,    58,    59,   260,   261,   748,
     262,   263,   264,   245,   246,   431,   837,   893,   354,    61,
     145,   269,   270,   453,   449,   917,   701,   643,   842,   979,
      62,    63,    64,   275,   457,  1098,  1138,  1139,  1226,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,   203,    79,   303,   304,   477,   305,   306,
     480,   894,   910,   435,   626,   488,   489,   709,   702,  1077,
    1078,  1079,   703,   704,  1005,    80,    81,    82,   247,    83,
     248,    84,    85,   249,   733,   250,   251,   252,    86,    87,
     155,   309,   310,   666,    88,   277,   278,   279,   280,    89,
     288,   289,    90,   293,   294,    91,   298,   299,    92,    93,
     312,   565,    94,   157,   316,   317,   485,    95,   175,    96,
     176,   177,   895,   798,    98,   179,   183,   184,   322,   323,
     884,   885,   705,   706,    99,   570,   896,   101,   897,  1168,
     102,   711,   313,   104,   492,  1009,  1045,   493,  1010
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      40,   393,   223,    56,    60,   325,   109,   327,   324,   450,
     257,   960,   276,   110,   111,   501,   112,   406,   113,   500,
     114,   899,   115,   883,   180,   181,   116,   311,   117,   886,
     118,   297,   119,   292,   120,   970,   121,   803,   122,   287,
     123,   863,   697,   888,   253,   578,   889,   674,   124,   274,
     329,   660,   700,  1054,   735,   125,   924,   126,    54,   127,
     483,   128,    45,   129,  1107,   130,   132,   134,   136,   136,
     139,   141,   136,   144,   136,   141,   136,   141,   136,   141,
     136,   141,   154,   156,   254,   178,   178,   178,   456,   769,
    1042,   750,   856,   764,   860,   419,  1003,   860,   864,  1281,
     860,   868,   198,   860,   200,   882,   860,  1056,  1050,  1052,
     394,   419,   832,   438,     7,     2,    15,   833,   875,     1,
     783,   824,   786,    15,   789,     4,   792,     2,   794,    12,
    1134,  1115,   105,   419,     5,   965,   836,     4,   433,  1259,
     296,    10,  1055,   420,   231,   214,   397,     5,  1115,  1274,
    1274,     5,  1279,  1283,    33,  1288,   667,  1293,  1293,   473,
    1297,    33,   238,     5,   832,    19,   484,    24,   441,   833,
     421,   233,  1125,   495,   865,   455,   131,   869,  1346,  1001,
     872,  1008,   494,   233,   405,   673,   421,   987,  1052,    34,
    1024,    31,    32,   591,    26,   282,   335,   301,    29,  1228,
       5,   336,   302,   338,   311,  1117,   340,   341,   421,   667,
     182,   828,   349,  1337,  1041,  1120,   948,   964,   950,    21,
     645,  1342,  1117,   185,    10,  1120,   351,   352,  1147,  1059,
     447,   355,   818,   819,   356,  1347,   671,   672,  1299,  1348,
    -619,   360,   694,    36,   988,   364,   860,   990,   860,   259,
     367,  1232,   860,  1149,   907,   370,   273,   908,   844,   698,
     373,   848,   566,   809,  1099,   376,   456,   379,   998,  1256,
    1084,   381,  1170,  1171,    31,    32,     1,   592,   383,   922,
     881,   912,   898,   920,   388,  1060,  1105,  1082,   398,   915,
     628,   414,   927,   412,   134,   598,   929,     5,   236,     5,
     932,   934,   935,   669,   670,   938,   939,   237,   667,   941,
    -621,   454,  1071,   452,   134,    19,  1072,  1073,  1074,  1075,
     989,   286,   462,   991,   267,   992,   671,   672,   238,   993,
     300,   301,    17,  1038,   476,   768,   672,   479,   657,    34,
     415,   659,  1061,  1063,   860,   233,   268,   437,   447,   696,
       2,    26,     5,   448,   444,    29,     5,     5,   650,     5,
       4,   667,   498,   461,   499,   281,   667,  1270,  1270,   504,
    1277,   670,   620,   622,  1076,     5,   725,   698,   670,   512,
    1259,  1090,   656,   654,   638,   640,   311,   513,   652,  1129,
     516,  -622,  1132,   671,   672,   649,   520,   395,   396,  1231,
     671,   672,   631,  -623,  1111,   393,   632,   810,  1112,  1113,
    1114,   533,   621,   623,     5,   308,  1140,   537,  1062,  1064,
    1065,  1066,   321,   500,   639,   641,   393,  1030,  1031,  1032,
    1033,  1336,   447,   718,  1339,  1252,   720,   843,   464,  1341,
       5,   467,  1344,   659,   470,  1267,  1345,    10,   326,     1,
     571,   -30,   501,  1289,   575,   216,   579,   581,  1153,   416,
      21,  1366,   106,   107,   236,   291,  1116,   433,   238,  1160,
    1161,   714,    17,   237,   618,   206,   207,   631,   625,  1365,
       7,   632,   271,   -59,    36,  1367,     5,     5,   -59,   -59,
     -59,   642,  1141,  1142,  1143,   667,   420,    31,    32,   669,
     670,   238,   583,  1103,   832,    17,   588,     5,   813,   833,
     487,   131,   215,   216,     5,   217,   218,   219,   220,   221,
     314,   315,   671,   672,    26,   768,   633,  1083,    29,   708,
     553,  1111,   554,   178,   491,  1112,  1113,  1114,   396,   432,
       5,     5,   395,  1258,  1258,  1259,  1259,   710,   846,   445,
     446,   659,   849,   220,   221,  1227,  1251,   724,   459,   814,
     815,   816,     5,  1254,   691,   466,  1268,  1259,   469,   691,
    1118,   472,   553,   691,   845,  1130,   473,   405,   138,   482,
    1136,   143,   -51,   146,   554,   148,   766,   150,   617,   152,
     593,   594,   624,  1116,   627,   668,   744,   629,   630,   878,
    1215,   637,  1222,   870,  1111,   554,   873,  1046,  1112,  1113,
    1114,  1007,   648,   672,     5,   318,   319,   320,   651,  1259,
     486,   653,   490,   973,   655,   765,   393,   974,   975,   976,
     233,   405,   820,   659,   849,   661,   671,   672,  1292,  1292,
     778,   905,   781,  1016,  1017,   784,   390,   787,   925,   790,
    1230,   793,   131,   215,   795,   921,   217,   218,   219,   768,
    1109,  1110,   799,   945,   946,   802,  1116,   804,   800,   801,
     854,   858,   807,  1111,  1046,   857,   986,  1112,  1113,  1114,
    1115,   215,   811,     5,   217,   218,   219,   220,   221,   766,
     577,   767,   667,  1069,  1236,   668,   669,   670,   768,  1152,
    1007,  1156,   829,  1111,   647,   982,  1131,  1112,  1113,  1114,
     147,  1275,   149,     5,   151,  1294,   153,  1268,  1259,   671,
     672,   673,  1287,  1046,   215,   216,   473,   217,   218,   219,
    1137,   967,     5,   768,   823,  1116,    43,   826,   272,   861,
     721,   667,   866,  1002,   668,   669,   670,   587,   890,   831,
     265,   876,   839,   158,  1117,   159,   994,   642,   996,   160,
     642,   161,  1023,   162,   266,  1116,   971,   659,   671,   672,
     673,   215,   532,   163,   217,   218,   219,   855,   164,   859,
     853,   646,   859,   393,  1224,   859,   165,   166,   859,   167,
     478,   859,   255,   168,   256,   662,   913,   691,   663,   169,
     664,   665,   170,     1,     2,   891,  1057,   171,   172,     3,
     173,   174,   796,    97,     4,   100,     5,     6,     5,  1349,
     103,  1085,     0,     7,     0,     0,     8,   667,     0,     0,
     668,   669,   670,     9,    10,     0,    11,     0,    12,   967,
       0,   659,     0,    13,   699,    14,   949,     0,    15,     0,
    1067,    16,     0,     0,   671,   672,   673,     0,     0,    17,
       5,     0,    18,     0,    19,    20,    21,    22,     0,   667,
       0,   829,   668,   669,   670,    23,    24,    25,    26,    27,
       0,    28,    29,    30,    31,    32,    33,     0,    34,    35,
      36,     0,     0,     0,     0,     0,   671,   672,   673,   215,
     216,   999,   217,   218,   219,   220,   221,     0,   215,   963,
    1004,   217,   218,   219,   220,   221,   967,     0,   767,  1111,
    1144,     0,     0,  1112,  1113,  1114,  1115,     0,     0,     5,
       0,   859,     0,   859,     0,     0,     0,   859,   667,  1020,
       0,   668,   669,   670,     0,     0,     0,   691,  1135,   215,
     216,     0,   217,   218,   219,   220,   221,     0,     0,   767,
       5,     0,     0,     3,     0,   671,   672,   673,  1043,   667,
       5,     6,   668,   669,   670,     0,     0,     0,   829,   667,
       8,  1116,   668,   669,   670,   967,   699,     9,   710,     0,
       0,     0,     0,     0,     0,     0,   671,   672,   673,    14,
    1117,   642,     0,  1081,     0,    16,   671,   672,   673,  1089,
       0,  1091,     0,  1094,     0,     0,    18,     0,     0,    20,
       0,    22,     0,     0,     0,     0,     0,     0,     0,   859,
       0,    25,     0,    27,     0,    28,     0,    30,     0,     0,
    1106,     0,     0,    35,   691,     0,     0,  1127,     0,  1127,
       0,     0,  1127,     0,     3,   967,     0,     0,     0,     0,
       0,     0,     6,     0,   829,     0,     0,     0,   131,   215,
     216,     8,   217,   218,   219,   220,   221,     0,     9,     0,
       0,    11,     0,  1150,     0,  1151,     0,   691,     0,     0,
       0,  1340,   691,     0,   447,     0,    16,   691,     0,     0,
    1162,     0,     5,     0,     0,     0,     0,    18,   829,   829,
      20,   667,    22,     0,   668,   669,   670,     0,     0,     0,
    1225,  1338,    25,   698,    27,     0,    28,     0,    30,  1343,
       0,     0,     0,     0,    35,     0,     0,     0,   671,   672,
     673,   600,   601,   602,   603,   604,   605,   606,   607,   608,
     609,   610,   611,   612,   613,     0,   186,     0,   187,   691,
     188,   189,     0,   190,     0,   191,   192,   691,   193,   194,
     195,   197,   195,   199,   195,   201,   202,     5,   204,     0,
     205,     0,     0,     0,     0,     0,   667,     0,     0,   668,
     669,   670,     0,  1255,   434,   436,  1135,     0,   439,   440,
     209,     0,   210,   211,   212,     0,   213,     0,     0,     0,
       0,   458,     0,   671,   672,   673,     0,     0,   465,     0,
       0,   468,     0,     0,   471,  1305,     0,  1308,     0,     0,
       0,     0,   481,     0,  1313,     0,     0,  1260,  1260,  1269,
    1269,     0,  1276,  1280,     0,  1285,  1260,  1290,  1290,     0,
    1295,     0,     0,     0,     0,   330,     0,   332,     0,     0,
       0,     0,     0,     0,   337,     0,   339,     0,     0,     0,
     342,   343,     0,     0,     0,     0,   344,   345,   346,   347,
       0,   348,   195,   350,     0,     0,     0,     0,     0,     0,
       0,     0,   353,     0,     0,     3,     0,   357,   358,     0,
     359,     0,     0,     6,     0,     0,   363,     0,     0,     0,
     365,   366,     8,     0,     0,   368,   369,     0,     0,     9,
     371,   372,     0,     0,     0,     0,     0,     0,   377,     0,
       0,    14,   380,     0,     0,   382,     0,    16,     0,   447,
       0,   384,   385,   386,     0,   387,     0,     5,    18,     0,
       0,    20,     0,    22,     0,     0,   667,     0,     0,   668,
     669,   670,     0,    25,     0,    27,     0,    28,   698,    30,
     447,     0,   880,   699,     0,    35,     0,     0,     5,     0,
       0,     0,     0,   671,   672,   673,     0,   667,     0,     0,
     668,   669,   670,     0,     0,     0,     0,     0,     0,   698,
       0,     0,     0,     0,   699,   600,   601,   602,   603,   604,
     605,   606,   607,   608,   671,   672,   673,     0,     0,     0,
       0,     0,   496,   497,     0,     0,     0,     0,     0,   332,
     502,     0,   505,     0,     0,     0,   506,   508,   509,     0,
     510,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   515,     0,     0,   517,   518,     0,   519,     0,   521,
     522,     0,   524,   525,     0,   526,     0,   528,   529,   530,
       0,     0,   353,     0,     0,     0,     0,   535,   536,     0,
       0,   539,   540,     0,     0,     0,     0,     0,   545,   546,
       0,   548,   549,     0,   551,   552,     0,     0,     0,     0,
       0,     0,     0,     0,   562,   563,   564,     0,     0,   568,
       0,   569,     0,   572,     0,   573,  1172,  1173,  1174,  1175,
    1176,  1177,  1178,  1179,  1180,  1181,  1182,  1183,  1184,  1185,
    1186,  1187,  1188,  1189,  1190,  1191,  1192,  1193,  1194,  1195,
    1196,  1197,  1198,  1199,  1200,  1201,  1202,  1203,  1204,  1205,
    1206,  1207,  1208,  1209,  1210,  1211,  1216,  1173,  1174,  1175,
    1176,  1177,  1178,  1179,  1217,  1181,  1182,  1183,  1184,  1185,
    1186,  1187,  1188,  1189,  1190,  1191,  1192,  1193,  1194,  1195,
    1196,  1197,  1198,  1199,  1200,  1201,  1202,  1203,  1204,  1218,
    1219,  1207,  1208,  1209,  1210,  1211,     0,     0,     0,     0,
     715,   374,   375,     0,   378,     0,     0,   502,     0,     0,
     502,     0,   506,     0,   722,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   727,   728,   729,
       0,     0,   730,   731,   732,   734,   732,     0,     0,   737,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   746,   747,   749,   747,     0,   751,     0,   752,     0,
     754,     0,   756,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   776,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   777,     0,   779,     0,     0,   782,     0,   785,     0,
     788,     0,   791,     0,   791,     0,     0,   564,     0,     0,
     797,     0,     0,     0,     0,   569,   569,     0,   573,     0,
       0,     0,   805,     0,     0,     0,     0,   808,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   531,     0,     0,
       0,     0,   534,     0,     0,     0,     0,     0,     0,   541,
     542,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1173,  1174,  1175,  1176,  1177,  1178,  1179,   560,  1181,  1182,
    1183,  1184,  1185,  1186,  1187,  1188,  1189,  1190,  1191,  1192,
    1193,  1194,  1195,  1196,  1197,  1198,  1199,  1200,  1201,  1202,
    1203,  1204,     0,     0,  1207,  1208,  1209,  1210,  1211,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   722,     0,   805,     0,     0,     0,     0,
       0,     0,     0,   903,     0,     0,   732,     0,     0,   732,
       0,   909,     0,   911,   747,     0,     0,     0,     0,   914,
       0,     0,   747,     0,     0,   916,   752,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   791,     0,
     928,     0,   791,     0,     0,     0,   931,   933,   791,     0,
       0,   937,   791,     0,   940,   791,     0,   942,     0,     0,
     943,     0,     0,     0,     0,   797,   797,   947,     0,   573,
       0,   951,   952,     0,     0,     0,     0,     0,     0,     0,
       0,   955,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   742,   743,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1006,     0,     0,
    1011,  1012,  1013,     0,     0,     0,     0,   909,   909,     0,
       0,     0,     0,  1019,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1022,     0,     0,     0,     0,  1026,
    1027,  1028,     0,  1029,   791,   791,   791,     0,  1034,     0,
    1035,     0,     0,     0,   573,     0,  1040,     0,   805,     0,
       0,     0,  1044,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   285,     0,   290,     0,   295,     0,     0,   307,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1087,     0,   573,     0,  1092,     0,     0,     0,  1096,  1097,
       0,     0,     0,  1100,  1101,     0,  1102,     0,     0,   791,
       0,  1104,     0,     0,     0,     0,     0,     0,     0,     0,
     805,     0,     0,     0,     0,  1108,     0,  1044,  1044,     0,
       0,     0,     0,     0,     0,     0,   900,   901,   902,   904,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   918,     0,     0,     0,     0,  1087,   573,
       0,  1154,  1155,     0,  1092,  1157,     0,     0,     0,     0,
    1159,   573,     0,  1163,     0,     0,     0,     0,     0,     0,
    1165,  1167,     0,     0,     0,     0,     0,     0,     0,   389,
       0,     0,     0,     0,     0,     0,   404,     0,   410,   411,
       0,     0,     0,  1235,  1165,  1237,     0,     0,     0,     0,
       0,     0,     0,  1238,   443,     0,     0,     0,     0,   451,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1239,  1240,
       0,     0,     0,     0,     0,     0,     0,  1242,  1243,  1245,
    1246,  1247,  1248,     0,     0,  1250,     0,   573,     0,   805,
       0,     0,     0,     0,   573,     0,     0,     0,     0,  1014,
       0,     0,  1015,     0,     0,     0,     0,  1018,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1021,     0,
       0,     0,   514,     0,     0,     0,  1303,     0,  1306,     0,
       0,     0,     0,   523,     0,  1311,     0,  1314,     0,     0,
       0,     0,  1318,     0,     0,     0,  1322,     0,     0,     0,
    1325,     0,   538,     0,     0,  1329,     0,     0,   543,   544,
       0,     0,   547,     0,     0,   550,     0,     0,     0,     0,
       0,     0,   559,     0,     0,   561,     0,     0,     0,     0,
     567,     0,     0,     0,     0,     0,     0,     0,     0,   576,
       0,     0,     0,  1095,     0,     0,     0,  1167,     0,     0,
    1351,  1353,   590,     0,     0,  1357,   597,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   636,     0,     0,     0,     0,     0,     0,   644,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     695,     0,     0,     0,     0,   707,     0,     0,     0,   713,
       0,     0,     0,  1158,     0,   716,     0,   717,     0,     0,
       0,   719,     0,     0,     0,     0,     0,   723,  1169,     0,
       0,     0,     0,     0,   726,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   736,
       0,     0,   738,   739,   740,   741,     0,     0,     0,     0,
       0,     0,   745,     0,     0,     0,     0,     0,     0,     0,
       0,   753,     0,   755,     0,   757,   758,   759,   760,   761,
     762,   763,     0,     0,     0,     0,   775,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1253,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1300,     0,     0,     0,  1301,
    1302,     0,  1304,     0,  1307,     0,     0,     0,  1309,  1310,
       0,  1312,     0,  1315,     0,  1316,  1317,     0,  1319,  1320,
    1321,     0,  1323,     0,  1324,     0,  1326,  1327,   825,  1328,
     827,  1330,  1331,     0,  1332,     0,  1333,     0,     0,     0,
       0,     0,     0,   838,     0,   840,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1350,  1352,     0,  1354,     0,
    1356,  1358,     0,  1360,  1361,  1362,  1363,  1364,     0,     0,
       0,     0,     0,   887,     0,     0,     0,     0,     0,     0,
       0,   892,     0,     0,  1368,     0,  1370,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   906,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     919,     0,     0,     0,     0,     0,     0,     0,   926,     0,
       0,     0,     0,     0,     0,     0,     0,   930,     0,     0,
       0,     0,   936,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   944,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   953,     0,     0,     0,     0,
       0,     0,   954,     0,   956,   957,   958,   959,     0,     0,
       0,     0,   961,   962,     0,   966,     0,   969,     0,     0,
     972,     0,   980,   981,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1025,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1037,     0,  1039,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1053,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1070,     0,     0,     0,     0,  1080,     0,     0,     0,
    1086,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1133,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1146,     0,  1148,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1164,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1223,     0,     0,     0,     0,     0,     0,
       0,  1229,     0,  1233,  1234,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1241,     0,     0,
       0,     0,     0,     0,     0,  1249,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1257,     0,     0,     0,     0,     0,     0,     0,  1284,
       0,     0,     0,     0,     0,     0,     0,  1298,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1334,  1335,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1355,     0,     0,  1359,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1369
};

static const yytype_int16 yycheck[] =
{
       0,   326,   186,     0,     0,   222,    45,   229,   214,   353,
     190,   903,   194,    52,    53,   406,    55,   332,    57,   406,
      59,   812,    61,   797,   128,   129,    65,   205,    67,   797,
      69,   201,    71,   199,    73,   913,    75,   705,    77,   197,
      79,   782,   568,   799,   189,   497,   802,   564,    87,   194,
     230,   558,   569,  1019,   623,    94,   850,    96,     0,    98,
     382,   100,     0,   102,  1081,   104,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   189,   127,   128,   129,   358,   660,
    1006,   641,   777,   657,   779,     6,   951,   782,   782,  1243,
     785,   785,   149,   788,   151,   797,   791,  1022,  1012,  1013,
     327,     6,     7,   345,    24,     5,    49,    12,   793,     4,
     683,   732,   685,    49,   687,    15,   689,     5,   691,    39,
    1096,    14,     0,     6,    17,   909,   747,    15,    48,    22,
      73,    35,  1020,    16,   186,   184,   330,    17,    14,  1239,
    1240,    17,  1242,  1243,    87,  1245,    26,  1247,  1248,   376,
    1250,    87,    56,    17,     7,    65,   383,    77,   348,    12,
      81,    61,  1087,   390,   782,   357,   146,   785,  1322,   947,
     788,   955,   388,    61,    62,    55,    81,   928,  1092,    89,
     984,    85,    86,   508,    79,   195,   235,    51,    83,  1165,
      17,   240,   202,   242,   382,    88,   245,   246,    81,    26,
      40,   737,   259,  1303,  1005,  1087,   884,   909,   886,    67,
     535,  1311,    88,   162,    35,  1097,   265,   266,  1106,  1023,
       9,   270,   728,   729,   273,  1325,    53,    54,  1255,  1329,
     162,   280,   564,    91,   928,   284,   931,   931,   933,   191,
     289,  1167,   937,  1108,   823,   294,   194,   826,   754,    38,
     299,   768,   484,   715,  1058,   304,   536,   306,   943,  1235,
    1044,   310,  1150,  1151,    85,    86,     4,   509,   317,   850,
     797,   831,   808,   847,   323,  1026,  1077,  1043,   330,   839,
     522,   336,   855,   335,   336,   512,   859,    17,    19,    17,
     863,   864,   865,    30,    31,   868,   869,    28,    26,   872,
     162,   356,     7,   355,   356,    65,    11,    12,    13,    14,
     928,    71,   364,   931,     3,   933,    53,    54,    56,   937,
      50,    51,    60,  1001,   376,   660,    54,   379,   555,    89,
     337,   558,  1026,  1027,  1029,    61,    25,   344,     9,   566,
       5,    79,    17,   353,   350,    83,    17,    17,   540,    17,
      15,    26,   401,   363,   403,    23,    26,  1239,  1240,   408,
    1242,    31,   517,   518,    69,    17,   598,    38,    31,   418,
      22,  1049,   552,   549,   529,   530,   564,   426,   546,  1091,
     429,   162,  1094,    53,    54,   540,   435,    32,    33,  1167,
      53,    54,     7,   162,     7,   730,    11,   722,    11,    12,
      13,   450,   517,   518,    17,    64,  1100,   456,  1026,  1027,
    1028,  1029,    41,   810,   529,   530,   751,   990,   991,   992,
     993,  1303,     9,   584,  1306,  1226,   587,   752,   365,  1311,
      17,   368,  1314,   660,   371,  1238,  1318,    35,   149,     4,
     489,   149,   843,  1246,   493,   148,   498,   499,  1126,    57,
      67,  1353,   158,   159,    19,    72,    69,    48,    56,  1137,
    1138,   575,    60,    28,   516,   160,   161,     7,   520,  1351,
      24,    11,    70,   155,    91,  1357,    17,    17,   160,   161,
     162,   533,  1100,  1101,  1102,    26,    16,    85,    86,    30,
      31,    56,   502,  1066,     7,    60,   506,    17,   725,    12,
      25,   146,   147,   148,    17,   150,   151,   152,   153,   154,
      74,    75,    53,    54,    79,   850,   526,  1044,    83,   571,
     154,     7,   156,   575,    47,    11,    12,    13,    33,   341,
      17,    17,    32,    20,    20,    22,    22,    45,   765,   351,
     352,   768,   769,   153,   154,  1163,  1224,   596,   360,   150,
     151,   152,    17,  1231,   564,   367,    21,    22,   370,   569,
    1087,   373,   154,   573,   756,  1092,   793,    62,   111,   381,
    1097,   114,   154,   116,   156,   118,   155,   120,   515,   122,
      58,    59,   519,    69,   521,    29,   635,   524,   525,   146,
    1155,   528,  1157,   787,     7,   156,   790,     8,    11,    12,
      13,   955,   539,    54,    17,   210,   211,   212,   545,    22,
     384,   548,   386,     7,   551,   658,   951,    11,    12,    13,
      61,    62,   730,   850,   851,   562,    53,    54,  1247,  1248,
     679,   821,   681,   964,   965,   684,   326,   686,   851,   688,
    1167,   690,   146,   147,   693,   848,   150,   151,   152,   984,
    1084,  1085,   701,   882,   883,   704,    69,   706,   702,   703,
     777,   779,   711,     7,     8,   778,   928,    11,    12,    13,
      14,   147,   724,    17,   150,   151,   152,   153,   154,   155,
     496,   157,    26,  1035,  1171,    29,    30,    31,  1023,  1125,
    1044,  1131,   744,     7,   537,   922,  1092,    11,    12,    13,
     117,  1240,   119,    17,   121,  1248,   123,    21,    22,    53,
      54,    55,  1245,     8,   147,   148,   943,   150,   151,   152,
    1097,   911,    17,  1058,   731,    69,     0,   734,   194,   781,
     589,    26,   784,   949,    29,    30,    31,   505,   804,   746,
     192,   793,   749,   126,    88,   126,   940,   799,   942,   126,
     802,   126,   984,   126,   192,    69,   914,   984,    53,    54,
      55,   147,   449,   126,   150,   151,   152,   777,   126,   779,
     776,   536,   782,  1108,  1159,   785,   126,   126,   788,   126,
     377,   791,   189,   126,   189,   563,   835,   797,   563,   126,
     563,   563,   126,     4,     5,   805,  1023,   126,   126,    10,
     126,   126,   694,     0,    15,     0,    17,    18,    17,  1334,
       0,  1044,    -1,    24,    -1,    -1,    27,    26,    -1,    -1,
      29,    30,    31,    34,    35,    -1,    37,    -1,    39,  1019,
      -1,  1058,    -1,    44,    43,    46,   885,    -1,    49,    -1,
    1034,    52,    -1,    -1,    53,    54,    55,    -1,    -1,    60,
      17,    -1,    63,    -1,    65,    66,    67,    68,    -1,    26,
      -1,   913,    29,    30,    31,    76,    77,    78,    79,    80,
      -1,    82,    83,    84,    85,    86,    87,    -1,    89,    90,
      91,    -1,    -1,    -1,    -1,    -1,    53,    54,    55,   147,
     148,   943,   150,   151,   152,   153,   154,    -1,   147,   909,
     952,   150,   151,   152,   153,   154,  1096,    -1,   157,     7,
    1104,    -1,    -1,    11,    12,    13,    14,    -1,    -1,    17,
      -1,   931,    -1,   933,    -1,    -1,    -1,   937,    26,   978,
      -1,    29,    30,    31,    -1,    -1,    -1,   947,    36,   147,
     148,    -1,   150,   151,   152,   153,   154,    -1,    -1,   157,
      17,    -1,    -1,    10,    -1,    53,    54,    55,  1007,    26,
      17,    18,    29,    30,    31,    -1,    -1,    -1,  1020,    26,
      27,    69,    29,    30,    31,  1165,    43,    34,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    53,    54,    55,    46,
      88,  1043,    -1,  1042,    -1,    52,    53,    54,    55,  1048,
      -1,  1050,    -1,  1052,    -1,    -1,    63,    -1,    -1,    66,
      -1,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1029,
      -1,    78,    -1,    80,    -1,    82,    -1,    84,    -1,    -1,
    1079,    -1,    -1,    90,  1044,    -1,    -1,  1089,    -1,  1091,
      -1,    -1,  1094,    -1,    10,  1235,    -1,    -1,    -1,    -1,
      -1,    -1,    18,    -1,  1106,    -1,    -1,    -1,   146,   147,
     148,    27,   150,   151,   152,   153,   154,    -1,    34,    -1,
      -1,    37,    -1,  1122,    -1,  1124,    -1,  1087,    -1,    -1,
      -1,  1308,  1092,    -1,     9,    -1,    52,  1097,    -1,    -1,
    1139,    -1,    17,    -1,    -1,    -1,    -1,    63,  1150,  1151,
      66,    26,    68,    -1,    29,    30,    31,    -1,    -1,    -1,
    1162,  1305,    78,    38,    80,    -1,    82,    -1,    84,  1313,
      -1,    -1,    -1,    -1,    90,    -1,    -1,    -1,    53,    54,
      55,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,    -1,   133,    -1,   135,  1159,
     137,   138,    -1,   140,    -1,   142,   143,  1167,   145,   146,
     147,   148,   149,   150,   151,   152,   153,    17,   155,    -1,
     157,    -1,    -1,    -1,    -1,    -1,    26,    -1,    -1,    29,
      30,    31,    -1,  1232,   342,   343,    36,    -1,   346,   347,
     177,    -1,   179,   180,   181,    -1,   183,    -1,    -1,    -1,
      -1,   359,    -1,    53,    54,    55,    -1,    -1,   366,    -1,
      -1,   369,    -1,    -1,   372,  1264,    -1,  1266,    -1,    -1,
      -1,    -1,   380,    -1,  1273,    -1,    -1,  1237,  1238,  1239,
    1240,    -1,  1242,  1243,    -1,  1245,  1246,  1247,  1248,    -1,
    1250,    -1,    -1,    -1,    -1,   232,    -1,   234,    -1,    -1,
      -1,    -1,    -1,    -1,   241,    -1,   243,    -1,    -1,    -1,
     247,   248,    -1,    -1,    -1,    -1,   253,   254,   255,   256,
      -1,   258,   259,   260,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   269,    -1,    -1,    10,    -1,   274,   275,    -1,
     277,    -1,    -1,    18,    -1,    -1,   283,    -1,    -1,    -1,
     287,   288,    27,    -1,    -1,   292,   293,    -1,    -1,    34,
     297,   298,    -1,    -1,    -1,    -1,    -1,    -1,   305,    -1,
      -1,    46,   309,    -1,    -1,   312,    -1,    52,    -1,     9,
      -1,   318,   319,   320,    -1,   322,    -1,    17,    63,    -1,
      -1,    66,    -1,    68,    -1,    -1,    26,    -1,    -1,    29,
      30,    31,    -1,    78,    -1,    80,    -1,    82,    38,    84,
       9,    -1,    42,    43,    -1,    90,    -1,    -1,    17,    -1,
      -1,    -1,    -1,    53,    54,    55,    -1,    26,    -1,    -1,
      29,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,    38,
      -1,    -1,    -1,    -1,    43,    92,    93,    94,    95,    96,
      97,    98,    99,   100,    53,    54,    55,    -1,    -1,    -1,
      -1,    -1,   399,   400,    -1,    -1,    -1,    -1,    -1,   406,
     407,    -1,   409,    -1,    -1,    -1,   413,   414,   415,    -1,
     417,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   428,    -1,    -1,   431,   432,    -1,   434,    -1,   436,
     437,    -1,   439,   440,    -1,   442,    -1,   444,   445,   446,
      -1,    -1,   449,    -1,    -1,    -1,    -1,   454,   455,    -1,
      -1,   458,   459,    -1,    -1,    -1,    -1,    -1,   465,   466,
      -1,   468,   469,    -1,   471,   472,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   481,   482,   483,    -1,    -1,   486,
      -1,   488,    -1,   490,    -1,   492,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,    -1,    -1,    -1,    -1,
     577,   302,   303,    -1,   305,    -1,    -1,   584,    -1,    -1,
     587,    -1,   589,    -1,   591,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   614,   615,   616,
      -1,    -1,   619,   620,   621,   622,   623,    -1,    -1,   626,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   638,   639,   640,   641,    -1,   643,    -1,   645,    -1,
     647,    -1,   649,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   666,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   678,    -1,   680,    -1,    -1,   683,    -1,   685,    -1,
     687,    -1,   689,    -1,   691,    -1,    -1,   694,    -1,    -1,
     697,    -1,    -1,    -1,    -1,   702,   703,    -1,   705,    -1,
      -1,    -1,   709,    -1,    -1,    -1,    -1,   714,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   448,    -1,    -1,
      -1,    -1,   453,    -1,    -1,    -1,    -1,    -1,    -1,   460,
     461,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     107,   108,   109,   110,   111,   112,   113,   478,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,    -1,    -1,   141,   142,   143,   144,   145,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   810,    -1,   812,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   820,    -1,    -1,   823,    -1,    -1,   826,
      -1,   828,    -1,   830,   831,    -1,    -1,    -1,    -1,   836,
      -1,    -1,   839,    -1,    -1,   842,   843,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   855,    -1,
     857,    -1,   859,    -1,    -1,    -1,   863,   864,   865,    -1,
      -1,   868,   869,    -1,   871,   872,    -1,   874,    -1,    -1,
     877,    -1,    -1,    -1,    -1,   882,   883,   884,    -1,   886,
      -1,   888,   889,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   898,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   633,   634,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   954,    -1,    -1,
     957,   958,   959,    -1,    -1,    -1,    -1,   964,   965,    -1,
      -1,    -1,    -1,   970,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   981,    -1,    -1,    -1,    -1,   986,
     987,   988,    -1,   990,   991,   992,   993,    -1,   995,    -1,
     997,    -1,    -1,    -1,  1001,    -1,  1003,    -1,  1005,    -1,
      -1,    -1,  1009,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   196,    -1,   198,    -1,   200,    -1,    -1,   203,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1047,    -1,  1049,    -1,  1051,    -1,    -1,    -1,  1055,  1056,
      -1,    -1,    -1,  1060,  1061,    -1,  1063,    -1,    -1,  1066,
      -1,  1068,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1077,    -1,    -1,    -1,    -1,  1082,    -1,  1084,  1085,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   817,   818,   819,   820,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   844,    -1,    -1,    -1,    -1,  1125,  1126,
      -1,  1128,  1129,    -1,  1131,  1132,    -1,    -1,    -1,    -1,
    1137,  1138,    -1,  1140,    -1,    -1,    -1,    -1,    -1,    -1,
    1147,  1148,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   324,
      -1,    -1,    -1,    -1,    -1,    -1,   331,    -1,   333,   334,
      -1,    -1,    -1,  1170,  1171,  1172,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1180,   349,    -1,    -1,    -1,    -1,   354,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1205,  1206,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1214,  1215,  1216,
    1217,  1218,  1219,    -1,    -1,  1222,    -1,  1224,    -1,  1226,
      -1,    -1,    -1,    -1,  1231,    -1,    -1,    -1,    -1,   960,
      -1,    -1,   963,    -1,    -1,    -1,    -1,   968,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   979,    -1,
      -1,    -1,   427,    -1,    -1,    -1,  1263,    -1,  1265,    -1,
      -1,    -1,    -1,   438,    -1,  1272,    -1,  1274,    -1,    -1,
      -1,    -1,  1279,    -1,    -1,    -1,  1283,    -1,    -1,    -1,
    1287,    -1,   457,    -1,    -1,  1292,    -1,    -1,   463,   464,
      -1,    -1,   467,    -1,    -1,   470,    -1,    -1,    -1,    -1,
      -1,    -1,   477,    -1,    -1,   480,    -1,    -1,    -1,    -1,
     485,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   494,
      -1,    -1,    -1,  1054,    -1,    -1,    -1,  1334,    -1,    -1,
    1337,  1338,   507,    -1,    -1,  1342,   511,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   527,    -1,    -1,    -1,    -1,    -1,    -1,   534,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     565,    -1,    -1,    -1,    -1,   570,    -1,    -1,    -1,   574,
      -1,    -1,    -1,  1134,    -1,   580,    -1,   582,    -1,    -1,
      -1,   586,    -1,    -1,    -1,    -1,    -1,   592,  1149,    -1,
      -1,    -1,    -1,    -1,   599,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   624,
      -1,    -1,   627,   628,   629,   630,    -1,    -1,    -1,    -1,
      -1,    -1,   637,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   646,    -1,   648,    -1,   650,   651,   652,   653,   654,
     655,   656,    -1,    -1,    -1,    -1,   661,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1228,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1256,    -1,    -1,    -1,  1260,
    1261,    -1,  1263,    -1,  1265,    -1,    -1,    -1,  1269,  1270,
      -1,  1272,    -1,  1274,    -1,  1276,  1277,    -1,  1279,  1280,
    1281,    -1,  1283,    -1,  1285,    -1,  1287,  1288,   733,  1290,
     735,  1292,  1293,    -1,  1295,    -1,  1297,    -1,    -1,    -1,
      -1,    -1,    -1,   748,    -1,   750,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1336,  1337,    -1,  1339,    -1,
    1341,  1342,    -1,  1344,  1345,  1346,  1347,  1348,    -1,    -1,
      -1,    -1,    -1,   798,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   806,    -1,    -1,  1365,    -1,  1367,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   822,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     845,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   853,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   862,    -1,    -1,
      -1,    -1,   867,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   879,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   890,    -1,    -1,    -1,    -1,
      -1,    -1,   897,    -1,   899,   900,   901,   902,    -1,    -1,
      -1,    -1,   907,   908,    -1,   910,    -1,   912,    -1,    -1,
     915,    -1,   917,   918,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     985,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1000,    -1,  1002,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1018,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1036,    -1,    -1,    -1,    -1,  1041,    -1,    -1,    -1,
    1045,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1095,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1105,    -1,  1107,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1145,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1158,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1166,    -1,  1168,  1169,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1212,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1220,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1236,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1244,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1252,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1299,  1300,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1340,    -1,    -1,  1343,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1366
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
     334,   335,   338,   339,   341,   343,   351,   352,   358,   359,
     370,   372,   383,   384,   385,   392,   393,   394,   395,   396,
     397,   398,   399,   400,   401,   402,   403,   404,   405,   407,
     428,   429,   430,   432,   434,   435,   441,   442,   447,   452,
     455,   458,   461,   462,   465,   470,   472,   475,   477,   487,
     489,   490,   493,   495,   496,     0,   158,   159,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   146,   204,   240,   204,   310,   204,   337,   337,   204,
     344,   204,   353,   337,   204,   373,   337,   353,   337,   353,
     337,   353,   337,   353,   204,   443,   204,   466,   332,   333,
     334,   335,   358,   384,   385,   395,   399,   403,   428,   441,
     447,   452,   455,   458,   461,   471,   473,   474,   204,   478,
     478,   478,    40,   479,   480,   162,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   354,   203,   354,   203,
     354,   203,   203,   406,   203,   203,   160,   161,   202,   203,
     203,   203,   203,   203,   201,   147,   148,   150,   151,   152,
     153,   154,   167,   168,   169,   171,   173,   174,   177,   179,
     180,   204,   241,    61,   311,   313,    19,    28,    56,   304,
     309,   320,   321,   340,   351,   366,   367,   431,   433,   436,
     438,   439,   440,   320,   340,   431,   433,   171,   345,   341,
     360,   361,   363,   364,   365,   366,   367,     3,    25,   374,
     375,    70,   307,   309,   320,   386,   393,   448,   449,   450,
     451,    23,   237,   355,   356,   202,    71,   397,   453,   454,
     202,    72,   401,   456,   457,   202,    73,   405,   459,   460,
      50,    51,   237,   408,   409,   411,   412,   202,    64,   444,
     445,   462,   463,   495,    74,    75,   467,   468,   479,   479,
     479,    41,   481,   482,   166,   179,   149,   167,   178,   171,
     203,   238,   203,   305,   308,   201,   201,   203,   201,   203,
     201,   201,   203,   203,   203,   203,   203,   203,   203,   354,
     203,   201,   201,   203,   371,   201,   201,   203,   203,   203,
     201,   203,   258,   203,   201,   203,   203,   201,   203,   203,
     201,   203,   203,   201,   258,   258,   201,   203,   258,   201,
     203,   201,   203,   201,   203,   203,   203,   203,   201,   202,
     172,   175,   176,   180,   179,    32,    33,   168,   204,   242,
     243,   244,   246,   247,   202,    62,   311,   316,   317,   341,
     202,   202,   204,   314,   310,   351,    57,   322,   323,     6,
      16,    81,   267,   268,   270,   271,   273,   336,   347,   348,
     204,   368,   368,    48,   414,   416,   414,   351,   336,   414,
     414,   171,   346,   202,   370,   368,   368,     9,   237,   377,
     379,   202,   204,   376,   310,   393,   273,   387,   414,   368,
     237,   237,   204,   357,   267,   414,   368,   267,   414,   368,
     267,   414,   368,   179,   183,   185,   204,   410,   408,   204,
     413,   414,   368,   463,   179,   469,   481,    25,   418,   419,
     481,    47,   497,   500,   166,   179,   203,   203,   201,   201,
     305,   308,   203,   306,   201,   203,   203,   312,   203,   203,
     203,   325,   201,   201,   202,   203,   201,   203,   203,   203,
     201,   203,   203,   202,   203,   203,   203,   342,   203,   203,
     203,   258,   371,   201,   258,   203,   203,   201,   202,   203,
     203,   258,   258,   202,   202,   203,   203,   202,   203,   203,
     202,   203,   203,   154,   156,   182,   184,   188,   191,   202,
     258,   202,   203,   203,   203,   464,   167,   202,   203,   203,
     488,   201,   203,   203,   206,   201,   202,   243,   246,   204,
     245,   204,   248,   237,   319,   168,   318,   316,   237,   315,
     202,   311,   336,    58,    59,   326,   328,   202,   179,   324,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   274,   275,   276,   267,   204,   349,
     320,   340,   320,   340,   267,   204,   417,   267,   336,   267,
     267,     7,    11,   237,   259,   263,   202,   267,   320,   340,
     320,   340,   204,   380,   202,   311,   387,   274,   267,   320,
     393,   267,   397,   267,   401,   267,   405,   179,   186,   179,
     192,   267,   436,   438,   439,   440,   446,    26,    29,    30,
      31,    53,    54,    55,   207,   209,   210,   211,   212,   213,
     214,   215,   218,   219,   220,   222,   223,   228,   230,   233,
     234,   237,   249,   250,   463,   202,   179,   418,    38,    43,
     207,   379,   421,   425,   426,   485,   486,   202,   204,   420,
      45,   494,   207,   202,   478,   203,   202,   202,   306,   202,
     306,   312,   203,   202,   201,   167,   202,   203,   203,   203,
     203,   203,   203,   437,   203,   437,   202,   203,   202,   202,
     202,   202,   258,   258,   201,   202,   203,   203,   362,   203,
     362,   203,   203,   202,   203,   202,   203,   202,   202,   202,
     202,   202,   202,   202,   184,   182,   155,   157,   180,   181,
     189,   192,   197,   198,   199,   202,   203,   203,   201,   203,
     208,   201,   203,   208,   201,   203,   208,   201,   203,   208,
     201,   203,   208,   201,   208,   201,   464,   203,   476,   201,
     488,   488,   201,   206,   201,   203,   327,   201,   203,   246,
     311,   204,   329,   179,   150,   151,   152,   277,   277,   277,
     170,   172,   350,   351,   347,   202,   351,   202,   418,   204,
     265,   351,     7,    12,   257,   262,   347,   369,   202,   351,
     202,   170,   381,   311,   277,   393,   179,   187,   192,   179,
     193,   181,   200,   370,   211,   237,   249,   214,   218,   237,
     249,   204,   216,   222,   228,   233,   204,   221,   228,   233,
     168,   224,   233,   168,   231,   183,   204,   235,   146,   205,
      42,   207,   421,   425,   483,   484,   485,   202,   380,   380,
     318,   237,   202,   370,   414,   475,   489,   491,   418,   327,
     258,   258,   258,   203,   258,   171,   202,   437,   437,   203,
     415,   203,   362,   201,   203,   362,   203,   378,   258,   202,
     184,   200,   181,   190,   197,   193,   202,   208,   203,   208,
     202,   203,   208,   203,   208,   208,   202,   203,   208,   208,
     203,   208,   203,   203,   202,   476,   476,   203,   206,   201,
     206,   203,   203,   202,   202,   203,   202,   202,   202,   202,
     257,   202,   202,   237,   421,   425,   202,   171,   266,   202,
     265,   369,   202,     7,    11,    12,    13,   255,   256,   382,
     202,   202,   179,   194,   195,   217,   219,   222,   228,   233,
     228,   233,   233,   233,   168,   225,   168,   232,   183,   204,
     236,   485,   166,   381,   204,   427,   203,   379,   425,   498,
     501,   203,   203,   203,   258,   258,   415,   415,   258,   203,
     201,   258,   203,   167,   197,   202,   203,   203,   203,   203,
     208,   208,   208,   208,   203,   203,   229,   202,   206,   202,
     203,   327,   494,   201,   203,   499,     8,   278,   281,   279,
     281,   280,   281,   202,   266,   265,   278,   179,   196,   197,
     222,   228,   233,   228,   233,   233,   233,   168,   226,   259,
     202,     7,    11,    12,    13,    14,    69,   422,   423,   424,
     202,   201,   380,   207,   425,   501,   202,   203,   269,   201,
     206,   201,   203,   272,   201,   258,   203,   203,   388,   197,
     203,   203,   203,   208,   203,   327,   201,   491,   203,   499,
     499,     7,    11,    12,    13,    14,    69,    88,   207,   251,
     252,   253,   254,   260,   264,   278,   302,   204,   282,   282,
     207,   280,   282,   202,   266,    36,   207,   302,   389,   390,
     228,   233,   233,   233,   168,   227,   202,   265,   202,   381,
     201,   201,   269,   206,   203,   203,   272,   203,   258,   203,
     206,   206,   201,   203,   202,   203,   261,   203,   492,   258,
     265,   265,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   283,   286,   295,   295,   106,   114,   139,   140,
     288,   291,   295,   202,   389,   204,   391,   233,   266,   202,
     207,   485,   494,   202,   202,   203,   261,   203,   203,   203,
     203,   202,   203,   203,   293,   203,   203,   203,   203,   202,
     203,   206,   327,   258,   206,   201,   266,   202,    20,    22,
     237,   252,   284,   296,   297,   300,   301,   284,    21,   237,
     252,   285,   298,   299,   300,   285,   237,   252,   287,   300,
     237,   260,   294,   300,   202,   237,   289,   296,   300,   284,
     237,   290,   298,   300,   290,   237,   292,   300,   202,   491,
     258,   258,   258,   203,   258,   201,   203,   258,   201,   258,
     258,   203,   258,   201,   203,   258,   258,   258,   203,   258,
     258,   258,   203,   258,   258,   203,   258,   258,   258,   203,
     258,   258,   258,   258,   202,   202,   252,   300,   168,   252,
     179,   252,   300,   168,   252,   252,   260,   300,   300,   492,
     258,   203,   258,   203,   258,   202,   258,   203,   258,   202,
     258,   258,   258,   258,   258,   252,   257,   252,   258,   202,
     258
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   163,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   165,   165,   165,   165,   165,   165,   166,   166,
     167,   168,   168,   169,   170,   171,   171,   172,   172,   173,
     174,   175,   176,   177,   177,   178,   178,   179,   179,   179,
     179,   180,   180,   181,   182,   183,   183,   183,   184,   184,
     185,   186,   187,   188,   189,   189,   190,   190,   191,   192,
     193,   194,   194,   194,   195,   196,   197,   197,   198,   199,
     199,   200,   200,   201,   201,   202,   202,   203,   204,   205,
     206,   206,   207,   207,   207,   208,   208,   208,   209,   209,
     210,   210,   210,   211,   211,   211,   211,   212,   213,   214,
     215,   216,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   229,   230,   231,   232,   233,   234,   235,
     235,   236,   236,   237,   238,   238,   238,   238,   238,   238,
     238,   239,   240,   241,   241,   242,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   251,   252,   253,   254,
     254,   254,   254,   254,   255,   256,   256,   256,   256,   257,
     258,   258,   259,   260,   261,   261,   262,   262,   263,   263,
     264,   264,   265,   266,   267,   267,   267,   268,   269,   269,
     269,   269,   270,   271,   272,   272,   272,   273,   274,   274,
     274,   274,   274,   274,   274,   274,   274,   275,   275,   275,
     276,   276,   277,   277,   277,   278,   279,   280,   281,   282,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   284,
     284,   284,   284,   284,   284,   284,   284,   285,   285,   285,
     285,   285,   285,   285,   285,   286,   286,   287,   287,   287,
     287,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     289,   289,   289,   289,   290,   290,   290,   290,   291,   291,
     292,   292,   293,   293,   294,   294,   294,   294,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   296,   297,   298,   299,   300,   301,
     302,   303,   303,   304,   305,   305,   306,   306,   307,   308,
     308,   309,   310,   311,   312,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   324,   324,
     325,   325,   326,   327,   327,   328,   328,   329,   330,   330,
     330,   331,   331,   332,   333,   334,   335,   336,   336,   337,
     338,   338,   339,   339,   340,   340,   341,   342,   342,   342,
     343,   343,   344,   345,   346,   347,   348,   348,   349,   350,
     350,   351,   352,   352,   352,   353,   354,   354,   354,   354,
     355,   356,   357,   358,   359,   359,   360,   360,   360,   360,
     361,   362,   362,   362,   362,   363,   364,   365,   366,   367,
     368,   369,   370,   371,   371,   371,   372,   373,   374,   375,
     375,   376,   377,   378,   378,   379,   380,   381,   382,   383,
     383,   384,   385,   386,   386,   387,   388,   388,   388,   388,
     388,   389,   390,   391,   392,   392,   393,   394,   394,   394,
     395,   396,   396,   397,   398,   398,   399,   400,   401,   402,
     402,   403,   404,   405,   406,   406,   406,   406,   406,   407,
     407,   408,   409,   410,   410,   411,   412,   413,   414,   415,
     415,   415,   415,   416,   417,   418,   419,   420,   421,   422,
     423,   424,   424,   424,   424,   424,   424,   425,   426,   427,
     428,   428,   428,   429,   429,   430,   431,   431,   432,   433,
     433,   434,   435,   436,   437,   437,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   446,   446,   446,   447,   448,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   459,   460,   461,   462,   462,   462,   462,   462,   462,
     462,   462,   462,   462,   462,   462,   463,   463,   464,   464,
     464,   465,   466,   467,   468,   468,   469,   469,   469,   470,
     471,   471,   472,   473,   473,   473,   473,   473,   473,   473,
     473,   473,   473,   473,   473,   473,   473,   474,   474,   474,
     474,   474,   474,   474,   475,   476,   476,   476,   476,   476,
     476,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   488,   488,   488,   488,   489,   490,
     491,   491,   491,   491,   492,   492,   492,   492,   493,   494,
     495,   496,   497,   498,   498,   499,   499,   499,   499,   500,
     501
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     1,     1,     0,     1,     3,
       1,     1,     2,     2,     2,     0,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     4,     6,     0,     1,
       1,     1,     1,     3,     3,     1,     2,     1,     1,     1,
       1,     3,     4,     2,     1,     1,     1,     1,     1,     3,
       2,     0,     2,     1,     1,     1,     1,     1,     1,     1,
       0,     2,     1,     2,     1,     0,     3,     2,     1,     1,
       3,     2,     1,     1,     3,     4,     3,     6,     1,     4,
       1,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     5,     5,     5,     5,     7,     2,     2,     2,
       2,     4,     4,     4,     4,     4,     4,     6,     6,     6,
       6,     8,     4,     1,     1,    10,     1,     1,     1,     1,
       1,     7,     0,     2,     1,     1,     1,     6,     1,     1,
       1,     1,     1,     7,     0,     2,     4,     6,     2,     4,
       2,     1,     1,     1,     1,     1,     1,     4,     1,     1,
       4,     1,     1,     4,     1,     1,     1,     1,     7,     1,
       1,     1,     1,     1,     7,     1,     1,     1,     1,     7,
       0,     3,     7,     5,     0,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,    10,     0,     3,
       3,     2,    10,    10,     0,     3,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     6,     7,     6,     1,     1,
       1,     1,     3,     1,     3,     1,     3,     1,     3,     2,
       4,     6,     4,     2,     4,     2,     2,     2,     4,     6,
       4,     2,     4,     2,     2,     1,     3,     2,     2,     4,
       2,     1,     1,     3,     1,     3,     1,     3,     1,     3,
       2,     4,     2,     2,     2,     4,     2,     2,     1,     3,
       2,     2,     0,     2,     2,     2,     4,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     6,     1,     4,     1,     4,     1,
       1,     1,     1,     7,     5,     3,     0,     3,     7,     3,
       3,     1,     1,     5,     0,     3,     1,     1,     1,     4,
       1,     1,     1,     5,     1,     4,     1,     1,     2,     3,
       0,     2,     5,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     8,    10,     8,    10,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     9,     0,     3,     3,
       1,     1,     1,     1,     1,     6,     1,     1,     1,     4,
       2,     7,     1,     1,     1,     1,     0,     2,     3,     5,
       4,     1,     1,    10,     1,     1,     1,     1,     1,     1,
       7,     0,     2,     4,     2,     9,     7,     9,     1,     1,
       1,     1,     7,     0,     3,     3,     1,     1,     5,     1,
       1,     1,     7,     0,     3,     1,     1,     1,     1,     1,
       1,     8,    10,     1,     1,    10,     0,     3,     5,     3,
       2,     5,     1,     1,     1,     1,     5,     1,     1,     1,
       8,     1,     1,     5,     1,     1,     8,     1,     5,     1,
       1,     8,     1,     5,     0,     3,     5,     3,     3,     1,
       1,     4,     1,     1,     1,     4,     1,     1,     7,     0,
       3,     3,     3,     1,     1,     5,     1,     1,     9,     1,
       5,     1,     1,     1,     1,     1,     1,     7,     1,     1,
       1,     1,     1,     1,     1,    10,     1,     1,    10,     1,
       1,    10,    10,     7,     0,     2,     9,     7,     9,    10,
       1,     1,     8,     1,     1,     1,     1,     1,    10,     1,
       1,     6,     8,     1,    10,     6,     1,    10,     6,     1,
      10,     6,     1,     9,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     3,
       2,     1,     1,     4,     1,     1,     1,     2,     3,     4,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,    11,     0,     3,     3,     3,     5,
       3,     2,     1,     1,     4,     1,     4,     1,     4,     1,
       4,     1,     9,     0,     3,     3,     3,     2,     1,    19,
       1,     1,     1,     1,     0,     6,     3,     2,     1,     1,
       9,     1,     9,     1,     1,     0,     3,     3,     2,     1,
       7
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
