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
    T_VERSION = 333,
    T_GEODETICCRS = 334,
    T_GEODETICDATUM = 335,
    T_PROJECTEDCRS = 336,
    T_PRIMEMERIDIAN = 337,
    T_GEOGRAPHICCRS = 338,
    T_TRF = 339,
    T_VERTICALCRS = 340,
    T_VERTICALDATUM = 341,
    T_VRF = 342,
    T_TIMEDATUM = 343,
    T_TEMPORALQUANTITY = 344,
    T_ENGINEERINGDATUM = 345,
    T_ENGINEERINGCRS = 346,
    T_PARAMETRICDATUM = 347,
    T_AFFINE = 348,
    T_CARTESIAN = 349,
    T_CYLINDRICAL = 350,
    T_ELLIPSOIDAL = 351,
    T_LINEAR = 352,
    T_PARAMETRIC = 353,
    T_POLAR = 354,
    T_SPHERICAL = 355,
    T_VERTICAL = 356,
    T_TEMPORAL = 357,
    T_TEMPORALCOUNT = 358,
    T_TEMPORALMEASURE = 359,
    T_ORDINAL = 360,
    T_TEMPORALDATETIME = 361,
    T_NORTH = 362,
    T_NORTHNORTHEAST = 363,
    T_NORTHEAST = 364,
    T_EASTNORTHEAST = 365,
    T_EAST = 366,
    T_EASTSOUTHEAST = 367,
    T_SOUTHEAST = 368,
    T_SOUTHSOUTHEAST = 369,
    T_SOUTH = 370,
    T_SOUTHSOUTHWEST = 371,
    T_SOUTHWEST = 372,
    T_WESTSOUTHWEST = 373,
    T_WEST = 374,
    T_WESTNORTHWEST = 375,
    T_NORTHWEST = 376,
    T_NORTHNORTHWEST = 377,
    T_UP = 378,
    T_DOWN = 379,
    T_GEOCENTRICX = 380,
    T_GEOCENTRICY = 381,
    T_GEOCENTRICZ = 382,
    T_COLUMNPOSITIVE = 383,
    T_COLUMNNEGATIVE = 384,
    T_ROWPOSITIVE = 385,
    T_ROWNEGATIVE = 386,
    T_DISPLAYRIGHT = 387,
    T_DISPLAYLEFT = 388,
    T_DISPLAYUP = 389,
    T_DISPLAYDOWN = 390,
    T_FORWARD = 391,
    T_AFT = 392,
    T_PORT = 393,
    T_STARBOARD = 394,
    T_CLOCKWISE = 395,
    T_COUNTERCLOCKWISE = 396,
    T_TOWARDS = 397,
    T_AWAYFROM = 398,
    T_FUTURE = 399,
    T_PAST = 400,
    T_UNSPECIFIED = 401,
    T_STRING = 402,
    T_UNSIGNED_INTEGER_DIFFERENT_ONE_TWO_THREE = 403
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
#define YYFINAL  106
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3219

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  164
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  353
/* YYNRULES -- Number of rules.  */
#define YYNRULES  695
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1429

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   403

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
     160,   162,     2,   154,   163,   155,   149,     2,     2,   151,
     152,   153,     2,     2,     2,     2,     2,     2,   156,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   150,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   157,     2,     2,     2,     2,     2,
     158,   159,     2,   161,     2,     2,     2,     2,     2,     2,
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
     145,   146,   147,   148
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   207,   207,   207,   207,   207,   207,   207,   208,   208,
     208,   209,   212,   212,   213,   213,   213,   214,   216,   216,
     220,   224,   224,   226,   228,   230,   230,   232,   232,   234,
     236,   238,   240,   242,   242,   244,   244,   246,   246,   246,
     246,   248,   248,   252,   254,   258,   259,   260,   262,   262,
     264,   266,   268,   270,   274,   275,   278,   279,   281,   283,
     285,   288,   289,   290,   292,   294,   296,   296,   298,   301,
     302,   304,   304,   309,   309,   311,   311,   313,   315,   317,
     321,   322,   325,   326,   327,   329,   329,   330,   333,   334,
     338,   339,   340,   344,   345,   346,   347,   349,   353,   355,
     358,   360,   363,   364,   365,   366,   367,   368,   369,   370,
     371,   372,   373,   374,   375,   376,   377,   380,   381,   382,
     383,   384,   385,   386,   387,   388,   389,   390,   391,   392,
     393,   394,   398,   400,   402,   406,   411,   413,   415,   417,
     419,   423,   428,   429,   431,   433,   435,   439,   443,   445,
     445,   447,   447,   452,   457,   458,   459,   460,   461,   462,
     463,   465,   467,   469,   469,   471,   471,   473,   475,   477,
     479,   481,   483,   487,   489,   493,   493,   496,   499,   504,
     504,   504,   504,   504,   507,   512,   512,   512,   512,   515,
     519,   520,   522,   538,   542,   543,   545,   545,   547,   547,
     553,   553,   555,   557,   564,   564,   564,   564,   566,   573,
     574,   575,   576,   578,   585,   586,   587,   588,   590,   597,
     604,   605,   606,   608,   610,   610,   610,   610,   610,   610,
     610,   610,   610,   612,   612,   614,   614,   616,   616,   616,
     618,   623,   629,   634,   637,   640,   641,   642,   643,   644,
     645,   646,   647,   648,   651,   652,   653,   654,   655,   656,
     657,   658,   661,   662,   663,   664,   665,   666,   667,   668,
     671,   672,   675,   676,   677,   678,   683,   684,   685,   686,
     687,   688,   689,   690,   691,   694,   695,   696,   697,   700,
     701,   702,   703,   706,   707,   710,   711,   716,   717,   720,
     721,   722,   723,   726,   727,   728,   729,   730,   731,   732,
     733,   734,   735,   736,   737,   738,   739,   740,   741,   742,
     743,   744,   745,   746,   747,   748,   749,   750,   751,   752,
     753,   754,   755,   756,   757,   758,   759,   760,   761,   763,
     766,   768,   770,   772,   774,   776,   792,   792,   794,   802,
     803,   805,   806,   808,   816,   817,   819,   821,   823,   828,
     829,   831,   833,   835,   837,   839,   841,   843,   848,   852,
     854,   857,   860,   861,   862,   864,   865,   867,   872,   873,
     875,   875,   877,   881,   881,   881,   883,   883,   885,   893,
     902,   910,   920,   921,   923,   925,   925,   927,   927,   930,
     931,   935,   941,   942,   943,   945,   945,   947,   949,   951,
     955,   960,   960,   962,   965,   966,   971,   972,   974,   979,
     979,   979,   981,   983,   984,   985,   986,   988,   991,   993,
     997,  1003,  1003,  1007,  1007,  1008,  1008,  1010,  1015,  1016,
    1017,  1018,  1019,  1021,  1027,  1032,  1038,  1040,  1042,  1044,
    1048,  1054,  1055,  1056,  1058,  1060,  1062,  1066,  1066,  1068,
    1070,  1075,  1076,  1077,  1079,  1081,  1083,  1085,  1089,  1089,
    1091,  1097,  1104,  1104,  1107,  1114,  1115,  1116,  1117,  1118,
    1120,  1124,  1126,  1128,  1128,  1132,  1137,  1137,  1137,  1141,
    1146,  1146,  1148,  1152,  1152,  1156,  1161,  1163,  1167,  1167,
    1171,  1176,  1178,  1182,  1183,  1184,  1185,  1186,  1188,  1188,
    1190,  1193,  1195,  1195,  1197,  1199,  1201,  1205,  1211,  1212,
    1213,  1214,  1216,  1218,  1222,  1227,  1229,  1232,  1237,  1241,
    1247,  1247,  1247,  1247,  1247,  1247,  1251,  1256,  1258,  1263,
    1263,  1264,  1266,  1266,  1268,  1275,  1275,  1277,  1284,  1284,
    1286,  1293,  1300,  1305,  1306,  1307,  1309,  1315,  1320,  1328,
    1334,  1336,  1338,  1344,  1346,  1346,  1347,  1347,  1351,  1357,
    1357,  1359,  1364,  1370,  1375,  1381,  1386,  1391,  1397,  1402,
    1407,  1413,  1418,  1423,  1429,  1429,  1430,  1430,  1431,  1431,
    1432,  1432,  1433,  1433,  1434,  1434,  1437,  1437,  1439,  1440,
    1441,  1443,  1445,  1449,  1452,  1452,  1455,  1456,  1457,  1459,
    1463,  1464,  1466,  1468,  1468,  1469,  1469,  1470,  1470,  1470,
    1471,  1472,  1472,  1473,  1473,  1474,  1474,  1476,  1476,  1477,
    1477,  1478,  1479,  1479,  1483,  1487,  1488,  1491,  1496,  1497,
    1498,  1499,  1500,  1501,  1502,  1504,  1506,  1508,  1511,  1513,
    1515,  1517,  1519,  1521,  1523,  1525,  1527,  1529,  1534,  1538,
    1539,  1542,  1547,  1548,  1549,  1550,  1551,  1553,  1558,  1563,
    1564,  1567,  1573,  1573,  1573,  1573,  1575,  1576,  1577,  1578,
    1580,  1582,  1587,  1593,  1595,  1600,  1601,  1604,  1610,  1610,
    1612,  1613,  1614,  1615,  1617,  1619
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
  "\"VERSION\"", "\"GEODETICCRS\"", "\"GEODETICDATUM\"",
  "\"PROJECTEDCRS\"", "\"PRIMEMERIDIAN\"", "\"GEOGRAPHICCRS\"", "\"TRF\"",
  "\"VERTICALCRS\"", "\"VERTICALDATUM\"", "\"VRF\"", "\"TIMEDATUM\"",
  "\"TEMPORALQUANTITY\"", "\"ENGINEERINGDATUM\"", "\"ENGINEERINGCRS\"",
  "\"PARAMETRICDATUM\"", "\"affine\"", "\"Cartesian\"", "\"cylindrical\"",
  "\"ellipsoidal\"", "\"linear\"", "\"parametric\"", "\"polar\"",
  "\"spherical\"", "\"vertical\"", "\"temporal\"", "\"temporalCount\"",
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
  "wkt2015temporal_cs_scope_extent_identifier_remark",
  "opt_separator_cs_unit_scope_extent_identifier_remark",
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
  "geodetic_reference_frame_with_opt_pm",
  "geodetic_reference_frame_without_pm",
  "geodetic_reference_frame_keyword", "datum_name",
  "opt_separator_datum_anchor_identifier_list", "datum_anchor",
  "datum_anchor_keyword", "datum_anchor_description", "projected_crs",
  "projected_crs_keyword", "base_geodetic_crs", "base_static_geodetic_crs",
  "opt_separator_pm_ellipsoidal_cs_unit_opt_separator_identifier_list",
  "base_dynamic_geodetic_crs", "base_static_geographic_crs",
  "base_dynamic_geographic_crs", "base_geodetic_crs_keyword",
  "base_geographic_crs_keyword", "base_crs_name", "ellipsoidal_cs_unit",
  "map_projection", "opt_separator_parameter_list_identifier_list",
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
  "base_static_geod_crs", "opt_separator_pm_opt_separator_identifier_list",
  "base_dynamic_geod_crs", "base_static_geog_crs", "base_dynamic_geog_crs",
  "derived_projected_crs", "derived_projected_crs_keyword",
  "derived_crs_name", "base_projected_crs", "base_projected_crs_keyword",
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
  "coordinate_operation", "coordinate_operation_next",
  "coordinate_operation_end",
  "opt_parameter_or_parameter_file_list_opt_interpolation_crs_opt_operation_accuracy_opt_separator_scope_extent_identifier_remark",
  "operation_keyword", "operation_name", "operation_version",
  "operation_version_keyword", "operation_version_text", "source_crs",
  "source_crs_keyword", "target_crs", "target_crs_keyword",
  "interpolation_crs", "interpolation_crs_keyword", "operation_accuracy",
  "operation_accuracy_keyword", "point_motion_operation",
  "point_motion_operation_next", "point_motion_operation_end",
  "opt_parameter_or_parameter_file_list_opt_operation_accuracy_opt_separator_scope_extent_identifier_remark",
  "point_motion_keyword", "concatenated_operation",
  "concatenated_operation_next", "concatenated_operation_end", "step",
  "opt_concatenated_operation_end", "concatenated_operation_keyword",
  "step_keyword", "bound_crs", "bound_crs_keyword",
  "abridged_coordinate_transformation",
  "abridged_coordinate_transformation_next",
  "abridged_coordinate_transformation_end",
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
     395,   396,   397,   398,   399,   400,   401,   402,   403,    46,
      69,    49,    50,    51,    43,    45,    58,    84,    90,    91,
      40,    93,    41,    44
};
# endif

#define YYPACT_NINF -1232

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-1232)))

#define YYTABLE_NINF -634

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1574, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232,    62, -1232, -1232,
   -1232,    98, -1232, -1232, -1232,    98, -1232, -1232, -1232, -1232,
   -1232, -1232,    98,    98, -1232,    98, -1232,   -87,    98, -1232,
      98, -1232,    98, -1232, -1232, -1232,    98, -1232,    98, -1232,
      98, -1232,    98, -1232,    98, -1232,    98, -1232,    98, -1232,
      98, -1232, -1232, -1232, -1232, -1232, -1232, -1232,    98, -1232,
   -1232, -1232, -1232, -1232, -1232,    98, -1232,    98, -1232,    98,
   -1232,    98, -1232,    98, -1232,    98, -1232, -1232, -1232,   -24,
     -24,   -24,   -24,   -24, -1232,    83,   -24,   -24,   -24,   -24,
     -24,   -24,   -24,   -24,   -24,   -24,   -24,   -24,   -24,   810,
     -24,   -24,   -24,   113, -1232, -1232,   -87, -1232,   -87, -1232,
     -87,   -87, -1232,   -87, -1232, -1232, -1232,    98, -1232,   -87,
     -87, -1232,   -87,   -87,   -87,   -87,   -87,   -87,   -87,   -87,
     -87, -1232,   -87, -1232,   -87, -1232, -1232, -1232, -1232,    26,
   -1232, -1232, -1232, -1232, -1232,    37,    52,   179, -1232, -1232,
   -1232, -1232,   360, -1232,   -87, -1232,   -87,   -87,   -87, -1232,
     -87,    98,  1048,   212,    59,    59,   761,   -24,   256,   138,
     156,   467,   304,   360,   203,   360,   322,   360,   217,   314,
     360,   275,  1439, -1232, -1232, -1232,   483,    69, -1232, -1232,
      69, -1232, -1232,    69, -1232, -1232,   335,   810, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232,   462, -1232, -1232, -1232, -1232,
     252,   265,   287,   761, -1232,   -87, -1232,   -87,    98, -1232,
   -1232, -1232, -1232,    98,   -87,    98,   -87, -1232,    98,    98,
     -87,   -87, -1232, -1232, -1232, -1232,   -87,   -87,   -87,   -87,
   -1232,   -87, -1232,   -87,   -87,   -87, -1232, -1232, -1232, -1232,
      98,    98, -1232, -1232,   -87,    98, -1232, -1232,    98,   -87,
     -87, -1232,   -87, -1232, -1232,    98, -1232,   -87,   -87,    98,
   -1232, -1232,   -87,   -87,    98, -1232, -1232,   -87,   -87,    98,
   -1232, -1232,   -87,   -87,    98, -1232, -1232,   -87,   -87,    98,
     -87,    98, -1232, -1232,   -87,    98, -1232,   -87, -1232, -1232,
   -1232, -1232,    98, -1232,   -87,    98,   -87,   -87,   -87,   -87,
     -87, -1232,   -87,    98,   360, -1232,   416,   462, -1232, -1232,
     558,   360,    75,   360,   360,   -24,   -24,    78,   390,   102,
     -24,   -24,   413,   413,    78,   102,   413,   413,   761,   416,
     360,   466,   -24,   -24,   364,   360,   -24,   -24,    89,   477,
     413,   -24,   488, -1232,   488,   -24,   477,   413,   -24,   477,
     413,   -24,   477,   413,   -24, -1232, -1232,   387,   104, -1232,
     -24,   413,   -24,  1439,   462,   113, -1232,   -24,   335,   113,
   -1232,   499,   113, -1232,   335,   461,   810, -1232,   462, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232,   -87,   -87,    98,
   -1232,    98, -1232, -1232,   -87,   -87,    98,   -87, -1232, -1232,
   -1232,   -87,   -87,   -87, -1232,   -87,    98, -1232, -1232, -1232,
   -1232, -1232, -1232,    98,   360,   -87, -1232,   -87,   -87, -1232,
     -87,    98,   -87,   -87,   360,   -87,   -87, -1232,   -87,   -87,
     761,   360, -1232,   -87,   -87,   -87, -1232,   -87,   -87,    98,
   -1232, -1232,   -87,   -87,   -87,    98,   360,   -87,   -87,   -87,
     -87, -1232,   360,   360,   -87,   -87,   360,   -87,   -87,   360,
     -87,   -87, -1232, -1232,   159, -1232,   360,   -87, -1232,   360,
     -87,   -87,   -87,   287,   360, -1232,   360,   -87, -1232,   -87,
      98,   -87, -1232,   -87,    98,   360, -1232,   481,   486,   -24,
     -24, -1232, -1232,   488, -1232,   823,   479,   488,   360,   212,
     102,   517,   360,   462,  1699, -1232,   477,   112,   112,   477,
     -24,   477,   102, -1232,   477,   477,   329,   360,   306, -1232,
   -1232, -1232,   477,   112,   112, -1232, -1232,   -24,   360,   212,
     477,   940, -1232,   477,   243, -1232, -1232, -1232, -1232,   477,
     108, -1232,   477,   192, -1232,   477,    25, -1232, -1232,   462,
   -1232, -1232,   462, -1232, -1232, -1232,   477,   138,  1409,   360,
     462, -1232, -1232,   499,  1212,   360,   -24,   498,  1047,   360,
     -24, -1232,   -87, -1232, -1232,   360, -1232,   360, -1232,   -87,
   -1232,   360,   -87, -1232,   -87, -1232,   -87,   360, -1232, -1232,
   -1232,    98, -1232,   287,   360, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232,   -87, -1232, -1232, -1232, -1232,   -87,
     -87,   -87, -1232,   -87,   -87,   -87,   -87,   360, -1232,   -87,
     360,   360,   360,   360, -1232, -1232,   -87,   -87,    98, -1232,
   -1232, -1232,   -87,    98,   360,   -87,   -87,   -87,   -87, -1232,
     -87, -1232,   -87,   360,   -87,   360,   -87,   -87,   360,   -87,
     360,   -87,   360,   -87,   205,   427, -1232,   497,   360, -1232,
   -1232, -1232, -1232,   -87, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232,   -87,    98,   -87,    98, -1232,
     -87,    98,   -87,    98,   -87,    98,   -87,    98,   -87, -1232,
      98,   -87, -1232, -1232,   -87, -1232, -1232, -1232,    98,   -87,
     -87,    98,   -87,    98, -1232, -1232,   -87, -1232,    98, -1232,
   -1232,   -87,   486, -1232, -1232, -1232, -1232, -1232, -1232,   117,
   -1232,   -24,   462, -1232,   398,   398,   398,   398,    78,   100,
     360,    78,   360, -1232,   499, -1232, -1232, -1232, -1232, -1232,
   -1232,   -24, -1232,   -24, -1232,    78,    58,   360,    78,   360,
     416,   518, -1232,   398, -1232,    89,   360, -1232,   360, -1232,
     360, -1232,   360, -1232,   462, -1232, -1232,   462,   462, -1232,
     430, -1232, -1232, -1232, -1232,   466,   176,   566,   530, -1232,
     -24,   547, -1232,   -24,   425, -1232,   823,   281, -1232,   823,
     333, -1232,   387, -1232,   469, -1232,  1134,   360,   -24, -1232,
   -1232,   -24, -1232,   823,   488,   360,   267,    47, -1232, -1232,
   -1232,   -87, -1232,   -87, -1232, -1232, -1232, -1232,   -87,   -87,
     -87,   -87,   -87,   -87,   -87, -1232,   -87, -1232,   -87, -1232,
     -87,   -87,   -87,   -87, -1232,   -87,   -87, -1232,   -87, -1232,
   -1232,   -87,   -87,   -87,   -87, -1232, -1232, -1232, -1232, -1232,
     465,   430, -1232,   497,   462, -1232,   -87, -1232,   -87, -1232,
     -87, -1232,   -87, -1232, -1232,   360,   -87,   -87,   -87, -1232,
     360,   -87,   -87, -1232,   -87,   -87, -1232,   -87, -1232, -1232,
     -87, -1232,   360, -1232, -1232,   -87,   -87,   -87,    98,   -87,
   -1232,   -87,   -87,   360, -1232, -1232, -1232, -1232, -1232, -1232,
     360,   -87,   -87,   360,   360,   360,   360,   360,   360, -1232,
   -1232,   360,   103,   360,   761,   761,   360, -1232,   306, -1232,
   -1232,   360,   746,   360,   360,   360, -1232, -1232,   462, -1232,
   -1232, -1232,   360, -1232,   338, -1232, -1232,   425, -1232,   281,
   -1232, -1232, -1232,   281, -1232, -1232,   823, -1232,   823,   387,
   -1232, -1232, -1232,  1054, -1232,   810, -1232,   416,   -24, -1232,
     -87,   168,   499, -1232, -1232,   -87,   -87,   -87,   -87, -1232,
   -1232,   -87,   -87,   -87, -1232, -1232,   -87,   -87, -1232,   -87,
   -1232, -1232, -1232, -1232, -1232,   -87, -1232,    98,   -87, -1232,
     -87, -1232, -1232, -1232,   681, -1232,   360,   -87,   -87,   -87,
   -1232,   -87,   -87,   -87,   -87, -1232,   -87, -1232,   -87, -1232,
   -1232,   360,   -87,   360,   -87, -1232,   -87,   498,    98, -1232,
     -87, -1232,   623,   623,   623,   623, -1232, -1232, -1232,   360,
     360, -1232, -1232,   -24, -1232,   623,   811, -1232, -1232,   199,
     535,   579,   281, -1232, -1232, -1232, -1232,   823,   372,   360,
   -1232, -1232, -1232,   297,   360,    98,   -24,  1415,   360, -1232,
     -87,    98,   -87,    98,   -87,   -87,    98, -1232, -1232,   -87,
     -87,   370,   811, -1232,   -87,   -87, -1232,   -87, -1232, -1232,
     -87, -1232,   -87, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232,   -87, -1232,    98, -1232,   267,   -87, -1232,   -87,   -87,
   -1232,  1348, -1232,   -24,  1001, -1232,   -24, -1232,   711, -1232,
     -24,   761,  1104, -1232, -1232,   535,   579,   579, -1232,   823,
     360,   -24,   360,   416, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232,    98, -1232,    98,
     -87, -1232,   -87, -1232,   -87,   -87,   -87, -1232,   -87,   -87,
     -87, -1232, -1232,   -87,   -87,    98,   -87, -1232, -1232, -1232,
   -1232,   360, -1232,   -87,   -87,   -87,   -24,   -24,  1047,  2503,
   -1232, -1232,  2115, -1232,  2716,   360,  1527, -1232, -1232,   -24,
     579, -1232,   761,   360,  1340,   360,   360,   -87,   -87, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232,   -87,   -87,   -87,   -87,
     -87,   360, -1232,   -87,   -87,   -87,   -87,   -87,   360, -1232,
     -87, -1232,   -87, -1232,   -87, -1232,   -87, -1232, -1232,   -87,
      98, -1232, -1232,   761,   360,   272,   360,   679,   679,   710,
     710, -1232,   870,   412,   679,   494,   494, -1232,   336, -1232,
     360, -1232, -1232,   267,   -87, -1232, -1232,   -87,   -87, -1232,
     -87,    98, -1232, -1232,   -87,   -87, -1232,   -87,    98,   -87,
   -1232, -1232,   -87,   -87, -1232,   -87,    98,   -87, -1232,   -87,
     -87, -1232,   -87,   -87, -1232,   -87,   -87, -1232,   -87, -1232,
     -87,   -87, -1232,   -87, -1232,   -87, -1232,   360,   360, -1232,
   -1232,    74, -1232,   462, -1232, -1232,   870, -1232,   823,   475,
   -1232, -1232, -1232,   870, -1232,   823,   475, -1232, -1232, -1232,
     475, -1232, -1232,   336, -1232, -1232, -1232,   336, -1232, -1232,
   -1232, -1232,   -87, -1232,   -87,   360,   -87,   -87,   -87,   -87,
     -87,   -87,   360,   -87,   -87,   -87,   -87, -1232, -1232, -1232,
   -1232,   475, -1232,   438, -1232, -1232,   475, -1232, -1232, -1232,
   -1232, -1232, -1232,   -87,   360,   -87, -1232, -1232, -1232
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   419,   406,   395,   405,   161,   431,   454,   397,   483,
     486,   601,   645,   680,   683,   508,   501,   356,   560,   493,
     490,   498,   496,   612,   667,   396,   421,   432,   398,   420,
     484,   488,   487,   509,   494,   491,   499,     0,     4,     5,
       2,     0,    13,   346,   347,     0,   584,   385,   383,   384,
     386,   387,     0,     0,     3,     0,    12,   416,     0,   586,
       0,    11,     0,   588,   468,   469,     0,    14,     0,   590,
       0,    15,     0,   592,     0,    16,     0,   594,     0,    17,
       0,   585,   541,   539,   540,   542,   543,   587,     0,   589,
     591,   593,   595,    19,    18,     0,     7,     0,     8,     0,
       9,     0,    10,     0,     6,     0,     1,    73,    74,     0,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    78,   162,     0,   357,     0,   394,
       0,     0,   407,     0,   411,   412,   417,     0,   422,     0,
       0,   455,     0,     0,   423,     0,   423,     0,   423,     0,
     503,   561,     0,   602,     0,   613,   627,   614,   628,   615,
     616,   630,   617,   618,   619,   620,   621,   622,   623,   624,
     625,   626,     0,   610,     0,   646,     0,     0,     0,   651,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    75,    76,   609,     0,     0,   634,   636,
       0,   658,   660,     0,   668,   670,     0,     0,    40,    20,
      37,    38,    39,    41,    42,     0,   163,    21,    22,    26,
       0,    25,    35,     0,   164,   154,   361,     0,     0,   446,
     447,   369,   400,     0,     0,     0,     0,   399,     0,     0,
       0,     0,   545,   548,   546,   549,     0,     0,     0,     0,
     408,     0,   413,     0,   423,     0,   433,   434,   435,   436,
       0,     0,   458,   457,   451,     0,   573,   473,     0,     0,
       0,   472,     0,   569,   570,     0,   428,   190,   424,     0,
     485,   576,     0,     0,     0,   492,   579,     0,     0,     0,
     497,   582,     0,     0,     0,   515,   511,   190,   190,     0,
     190,     0,   502,   563,     0,     0,   596,     0,   597,   604,
     605,   611,     0,   648,     0,     0,     0,     0,     0,     0,
       0,   653,     0,     0,     0,    34,    27,     0,    33,    23,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   425,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   507,   506,     0,     0,   504,
       0,     0,     0,     0,     0,     0,   635,     0,     0,     0,
     659,     0,     0,   669,     0,     0,     0,   650,     0,    29,
      31,    28,    36,   168,   171,   165,   166,   155,   158,     0,
     160,     0,   153,   365,     0,   351,     0,     0,   348,   353,
     362,   359,     0,     0,   371,   375,     0,   223,   393,   204,
     205,   206,   207,     0,     0,     0,   448,     0,     0,   522,
       0,     0,     0,     0,     0,     0,     0,   409,   402,   190,
       0,     0,   418,     0,     0,     0,   464,   190,   451,     0,
     450,   459,   190,     0,     0,     0,     0,     0,     0,   190,
     190,   429,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    50,   512,    48,   513,     0,   190,   516,     0,
       0,     0,   598,   606,     0,   649,     0,     0,   525,   662,
       0,     0,   694,    80,     0,     0,    32,     0,     0,     0,
       0,   350,   355,     0,   354,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   388,     0,     0,     0,     0,
       0,     0,     0,   390,     0,     0,     0,     0,     0,   415,
      24,   410,     0,     0,     0,   452,   453,     0,     0,     0,
       0,     0,   470,     0,     0,   191,   426,   427,   489,     0,
       0,   495,     0,     0,   500,     0,     0,    44,    58,     0,
      45,    49,     0,   510,   505,   514,     0,     0,     0,     0,
     607,   603,   647,     0,     0,     0,     0,     0,     0,     0,
       0,   652,   156,   159,   169,     0,   172,     0,   367,   351,
     366,     0,   351,   363,   359,   358,     0,     0,   380,   381,
     376,     0,   368,   372,     0,   224,   225,   226,   227,   228,
     229,   230,   231,   232,     0,   233,   234,   235,   236,     0,
       0,     0,   392,     0,   553,     0,   553,     0,   523,     0,
       0,     0,     0,     0,   199,   198,   190,   190,     0,   401,
     197,   196,   190,     0,     0,     0,   438,     0,   438,   465,
       0,   456,     0,     0,     0,     0,     0,   190,     0,   190,
       0,   190,     0,   190,    48,     0,    59,     0,     0,   564,
     565,   566,   567,     0,   174,   100,   133,   136,   144,   148,
      98,   600,    82,    88,    89,    93,     0,    85,     0,    92,
      85,     0,    85,     0,    85,     0,    85,     0,    85,    84,
       0,   598,   583,   608,   638,   537,   657,   666,     0,   662,
     662,     0,    80,     0,   661,   526,   378,   681,     0,    81,
     682,     0,     0,   167,   170,   352,   364,   349,   360,     0,
     389,     0,   373,   370,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   544,     0,   547,   391,   550,   551,   404,
     403,     0,   414,     0,   430,     0,     0,     0,     0,     0,
      27,     0,   471,     0,   568,     0,     0,   574,     0,   577,
       0,   580,     0,    46,     0,    43,    68,     0,     0,    53,
      71,    55,    66,    67,   559,     0,     0,     0,     0,    91,
       0,     0,   117,     0,     0,   118,     0,     0,   119,     0,
       0,   120,     0,    83,     0,   599,     0,     0,     0,   663,
     664,     0,   665,     0,     0,     0,     0,     0,   684,   686,
     157,     0,   382,   378,   374,   237,   238,   239,   190,   190,
     190,   190,   553,   190,   190,   552,   553,   557,   518,   202,
       0,     0,   438,   190,   449,   190,   190,   437,   438,   444,
     466,   461,     0,   190,   190,   571,   575,   578,   581,    52,
      48,    71,    60,     0,     0,    70,   190,    96,    85,    94,
       0,    90,    85,    87,   101,     0,    85,    85,    85,   134,
       0,    85,    85,   137,     0,    85,   145,     0,   149,   150,
       0,    79,     0,   655,   644,   638,   638,    80,     0,    80,
     637,     0,     0,     0,   379,   524,   674,   675,   672,   673,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   555,
     554,     0,     0,     0,     0,     0,     0,   442,     0,   439,
     441,     0,     0,     0,     0,     0,    47,    69,     0,    54,
      57,    72,     0,    95,     0,    86,    99,     0,   121,     0,
     122,   123,   132,     0,   124,   125,     0,   126,     0,     0,
     173,   639,   640,     0,   641,     0,   643,    27,     0,   656,
       0,     0,     0,   685,   377,     0,     0,     0,     0,   556,
     558,   190,   518,   518,   517,   203,   190,   190,   443,   190,
     445,   188,   186,   185,   187,   190,   467,     0,   190,   460,
       0,   572,    64,    56,     0,   562,     0,   102,   103,   104,
     105,    85,    85,    85,    85,   138,     0,   146,   142,   151,
     152,     0,    80,     0,     0,   538,   378,     0,     0,   689,
     690,   688,     0,     0,     0,     0,   521,   519,   520,     0,
       0,   440,   462,     0,   463,     0,     0,    63,    97,     0,
       0,     0,     0,   127,   128,   129,   130,     0,     0,     0,
     147,   642,   654,     0,     0,     0,     0,     0,     0,   243,
     214,     0,   209,     0,    80,   220,     0,   192,   189,     0,
     475,    65,     0,    61,   106,   107,   108,   109,   110,   111,
      85,   139,     0,   143,   141,   535,   530,   531,   532,   533,
     534,   378,   528,     0,   536,     0,     0,   693,   690,   690,
     687,     0,   213,     0,     0,   208,     0,   218,     0,   219,
       0,     0,     0,   474,    62,     0,     0,     0,   131,     0,
       0,     0,     0,    27,   692,   691,   183,   180,   179,   182,
     200,   181,   201,   217,   345,   175,   177,     0,   176,     0,
     215,   244,     0,   212,   209,    80,     0,   222,   220,     0,
     190,   481,   479,    80,    80,     0,   112,   113,   114,   115,
     140,     0,   527,   194,   676,   190,     0,     0,     0,     0,
     211,   210,     0,   221,     0,     0,     0,   476,   478,     0,
       0,   135,     0,     0,     0,     0,     0,     0,   194,   216,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,   297,   246,   248,   250,
     252,     0,   245,   270,   277,   279,   281,   283,     0,   276,
     293,   184,    80,   482,   378,   116,   190,   529,   679,    80,
       0,   671,   695,     0,     0,     0,     0,     0,     0,     0,
       0,   240,     0,     0,     0,     0,     0,   242,     0,   477,
       0,   195,   678,     0,   190,   193,   344,   190,   190,   298,
     190,     0,   241,   340,   190,   190,   247,   190,     0,   190,
     249,   342,   190,   190,   251,   190,     0,   190,   253,   190,
     190,   271,   190,   190,   278,   190,   190,   280,   190,   282,
     190,   190,   284,   190,   294,   190,   480,     0,     0,   299,
     302,     0,   300,     0,   254,   261,     0,   258,     0,     0,
     260,   262,   269,     0,   266,     0,     0,   268,   272,   275,
       0,   273,   285,     0,   287,   288,   289,     0,   291,   292,
     295,   296,   676,   178,   190,     0,   190,   190,     0,   190,
     190,   190,     0,   190,   190,   190,   190,   677,   301,   343,
     257,     0,   255,     0,   259,   265,     0,   263,   341,   267,
     274,   286,   290,   190,     0,   190,   256,   339,   264
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1232, -1232, -1232,  -221,  -226,  -189, -1232,   266,  -194,   293,
   -1232, -1232, -1232, -1232, -1232, -1232,  -196,  -341,  -654,   -53,
    -782,  -642, -1232, -1232, -1232, -1232, -1232, -1232, -1232,  -563,
    -250, -1232, -1232, -1232,  -847, -1232, -1232,  -237,  1251,  1249,
     -57,  1948, -1232,  -665,  -567,  -574, -1232, -1232,  -157, -1232,
   -1232,  -156, -1232, -1232, -1232,  -154,  -306, -1232, -1232,  -791,
   -1232, -1232, -1232, -1232, -1232,  -783, -1232, -1232, -1232, -1232,
    -657, -1232, -1232, -1232,   110, -1232, -1232, -1232, -1232, -1232,
     143, -1232, -1232,  -505, -1232, -1232,  -473, -1232, -1232, -1231,
   -1232, -1232, -1232, -1232,  -557,  1794,  -410, -1201,  -544, -1232,
   -1232, -1232,  -748,  -930,   -37, -1232,  -497, -1232, -1232, -1232,
   -1232,  -500,  -334,   124, -1232, -1232,  -290, -1018,  -364,  -446,
   -1004,  -578, -1232,  -852,  -597, -1232, -1232, -1232, -1232,  -602,
   -1232, -1232, -1232, -1232,  -729,  -588, -1232,  -701, -1232,  -556,
   -1232,  -724, -1232,   716,  -416,  -148,   519,  -419,    36,  -102,
    -326,   101, -1232, -1232, -1232,   188, -1232,  -117, -1232,   -67,
   -1232, -1232, -1232, -1232, -1232, -1232,  -836, -1232, -1232, -1232,
   -1232,   600,   604,   605,   607,  -280,   663, -1232, -1232,   -91,
      42, -1232, -1232, -1232, -1232, -1232,  -107, -1232, -1232, -1232,
   -1232,    10, -1232,   506,  -105, -1232, -1232, -1232,   609, -1232,
   -1232, -1232,  -644, -1232, -1232, -1232,   546,   552,   527,  -204,
       4,   277, -1232, -1232, -1232, -1232, -1232, -1232, -1232,  -367,
    -794,  -938, -1232, -1232,   627,   633, -1232,   197, -1232,  -448,
   -1232, -1232, -1232,  -192, -1232,   640, -1232,  -185, -1232,   641,
   -1232,  -186, -1232,   644, -1232,  -187, -1232, -1232,   378, -1232,
   -1232, -1232, -1232, -1232,   540,  -406, -1232, -1232,  -384, -1232,
   -1232,  -781, -1232, -1232, -1232,  -796, -1232, -1232,   648, -1232,
   -1232,   588, -1232,   590, -1232, -1232,   190,  -624,   193,   194,
     195,   660, -1232, -1232, -1232, -1232, -1232,   671, -1232, -1232,
   -1232, -1232,   672, -1232, -1232,   673, -1232, -1232,   674, -1232,
   -1232,   675,  -179,  -349,    76, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232,   806, -1232,   478,  -253, -1232,
    -119,  -209, -1232, -1232,  -109, -1232,    81, -1232, -1232, -1232,
    -814, -1232, -1232, -1232,   489,   -64,   816, -1232, -1232,   484,
   -1087,  -552, -1232, -1001,   842, -1232, -1232, -1232,   -96, -1232,
    -458, -1232,  -249
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   235,   620,   237,   880,   238,   470,
     239,   240,   419,   420,   241,   348,   242,   243,   894,   589,
     503,   590,   504,   695,   890,   591,   809,   969,   592,   810,
     893,  1033,  1034,  1112,   811,   812,   813,   895,   109,   215,
     382,   456,   922,   609,   749,   819,   712,   713,   714,   715,
     716,   717,   718,   905,  1036,   719,   720,   721,   910,   722,
     723,   914,  1046,  1122,  1201,   724,  1089,   725,   917,  1048,
     726,   727,   920,  1051,   489,   351,    41,   136,   245,   427,
     428,   429,   615,   430,   431,   617,   729,   730,  1174,  1175,
    1176,  1177,  1026,  1027,   874,   383,   667,  1178,  1223,   673,
     668,  1179,   870,  1016,   448,   449,  1145,   450,  1142,   451,
     452,  1149,   453,   649,   650,   651,   858,  1102,  1100,  1105,
    1103,  1182,  1271,  1326,  1334,  1272,  1341,  1278,  1344,  1349,
    1279,  1354,  1296,  1319,  1266,  1327,  1328,  1335,  1336,  1329,
    1321,  1180,    42,   252,   353,   534,    44,   354,   253,   138,
     247,   538,   248,   441,   624,   435,   436,   621,   619,   254,
     255,   445,   446,   634,   542,   630,   845,   631,   853,    46,
      47,    48,    49,    50,    51,   454,   140,    52,    53,   256,
     437,   557,    55,   143,   271,   468,   455,   147,   273,   471,
      56,   257,    58,   149,   203,   298,   299,   492,    59,    60,
     275,   276,   787,   277,   278,   279,   258,   259,   457,   876,
     936,   375,    62,   152,   284,   285,   482,   478,   963,   738,
     680,   881,  1028,    63,    64,    65,   290,   486,  1153,  1194,
    1195,  1284,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,   210,    80,   318,   319,
     506,   320,   321,   509,   937,   953,   461,   659,   941,   520,
     746,   739,  1131,  1132,  1133,   740,   741,  1056,    81,    82,
      83,   260,    84,   261,    85,    86,   262,   770,   263,   264,
     265,    87,    88,   162,   324,   325,   703,    89,   292,   293,
     294,   295,    90,   303,   304,    91,   308,   309,    92,   313,
     314,    93,    94,   327,   599,    95,   164,   331,   332,   514,
      96,   182,    97,   183,   184,   938,   218,   219,   837,    99,
     186,   334,   335,   516,   336,   191,   342,   343,   927,   928,
     742,   743,   100,   221,   222,   605,   939,   102,   224,   225,
     940,  1225,   103,   748,   328,   105,   523,   848,   849,  1060,
    1098,   524,  1061
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     115,   672,   270,   236,    61,   421,   344,   479,   146,   291,
      57,   337,   187,   188,   339,   532,   347,   943,   531,   302,
     307,   312,   929,   613,   190,  1017,   434,   519,   421,   697,
     906,   711,   772,   326,   789,   871,    45,   737,   907,   345,
     926,   911,    54,   808,   931,   485,   970,   932,  1162,   349,
     918,   205,   803,   207,   512,   925,  1095,  1110,  1101,  1054,
    1101,  1106,   106,     1,   144,   670,  1325,  1325,  1333,  1333,
     671,  1340,   518,  1325,    15,     5,   114,   842,   249,   192,
       2,   193,     1,   194,   195,   464,   196,   250,  1170,   144,
       4,     5,   198,   199,  1318,   200,   201,   202,   204,   202,
     206,   202,   208,   209,   267,   211,   144,   212,   144,   189,
      40,   338,   476,    33,   340,   251,     1,     5,   447,    17,
       5,     5,     2,   134,    10,   333,  1184,   216,   266,   217,
     220,   223,     4,   226,   289,  1396,   246,   433,  1399,    26,
     145,   735,  1400,    29,  1106,  1403,   822,   333,   825,  1404,
     828,   422,   831,   189,   833,   316,  1013,   249,    26,   282,
    1394,   425,    29,  1172,   908,   145,   250,   912,   251,   370,
     915,  1012,    17,    19,   467,    31,    32,   476,   246,  1052,
    1423,   283,   145,  1038,   145,  1425,   484,  1077,   350,  -629,
     352,  1039,    26,     5,  1041,   525,    29,   357,    34,   359,
    -631,   502,   704,   362,   363,  1059,   735,  1049,   513,   364,
     365,   366,   367,   626,   368,  -632,   369,   202,   371,   734,
    1094,  1190,   526,  1290,   326,  1205,  1357,   374,   956,  1113,
     707,   710,   378,   379,   961,   380,   485,   288,   948,   968,
     274,   384,   951,   682,   891,   386,   387,   850,   966,   731,
     389,   390,   708,   709,   442,   392,   393,   107,   108,    21,
     627,     2,   994,   398,   996,  1154,    15,   401,    19,   924,
     403,     4,   661,   246,   301,   483,   560,   405,    10,   408,
     409,   411,   412,   414,    36,   415,  1170,   600,  1114,     5,
     311,     7,  1286,    34,  1316,  1160,  1115,  1117,     5,   251,
     338,  1138,  1136,   340,  1125,    33,    12,   704,  1126,  1127,
    1128,  1129,   297,   670,   587,   459,   588,  1040,   671,   317,
    1042,     5,  1043,     5,   973,  1109,  1044,   296,   975,    31,
      32,     5,   978,   980,   981,   709,   664,   984,   985,   323,
     665,   987,  -633,   899,    24,   903,     5,   633,   903,   493,
       5,   903,   496,     5,   903,   499,   807,   903,  1316,   704,
     -51,  1172,   588,  1314,   315,   316,  1130,   443,   706,   707,
     527,   528,  1196,   476,   463,   473,   341,   352,   533,   664,
     536,     5,   687,   665,   537,   539,   540,  1091,   541,    21,
     868,   708,   709,   694,   306,   689,   696,   691,   546,   693,
     547,   548,   346,   549,   733,   551,   552,   762,   554,   555,
    1289,   556,   558,  1203,    36,   -30,   562,   563,   564,   326,
    1185,   374,  1116,  1118,  1119,  1120,   569,   570,  1193,     5,
     573,   574,  1323,   851,  1316,   531,   229,   579,   580,  1147,
     582,   583,     5,   585,   586,   670,  1330,   444,  1310,   421,
     671,   704,  1347,   596,   597,   598,   654,   656,  1227,  1228,
     603,   459,   604,   532,   607,   882,   608,  1083,  1084,  1085,
    1086,   755,   676,   678,   757,   859,   860,   861,   708,   709,
     653,   655,  1166,  1273,   477,  1280,  1167,  1168,  1169,   517,
       7,   751,     5,   447,   490,   521,   675,   677,  1197,  1198,
    1199,   696,    10,   883,   903,     5,   903,   686,   522,   652,
     903,     5,   657,   423,   660,  1331,  1316,   662,   663,   424,
    1211,   213,   214,   251,   518,   674,   -59,    17,  1217,  1218,
    1137,   -59,   -59,   -59,   134,   228,   685,   286,   230,   231,
     232,   433,   688,   747,  1171,   690,  1158,     5,   692,   855,
     856,   857,   807,    31,    32,   752,   704,   329,   330,   698,
     706,   707,   533,  1285,     5,   533,   854,   537,  1186,   759,
     233,   234,  1189,   704,  1173,   628,   629,  1183,   707,   246,
     433,  1187,   587,   708,   709,  1192,   805,   764,   708,   709,
     423,   424,   765,   766,   767,   705,   768,   769,   771,   769,
     708,   709,   774,   884,  1350,  1350,  1067,  1068,   889,   903,
     228,   696,   892,   230,   231,   232,   921,  1309,   785,   786,
     788,   786,   588,   790,  1312,   791,   154,   793,   156,   795,
     158,  1099,   160,   709,  1058,   469,   502,   913,   942,   418,
     916,  1229,   804,   618,   971,   228,   815,   623,   230,   231,
     232,   233,   234,   805,   967,   806,   421,  1288,   816,   897,
     818,   900,   864,   821,   901,   824,   666,   827,  1037,   830,
     612,   830,   991,   992,   598,   839,   840,   836,  1123,   875,
    1164,  1165,   604,   604,  1294,   608,  1166,  1210,  1213,   844,
    1167,  1168,  1169,   807,   847,   684,     5,   696,   892,  1323,
    1104,  1316,  1188,  1338,  1352,   134,   228,   229,   728,   230,
     231,   232,   233,   234,   728,  1345,    43,  1166,   728,  1099,
     287,  1167,  1168,  1169,   622,   758,   933,     5,     5,   165,
    1058,  1331,  1316,   166,   167,   807,   168,   704,   169,  1320,
     705,   706,   707,  1337,  1337,   280,  1342,  1346,  1171,  1351,
    1351,   281,  1355,  1021,  1019,   566,   170,  1022,  1023,  1024,
    1015,  1015,   171,     5,   708,   709,   710,   683,  1282,   172,
     173,   807,  1032,   174,  1053,   141,   507,   175,   862,  1171,
     150,   866,   153,   268,   155,   269,   157,   699,   159,   176,
     700,   701,   702,   502,   759,   872,   844,  1045,   878,  1047,
     177,   178,   179,   180,   181,   769,    98,   835,  1076,   769,
    1397,   952,   406,   954,   955,   786,   101,  1401,   958,   896,
       3,   786,   421,   413,   962,   791,   410,  1405,     6,   228,
     229,  1406,   230,   231,   232,   233,   234,     8,   696,   806,
    1407,   830,   104,   974,     9,   830,  1003,    11,  1139,   977,
     979,   830,     0,     0,   983,   830,  1424,   986,   830,     0,
     988,     0,    16,   989,     0,     0,     0,     0,   836,   836,
     993,     0,   608,    18,   997,   998,    20,  1166,    22,   863,
    1111,  1167,  1168,  1169,  1001,  1002,     0,     5,   458,    25,
       0,    27,  1316,    28,     0,    30,   873,     0,  1121,   474,
     475,    35,   460,   462,     0,     0,   465,   466,   488,   228,
     229,     0,   230,   231,   232,   495,   696,     0,   498,     0,
     487,   501,     0,     0,     0,     0,   898,   494,   902,   511,
     497,   902,     0,   500,   902,     0,     0,   902,     0,  1171,
     902,   510,     0,  1057,     0,     0,   728,     0,  1062,  1063,
    1064,  1065,     0,     0,   934,   952,   952,  1015,     0,   228,
       0,     0,   230,   231,   232,   233,   234,     0,     0,   806,
    1200,   228,   229,  1075,   230,   231,   232,   233,   234,     0,
    1079,  1080,  1081,     0,  1082,   830,   830,   830,     0,  1087,
       0,  1088,     0,     0,     0,   608,     0,  1093,     0,   844,
       0,     0,     0,  1097,     0,     0,     0,     0,  1166,  1099,
       0,     0,  1167,  1168,  1169,  1170,     0,     0,     5,     0,
       0,     0,     0,     0,     0,     0,     0,   704,  1015,     0,
     705,   706,   707,   635,   636,   637,   638,   639,   640,   641,
     642,   643,     0,  1141,     0,  1144,     0,   608,  1148,     0,
       0,     0,  1151,  1152,   708,   709,   710,  1155,  1156,     0,
    1157,     0,  1011,   830,     5,  1159,     0,     0,     0,     0,
    1171,     5,  1025,   704,   844,     0,   705,   706,   707,  1163,
     704,  1097,  1097,   705,   706,   707,     0,   902,     0,   902,
    1172,     0,     0,   902,     0,     0,     0,   736,     0,  1015,
     708,   709,   710,   728,     0,     0,     0,   708,   709,   710,
       0,  1166,     0,     0,     0,  1167,  1168,  1169,  1170,     0,
       0,     5,     0,  1208,     0,  1209,     0,  1144,   608,  1212,
     704,  1148,  1214,   705,   706,   707,  1216,   608,     0,  1220,
    1191,     0,     0,   476,     0,     0,  1222,  1224,     0,     0,
       0,     5,     0,     0,     0,     0,     0,   708,   709,   710,
     704,     0,     0,   705,   706,   707,     0,  1395,     0,     0,
    1293,  1222,   735,  1171,     0,     0,   923,   736,     0,  1398,
       0,     0,     0,     0,     0,     0,  1402,   708,   709,   710,
       0,     0,   902,  1172,     0,   134,   228,   229,     0,   230,
     231,   232,   233,   234,     0,     0,     0,   728,     0,  1295,
    1297,  1298,  1299,  1300,     0,     0,  1302,  1303,  1304,  1305,
    1306,   476,     0,  1308,     0,   608,     0,   844,     0,     5,
       0,     0,   608,     0,     0,     0,     0,     0,   704,     0,
       0,   705,   706,   707,     0,     0,     0,     0,     0,     0,
     735,   728,     0,     0,   728,   736,     0,     0,   728,     0,
       0,     0,   728,  1361,     0,   708,   709,   710,     0,     0,
    1366,     0,  1369,     0,     0,     0,     0,     0,  1373,     0,
    1376,     0,     0,     0,     0,  1380,     0,     0,  1383,     0,
       0,     0,     0,  1387,     0,     0,   110,     0,     0,     0,
       0,     0,     0,   111,   112,     0,   113,     0,     0,   116,
       0,   117,     0,   118,     0,     0,     0,   119,   728,   120,
       0,   121,     0,   122,     0,   123,   728,   124,     0,   125,
       0,   126,     0,     0,   728,  1224,     0,     0,     0,   127,
    1411,  1413,     0,     0,  1416,     0,   128,     0,   129,     0,
     130,     0,   131,     0,   132,  1166,   133,     5,     0,  1167,
    1168,  1169,  1170,     0,     0,     5,   704,     0,     0,   705,
     706,   707,     0,     0,   704,     0,     0,   705,   706,   707,
       0,     0,     0,   736,     0,   747,     0,     0,     0,     0,
       0,     0,     0,   708,   709,   710,     0,     0,   197,     0,
       0,   708,   709,   710,     0,  1317,     0,  1324,  1324,  1332,
    1332,     0,  1339,  1343,  1324,  1348,  1348,  1171,  1353,     3,
       0,     0,     0,     0,   476,     0,     5,     6,     0,     0,
       0,     0,     5,     0,     0,   704,     8,  1172,   705,   706,
     707,   704,   227,     9,   705,   706,   707,     0,     0,     3,
       0,     0,   300,   735,   305,    14,   310,     6,     0,   322,
       0,    16,   708,   709,   710,     0,     8,     0,   708,   709,
     710,     0,    18,     9,     0,    20,     0,    22,     0,     0,
       0,     0,     0,     0,     0,    14,     0,     0,    25,     0,
      27,    16,    28,     0,    30,     0,     0,     0,     0,   355,
      35,     0,    18,     0,   356,    20,   358,    22,     0,   360,
     361,     0,     0,     0,     0,     0,     0,     0,    25,     0,
      27,     0,    28,     0,    30,     0,     0,     0,     0,     0,
      35,   372,   373,     0,     0,     0,   376,     0,     0,   377,
       0,     0,     0,     0,     5,     0,   381,     0,     0,     0,
     385,     0,     0,   704,     0,   388,   705,   706,   707,     0,
     391,     0,     0,  1191,     0,   394,     0,     0,     0,     0,
     397,     0,   400,     0,     0,     0,   402,     0,     1,     2,
     708,   709,   710,   404,     3,     0,   407,     0,     0,     4,
       0,     5,     6,   417,   416,     0,     0,     0,     7,     0,
     432,     8,   438,   439,     0,     0,     0,     0,     9,    10,
       0,    11,     0,    12,     0,     0,     0,     0,    13,   472,
      14,     0,     0,    15,   480,     0,    16,     0,     0,     0,
       0,     0,     0,     0,    17,     0,     0,    18,     0,    19,
      20,    21,    22,     0,     0,     0,     0,     0,     0,     0,
      23,    24,     0,    25,    26,    27,     0,    28,    29,    30,
      31,    32,    33,     0,    34,    35,    36,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     529,     0,   530,     0,     0,     0,     0,   535,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   543,     0,     0,
       0,     0,     0,   545,   544,     0,     0,     0,     0,     0,
       0,     0,   550,   553,     0,     0,     0,     0,     0,     0,
     561,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     567,     0,     0,     0,     0,   572,   571,     0,     0,     0,
       0,   577,   578,     0,     0,   581,     0,     0,   584,     0,
       0,     0,     0,     0,     0,   593,     0,     0,   595,     0,
       0,     0,     0,   601,     0,   602,     0,     0,     0,     0,
       0,   606,     0,     0,   611,   610,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   625,     0,     0,
       0,   632,   635,   636,   637,   638,   639,   640,   641,   642,
     643,   644,   645,   646,   647,   648,   669,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   681,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   732,     0,
       0,     0,     0,     0,   744,     0,     0,     0,   750,     0,
       0,     0,     0,     0,   753,     0,   754,     0,     0,     0,
     756,     0,     0,     0,     0,     0,   760,     0,     0,     0,
       0,     0,   761,   763,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   773,     0,     0,   775,
     776,   777,   778,     0,     0,     0,     0,     0,     0,   781,
       0,     0,     0,   784,   783,     0,     0,     0,     0,     0,
       0,     0,   792,     0,   794,     0,     0,   797,     0,   799,
       0,   801,     0,     0,     0,     0,     0,   814,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   817,     0,   820,
       0,     0,   823,     0,   826,     0,   829,     0,   832,     0,
       0,   834,     0,     0,     0,     0,     0,     0,     0,   838,
       0,     0,   841,     0,   843,     0,     0,     0,     0,   846,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   865,
       0,   867,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   877,     0,   879,     0,
       0,     0,     0,     0,     0,   885,     0,   886,     0,   887,
       0,   888,     0,     0,     0,     0,     0,   135,   137,   139,
     139,   142,     0,     0,   148,   139,   151,   139,   148,   139,
     148,   139,   148,   139,   148,   161,   163,     0,   185,   185,
     185,     0,     0,     0,     0,     0,   930,     0,     0,     0,
       0,     0,     0,     0,   935,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   395,   396,     0,   399,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     244,     0,     0,     0,     0,   272,     0,     0,     0,     0,
       0,     0,     0,     0,   976,     0,     0,     0,     0,   982,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   990,     0,     0,     0,     0,     0,     0,     0,   995,
       0,     0,   999,     0,     0,     0,     0,     0,     0,  1000,
       0,     0,  1004,  1005,  1006,  1007,  1008,  1009,     0,     0,
    1010,     0,  1014,     0,     0,  1018,     0,     0,     0,     0,
    1020,     0,  1029,  1030,  1031,     0,     0,     0,     0,     0,
       0,  1035,  1267,  1230,  1231,  1232,  1233,  1234,  1235,  1236,
    1268,  1237,  1238,  1239,  1240,  1241,  1242,  1243,  1244,  1245,
    1246,  1247,  1248,  1249,  1250,  1251,  1252,  1253,  1254,  1255,
    1256,  1257,  1258,  1259,  1260,  1269,  1270,  1261,  1262,  1263,
    1264,  1265,     0,   559,     0,     0,     0,     0,     0,     0,
       0,   565,     0,     0,     0,     0,   568,     0,  1073,     0,
       0,     0,     0,   575,   576,  1078,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   426,     0,
    1090,   594,  1092,   440,   137,     0,     0,     0,     0,  1096,
       0,     0,     0,     0,     0,     0,     0,     0,  1107,  1108,
       0,     0,     0,     0,   481,   137,     0,     0,     0,     0,
       0,     0,     0,   491,     0,     0,     0,     0,  1124,     0,
       0,     0,     0,  1134,     0,   505,  1135,  1140,   508,     0,
       0,     0,  1143,     0,  1146,   515,     0,  1150,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1161,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1202,
       0,  1204,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1206,     0,
    1207,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1219,     0,     0,     0,
    1221,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     779,   780,     0,     0,  1281,     0,   782,     0,     0,     0,
       0,     0,  1287,     0,  1291,  1292,     0,   614,   616,     0,
       0,   796,     0,   798,     0,   800,     0,   802,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   658,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   679,     0,     0,     0,     0,
    1301,     0,     0,     0,     0,     0,     0,  1307,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1313,     0,  1315,     0,  1322,     0,     0,     0,     0,
       0,     0,     0,     0,   745,     0,     0,     0,   185,  1356,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1363,     0,     0,     0,     0,     0,     0,  1368,
       0,     0,     0,     0,     0,     0,     0,  1375,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1392,  1393,     0,     0,
       0,  1230,  1231,  1232,  1233,  1234,  1235,  1236,     0,  1237,
    1238,  1239,  1240,  1241,  1242,  1243,  1244,  1245,  1246,  1247,
    1248,  1249,  1250,  1251,  1252,  1253,  1254,  1255,  1256,  1257,
    1258,  1259,  1260,     0,  1409,  1261,  1262,  1263,  1264,  1265,
       0,  1418,   944,   945,   946,   947,     0,   949,   950,     0,
       0,     0,     0,     0,     0,     0,     0,   957,     0,   959,
     960,     0,     0,  1427,     0,     0,     0,   964,   965,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     972,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   852,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   869,
       0,   869,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   904,     0,
       0,   909,     0,     0,     0,     0,     0,     0,     0,     0,
     919,     0,     0,     0,     0,     0,   679,     0,     0,   679,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1066,     0,     0,     0,     0,
    1069,  1070,     0,  1071,     0,     0,     0,     0,     0,  1072,
       0,     0,  1074,  1274,  1230,  1231,  1232,  1233,  1234,  1235,
    1236,  1275,  1237,  1238,  1239,  1240,  1241,  1242,  1243,  1244,
    1245,  1246,  1247,  1248,  1249,  1250,  1251,  1252,  1253,  1254,
    1255,  1256,  1257,  1258,  1259,  1260,  1276,  1277,  1261,  1262,
    1263,  1264,  1265,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1050,     0,     0,
       0,     0,     0,     0,     0,     0,  1055,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1215,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1226,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   869,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   679,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1311,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1181,     0,     0,  1181,     0,     0,     0,  1181,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1358,   869,
       0,  1359,  1360,     0,  1362,     0,     0,     0,  1364,  1365,
       0,  1367,     0,  1370,     0,     0,  1371,  1372,     0,  1374,
       0,  1377,     0,  1378,  1379,     0,  1381,  1382,     0,  1384,
    1385,     0,  1386,     0,  1388,  1389,     0,  1390,     0,  1391,
       0,     0,     0,     0,   869,   869,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1283,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1408,     0,
    1410,  1412,     0,  1414,  1415,  1417,     0,  1419,  1420,  1421,
    1422,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1426,     0,  1428
};

static const yytype_int16 yycheck[] =
{
      57,   558,   196,   192,     0,   346,   227,   374,   115,   201,
       0,   220,   131,   132,   223,   434,   242,   853,   434,   204,
     206,   208,   836,   528,   133,   955,   352,   411,   369,   592,
     821,   598,   656,   212,   678,   783,     0,   604,   821,   235,
     836,   824,     0,   697,   838,   379,   893,   841,  1135,   243,
     832,   156,   694,   158,   403,   836,  1057,  1075,  1062,   997,
    1064,  1065,     0,     4,     6,     7,  1297,  1298,  1299,  1300,
      12,  1302,    25,  1304,    49,    17,   163,   742,    19,   136,
       5,   138,     4,   140,   141,   365,   143,    28,    14,     6,
      15,    17,   149,   150,  1295,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   195,   162,     6,   164,     6,    40,
       0,   220,     9,    88,   223,    56,     4,    17,    16,    60,
      17,    17,     5,   147,    35,    78,  1144,   184,   195,   186,
     187,   188,    15,   190,   201,  1366,    61,    62,  1369,    80,
      82,    38,  1373,    84,  1148,  1376,   720,    78,   722,  1380,
     724,   347,   726,    40,   728,    51,   952,    19,    80,     3,
    1361,   350,    84,    89,   821,    82,    28,   824,    56,   274,
     827,   952,    60,    65,   368,    86,    87,     9,    61,   993,
    1411,    25,    82,   974,    82,  1416,   378,  1034,   245,   163,
     247,   974,    80,    17,   977,   416,    84,   254,    90,   256,
     163,   397,    26,   260,   261,  1001,    38,   989,   404,   266,
     267,   268,   269,   539,   271,   163,   273,   274,   275,   603,
    1056,  1151,   418,  1224,   403,  1163,  1313,   284,   872,  1076,
      31,    55,   289,   290,   878,   292,   570,   201,   862,   893,
     198,   298,   866,   569,   807,   302,   303,   752,   890,   598,
     307,   308,    53,    54,   356,   312,   313,   159,   160,    67,
     540,     5,   927,   320,   929,  1112,    49,   324,    65,   836,
     327,    15,   552,    61,    71,   377,   470,   334,    35,   336,
     337,   338,   339,   340,    92,   342,    14,   513,  1079,    17,
      73,    24,  1222,    90,    22,  1131,  1079,  1080,    17,    56,
     409,  1097,  1096,   412,     7,    88,    39,    26,    11,    12,
      13,    14,   202,     7,   155,    48,   157,   974,    12,   209,
     977,    17,   979,    17,   898,  1073,   983,    23,   902,    86,
      87,    17,   906,   907,   908,    54,     7,   911,   912,    64,
      11,   915,   163,   816,    77,   818,    17,   543,   821,   386,
      17,   824,   389,    17,   827,   392,   697,   830,    22,    26,
     155,    89,   157,  1293,    50,    51,    69,   357,    30,    31,
     427,   428,  1155,     9,   364,   371,    41,   434,   435,     7,
     437,    17,   574,    11,   441,   442,   443,  1052,   445,    67,
     774,    53,    54,   589,    72,   580,   592,   583,   455,   586,
     457,   458,   150,   460,   600,   462,   463,   633,   465,   466,
    1224,   468,   469,  1161,    92,   150,   473,   474,   475,   598,
    1144,   478,  1079,  1080,  1081,  1082,   483,   484,  1152,    17,
     487,   488,    20,   759,    22,   851,   149,   494,   495,  1104,
     497,   498,    17,   500,   501,     7,  1298,    57,  1284,   790,
      12,    26,  1304,   510,   511,   512,   547,   548,  1206,  1207,
     517,    48,   519,   882,   521,   791,   523,  1041,  1042,  1043,
    1044,   619,   563,   564,   622,   765,   766,   767,    53,    54,
     547,   548,     7,  1212,   374,  1214,    11,    12,    13,   408,
      24,   610,    17,    16,   384,   414,   563,   564,  1155,  1156,
    1157,   697,    35,   793,   977,    17,   979,   574,    47,   546,
     983,    17,   549,    32,   551,    21,    22,   554,   555,    33,
    1185,   161,   162,    56,    25,   562,   156,    60,  1193,  1194,
    1097,   161,   162,   163,   147,   148,   573,    70,   151,   152,
     153,    62,   579,    45,    69,   582,  1120,    17,   585,   151,
     152,   153,   893,    86,    87,   612,    26,    74,    75,   596,
      30,    31,   619,  1220,    17,   622,   762,   624,  1146,   626,
     154,   155,  1150,    26,  1141,    58,    59,  1144,    31,    61,
      62,  1148,   155,    53,    54,  1152,   156,   644,    53,    54,
      32,    33,   649,   650,   651,    29,   653,   654,   655,   656,
      53,    54,   659,   795,  1305,  1306,  1012,  1013,   804,  1082,
     148,   807,   808,   151,   152,   153,   147,  1282,   675,   676,
     677,   678,   157,   680,  1289,   682,   120,   684,   122,   686,
     124,     8,   126,    54,  1001,   369,   832,   826,   847,   346,
     829,  1208,   695,   533,   894,   148,   703,   537,   151,   152,
     153,   154,   155,   156,   891,   158,   997,  1224,   715,   816,
     717,   817,   769,   720,   818,   722,   556,   724,   974,   726,
     527,   728,   925,   926,   731,   739,   740,   734,  1088,   786,
    1138,  1139,   739,   740,  1228,   742,     7,  1184,  1188,   746,
      11,    12,    13,  1034,   751,   571,    17,   893,   894,    20,
    1064,    22,  1148,  1300,  1306,   147,   148,   149,   598,   151,
     152,   153,   154,   155,   604,  1303,     0,     7,   608,     8,
     201,    11,    12,    13,   536,   624,   843,    17,    17,   129,
    1097,    21,    22,   129,   129,  1076,   129,    26,   129,  1295,
      29,    30,    31,  1299,  1300,   199,  1302,  1303,    69,  1305,
    1306,   199,  1308,     7,   958,   478,   129,    11,    12,    13,
     954,   955,   129,    17,    53,    54,    55,   570,  1216,   129,
     129,  1112,   968,   129,   995,   112,   398,   129,   768,    69,
     117,   771,   119,   195,   121,   195,   123,   597,   125,   129,
     597,   597,   597,   989,   851,   785,   853,   986,   788,   988,
     129,   129,   129,   129,   129,   862,     0,   731,  1034,   866,
    1366,   868,   334,   870,   871,   872,     0,  1373,   875,   815,
      10,   878,  1163,   339,   881,   882,   337,  1383,    18,   148,
     149,  1387,   151,   152,   153,   154,   155,    27,  1034,   158,
    1392,   898,     0,   900,    34,   902,   942,    37,  1097,   906,
     907,   908,    -1,    -1,   911,   912,  1413,   914,   915,    -1,
     917,    -1,    52,   920,    -1,    -1,    -1,    -1,   925,   926,
     927,    -1,   929,    63,   931,   932,    66,     7,    68,   769,
    1076,    11,    12,    13,   941,   942,    -1,    17,   361,    79,
      -1,    81,    22,    83,    -1,    85,   786,    -1,  1087,   372,
     373,    91,   362,   363,    -1,    -1,   366,   367,   381,   148,
     149,    -1,   151,   152,   153,   388,  1112,    -1,   391,    -1,
     380,   394,    -1,    -1,    -1,    -1,   816,   387,   818,   402,
     390,   821,    -1,   393,   824,    -1,    -1,   827,    -1,    69,
     830,   401,    -1,  1000,    -1,    -1,   836,    -1,  1005,  1006,
    1007,  1008,    -1,    -1,   844,  1012,  1013,  1151,    -1,   148,
      -1,    -1,   151,   152,   153,   154,   155,    -1,    -1,   158,
    1159,   148,   149,  1030,   151,   152,   153,   154,   155,    -1,
    1037,  1038,  1039,    -1,  1041,  1042,  1043,  1044,    -1,  1046,
      -1,  1048,    -1,    -1,    -1,  1052,    -1,  1054,    -1,  1056,
      -1,    -1,    -1,  1060,    -1,    -1,    -1,    -1,     7,     8,
      -1,    -1,    11,    12,    13,    14,    -1,    -1,    17,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,  1222,    -1,
      29,    30,    31,    93,    94,    95,    96,    97,    98,    99,
     100,   101,    -1,  1100,    -1,  1102,    -1,  1104,  1105,    -1,
      -1,    -1,  1109,  1110,    53,    54,    55,  1114,  1115,    -1,
    1117,    -1,   952,  1120,    17,  1122,    -1,    -1,    -1,    -1,
      69,    17,   962,    26,  1131,    -1,    29,    30,    31,  1136,
      26,  1138,  1139,    29,    30,    31,    -1,   977,    -1,   979,
      89,    -1,    -1,   983,    -1,    -1,    -1,    43,    -1,  1293,
      53,    54,    55,   993,    -1,    -1,    -1,    53,    54,    55,
      -1,     7,    -1,    -1,    -1,    11,    12,    13,    14,    -1,
      -1,    17,    -1,  1180,    -1,  1182,    -1,  1184,  1185,  1186,
      26,  1188,  1189,    29,    30,    31,  1193,  1194,    -1,  1196,
      36,    -1,    -1,     9,    -1,    -1,  1203,  1204,    -1,    -1,
      -1,    17,    -1,    -1,    -1,    -1,    -1,    53,    54,    55,
      26,    -1,    -1,    29,    30,    31,    -1,  1363,    -1,    -1,
    1227,  1228,    38,    69,    -1,    -1,    42,    43,    -1,  1368,
      -1,    -1,    -1,    -1,    -1,    -1,  1375,    53,    54,    55,
      -1,    -1,  1082,    89,    -1,   147,   148,   149,    -1,   151,
     152,   153,   154,   155,    -1,    -1,    -1,  1097,    -1,  1266,
    1267,  1268,  1269,  1270,    -1,    -1,  1273,  1274,  1275,  1276,
    1277,     9,    -1,  1280,    -1,  1282,    -1,  1284,    -1,    17,
      -1,    -1,  1289,    -1,    -1,    -1,    -1,    -1,    26,    -1,
      -1,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      38,  1141,    -1,    -1,  1144,    43,    -1,    -1,  1148,    -1,
      -1,    -1,  1152,  1320,    -1,    53,    54,    55,    -1,    -1,
    1327,    -1,  1329,    -1,    -1,    -1,    -1,    -1,  1335,    -1,
    1337,    -1,    -1,    -1,    -1,  1342,    -1,    -1,  1345,    -1,
      -1,    -1,    -1,  1350,    -1,    -1,    45,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    53,    -1,    55,    -1,    -1,    58,
      -1,    60,    -1,    62,    -1,    -1,    -1,    66,  1208,    68,
      -1,    70,    -1,    72,    -1,    74,  1216,    76,    -1,    78,
      -1,    80,    -1,    -1,  1224,  1392,    -1,    -1,    -1,    88,
    1397,  1398,    -1,    -1,  1401,    -1,    95,    -1,    97,    -1,
      99,    -1,   101,    -1,   103,     7,   105,    17,    -1,    11,
      12,    13,    14,    -1,    -1,    17,    26,    -1,    -1,    29,
      30,    31,    -1,    -1,    26,    -1,    -1,    29,    30,    31,
      -1,    -1,    -1,    43,    -1,    45,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    53,    54,    55,    -1,    -1,   147,    -1,
      -1,    53,    54,    55,    -1,  1295,    -1,  1297,  1298,  1299,
    1300,    -1,  1302,  1303,  1304,  1305,  1306,    69,  1308,    10,
      -1,    -1,    -1,    -1,     9,    -1,    17,    18,    -1,    -1,
      -1,    -1,    17,    -1,    -1,    26,    27,    89,    29,    30,
      31,    26,   191,    34,    29,    30,    31,    -1,    -1,    10,
      -1,    -1,   203,    38,   205,    46,   207,    18,    -1,   210,
      -1,    52,    53,    54,    55,    -1,    27,    -1,    53,    54,
      55,    -1,    63,    34,    -1,    66,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    79,    -1,
      81,    52,    83,    -1,    85,    -1,    -1,    -1,    -1,   248,
      91,    -1,    63,    -1,   253,    66,   255,    68,    -1,   258,
     259,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    79,    -1,
      81,    -1,    83,    -1,    85,    -1,    -1,    -1,    -1,    -1,
      91,   280,   281,    -1,    -1,    -1,   285,    -1,    -1,   288,
      -1,    -1,    -1,    -1,    17,    -1,   295,    -1,    -1,    -1,
     299,    -1,    -1,    26,    -1,   304,    29,    30,    31,    -1,
     309,    -1,    -1,    36,    -1,   314,    -1,    -1,    -1,    -1,
     319,    -1,   321,    -1,    -1,    -1,   325,    -1,     4,     5,
      53,    54,    55,   332,    10,    -1,   335,    -1,    -1,    15,
      -1,    17,    18,   344,   343,    -1,    -1,    -1,    24,    -1,
     351,    27,   353,   354,    -1,    -1,    -1,    -1,    34,    35,
      -1,    37,    -1,    39,    -1,    -1,    -1,    -1,    44,   370,
      46,    -1,    -1,    49,   375,    -1,    52,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    -1,    -1,    63,    -1,    65,
      66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    77,    -1,    79,    80,    81,    -1,    83,    84,    85,
      86,    87,    88,    -1,    90,    91,    92,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     429,    -1,   431,    -1,    -1,    -1,    -1,   436,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   446,    -1,    -1,
      -1,    -1,    -1,   454,   453,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   461,   464,    -1,    -1,    -1,    -1,    -1,    -1,
     471,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     479,    -1,    -1,    -1,    -1,   486,   485,    -1,    -1,    -1,
      -1,   492,   493,    -1,    -1,   496,    -1,    -1,   499,    -1,
      -1,    -1,    -1,    -1,    -1,   506,    -1,    -1,   509,    -1,
      -1,    -1,    -1,   514,    -1,   516,    -1,    -1,    -1,    -1,
      -1,   520,    -1,    -1,   525,   524,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   538,    -1,    -1,
      -1,   542,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   557,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   568,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   599,    -1,
      -1,    -1,    -1,    -1,   605,    -1,    -1,    -1,   609,    -1,
      -1,    -1,    -1,    -1,   615,    -1,   617,    -1,    -1,    -1,
     621,    -1,    -1,    -1,    -1,    -1,   627,    -1,    -1,    -1,
      -1,    -1,   631,   634,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   657,    -1,    -1,   660,
     661,   662,   663,    -1,    -1,    -1,    -1,    -1,    -1,   668,
      -1,    -1,    -1,   674,   673,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   683,    -1,   685,    -1,    -1,   688,    -1,   690,
      -1,   692,    -1,    -1,    -1,    -1,    -1,   698,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   716,    -1,   718,
      -1,    -1,   721,    -1,   723,    -1,   725,    -1,   727,    -1,
      -1,   730,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   738,
      -1,    -1,   741,    -1,   743,    -1,    -1,    -1,    -1,   748,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   770,
      -1,   772,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   787,    -1,   789,    -1,
      -1,    -1,    -1,    -1,    -1,   796,    -1,   798,    -1,   800,
      -1,   802,    -1,    -1,    -1,    -1,    -1,   109,   110,   111,
     112,   113,    -1,    -1,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,   130,   131,
     132,    -1,    -1,    -1,    -1,    -1,   837,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   845,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   317,   318,    -1,   320,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     192,    -1,    -1,    -1,    -1,   197,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   905,    -1,    -1,    -1,    -1,   910,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   922,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   928,
      -1,    -1,   933,    -1,    -1,    -1,    -1,    -1,    -1,   940,
      -1,    -1,   943,   944,   945,   946,   947,   948,    -1,    -1,
     951,    -1,   953,    -1,    -1,   956,    -1,    -1,    -1,    -1,
     961,    -1,   963,   964,   965,    -1,    -1,    -1,    -1,    -1,
      -1,   972,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,    -1,   469,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   477,    -1,    -1,    -1,    -1,   482,    -1,  1027,    -1,
      -1,    -1,    -1,   489,   490,  1036,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   350,    -1,
    1051,   507,  1053,   355,   356,    -1,    -1,    -1,    -1,  1058,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1069,  1070,
      -1,    -1,    -1,    -1,   376,   377,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   385,    -1,    -1,    -1,    -1,  1089,    -1,
      -1,    -1,    -1,  1094,    -1,   397,  1095,  1098,   400,    -1,
      -1,    -1,  1101,    -1,  1103,   407,    -1,  1106,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1133,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1160,
      -1,  1162,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1177,    -1,
    1179,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1195,    -1,    -1,    -1,
    1201,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     666,   667,    -1,    -1,  1215,    -1,   672,    -1,    -1,    -1,
      -1,    -1,  1223,    -1,  1225,  1226,    -1,   529,   530,    -1,
      -1,   687,    -1,   689,    -1,   691,    -1,   693,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   550,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   567,    -1,    -1,    -1,    -1,
    1271,    -1,    -1,    -1,    -1,    -1,    -1,  1278,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1290,    -1,  1294,    -1,  1296,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   606,    -1,    -1,    -1,   610,  1310,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1321,    -1,    -1,    -1,    -1,    -1,    -1,  1328,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1336,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1357,  1358,    -1,    -1,
      -1,   108,   109,   110,   111,   112,   113,   114,    -1,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,    -1,  1395,   142,   143,   144,   145,   146,
      -1,  1402,   858,   859,   860,   861,    -1,   863,   864,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   873,    -1,   875,
     876,    -1,    -1,  1424,    -1,    -1,    -1,   883,   884,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     896,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   761,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   781,
      -1,   783,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   820,    -1,
      -1,   823,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     832,    -1,    -1,    -1,    -1,    -1,   838,    -1,    -1,   841,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1011,    -1,    -1,    -1,    -1,
    1016,  1017,    -1,  1019,    -1,    -1,    -1,    -1,    -1,  1025,
      -1,    -1,  1028,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   989,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   998,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1190,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1205,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1073,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1096,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1286,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1143,    -1,    -1,  1146,    -1,    -1,    -1,  1150,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1314,  1161,
      -1,  1317,  1318,    -1,  1320,    -1,    -1,    -1,  1324,  1325,
      -1,  1327,    -1,  1329,    -1,    -1,  1332,  1333,    -1,  1335,
      -1,  1337,    -1,  1339,  1340,    -1,  1342,  1343,    -1,  1345,
    1346,    -1,  1348,    -1,  1350,  1351,    -1,  1353,    -1,  1355,
      -1,    -1,    -1,    -1,  1206,  1207,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1219,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1394,    -1,
    1396,  1397,    -1,  1399,  1400,  1401,    -1,  1403,  1404,  1405,
    1406,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1423,    -1,  1425
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     4,     5,    10,    15,    17,    18,    24,    27,    34,
      35,    37,    39,    44,    46,    49,    52,    60,    63,    65,
      66,    67,    68,    76,    77,    79,    80,    81,    83,    84,
      85,    86,    87,    88,    90,    91,    92,   165,   166,   167,
     238,   240,   306,   307,   310,   312,   333,   334,   335,   336,
     337,   338,   341,   342,   344,   346,   354,   355,   356,   362,
     363,   374,   376,   387,   388,   389,   396,   397,   398,   399,
     400,   401,   402,   403,   404,   405,   406,   407,   408,   409,
     411,   432,   433,   434,   436,   438,   439,   445,   446,   451,
     456,   459,   462,   465,   466,   469,   474,   476,   479,   483,
     496,   500,   501,   506,   508,   509,     0,   159,   160,   202,
     202,   202,   202,   202,   163,   204,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   147,   205,   241,   205,   313,   205,
     340,   340,   205,   347,     6,    82,   350,   351,   205,   357,
     340,   205,   377,   340,   357,   340,   357,   340,   357,   340,
     357,   205,   447,   205,   470,   335,   336,   337,   338,   362,
     388,   389,   399,   403,   407,   432,   445,   451,   456,   459,
     462,   465,   475,   477,   478,   205,   484,   484,   484,    40,
     488,   489,   204,   204,   204,   204,   204,   202,   204,   204,
     204,   204,   204,   358,   204,   358,   204,   358,   204,   204,
     410,   204,   204,   161,   162,   203,   204,   204,   480,   481,
     204,   497,   498,   204,   502,   503,   204,   202,   148,   149,
     151,   152,   153,   154,   155,   168,   169,   170,   172,   174,
     175,   178,   180,   181,   205,   242,    61,   314,   316,    19,
      28,    56,   307,   312,   323,   324,   343,   355,   370,   371,
     435,   437,   440,   442,   443,   444,   323,   343,   435,   437,
     172,   348,   205,   352,   344,   364,   365,   367,   368,   369,
     370,   371,     3,    25,   378,   379,    70,   310,   312,   323,
     390,   397,   452,   453,   454,   455,    23,   238,   359,   360,
     203,    71,   401,   457,   458,   203,    72,   405,   460,   461,
     203,    73,   409,   463,   464,    50,    51,   238,   412,   413,
     415,   416,   203,    64,   448,   449,   466,   467,   508,    74,
      75,   471,   472,    78,   485,   486,   488,   485,   488,   485,
     488,    41,   490,   491,   167,   180,   150,   168,   179,   172,
     204,   239,   204,   308,   311,   202,   202,   204,   202,   204,
     202,   202,   204,   204,   204,   204,   204,   204,   204,   204,
     358,   204,   202,   202,   204,   375,   202,   202,   204,   204,
     204,   202,   204,   259,   204,   202,   204,   204,   202,   204,
     204,   202,   204,   204,   202,   259,   259,   202,   204,   259,
     202,   204,   202,   204,   202,   204,   481,   202,   204,   204,
     498,   204,   204,   503,   204,   204,   202,   203,   173,   176,
     177,   181,   180,    32,    33,   169,   205,   243,   244,   245,
     247,   248,   203,    62,   314,   319,   320,   344,   203,   203,
     205,   317,   313,   355,    57,   325,   326,    16,   268,   269,
     271,   273,   274,   276,   339,   350,   205,   372,   372,    48,
     418,   420,   418,   355,   339,   418,   418,   172,   349,   171,
     173,   353,   203,   374,   372,   372,     9,   238,   381,   383,
     203,   205,   380,   313,   397,   276,   391,   418,   372,   238,
     238,   205,   361,   268,   418,   372,   268,   418,   372,   268,
     418,   372,   180,   184,   186,   205,   414,   412,   205,   417,
     418,   372,   467,   180,   473,   205,   487,   490,    25,   422,
     423,   490,    47,   510,   515,   167,   180,   204,   204,   202,
     202,   308,   311,   204,   309,   202,   204,   204,   315,   204,
     204,   204,   328,   202,   202,   203,   204,   204,   204,   204,
     202,   204,   204,   203,   204,   204,   204,   345,   204,   259,
     172,   203,   204,   204,   204,   259,   375,   202,   259,   204,
     204,   202,   203,   204,   204,   259,   259,   203,   203,   204,
     204,   203,   204,   204,   203,   204,   204,   155,   157,   183,
     185,   189,   192,   203,   259,   203,   204,   204,   204,   468,
     168,   203,   203,   204,   204,   499,   202,   204,   204,   207,
     202,   203,   244,   247,   205,   246,   205,   249,   238,   322,
     169,   321,   319,   238,   318,   203,   314,   339,    58,    59,
     329,   331,   203,   180,   327,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   277,
     278,   279,   268,   323,   343,   323,   343,   268,   205,   421,
     268,   339,   268,   268,     7,    11,   238,   260,   264,   203,
       7,    12,   258,   263,   268,   323,   343,   323,   343,   205,
     384,   203,   314,   391,   277,   268,   323,   397,   268,   401,
     268,   405,   268,   409,   180,   187,   180,   193,   268,   440,
     442,   443,   444,   450,    26,    29,    30,    31,    53,    54,
      55,   208,   210,   211,   212,   213,   214,   215,   216,   219,
     220,   221,   223,   224,   229,   231,   234,   235,   238,   250,
     251,   467,   203,   180,   422,    38,    43,   208,   383,   425,
     429,   430,   494,   495,   203,   205,   424,    45,   507,   208,
     203,   484,   204,   203,   203,   309,   203,   309,   315,   204,
     203,   202,   168,   203,   204,   204,   204,   204,   204,   204,
     441,   204,   441,   203,   204,   203,   203,   203,   203,   259,
     259,   202,   259,   202,   203,   204,   204,   366,   204,   366,
     204,   204,   203,   204,   203,   204,   259,   203,   259,   203,
     259,   203,   259,   185,   183,   156,   158,   181,   182,   190,
     193,   198,   199,   200,   203,   204,   204,   202,   204,   209,
     202,   204,   209,   202,   204,   209,   202,   204,   209,   202,
     204,   209,   202,   209,   202,   468,   204,   482,   202,   499,
     499,   202,   207,   202,   204,   330,   202,   204,   511,   512,
     247,   314,   205,   332,   180,   151,   152,   153,   280,   280,
     280,   280,   355,   238,   350,   203,   355,   203,   422,   205,
     266,   266,   355,   238,   258,   350,   373,   203,   355,   203,
     171,   385,   314,   280,   397,   203,   203,   203,   203,   180,
     188,   193,   180,   194,   182,   201,   374,   212,   238,   250,
     215,   219,   238,   250,   205,   217,   223,   229,   234,   205,
     222,   229,   234,   169,   225,   234,   169,   232,   184,   205,
     236,   147,   206,    42,   208,   425,   429,   492,   493,   494,
     203,   384,   384,   321,   238,   203,   374,   418,   479,   500,
     504,   422,   485,   330,   259,   259,   259,   259,   441,   259,
     259,   441,   204,   419,   204,   204,   366,   259,   204,   259,
     259,   366,   204,   382,   259,   259,   185,   201,   182,   191,
     198,   194,   259,   209,   204,   209,   203,   204,   209,   204,
     209,   209,   203,   204,   209,   209,   204,   209,   204,   204,
     203,   482,   482,   204,   207,   202,   207,   204,   204,   203,
     203,   204,   204,   512,   203,   203,   203,   203,   203,   203,
     203,   238,   425,   429,   203,   172,   267,   267,   203,   373,
     203,     7,    11,    12,    13,   238,   256,   257,   386,   203,
     203,   203,   180,   195,   196,   203,   218,   220,   223,   229,
     234,   229,   234,   234,   234,   169,   226,   169,   233,   184,
     205,   237,   494,   167,   385,   205,   431,   204,   383,   429,
     513,   516,   204,   204,   204,   204,   259,   419,   419,   259,
     259,   259,   259,   202,   259,   204,   168,   198,   203,   204,
     204,   204,   204,   209,   209,   209,   209,   204,   204,   230,
     203,   207,   203,   204,   330,   507,   202,   204,   514,     8,
     282,   284,   281,   284,   282,   283,   284,   203,   203,   266,
     281,   180,   197,   198,   223,   229,   234,   229,   234,   234,
     234,   169,   227,   260,   203,     7,    11,    12,    13,    14,
      69,   426,   427,   428,   203,   202,   384,   208,   429,   516,
     203,   204,   272,   202,   204,   270,   202,   207,   204,   275,
     202,   204,   204,   392,   198,   204,   204,   204,   209,   204,
     330,   202,   504,   204,   514,   514,     7,    11,    12,    13,
      14,    69,    89,   208,   252,   253,   254,   255,   261,   265,
     305,   205,   285,   208,   281,   305,   285,   208,   283,   285,
     267,    36,   208,   305,   393,   394,   229,   234,   234,   234,
     169,   228,   203,   266,   203,   385,   202,   202,   204,   204,
     270,   207,   204,   275,   204,   259,   204,   207,   207,   202,
     204,   203,   204,   262,   204,   505,   259,   266,   266,   208,
     108,   109,   110,   111,   112,   113,   114,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   142,   143,   144,   145,   146,   298,   107,   115,   140,
     141,   286,   289,   298,   107,   115,   140,   141,   291,   294,
     298,   203,   393,   205,   395,   234,   267,   203,   208,   494,
     507,   203,   203,   204,   262,   204,   296,   204,   204,   204,
     204,   203,   204,   204,   204,   204,   204,   203,   204,   207,
     330,   259,   207,   202,   267,   203,    22,   238,   261,   297,
     303,   304,   203,    20,   238,   253,   287,   299,   300,   303,
     287,    21,   238,   253,   288,   301,   302,   303,   288,   238,
     253,   290,   303,   238,   292,   299,   303,   287,   238,   293,
     301,   303,   293,   238,   295,   303,   203,   504,   259,   259,
     259,   204,   259,   202,   259,   259,   204,   259,   202,   204,
     259,   259,   259,   204,   259,   202,   204,   259,   259,   259,
     204,   259,   259,   204,   259,   259,   259,   204,   259,   259,
     259,   259,   203,   203,   261,   180,   253,   303,   169,   253,
     253,   303,   169,   253,   253,   303,   303,   505,   259,   203,
     259,   204,   259,   204,   259,   259,   204,   259,   203,   259,
     259,   259,   259,   253,   258,   253,   259,   203,   259
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   164,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   166,   166,   166,   166,   166,   166,   167,   167,
     168,   169,   169,   170,   171,   172,   172,   173,   173,   174,
     175,   176,   177,   178,   178,   179,   179,   180,   180,   180,
     180,   181,   181,   182,   183,   184,   184,   184,   185,   185,
     186,   187,   188,   189,   190,   190,   191,   191,   192,   193,
     194,   195,   195,   195,   196,   197,   198,   198,   199,   200,
     200,   201,   201,   202,   202,   203,   203,   204,   205,   206,
     207,   207,   208,   208,   208,   209,   209,   209,   210,   210,
     211,   211,   211,   212,   212,   212,   212,   213,   214,   215,
     216,   217,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   230,   231,   232,   233,   234,   235,   236,
     236,   237,   237,   238,   239,   239,   239,   239,   239,   239,
     239,   240,   241,   242,   242,   243,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   252,   253,   254,   255,
     255,   255,   255,   255,   256,   257,   257,   257,   257,   258,
     259,   259,   260,   261,   262,   262,   263,   263,   264,   264,
     265,   265,   266,   267,   268,   268,   268,   268,   269,   270,
     270,   270,   270,   271,   272,   272,   272,   272,   273,   274,
     275,   275,   275,   276,   277,   277,   277,   277,   277,   277,
     277,   277,   277,   278,   278,   279,   279,   280,   280,   280,
     281,   282,   283,   284,   285,   286,   286,   286,   286,   286,
     286,   286,   286,   286,   287,   287,   287,   287,   287,   287,
     287,   287,   288,   288,   288,   288,   288,   288,   288,   288,
     289,   289,   290,   290,   290,   290,   291,   291,   291,   291,
     291,   291,   291,   291,   291,   292,   292,   292,   292,   293,
     293,   293,   293,   294,   294,   295,   295,   296,   296,   297,
     297,   297,   297,   298,   298,   298,   298,   298,   298,   298,
     298,   298,   298,   298,   298,   298,   298,   298,   298,   298,
     298,   298,   298,   298,   298,   298,   298,   298,   298,   298,
     298,   298,   298,   298,   298,   298,   298,   298,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   306,   307,   308,
     308,   309,   309,   310,   311,   311,   312,   313,   314,   315,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   327,   327,   328,   328,   329,   330,   330,
     331,   331,   332,   333,   333,   333,   334,   334,   335,   336,
     337,   338,   339,   339,   340,   341,   341,   342,   342,   343,
     343,   344,   345,   345,   345,   346,   346,   347,   348,   349,
     350,   351,   351,   352,   353,   353,   354,   354,   355,   356,
     356,   356,   357,   358,   358,   358,   358,   359,   360,   361,
     362,   363,   363,   364,   364,   364,   364,   365,   366,   366,
     366,   366,   366,   367,   368,   369,   370,   371,   372,   373,
     374,   375,   375,   375,   376,   377,   378,   379,   379,   380,
     381,   382,   382,   382,   383,   384,   385,   386,   387,   387,
     388,   389,   390,   390,   391,   392,   392,   392,   392,   392,
     393,   394,   395,   396,   396,   397,   398,   398,   398,   399,
     400,   400,   401,   402,   402,   403,   404,   405,   406,   406,
     407,   408,   409,   410,   410,   410,   410,   410,   411,   411,
     412,   413,   414,   414,   415,   416,   417,   418,   419,   419,
     419,   419,   420,   421,   422,   423,   424,   425,   426,   427,
     428,   428,   428,   428,   428,   428,   429,   430,   431,   432,
     432,   432,   433,   433,   434,   435,   435,   436,   437,   437,
     438,   439,   440,   441,   441,   441,   442,   443,   444,   445,
     446,   447,   448,   449,   450,   450,   450,   450,   451,   452,
     452,   453,   454,   455,   456,   457,   458,   459,   460,   461,
     462,   463,   464,   465,   466,   466,   466,   466,   466,   466,
     466,   466,   466,   466,   466,   466,   467,   467,   468,   468,
     468,   469,   470,   471,   472,   472,   473,   473,   473,   474,
     475,   475,   476,   477,   477,   477,   477,   477,   477,   477,
     477,   477,   477,   477,   477,   477,   477,   478,   478,   478,
     478,   478,   478,   478,   479,   480,   480,   481,   482,   482,
     482,   482,   482,   482,   482,   483,   484,   485,   486,   487,
     488,   489,   490,   491,   492,   493,   494,   495,   496,   497,
     497,   498,   499,   499,   499,   499,   499,   500,   501,   502,
     502,   503,   504,   504,   504,   504,   505,   505,   505,   505,
     506,   507,   508,   509,   510,   511,   511,   512,   513,   513,
     514,   514,   514,   514,   515,   516
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
       1,     1,     1,     1,     1,     1,     1,     1,    10,     0,
       3,     3,     2,    10,     0,     2,     4,     2,    10,    10,
       0,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       6,     7,     6,     1,     1,     1,     1,     3,     1,     3,
       1,     3,     1,     3,     2,     4,     6,     4,     2,     4,
       2,     2,     2,     4,     6,     4,     2,     4,     2,     2,
       1,     3,     2,     2,     4,     2,     1,     1,     3,     1,
       3,     1,     3,     1,     3,     2,     4,     2,     2,     2,
       4,     2,     2,     1,     3,     2,     2,     0,     2,     2,
       2,     4,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     6,
       1,     4,     1,     4,     1,     1,     1,     1,     7,     5,
       3,     0,     3,     7,     3,     3,     1,     1,     5,     0,
       3,     1,     1,     1,     4,     1,     1,     1,     5,     1,
       4,     1,     1,     2,     3,     0,     2,     5,     0,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     8,    10,
       8,    10,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     9,     0,     3,     3,     1,     1,     1,     1,     1,
       6,     1,     1,     1,     4,     2,     1,     3,     7,     1,
       1,     1,     1,     0,     2,     3,     5,     4,     1,     1,
      10,     1,     1,     1,     1,     1,     1,     7,     0,     3,
       5,     3,     3,     9,     7,     9,     1,     1,     1,     1,
       7,     0,     3,     3,     1,     1,     5,     1,     1,     1,
       7,     0,     3,     3,     1,     1,     1,     1,     1,     1,
       8,    10,     1,     1,    10,     0,     3,     5,     3,     2,
       5,     1,     1,     1,     1,     5,     1,     1,     1,     8,
       1,     1,     5,     1,     1,     8,     1,     5,     1,     1,
       8,     1,     5,     0,     3,     5,     3,     3,     1,     1,
       4,     1,     1,     1,     4,     1,     1,     7,     0,     3,
       3,     3,     1,     1,     5,     1,     1,     9,     1,     5,
       1,     1,     1,     1,     1,     1,     7,     1,     1,     1,
       1,     1,     1,     1,    10,     1,     1,    10,     1,     1,
      10,    10,     7,     0,     3,     3,     9,     7,     9,    10,
       1,     1,     9,     1,     1,     1,     1,     1,    10,     1,
       1,     7,     9,     1,    10,     7,     1,    10,     7,     1,
      10,     7,     1,     9,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     3,
       2,     1,     1,     4,     1,     1,     1,     2,     3,     4,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     3,     1,     8,     0,     3,
       3,     3,     5,     3,     2,     1,     1,     4,     1,     1,
       4,     1,     4,     1,     4,     1,     4,     1,     4,     3,
       1,     6,     0,     3,     3,     3,     2,     1,     4,     3,
       1,    16,     1,     1,     1,     1,     0,     6,     3,     2,
       1,     1,     9,     1,     4,     3,     1,     6,     1,     1,
       0,     3,     3,     2,     1,     7
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
