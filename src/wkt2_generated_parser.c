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
#define YYFINAL  105
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3315

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  164
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  352
/* YYNRULES -- Number of rules.  */
#define YYNRULES  692
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1424

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
     955,   960,   960,   962,   965,   966,   970,   975,   975,   975,
     977,   979,   980,   981,   982,   984,   987,   989,   993,   999,
     999,  1003,  1003,  1004,  1004,  1006,  1011,  1012,  1013,  1014,
    1015,  1017,  1023,  1028,  1034,  1036,  1038,  1040,  1044,  1050,
    1051,  1052,  1054,  1056,  1058,  1062,  1062,  1064,  1066,  1071,
    1072,  1074,  1076,  1078,  1080,  1084,  1084,  1086,  1092,  1099,
    1099,  1102,  1109,  1110,  1111,  1112,  1113,  1115,  1119,  1121,
    1123,  1123,  1127,  1132,  1132,  1132,  1136,  1141,  1141,  1143,
    1147,  1147,  1151,  1156,  1158,  1162,  1162,  1166,  1171,  1173,
    1177,  1178,  1179,  1180,  1181,  1183,  1183,  1185,  1188,  1190,
    1190,  1192,  1194,  1196,  1200,  1206,  1207,  1208,  1209,  1211,
    1213,  1217,  1222,  1224,  1227,  1232,  1236,  1242,  1242,  1242,
    1242,  1242,  1242,  1246,  1251,  1253,  1258,  1258,  1259,  1261,
    1261,  1263,  1270,  1270,  1272,  1279,  1279,  1281,  1288,  1295,
    1300,  1301,  1302,  1304,  1310,  1315,  1323,  1329,  1331,  1333,
    1339,  1341,  1341,  1342,  1342,  1346,  1352,  1352,  1354,  1359,
    1365,  1370,  1376,  1381,  1386,  1392,  1397,  1402,  1408,  1413,
    1418,  1424,  1424,  1425,  1425,  1426,  1426,  1427,  1427,  1428,
    1428,  1429,  1429,  1432,  1432,  1434,  1435,  1436,  1438,  1440,
    1444,  1447,  1447,  1450,  1451,  1452,  1454,  1458,  1459,  1461,
    1463,  1463,  1464,  1464,  1465,  1465,  1465,  1466,  1467,  1467,
    1468,  1468,  1469,  1469,  1471,  1471,  1472,  1472,  1473,  1474,
    1474,  1478,  1482,  1483,  1486,  1491,  1492,  1493,  1494,  1495,
    1496,  1497,  1499,  1501,  1503,  1506,  1508,  1510,  1512,  1514,
    1516,  1518,  1520,  1522,  1524,  1529,  1533,  1534,  1537,  1542,
    1543,  1544,  1545,  1546,  1548,  1553,  1558,  1559,  1562,  1568,
    1568,  1568,  1568,  1570,  1571,  1572,  1573,  1575,  1577,  1582,
    1588,  1590,  1595,  1596,  1599,  1605,  1605,  1607,  1608,  1609,
    1610,  1612,  1614
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

#define YYPACT_NINF -1164

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-1164)))

#define YYTABLE_NINF -631

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1563, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164,
   -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164,
   -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164,
   -1164, -1164, -1164, -1164, -1164, -1164, -1164,   160, -1164, -1164,
   -1164,   166, -1164, -1164, -1164,   166, -1164, -1164, -1164, -1164,
   -1164, -1164,   166,   166, -1164,   166, -1164,   166, -1164,   166,
   -1164,   166, -1164, -1164, -1164,   166, -1164,   166, -1164,   166,
   -1164,   166, -1164,   166, -1164,   166, -1164,   166, -1164,   166,
   -1164, -1164, -1164, -1164, -1164, -1164, -1164,   166, -1164, -1164,
   -1164, -1164, -1164, -1164,   166, -1164,   166, -1164,   166, -1164,
     166, -1164,   166, -1164,   166, -1164, -1164, -1164,    39,    39,
      39,    39,    39,    39,    39,    39,    39,    39,    39,    39,
      39,    39,    39,    39,    39,    39,   956,    39,    39,    39,
     167, -1164, -1164,    57, -1164,    57, -1164,    57,    57, -1164,
      57, -1164,    57,    57, -1164,    57,    57,    57,    57,    57,
      57,    57,    57,    57, -1164,    57, -1164,    57, -1164, -1164,
   -1164, -1164,   130, -1164, -1164, -1164, -1164, -1164,   149,   203,
     212, -1164, -1164, -1164, -1164,   270, -1164,    57, -1164,    57,
      57,    57, -1164,    57,   166, -1164,   572,   221,   161,   161,
     426,   218,   280,   163,   609,   239,   270,   246,   270,   309,
     270,    86,   306,   270,   318,  1033, -1164, -1164, -1164,    40,
     100, -1164, -1164,   100, -1164, -1164,   100, -1164, -1164,   348,
     956, -1164, -1164, -1164, -1164, -1164, -1164, -1164,   615, -1164,
   -1164, -1164, -1164,   249,   261,   272,   426, -1164,    57, -1164,
      57,   166, -1164, -1164, -1164, -1164,   166,    57,   166,    57,
   -1164,   166,   166,    57,    57, -1164, -1164, -1164, -1164,    57,
      57,    57,    57, -1164,    57,    57,    57, -1164, -1164, -1164,
   -1164,   166,   166, -1164, -1164,    57,   166, -1164, -1164,   166,
      57,    57, -1164,    57, -1164, -1164,   166, -1164,    57,    57,
     166, -1164, -1164,    57,    57,   166, -1164, -1164,    57,    57,
     166, -1164, -1164,    57,    57,   166, -1164, -1164,    57,    57,
     166,    57,   166, -1164, -1164,    57,   166, -1164,    57, -1164,
   -1164, -1164, -1164,   166, -1164,    57,   166,    57,    57,    57,
      57,    57, -1164,    57,   166,   270, -1164,   350,   615, -1164,
   -1164,   500,   270,   336,   270,   270,    39,    39,   101,   392,
     116,    39,    39,   421,   421,   101,   116,   421,   421,   426,
     270,   449,    39,    39,   419,   270,    39,    39,   191,   463,
     421,    39,   466, -1164,   466,    39,   463,   421,    39,   463,
     421,    39,   463,   421,    39, -1164, -1164,   591,   122, -1164,
      39,   421,    39,  1033,   615,   167, -1164,    39,   348,   167,
   -1164,   469,   167, -1164,   348,   450,   956, -1164,   615, -1164,
   -1164, -1164, -1164, -1164, -1164, -1164, -1164,    57,    57,   166,
   -1164,   166, -1164, -1164,    57,    57,   166,    57, -1164, -1164,
   -1164,    57,    57,    57, -1164,    57,   166, -1164, -1164, -1164,
   -1164, -1164, -1164, -1164, -1164,   166,   270,    57,   166, -1164,
      57,    57, -1164,    57,   166,    57,    57,   270,    57,    57,
   -1164,    57, -1164,    57,    57,    57, -1164,    57,    57,   166,
   -1164, -1164,    57,    57,    57,   166,   270,    57,    57,    57,
      57, -1164,   270,   270,    57,    57,   270,    57,    57,   270,
      57,    57, -1164, -1164,   -10, -1164,   270,    57, -1164,   270,
      57,    57,    57,   272,   270, -1164,   270,    57, -1164,    57,
     166,    57, -1164,    57,   166,   270, -1164,   483,   487,    39,
      39, -1164, -1164,   466, -1164,   523,   464,   466,   270,   221,
     116,   511,   270,   615,  1166, -1164,   463,    39,   107,   107,
     463,    39,   463,   116, -1164,   463,   463,   454,   270,   463,
     107,   107, -1164, -1164,    39,   270,   221,   463,  1211, -1164,
     463,   141, -1164, -1164, -1164, -1164,   463,   189, -1164,   463,
     242, -1164,   463,    89, -1164, -1164,   615, -1164, -1164,   615,
   -1164, -1164, -1164,   463,   280,   896,   270,   615, -1164, -1164,
     469,  1121,   270,    39,   492,  1151,   270,    39, -1164,    57,
   -1164, -1164,   270, -1164,   270, -1164,    57, -1164,   270,    57,
   -1164,    57, -1164,    57,   270, -1164, -1164, -1164,   166, -1164,
     272,   270, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164,
   -1164,    57, -1164, -1164, -1164, -1164,    57,    57,    57, -1164,
   -1164,    57,    57,    57,    57,    57,   270, -1164,    57,   270,
     270,   270,   270, -1164, -1164,    57,    57,   166, -1164,   270,
      57,    57,    57,    57, -1164,    57, -1164,    57,   270,    57,
     270,    57,    57,   270,    57,   270,    57,   270,    57,   333,
     387, -1164,  1043,   270, -1164, -1164, -1164, -1164,    57, -1164,
   -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164,
      57,   166,    57,   166, -1164,    57,   166,    57,   166,    57,
     166,    57,   166,    57, -1164,   166,    57, -1164, -1164,    57,
   -1164, -1164, -1164,   166,    57,    57,   166,    57,   166, -1164,
   -1164,    57, -1164,   166, -1164, -1164,    57,   487, -1164, -1164,
   -1164, -1164, -1164, -1164,   118, -1164,    39,   615, -1164,   600,
     600,   600,   600,   350,   101,   119,   270,   101,   270, -1164,
     469, -1164, -1164, -1164, -1164, -1164, -1164,    39, -1164,   101,
     298,   270,   101,   270,   350,   561, -1164,   600, -1164,   191,
     270, -1164,   270, -1164,   270, -1164,   270, -1164,   615, -1164,
   -1164,   615,   615, -1164,   390, -1164, -1164, -1164, -1164,   449,
     120,   525,   683, -1164,    39,   424, -1164,    39,   399, -1164,
     523,   177, -1164,   523,   314, -1164,   591, -1164,   416, -1164,
     975,   270,    39, -1164, -1164,    39, -1164,   523,   466,   270,
      95,   105, -1164, -1164, -1164,    57, -1164,    57, -1164, -1164,
   -1164, -1164,    57,    57,    57,    57,    57,   426,   270,    57,
      57,    57, -1164,    57, -1164,    57, -1164,    57,    57, -1164,
   -1164,    57, -1164,   166,    57,    57, -1164,    57, -1164, -1164,
      57,    57,    57,    57, -1164, -1164, -1164, -1164, -1164,   437,
     390, -1164,  1043,   615, -1164,    57, -1164,    57, -1164,    57,
   -1164,    57, -1164, -1164,   270,    57,    57,    57, -1164,   270,
      57,    57, -1164,    57,    57, -1164,    57, -1164, -1164,    57,
   -1164,   270, -1164, -1164,    57,    57,    57,   166,    57, -1164,
      57,    57,   270, -1164, -1164, -1164, -1164, -1164, -1164,   270,
      57,    57,   270,   270,   270,   270,   270,   351, -1164, -1164,
   -1164,   270, -1164, -1164,   270,   311,   270,   426,   270, -1164,
      39,   351, -1164, -1164,   270,   704,   270,   270,   270, -1164,
   -1164,   615, -1164, -1164, -1164,   270, -1164,   589, -1164, -1164,
     399, -1164,   177, -1164, -1164, -1164,   177, -1164, -1164,   523,
   -1164,   523,   591, -1164, -1164, -1164,  1019, -1164,   956, -1164,
     350,    39, -1164,    57,   205,   469, -1164, -1164,    57,    57,
      57,    57,    57, -1164, -1164,    57,    57,    57, -1164, -1164,
      57, -1164,    57,    57, -1164, -1164, -1164, -1164, -1164, -1164,
     166,    57, -1164,    57, -1164, -1164, -1164,  1086, -1164,   270,
      57,    57,    57, -1164,    57,    57,    57,    57, -1164,    57,
   -1164,    57, -1164, -1164,   270,    57,   270,    57, -1164,    57,
     492,   166, -1164,    57, -1164,   597,   597,   597,   597, -1164,
   -1164, -1164, -1164,   270,   426, -1164,    39, -1164,   597,  1125,
   -1164, -1164,   386,   587,   558,   177, -1164, -1164, -1164, -1164,
     523,   389,   270, -1164, -1164, -1164,   431,   270,   166,    39,
     902,   270, -1164,    57,   166,    57,   166,    57,    57,   166,
   -1164,    57,    57,    57,   368,  1125, -1164,    57,    57, -1164,
      57, -1164, -1164,    57, -1164,    57, -1164, -1164, -1164, -1164,
   -1164, -1164, -1164, -1164,    57, -1164,   166, -1164,    95,    57,
   -1164,    57,    57, -1164,   818, -1164,    39,   527, -1164,    39,
   -1164,   787, -1164,    39,   270,   426,   676, -1164, -1164,   587,
     558,   558, -1164,   523,   270,    39,   270,   350, -1164, -1164,
   -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164,
   -1164,   166, -1164,   166,    57, -1164,    57, -1164,    57,    57,
      57, -1164,    57,    57, -1164,    57, -1164, -1164,    57,    57,
     166,    57, -1164, -1164, -1164, -1164,   270, -1164,    57,    57,
      57,    39,    39,  1151,  1704, -1164, -1164,  1302, -1164,  1342,
     270,  1051, -1164, -1164,    39,   558, -1164,   426,   270,   839,
     270,   270,    57,    57, -1164, -1164, -1164, -1164, -1164, -1164,
   -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164,
   -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164,
   -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164, -1164,
   -1164,    57,    57,    57,    57,    57,   270, -1164,    57,    57,
      57,    57,    57,   270, -1164,    57, -1164,    57, -1164,    57,
   -1164,    57, -1164, -1164,    57,   166, -1164, -1164,   426,   270,
     136,   270,   326,   326,   576,   576, -1164,   538,   486,   326,
     550,   550, -1164,   183, -1164,   270, -1164, -1164,    95,    57,
   -1164, -1164,    57,    57, -1164,    57,   166, -1164, -1164,    57,
      57, -1164,    57,   166,    57, -1164, -1164,    57,    57, -1164,
      57,   166,    57, -1164,    57,    57, -1164,    57,    57, -1164,
      57,    57, -1164,    57, -1164,    57,    57, -1164,    57, -1164,
      57, -1164,   270,   270, -1164, -1164,   302, -1164,   615, -1164,
   -1164,   538, -1164,   523,   474, -1164, -1164, -1164,   538, -1164,
     523,   474, -1164, -1164, -1164,   474, -1164, -1164,   183, -1164,
   -1164, -1164,   183, -1164, -1164, -1164, -1164,    57, -1164,    57,
     270,    57,    57,    57,    57,    57,    57,   270,    57,    57,
      57,    57, -1164, -1164, -1164, -1164,   474, -1164,   366, -1164,
   -1164,   474, -1164, -1164, -1164, -1164, -1164, -1164,    57,   270,
      57, -1164, -1164, -1164
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   417,   406,   395,   405,   161,   429,   452,   397,   480,
     483,   598,   642,   677,   680,   505,   498,   356,   557,   490,
     487,   495,   493,   609,   664,   396,   419,   430,   398,   418,
     481,   485,   484,   506,   491,   488,   496,     0,     4,     5,
       2,     0,    13,   346,   347,     0,   581,   385,   383,   384,
     386,   387,     0,     0,     3,     0,    12,     0,   583,     0,
      11,     0,   585,   465,   466,     0,    14,     0,   587,     0,
      15,     0,   589,     0,    16,     0,   591,     0,    17,     0,
     582,   538,   536,   537,   539,   540,   584,     0,   586,   588,
     590,   592,    19,    18,     0,     7,     0,     8,     0,     9,
       0,    10,     0,     6,     0,     1,    73,    74,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    78,   162,     0,   357,     0,   394,     0,     0,   407,
       0,   420,     0,     0,   453,     0,     0,   421,     0,   421,
       0,   421,     0,   500,   558,     0,   599,     0,   610,   624,
     611,   625,   612,   613,   627,   614,   615,   616,   617,   618,
     619,   620,   621,   622,   623,     0,   607,     0,   643,     0,
       0,     0,   648,     0,     0,    77,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    75,    76,   606,     0,
       0,   631,   633,     0,   655,   657,     0,   665,   667,     0,
       0,    40,    20,    37,    38,    39,    41,    42,     0,   163,
      21,    22,    26,     0,    25,    35,     0,   164,   154,   361,
       0,     0,   444,   445,   369,   400,     0,     0,     0,     0,
     399,     0,     0,     0,     0,   542,   545,   543,   546,     0,
       0,     0,     0,   408,     0,   421,     0,   431,   432,   433,
     434,     0,     0,   456,   455,   449,     0,   570,   470,     0,
       0,     0,   469,     0,   566,   567,     0,   426,   190,   422,
       0,   482,   573,     0,     0,     0,   489,   576,     0,     0,
       0,   494,   579,     0,     0,     0,   512,   508,   190,   190,
       0,   190,     0,   499,   560,     0,     0,   593,     0,   594,
     601,   602,   608,     0,   645,     0,     0,     0,     0,     0,
       0,     0,   650,     0,     0,     0,    34,    27,     0,    33,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   423,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   504,   503,     0,     0,   501,
       0,     0,     0,     0,     0,     0,   632,     0,     0,     0,
     656,     0,     0,   666,     0,     0,     0,   647,     0,    29,
      31,    28,    36,   168,   171,   165,   166,   155,   158,     0,
     160,     0,   153,   365,     0,   351,     0,     0,   348,   353,
     362,   359,     0,     0,   371,   375,     0,   411,   223,   412,
     393,   204,   205,   206,   207,     0,     0,     0,     0,   446,
       0,     0,   519,     0,     0,     0,     0,     0,     0,     0,
     409,   402,   416,     0,     0,     0,   461,   190,   449,     0,
     448,   457,   190,     0,     0,     0,     0,     0,     0,   190,
     190,   427,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    50,   509,    48,   510,     0,   190,   513,     0,
       0,     0,   595,   603,     0,   646,     0,     0,   522,   659,
       0,     0,   691,    80,     0,     0,    32,     0,     0,     0,
       0,   350,   355,     0,   354,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   388,     0,     0,     0,     0,
       0,     0,     0,     0,   390,     0,     0,     0,     0,     0,
       0,     0,   450,   451,     0,     0,     0,     0,     0,   467,
       0,     0,   191,   424,   425,   486,     0,     0,   492,     0,
       0,   497,     0,     0,    44,    58,     0,    45,    49,     0,
     507,   502,   511,     0,     0,     0,     0,   604,   600,   644,
       0,     0,     0,     0,     0,     0,     0,     0,   649,   156,
     159,   169,     0,   172,     0,   367,   351,   366,     0,   351,
     363,   359,   358,     0,     0,   380,   381,   376,     0,   368,
     372,     0,   224,   225,   226,   227,   228,   229,   230,   231,
     232,     0,   233,   234,   235,   236,     0,     0,     0,   392,
     413,     0,     0,   550,     0,   550,     0,   520,     0,     0,
       0,     0,     0,   199,   198,   190,   190,     0,   401,     0,
       0,   436,     0,   436,   462,     0,   454,     0,     0,     0,
       0,     0,   190,     0,   190,     0,   190,     0,   190,    48,
       0,    59,     0,     0,   561,   562,   563,   564,     0,   174,
     100,   133,   136,   144,   148,    98,   597,    82,    88,    89,
      93,     0,    85,     0,    92,    85,     0,    85,     0,    85,
       0,    85,     0,    85,    84,     0,   595,   580,   605,   635,
     534,   654,   663,     0,   659,   659,     0,    80,     0,   658,
     523,   378,   678,     0,    81,   679,     0,     0,   167,   170,
     352,   364,   349,   360,     0,   389,     0,   373,   370,     0,
       0,     0,     0,    27,     0,     0,     0,     0,     0,   541,
       0,   544,   391,   547,   548,   404,   403,     0,   428,     0,
       0,     0,     0,     0,    27,     0,   468,     0,   565,     0,
       0,   571,     0,   574,     0,   577,     0,    46,     0,    43,
      68,     0,     0,    53,    71,    55,    66,    67,   556,     0,
       0,     0,     0,    91,     0,     0,   117,     0,     0,   118,
       0,     0,   119,     0,     0,   120,     0,    83,     0,   596,
       0,     0,     0,   660,   661,     0,   662,     0,     0,     0,
       0,     0,   681,   683,   157,     0,   382,   378,   374,   237,
     238,   239,   190,   190,   190,   190,   190,     0,     0,   550,
     190,   190,   549,   550,   554,   515,   202,     0,   436,   197,
     196,   190,   447,     0,   190,   190,   435,   436,   442,   463,
     459,     0,   190,   190,   568,   572,   575,   578,    52,    48,
      71,    60,     0,     0,    70,   190,    96,    85,    94,     0,
      90,    85,    87,   101,     0,    85,    85,    85,   134,     0,
      85,    85,   137,     0,    85,   145,     0,   149,   150,     0,
      79,     0,   652,   641,   635,   635,    80,     0,    80,   634,
       0,     0,     0,   379,   521,   671,   672,   669,   670,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   415,    24,
     410,     0,   552,   551,     0,     0,     0,     0,     0,   440,
       0,     0,   437,   439,     0,     0,     0,     0,     0,    47,
      69,     0,    54,    57,    72,     0,    95,     0,    86,    99,
       0,   121,     0,   122,   123,   132,     0,   124,   125,     0,
     126,     0,     0,   173,   636,   637,     0,   638,     0,   640,
      27,     0,   653,     0,     0,     0,   682,   377,     0,     0,
       0,     0,   190,   553,   555,   190,   515,   515,   514,   203,
     190,   441,     0,   190,   443,   188,   186,   185,   187,   464,
       0,   190,   458,     0,   569,    64,    56,     0,   559,     0,
     102,   103,   104,   105,    85,    85,    85,    85,   138,     0,
     146,   142,   151,   152,     0,    80,     0,     0,   535,   378,
       0,     0,   686,   687,   685,     0,     0,     0,     0,   414,
     518,   516,   517,     0,     0,   438,     0,   460,     0,     0,
      63,    97,     0,     0,     0,     0,   127,   128,   129,   130,
       0,     0,     0,   147,   639,   651,     0,     0,     0,     0,
       0,     0,   243,   214,     0,   209,     0,    80,   220,     0,
     192,   190,     0,   472,    65,     0,    61,   106,   107,   108,
     109,   110,   111,    85,   139,     0,   143,   141,   532,   527,
     528,   529,   530,   531,   378,   525,     0,   533,     0,     0,
     690,   687,   687,   684,     0,   213,     0,     0,   208,     0,
     218,     0,   219,     0,     0,     0,     0,   471,    62,     0,
       0,     0,   131,     0,     0,     0,     0,    27,   689,   688,
     183,   180,   179,   182,   200,   181,   201,   217,   345,   175,
     177,     0,   176,     0,   215,   244,     0,   212,   209,    80,
       0,   222,   220,     0,   189,   190,   478,   476,    80,    80,
       0,   112,   113,   114,   115,   140,     0,   524,   194,   673,
     190,     0,     0,     0,     0,   211,   210,     0,   221,     0,
       0,     0,   473,   475,     0,     0,   135,     0,     0,     0,
       0,     0,     0,   194,   216,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   297,   246,   248,   250,   252,     0,   245,   270,   277,
     279,   281,   283,     0,   276,   293,   184,    80,   479,   378,
     116,   190,   526,   676,    80,     0,   668,   692,     0,     0,
       0,     0,     0,     0,     0,     0,   240,     0,     0,     0,
       0,     0,   242,     0,   474,     0,   195,   675,     0,   190,
     193,   344,   190,   190,   298,   190,     0,   241,   340,   190,
     190,   247,   190,     0,   190,   249,   342,   190,   190,   251,
     190,     0,   190,   253,   190,   190,   271,   190,   190,   278,
     190,   190,   280,   190,   282,   190,   190,   284,   190,   294,
     190,   477,     0,     0,   299,   302,     0,   300,     0,   254,
     261,     0,   258,     0,     0,   260,   262,   269,     0,   266,
       0,     0,   268,   272,   275,     0,   273,   285,     0,   287,
     288,   289,     0,   291,   292,   295,   296,   673,   178,   190,
       0,   190,   190,     0,   190,   190,   190,     0,   190,   190,
     190,   190,   674,   301,   343,   257,     0,   255,     0,   259,
     265,     0,   263,   341,   267,   274,   286,   290,   190,     0,
     190,   256,   339,   264
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1164, -1164, -1164,  -214,  -230,  -185, -1164,  -126,  -148,   297,
   -1164, -1164, -1164, -1164, -1164, -1164,  -226,  -323,  -627,   -44,
    -718,  -622, -1164, -1164, -1164, -1164, -1164, -1164, -1164,  -532,
    -233, -1164, -1164, -1164,  -781, -1164, -1164,  -222,   -42,  1896,
    1075,   -45, -1164,  -678,  -573,  -287, -1164, -1164,  -140, -1164,
   -1164,  -121, -1164, -1164, -1164,  -120,  -282, -1164, -1164,  -754,
   -1164, -1164, -1164, -1164, -1164,  -701, -1164, -1164, -1164, -1164,
    -687, -1164, -1164, -1164,     0, -1164, -1164, -1164, -1164, -1164,
     175, -1164, -1164,  -465, -1164, -1164,  -705, -1164, -1164,  -509,
   -1164, -1164, -1164, -1164,  -928,  1884,  -387, -1163,  -520, -1164,
   -1164, -1164,  -904,  -975,   -47, -1164,  -470, -1164, -1164, -1164,
   -1164,  -472,  -308,   170, -1164, -1164,  -293,  -966,  -339,  -381,
    -929,  -729, -1164,  -833,  -541, -1164, -1164, -1164, -1164,  -529,
   -1164, -1164, -1164, -1164,  -623,  -525, -1164,  -638, -1164, -1146,
   -1164,  -777, -1164,   769,  -398,  -262,   581,  -404,    81,  -183,
    -321,   176, -1164, -1164, -1164,   252, -1164,   -46, -1164,  -146,
   -1164, -1164, -1164, -1164, -1164, -1164,  -797, -1164, -1164, -1164,
   -1164,   663,   665,   666,   667,  -243,   515, -1164, -1164,  -104,
      59, -1164, -1164, -1164, -1164, -1164,  -639, -1164, -1164, -1164,
       7, -1164,   395,   -41, -1164, -1164, -1164,   672, -1164, -1164,
   -1164,  -572, -1164, -1164, -1164,   614,   617,   240,  -144,     4,
     342, -1164, -1164, -1164, -1164, -1164, -1164, -1164,  -356,  -735,
    -897, -1164, -1164,   686,   689, -1164,   262, -1164,  -389, -1164,
   -1164, -1164,  -178, -1164,   697, -1164,  -165, -1164,   698, -1164,
    -163, -1164,   701, -1164,  -160, -1164, -1164,   445, -1164, -1164,
   -1164, -1164, -1164,   809,  -309, -1164, -1164,  -351, -1164, -1164,
    -726, -1164, -1164, -1164,  -776, -1164, -1164,   710, -1164, -1164,
     648, -1164,   649, -1164, -1164,   255,  -559,   266,   267,   269,
     719, -1164, -1164, -1164, -1164, -1164,   720, -1164, -1164, -1164,
   -1164,   728, -1164, -1164,   731, -1164, -1164,   732, -1164, -1164,
     734,  -177,  -294,   145, -1164, -1164, -1164, -1164, -1164, -1164,
   -1164, -1164, -1164, -1164,   863, -1164,   539,  -182, -1164,   -90,
    -192, -1164, -1164,   -96, -1164,    72, -1164, -1164, -1164,  -790,
   -1164, -1164, -1164,   546,    16,   867, -1164, -1164,   547, -1032,
    -511, -1164,  -958,   878, -1164, -1164, -1164,   -51, -1164,  -374,
   -1164,  -209
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   228,   607,   230,   869,   231,   847,
     232,   233,   409,   410,   234,   339,   235,   236,   883,   576,
     493,   577,   494,   680,   879,   578,   793,   962,   579,   794,
     882,  1026,  1027,  1105,   795,   796,   797,   884,   108,   208,
     372,   449,   911,   596,   734,   803,   697,   698,   699,   700,
     701,   702,   703,   894,  1029,   704,   705,   706,   899,   707,
     708,   903,  1039,  1115,  1196,   709,  1082,   710,   906,  1041,
     711,   712,   909,  1044,   479,   342,    41,   133,   238,   417,
     418,   419,   602,   420,   421,   604,   714,   715,  1168,  1169,
    1170,  1171,  1019,  1020,   862,   373,   656,  1172,  1218,   863,
     657,  1173,   857,  1010,   440,   441,  1138,   442,  1135,   443,
     444,  1142,   445,   636,   637,   638,   842,  1095,  1093,  1098,
    1096,  1176,  1266,  1321,  1329,  1267,  1336,  1273,  1339,  1344,
    1274,  1349,  1291,  1314,  1261,  1322,  1323,  1330,  1331,  1324,
    1316,  1174,    42,   245,   344,   524,    44,   345,   246,   135,
     240,   528,   241,   431,   611,   425,   426,   608,   606,   247,
     248,   435,   436,   621,   532,   617,   829,   618,   837,    46,
      47,    48,    49,    50,    51,   446,   137,    52,    53,   249,
     427,   548,    55,   140,   264,   461,   447,   448,   641,   848,
     250,    57,   142,   196,   289,   290,   482,    58,    59,   266,
     267,   771,   268,   269,   270,   251,   252,   450,   865,   925,
     365,    61,   145,   275,   276,   472,   468,   956,   723,   665,
     870,  1021,    62,    63,    64,   281,   476,  1147,  1189,  1190,
    1279,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,   203,    79,   309,   310,   496,
     311,   312,   499,   926,   946,   454,   648,   930,   510,   731,
     724,  1124,  1125,  1126,   725,   726,  1049,    80,    81,    82,
     253,    83,   254,    84,    85,   255,   756,   256,   257,   258,
      86,    87,   155,   315,   316,   688,    88,   283,   284,   285,
     286,    89,   294,   295,    90,   299,   300,    91,   304,   305,
      92,    93,   318,   586,    94,   157,   322,   323,   504,    95,
     175,    96,   176,   177,   927,   211,   212,   821,    98,   179,
     325,   326,   506,   327,   184,   333,   334,   916,   917,   727,
     728,    99,   214,   215,   592,   928,   101,   217,   218,   929,
    1220,   102,   733,   319,   104,   513,   832,   833,  1053,  1091,
     514,  1054
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      40,   229,   336,   109,    60,   338,   335,    56,   469,  1002,
     110,   111,   696,   112,   411,   113,   282,   114,   722,   115,
     522,   328,   424,   116,   330,   117,   521,   118,   317,   119,
     918,   120,   293,   121,   183,   122,   298,   123,   180,   181,
     932,   303,   263,   259,   915,   124,  1012,   682,   280,   826,
     509,   895,   125,   600,   126,   792,   127,   787,   128,    54,
     129,   475,   130,   132,   134,   136,   136,   139,   141,   136,
     144,   136,   141,   136,   141,   136,   141,   136,   141,   154,
     156,    45,   178,   178,   178,   260,   758,   920,   340,  1101,
     921,   773,  1088,  1047,   914,   888,  1156,   892,   907,   502,
     892,   963,  1103,   892,   896,     1,   892,   900,   198,   892,
     200,     1,   412,   457,   320,   321,   851,   329,   897,     7,
     331,   901,   437,     2,   904,   437,  1094,  1313,  1094,  1099,
     508,   864,   438,     4,    12,    15,     5,     5,    15,     5,
     182,   237,   220,   452,  1315,   574,   689,   575,  1332,  1332,
    1164,  1337,  1341,     5,  1346,  1346,   415,  1350,  1311,   302,
     105,   492,  1102,   244,   432,     1,   273,    17,   503,  1007,
    1185,  1178,    24,   307,    33,   695,    10,    33,   324,   239,
     242,    26,   516,   324,   473,    29,   131,    26,   274,   243,
     474,    29,   515,  1389,     5,   288,  1045,   244,   439,   346,
       5,   439,   308,   689,   347,  1311,   349,   182,   613,   351,
     352,   460,  1099,  1031,   466,  1392,   317,   244,  1052,  1006,
     185,    17,  1396,     2,   360,  1166,    10,    31,    32,   362,
     363,   694,  1400,     4,   366,   667,  1401,   367,   987,   719,
     989,    26,  1281,   720,   371,    29,  1070,   913,   375,   475,
     265,  1198,  1087,   378,    19,   961,     5,   959,   381,   880,
    1200,  1285,   287,   384,  1042,   892,  1032,   892,   387,  1034,
     390,   892,   834,   587,   392,   279,  1352,    31,    32,    34,
    1033,   394,   239,  1035,   397,  1036,   948,   614,  1106,  1037,
     941,   716,   406,  -626,   944,   954,   416,  1222,  1223,   242,
     650,   430,   134,   329,   437,   859,   331,   620,   243,    21,
     860,    19,  -628,  1309,  1131,     5,  1164,   292,  1107,     5,
     466,   471,   134,     5,  1148,   106,   107,  1154,     5,   483,
     481,     5,   486,  1160,    36,   489,    34,  1161,  1162,  1163,
     689,     2,   495,     5,   740,   498,  1318,   742,  1311,   720,
     679,     4,   505,   681,  1129,   433,   306,   307,   859,   791,
    1179,   718,   456,   860,   467,   463,  -629,  1084,     5,  1188,
     892,  1108,  1110,   859,   480,  -630,    21,   519,   860,   520,
     439,   297,   314,   672,   525,  1109,  1111,  1112,  1113,   332,
     747,  1166,   642,   644,   533,  1165,   653,   239,   423,   337,
     654,    36,   674,   534,   660,   662,   537,   676,   317,   855,
    1180,   -30,   541,   678,  1183,   671,     5,   692,   806,  1140,
     809,   222,   812,   835,   815,   689,   817,   554,   466,  1284,
     411,   206,   207,   558,   643,   645,     5,   521,  1118,   693,
     694,     5,  1119,  1120,  1121,  1122,   661,   663,  1191,   434,
     689,   411,   693,   694,   871,   692,   681,   843,   844,   845,
    1325,   653,  1192,  1193,  1194,   654,  1342,   522,   593,   452,
     507,     5,   597,     7,   601,   603,   511,   693,   694,   438,
    1419,  1160,  1305,     5,   872,  1161,  1162,  1163,   -51,   639,
     575,     5,   640,   646,   508,   649,   647,   512,   651,   652,
    1123,  1206,   659,     5,   226,   227,  1318,   736,  1311,   664,
    1212,  1213,   147,   670,   149,   413,   151,  1130,   153,   673,
     414,   838,   675,   605,   -59,   677,   423,   610,  1280,   -59,
     -59,   -59,   413,   414,  1160,  1092,   683,   732,  1161,  1162,
    1163,  1164,   574,  1165,     5,  1160,   789,   655,   730,  1161,
    1162,  1163,   178,   689,   690,     5,   690,   691,   692,   791,
    1311,  1167,   878,   910,  1177,   681,   881,     5,  1181,   615,
     616,  1326,  1311,  1187,   221,   222,   746,   223,   224,   225,
     693,   694,   695,  1160,  1268,   713,  1275,  1161,  1162,  1163,
     492,   713,   451,     5,   575,   713,  1165,  1326,  1311,  1304,
     966,   873,   464,   465,   968,  1092,  1307,  1165,   971,   973,
     974,   478,   694,   977,   978,   767,  1166,   980,   485,   691,
     692,   488,   239,   423,   491,   902,   138,   846,   905,   143,
    1224,   146,   501,   148,   408,   150,   788,   152,  1051,   931,
     693,   694,   693,   694,    10,  1165,  1283,   131,   221,   222,
     964,   223,   224,   225,   226,   227,   681,   881,   960,   801,
     886,   804,  1345,  1345,   807,   244,   810,   411,   813,    17,
     816,   221,   222,   818,   223,   224,   225,   226,   227,   277,
     889,   822,   890,  1160,   825,  1030,   827,  1161,  1162,  1163,
    1164,   830,   599,     5,  1116,    31,    32,  1061,  1062,   939,
       5,   836,   689,  1289,   791,   690,   691,   692,  1205,   689,
    1208,  1015,  1186,   691,   692,  1016,  1017,  1018,  1097,   131,
     221,   222,   856,   223,   224,   225,   226,   227,   669,   693,
     694,   695,   984,   985,  1051,  1025,   693,   694,   131,   221,
     823,   824,   223,   224,   225,  1165,   791,  1076,  1077,  1078,
    1079,   839,   840,   841,  1333,   850,   492,  1158,  1159,   893,
    1182,   849,   898,   221,   853,  1166,   223,   224,   225,    43,
     861,   908,  1347,  1340,  1046,   278,   858,   664,   609,   867,
     664,   922,   791,  1320,  1320,  1328,  1328,   743,  1335,   158,
    1320,   159,   160,   161,  1038,  1092,  1040,  1069,   162,  1009,
     887,   681,   891,   885,     5,   891,   271,  1013,   891,   272,
     553,   891,   163,   689,   891,   164,   690,   691,   692,   668,
     713,   950,  1277,   165,   166,  1160,  1152,   167,   923,  1161,
    1162,  1163,  1164,   497,   411,     5,   168,   261,   262,   684,
     693,   694,   695,  1104,   689,   169,   170,   690,   691,   692,
     685,   686,  1391,   687,   171,  1394,     5,   172,   173,  1395,
     174,   819,  1398,    97,   396,   689,  1399,   100,   690,   691,
     692,   693,   694,   695,   400,   988,  1402,   403,   103,   681,
     996,  1132,   721,     0,   732,     0,     0,  1165,     0,     0,
       0,     0,   693,   694,   695,  1114,     0,  1418,     0,     0,
       0,     0,  1420,     0,     0,   856,     3,  1166,     0,     0,
       0,   466,     0,     5,     6,     0,  1009,     0,     0,     5,
       0,     0,   689,     8,     0,   690,   691,   692,   689,     0,
       9,   690,   691,   692,     0,     0,     0,  1043,     0,     0,
     720,     0,    14,     0,     0,  1005,  1048,     0,    16,   693,
     694,   695,     0,     0,     0,   693,   694,   695,     0,    18,
       0,     0,    20,     0,    22,     0,     3,     0,  1195,     0,
     891,     0,   891,     0,     6,    25,   891,    27,  1066,    28,
       0,    30,     0,     8,   466,     0,   713,    35,     0,     0,
       9,     0,     5,    11,     0,     0,     0,  1009,     0,     0,
       0,   689,     0,     0,   690,   691,   692,     0,    16,  1089,
       0,     0,     0,   720,     0,     0,     0,   912,   721,    18,
       0,   856,    20,     0,    22,     0,     0,     0,   693,   694,
     695,     0,     0,     0,     0,    25,     5,    27,     0,    28,
       0,    30,     0,     3,   664,   689,  1128,    35,   690,   691,
     692,     6,  1136,     0,  1139,     0,     0,  1143,     0,     0,
       8,     0,   721,     0,     0,     0,     0,     9,     5,  1009,
       0,     0,   693,   694,   695,   891,     0,   689,     0,    14,
     690,   691,   692,     0,  1155,    16,     0,  1186,     0,     0,
     713,  1175,     0,     0,  1175,     0,    18,     0,  1175,    20,
       0,    22,     0,     0,   693,   694,   695,     0,     0,     0,
     856,     0,    25,     0,    27,     0,    28,     0,    30,     0,
       0,     0,     0,     0,    35,     0,     0,     0,     0,  1201,
     466,  1202,  1390,     0,   713,     0,     0,   713,     5,     0,
    1009,   713,     0,     0,     0,     0,   713,   689,  1214,     0,
     690,   691,   692,     0,     0,     0,   856,   856,     0,   720,
       0,     0,   453,   455,   721,     0,   458,   459,     5,  1278,
       0,     0,     0,     0,   693,   694,   695,   689,  1393,   477,
     690,   691,   692,     0,     0,  1397,   484,     0,     0,   487,
       0,   221,   490,     0,   223,   224,   225,   226,   227,   789,
     500,   790,     0,   713,   693,   694,   695,     0,   186,     0,
     187,   713,   188,   189,     0,   190,     0,   191,   192,   713,
     193,   194,   195,   197,   195,   199,   195,   201,   202,     0,
     204,     0,   205,     0,   221,   222,     0,   223,   224,   225,
     226,   227,     0,  1308,   790,     0,     0,     0,     0,     0,
       0,     0,   209,     0,   210,   213,   216,     0,   219,   622,
     623,   624,   625,   626,   627,   628,   629,   630,   631,   632,
     633,   634,   635,   221,  1358,     0,   223,   224,   225,   226,
     227,  1363,     0,   790,     0,     0,     0,     0,     0,  1370,
    1312,     0,  1319,  1319,  1327,  1327,     0,  1334,  1338,  1319,
    1343,  1343,     0,  1348,   622,   623,   624,   625,   626,   627,
     628,   629,   630,   341,     0,   343,     0,     0,     0,     0,
       0,     0,   348,     0,   350,     0,     0,     0,   353,   354,
       0,     0,     0,     0,   355,   356,   357,   358,     0,   359,
     195,   361,     0,     0,     0,     0,     0,     0,     0,     0,
     364,     0,     0,     0,     0,   368,   369,     0,   370,     0,
       0,     0,     0,     0,   374,     0,     0,     0,   376,   377,
       0,     0,     0,   379,   380,     0,     0,     0,   382,   383,
       0,     0,     0,     0,     0,     0,   388,     0,     0,     0,
     391,     0,     0,   393,     0,     0,     0,     0,     0,     0,
     395,     0,   398,   399,   401,   402,   404,     0,   405,  1262,
    1225,  1226,  1227,  1228,  1229,  1230,  1231,  1263,  1232,  1233,
    1234,  1235,  1236,  1237,  1238,  1239,  1240,  1241,  1242,  1243,
    1244,  1245,  1246,  1247,  1248,  1249,  1250,  1251,  1252,  1253,
    1254,  1255,  1264,  1265,  1256,  1257,  1258,  1259,  1260,  1269,
    1225,  1226,  1227,  1228,  1229,  1230,  1231,  1270,  1232,  1233,
    1234,  1235,  1236,  1237,  1238,  1239,  1240,  1241,  1242,  1243,
    1244,  1245,  1246,  1247,  1248,  1249,  1250,  1251,  1252,  1253,
    1254,  1255,  1271,  1272,  1256,  1257,  1258,  1259,  1260,     0,
       0,     0,   517,   518,     0,     0,     0,     0,     0,   343,
     523,     0,   526,     0,     0,     0,   527,   529,   530,     0,
     531,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   536,     0,     0,   538,   539,     0,   540,     0,
     542,   543,     0,   545,   546,     0,   547,     0,   549,   550,
     551,     0,     0,   364,     0,     0,     0,     0,   556,   557,
       0,     0,   560,   561,     0,     0,     0,     0,     0,   566,
     567,     0,   569,   570,     0,   572,   573,     1,     2,     0,
       0,     0,     0,     3,     0,   583,   584,   585,     4,     0,
       5,     6,   590,     0,   591,     0,   594,     7,   595,     0,
       8,     0,     0,     0,     0,     0,     0,     9,    10,     0,
      11,     0,    12,     0,     0,     0,     0,    13,     0,    14,
       0,     0,    15,     0,     0,    16,     0,     0,     0,     0,
       0,     0,     0,    17,     0,     0,    18,     0,    19,    20,
      21,    22,     0,     0,     0,     0,     0,     0,     0,    23,
      24,     0,    25,    26,    27,     0,    28,    29,    30,    31,
      32,    33,     0,    34,    35,    36,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   737,     0,     0,     0,     0,     0,
       0,   523,     0,     0,   523,     0,   527,     0,   744,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   749,     0,     0,     0,
       0,   750,   751,   752,     0,     0,   753,   754,   755,   757,
     755,     0,     0,   760,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   769,   770,   772,   770,     0,
     774,     0,   775,     0,   777,     0,   779,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   799,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   800,     0,   802,     0,     0,
     805,     0,   808,     0,   811,     0,   814,     0,   814,     0,
       0,   585,     0,     0,   820,     0,     0,     0,     0,   591,
     591,     0,   595,     0,     0,     0,   828,     0,     0,     0,
       0,   831,  1225,  1226,  1227,  1228,  1229,  1230,  1231,     0,
    1232,  1233,  1234,  1235,  1236,  1237,  1238,  1239,  1240,  1241,
    1242,  1243,  1244,  1245,  1246,  1247,  1248,  1249,  1250,  1251,
    1252,  1253,  1254,  1255,     0,     0,  1256,  1257,  1258,  1259,
    1260,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     744,     0,   828,     0,     0,     0,     0,     0,     0,     0,
       0,   937,     0,     0,   755,     0,     0,     0,   755,     0,
     945,     0,   947,   770,     0,     0,     0,     0,     0,   951,
       0,     0,   770,     0,     0,   955,   775,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   814,     0,   967,     0,   814,     0,     0,     0,
     970,   972,   814,     0,     0,   976,   814,     0,   979,   814,
       0,   981,     0,     0,   982,     0,     0,     0,     0,   820,
     820,   986,     0,   595,     0,   990,   991,     0,     0,     0,
       0,     0,     0,     0,     0,   994,   995,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1050,     0,
       0,     0,     0,  1055,  1056,  1057,  1058,     0,     0,     0,
       0,   945,   945,     0,     0,     0,     0,  1064,     0,     0,
       0,     0,   291,     0,   296,     0,   301,     0,  1068,   313,
       0,     0,     0,     0,     0,  1072,  1073,  1074,     0,  1075,
     814,   814,   814,     0,  1080,     0,  1081,     0,     0,     0,
     595,     0,  1086,     0,   828,     0,     0,     0,  1090,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1134,     0,
    1137,     0,   595,  1141,     0,     0,     0,  1145,  1146,     0,
       0,     0,  1149,  1150,     0,  1151,     0,     0,   814,     0,
    1153,     0,   385,   386,     0,   389,     0,     0,     0,   828,
       0,     0,     0,     0,  1157,     0,  1090,  1090,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   407,     0,     0,     0,     0,     0,     0,   422,     0,
     428,   429,     0,     0,     0,     0,     0,     0,     0,  1203,
       0,  1204,     0,  1137,   595,  1207,   462,  1141,  1209,     0,
       0,   470,     0,  1211,   595,     0,  1215,     0,     0,     0,
       0,     0,     0,  1217,  1219,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1288,  1217,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1290,  1292,  1293,  1294,
    1295,     0,   535,  1297,  1298,  1299,  1300,  1301,     0,     0,
    1303,   552,   595,   544,   828,     0,   555,     0,     0,   595,
       0,     0,     0,   562,   563,     0,     0,     0,     0,     0,
       0,     0,   559,     0,     0,     0,     0,     0,   564,   565,
       0,   581,   568,     0,     0,   571,     0,     0,     0,     0,
    1356,     0,   580,     0,     0,   582,     0,  1361,     0,  1364,
     588,     0,   589,     0,     0,  1368,     0,  1371,     0,     0,
       0,   598,  1375,     0,     0,  1378,     0,     0,     0,     0,
    1382,     0,     0,     0,   612,     0,     0,     0,   619,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   658,     0,     0,     0,     0,     0,
       0,   666,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1219,     0,     0,     0,     0,  1406,  1408,     0,
       0,  1411,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   717,     0,     0,     0,     0,     0,   729,     0,
       0,     0,   735,     0,     0,     0,     0,     0,   738,     0,
     739,     0,     0,     0,   741,     0,     0,     0,     0,     0,
     745,     0,     0,     0,     0,     0,     0,   748,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   765,
     766,     0,   759,     0,     0,   761,   762,   763,   764,     0,
       0,     0,     0,     0,     0,   768,   780,     0,   782,     0,
     784,     0,   786,     0,   776,     0,   778,     0,     0,   781,
       0,   783,     0,   785,     0,     0,     0,     0,     0,   798,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   852,     0,   854,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   866,     0,   868,
       0,     0,     0,     0,     0,     0,   874,     0,   875,     0,
     876,     0,   877,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   919,     0,     0,
       0,     0,     0,     0,     0,   924,   933,   934,   935,   936,
     938,     0,     0,     0,   942,   943,     0,     0,     0,     0,
       0,     0,     0,     0,   940,   949,     0,     0,   952,   953,
       0,     0,     0,     0,     0,     0,   957,   958,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   965,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     969,     0,     0,     0,     0,   975,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   983,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   992,     0,
       0,     0,     0,     0,     0,   993,     0,     0,   997,   998,
     999,  1000,  1001,     0,     0,     0,     0,  1003,     0,     0,
    1004,     0,  1008,     0,  1011,     0,     0,     0,     0,     0,
    1014,     0,  1022,  1023,  1024,     0,     0,     0,     0,     0,
       0,  1028,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1059,     0,     0,  1060,
       0,     0,     0,     0,  1063,     0,     0,  1065,     0,     0,
       0,     0,     0,     0,     0,  1067,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1071,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1083,     0,  1085,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1100,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1117,     0,
       0,     0,     0,  1127,     0,  1144,     0,  1133,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1184,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1197,     0,  1199,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1210,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1221,     0,     0,     0,     0,     0,
       0,     0,  1216,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1276,     0,     0,     0,
       0,     0,     0,     0,  1282,     0,  1286,  1287,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1296,     0,     0,  1306,     0,     0,     0,  1302,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1310,     0,  1317,     0,     0,
       0,     0,     0,  1353,     0,     0,  1354,  1355,     0,  1357,
       0,  1351,     0,  1359,  1360,     0,  1362,     0,  1365,     0,
       0,  1366,  1367,     0,  1369,     0,  1372,     0,  1373,  1374,
       0,  1376,  1377,     0,  1379,  1380,     0,  1381,     0,  1383,
    1384,     0,  1385,     0,  1386,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1387,  1388,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1403,     0,  1405,  1407,     0,  1409,  1410,
    1412,     0,  1414,  1415,  1416,  1417,  1404,     0,     0,     0,
       0,     0,     0,  1413,     0,     0,     0,     0,     0,     0,
       0,     0,  1421,     0,  1423,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1422
};

static const yytype_int16 yycheck[] =
{
       0,   186,   228,    45,     0,   235,   220,     0,   364,   937,
      52,    53,   585,    55,   337,    57,   194,    59,   591,    61,
     424,   213,   343,    65,   216,    67,   424,    69,   205,    71,
     820,    73,   197,    75,   130,    77,   199,    79,   128,   129,
     837,   201,   190,   189,   820,    87,   950,   579,   194,   727,
     401,   805,    94,   518,    96,   682,    98,   679,   100,     0,
     102,   369,   104,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,     0,   127,   128,   129,   189,   645,   822,   236,  1064,
     825,   663,  1050,   990,   820,   800,  1128,   802,   816,   393,
     805,   882,  1068,   808,   805,     4,   811,   808,   149,   814,
     151,     4,   338,   356,    74,    75,   755,   213,   805,    24,
     216,   808,     6,     5,   811,     6,  1055,  1290,  1057,  1058,
      25,   770,    16,    15,    39,    49,    17,    17,    49,    17,
      40,   186,   184,    48,  1290,   155,    26,   157,  1294,  1295,
      14,  1297,  1298,    17,  1300,  1301,   341,  1303,    22,    73,
       0,   387,  1066,    56,   347,     4,     3,    60,   394,   945,
    1145,  1137,    77,    51,    88,    55,    35,    88,    78,    61,
      19,    80,   408,    78,   367,    84,   147,    80,    25,    28,
     368,    84,   406,  1356,    17,   195,   986,    56,    82,   241,
      17,    82,   202,    26,   246,    22,   248,    40,   529,   251,
     252,   359,  1141,   967,     9,  1361,   393,    56,   994,   945,
     163,    60,  1368,     5,   265,    89,    35,    86,    87,   271,
     272,    54,  1378,    15,   276,   556,  1382,   279,   916,   590,
     918,    80,  1217,    38,   286,    84,  1027,   820,   290,   557,
     191,  1155,  1049,   295,    65,   882,    17,   879,   300,   791,
    1157,  1219,    23,   305,   982,   970,   967,   972,   310,   970,
     312,   976,   737,   503,   316,   194,  1308,    86,    87,    90,
     967,   323,    61,   970,   326,   972,   858,   530,  1069,   976,
     849,   585,   334,   163,   853,   867,   341,  1201,  1202,    19,
     543,   346,   347,   399,     6,     7,   402,   533,    28,    67,
      12,    65,   163,  1288,  1090,    17,    14,    71,  1072,    17,
       9,   366,   367,    17,  1105,   159,   160,  1124,    17,   376,
     375,    17,   379,     7,    92,   382,    90,    11,    12,    13,
      26,     5,   387,    17,   606,   390,    20,   609,    22,    38,
     576,    15,   397,   579,  1089,   348,    50,    51,     7,   682,
    1137,   587,   355,    12,   364,   361,   163,  1045,    17,  1146,
    1075,  1072,  1073,     7,   374,   163,    67,   419,    12,   421,
      82,    72,    64,   561,   426,  1072,  1073,  1074,  1075,    41,
     620,    89,   538,   539,   436,    69,     7,    61,    62,   150,
      11,    92,   567,   445,   550,   551,   448,   570,   585,   760,
    1139,   150,   454,   573,  1143,   561,    17,    31,   705,  1097,
     707,   149,   709,   744,   711,    26,   713,   469,     9,  1219,
     753,   161,   162,   475,   538,   539,    17,   835,     7,    53,
      54,    17,    11,    12,    13,    14,   550,   551,  1149,    57,
      26,   774,    53,    54,   775,    31,   682,   750,   751,   752,
    1293,     7,  1149,  1150,  1151,    11,  1299,   871,   510,    48,
     398,    17,   514,    24,   519,   520,   404,    53,    54,    16,
    1408,     7,  1279,    17,   777,    11,    12,    13,   155,   536,
     157,    17,   537,   540,    25,   542,   541,    47,   545,   546,
      69,  1179,   549,    17,   154,   155,    20,   597,    22,   554,
    1188,  1189,   117,   560,   119,    32,   121,  1090,   123,   566,
      33,   747,   569,   523,   156,   572,    62,   527,  1215,   161,
     162,   163,    32,    33,     7,     8,   583,    45,    11,    12,
      13,    14,   155,    69,    17,     7,   156,   547,   593,    11,
      12,    13,   597,    26,    29,    17,    29,    30,    31,   882,
      22,  1134,   788,   147,  1137,   791,   792,    17,  1141,    58,
      59,    21,    22,  1146,   148,   149,   618,   151,   152,   153,
      53,    54,    55,     7,  1207,   585,  1209,    11,    12,    13,
     816,   591,   352,    17,   157,   595,    69,    21,    22,  1277,
     887,   779,   362,   363,   891,     8,  1284,    69,   895,   896,
     897,   371,    54,   900,   901,   657,    89,   904,   378,    30,
      31,   381,    61,    62,   384,   810,   111,   753,   813,   114,
    1203,   116,   392,   118,   337,   120,   680,   122,   994,   831,
      53,    54,    53,    54,    35,    69,  1219,   147,   148,   149,
     883,   151,   152,   153,   154,   155,   882,   883,   880,   701,
     800,   703,  1300,  1301,   706,    56,   708,   990,   710,    60,
     712,   148,   149,   715,   151,   152,   153,   154,   155,    70,
     801,   723,   802,     7,   726,   967,   728,    11,    12,    13,
      14,   733,   517,    17,  1081,    86,    87,  1006,  1007,   847,
      17,   746,    26,  1223,  1027,    29,    30,    31,  1178,    26,
    1182,     7,    36,    30,    31,    11,    12,    13,  1057,   147,
     148,   149,   767,   151,   152,   153,   154,   155,   558,    53,
      54,    55,   914,   915,  1090,   961,    53,    54,   147,   148,
     724,   725,   151,   152,   153,    69,  1069,  1034,  1035,  1036,
    1037,   151,   152,   153,  1295,   755,   982,  1131,  1132,   804,
    1141,   754,   807,   148,   757,    89,   151,   152,   153,     0,
     770,   816,  1301,  1298,   988,   194,   769,   822,   526,   772,
     825,   827,  1105,  1292,  1293,  1294,  1295,   611,  1297,   126,
    1299,   126,   126,   126,   979,     8,   981,  1027,   126,   947,
     800,  1027,   802,   799,    17,   805,   192,   951,   808,   192,
     468,   811,   126,    26,   814,   126,    29,    30,    31,   557,
     820,   863,  1211,   126,   126,     7,  1113,   126,   828,    11,
      12,    13,    14,   388,  1157,    17,   126,   189,   189,   584,
      53,    54,    55,  1069,    26,   126,   126,    29,    30,    31,
     584,   584,  1361,   584,   126,  1364,    17,   126,   126,  1368,
     126,   716,  1371,     0,   325,    26,  1375,     0,    29,    30,
      31,    53,    54,    55,   328,   917,  1387,   330,     0,  1105,
     931,  1090,    43,    -1,    45,    -1,    -1,    69,    -1,    -1,
      -1,    -1,    53,    54,    55,  1080,    -1,  1406,    -1,    -1,
      -1,    -1,  1411,    -1,    -1,   950,    10,    89,    -1,    -1,
      -1,     9,    -1,    17,    18,    -1,  1064,    -1,    -1,    17,
      -1,    -1,    26,    27,    -1,    29,    30,    31,    26,    -1,
      34,    29,    30,    31,    -1,    -1,    -1,   982,    -1,    -1,
      38,    -1,    46,    -1,    -1,   945,   991,    -1,    52,    53,
      54,    55,    -1,    -1,    -1,    53,    54,    55,    -1,    63,
      -1,    -1,    66,    -1,    68,    -1,    10,    -1,  1153,    -1,
     970,    -1,   972,    -1,    18,    79,   976,    81,  1020,    83,
      -1,    85,    -1,    27,     9,    -1,   986,    91,    -1,    -1,
      34,    -1,    17,    37,    -1,    -1,    -1,  1145,    -1,    -1,
      -1,    26,    -1,    -1,    29,    30,    31,    -1,    52,  1051,
      -1,    -1,    -1,    38,    -1,    -1,    -1,    42,    43,    63,
      -1,  1066,    66,    -1,    68,    -1,    -1,    -1,    53,    54,
      55,    -1,    -1,    -1,    -1,    79,    17,    81,    -1,    83,
      -1,    85,    -1,    10,  1089,    26,  1088,    91,    29,    30,
      31,    18,  1094,    -1,  1096,    -1,    -1,  1099,    -1,    -1,
      27,    -1,    43,    -1,    -1,    -1,    -1,    34,    17,  1217,
      -1,    -1,    53,    54,    55,  1075,    -1,    26,    -1,    46,
      29,    30,    31,    -1,  1126,    52,    -1,    36,    -1,    -1,
    1090,  1136,    -1,    -1,  1139,    -1,    63,    -1,  1143,    66,
      -1,    68,    -1,    -1,    53,    54,    55,    -1,    -1,    -1,
    1155,    -1,    79,    -1,    81,    -1,    83,    -1,    85,    -1,
      -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,    -1,  1171,
       9,  1173,  1358,    -1,  1134,    -1,    -1,  1137,    17,    -1,
    1288,  1141,    -1,    -1,    -1,    -1,  1146,    26,  1190,    -1,
      29,    30,    31,    -1,    -1,    -1,  1201,  1202,    -1,    38,
      -1,    -1,   353,   354,    43,    -1,   357,   358,    17,  1214,
      -1,    -1,    -1,    -1,    53,    54,    55,    26,  1363,   370,
      29,    30,    31,    -1,    -1,  1370,   377,    -1,    -1,   380,
      -1,   148,   383,    -1,   151,   152,   153,   154,   155,   156,
     391,   158,    -1,  1203,    53,    54,    55,    -1,   133,    -1,
     135,  1211,   137,   138,    -1,   140,    -1,   142,   143,  1219,
     145,   146,   147,   148,   149,   150,   151,   152,   153,    -1,
     155,    -1,   157,    -1,   148,   149,    -1,   151,   152,   153,
     154,   155,    -1,  1285,   158,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   177,    -1,   179,   180,   181,    -1,   183,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   148,  1316,    -1,   151,   152,   153,   154,
     155,  1323,    -1,   158,    -1,    -1,    -1,    -1,    -1,  1331,
    1290,    -1,  1292,  1293,  1294,  1295,    -1,  1297,  1298,  1299,
    1300,  1301,    -1,  1303,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   238,    -1,   240,    -1,    -1,    -1,    -1,
      -1,    -1,   247,    -1,   249,    -1,    -1,    -1,   253,   254,
      -1,    -1,    -1,    -1,   259,   260,   261,   262,    -1,   264,
     265,   266,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     275,    -1,    -1,    -1,    -1,   280,   281,    -1,   283,    -1,
      -1,    -1,    -1,    -1,   289,    -1,    -1,    -1,   293,   294,
      -1,    -1,    -1,   298,   299,    -1,    -1,    -1,   303,   304,
      -1,    -1,    -1,    -1,    -1,    -1,   311,    -1,    -1,    -1,
     315,    -1,    -1,   318,    -1,    -1,    -1,    -1,    -1,    -1,
     325,    -1,   327,   328,   329,   330,   331,    -1,   333,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,    -1,
      -1,    -1,   417,   418,    -1,    -1,    -1,    -1,    -1,   424,
     425,    -1,   427,    -1,    -1,    -1,   431,   432,   433,    -1,
     435,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   447,    -1,    -1,   450,   451,    -1,   453,    -1,
     455,   456,    -1,   458,   459,    -1,   461,    -1,   463,   464,
     465,    -1,    -1,   468,    -1,    -1,    -1,    -1,   473,   474,
      -1,    -1,   477,   478,    -1,    -1,    -1,    -1,    -1,   484,
     485,    -1,   487,   488,    -1,   490,   491,     4,     5,    -1,
      -1,    -1,    -1,    10,    -1,   500,   501,   502,    15,    -1,
      17,    18,   507,    -1,   509,    -1,   511,    24,   513,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    34,    35,    -1,
      37,    -1,    39,    -1,    -1,    -1,    -1,    44,    -1,    46,
      -1,    -1,    49,    -1,    -1,    52,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    -1,    -1,    63,    -1,    65,    66,
      67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,
      77,    -1,    79,    80,    81,    -1,    83,    84,    85,    86,
      87,    88,    -1,    90,    91,    92,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   599,    -1,    -1,    -1,    -1,    -1,
      -1,   606,    -1,    -1,   609,    -1,   611,    -1,   613,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   631,    -1,    -1,    -1,
      -1,   636,   637,   638,    -1,    -1,   641,   642,   643,   644,
     645,    -1,    -1,   648,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   660,   661,   662,   663,    -1,
     665,    -1,   667,    -1,   669,    -1,   671,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   688,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   700,    -1,   702,    -1,    -1,
     705,    -1,   707,    -1,   709,    -1,   711,    -1,   713,    -1,
      -1,   716,    -1,    -1,   719,    -1,    -1,    -1,    -1,   724,
     725,    -1,   727,    -1,    -1,    -1,   731,    -1,    -1,    -1,
      -1,   736,   108,   109,   110,   111,   112,   113,   114,    -1,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,    -1,    -1,   142,   143,   144,   145,
     146,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     835,    -1,   837,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   846,    -1,    -1,   849,    -1,    -1,    -1,   853,    -1,
     855,    -1,   857,   858,    -1,    -1,    -1,    -1,    -1,   864,
      -1,    -1,   867,    -1,    -1,   870,   871,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   887,    -1,   889,    -1,   891,    -1,    -1,    -1,
     895,   896,   897,    -1,    -1,   900,   901,    -1,   903,   904,
      -1,   906,    -1,    -1,   909,    -1,    -1,    -1,    -1,   914,
     915,   916,    -1,   918,    -1,   920,   921,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   930,   931,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   993,    -1,
      -1,    -1,    -1,   998,   999,  1000,  1001,    -1,    -1,    -1,
      -1,  1006,  1007,    -1,    -1,    -1,    -1,  1012,    -1,    -1,
      -1,    -1,   196,    -1,   198,    -1,   200,    -1,  1023,   203,
      -1,    -1,    -1,    -1,    -1,  1030,  1031,  1032,    -1,  1034,
    1035,  1036,  1037,    -1,  1039,    -1,  1041,    -1,    -1,    -1,
    1045,    -1,  1047,    -1,  1049,    -1,    -1,    -1,  1053,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1093,    -1,
    1095,    -1,  1097,  1098,    -1,    -1,    -1,  1102,  1103,    -1,
      -1,    -1,  1107,  1108,    -1,  1110,    -1,    -1,  1113,    -1,
    1115,    -1,   308,   309,    -1,   311,    -1,    -1,    -1,  1124,
      -1,    -1,    -1,    -1,  1129,    -1,  1131,  1132,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   335,    -1,    -1,    -1,    -1,    -1,    -1,   342,    -1,
     344,   345,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1174,
      -1,  1176,    -1,  1178,  1179,  1180,   360,  1182,  1183,    -1,
      -1,   365,    -1,  1188,  1189,    -1,  1191,    -1,    -1,    -1,
      -1,    -1,    -1,  1198,  1199,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1222,  1223,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1261,  1262,  1263,  1264,
    1265,    -1,   446,  1268,  1269,  1270,  1271,  1272,    -1,    -1,
    1275,   467,  1277,   457,  1279,    -1,   472,    -1,    -1,  1284,
      -1,    -1,    -1,   479,   480,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   476,    -1,    -1,    -1,    -1,    -1,   482,   483,
      -1,   497,   486,    -1,    -1,   489,    -1,    -1,    -1,    -1,
    1315,    -1,   496,    -1,    -1,   499,    -1,  1322,    -1,  1324,
     504,    -1,   506,    -1,    -1,  1330,    -1,  1332,    -1,    -1,
      -1,   515,  1337,    -1,    -1,  1340,    -1,    -1,    -1,    -1,
    1345,    -1,    -1,    -1,   528,    -1,    -1,    -1,   532,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   548,    -1,    -1,    -1,    -1,    -1,
      -1,   555,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1387,    -1,    -1,    -1,    -1,  1392,  1393,    -1,
      -1,  1396,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   586,    -1,    -1,    -1,    -1,    -1,   592,    -1,
      -1,    -1,   596,    -1,    -1,    -1,    -1,    -1,   602,    -1,
     604,    -1,    -1,    -1,   608,    -1,    -1,    -1,    -1,    -1,
     614,    -1,    -1,    -1,    -1,    -1,    -1,   621,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   655,
     656,    -1,   646,    -1,    -1,   649,   650,   651,   652,    -1,
      -1,    -1,    -1,    -1,    -1,   659,   672,    -1,   674,    -1,
     676,    -1,   678,    -1,   668,    -1,   670,    -1,    -1,   673,
      -1,   675,    -1,   677,    -1,    -1,    -1,    -1,    -1,   683,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   756,    -1,   758,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   771,    -1,   773,
      -1,    -1,    -1,    -1,    -1,    -1,   780,    -1,   782,    -1,
     784,    -1,   786,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   821,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   829,   842,   843,   844,   845,
     846,    -1,    -1,    -1,   850,   851,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   848,   861,    -1,    -1,   864,   865,
      -1,    -1,    -1,    -1,    -1,    -1,   872,   873,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   885,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     894,    -1,    -1,    -1,    -1,   899,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   911,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   922,    -1,
      -1,    -1,    -1,    -1,    -1,   929,    -1,    -1,   932,   933,
     934,   935,   936,    -1,    -1,    -1,    -1,   941,    -1,    -1,
     944,    -1,   946,    -1,   948,    -1,    -1,    -1,    -1,    -1,
     954,    -1,   956,   957,   958,    -1,    -1,    -1,    -1,    -1,
      -1,   965,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1002,    -1,    -1,  1005,
      -1,    -1,    -1,    -1,  1010,    -1,    -1,  1013,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1021,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1029,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1044,    -1,  1046,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1063,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1082,    -1,
      -1,    -1,    -1,  1087,    -1,  1101,    -1,  1091,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1144,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1154,    -1,  1156,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1185,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1200,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1196,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1210,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1218,    -1,  1220,  1221,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1266,    -1,    -1,  1281,    -1,    -1,    -1,  1273,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1289,    -1,  1291,    -1,    -1,
      -1,    -1,    -1,  1309,    -1,    -1,  1312,  1313,    -1,  1315,
      -1,  1305,    -1,  1319,  1320,    -1,  1322,    -1,  1324,    -1,
      -1,  1327,  1328,    -1,  1330,    -1,  1332,    -1,  1334,  1335,
      -1,  1337,  1338,    -1,  1340,  1341,    -1,  1343,    -1,  1345,
    1346,    -1,  1348,    -1,  1350,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1352,  1353,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1389,    -1,  1391,  1392,    -1,  1394,  1395,
    1396,    -1,  1398,  1399,  1400,  1401,  1390,    -1,    -1,    -1,
      -1,    -1,    -1,  1397,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1418,    -1,  1420,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1419
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
     337,   338,   341,   342,   344,   346,   354,   355,   361,   362,
     373,   375,   386,   387,   388,   395,   396,   397,   398,   399,
     400,   401,   402,   403,   404,   405,   406,   407,   408,   410,
     431,   432,   433,   435,   437,   438,   444,   445,   450,   455,
     458,   461,   464,   465,   468,   473,   475,   478,   482,   495,
     499,   500,   505,   507,   508,     0,   159,   160,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   147,   205,   241,   205,   313,   205,   340,   340,   205,
     347,   205,   356,   340,   205,   376,   340,   356,   340,   356,
     340,   356,   340,   356,   205,   446,   205,   469,   335,   336,
     337,   338,   361,   387,   388,   398,   402,   406,   431,   444,
     450,   455,   458,   461,   464,   474,   476,   477,   205,   483,
     483,   483,    40,   487,   488,   163,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   357,   204,   357,   204,
     357,   204,   204,   409,   204,   204,   161,   162,   203,   204,
     204,   479,   480,   204,   496,   497,   204,   501,   502,   204,
     202,   148,   149,   151,   152,   153,   154,   155,   168,   169,
     170,   172,   174,   175,   178,   180,   181,   205,   242,    61,
     314,   316,    19,    28,    56,   307,   312,   323,   324,   343,
     354,   369,   370,   434,   436,   439,   441,   442,   443,   323,
     343,   434,   436,   172,   348,   344,   363,   364,   366,   367,
     368,   369,   370,     3,    25,   377,   378,    70,   310,   312,
     323,   389,   396,   451,   452,   453,   454,    23,   238,   358,
     359,   203,    71,   400,   456,   457,   203,    72,   404,   459,
     460,   203,    73,   408,   462,   463,    50,    51,   238,   411,
     412,   414,   415,   203,    64,   447,   448,   465,   466,   507,
      74,    75,   470,   471,    78,   484,   485,   487,   484,   487,
     484,   487,    41,   489,   490,   167,   180,   150,   168,   179,
     172,   204,   239,   204,   308,   311,   202,   202,   204,   202,
     204,   202,   202,   204,   204,   204,   204,   204,   204,   204,
     357,   204,   202,   202,   204,   374,   202,   202,   204,   204,
     204,   202,   204,   259,   204,   202,   204,   204,   202,   204,
     204,   202,   204,   204,   202,   259,   259,   202,   204,   259,
     202,   204,   202,   204,   202,   204,   480,   202,   204,   204,
     497,   204,   204,   502,   204,   204,   202,   203,   173,   176,
     177,   181,   180,    32,    33,   169,   205,   243,   244,   245,
     247,   248,   203,    62,   314,   319,   320,   344,   203,   203,
     205,   317,   313,   354,    57,   325,   326,     6,    16,    82,
     268,   269,   271,   273,   274,   276,   339,   350,   351,   205,
     371,   371,    48,   417,   419,   417,   354,   339,   417,   417,
     172,   349,   203,   373,   371,   371,     9,   238,   380,   382,
     203,   205,   379,   313,   396,   276,   390,   417,   371,   238,
     238,   205,   360,   268,   417,   371,   268,   417,   371,   268,
     417,   371,   180,   184,   186,   205,   413,   411,   205,   416,
     417,   371,   466,   180,   472,   205,   486,   489,    25,   421,
     422,   489,    47,   509,   514,   167,   180,   204,   204,   202,
     202,   308,   311,   204,   309,   202,   204,   204,   315,   204,
     204,   204,   328,   202,   202,   203,   204,   202,   204,   204,
     204,   202,   204,   204,   203,   204,   204,   204,   345,   204,
     204,   204,   259,   374,   202,   259,   204,   204,   202,   203,
     204,   204,   259,   259,   203,   203,   204,   204,   203,   204,
     204,   203,   204,   204,   155,   157,   183,   185,   189,   192,
     203,   259,   203,   204,   204,   204,   467,   168,   203,   203,
     204,   204,   498,   202,   204,   204,   207,   202,   203,   244,
     247,   205,   246,   205,   249,   238,   322,   169,   321,   319,
     238,   318,   203,   314,   339,    58,    59,   329,   331,   203,
     180,   327,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   277,   278,   279,   268,
     205,   352,   323,   343,   323,   343,   268,   205,   420,   268,
     339,   268,   268,     7,    11,   238,   260,   264,   203,   268,
     323,   343,   323,   343,   205,   383,   203,   314,   390,   277,
     268,   323,   396,   268,   400,   268,   404,   268,   408,   180,
     187,   180,   193,   268,   439,   441,   442,   443,   449,    26,
      29,    30,    31,    53,    54,    55,   208,   210,   211,   212,
     213,   214,   215,   216,   219,   220,   221,   223,   224,   229,
     231,   234,   235,   238,   250,   251,   466,   203,   180,   421,
      38,    43,   208,   382,   424,   428,   429,   493,   494,   203,
     205,   423,    45,   506,   208,   203,   483,   204,   203,   203,
     309,   203,   309,   315,   204,   203,   202,   168,   203,   204,
     204,   204,   204,   204,   204,   204,   440,   204,   440,   203,
     204,   203,   203,   203,   203,   259,   259,   202,   203,   204,
     204,   365,   204,   365,   204,   204,   203,   204,   203,   204,
     259,   203,   259,   203,   259,   203,   259,   185,   183,   156,
     158,   181,   182,   190,   193,   198,   199,   200,   203,   204,
     204,   202,   204,   209,   202,   204,   209,   202,   204,   209,
     202,   204,   209,   202,   204,   209,   202,   209,   202,   467,
     204,   481,   202,   498,   498,   202,   207,   202,   204,   330,
     202,   204,   510,   511,   247,   314,   205,   332,   180,   151,
     152,   153,   280,   280,   280,   280,   171,   173,   353,   354,
     238,   350,   203,   354,   203,   421,   205,   266,   354,     7,
      12,   238,   258,   263,   350,   372,   203,   354,   203,   171,
     384,   314,   280,   396,   203,   203,   203,   203,   180,   188,
     193,   180,   194,   182,   201,   373,   212,   238,   250,   215,
     219,   238,   250,   205,   217,   223,   229,   234,   205,   222,
     229,   234,   169,   225,   234,   169,   232,   184,   205,   236,
     147,   206,    42,   208,   424,   428,   491,   492,   493,   203,
     383,   383,   321,   238,   203,   373,   417,   478,   499,   503,
     421,   484,   330,   259,   259,   259,   259,   204,   259,   172,
     203,   440,   259,   259,   440,   204,   418,   204,   365,   259,
     202,   204,   259,   259,   365,   204,   381,   259,   259,   185,
     201,   182,   191,   198,   194,   259,   209,   204,   209,   203,
     204,   209,   204,   209,   209,   203,   204,   209,   209,   204,
     209,   204,   204,   203,   481,   481,   204,   207,   202,   207,
     204,   204,   203,   203,   204,   204,   511,   203,   203,   203,
     203,   203,   258,   203,   203,   238,   424,   428,   203,   172,
     267,   203,   266,   372,   203,     7,    11,    12,    13,   256,
     257,   385,   203,   203,   203,   180,   195,   196,   203,   218,
     220,   223,   229,   234,   229,   234,   234,   234,   169,   226,
     169,   233,   184,   205,   237,   493,   167,   384,   205,   430,
     204,   382,   428,   512,   515,   204,   204,   204,   204,   259,
     259,   418,   418,   259,   204,   259,   202,   259,   204,   168,
     198,   203,   204,   204,   204,   204,   209,   209,   209,   209,
     204,   204,   230,   203,   207,   203,   204,   330,   506,   202,
     204,   513,     8,   282,   284,   281,   284,   282,   283,   284,
     203,   267,   266,   281,   180,   197,   198,   223,   229,   234,
     229,   234,   234,   234,   169,   227,   260,   203,     7,    11,
      12,    13,    14,    69,   425,   426,   427,   203,   202,   383,
     208,   428,   515,   203,   204,   272,   202,   204,   270,   202,
     207,   204,   275,   202,   259,   204,   204,   391,   198,   204,
     204,   204,   209,   204,   330,   202,   503,   204,   513,   513,
       7,    11,    12,    13,    14,    69,    89,   208,   252,   253,
     254,   255,   261,   265,   305,   205,   285,   208,   281,   305,
     285,   208,   283,   285,   203,   267,    36,   208,   305,   392,
     393,   229,   234,   234,   234,   169,   228,   203,   266,   203,
     384,   202,   202,   204,   204,   270,   207,   204,   275,   204,
     259,   204,   207,   207,   202,   204,   203,   204,   262,   204,
     504,   259,   266,   266,   208,   108,   109,   110,   111,   112,
     113,   114,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   142,   143,   144,   145,
     146,   298,   107,   115,   140,   141,   286,   289,   298,   107,
     115,   140,   141,   291,   294,   298,   203,   392,   205,   394,
     234,   267,   203,   208,   493,   506,   203,   203,   204,   262,
     204,   296,   204,   204,   204,   204,   203,   204,   204,   204,
     204,   204,   203,   204,   207,   330,   259,   207,   202,   267,
     203,    22,   238,   261,   297,   303,   304,   203,    20,   238,
     253,   287,   299,   300,   303,   287,    21,   238,   253,   288,
     301,   302,   303,   288,   238,   253,   290,   303,   238,   292,
     299,   303,   287,   238,   293,   301,   303,   293,   238,   295,
     303,   203,   503,   259,   259,   259,   204,   259,   202,   259,
     259,   204,   259,   202,   204,   259,   259,   259,   204,   259,
     202,   204,   259,   259,   259,   204,   259,   259,   204,   259,
     259,   259,   204,   259,   259,   259,   259,   203,   203,   261,
     180,   253,   303,   169,   253,   253,   303,   169,   253,   253,
     303,   303,   504,   259,   203,   259,   204,   259,   204,   259,
     259,   204,   259,   203,   259,   259,   259,   259,   253,   258,
     253,   259,   203,   259
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
     350,   351,   351,   352,   353,   353,   354,   355,   355,   355,
     356,   357,   357,   357,   357,   358,   359,   360,   361,   362,
     362,   363,   363,   363,   363,   364,   365,   365,   365,   365,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     374,   374,   375,   376,   377,   378,   378,   379,   380,   381,
     381,   382,   383,   384,   385,   386,   386,   387,   388,   389,
     389,   390,   391,   391,   391,   391,   391,   392,   393,   394,
     395,   395,   396,   397,   397,   397,   398,   399,   399,   400,
     401,   401,   402,   403,   404,   405,   405,   406,   407,   408,
     409,   409,   409,   409,   409,   410,   410,   411,   412,   413,
     413,   414,   415,   416,   417,   418,   418,   418,   418,   419,
     420,   421,   422,   423,   424,   425,   426,   427,   427,   427,
     427,   427,   427,   428,   429,   430,   431,   431,   431,   432,
     432,   433,   434,   434,   435,   436,   436,   437,   438,   439,
     440,   440,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   449,   449,   449,   450,   451,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   461,   462,   463,
     464,   465,   465,   465,   465,   465,   465,   465,   465,   465,
     465,   465,   465,   466,   466,   467,   467,   467,   468,   469,
     470,   471,   471,   472,   472,   472,   473,   474,   474,   475,
     476,   476,   476,   476,   476,   476,   476,   476,   476,   476,
     476,   476,   476,   476,   477,   477,   477,   477,   477,   477,
     477,   478,   479,   479,   480,   481,   481,   481,   481,   481,
     481,   481,   482,   483,   484,   485,   486,   487,   488,   489,
     490,   491,   492,   493,   494,   495,   496,   496,   497,   498,
     498,   498,   498,   498,   499,   500,   501,   501,   502,   503,
     503,   503,   503,   504,   504,   504,   504,   505,   506,   507,
     508,   509,   510,   510,   511,   512,   512,   513,   513,   513,
     513,   514,   515
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
       6,     1,     1,     1,     4,     2,     7,     1,     1,     1,
       1,     0,     2,     3,     5,     4,     1,     1,    10,     1,
       1,     1,     1,     1,     1,     7,     0,     3,     5,     3,
       3,     9,     7,     9,     1,     1,     1,     1,     7,     0,
       3,     3,     1,     1,     5,     1,     1,     1,     7,     0,
       3,     1,     1,     1,     1,     1,     1,     8,    10,     1,
       1,    10,     0,     3,     5,     3,     2,     5,     1,     1,
       1,     1,     5,     1,     1,     1,     8,     1,     1,     5,
       1,     1,     8,     1,     5,     1,     1,     8,     1,     5,
       0,     3,     5,     3,     3,     1,     1,     4,     1,     1,
       1,     4,     1,     1,     7,     0,     3,     3,     3,     1,
       1,     5,     1,     1,     9,     1,     5,     1,     1,     1,
       1,     1,     1,     7,     1,     1,     1,     1,     1,     1,
       1,    10,     1,     1,    10,     1,     1,    10,    10,     7,
       0,     3,     3,     9,     7,     9,    10,     1,     1,     9,
       1,     1,     1,     1,     1,    10,     1,     1,     7,     9,
       1,    10,     7,     1,    10,     7,     1,    10,     7,     1,
       9,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     3,     2,     1,     1,
       4,     1,     1,     1,     2,     3,     4,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     3,     1,     8,     0,     3,     3,     3,     5,
       3,     2,     1,     1,     4,     1,     1,     4,     1,     4,
       1,     4,     1,     4,     1,     4,     3,     1,     6,     0,
       3,     3,     3,     2,     1,     4,     3,     1,    16,     1,
       1,     1,     1,     0,     6,     3,     2,     1,     1,     9,
       1,     4,     3,     1,     6,     1,     1,     0,     3,     3,
       2,     1,     7
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
