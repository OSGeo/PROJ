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
#define YYFINAL  104
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3205

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  163
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  343
/* YYNRULES -- Number of rules.  */
#define YYNRULES  689
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1389

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
     216,   216,   217,   217,   217,   218,   218,   218,   219,   219,
     219,   223,   227,   227,   229,   231,   233,   233,   235,   235,
     237,   239,   241,   243,   245,   245,   247,   247,   249,   249,
     249,   249,   251,   251,   255,   257,   261,   262,   263,   265,
     265,   267,   269,   271,   273,   277,   278,   281,   282,   284,
     286,   288,   291,   292,   293,   295,   297,   299,   299,   301,
     304,   305,   307,   307,   312,   312,   314,   314,   316,   318,
     320,   324,   325,   328,   329,   330,   332,   332,   333,   336,
     337,   341,   342,   343,   347,   348,   349,   350,   352,   356,
     358,   361,   363,   366,   367,   368,   369,   370,   371,   372,
     373,   374,   375,   376,   377,   378,   379,   380,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   401,   403,   405,   409,   414,   416,   418,
     420,   422,   426,   431,   432,   434,   436,   438,   442,   446,
     448,   448,   450,   450,   455,   460,   461,   462,   463,   464,
     465,   466,   468,   470,   472,   472,   474,   474,   476,   478,
     480,   482,   484,   486,   490,   492,   496,   496,   499,   502,
     507,   507,   507,   507,   507,   510,   515,   515,   515,   515,
     518,   522,   523,   525,   541,   545,   546,   548,   548,   550,
     550,   556,   556,   558,   560,   567,   567,   567,   569,   576,
     577,   578,   579,   581,   588,   595,   596,   597,   599,   601,
     601,   601,   601,   601,   601,   601,   601,   601,   604,   604,
     604,   606,   606,   608,   608,   608,   610,   615,   621,   626,
     629,   632,   633,   634,   635,   636,   637,   638,   639,   640,
     643,   644,   645,   646,   647,   648,   649,   650,   653,   654,
     655,   656,   657,   658,   659,   660,   663,   664,   667,   668,
     669,   670,   675,   676,   677,   678,   679,   680,   681,   682,
     683,   686,   687,   688,   689,   692,   693,   694,   695,   698,
     699,   702,   703,   708,   709,   712,   713,   714,   715,   718,
     719,   720,   721,   722,   723,   724,   725,   726,   727,   728,
     729,   730,   731,   732,   733,   734,   735,   736,   737,   738,
     739,   740,   741,   742,   743,   744,   745,   746,   747,   748,
     749,   750,   751,   752,   753,   755,   758,   760,   762,   764,
     766,   768,   784,   784,   786,   794,   795,   797,   798,   800,
     808,   809,   811,   813,   815,   820,   821,   823,   825,   827,
     829,   831,   833,   835,   840,   844,   846,   849,   852,   853,
     854,   856,   857,   859,   864,   865,   867,   867,   869,   873,
     873,   873,   875,   875,   877,   885,   894,   902,   912,   913,
     915,   917,   917,   919,   919,   922,   923,   927,   933,   934,
     935,   937,   937,   939,   941,   943,   947,   952,   952,   954,
     957,   958,   962,   967,   967,   967,   969,   971,   972,   973,
     974,   976,   979,   981,   985,   991,   991,   995,   995,   996,
     996,   998,  1003,  1004,  1005,  1006,  1008,  1014,  1019,  1025,
    1027,  1029,  1031,  1035,  1043,  1044,  1046,  1048,  1050,  1054,
    1054,  1056,  1058,  1063,  1064,  1066,  1068,  1070,  1072,  1076,
    1076,  1078,  1084,  1091,  1091,  1094,  1101,  1102,  1103,  1104,
    1105,  1107,  1111,  1113,  1115,  1115,  1119,  1124,  1124,  1124,
    1128,  1133,  1133,  1135,  1139,  1139,  1143,  1148,  1150,  1154,
    1154,  1158,  1163,  1165,  1169,  1170,  1171,  1172,  1173,  1175,
    1175,  1177,  1180,  1182,  1182,  1184,  1186,  1188,  1192,  1199,
    1199,  1201,  1202,  1203,  1204,  1206,  1208,  1212,  1217,  1219,
    1222,  1227,  1231,  1237,  1237,  1237,  1237,  1237,  1237,  1241,
    1246,  1248,  1253,  1253,  1254,  1256,  1256,  1259,  1266,  1266,
    1268,  1275,  1275,  1277,  1284,  1293,  1299,  1301,  1303,  1308,
    1313,  1319,  1319,  1321,  1324,  1328,  1333,  1339,  1342,  1347,
    1353,  1356,  1361,  1367,  1370,  1375,  1380,  1381,  1384,  1385,
    1386,  1388,  1390,  1390,  1392,  1392,  1394,  1395,  1396,  1398,
    1398,  1400,  1401,  1402,  1403,  1405,  1407,  1409,  1411,  1413,
    1413,  1413,  1414,  1414,  1414,  1416,  1416,  1416,  1416,  1418,
    1420,  1422,  1426,  1429,  1429,  1432,  1433,  1434,  1436,  1440,
    1441,  1443,  1445,  1445,  1446,  1446,  1447,  1447,  1447,  1448,
    1449,  1449,  1450,  1450,  1451,  1451,  1453,  1453,  1454,  1454,
    1455,  1456,  1456,  1460,  1467,  1468,  1469,  1470,  1471,  1472,
    1473,  1475,  1477,  1479,  1481,  1483,  1485,  1487,  1489,  1491,
    1493,  1498,  1505,  1506,  1507,  1508,  1509,  1511,  1516,  1524,
    1524,  1524,  1524,  1526,  1527,  1528,  1529,  1531,  1533,  1538,
    1544,  1546,  1553,  1553,  1555,  1556,  1557,  1558,  1560,  1562
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
  "parameter_list_identifier_list", "map_projection_keyword",
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
  "parameter_or_parameter_file", "opt_separator_deriving_conversion_end",
  "deriving_conversion_keyword", "deriving_conversion_name",
  "operation_method", "operation_method_keyword", "operation_method_name",
  "operation_parameter", "parameter_unit",
  "length_or_angle_or_scale_or_time_or_parametric_unit",
  "length_or_angle_or_scale_or_time_or_parametric_unit_keyword",
  "operation_parameter_file", "parameter_file_keyword",
  "parameter_file_name", "derived_geodetic_crs", "derived_geographic_crs",
  "derived_static_geodetic_crs",
  "base_static_geodetic_crs_or_base_static_geographic_crs",
  "derived_dynamic_geodetic_crs",
  "base_dynamic_geodetic_crs_or_base_dynamic_geographic_crs",
  "derived_static_geographic_crs", "derived_dynamic_geographic_crs",
  "derived_projected_crs", "derived_projected_crs_keyword",
  "derived_crs_name", "base_projected_crs", "base_projected_crs_keyword",
  "derived_vertical_crs", "base_vertical_crs", "base_static_vertical_crs",
  "base_dynamic_vertical_crs", "base_vertical_crs_keyword",
  "derived_engineering_crs", "base_engineering_crs",
  "base_engineering_crs_keyword", "derived_parametric_crs",
  "base_parametric_crs", "base_parametric_crs_keyword",
  "derived_temporal_crs", "base_temporal_crs", "base_temporal_crs_keyword",
  "compound_crs", "compound_crs_choice",
  "compound_crs_choice_after_horizontal_crs",
  "vertical_crs_or_derived_vertical_crs",
  "temporal_crs_or_derived_temporal_crs",
  "opt_temporal_crs_or_derived_temporal_crs_list",
  "parametric_crs_or_derived_parametric_crs",
  "opt_parametric_crs_or_derived_parametric_crs_list_then_opt_temporal_crs_or_derived_temporal_crs_list",
  "spatio_temporal_ccrs_no_horizontal_crs", "spatio_temporal_ccrs_first",
  "compound_crs_keyword", "compound_crs_name", "horizontal_crs",
  "geographic2D_crs", "derived_geographic2D_crs",
  "metadata_coordinate_epoch", "coordinate_epoch_keyword",
  "coordinate_epoch", "coordinate_metadata", "coordinate_metadata_crs",
  "coordinate_metadata_keyword", "static_crs_coordinate_metadata",
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

#define YYPACT_NINF -1146

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-1146)))

#define YYTABLE_NINF -643

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     819, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146,
   -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146,
   -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146,
   -1146, -1146, -1146, -1146, -1146, -1146, -1146,   132, -1146, -1146,
   -1146,   156, -1146, -1146, -1146,   156, -1146, -1146, -1146, -1146,
   -1146, -1146,   156,   156, -1146,   156, -1146,   156, -1146,   156,
   -1146,   156, -1146, -1146, -1146,   156, -1146,   156, -1146,   156,
   -1146,   156, -1146,   156, -1146,   156, -1146,   156, -1146,   156,
   -1146, -1146, -1146, -1146, -1146, -1146, -1146,   156, -1146, -1146,
   -1146, -1146, -1146,   156, -1146,   156, -1146,   156, -1146,   156,
   -1146,   156, -1146,   156, -1146, -1146, -1146,    20,    20,    20,
      20,    20,    20,    20,    20,    20,    20,    20,    20,    20,
      20,    20,    20,    20,    20,   626,    20,    20,    20,   137,
   -1146, -1146,    72, -1146,    72, -1146,    72,    72, -1146,    72,
   -1146,    72,    72, -1146,    72,    72,    72,    72,    72,    72,
      72,    72,    72, -1146,    72, -1146,    72, -1146, -1146, -1146,
   -1146,    86, -1146, -1146, -1146, -1146, -1146,   159,   216,   245,
   -1146, -1146, -1146, -1146,   321, -1146,    72, -1146,    72,    72,
      72, -1146,    72,   156, -1146,  1254,   220,   427,   427,   507,
     357,   320,   125,   297,   138,   321,   161,   321,   448,   321,
     328,   119,   321,   353,  1225, -1146, -1146, -1146,   470,   137,
     137,   137,   381,   626, -1146, -1146, -1146, -1146, -1146, -1146,
   -1146,   592, -1146, -1146, -1146, -1146,   277,   298,   324,   507,
   -1146,    72, -1146,    72,   156, -1146, -1146, -1146, -1146,   156,
      72,   156,    72, -1146, -1146, -1146, -1146, -1146,   156,   156,
      72,    72,    72,    72,    72,    72, -1146,    72,    72,    72,
   -1146, -1146, -1146, -1146, -1146, -1146,    72,   156, -1146, -1146,
     156,    72,    72, -1146,    72, -1146, -1146,   156, -1146,    72,
      72,   156, -1146, -1146,    72,    72,   156, -1146, -1146,    72,
      72,   156, -1146, -1146,    72,    72,   156, -1146, -1146,    72,
      72,   156,    72,   156, -1146, -1146,    72,   156, -1146, -1146,
   -1146, -1146, -1146,   156, -1146, -1146, -1146, -1146, -1146, -1146,
     321, -1146,    72,    72, -1146, -1146, -1146, -1146, -1146, -1146,
     156,    72,    72,    72, -1146,    72,   156,   321, -1146,   348,
     592, -1146, -1146,   480,   321,   241,   321,   321,    20,    20,
     100,   411,   104,    20,    20,   460,   460,   100,   104,   460,
     460,   507,   321,   487,   520,    20,    20,   102,   534,   460,
      20,   550, -1146,   550,    20,   534,   460,    20,   534,   460,
      20,   534,   460,    20, -1146, -1146,   496,   113, -1146,    20,
     460,    20,    20, -1146,   528,   276,   592,   381,   562,   381,
     552,   626, -1146,   592, -1146, -1146, -1146, -1146, -1146, -1146,
   -1146, -1146,    72,    72,   156, -1146,   156, -1146, -1146,    72,
      72,   156,    72, -1146, -1146, -1146,    72,    72,    72, -1146,
      72,   156, -1146, -1146, -1146, -1146, -1146, -1146, -1146,   156,
     321,    72,   156, -1146,    72,    72, -1146,    72,   156,    72,
      72,   321,    72,    72, -1146,    72, -1146,    72, -1146,   321,
      72,   156, -1146,    72,    72,    72,   156,   321,    72,    72,
      72,    72, -1146,   321,   321,    72,    72,   321,    72,    72,
     321,    72,    72, -1146, -1146,   212, -1146,   321,    72, -1146,
     321,    72,    72,    72, -1146, -1146,    72, -1146, -1146, -1146,
   -1146, -1146,    72,    72,    72,    72,   324,   321,    72, -1146,
      72,   156,    72, -1146,    72,   156,   321, -1146,   564,   578,
      20,    20, -1146, -1146,   550, -1146,  1097,   543,   550,   321,
     220,   104,   500,   321,   592,  1475, -1146,   534,    20,    84,
      84,   534,    20,   534,   104, -1146,   534,   534,   232,   321,
     534, -1146,   214, -1146,    20,   321,   220,   534,   929, -1146,
     534,   410, -1146, -1146, -1146, -1146,   534,    49, -1146,   534,
      48, -1146,   534,    73, -1146, -1146,   592, -1146, -1146,   592,
   -1146, -1146, -1146,   534,   320,   400,  1065, -1146, -1146, -1146,
     863, -1146,  1186, -1146,   592, -1146,   562,   116,    20,   584,
     321,    20, -1146,    72, -1146, -1146,   321, -1146,   321, -1146,
      72, -1146,   321,    72, -1146,    72, -1146,    72,   321, -1146,
   -1146, -1146,   156, -1146,   324,   321, -1146, -1146, -1146, -1146,
   -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146,
      72,    72,    72, -1146, -1146,    72,    72,    72,    72,    72,
     321, -1146,    72,   321,   321,   321,   321, -1146, -1146,    72,
      72,   156, -1146,   321, -1146, -1146,    72, -1146,    72,   321,
      72,   321,    72,   321,   321,   321,   321,   321,   321,   321,
     362,   466, -1146,  1229,   321,    72, -1146, -1146, -1146, -1146,
   -1146, -1146, -1146, -1146, -1146, -1146, -1146,    72,   156,    72,
     156, -1146,    72,   156,    72,   156,    72,   156,    72,   156,
      72, -1146,   156,    72, -1146,    72,    72, -1146, -1146,    72,
   -1146,   156,    72, -1146, -1146,   156, -1146,    72, -1146,   156,
   -1146,    72,   578, -1146, -1146, -1146, -1146, -1146, -1146,   237,
   -1146,    20,   592, -1146,   575,   575,   575,   348,   100,   111,
     321,   100,   321, -1146,   562, -1146, -1146, -1146, -1146, -1146,
   -1146,    20, -1146,   348,   512, -1146,   575, -1146,   102, -1146,
   -1146, -1146, -1146, -1146, -1146, -1146, -1146,   592, -1146, -1146,
     592,   592, -1146,   519, -1146, -1146, -1146, -1146,   487,   314,
     638,   444, -1146,    20,   488, -1146,    20,   326, -1146,  1097,
     450, -1146,  1097,   454, -1146,   496, -1146,   533, -1146, -1146,
   -1146,   116,    20,  1420,   321,    20,   550,   321,   373,   562,
   -1146,    72, -1146,    72, -1146, -1146, -1146, -1146,    72,    72,
      72,    72,   507,   321,    72, -1146, -1146, -1146,   156,    72,
   -1146, -1146,    72, -1146,    72, -1146,    72, -1146,    72,    72,
      72,   321, -1146,   531,   519, -1146,  1229,   592, -1146,   321,
   -1146,    72, -1146,    72, -1146,    72, -1146, -1146,   321,    72,
      72,    72, -1146,   321,    72,    72, -1146,    72,    72, -1146,
      72, -1146, -1146,    72, -1146,   321,    72,    72, -1146, -1146,
      72,    72,    72,   156, -1146,    72, -1146, -1146, -1146, -1146,
   -1146, -1146,   321,    72,   321,   321,   321,   321,   549, -1146,
   -1146, -1146,   321,    20,   393,   321,   116,   507,   558,   321,
     321, -1146, -1146, -1146,   592, -1146, -1146, -1146, -1146, -1146,
     360, -1146, -1146,   326, -1146,   450, -1146, -1146, -1146,   450,
   -1146, -1146,  1097, -1146,  1097,   496, -1146,  1316,   321,   348,
   -1146, -1146, -1146,  1097,    20,    72,   116, -1146,    72,    72,
      72,    72, -1146,    72, -1146, -1146,    72, -1146,    72, -1146,
   -1146, -1146, -1146, -1146,   156,    72, -1146,    72, -1146, -1146,
     599,   321,    72,    72,    72, -1146,    72,    72,    72,    72,
   -1146,    72, -1146,    72, -1146, -1146,   321, -1146, -1146,    72,
      72,    72,   156,    72, -1146,    72,   321, -1146,    72,   584,
     156, -1146,    72, -1146,   676,   676,   676, -1146,   507,   134,
     321,   321,    20, -1146,   676,   518, -1146, -1146,   380,   661,
     636,   450, -1146, -1146, -1146, -1146,  1097,   377,   321, -1146,
   -1146, -1146,  1080, -1146,   626, -1146,   595, -1146,   321,   156,
      20,  1023,   321, -1146,    72,   156,    72,   156,    72,   156,
      72,    72,    72,    72, -1146, -1146,    72,    72,   521,   518,
   -1146,    72,    72, -1146,    72, -1146, -1146,    72, -1146,    72,
   -1146, -1146,    72,   321, -1146, -1146, -1146, -1146, -1146, -1146,
      72, -1146,   156, -1146,   373,    72, -1146,    72,    72, -1146,
     756, -1146,    20, -1146,    20,   911, -1146,    20,   321, -1146,
   -1146, -1146,   507,  1072, -1146, -1146,   661,   636,   636, -1146,
    1097, -1146, -1146,   321,    20,   321,   348, -1146, -1146, -1146,
   -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146,
     156, -1146,   156,    72,    72, -1146,    72,    72, -1146,    72,
      72, -1146,    72, -1146, -1146,    72,    72,   156,    72, -1146,
   -1146, -1146, -1146,   321, -1146,    72,    72,    72,    20,    20,
   -1146, -1146,  1494,  1692, -1146,  2400,   321,  1178, -1146, -1146,
      20,   636, -1146,   507,   321,   927,   321,   321,    72,    72,
      72, -1146, -1146, -1146, -1146, -1146, -1146, -1146,    72, -1146,
   -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146,
   -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146,
   -1146, -1146, -1146,    72,    72, -1146, -1146, -1146, -1146, -1146,
     321, -1146,    72,    72,    72,    72,    72,    72,   321, -1146,
      72, -1146,    72, -1146,    72, -1146,    72, -1146, -1146,    72,
     156, -1146, -1146,   507,   321,   711,   711,   431,   431, -1146,
     572,   131,   321,   618,   711,   690,   690, -1146,   415, -1146,
     321, -1146, -1146,   373,    72, -1146, -1146, -1146,    72,    72,
   -1146,    72,   156,    72,   156, -1146, -1146,    72,    72, -1146,
      72,   156,    72, -1146,    72,    72, -1146,    72,    72,    72,
   -1146,    72, -1146,    72, -1146,    72,    72, -1146,    72, -1146,
      72,    72, -1146,    72, -1146,    72, -1146,   321,   321, -1146,
   -1146,   572, -1146,  1097,   145, -1146,   592, -1146, -1146,   572,
   -1146,  1097,   145, -1146, -1146, -1146,   145, -1146, -1146, -1146,
     112, -1146, -1146,   415, -1146, -1146, -1146,   415, -1146, -1146,
   -1146, -1146,    72, -1146,    72,    72,    72,    72,   321,    72,
      72,   321,    72,    72,    72,    72,    72, -1146, -1146,   145,
   -1146,   393, -1146, -1146, -1146,   145, -1146, -1146, -1146, -1146,
   -1146, -1146, -1146,    72,   321,    72, -1146, -1146, -1146
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   423,   412,   401,   411,   172,   435,   456,   403,   484,
     487,   597,   651,   677,   680,   509,   502,   362,   556,   494,
     491,   499,   497,   621,   667,   402,   425,   436,   404,   424,
     485,   489,   488,   510,   495,   492,   500,     0,     4,     5,
       2,     0,    13,   352,   353,     0,    18,   391,   389,   390,
     392,   393,     0,     0,     3,     0,    12,     0,    19,     0,
      11,     0,    20,   469,   470,     0,    14,     0,    21,     0,
      15,     0,    22,     0,    16,     0,    23,     0,    17,     0,
      24,   544,   542,   543,   545,   546,    25,     0,    26,    27,
      28,    29,    30,     0,     7,     0,     8,     0,     9,     0,
      10,     0,     6,     0,     1,    84,    85,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      89,   173,     0,   363,     0,   400,     0,     0,   413,     0,
     426,     0,     0,   457,     0,     0,   427,     0,   427,     0,
     427,     0,   504,   557,     0,   598,     0,   622,   636,   623,
     637,   624,   625,   639,   626,   627,   628,   629,   630,   631,
     632,   633,   634,   635,     0,   619,     0,   652,     0,     0,
       0,   654,     0,     0,    88,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    86,    87,   618,     0,     0,
       0,     0,     0,     0,    51,    31,    48,    49,    50,    52,
      53,     0,   174,    32,    33,    37,     0,    36,    46,     0,
     175,   165,   367,     0,     0,   449,   450,   375,   406,     0,
       0,     0,     0,   405,   548,   551,   549,   552,     0,     0,
       0,     0,     0,     0,     0,     0,   414,     0,   427,     0,
     437,   438,   439,   440,   460,   459,     0,     0,   565,   474,
       0,     0,     0,   473,     0,   561,   562,     0,   432,   201,
     428,     0,   486,   568,     0,     0,     0,   493,   571,     0,
       0,     0,   498,   574,     0,     0,     0,   516,   512,   201,
     201,     0,   201,     0,   503,   559,     0,     0,   610,   605,
     606,   600,   596,     0,   601,   611,   607,   608,   603,   604,
       0,   577,     0,     0,   599,   602,   609,   613,   614,   620,
       0,     0,     0,     0,   656,     0,     0,     0,    45,    38,
       0,    44,    34,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   429,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   508,   507,     0,     0,   505,     0,
       0,     0,     0,   575,     0,     0,     0,     0,     0,     0,
       0,     0,   653,     0,    40,    42,    39,    47,   179,   182,
     176,   177,   166,   169,     0,   171,     0,   164,   371,     0,
     357,     0,     0,   354,   359,   368,   365,     0,     0,   377,
     381,     0,   417,   228,   418,   399,   215,   216,   217,     0,
       0,     0,     0,   451,     0,     0,   525,     0,     0,     0,
       0,     0,     0,     0,   415,   408,   422,     0,   465,     0,
     201,     0,   461,   201,     0,     0,     0,     0,     0,     0,
     201,   201,   433,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    61,   513,    59,   514,     0,   201,   517,
       0,     0,     0,     0,   584,   585,   586,   582,   589,   583,
     590,   576,   586,   586,   591,    91,   615,     0,     0,   528,
       0,     0,     0,   688,    91,     0,     0,    43,     0,     0,
       0,     0,   356,   361,     0,   360,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   394,     0,     0,     0,
       0,     0,     0,     0,     0,   396,     0,     0,     0,     0,
       0,   453,     0,   454,     0,     0,     0,     0,     0,   471,
       0,     0,   202,   430,   431,   490,     0,     0,   496,     0,
       0,   501,     0,     0,    55,    69,     0,    56,    60,     0,
     511,   506,   515,     0,     0,     0,     0,   595,   578,   579,
       0,   580,     0,   581,   616,   612,     0,     0,     0,     0,
       0,     0,   655,   167,   170,   180,     0,   183,     0,   373,
     357,   372,     0,   357,   369,   365,   364,     0,     0,   386,
     387,   382,     0,   374,   378,     0,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
       0,     0,     0,   398,   419,     0,     0,   442,     0,   442,
       0,   526,     0,     0,     0,     0,     0,   210,   209,   201,
     201,     0,   407,     0,   455,   466,     0,   458,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      59,     0,    70,     0,     0,     0,   185,   111,   144,   147,
     155,   159,   109,   588,    93,    99,   100,   104,     0,    96,
       0,   103,    96,     0,    96,     0,    96,     0,    96,     0,
      96,    95,     0,   586,   594,   586,   591,    92,   617,     0,
     540,     0,   662,   519,   520,     0,   529,   384,   678,     0,
     679,     0,     0,   178,   181,   358,   370,   355,   366,     0,
     395,     0,   379,   376,     0,     0,     0,    38,     0,     0,
       0,     0,     0,   547,     0,   550,   397,   553,   554,   410,
     409,     0,   434,    38,     0,   472,     0,   560,     0,   563,
     566,   567,   569,   570,   572,   573,    57,     0,    54,    79,
       0,     0,    64,    82,    66,    77,    78,   555,     0,     0,
       0,     0,   102,     0,     0,   128,     0,     0,   129,     0,
       0,   130,     0,     0,   131,     0,    94,     0,   587,   593,
     592,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     168,     0,   388,   384,   380,   243,   244,   245,   201,   201,
     201,   201,     0,     0,   442,   208,   207,   452,     0,   443,
     445,   441,   442,   447,     0,   213,     0,   467,   463,     0,
     201,     0,    63,    59,    82,    71,     0,     0,    81,     0,
     107,    96,   105,     0,   101,    96,    98,   112,     0,    96,
      96,    96,   145,     0,    96,    96,   148,     0,    96,   156,
       0,   160,   161,     0,    90,     0,   644,     0,   660,   666,
     662,   662,    91,     0,   661,     0,   385,   527,   671,   672,
     669,   670,     0,     0,     0,     0,     0,     0,     0,   421,
      35,   416,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   564,    58,    80,     0,    65,    68,    83,   558,   106,
       0,    97,   110,     0,   132,     0,   133,   134,   143,     0,
     135,   136,     0,   137,     0,     0,   184,     0,     0,    38,
     663,   664,   665,     0,     0,     0,     0,   383,     0,     0,
       0,   201,   446,     0,   444,   448,   521,   214,   201,   199,
     197,   196,   198,   468,     0,   201,   462,     0,    75,    67,
       0,     0,   113,   114,   115,   116,    96,    96,    96,    96,
     149,     0,   157,   153,   162,   163,     0,   658,   650,   644,
     644,    91,     0,    91,   643,     0,     0,   541,   384,     0,
       0,   683,   684,   682,     0,     0,     0,   420,     0,     0,
       0,     0,     0,   464,     0,     0,    74,   108,     0,     0,
       0,     0,   138,   139,   140,   141,     0,     0,     0,   158,
     645,   646,     0,   647,     0,   649,     0,   659,     0,     0,
       0,     0,     0,   249,   219,     0,    91,     0,   225,     0,
     201,   384,   521,   521,   518,   203,     0,   476,    76,     0,
      72,   117,   118,   119,   120,   121,   122,    96,   150,     0,
     154,   152,    91,     0,   538,   533,   534,   535,   536,   537,
     384,   531,     0,   539,     0,     0,   687,   684,   684,   681,
       0,   218,     0,   223,     0,     0,   224,     0,     0,   524,
     522,   523,     0,     0,   475,    73,     0,     0,     0,   142,
       0,   648,   657,     0,     0,     0,    38,   686,   685,   194,
     191,   190,   193,   211,   192,   212,   222,   351,   186,   188,
       0,   187,     0,   219,    91,   250,     0,     0,   227,   225,
       0,   200,   201,   482,   480,    91,    91,     0,   123,   124,
     125,   126,   151,     0,   530,   205,   673,   201,     0,     0,
     221,   220,     0,     0,   226,     0,     0,     0,   477,   479,
       0,     0,   146,     0,     0,     0,     0,     0,     0,   205,
     252,   309,   310,   311,   312,   313,   314,   315,   254,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   339,   256,   258,   340,   341,   342,   343,   344,
       0,   251,   276,   303,   283,   285,   287,   289,     0,   282,
     299,   195,    91,   483,   384,   127,   201,   532,   676,    91,
       0,   668,   689,     0,     0,     0,     0,     0,     0,   246,
       0,     0,     0,     0,     0,     0,     0,   248,     0,   478,
       0,   206,   675,     0,   201,   204,   346,   350,   201,   201,
     253,   201,     0,   201,     0,   255,   348,   201,   201,   257,
     201,     0,   201,   259,   201,   201,   277,   201,   201,   201,
     304,   201,   247,   201,   284,   201,   201,   286,   201,   288,
     201,   201,   290,   201,   300,   201,   481,     0,     0,   260,
     267,     0,   264,     0,     0,   266,     0,   268,   275,     0,
     272,     0,     0,   274,   278,   281,     0,   279,   305,   308,
       0,   306,   291,     0,   293,   294,   295,     0,   297,   298,
     301,   302,   673,   189,   201,   201,     0,   201,     0,   201,
     201,     0,   201,   201,   201,   201,   201,   674,   263,     0,
     261,     0,   265,   349,   271,     0,   269,   347,   273,   280,
     307,   292,   296,   201,     0,   201,   262,   345,   270
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1146, -1146, -1146,  -204,  -218,  -184, -1146,   -51,  -187,   363,
   -1146, -1146, -1146, -1146, -1146, -1146,  -205,  -324,  -585,    54,
    -710,  -559, -1146, -1146, -1146, -1146, -1146, -1146, -1146,  -513,
    -152, -1146, -1146, -1146,  -784, -1146, -1146,  -113,   -12,  1821,
    1042,  1045, -1146,  -494,  -562,  -524, -1146, -1146,   -60, -1146,
   -1146,   -53, -1146, -1146, -1146,   -32,  -169, -1146, -1146,  -712,
   -1146, -1146, -1146, -1146, -1146,  -717, -1146, -1146, -1146, -1146,
    -724, -1146, -1146, -1146,     0, -1146, -1146, -1146, -1146, -1146,
     248, -1146, -1146,  -473, -1146, -1146,  -695, -1146, -1146,  -994,
   -1146, -1146, -1146, -1146,  -894,  -295,  -265, -1145,  -415, -1146,
   -1146, -1146,  -849,  -981,   -20, -1146,  -364, -1146, -1146,  -368,
    -275,   230, -1146, -1146,  -287,  -890, -1146,  -315,  -915,  -614,
   -1146,  -930,  -454, -1146, -1146, -1146, -1146,  -461, -1146, -1146,
   -1146, -1146,  -580,  -457, -1146,  -545, -1146,  -644, -1146,  -306,
   -1146,   808,  -393,   -88,   812,  -398,    12,  -119,  -323,   199,
   -1146, -1146, -1146,   288, -1146,  -136, -1146,  -153, -1146, -1146,
   -1146, -1146, -1146, -1146,  -804, -1146, -1146, -1146,   614,    -1,
       8,   694,   696,  -231,   -59, -1146, -1146,  -144,    78, -1146,
   -1146, -1146, -1146, -1146,    79, -1146, -1146, -1146,     6, -1146,
     616,   -41, -1146, -1146, -1146,    16, -1146,   238,  -166,  -581,
    -160,  -159,  -155, -1146, -1146,   177,   -84,    13,   275, -1146,
   -1146, -1146, -1146, -1146, -1146, -1146,  -353,  -741,  -846, -1146,
    -135,   707,   708, -1146,   278, -1146,  -339, -1146, -1146,   635,
       3, -1146,    17, -1146,  -157, -1146,   -98, -1146,  -150, -1146,
    -117, -1146,  -170, -1146, -1146,   455, -1146, -1146, -1146, -1146,
   -1146,   222,  -727,  -279, -1146, -1146,  -546, -1146, -1146,  -723,
   -1146, -1146, -1146,  -702, -1146, -1146,   716,   641,   643,   660,
     645,   662, -1146, -1146,    21, -1146, -1146, -1146, -1146,   -87,
   -1146, -1146, -1146, -1146,    24, -1146, -1146,   -96, -1146, -1146,
    -102, -1146, -1146,   726, -1146, -1146, -1146,  -303,  -390,   265,
     143, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146, -1146,
   -1146, -1146, -1146, -1146, -1146, -1146,   860,  -207, -1146,  -110,
     567, -1146,   240, -1146, -1146, -1146,  -895, -1146, -1146,   -95,
     861, -1146,  -997,  -490, -1146,  -923,  -125, -1146, -1146, -1146,
    -299, -1146,  -185
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   221,   611,   223,   847,   224,   832,
     225,   226,   404,   405,   227,   341,   228,   229,   857,   576,
     484,   577,   485,   681,   853,   578,   782,   925,   579,   783,
     856,   979,   980,  1069,   784,   785,   786,   858,   107,   207,
     371,   135,   885,   593,   717,   792,   694,   695,   696,   697,
     698,   699,   700,   868,   981,   701,   702,   703,   873,   704,
     705,   877,   991,  1079,  1163,   706,  1038,   707,   880,   993,
     708,   709,   883,   996,   470,   344,    41,   132,   231,   412,
     413,   414,   606,   415,   416,   608,   711,   712,  1137,  1279,
    1139,  1140,   973,   974,   837,   372,   660,  1141,  1184,   838,
     661,  1142,   846,   968,   435,   436,  1101,   437,   438,  1106,
     439,   640,   641,   642,   828,  1054,  1056,  1058,  1055,  1146,
    1230,  1280,  1289,  1231,  1296,  1238,  1304,  1309,  1239,  1314,
    1262,  1300,  1232,  1281,  1282,  1290,  1291,  1283,  1284,  1144,
      42,   238,   346,   525,   269,   347,   239,   134,   233,   529,
     234,   426,   615,   420,   421,   612,   610,   271,   241,   430,
     431,   625,   533,   621,   817,   622,   823,    46,    47,    48,
      49,    50,    51,   440,   136,    52,    53,   242,   422,   549,
      55,   139,   257,   455,   441,   442,   645,   833,   243,    57,
     141,   195,   280,   281,   473,    58,    59,   259,   244,   750,
     245,   246,   247,   248,   249,   444,   840,   898,   459,    61,
     144,   266,   267,   463,   460,   919,   721,   666,   848,   975,
      62,    63,    64,   272,   467,  1114,  1156,  1157,  1244,    65,
     273,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,   202,    79,   300,   301,   487,   302,   303,
     490,   899,   722,  1020,   448,   652,   510,   511,   727,   723,
    1090,  1091,  1092,   724,   725,  1008,    80,    81,    82,   250,
      83,   251,    84,    85,    86,    87,   154,   306,   307,    88,
     274,   275,   276,   277,    89,   285,   286,    90,   290,   291,
      91,   295,   296,    92,   320,   501,   502,   496,   587,   504,
     591,   321,   322,    93,   156,   323,   324,   325,   329,   330,
     507,    94,   174,    95,   175,   176,   900,   948,    97,   178,
     182,   183,   335,   336,  1001,  1002,   892,   893,    98,   814,
     901,   100,   902,  1186,   101,   729,   102,   103,   514,  1012,
    1052,   515,  1013
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      40,   222,   256,    66,   384,   385,    56,   388,   166,   337,
     340,   461,    45,    60,   961,   406,   338,   179,   180,   904,
     600,   523,   419,   172,   693,   260,   522,   165,   714,   171,
     294,   261,   262,   108,   240,   252,   263,  1060,   169,   284,
     109,   110,   342,   111,   253,   112,   604,   113,   289,   114,
     719,   137,  1003,   115,   142,   116,   145,   117,   147,   118,
     149,   119,   151,   120,   963,   121,   683,   122,   752,   312,
     871,   887,   926,   875,   895,   123,   878,   870,    54,   326,
     874,   124,   869,   125,   886,   126,  1049,   127,     1,   128,
     890,   129,   503,   466,   862,   881,   866,  1125,   781,   866,
    1057,  1059,   866,  1005,     1,   866,  1138,   197,   866,   199,
     432,   891,   588,   589,    19,    21,  1299,   432,   835,  1138,
     433,   776,    15,   836,   157,   458,  1133,   451,   264,     5,
       5,  1152,   104,   158,  1067,   407,     5,    10,    34,    36,
     237,   161,   164,   458,    17,  1133,   168,  1082,     5,   170,
     265,     5,  1129,  1277,   720,     5,  1130,  1131,  1132,   410,
      33,   278,     5,    26,   298,   553,   130,    29,   555,   297,
     298,   213,   720,  1066,   454,   562,   563,   181,   795,    26,
     798,   483,   801,    29,   804,   434,   806,    31,    32,   966,
    1059,   506,   434,   581,   279,  1364,  1026,   516,   517,   461,
    1135,   299,  1246,   309,  1048,   270,   985,   617,   844,   987,
    1143,   988,   310,   984,  1134,   989,   986,   362,   983,  1135,
     311,   314,   348,   458,   999,   318,    19,   349,   319,   351,
     427,     5,   283,   668,   184,   994,   353,   354,   866,   657,
     866,  1070,     2,   658,   866,  1000,     2,   464,  -638,     5,
      34,   889,     4,   912,  1011,   365,     4,  1109,   366,   820,
     497,   915,  1250,  1288,  1288,   370,  1295,   854,   258,   374,
     505,   924,  1274,   903,   377,  1165,  1317,   494,   494,   380,
    1167,   232,   466,   713,   383,  1115,  1123,   715,   594,   386,
    1249,   389,   495,   495,   922,   391,  1062,   498,   232,   500,
     618,   392,   232,   418,  1073,  1075,  1076,  1077,   499,  1095,
       9,  1072,  1074,   654,   105,   106,  1071,  1063,   396,  1188,
    1189,  -640,    14,   808,   401,   809,  1285,  1354,    16,   624,
    1357,     5,    10,   493,  1307,  1359,   866,   929,  1362,   235,
     686,   931,  1363,     5,    22,   934,   936,   937,   236,  1097,
     940,   941,   686,   237,   943,   474,   428,    17,   477,   780,
      30,   480,     2,   450,   759,   760,   574,   268,   575,   692,
     465,   680,     4,   471,   682,  1383,   457,    15,  -641,   690,
     691,  1385,    31,    32,   657,   998,   646,   648,   658,   718,
     688,   689,  1159,  1160,  1161,   647,   649,     7,   952,  1158,
     835,   293,   520,   679,   521,   836,   742,  -642,   672,   526,
     675,   689,    12,   690,   691,    33,   821,   305,   260,   534,
     677,   446,   334,   406,   261,   262,   339,   535,   522,   263,
     538,     1,     5,   690,   691,    10,   542,  1277,  1129,   406,
    1270,   849,  1130,  1131,  1132,    10,   235,   -41,     5,   554,
      24,   523,  1286,  1277,   558,   236,   237,  1245,   829,   830,
      17,     5,  1032,  1033,  1034,  1035,   237,     5,   429,   494,
     686,     5,   215,   494,   688,   689,   686,  1384,   682,   850,
     686,   205,   206,   237,   495,    31,    32,    17,   495,  1096,
    1147,   731,   498,  1150,   500,    31,    32,   690,   691,   598,
    1134,   219,   220,   601,   691,     5,    26,  1043,   446,  1045,
      29,     7,   408,   409,   686,    21,   -62,   643,   575,   689,
     288,   650,   735,   653,   609,   737,   655,   656,   614,   458,
     663,   445,   780,   905,   906,   907,   909,   824,  1136,    36,
     671,   690,   691,  1148,   327,   328,   674,   469,   659,   676,
     433,  1154,   678,  1119,   476,   920,   835,   479,   619,   620,
     482,   836,  1103,   684,   673,   969,     5,     5,   492,   970,
     971,   972,   852,   232,   418,   682,   855,   447,   449,  1129,
      16,   452,   453,  1130,  1131,  1132,   710,   509,  1121,     5,
     710,   468,   710,  1233,  1277,  1240,   408,   270,   475,   513,
     483,   478,  1084,  1010,   481,   418,  1085,  1086,  1087,  1088,
     741,   409,   491,  1292,  1292,   876,  1297,  1301,   879,  1306,
     574,  1311,  1311,  1248,  1315,   406,   130,   214,   215,   728,
     216,   217,   218,   219,   220,     5,     3,   508,  1276,   512,
    1277,  1134,   130,   214,     6,   910,   216,   217,   218,   761,
    1171,   682,   855,     8,   214,   215,   780,   216,   217,   218,
       9,  1178,  1179,    11,  1089,   214,  1017,   687,   216,   217,
     218,   219,   220,  1021,   778,   779,   -70,  1355,    16,   884,
    1023,   -70,   -70,   -70,  1053,  1360,   790,   575,   793,    18,
     691,   796,    20,   799,    22,   802,   831,   805,  1010,  1365,
     807,   780,   403,  1366,    25,   927,    27,     5,    28,   812,
      30,  1286,  1277,   815,   690,   691,    35,   818,  1129,   978,
    1310,  1310,  1130,  1131,  1132,   825,   826,   827,     5,   860,
     967,  1276,   146,  1277,   148,   777,   150,   863,   152,   214,
     483,   923,   216,   217,   218,   780,   214,   215,  1269,   216,
     217,   218,   219,   220,   834,  1272,   779,   842,   990,   864,
     992,   982,  1025,  1129,  1053,  1108,   603,  1130,  1131,  1132,
    1133,   851,  1080,     5,  1254,   682,   331,   332,   333,  1170,
    1134,  1174,   686,  1110,  1111,   687,   688,   689,   670,   861,
    1149,   865,  1040,  1041,   865,   950,   951,   865,  1127,  1128,
     865,   859,   406,   865,  1293,  1312,  1305,  1155,    43,   690,
     691,   692,    44,   710,   738,   613,   896,  1006,   308,   159,
    1068,   160,   685,     1,     2,  1134,   913,   664,   839,     3,
     964,   967,   162,   163,     4,   669,     5,     6,  1242,   313,
    1083,   167,   488,     7,  1135,   315,     8,   316,   254,   317,
     255,   173,  1078,     9,    10,   716,    11,  1176,    12,   810,
      96,    99,  1367,    13,   682,    14,  1098,     0,    15,     0,
       0,    16,  1187,     0,     0,     0,     0,     0,     0,    17,
       5,   953,    18,     0,    19,    20,    21,    22,     0,   686,
       0,     0,   687,   688,   689,    23,    24,    25,    26,    27,
       0,    28,    29,    30,    31,    32,    33,     0,    34,    35,
      36,     0,     0,     0,     0,    16,   690,   691,   692,  1053,
       0,     0,     0,     0,     0,   967,     0,     0,     5,     0,
       0,    22,     0,   865,     0,   865,  1162,   686,     0,   865,
     687,   688,   689,     0,     5,     0,     0,   710,     0,     0,
       0,  1271,     0,   686,     0,     0,   687,   688,   689,     0,
       0,     0,  1022,     0,   690,   691,   692,     0,     0,     0,
     888,     0,   728,     0,     0,     0,     0,     0,     0,  1318,
     690,   691,   692,  1319,  1320,     0,  1322,     0,  1325,     0,
    1044,     0,  1327,  1328,     0,  1330,   967,  1333,  1050,  1334,
    1335,     0,  1337,  1338,  1339,     0,  1341,     0,  1342,     0,
    1344,  1345,     0,  1346,     0,  1348,  1349,     0,  1350,  1061,
    1351,   626,   627,   628,   629,   630,   631,   632,   633,   634,
       0,   865,   458,     0,     0,     0,     0,  1094,     0,     0,
       5,     0,   710,  1102,     0,  1104,     0,  1107,     0,   686,
       0,   710,   687,   688,   689,     0,     0,     0,     0,  1368,
    1370,   720,  1372,     0,  1374,  1376,   967,  1378,  1379,  1380,
    1381,  1382,     0,     0,     0,     0,   690,   691,   692,  1129,
    1124,     0,     5,  1130,  1131,  1132,  1133,     0,  1386,     5,
    1388,   686,     0,     0,   687,   688,   689,     5,   686,     0,
     710,   687,   688,   689,     0,   710,   686,     0,  1153,   687,
     688,   689,     0,   710,     0,     0,     0,    16,   690,   691,
     692,  1358,     0,   888,     0,   690,   691,   692,  1168,     0,
    1169,     0,     0,   690,   691,   692,     0,     0,     0,  1356,
       0,  1134,     0,     0,     0,  1180,     0,  1361,     0,     0,
       0,     0,   131,   133,     0,     0,   138,   140,     0,   143,
    1135,   140,     0,   140,     0,   140,     0,   140,   153,   155,
       0,   177,   177,   177,   185,     0,   186,   710,   187,   188,
       0,   189,     0,   190,   191,   710,   192,   193,   194,   196,
     194,   198,   194,   200,   201,     5,   203,     0,   204,     0,
       0,     0,     0,     5,   686,     0,     0,   687,   688,   689,
       0,     0,   686,     0,  1153,   687,   688,   689,   208,     0,
     209,   210,   211,     0,   212,     0,     0,     0,     0,     0,
     230,   690,   691,   692,     0,     3,     0,     0,  1273,   690,
     691,   692,     0,     6,   214,   215,     0,   216,   217,   218,
     219,   220,     8,     0,     0,  1278,  1278,  1287,  1287,     9,
    1294,  1298,     0,  1303,  1278,  1308,  1308,     0,  1313,     0,
    1323,    14,  1326,   343,     0,   345,     0,     0,     0,  1331,
       0,     0,   350,     0,   352,     0,     0,     0,    18,     0,
       0,    20,   355,   356,   357,   358,   359,   360,     0,   361,
     194,   363,     0,    25,     0,    27,     0,    28,   364,    30,
       0,     0,     0,   367,   368,    35,   369,     0,     0,     0,
       0,     0,   373,     0,     0,   458,   375,   376,     0,     0,
       0,   378,   379,     5,     0,     0,   381,   382,     0,     0,
       0,     0,   686,     0,   387,   687,   688,   689,   390,     0,
       0,     0,     0,     0,   720,     0,     0,     0,   997,   888,
       0,     0,     0,     0,   394,   395,     0,     0,     0,   690,
     691,   692,     0,   397,   398,   399,   214,   400,     0,   216,
     217,   218,   219,   220,   778,     0,   779,     0,   411,     0,
       0,     0,     0,   425,   133,     0,     0,     0,   443,   443,
     130,   214,   215,     0,   216,   217,   218,   219,   220,     0,
     462,   133,     0,     0,     0,   443,     0,     0,     0,   472,
       0,     0,   443,     0,     0,   443,     0,     0,   443,   458,
       0,   486,     0,     0,   489,     0,   443,     5,     0,     0,
       0,     0,     0,     0,     0,     0,   686,     0,     0,   687,
     688,   689,     0,     0,   518,   519,     0,     0,   720,     0,
       0,   345,   524,   888,   527,     0,     0,     0,   528,   530,
     531,     0,   532,   690,   691,   692,     0,     0,     0,     0,
       0,     0,     0,   537,     0,     0,   539,   540,     0,   541,
       0,   543,   544,     0,   546,   547,     0,   548,     0,   550,
       0,     0,   552,     0,     0,     0,   556,   557,     0,     0,
     560,   561,     0,     0,     0,     0,     0,   566,   567,     0,
     569,   570,     0,   572,   573,     0,     0,     0,     0,     0,
       0,     0,     0,   583,   584,   585,     0,     0,   586,     0,
       0,     0,     0,     0,   586,   586,   590,   592,     0,     0,
     596,     0,   597,     0,   599,     0,   592,     0,     0,     0,
       0,     0,     0,     0,     0,   605,   607,   626,   627,   628,
     629,   630,   631,   632,   633,   634,   635,   636,   637,   638,
     639,     0,     0,   644,     0,     0,     0,   651,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   665,
    1190,  1191,  1192,  1193,  1194,  1195,  1196,  1197,  1198,  1199,
    1200,  1201,  1202,  1203,  1204,  1205,  1206,  1207,  1208,  1209,
    1210,  1211,  1212,  1213,  1214,  1215,  1216,  1217,  1218,  1219,
    1220,  1221,  1222,  1223,  1224,  1225,  1226,  1227,  1228,  1229,
       0,     0,     0,   726,     0,   732,   177,     0,     0,     0,
       0,     0,   524,     0,     0,   524,     0,   528,     0,   739,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   744,   745,   746,     0,     0,   747,   748,   749,
     751,   749,     0,     0,   754,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   763,     0,
     764,     0,   766,     0,   768,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   788,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   789,
       0,   791,     0,     0,   794,     0,   797,     0,   800,     0,
     803,     0,   803,     0,     0,   586,     0,   586,   590,     0,
       0,   811,     0,     0,   813,     0,     0,     0,     0,   816,
       0,     0,     0,   819,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   822,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1191,
    1192,  1193,  1194,  1195,  1196,  1197,   845,  1199,  1200,  1201,
    1202,  1203,  1204,  1205,  1206,  1207,  1208,  1209,  1210,  1211,
    1212,  1213,  1214,  1215,  1216,  1217,  1218,  1219,  1220,  1221,
    1222,     0,     0,  1225,  1226,  1227,  1228,  1229,   867,     0,
       0,   872,     0,     0,     0,     0,     0,     0,     0,     0,
     882,     0,     0,     0,     0,     0,     0,   665,     0,     0,
     665,     0,     0,   739,     0,   816,     0,     0,     0,     0,
       0,     0,     0,   908,     0,     0,   749,     0,     0,     0,
       0,   914,     0,     0,   749,     0,   916,     0,   917,     0,
     918,   764,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   803,     0,   930,     0,   803,     0,     0,
       0,   933,   935,   803,     0,     0,   939,   803,     0,   942,
     803,     0,   944,     0,     0,   945,     0,     0,   947,   949,
       0,     0,   813,   813,   592,     0,     0,   954,     0,     0,
       0,     0,     0,     0,     0,   956,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   845,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     995,     0,     0,     0,     0,     0,     0,  1009,     0,  1007,
    1014,  1015,  1016,     0,     0,  1018,     0,     0,  1019,     0,
       0,     0,     0,     0,     0,     0,   282,     0,   287,  1024,
     292,     0,     0,   304,  1028,  1029,  1030,     0,  1031,   803,
     803,   803,     0,  1036,     0,  1037,     0,     0,     0,     0,
       0,   947,   947,  1042,     0,   592,     0,  1046,     0,     0,
     816,     0,     0,     0,  1051,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   845,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   665,  1100,     0,   592,     0,
    1105,     0,     0,   816,  1019,  1019,     0,     0,  1112,  1113,
       0,     0,     0,  1116,  1117,     0,  1118,     0,     0,   803,
       0,  1120,     0,     0,   592,     0,     0,     0,     0,     0,
       0,     0,   816,     0,     0,     0,     0,  1126,     0,  1051,
    1051,   393,     0,     0,     0,     0,     0,  1145,     0,  1145,
       0,     0,  1145,     0,     0,     0,     0,     0,   402,     0,
       0,     0,     0,     0,     0,   417,     0,   423,   424,   845,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   456,     0,  1100,   592,     0,  1172,  1173,
       0,  1105,  1175,     0,     0,     0,     0,  1177,   592,     0,
    1181,     0,     0,     0,     0,     0,     0,  1183,  1185,     0,
       0,     0,     0,   845,   845,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1243,     0,     0,     0,     0,
    1253,  1183,  1255,     0,     0,     0,     0,     0,     0,     0,
    1256,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   536,     0,     0,     0,  1257,  1258,     0,     0,     0,
       0,     0,   545,     0,  1260,  1261,  1263,  1264,  1265,  1266,
     551,     0,  1268,     0,   592,     0,   816,     0,   559,     0,
       0,   592,     0,     0,   564,   565,     0,     0,   568,     0,
       0,   571,     0,     0,     0,     0,     0,     0,   580,     0,
       0,   582,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1321,     0,  1324,     0,     0,   595,     0,
       0,     0,  1329,     0,  1332,     0,     0,   602,     0,  1336,
       0,     0,     0,  1340,     0,     0,     0,  1343,     0,     0,
     616,     0,  1347,     0,   623,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     662,     0,     0,     0,     0,     0,   667,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1185,     0,     0,  1369,  1371,     0,
       0,     0,  1375,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   730,     0,     0,     0,     0,     0,   733,     0,   734,
       0,     0,     0,   736,     0,     0,     0,     0,     0,   740,
       0,     0,     0,     0,     0,     0,   743,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   753,     0,     0,   755,   756,   757,   758,     0,     0,
       0,     0,     0,     0,   762,     0,     0,     0,     0,     0,
     765,     0,   767,     0,   769,   770,   771,   772,   773,   774,
     775,     0,     0,     0,     0,   787,  1234,  1191,  1192,  1193,
    1194,  1195,  1196,  1197,  1235,  1199,  1200,  1201,  1202,  1203,
    1204,  1205,  1206,  1207,  1208,  1209,  1210,  1211,  1212,  1213,
    1214,  1215,  1216,  1217,  1218,  1219,  1220,  1221,  1222,  1236,
    1237,  1225,  1226,  1227,  1228,  1229,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   841,     0,   843,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   894,     0,     0,   897,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   911,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   921,     0,     0,     0,     0,     0,     0,     0,
     928,     0,     0,     0,     0,     0,     0,     0,     0,   932,
       0,     0,     0,     0,   938,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   946,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   955,     0,   957,   958,   959,   960,     0,
       0,     0,     0,   962,     0,     0,   965,     0,     0,     0,
     976,   977,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1004,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1027,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1039,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1047,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1064,  1065,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1081,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1093,
       0,     0,     0,  1099,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1122,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1151,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1164,     0,  1166,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1182,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1241,     0,     0,
       0,     0,     0,     0,     0,  1247,     0,  1251,  1252,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1259,     0,     0,     0,     0,     0,     0,     0,  1267,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1275,     0,     0,     0,     0,
       0,     0,     0,  1302,     0,     0,     0,     0,     0,     0,
       0,  1316,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1352,  1353,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1373,
       0,     0,  1377,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1387
};

static const yytype_int16 yycheck[] =
{
       0,   185,   189,     0,   299,   300,     0,   302,   125,   213,
     228,   364,     0,     0,   908,   339,   221,   127,   128,   823,
     514,   419,   345,   125,   586,   191,   419,   125,   590,   125,
     200,   191,   191,    45,   187,   188,   191,  1018,   125,   196,
      52,    53,   229,    55,   188,    57,   519,    59,   198,    61,
     596,   110,   947,    65,   113,    67,   115,    69,   117,    71,
     119,    73,   121,    75,   913,    77,   579,    79,   649,   204,
     794,   812,   856,   797,   815,    87,   800,   794,     0,   204,
     797,    93,   794,    95,   811,    97,  1009,    99,     4,   101,
     813,   103,   395,   368,   789,   805,   791,  1094,   683,   794,
    1015,  1016,   797,   949,     4,   800,  1100,   148,   803,   150,
       6,   813,   502,   503,    65,    67,  1261,     6,     7,  1113,
      16,   680,    49,    12,   125,     9,    14,   358,     3,    17,
      17,  1112,     0,   125,  1024,   340,    17,    35,    89,    91,
      56,   125,   125,     9,    60,    14,   125,  1042,    17,   125,
      25,    17,     7,    22,    38,    17,    11,    12,    13,   343,
      87,    23,    17,    79,    51,   460,   146,    83,   463,    50,
      51,   183,    38,  1022,   361,   470,   471,    40,   702,    79,
     704,   386,   706,    83,   708,    81,   710,    85,    86,   916,
    1105,   396,    81,   488,   194,  1340,   980,   401,   403,   552,
      88,   201,  1183,   204,  1008,   193,   930,   530,   754,   933,
    1100,   935,   204,   930,    69,   939,   933,   258,   930,    88,
     204,   204,   234,     9,   947,   204,    65,   239,   204,   241,
     349,    17,    71,   556,   162,   945,   248,   249,   933,     7,
     935,  1025,     5,    11,   939,   947,     5,   366,   162,    17,
      89,   813,    15,   834,   956,   267,    15,  1061,   270,   732,
     395,   842,  1185,  1257,  1258,   277,  1260,   780,   190,   281,
     395,   856,  1253,   819,   286,  1124,  1273,   394,   395,   291,
    1126,    61,   557,   586,   296,  1069,  1090,   590,   506,   301,
    1185,   303,   394,   395,   853,   307,  1019,   395,    61,   395,
     531,   313,    61,    62,  1028,  1029,  1030,  1031,   395,  1050,
      34,  1028,  1029,   544,   158,   159,  1028,  1019,   330,  1168,
    1169,   162,    46,   713,   336,   715,  1256,  1321,    52,   534,
    1324,    17,    35,   392,  1264,  1329,  1031,   861,  1332,    19,
      26,   865,  1336,    17,    68,   869,   870,   871,    28,  1051,
     874,   875,    26,    56,   878,   375,   350,    60,   378,   683,
      84,   381,     5,   357,   659,   660,   154,    70,   156,    55,
     367,   576,    15,   373,   579,  1369,   363,    49,   162,    53,
      54,  1375,    85,    86,     7,   947,   539,   540,    11,   594,
      30,    31,  1116,  1117,  1118,   539,   540,    24,   892,  1116,
       7,    73,   414,   573,   416,    12,   624,   162,   561,   421,
     567,    31,    39,    53,    54,    87,   739,    64,   584,   431,
     570,    48,    41,   747,   584,   584,   149,   439,   821,   584,
     442,     4,    17,    53,    54,    35,   448,    22,     7,   763,
    1244,   764,    11,    12,    13,    35,    19,   149,    17,   461,
      77,   849,    21,    22,   466,    28,    56,  1181,   745,   746,
      60,    17,   986,   987,   988,   989,    56,    17,    57,   586,
      26,    17,   148,   590,    30,    31,    26,  1371,   683,   766,
      26,   160,   161,    56,   586,    85,    86,    60,   590,  1051,
    1104,   601,   590,  1107,   590,    85,    86,    53,    54,   511,
      69,   153,   154,   515,    54,    17,    79,  1001,    48,  1003,
      83,    24,    32,    33,    26,    67,   154,   537,   156,    31,
      72,   541,   610,   543,   524,   613,   546,   547,   528,     9,
     550,   354,   856,   828,   829,   830,   831,   742,  1100,    91,
     560,    53,    54,  1105,    74,    75,   566,   370,   548,   569,
      16,  1113,   572,  1077,   377,   850,     7,   380,    58,    59,
     383,    12,  1056,   583,   561,     7,    17,    17,   391,    11,
      12,    13,   777,    61,    62,   780,   781,   355,   356,     7,
      52,   359,   360,    11,    12,    13,   586,    25,  1082,    17,
     590,   369,   592,  1173,    22,  1175,    32,   585,   376,    47,
     805,   379,     7,   956,   382,    62,    11,    12,    13,    14,
     622,    33,   390,  1257,  1258,   799,  1260,  1261,   802,  1263,
     154,  1265,  1266,  1185,  1268,   949,   146,   147,   148,    45,
     150,   151,   152,   153,   154,    17,    10,   397,    20,   399,
      22,    69,   146,   147,    18,   832,   150,   151,   152,   661,
    1144,   856,   857,    27,   147,   148,   980,   150,   151,   152,
      34,  1155,  1156,    37,    69,   147,   961,    29,   150,   151,
     152,   153,   154,   968,   155,   157,   155,  1321,    52,   146,
     975,   160,   161,   162,     8,  1329,   698,   156,   700,    63,
      54,   703,    66,   705,    68,   707,   747,   709,  1051,  1343,
     712,  1025,   339,  1347,    78,   857,    80,    17,    82,   721,
      84,    21,    22,   725,    53,    54,    90,   729,     7,   924,
    1265,  1266,    11,    12,    13,   150,   151,   152,    17,   789,
     917,    20,   116,    22,   118,   681,   120,   790,   122,   147,
     945,   854,   150,   151,   152,  1069,   147,   148,  1242,   150,
     151,   152,   153,   154,   748,  1249,   157,   751,   942,   791,
     944,   930,   980,     7,     8,  1060,   518,    11,    12,    13,
      14,   768,  1037,    17,  1189,   980,   209,   210,   211,  1143,
      69,  1149,    26,  1062,  1063,    29,    30,    31,   558,   789,
    1105,   791,   999,  1000,   794,   890,   891,   797,  1097,  1098,
     800,   788,  1126,   803,  1258,  1266,  1263,  1113,     0,    53,
      54,    55,     0,   813,   615,   527,   816,   953,   204,   125,
    1025,   125,   584,     4,     5,    69,   838,   552,   749,    10,
     914,  1018,   125,   125,    15,   557,    17,    18,  1177,   204,
    1044,   125,   387,    24,    88,   204,    27,   204,   188,   204,
     188,   125,  1036,    34,    35,   590,    37,  1152,    39,   716,
       0,     0,  1352,    44,  1069,    46,  1051,    -1,    49,    -1,
      -1,    52,  1167,    -1,    -1,    -1,    -1,    -1,    -1,    60,
      17,   893,    63,    -1,    65,    66,    67,    68,    -1,    26,
      -1,    -1,    29,    30,    31,    76,    77,    78,    79,    80,
      -1,    82,    83,    84,    85,    86,    87,    -1,    89,    90,
      91,    -1,    -1,    -1,    -1,    52,    53,    54,    55,     8,
      -1,    -1,    -1,    -1,    -1,  1112,    -1,    -1,    17,    -1,
      -1,    68,    -1,   933,    -1,   935,  1120,    26,    -1,   939,
      29,    30,    31,    -1,    17,    -1,    -1,   947,    -1,    -1,
      -1,  1246,    -1,    26,    -1,    -1,    29,    30,    31,    -1,
      -1,    -1,   974,    -1,    53,    54,    55,    -1,    -1,    -1,
      43,    -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,  1274,
      53,    54,    55,  1278,  1279,    -1,  1281,    -1,  1283,    -1,
    1002,    -1,  1287,  1288,    -1,  1290,  1183,  1292,  1010,  1294,
    1295,    -1,  1297,  1298,  1299,    -1,  1301,    -1,  1303,    -1,
    1305,  1306,    -1,  1308,    -1,  1310,  1311,    -1,  1313,  1019,
    1315,    92,    93,    94,    95,    96,    97,    98,    99,   100,
      -1,  1031,     9,    -1,    -1,    -1,    -1,  1049,    -1,    -1,
      17,    -1,  1042,  1055,    -1,  1057,    -1,  1059,    -1,    26,
      -1,  1051,    29,    30,    31,    -1,    -1,    -1,    -1,  1354,
    1355,    38,  1357,    -1,  1359,  1360,  1253,  1362,  1363,  1364,
    1365,  1366,    -1,    -1,    -1,    -1,    53,    54,    55,     7,
    1092,    -1,    17,    11,    12,    13,    14,    -1,  1383,    17,
    1385,    26,    -1,    -1,    29,    30,    31,    17,    26,    -1,
    1100,    29,    30,    31,    -1,  1105,    26,    -1,    36,    29,
      30,    31,    -1,  1113,    -1,    -1,    -1,    52,    53,    54,
      55,  1326,    -1,    43,    -1,    53,    54,    55,  1140,    -1,
    1142,    -1,    -1,    53,    54,    55,    -1,    -1,    -1,  1323,
      -1,    69,    -1,    -1,    -1,  1157,    -1,  1331,    -1,    -1,
      -1,    -1,   107,   108,    -1,    -1,   111,   112,    -1,   114,
      88,   116,    -1,   118,    -1,   120,    -1,   122,   123,   124,
      -1,   126,   127,   128,   132,    -1,   134,  1177,   136,   137,
      -1,   139,    -1,   141,   142,  1185,   144,   145,   146,   147,
     148,   149,   150,   151,   152,    17,   154,    -1,   156,    -1,
      -1,    -1,    -1,    17,    26,    -1,    -1,    29,    30,    31,
      -1,    -1,    26,    -1,    36,    29,    30,    31,   176,    -1,
     178,   179,   180,    -1,   182,    -1,    -1,    -1,    -1,    -1,
     185,    53,    54,    55,    -1,    10,    -1,    -1,  1250,    53,
      54,    55,    -1,    18,   147,   148,    -1,   150,   151,   152,
     153,   154,    27,    -1,    -1,  1255,  1256,  1257,  1258,    34,
    1260,  1261,    -1,  1263,  1264,  1265,  1266,    -1,  1268,    -1,
    1282,    46,  1284,   231,    -1,   233,    -1,    -1,    -1,  1291,
      -1,    -1,   240,    -1,   242,    -1,    -1,    -1,    63,    -1,
      -1,    66,   250,   251,   252,   253,   254,   255,    -1,   257,
     258,   259,    -1,    78,    -1,    80,    -1,    82,   266,    84,
      -1,    -1,    -1,   271,   272,    90,   274,    -1,    -1,    -1,
      -1,    -1,   280,    -1,    -1,     9,   284,   285,    -1,    -1,
      -1,   289,   290,    17,    -1,    -1,   294,   295,    -1,    -1,
      -1,    -1,    26,    -1,   302,    29,    30,    31,   306,    -1,
      -1,    -1,    -1,    -1,    38,    -1,    -1,    -1,    42,    43,
      -1,    -1,    -1,    -1,   322,   323,    -1,    -1,    -1,    53,
      54,    55,    -1,   331,   332,   333,   147,   335,    -1,   150,
     151,   152,   153,   154,   155,    -1,   157,    -1,   343,    -1,
      -1,    -1,    -1,   348,   349,    -1,    -1,    -1,   353,   354,
     146,   147,   148,    -1,   150,   151,   152,   153,   154,    -1,
     365,   366,    -1,    -1,    -1,   370,    -1,    -1,    -1,   374,
      -1,    -1,   377,    -1,    -1,   380,    -1,    -1,   383,     9,
      -1,   386,    -1,    -1,   389,    -1,   391,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,    -1,    29,
      30,    31,    -1,    -1,   412,   413,    -1,    -1,    38,    -1,
      -1,   419,   420,    43,   422,    -1,    -1,    -1,   426,   427,
     428,    -1,   430,    53,    54,    55,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   441,    -1,    -1,   444,   445,    -1,   447,
      -1,   449,   450,    -1,   452,   453,    -1,   455,    -1,   457,
      -1,    -1,   460,    -1,    -1,    -1,   464,   465,    -1,    -1,
     468,   469,    -1,    -1,    -1,    -1,    -1,   475,   476,    -1,
     478,   479,    -1,   481,   482,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   491,   492,   493,    -1,    -1,   496,    -1,
      -1,    -1,    -1,    -1,   502,   503,   504,   505,    -1,    -1,
     508,    -1,   510,    -1,   512,    -1,   514,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   520,   521,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,    -1,    -1,   538,    -1,    -1,    -1,   542,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   554,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
      -1,    -1,    -1,   598,    -1,   603,   601,    -1,    -1,    -1,
      -1,    -1,   610,    -1,    -1,   613,    -1,   615,    -1,   617,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   640,   641,   642,    -1,    -1,   645,   646,   647,
     648,   649,    -1,    -1,   652,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   666,    -1,
     668,    -1,   670,    -1,   672,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   685,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   697,
      -1,   699,    -1,    -1,   702,    -1,   704,    -1,   706,    -1,
     708,    -1,   710,    -1,    -1,   713,    -1,   715,   716,    -1,
      -1,   719,    -1,    -1,   722,    -1,    -1,    -1,    -1,   727,
      -1,    -1,    -1,   731,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   741,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,
     108,   109,   110,   111,   112,   113,   761,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,    -1,    -1,   141,   142,   143,   144,   145,   793,    -1,
      -1,   796,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     805,    -1,    -1,    -1,    -1,    -1,    -1,   812,    -1,    -1,
     815,    -1,    -1,   821,    -1,   823,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   831,    -1,    -1,   834,    -1,    -1,    -1,
      -1,   839,    -1,    -1,   842,    -1,   844,    -1,   846,    -1,
     848,   849,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   861,    -1,   863,    -1,   865,    -1,    -1,
      -1,   869,   870,   871,    -1,    -1,   874,   875,    -1,   877,
     878,    -1,   880,    -1,    -1,   883,    -1,    -1,   886,   887,
      -1,    -1,   890,   891,   892,    -1,    -1,   895,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   903,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   913,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     945,    -1,    -1,    -1,    -1,    -1,    -1,   955,    -1,   954,
     958,   959,   960,    -1,    -1,   963,    -1,    -1,   966,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   195,    -1,   197,   977,
     199,    -1,    -1,   202,   982,   983,   984,    -1,   986,   987,
     988,   989,    -1,   991,    -1,   993,    -1,    -1,    -1,    -1,
      -1,   999,  1000,  1001,    -1,  1003,    -1,  1005,    -1,    -1,
    1008,    -1,    -1,    -1,  1012,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1022,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1050,  1054,    -1,  1056,    -1,
    1058,    -1,    -1,  1061,  1062,  1063,    -1,    -1,  1066,  1067,
      -1,    -1,    -1,  1071,  1072,    -1,  1074,    -1,    -1,  1077,
      -1,  1079,    -1,    -1,  1082,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1090,    -1,    -1,    -1,    -1,  1095,    -1,  1097,
    1098,   320,    -1,    -1,    -1,    -1,    -1,  1102,    -1,  1104,
      -1,    -1,  1107,    -1,    -1,    -1,    -1,    -1,   337,    -1,
      -1,    -1,    -1,    -1,    -1,   344,    -1,   346,   347,  1124,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   362,    -1,  1143,  1144,    -1,  1146,  1147,
      -1,  1149,  1150,    -1,    -1,    -1,    -1,  1155,  1156,    -1,
    1158,    -1,    -1,    -1,    -1,    -1,    -1,  1165,  1166,    -1,
      -1,    -1,    -1,  1168,  1169,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1180,    -1,    -1,    -1,    -1,
    1188,  1189,  1190,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1198,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   440,    -1,    -1,    -1,  1223,  1224,    -1,    -1,    -1,
      -1,    -1,   451,    -1,  1232,  1233,  1234,  1235,  1236,  1237,
     459,    -1,  1240,    -1,  1242,    -1,  1244,    -1,   467,    -1,
      -1,  1249,    -1,    -1,   473,   474,    -1,    -1,   477,    -1,
      -1,   480,    -1,    -1,    -1,    -1,    -1,    -1,   487,    -1,
      -1,   490,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1281,    -1,  1283,    -1,    -1,   507,    -1,
      -1,    -1,  1290,    -1,  1292,    -1,    -1,   516,    -1,  1297,
      -1,    -1,    -1,  1301,    -1,    -1,    -1,  1305,    -1,    -1,
     529,    -1,  1310,    -1,   533,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     549,    -1,    -1,    -1,    -1,    -1,   555,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1352,    -1,    -1,  1355,  1356,    -1,
      -1,    -1,  1360,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   600,    -1,    -1,    -1,    -1,    -1,   606,    -1,   608,
      -1,    -1,    -1,   612,    -1,    -1,    -1,    -1,    -1,   618,
      -1,    -1,    -1,    -1,    -1,    -1,   625,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   650,    -1,    -1,   653,   654,   655,   656,    -1,    -1,
      -1,    -1,    -1,    -1,   663,    -1,    -1,    -1,    -1,    -1,
     669,    -1,   671,    -1,   673,   674,   675,   676,   677,   678,
     679,    -1,    -1,    -1,    -1,   684,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   750,    -1,   752,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   814,    -1,    -1,   817,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   833,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   851,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     859,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   868,
      -1,    -1,    -1,    -1,   873,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   885,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   902,    -1,   904,   905,   906,   907,    -1,
      -1,    -1,    -1,   912,    -1,    -1,   915,    -1,    -1,    -1,
     919,   920,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   948,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   981,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   996,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1006,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1020,  1021,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1038,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1048,
      -1,    -1,    -1,  1052,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1083,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1108,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1123,    -1,  1125,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1163,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1176,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1184,    -1,  1186,  1187,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1230,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1238,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1254,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1262,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1270,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1317,  1318,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1358,
      -1,    -1,  1361,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1384
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
     429,   430,   431,   433,   435,   436,   437,   438,   442,   447,
     450,   453,   456,   466,   474,   476,   479,   481,   491,   493,
     494,   497,   499,   500,     0,   158,   159,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     146,   204,   240,   204,   310,   204,   337,   337,   204,   344,
     204,   353,   337,   204,   373,   337,   353,   337,   353,   337,
     353,   337,   353,   204,   439,   204,   467,   332,   333,   334,
     335,   358,   384,   385,   395,   399,   403,   429,   437,   442,
     447,   450,   453,   456,   475,   477,   478,   204,   482,   482,
     482,    40,   483,   484,   162,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   354,   203,   354,   203,   354,
     203,   203,   406,   203,   203,   160,   161,   202,   203,   203,
     203,   203,   203,   201,   147,   148,   150,   151,   152,   153,
     154,   167,   168,   169,   171,   173,   174,   177,   179,   180,
     204,   241,    61,   311,   313,    19,    28,    56,   304,   309,
     320,   321,   340,   351,   361,   363,   364,   365,   366,   367,
     432,   434,   320,   340,   432,   434,   171,   345,   341,   360,
     361,   363,   364,   365,     3,    25,   374,   375,    70,   307,
     309,   320,   386,   393,   443,   444,   445,   446,    23,   237,
     355,   356,   202,    71,   397,   448,   449,   202,    72,   401,
     451,   452,   202,    73,   405,   454,   455,    50,    51,   237,
     408,   409,   411,   412,   202,    64,   440,   441,   331,   332,
     333,   358,   383,   392,   395,   430,   431,   433,   437,   447,
     457,   464,   465,   468,   469,   470,   499,    74,    75,   471,
     472,   483,   483,   483,    41,   485,   486,   166,   179,   149,
     167,   178,   171,   203,   238,   203,   305,   308,   201,   201,
     203,   201,   203,   201,   201,   203,   203,   203,   203,   203,
     203,   203,   354,   203,   203,   201,   201,   203,   203,   203,
     201,   203,   258,   203,   201,   203,   203,   201,   203,   203,
     201,   203,   203,   201,   258,   258,   201,   203,   258,   201,
     203,   201,   201,   202,   203,   203,   201,   203,   203,   203,
     203,   201,   202,   172,   175,   176,   180,   179,    32,    33,
     168,   204,   242,   243,   244,   246,   247,   202,    62,   311,
     316,   317,   341,   202,   202,   204,   314,   310,   351,    57,
     322,   323,     6,    16,    81,   267,   268,   270,   271,   273,
     336,   347,   348,   204,   368,   368,    48,   414,   417,   414,
     351,   336,   414,   414,   171,   346,   202,   370,     9,   371,
     377,   379,   204,   376,   310,   393,   273,   387,   414,   368,
     237,   237,   204,   357,   267,   414,   368,   267,   414,   368,
     267,   414,   368,   179,   183,   185,   204,   410,   408,   204,
     413,   414,   368,   337,   403,   453,   460,   383,   399,   442,
     450,   458,   459,   460,   462,   499,   179,   473,   485,    25,
     419,   420,   485,    47,   501,   504,   166,   179,   203,   203,
     201,   201,   305,   308,   203,   306,   201,   203,   203,   312,
     203,   203,   203,   325,   201,   201,   202,   203,   201,   203,
     203,   203,   201,   203,   203,   202,   203,   203,   203,   342,
     203,   202,   203,   258,   201,   258,   203,   203,   201,   202,
     203,   203,   258,   258,   202,   202,   203,   203,   202,   203,
     203,   202,   203,   203,   154,   156,   182,   184,   188,   191,
     202,   258,   202,   203,   203,   203,   203,   461,   461,   461,
     203,   463,   203,   206,   167,   202,   203,   203,   201,   203,
     206,   201,   202,   243,   246,   204,   245,   204,   248,   237,
     319,   168,   318,   316,   237,   315,   202,   311,   336,    58,
      59,   326,   328,   202,   179,   324,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     274,   275,   276,   267,   204,   349,   320,   340,   320,   340,
     267,   204,   418,   267,   336,   267,   267,     7,    11,   237,
     259,   263,   202,   267,   371,   204,   380,   202,   311,   387,
     274,   267,   320,   393,   267,   397,   267,   401,   267,   405,
     179,   186,   179,   192,   267,   360,    26,    29,    30,    31,
      53,    54,    55,   207,   209,   210,   211,   212,   213,   214,
     215,   218,   219,   220,   222,   223,   228,   230,   233,   234,
     237,   249,   250,   460,   207,   460,   462,   207,   179,   419,
      38,   379,   415,   422,   426,   427,   204,   421,    45,   498,
     202,   482,   203,   202,   202,   306,   202,   306,   312,   203,
     202,   201,   167,   202,   203,   203,   203,   203,   203,   203,
     362,   203,   362,   202,   203,   202,   202,   202,   202,   258,
     258,   201,   202,   203,   203,   202,   203,   202,   203,   202,
     202,   202,   202,   202,   202,   202,   184,   182,   155,   157,
     180,   181,   189,   192,   197,   198,   199,   202,   203,   203,
     201,   203,   208,   201,   203,   208,   201,   203,   208,   201,
     203,   208,   201,   203,   208,   201,   208,   201,   461,   461,
     463,   203,   201,   203,   492,   201,   203,   327,   201,   203,
     246,   311,   204,   329,   179,   150,   151,   152,   277,   277,
     277,   170,   172,   350,   351,     7,    12,   257,   262,   347,
     369,   202,   351,   202,   419,   204,   265,   170,   381,   311,
     277,   393,   179,   187,   192,   179,   193,   181,   200,   370,
     211,   237,   249,   214,   218,   237,   249,   204,   216,   222,
     228,   233,   204,   221,   228,   233,   168,   224,   233,   168,
     231,   183,   204,   235,   146,   205,   415,   380,    43,   207,
     422,   426,   489,   490,   202,   380,   237,   202,   370,   414,
     479,   493,   495,   419,   327,   258,   258,   258,   203,   258,
     171,   202,   362,   201,   203,   362,   203,   203,   203,   378,
     258,   202,   184,   200,   181,   190,   197,   193,   202,   208,
     203,   208,   202,   203,   208,   203,   208,   208,   202,   203,
     208,   208,   203,   208,   203,   203,   202,   203,   480,   203,
     492,   492,   206,   201,   203,   202,   203,   202,   202,   202,
     202,   257,   202,   265,   369,   202,   415,   171,   266,     7,
      11,    12,    13,   255,   256,   382,   202,   202,   179,   194,
     195,   217,   219,   222,   228,   233,   228,   233,   233,   233,
     168,   225,   168,   232,   183,   204,   236,    42,   207,   422,
     426,   487,   488,   489,   202,   381,   318,   204,   428,   203,
     379,   426,   502,   505,   203,   203,   203,   258,   203,   203,
     416,   258,   201,   258,   203,   167,   197,   202,   203,   203,
     203,   203,   208,   208,   208,   208,   203,   203,   229,   202,
     480,   480,   203,   206,   201,   206,   203,   202,   327,   498,
     201,   203,   503,     8,   278,   281,   279,   281,   280,   281,
     266,   237,   422,   426,   202,   202,   265,   278,   179,   196,
     197,   222,   228,   233,   228,   233,   233,   233,   168,   226,
     259,   202,   489,   166,     7,    11,    12,    13,    14,    69,
     423,   424,   425,   202,   201,   380,   207,   426,   505,   202,
     203,   269,   201,   206,   201,   203,   272,   201,   258,   327,
     416,   416,   203,   203,   388,   197,   203,   203,   203,   208,
     203,   206,   202,   327,   201,   495,   203,   503,   503,     7,
      11,    12,    13,    14,    69,    88,   207,   251,   252,   253,
     254,   260,   264,   278,   302,   204,   282,   282,   207,   280,
     282,   202,   266,    36,   207,   302,   389,   390,   228,   233,
     233,   233,   168,   227,   202,   265,   202,   381,   201,   201,
     269,   206,   203,   203,   272,   203,   258,   203,   206,   206,
     201,   203,   202,   203,   261,   203,   496,   258,   265,   265,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     283,   286,   295,   295,   106,   114,   139,   140,   288,   291,
     295,   202,   389,   204,   391,   233,   266,   202,   207,   489,
     498,   202,   202,   203,   261,   203,   203,   203,   203,   202,
     203,   203,   293,   203,   203,   203,   203,   202,   203,   206,
     327,   258,   206,   201,   266,   202,    20,    22,   237,   252,
     284,   296,   297,   300,   301,   284,    21,   237,   252,   285,
     298,   299,   300,   285,   237,   252,   287,   300,   237,   260,
     294,   300,   202,   237,   289,   296,   300,   284,   237,   290,
     298,   300,   290,   237,   292,   300,   202,   495,   258,   258,
     258,   203,   258,   201,   203,   258,   201,   258,   258,   203,
     258,   201,   203,   258,   258,   258,   203,   258,   258,   258,
     203,   258,   258,   203,   258,   258,   258,   203,   258,   258,
     258,   258,   202,   202,   252,   300,   168,   252,   179,   252,
     300,   168,   252,   252,   260,   300,   300,   496,   258,   203,
     258,   203,   258,   202,   258,   203,   258,   202,   258,   258,
     258,   258,   258,   252,   257,   252,   258,   202,   258
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   163,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   165,   165,   165,   165,   165,   165,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   167,   168,   168,   169,   170,   171,   171,   172,   172,
     173,   174,   175,   176,   177,   177,   178,   178,   179,   179,
     179,   179,   180,   180,   181,   182,   183,   183,   183,   184,
     184,   185,   186,   187,   188,   189,   189,   190,   190,   191,
     192,   193,   194,   194,   194,   195,   196,   197,   197,   198,
     199,   199,   200,   200,   201,   201,   202,   202,   203,   204,
     205,   206,   206,   207,   207,   207,   208,   208,   208,   209,
     209,   210,   210,   210,   211,   211,   211,   211,   212,   213,
     214,   215,   216,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   229,   230,   231,   232,   233,   234,
     235,   235,   236,   236,   237,   238,   238,   238,   238,   238,
     238,   238,   239,   240,   241,   241,   242,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   251,   252,   253,
     254,   254,   254,   254,   254,   255,   256,   256,   256,   256,
     257,   258,   258,   259,   260,   261,   261,   262,   262,   263,
     263,   264,   264,   265,   266,   267,   267,   267,   268,   269,
     269,   269,   269,   270,   271,   272,   272,   272,   273,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   275,   275,
     275,   276,   276,   277,   277,   277,   278,   279,   280,   281,
     282,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     284,   284,   284,   284,   284,   284,   284,   284,   285,   285,
     285,   285,   285,   285,   285,   285,   286,   286,   287,   287,
     287,   287,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   289,   289,   289,   289,   290,   290,   290,   290,   291,
     291,   292,   292,   293,   293,   294,   294,   294,   294,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   303,   304,   305,   305,   306,   306,   307,
     308,   308,   309,   310,   311,   312,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   324,
     324,   325,   325,   326,   327,   327,   328,   328,   329,   330,
     330,   330,   331,   331,   332,   333,   334,   335,   336,   336,
     337,   338,   338,   339,   339,   340,   340,   341,   342,   342,
     342,   343,   343,   344,   345,   346,   347,   348,   348,   349,
     350,   350,   351,   352,   352,   352,   353,   354,   354,   354,
     354,   355,   356,   357,   358,   359,   359,   360,   360,   360,
     360,   361,   362,   362,   362,   362,   363,   364,   365,   366,
     367,   368,   369,   370,   371,   371,   372,   373,   374,   375,
     375,   376,   377,   378,   378,   379,   380,   381,   382,   383,
     383,   384,   385,   386,   386,   387,   388,   388,   388,   388,
     388,   389,   390,   391,   392,   392,   393,   394,   394,   394,
     395,   396,   396,   397,   398,   398,   399,   400,   401,   402,
     402,   403,   404,   405,   406,   406,   406,   406,   406,   407,
     407,   408,   409,   410,   410,   411,   412,   413,   414,   415,
     415,   416,   416,   416,   416,   417,   418,   419,   420,   421,
     422,   423,   424,   425,   425,   425,   425,   425,   425,   426,
     427,   428,   429,   429,   429,   430,   430,   431,   432,   432,
     433,   434,   434,   435,   436,   437,   438,   439,   440,   441,
     442,   443,   443,   444,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   457,   458,   458,
     458,   458,   459,   459,   460,   460,   461,   461,   461,   462,
     462,   463,   463,   463,   463,   464,   465,   466,   467,   468,
     468,   468,   468,   468,   468,   468,   468,   468,   468,   468,
     469,   470,   471,   472,   472,   473,   473,   473,   474,   475,
     475,   476,   477,   477,   477,   477,   477,   477,   477,   477,
     477,   477,   477,   477,   477,   477,   478,   478,   478,   478,
     478,   478,   478,   479,   480,   480,   480,   480,   480,   480,
     480,   481,   482,   483,   484,   485,   486,   487,   488,   489,
     490,   491,   492,   492,   492,   492,   492,   493,   494,   495,
     495,   495,   495,   496,   496,   496,   496,   497,   498,   499,
     500,   501,   502,   502,   503,   503,   503,   503,   504,   505
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     1,     1,     0,     1,
       3,     1,     1,     2,     2,     2,     0,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     4,     6,     0,
       1,     1,     1,     1,     3,     3,     1,     2,     1,     1,
       1,     1,     3,     4,     2,     1,     1,     1,     1,     1,
       3,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     0,     2,     1,     2,     1,     0,     3,     2,     1,
       1,     3,     2,     1,     1,     3,     4,     3,     6,     1,
       4,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     5,     5,     5,     5,     7,     2,     2,
       2,     2,     4,     4,     4,     4,     4,     4,     6,     6,
       6,     6,     8,     4,     1,     1,    10,     1,     1,     1,
       1,     1,     7,     0,     2,     1,     1,     1,     6,     1,
       1,     1,     1,     1,     7,     0,     2,     4,     6,     2,
       4,     2,     1,     1,     1,     1,     1,     1,     4,     1,
       1,     4,     1,     1,     4,     1,     1,     1,     1,     7,
       1,     1,     1,     1,     1,     7,     1,     1,     1,     1,
       7,     0,     3,     7,     5,     0,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,    10,     0,
       3,     3,     2,    10,    10,     0,     3,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     6,     7,     6,     1,
       1,     1,     1,     3,     1,     3,     1,     3,     1,     3,
       2,     4,     6,     4,     2,     4,     2,     2,     2,     4,
       6,     4,     2,     4,     2,     2,     1,     3,     2,     2,
       4,     2,     1,     1,     3,     1,     3,     1,     3,     1,
       3,     2,     4,     2,     2,     2,     4,     2,     2,     1,
       3,     2,     2,     0,     2,     2,     2,     4,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     6,     1,     4,     1,     4,
       1,     1,     1,     1,     7,     5,     3,     0,     3,     7,
       3,     3,     1,     1,     5,     0,     3,     1,     1,     1,
       4,     1,     1,     1,     5,     1,     4,     1,     1,     2,
       3,     0,     2,     5,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     8,    10,     8,    10,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     9,     0,     3,
       3,     1,     1,     1,     1,     1,     6,     1,     1,     1,
       4,     2,     7,     1,     1,     1,     1,     0,     2,     3,
       5,     4,     1,     1,    10,     1,     1,     1,     1,     1,
       1,     7,     0,     2,     4,     2,     9,     7,     9,     1,
       1,     1,     1,     8,     2,     3,     1,     1,     5,     1,
       1,     1,     7,     0,     3,     1,     1,     1,     1,     1,
       1,     8,    10,     1,     1,    10,     0,     3,     5,     3,
       2,     5,     1,     1,     1,     1,     5,     1,     1,     1,
       8,     1,     1,     5,     1,     1,     8,     1,     5,     1,
       1,     8,     1,     5,     0,     3,     5,     3,     3,     1,
       1,     4,     1,     1,     1,     4,     1,     1,     9,     1,
       1,     0,     3,     3,     3,     1,     1,     5,     1,     1,
       9,     1,     5,     1,     1,     1,     1,     1,     1,     7,
       1,     1,     1,     1,     1,     1,     1,    10,     1,     1,
      10,     1,     1,    10,    10,    10,     1,     1,     8,     1,
      10,     1,     1,     6,     8,     1,    10,     6,     1,    10,
       6,     1,    10,     6,     1,     6,     3,     1,     2,     2,
       2,     2,     1,     1,     1,     1,     0,     3,     2,     1,
       1,     0,     3,     3,     2,     4,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     1,     1,     1,     2,     3,     4,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,    13,     0,     3,     3,     3,     5,     3,
       2,     1,     1,     4,     1,     4,     1,     4,     1,     4,
       1,    11,     0,     3,     3,     3,     2,     1,    19,     1,
       1,     1,     1,     0,     6,     3,     2,     1,     1,     9,
       1,     9,     1,     1,     0,     3,     3,     2,     1,     7
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
