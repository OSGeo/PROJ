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
#define YYLAST   3356

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  163
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  349
/* YYNRULES -- Number of rules.  */
#define YYNRULES  700
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1417

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
    1027,  1029,  1031,  1035,  1041,  1042,  1043,  1045,  1047,  1049,
    1053,  1053,  1055,  1057,  1062,  1063,  1065,  1067,  1069,  1071,
    1075,  1075,  1077,  1083,  1090,  1090,  1093,  1100,  1101,  1102,
    1103,  1104,  1106,  1110,  1112,  1114,  1114,  1118,  1123,  1123,
    1123,  1127,  1132,  1132,  1134,  1138,  1138,  1142,  1147,  1149,
    1153,  1153,  1157,  1162,  1164,  1168,  1169,  1170,  1171,  1172,
    1174,  1174,  1176,  1179,  1181,  1181,  1183,  1185,  1187,  1191,
    1197,  1197,  1199,  1200,  1201,  1202,  1204,  1206,  1210,  1215,
    1217,  1220,  1225,  1229,  1235,  1235,  1235,  1235,  1235,  1235,
    1239,  1244,  1246,  1251,  1251,  1252,  1254,  1254,  1256,  1263,
    1263,  1265,  1272,  1272,  1274,  1281,  1288,  1293,  1294,  1296,
    1302,  1307,  1315,  1321,  1323,  1325,  1330,  1332,  1332,  1333,
    1333,  1337,  1343,  1343,  1345,  1348,  1352,  1357,  1363,  1366,
    1371,  1377,  1380,  1385,  1391,  1394,  1399,  1404,  1405,  1408,
    1409,  1410,  1412,  1414,  1414,  1416,  1416,  1418,  1419,  1420,
    1422,  1422,  1424,  1425,  1426,  1427,  1429,  1431,  1433,  1435,
    1437,  1437,  1437,  1438,  1438,  1438,  1440,  1440,  1440,  1440,
    1442,  1444,  1446,  1450,  1453,  1453,  1456,  1457,  1458,  1460,
    1464,  1465,  1467,  1469,  1469,  1470,  1470,  1471,  1471,  1471,
    1472,  1473,  1473,  1474,  1474,  1475,  1475,  1477,  1477,  1478,
    1478,  1479,  1480,  1480,  1484,  1491,  1492,  1493,  1494,  1495,
    1496,  1497,  1499,  1501,  1503,  1505,  1507,  1509,  1511,  1513,
    1515,  1517,  1522,  1529,  1530,  1531,  1532,  1533,  1535,  1540,
    1548,  1548,  1548,  1548,  1550,  1551,  1552,  1553,  1555,  1557,
    1562,  1568,  1570,  1577,  1577,  1579,  1580,  1581,  1582,  1584,
    1586
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
  "parameter_or_parameter_file",
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

#define YYPACT_NINF -1161

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-1161)))

#define YYTABLE_NINF -654

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     845, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161,
   -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161,
   -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161,
   -1161, -1161, -1161, -1161, -1161, -1161, -1161,   114, -1161, -1161,
   -1161,   353, -1161, -1161, -1161,   353, -1161, -1161, -1161, -1161,
   -1161, -1161,   353,   353, -1161,   353, -1161,   353, -1161,   353,
   -1161,   353, -1161, -1161, -1161,   353, -1161,   353, -1161,   353,
   -1161,   353, -1161,   353, -1161,   353, -1161,   353, -1161,   353,
   -1161, -1161, -1161, -1161, -1161, -1161, -1161,   353, -1161, -1161,
   -1161, -1161, -1161,   353, -1161,   353, -1161,   353, -1161,   353,
   -1161,   353, -1161,   353, -1161, -1161, -1161,   -16,   -16,   -16,
     -16,   -16,   -16,   -16,   -16,   -16,   -16,   -16,   -16,   -16,
     -16,   -16,   -16,   -16,   -16,   973,   -16,   -16,   -16,   111,
   -1161, -1161,    20, -1161,    20, -1161,    20,    20, -1161,    20,
   -1161,    20,    20, -1161,    20,    20,    20,    20,    20,    20,
      20,    20,    20, -1161,    20, -1161,    20, -1161, -1161, -1161,
   -1161,    26, -1161, -1161, -1161, -1161, -1161,    77,    91,   110,
   -1161, -1161, -1161, -1161,   415, -1161,    20, -1161,    20,    20,
      20, -1161,    20,   353, -1161,  1128,   238,   434,   434,   830,
     343,   363,   254,   422,   160,   415,   384,   415,   374,   415,
      86,   401,   415,   287,  1022, -1161, -1161, -1161,   516,   111,
     111,   111,   325,   973, -1161, -1161, -1161, -1161, -1161, -1161,
   -1161,   519, -1161, -1161, -1161, -1161,   234,   263,   280,   830,
   -1161,    20, -1161,    20,   353, -1161, -1161, -1161, -1161,   353,
      20,   353,    20, -1161,   353,   353,    20,    20, -1161, -1161,
   -1161, -1161,    20,    20,    20,    20, -1161,    20,    20,    20,
   -1161, -1161, -1161, -1161,   353,   353, -1161, -1161,    20,   353,
   -1161, -1161,   353,    20,    20, -1161,    20, -1161, -1161,   353,
   -1161,    20,    20,   353, -1161, -1161,    20,    20,   353, -1161,
   -1161,    20,    20,   353, -1161, -1161,    20,    20,   353, -1161,
   -1161,    20,    20,   353,    20,   353, -1161, -1161,    20,   353,
   -1161, -1161, -1161, -1161, -1161,   353, -1161, -1161, -1161, -1161,
   -1161, -1161,   415, -1161,    20,    20, -1161, -1161, -1161, -1161,
   -1161, -1161,   353,    20,    20,    20, -1161,    20,   353,   415,
   -1161,   443,   519, -1161, -1161,   501,   415,   352,   415,   415,
     -16,   -16,   128,   377,   140,   -16,   -16,   395,   395,   128,
     140,   395,   395,   830,   415,   432,   -16,   -16,   337,   415,
     -16,   -16,    93,   450,   395,   -16,   476, -1161,   476,   -16,
     450,   395,   -16,   450,   395,   -16,   450,   395,   -16, -1161,
   -1161,   768,   123, -1161,   -16,   395,   -16,   -16, -1161,   449,
     197,   519,   325,   512,   325,   495,   973, -1161,   519, -1161,
   -1161, -1161, -1161, -1161, -1161, -1161, -1161,    20,    20,   353,
   -1161,   353, -1161, -1161,    20,    20,   353,    20, -1161, -1161,
   -1161,    20,    20,    20, -1161,    20,   353, -1161, -1161, -1161,
   -1161, -1161, -1161, -1161,   353,   415,    20,   353, -1161,    20,
      20, -1161,    20,   353,    20,    20,   415,    20,    20, -1161,
      20, -1161,    20,    20,    20, -1161,    20,    20,   353, -1161,
   -1161,    20,    20,    20,   353,   415,    20,    20,    20,    20,
   -1161,   415,   415,    20,    20,   415,    20,    20,   415,    20,
      20, -1161, -1161,   243, -1161,   415,    20, -1161,   415,    20,
      20,    20, -1161, -1161,    20, -1161, -1161, -1161, -1161, -1161,
      20,    20,    20,    20,   280,   415,    20, -1161,    20,   353,
      20, -1161,    20,   353,   415, -1161,   514,   537,   -16,   -16,
   -1161, -1161,   476, -1161,  1232,   523,   476,   415,   238,   140,
     566,   415,   519,  1541, -1161,   450,   -16,   108,   108,   450,
     -16,   450,   140, -1161,   450,   450,   379,   415,   450,   108,
     108, -1161, -1161,   -16,   415,   238,   450,  1414, -1161,   450,
     398, -1161, -1161, -1161, -1161,   450,   100, -1161,   450,   269,
   -1161,   450,    94, -1161, -1161,   519, -1161, -1161,   519, -1161,
   -1161, -1161,   450,   363,   672,  1278, -1161, -1161, -1161,   605,
   -1161,  1509, -1161,   519, -1161,   512,   334,   -16,   536,   415,
     -16, -1161,    20, -1161, -1161,   415, -1161,   415, -1161,    20,
   -1161,   415,    20, -1161,    20, -1161,    20,   415, -1161, -1161,
   -1161,   353, -1161,   280,   415, -1161, -1161, -1161, -1161, -1161,
   -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161,    20,
      20,    20, -1161, -1161,    20,    20,    20,    20,    20,   415,
   -1161,    20,   415,   415,   415,   415, -1161, -1161,    20,    20,
     353, -1161,   415,    20,    20,    20,    20, -1161,    20, -1161,
      20,   415,    20,   415,    20,   415,   415,   415,   415,   415,
     415,   415,   335,   455, -1161,   590,   415, -1161, -1161, -1161,
   -1161,    20, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161,
   -1161, -1161, -1161,    20,   353,    20,   353, -1161,    20,   353,
      20,   353,    20,   353,    20,   353,    20, -1161,   353,    20,
   -1161,    20,    20, -1161, -1161,    20, -1161,   353,    20, -1161,
   -1161,   353, -1161,    20, -1161,   353, -1161,    20,   537, -1161,
   -1161, -1161, -1161, -1161, -1161,   165, -1161,   -16,   519, -1161,
     324,   324,   324,   443,   128,   143,   415,   128,   415, -1161,
     512, -1161, -1161, -1161, -1161, -1161, -1161,   -16, -1161,   128,
     148,   415,   128,   415,   443,   571, -1161,   324, -1161,    93,
   -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161,   519, -1161,
   -1161,   519,   519, -1161,   459, -1161, -1161, -1161, -1161,   432,
     116,   598,   800, -1161,   -16,   414, -1161,   -16,   216, -1161,
    1232,   146, -1161,  1232,   413, -1161,   768, -1161,   515, -1161,
   -1161, -1161,   334,   -16,  1092,   415,   -16,   476,   415,   323,
     512, -1161,    20, -1161,    20, -1161, -1161, -1161, -1161,    20,
      20,    20,    20,   830,   415,    20, -1161, -1161,    20, -1161,
      20, -1161,    20,    20, -1161, -1161, -1161,   353,    20, -1161,
   -1161,    20, -1161, -1161,    20,    20,    20,   415, -1161,   522,
     459, -1161,   590,   519, -1161,   415, -1161,    20, -1161,    20,
   -1161,    20, -1161, -1161,   415,    20,    20,    20, -1161,   415,
      20,    20, -1161,    20,    20, -1161,    20, -1161, -1161,    20,
   -1161,   415,    20,    20, -1161, -1161,    20,    20,    20,   353,
   -1161,    20, -1161, -1161, -1161, -1161, -1161, -1161,   415,    20,
     415,   415,   415,   415,   178, -1161, -1161, -1161,   415,   415,
     259,   415,   830,   415,   -16,   503,   415,   888,   415,   415,
   -1161, -1161, -1161,   519, -1161, -1161, -1161, -1161, -1161,   699,
   -1161, -1161,   216, -1161,   146, -1161, -1161, -1161,   146, -1161,
   -1161,  1232, -1161,  1232,   768, -1161,   975,   415,   443, -1161,
   -1161, -1161,  1232,   -16,    20,   334, -1161,    20,    20,    20,
      20, -1161, -1161,    20,    20,    20, -1161, -1161,    20, -1161,
      20, -1161, -1161, -1161, -1161, -1161, -1161, -1161,   353,    20,
   -1161,    20, -1161, -1161,   922,   415,    20,    20,    20, -1161,
      20,    20,    20,    20, -1161,    20, -1161,    20, -1161, -1161,
     415, -1161, -1161,    20,    20,    20,   353,    20, -1161,    20,
     415, -1161,    20,   536,   353, -1161,    20, -1161,   667,   667,
     667, -1161, -1161, -1161, -1161,   415,   830,   -16, -1161,   667,
     794, -1161, -1161,   417,   585,   628,   146, -1161, -1161, -1161,
   -1161,  1232,   452,   415, -1161, -1161, -1161,  1334, -1161,   973,
   -1161,   593, -1161,   415,   353,   -16,  1131,   415, -1161,    20,
     353,    20,   353,    20,   353, -1161,    20,    20,    20,   402,
     794, -1161,    20,    20, -1161,    20, -1161, -1161,    20, -1161,
      20, -1161, -1161,    20,   415, -1161, -1161, -1161, -1161, -1161,
   -1161,    20, -1161,   353, -1161,   323,    20, -1161,    20,    20,
   -1161,   680, -1161,   -16, -1161,   -16,   752, -1161,   -16,   415,
     830,   943, -1161, -1161,   585,   628,   628, -1161,  1232, -1161,
   -1161,   415,   -16,   415,   443, -1161, -1161, -1161, -1161, -1161,
   -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161,   353, -1161,
     353,    20,    20, -1161,    20,    20, -1161,    20,    20, -1161,
      20, -1161, -1161,    20,    20,   353,    20, -1161, -1161, -1161,
   -1161,   415, -1161,    20,    20,    20,   -16,   -16, -1161, -1161,
    1302,  1341, -1161,  1487,   415,   812, -1161, -1161,   -16,   628,
   -1161,   830,   415,  1208,   415,   415,    20,    20,    20, -1161,
   -1161, -1161, -1161, -1161, -1161, -1161,    20, -1161, -1161, -1161,
   -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161,
   -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161,
   -1161,    20,    20, -1161, -1161, -1161, -1161, -1161,   415, -1161,
      20,    20,    20,    20,    20,    20,   415, -1161,    20, -1161,
      20, -1161,    20, -1161,    20, -1161, -1161,    20,   353, -1161,
   -1161,   830,   415,   560,   560,   581,   581, -1161,   701,   144,
     415,   499,   560,   509,   509, -1161,   518, -1161,   415, -1161,
   -1161,   323,    20, -1161, -1161, -1161,    20,    20, -1161,    20,
     353,    20,   353, -1161, -1161,    20,    20, -1161,    20,   353,
      20, -1161,    20,    20, -1161,    20,    20,    20, -1161,    20,
   -1161,    20, -1161,    20,    20, -1161,    20, -1161,    20,    20,
   -1161,    20, -1161,    20, -1161,   415,   415, -1161, -1161,   701,
   -1161,  1232,   708, -1161,   519, -1161, -1161,   701, -1161,  1232,
     708, -1161, -1161, -1161,   708, -1161, -1161, -1161,   131, -1161,
   -1161,   518, -1161, -1161, -1161,   518, -1161, -1161, -1161, -1161,
      20, -1161,    20,    20,    20,    20,   415,    20,    20,   415,
      20,    20,    20,    20,    20, -1161, -1161,   708, -1161,   503,
   -1161, -1161, -1161,   708, -1161, -1161, -1161, -1161, -1161, -1161,
   -1161,    20,   415,    20, -1161, -1161, -1161
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   423,   412,   401,   411,   172,   435,   457,   403,   485,
     488,   608,   662,   688,   691,   510,   503,   362,   563,   495,
     492,   500,   498,   632,   678,   402,   425,   436,   404,   424,
     486,   490,   489,   511,   496,   493,   501,     0,     4,     5,
       2,     0,    13,   352,   353,     0,    18,   391,   389,   390,
     392,   393,     0,     0,     3,     0,    12,     0,    19,     0,
      11,     0,    20,   470,   471,     0,    14,     0,    21,     0,
      15,     0,    22,     0,    16,     0,    23,     0,    17,     0,
      24,   545,   543,   544,   546,   547,    25,     0,    26,    27,
      28,    29,    30,     0,     7,     0,     8,     0,     9,     0,
      10,     0,     6,     0,     1,    84,    85,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      89,   173,     0,   363,     0,   400,     0,     0,   413,     0,
     426,     0,     0,   458,     0,     0,   427,     0,   427,     0,
     427,     0,   505,   564,     0,   609,     0,   633,   647,   634,
     648,   635,   636,   650,   637,   638,   639,   640,   641,   642,
     643,   644,   645,   646,     0,   630,     0,   663,     0,     0,
       0,   665,     0,     0,    88,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    86,    87,   629,     0,     0,
       0,     0,     0,     0,    51,    31,    48,    49,    50,    52,
      53,     0,   174,    32,    33,    37,     0,    36,    46,     0,
     175,   165,   367,     0,     0,   449,   450,   375,   406,     0,
       0,     0,     0,   405,     0,     0,     0,     0,   549,   552,
     550,   553,     0,     0,     0,     0,   414,     0,   427,     0,
     437,   438,   439,   440,     0,     0,   461,   460,   454,     0,
     576,   475,     0,     0,     0,   474,     0,   572,   573,     0,
     432,   201,   428,     0,   487,   579,     0,     0,     0,   494,
     582,     0,     0,     0,   499,   585,     0,     0,     0,   517,
     513,   201,   201,     0,   201,     0,   504,   566,     0,     0,
     621,   616,   617,   611,   607,     0,   612,   622,   618,   619,
     614,   615,     0,   588,     0,     0,   610,   613,   620,   624,
     625,   631,     0,     0,     0,     0,   667,     0,     0,     0,
      45,    38,     0,    44,    34,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   429,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   509,
     508,     0,     0,   506,     0,     0,     0,     0,   586,     0,
       0,     0,     0,     0,     0,     0,     0,   664,     0,    40,
      42,    39,    47,   179,   182,   176,   177,   166,   169,     0,
     171,     0,   164,   371,     0,   357,     0,     0,   354,   359,
     368,   365,     0,     0,   377,   381,     0,   417,   228,   418,
     399,   215,   216,   217,     0,     0,     0,     0,   451,     0,
       0,   526,     0,     0,     0,     0,     0,     0,     0,   415,
     408,   422,     0,     0,     0,   466,   201,   454,     0,   453,
     462,   201,     0,     0,     0,     0,     0,     0,   201,   201,
     433,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    61,   514,    59,   515,     0,   201,   518,     0,     0,
       0,     0,   595,   596,   597,   593,   600,   594,   601,   587,
     597,   597,   602,    91,   626,     0,     0,   529,     0,     0,
       0,   699,    91,     0,     0,    43,     0,     0,     0,     0,
     356,   361,     0,   360,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   394,     0,     0,     0,     0,     0,
       0,     0,     0,   396,     0,     0,     0,     0,     0,     0,
       0,   455,   456,     0,     0,     0,     0,     0,   472,     0,
       0,   202,   430,   431,   491,     0,     0,   497,     0,     0,
     502,     0,     0,    55,    69,     0,    56,    60,     0,   512,
     507,   516,     0,     0,     0,     0,   606,   589,   590,     0,
     591,     0,   592,   627,   623,     0,     0,     0,     0,     0,
       0,   666,   167,   170,   180,     0,   183,     0,   373,   357,
     372,     0,   357,   369,   365,   364,     0,     0,   386,   387,
     382,     0,   374,   378,     0,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,     0,
       0,     0,   398,   419,     0,     0,   557,     0,   557,     0,
     527,     0,     0,     0,     0,     0,   210,   209,   201,   201,
       0,   407,     0,     0,   442,     0,   442,   467,     0,   459,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    59,     0,    70,     0,     0,   567,   568,   569,
     570,     0,   185,   111,   144,   147,   155,   159,   109,   599,
      93,    99,   100,   104,     0,    96,     0,   103,    96,     0,
      96,     0,    96,     0,    96,     0,    96,    95,     0,   597,
     605,   597,   602,    92,   628,     0,   541,     0,   673,   520,
     521,     0,   530,   384,   689,     0,   690,     0,     0,   178,
     181,   358,   370,   355,   366,     0,   395,     0,   379,   376,
       0,     0,     0,    38,     0,     0,     0,     0,     0,   548,
       0,   551,   397,   554,   555,   410,   409,     0,   434,     0,
       0,     0,     0,     0,    38,     0,   473,     0,   571,     0,
     574,   577,   578,   580,   581,   583,   584,    57,     0,    54,
      79,     0,     0,    64,    82,    66,    77,    78,   562,     0,
       0,     0,     0,   102,     0,     0,   128,     0,     0,   129,
       0,     0,   130,     0,     0,   131,     0,    94,     0,   598,
     604,   603,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   168,     0,   388,   384,   380,   243,   244,   245,   201,
     201,   201,   201,     0,     0,   557,   558,   556,   557,   560,
     522,   213,     0,   442,   208,   207,   452,     0,   443,   445,
     441,   442,   447,   468,   464,     0,   201,     0,    63,    59,
      82,    71,     0,     0,    81,     0,   107,    96,   105,     0,
     101,    96,    98,   112,     0,    96,    96,    96,   145,     0,
      96,    96,   148,     0,    96,   156,     0,   160,   161,     0,
      90,     0,   655,     0,   671,   677,   673,   673,    91,     0,
     672,     0,   385,   528,   682,   683,   680,   681,     0,     0,
       0,     0,     0,     0,     0,   421,    35,   416,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     575,    58,    80,     0,    65,    68,    83,   565,   106,     0,
      97,   110,     0,   132,     0,   133,   134,   143,     0,   135,
     136,     0,   137,     0,     0,   184,     0,     0,    38,   674,
     675,   676,     0,     0,     0,     0,   383,     0,     0,     0,
     201,   559,   561,   201,   522,   522,   519,   214,   201,   446,
       0,   444,   448,   199,   197,   196,   198,   469,     0,   201,
     463,     0,    75,    67,     0,     0,   113,   114,   115,   116,
      96,    96,    96,    96,   149,     0,   157,   153,   162,   163,
       0,   669,   661,   655,   655,    91,     0,    91,   654,     0,
       0,   542,   384,     0,     0,   694,   695,   693,     0,     0,
       0,   420,   525,   523,   524,     0,     0,     0,   465,     0,
       0,    74,   108,     0,     0,     0,     0,   138,   139,   140,
     141,     0,     0,     0,   158,   656,   657,     0,   658,     0,
     660,     0,   670,     0,     0,     0,     0,     0,   249,   219,
       0,    91,     0,   225,     0,   203,   201,     0,   477,    76,
       0,    72,   117,   118,   119,   120,   121,   122,    96,   150,
       0,   154,   152,    91,     0,   539,   534,   535,   536,   537,
     538,   384,   532,     0,   540,     0,     0,   698,   695,   695,
     692,     0,   218,     0,   223,     0,     0,   224,     0,     0,
       0,     0,   476,    73,     0,     0,     0,   142,     0,   659,
     668,     0,     0,     0,    38,   697,   696,   194,   191,   190,
     193,   211,   192,   212,   222,   351,   186,   188,     0,   187,
       0,   219,    91,   250,     0,     0,   227,   225,     0,   200,
     201,   483,   481,    91,    91,     0,   123,   124,   125,   126,
     151,     0,   531,   205,   684,   201,     0,     0,   221,   220,
       0,     0,   226,     0,     0,     0,   478,   480,     0,     0,
     146,     0,     0,     0,     0,     0,     0,   205,   252,   309,
     310,   311,   312,   313,   314,   315,   254,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   256,   258,   340,   341,   342,   343,   344,     0,   251,
     276,   303,   283,   285,   287,   289,     0,   282,   299,   195,
      91,   484,   384,   127,   201,   533,   687,    91,     0,   679,
     700,     0,     0,     0,     0,     0,     0,   246,     0,     0,
       0,     0,     0,     0,     0,   248,     0,   479,     0,   206,
     686,     0,   201,   204,   346,   350,   201,   201,   253,   201,
       0,   201,     0,   255,   348,   201,   201,   257,   201,     0,
     201,   259,   201,   201,   277,   201,   201,   201,   304,   201,
     247,   201,   284,   201,   201,   286,   201,   288,   201,   201,
     290,   201,   300,   201,   482,     0,     0,   260,   267,     0,
     264,     0,     0,   266,     0,   268,   275,     0,   272,     0,
       0,   274,   278,   281,     0,   279,   305,   308,     0,   306,
     291,     0,   293,   294,   295,     0,   297,   298,   301,   302,
     684,   189,   201,   201,     0,   201,     0,   201,   201,     0,
     201,   201,   201,   201,   201,   685,   263,     0,   261,     0,
     265,   349,   271,     0,   269,   347,   273,   280,   307,   292,
     296,   201,     0,   201,   262,   345,   270
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1161, -1161, -1161,  -208,  -216,  -183, -1161,   -78,  -188,   355,
   -1161, -1161, -1161, -1161, -1161, -1161,  -215,  -334,  -589,    -3,
    -716,  -584, -1161, -1161, -1161, -1161, -1161, -1161, -1161,  -526,
    -166, -1161, -1161, -1161,  -794, -1161, -1161,  -158,   -42,  1944,
    1070,   -41, -1161,  -483,  -557,  -613, -1161, -1161,   -86, -1161,
   -1161,   -80, -1161, -1161, -1161,   -73,  -213, -1161, -1161,  -755,
   -1161, -1161, -1161, -1161, -1161,  -700, -1161, -1161, -1161, -1161,
    -724, -1161, -1161, -1161,     0, -1161, -1161, -1161, -1161, -1161,
     222, -1161, -1161,  -431, -1161, -1161,  -423, -1161, -1161, -1038,
   -1161, -1161, -1161, -1161,  -920,  1375,  -317, -1160,  -461, -1161,
   -1161, -1161,  -821,  -956,   -31, -1161,  -410, -1161, -1161,  -415,
    -274,   196, -1161, -1161,  -301,  -935, -1161,  -372,  -930,  -610,
   -1161,  -899,  -521, -1161, -1161, -1161, -1161,  -527, -1161, -1161,
   -1161, -1161,  -704,  -519, -1161,  -652, -1161,  -673, -1161,  -367,
   -1161,   771,  -384,   -54,   775,  -406,    16,  -204,  -313,   155,
   -1161, -1161, -1161,   245, -1161,  -198, -1161,  -123, -1161, -1161,
   -1161, -1161, -1161, -1161,  -808, -1161, -1161, -1161,   587,     6,
       9,   668,   671,  -207,   -63, -1161, -1161,  -144,   126, -1161,
   -1161, -1161, -1161, -1161,  -520, -1161, -1161, -1161,    22, -1161,
     433,   -46, -1161, -1161, -1161,    11, -1161, -1161, -1161,  -551,
   -1161, -1161, -1161,   609,   611,   508,  -142,     4,   341, -1161,
   -1161, -1161, -1161, -1161, -1161, -1161,  -359,  -741,  -826, -1161,
    -120,   684,   686, -1161,   248, -1161,  -389, -1161, -1161,   615,
       8, -1161,    12, -1161,  -153, -1161,   -95, -1161,  -152, -1161,
     -99, -1161,  -176, -1161, -1161,   430, -1161, -1161, -1161, -1161,
   -1161,   813,    -9,  -349, -1161, -1161,  -516, -1161, -1161,  -712,
   -1161, -1161, -1161,  -771, -1161, -1161,   702,   624,   629,   644,
     631,   648, -1161, -1161,   246,  -537,   247,   251,   253,    13,
   -1161, -1161, -1161, -1161, -1161,   -77, -1161, -1161, -1161, -1161,
      14, -1161, -1161,   -93, -1161, -1161,   -97, -1161, -1161,   722,
   -1161, -1161, -1161,  -310,  -394,   252,   120, -1161, -1161, -1161,
   -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161, -1161,
   -1161, -1161,   856,  -370, -1161,  -107,   349, -1161,   215, -1161,
   -1161, -1161,  -927, -1161, -1161,  -212,   857, -1161,  -968,  -522,
   -1161,  -942,  -106, -1161, -1161, -1161,  -378, -1161,  -227
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   221,   620,   223,   873,   224,   853,
     225,   226,   409,   410,   227,   343,   228,   229,   883,   585,
     492,   586,   493,   693,   879,   587,   803,   954,   588,   804,
     882,  1013,  1014,  1100,   805,   806,   807,   884,   107,   207,
     376,   448,   911,   602,   733,   813,   710,   711,   712,   713,
     714,   715,   716,   894,  1015,   717,   718,   719,   899,   720,
     721,   903,  1025,  1110,  1191,   722,  1073,   723,   906,  1027,
     724,   725,   909,  1030,   478,   346,    41,   132,   231,   417,
     418,   419,   615,   420,   421,   617,   727,   728,  1165,  1307,
    1167,  1168,  1007,  1008,   866,   377,   669,  1169,  1212,   867,
     670,  1170,   862,   998,   440,   441,  1132,   442,   443,  1137,
     444,   649,   650,   651,   849,  1089,  1091,  1093,  1090,  1174,
    1258,  1308,  1317,  1259,  1324,  1266,  1332,  1337,  1267,  1342,
    1290,  1328,  1260,  1309,  1310,  1318,  1319,  1311,  1312,  1172,
      42,   238,   348,   533,   271,   349,   239,   134,   233,   537,
     234,   431,   624,   425,   426,   621,   619,   273,   241,   435,
     436,   634,   541,   630,   838,   631,   844,    46,    47,    48,
      49,    50,    51,   445,   136,    52,    53,   242,   427,   557,
      55,   139,   257,   460,   446,   447,   654,   854,   243,    57,
     141,   195,   282,   283,   481,    58,    59,   259,   260,   781,
     261,   262,   263,   244,   245,   449,   869,   924,   369,    61,
     144,   268,   269,   471,   467,   948,   737,   678,   874,  1009,
      62,    63,    64,   274,   475,  1142,  1184,  1185,  1272,    65,
     275,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,   202,    79,   302,   303,   495,   304,   305,
     498,   925,   738,   941,   453,   661,   518,   519,   743,   739,
    1121,  1122,  1123,   740,   741,  1042,    80,    81,    82,   246,
      83,   247,    84,    85,   248,   766,   249,   250,   251,    86,
      87,   154,   308,   309,   701,    88,   276,   277,   278,   279,
      89,   287,   288,    90,   292,   293,    91,   297,   298,    92,
     322,   509,   510,   504,   596,   512,   600,   323,   324,    93,
     156,   325,   326,   327,   331,   332,   515,    94,   174,    95,
     175,   176,   926,   977,    97,   178,   182,   183,   337,   338,
    1035,  1036,   918,   919,    98,   835,   927,   100,   928,  1214,
     101,   745,   102,   103,   522,  1046,  1087,   523,  1047
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      40,   256,   222,   108,    60,   339,   340,   411,    66,   468,
     109,   110,   342,   111,   990,   112,    45,   113,   531,   114,
     179,   180,    56,   115,   296,   116,   166,   117,   172,   118,
     165,   119,   171,   120,   424,   121,   930,   122,   709,   609,
     530,   344,   730,   286,   253,   123,   291,   137,   169,  1037,
     142,   124,   145,   125,   147,   126,   149,   127,   151,   128,
     895,   129,   695,   917,   240,   252,   131,   133,   135,   135,
     138,   140,   135,   143,   135,   140,   135,   140,   135,   140,
     135,   140,   153,   155,   314,   177,   177,   177,   955,   735,
     511,   897,   913,  1166,   901,   921,   613,   904,   328,   474,
    1096,  1084,   197,  1166,   199,   816,   802,   819,   797,   822,
     907,   825,     1,   827,   104,   896,   597,   598,   900,  1092,
    1094,   768,   916,  1000,  1098,   783,    54,   412,    10,  1327,
     130,   157,     1,     5,   158,    15,   161,   164,   168,   170,
       5,   213,   702,    15,   230,  1161,   437,   432,     5,   437,
    1113,   181,  1039,   456,   437,   864,   438,  1153,  1161,   295,
     865,     5,   415,     5,   237,    19,  1305,   472,    17,   995,
       2,   708,   702,    33,   300,   459,   491,     5,    31,    32,
       4,    33,   184,   280,  1180,   864,   514,    26,  -649,    34,
     865,    29,   350,   525,   281,     5,  1171,   351,   524,   353,
     707,   301,   355,   356,  1017,  1034,  1094,    26,  1392,   272,
     311,    29,   364,   312,  1045,   313,   316,   320,   321,  1163,
    1061,   439,   366,   367,   439,   626,   232,   370,   994,   439,
     371,     9,  1163,     5,  1083,  1019,  1097,   375,  1021,  -651,
    1022,   379,   702,    14,  1023,   856,   382,  1316,  1316,    16,
    1323,   385,   680,  -652,   860,  1274,   388,   266,  1028,  1018,
     868,   391,  1020,   394,  1033,    22,  1101,   396,   465,   706,
     707,  1278,  -653,   397,   958,   880,     5,   915,   960,   267,
     505,    30,   963,   965,   966,   729,  1277,   969,   970,   731,
     401,   972,   474,   953,   513,   951,   406,   736,   603,   232,
     502,   502,   503,   503,   416,   506,  1143,   508,  1102,   430,
     133,  1382,   943,  1151,  1385,  1128,   258,   841,   938,  1387,
     946,   939,  1390,   507,   929,  1302,  1391,   633,  1195,   470,
     133,  1193,   627,  1345,   501,   829,    21,   830,   480,  1104,
    1106,  1107,  1108,   465,  1126,   663,   465,     7,     2,   482,
     494,   307,   485,   497,     5,   488,   135,     2,     4,  1411,
      36,   801,    12,  1103,  1105,  1413,   336,     4,   466,   462,
     692,   451,   736,   694,   433,  1216,  1217,   528,   479,   529,
     473,   455,   235,   341,   534,  1313,   666,   888,   734,   892,
     667,   236,   892,  1335,   542,   892,     5,   583,   892,   584,
      24,   892,   543,   656,   658,   546,   691,  1067,  1068,  1069,
    1070,   550,   -41,   232,   423,   674,   676,   758,     5,  1032,
    1187,  1188,  1189,   687,   655,   657,   563,   689,   215,   411,
       5,     5,   567,    10,   434,   981,   673,   675,     1,   702,
     702,    21,   842,   451,  1186,   705,   290,   684,   705,    19,
     411,   299,   300,   235,   237,   285,     7,    10,   530,   666,
     850,   851,   236,   667,  1298,    36,   438,   706,   707,   531,
     706,   707,   875,    34,   846,   847,   848,   607,   237,  1412,
     694,   610,    17,    31,    32,  1273,   876,   614,   616,   -62,
     237,   584,   270,     5,    17,  1147,   502,  1261,   503,  1268,
     502,    16,   503,   747,   506,   653,   508,    31,    32,   660,
     864,   105,   106,    26,   652,   865,     5,    29,   659,  1304,
     662,  1305,   677,   664,   665,  1175,     5,   672,  1178,  1127,
    1314,  1305,   618,   413,   414,     5,   623,   517,   683,   892,
    1305,   892,   521,   845,   686,   892,   413,   688,   801,   146,
     690,   148,  1078,   150,  1080,   152,   668,   -70,   333,   334,
     335,   696,   -70,   -70,   -70,   751,   742,  1157,   753,   177,
     414,  1158,  1159,  1160,  1164,   205,   206,     5,   685,  1176,
    1304,   744,  1305,   878,  1182,   423,   694,   881,  1157,   757,
     329,   330,  1158,  1159,  1160,   726,   219,   220,     5,   726,
    1115,   726,  1314,  1305,  1116,  1117,  1118,  1119,  1134,   583,
     272,   491,  1320,  1320,   799,  1325,  1329,   516,  1334,   520,
    1339,  1339,     5,  1343,   628,   629,  1044,   703,   777,  1162,
    1149,   702,   232,   423,   703,   704,   705,   902,   706,   707,
     905,  1338,  1338,   892,   411,  1053,  1054,   130,   214,   215,
    1162,   216,   217,   218,   219,   220,  1276,    16,   706,   707,
     708,   910,  1120,  1075,  1076,   936,   214,   694,   881,   216,
     217,   218,   811,    22,   814,  1088,  1383,   817,   584,   820,
     801,   823,   707,   826,  1388,   852,   828,  1157,  1088,  1199,
     798,  1158,  1159,  1160,  1161,   833,   408,     5,  1393,   836,
    1206,  1207,  1394,   839,   979,   980,   702,    10,  1157,   703,
     704,   705,  1158,  1159,  1160,  1157,   843,   956,     5,  1158,
    1159,  1160,   952,  1305,   886,     5,   801,  1044,   237,   704,
     705,   889,    17,   706,   707,   708,   861,   214,  1012,   890,
     216,   217,   218,   219,   220,   799,  1016,   800,   612,  1162,
    1155,  1156,   706,   707,   997,  1111,  1282,    31,    32,   491,
    1088,  1198,  1202,   682,  1177,  1321,   801,  1340,  1163,     5,
    1162,    43,  1333,   893,  1183,    44,   898,  1162,   702,   754,
     622,   703,   704,   705,  1040,   908,   855,  1297,  1024,   858,
    1026,   310,   677,   159,  1300,   677,   160,   877,  1060,   694,
     264,   863,   265,  1001,   871,   706,   707,   708,   562,   162,
     887,   163,   891,   885,   681,   891,  1270,     5,   891,   315,
     411,   891,   496,   912,   891,   944,   702,   167,   317,     5,
     704,   705,   254,   318,   726,   319,   255,   922,   702,   697,
     698,   703,   704,   705,   699,  1099,   700,   173,  1181,     1,
       2,   732,   831,   706,   707,     3,    96,    99,  1395,  1129,
       4,     0,     5,     6,   450,   706,   707,   708,   997,     7,
       0,  1114,     8,     0,   463,   464,     0,   982,     0,     9,
      10,     0,    11,   477,    12,   694,     0,     0,  1109,    13,
     484,    14,     0,   487,    15,  1003,   490,    16,     0,  1004,
    1005,  1006,     0,   861,   500,    17,     0,     0,    18,     0,
      19,    20,    21,    22,   130,   214,     0,     0,   216,   217,
     218,    23,    24,    25,    26,    27,     0,    28,    29,    30,
      31,    32,    33,  1029,    34,    35,    36,     0,     0,     0,
     993,   214,  1041,     0,   216,   217,   218,   219,   220,     0,
    1157,   800,   997,     0,  1158,  1159,  1160,  1161,     0,     0,
       5,     0,   891,     0,   891,  1190,  1057,     0,   891,   702,
       0,     0,   703,   704,   705,     0,   726,   214,   215,  1181,
     216,   217,   218,     3,   465,     0,     0,     0,     0,     0,
       0,     6,     5,     0,  1079,     0,   706,   707,   708,     0,
       8,   702,  1085,     0,   703,   704,   705,     9,     0,     0,
      11,     0,  1162,   736,     0,     0,   861,  1031,   914,     0,
       0,     0,     0,   997,     0,    16,     0,     0,   706,   707,
     708,  1163,     3,     0,     0,     0,    18,     0,     0,    20,
       6,    22,  1125,     0,   677,     0,     0,     0,  1133,     8,
    1135,    25,  1138,    27,     0,    28,     9,    30,     0,     0,
       0,     0,     0,    35,     0,     0,   891,     0,    14,   214,
     215,     0,   216,   217,   218,   219,   220,   726,     0,   800,
       0,  1152,     0,     0,     0,    18,   726,     0,    20,     0,
       0,     0,  1173,   997,  1173,     0,     0,  1173,     0,     0,
      25,   465,    27,     0,    28,     0,    30,     0,     0,     5,
       0,   861,    35,     0,     0,     0,     0,     0,   702,     0,
       0,   703,   704,   705,     0,     0,  1196,     0,  1197,     0,
     736,   726,     0,     0,     0,   914,   726,     0,     0,  1386,
     465,   726,     0,  1208,     0,   706,   707,   708,     5,     0,
       0,     0,     0,     0,     0,   861,   861,   702,     0,     0,
     703,   704,   705,     0,     0,     0,     0,  1271,  1384,   736,
     452,   454,     0,     0,   457,   458,  1389,     0,     0,     0,
       0,     0,     0,     0,   706,   707,   708,   476,     0,     0,
       0,     0,     0,     0,   483,     0,     0,   486,     0,     0,
     489,     0,   185,     0,   186,   726,   187,   188,   499,   189,
       0,   190,   191,   726,   192,   193,   194,   196,   194,   198,
     194,   200,   201,     0,   203,     5,   204,     0,     0,     0,
       0,     0,     0,     0,   702,     0,  1301,   703,   704,   705,
       0,     0,     0,     0,     0,     0,   208,     0,   209,   210,
     211,   914,   212,   744,     0,     0,     0,     0,     0,     0,
       0,   706,   707,   708,     0,     0,     0,     0,  1351,     0,
    1354,     0,     0,     0,   130,   214,   215,  1359,   216,   217,
     218,   219,   220,  1306,  1306,  1315,  1315,     0,  1322,  1326,
       0,  1331,  1306,  1336,  1336,     5,  1341,     0,     0,     0,
       0,   345,     0,   347,   702,     0,     0,   703,   704,   705,
     352,     0,   354,     0,     0,     0,   357,   358,     0,     0,
       0,     0,   359,   360,   361,   362,     0,   363,   194,   365,
      16,   706,   707,   708,     0,     0,     0,     0,   368,     0,
       0,     0,     0,   372,   373,     0,   374,     0,     0,     0,
       0,     5,   378,     0,     0,     0,   380,   381,     0,     0,
     702,   383,   384,   703,   704,   705,   386,   387,     0,     0,
       0,     0,     0,     0,   392,     0,     0,   914,   395,   214,
     215,     0,   216,   217,   218,   219,   220,   706,   707,   708,
       0,     0,     0,     0,   399,   400,     0,     0,     0,     0,
       0,     0,     0,   402,   403,   404,     0,   405,  1218,  1219,
    1220,  1221,  1222,  1223,  1224,  1225,  1226,  1227,  1228,  1229,
    1230,  1231,  1232,  1233,  1234,  1235,  1236,  1237,  1238,  1239,
    1240,  1241,  1242,  1243,  1244,  1245,  1246,  1247,  1248,  1249,
    1250,  1251,  1252,  1253,  1254,  1255,  1256,  1257,  1219,  1220,
    1221,  1222,  1223,  1224,  1225,     0,  1227,  1228,  1229,  1230,
    1231,  1232,  1233,  1234,  1235,  1236,  1237,  1238,  1239,  1240,
    1241,  1242,  1243,  1244,  1245,  1246,  1247,  1248,  1249,  1250,
       0,     0,  1253,  1254,  1255,  1256,  1257,   526,   527,     0,
       0,     0,     0,     0,   347,   532,     0,   535,     0,     0,
       0,   536,   538,   539,     0,   540,   635,   636,   637,   638,
     639,   640,   641,   642,   643,     0,   545,     0,     0,   547,
     548,     0,   549,     0,   551,   552,     5,   554,   555,     0,
     556,     0,   558,   559,   560,   702,     0,   368,   703,   704,
     705,     0,   565,   566,     0,     0,   569,   570,     0,     0,
       0,     0,     0,   575,   576,     0,   578,   579,     0,   581,
     582,     0,   706,   707,   708,     0,     0,     0,     0,   592,
     593,   594,     0,     0,   595,     0,     0,     0,     0,     0,
     595,   595,   599,   601,     0,     0,   605,     0,   606,     0,
     608,     0,   601,  1262,  1219,  1220,  1221,  1222,  1223,  1224,
    1225,  1263,  1227,  1228,  1229,  1230,  1231,  1232,  1233,  1234,
    1235,  1236,  1237,  1238,  1239,  1240,  1241,  1242,  1243,  1244,
    1245,  1246,  1247,  1248,  1249,  1250,  1264,  1265,  1253,  1254,
    1255,  1256,  1257,   635,   636,   637,   638,   639,   640,   641,
     642,   643,   644,   645,   646,   647,   648,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   389,   390,     0,   393,
       0,     0,   748,     0,     0,     0,     0,     0,     0,   532,
       0,     0,   532,     0,   536,     0,   755,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   760,
     761,   762,     0,     0,   763,   764,   765,   767,   765,     0,
       0,   770,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   779,   780,   782,   780,     0,   784,     0,
     785,     0,   787,     0,   789,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   809,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   810,     0,   812,     0,     0,   815,     0,
     818,     0,   821,     0,   824,     0,   824,     0,     0,   595,
       0,   595,   599,     0,     0,   832,     0,     0,   834,     0,
       0,     0,     0,   837,     0,     0,     0,   840,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   561,     0,     0,     0,     0,   564,     0,     0,     0,
       0,     0,     0,   571,   572,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   590,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   755,     0,   837,     0,     0,     0,     0,     0,
       0,     0,   934,     0,     0,   765,     0,     0,   765,     0,
     940,     0,   942,   780,     0,     0,     0,     0,   945,     0,
       0,   780,     0,     0,   947,   785,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   824,     0,   959,
       0,   824,     0,     0,     0,   962,   964,   824,     0,     0,
     968,   824,     0,   971,   824,     0,   973,     0,     0,   974,
       0,     0,   976,   978,     0,     0,   834,   834,   601,     0,
       0,   983,     0,     0,     0,     0,     0,     0,     0,   985,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   775,   776,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1043,     0,     0,  1048,  1049,  1050,
       0,     0,     0,     0,   940,   940,     0,     0,     0,     0,
    1056,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1059,     0,     0,     0,     0,  1063,  1064,  1065,     0,
    1066,   824,   824,   824,     0,  1071,     0,  1072,     0,     0,
       0,     0,     0,   976,   976,  1077,     0,   601,     0,  1081,
       0,     0,   837,     0,     0,     0,  1086,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   284,
       0,   289,     0,   294,     0,     0,   306,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1131,
       0,   601,     0,  1136,     0,     0,     0,  1140,  1141,     0,
       0,     0,  1144,  1145,     0,  1146,     0,     0,   824,     0,
    1148,     0,     0,   601,     0,     0,     0,     0,     0,     0,
       0,   837,     0,     0,     0,     0,  1154,     0,  1086,  1086,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   931,   932,   933,   935,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1131,   601,     0,  1200,  1201,     0,  1136,  1203,     0,
       0,   949,     0,  1205,   601,     0,  1209,     0,     0,     0,
       0,     0,     0,  1211,  1213,     0,   398,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   407,     0,     0,  1281,  1211,  1283,     0,
     422,     0,   428,   429,     0,     0,  1284,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   461,     0,
       0,     0,     0,   469,     0,     0,     0,     0,     0,     0,
       0,  1285,  1286,     0,     0,     0,     0,     0,     0,     0,
    1288,  1289,  1291,  1292,  1293,  1294,     0,     0,  1296,     0,
     601,     0,   837,     0,     0,     0,     0,   601,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1051,     0,     0,  1052,     0,
       0,     0,     0,  1055,     0,     0,     0,     0,     0,  1349,
       0,  1352,     0,     0,  1058,     0,     0,     0,  1357,   544,
    1360,     0,     0,     0,     0,  1364,     0,     0,     0,  1368,
     553,     0,     0,  1371,     0,     0,     0,     0,  1375,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   568,
       0,     0,     0,     0,     0,   573,   574,     0,     0,   577,
       0,     0,   580,     0,     0,     0,     0,     0,     0,   589,
       0,     0,   591,     0,     0,     0,     0,     0,     0,     0,
    1213,     0,     0,  1397,  1399,     0,     0,     0,  1403,   604,
       0,     0,     0,     0,     0,     0,     0,     0,   611,     0,
       0,  1139,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   625,     0,     0,     0,   632,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   671,     0,     0,     0,     0,     0,     0,   679,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   746,     0,  1204,     0,     0,     0,   749,
       0,   750,     0,     0,     0,   752,     0,     0,     0,     0,
    1215,   756,     0,     0,     0,     0,     0,     0,   759,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   769,     0,     0,   771,   772,   773,   774,
       0,     0,     0,     0,     0,     0,   778,     0,     0,     0,
       0,     0,     0,     0,     0,   786,     0,   788,     0,   790,
     791,   792,   793,   794,   795,   796,     0,     0,     0,     0,
     808,     0,     0,     0,     0,     0,     0,     0,     0,  1299,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1346,     0,     0,
       0,  1347,  1348,     0,  1350,     0,  1353,     0,     0,     0,
    1355,  1356,     0,  1358,     0,  1361,     0,  1362,  1363,     0,
    1365,  1366,  1367,     0,  1369,     0,  1370,     0,  1372,  1373,
     857,  1374,   859,  1376,  1377,     0,  1378,     0,  1379,     0,
       0,     0,     0,     0,     0,   870,     0,   872,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1396,  1398,     0,
    1400,     0,  1402,  1404,     0,  1406,  1407,  1408,  1409,  1410,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   920,
       0,     0,   923,     0,     0,     0,  1414,     0,  1416,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   937,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   950,     0,     0,     0,     0,     0,     0,     0,   957,
       0,     0,     0,     0,     0,     0,     0,     0,   961,     0,
       0,     0,     0,   967,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   975,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   984,     0,   986,   987,   988,   989,     0,     0,
       0,     0,   991,   992,     0,   996,     0,   999,     0,     0,
    1002,     0,  1010,  1011,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1038,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1062,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1074,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1082,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1095,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1112,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1124,     0,     0,
       0,  1130,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1150,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1179,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1192,     0,  1194,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1210,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1269,     0,
       0,     0,     0,     0,     0,     0,  1275,     0,  1279,  1280,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1287,     0,     0,     0,     0,     0,     0,     0,
    1295,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1303,     0,     0,     0,
       0,     0,     0,     0,  1330,     0,     0,     0,     0,     0,
       0,     0,  1344,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1380,
    1381,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1401,     0,     0,  1405,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1415
};

static const yytype_int16 yycheck[] =
{
       0,   189,   185,    45,     0,   213,   221,   341,     0,   368,
      52,    53,   228,    55,   934,    57,     0,    59,   424,    61,
     127,   128,     0,    65,   200,    67,   125,    69,   125,    71,
     125,    73,   125,    75,   347,    77,   844,    79,   595,   522,
     424,   229,   599,   196,   188,    87,   198,   110,   125,   976,
     113,    93,   115,    95,   117,    97,   119,    99,   121,   101,
     815,   103,   588,   834,   187,   188,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   204,   126,   127,   128,   882,   605,
     400,   815,   833,  1131,   818,   836,   527,   821,   204,   373,
    1056,  1043,   148,  1141,   150,   718,   695,   720,   692,   722,
     826,   724,     4,   726,     0,   815,   510,   511,   818,  1049,
    1050,   658,   834,   944,  1059,   676,     0,   342,    35,  1289,
     146,   125,     4,    17,   125,    49,   125,   125,   125,   125,
      17,   183,    26,    49,   185,    14,     6,   351,    17,     6,
    1077,    40,   978,   360,     6,     7,    16,  1125,    14,    73,
      12,    17,   345,    17,    56,    65,    22,   371,    60,   940,
       5,    55,    26,    87,    51,   363,   391,    17,    85,    86,
      15,    87,   162,    23,  1140,     7,   401,    79,   162,    89,
      12,    83,   234,   408,   194,    17,  1131,   239,   406,   241,
      54,   201,   244,   245,   959,   976,  1136,    79,  1368,   193,
     204,    83,   258,   204,   985,   204,   204,   204,   204,    88,
    1014,    81,   264,   265,    81,   538,    61,   269,   940,    81,
     272,    34,    88,    17,  1042,   959,  1057,   279,   962,   162,
     964,   283,    26,    46,   968,   765,   288,  1285,  1286,    52,
    1288,   293,   565,   162,   770,  1211,   298,     3,   974,   959,
     780,   303,   962,   305,   976,    68,  1060,   309,     9,    53,
      54,  1213,   162,   315,   887,   801,    17,   834,   891,    25,
     400,    84,   895,   896,   897,   595,  1213,   900,   901,   599,
     332,   904,   566,   882,   400,   879,   338,    38,   514,    61,
     399,   400,   399,   400,   345,   400,  1100,   400,  1063,   350,
     351,  1349,   863,  1121,  1352,  1086,   190,   748,   855,  1357,
     871,   858,  1360,   400,   840,  1281,  1364,   542,  1154,   370,
     371,  1152,   539,  1301,   397,   729,    67,   731,   379,  1063,
    1064,  1065,  1066,     9,  1085,   552,     9,    24,     5,   380,
     391,    64,   383,   394,    17,   386,   397,     5,    15,  1397,
      91,   695,    39,  1063,  1064,  1403,    41,    15,   368,   365,
     585,    48,    38,   588,   352,  1196,  1197,   419,   378,   421,
     372,   359,    19,   149,   426,  1284,     7,   810,   603,   812,
      11,    28,   815,  1292,   436,   818,    17,   154,   821,   156,
      77,   824,   444,   547,   548,   447,   582,  1020,  1021,  1022,
    1023,   453,   149,    61,    62,   559,   560,   633,    17,   976,
    1144,  1145,  1146,   576,   547,   548,   468,   579,   148,   763,
      17,    17,   474,    35,    57,   918,   559,   560,     4,    26,
      26,    67,   755,    48,  1144,    31,    72,   570,    31,    65,
     784,    50,    51,    19,    56,    71,    24,    35,   842,     7,
     761,   762,    28,    11,  1272,    91,    16,    53,    54,   875,
      53,    54,   785,    89,   150,   151,   152,   519,    56,  1399,
     695,   523,    60,    85,    86,  1209,   787,   528,   529,   154,
      56,   156,    70,    17,    60,  1108,   595,  1201,   595,  1203,
     599,    52,   599,   610,   599,   546,   599,    85,    86,   550,
       7,   158,   159,    79,   545,    12,    17,    83,   549,    20,
     551,    22,   563,   554,   555,  1135,    17,   558,  1138,  1086,
      21,    22,   532,    32,    33,    17,   536,    25,   569,   962,
      22,   964,    47,   758,   575,   968,    32,   578,   882,   116,
     581,   118,  1035,   120,  1037,   122,   556,   155,   209,   210,
     211,   592,   160,   161,   162,   619,   607,     7,   622,   610,
      33,    11,    12,    13,  1131,   160,   161,    17,   570,  1136,
      20,    45,    22,   798,  1141,    62,   801,   802,     7,   631,
      74,    75,    11,    12,    13,   595,   153,   154,    17,   599,
       7,   601,    21,    22,    11,    12,    13,    14,  1091,   154,
     594,   826,  1285,  1286,   155,  1288,  1289,   402,  1291,   404,
    1293,  1294,    17,  1296,    58,    59,   985,    29,   670,    69,
    1113,    26,    61,    62,    29,    30,    31,   820,    53,    54,
     823,  1293,  1294,  1066,   978,   994,   995,   146,   147,   148,
      69,   150,   151,   152,   153,   154,  1213,    52,    53,    54,
      55,   146,    69,  1033,  1034,   853,   147,   882,   883,   150,
     151,   152,   714,    68,   716,     8,  1349,   719,   156,   721,
    1014,   723,    54,   725,  1357,   763,   728,     7,     8,  1172,
     693,    11,    12,    13,    14,   737,   341,    17,  1371,   741,
    1183,  1184,  1375,   745,   916,   917,    26,    35,     7,    29,
      30,    31,    11,    12,    13,     7,   757,   883,    17,    11,
      12,    13,   880,    22,   810,    17,  1060,  1086,    56,    30,
      31,   811,    60,    53,    54,    55,   777,   147,   953,   812,
     150,   151,   152,   153,   154,   155,   959,   157,   526,    69,
    1128,  1129,    53,    54,   942,  1072,  1217,    85,    86,   974,
       8,  1171,  1177,   567,  1136,  1286,  1100,  1294,    88,    17,
      69,     0,  1291,   814,  1141,     0,   817,    69,    26,   624,
     535,    29,    30,    31,   982,   826,   764,  1270,   971,   767,
     973,   204,   833,   125,  1277,   836,   125,   789,  1014,  1014,
     191,   779,   191,   945,   782,    53,    54,    55,   467,   125,
     810,   125,   812,   809,   566,   815,  1205,    17,   818,   204,
    1154,   821,   392,   832,   824,   867,    26,   125,   204,    17,
      30,    31,   188,   204,   834,   204,   188,   837,    26,   593,
     593,    29,    30,    31,   593,  1060,   593,   125,    36,     4,
       5,   599,   732,    53,    54,    10,     0,     0,  1380,  1086,
      15,    -1,    17,    18,   356,    53,    54,    55,  1056,    24,
      -1,  1079,    27,    -1,   366,   367,    -1,   919,    -1,    34,
      35,    -1,    37,   375,    39,  1100,    -1,    -1,  1071,    44,
     382,    46,    -1,   385,    49,     7,   388,    52,    -1,    11,
      12,    13,    -1,   944,   396,    60,    -1,    -1,    63,    -1,
      65,    66,    67,    68,   146,   147,    -1,    -1,   150,   151,
     152,    76,    77,    78,    79,    80,    -1,    82,    83,    84,
      85,    86,    87,   974,    89,    90,    91,    -1,    -1,    -1,
     940,   147,   983,    -1,   150,   151,   152,   153,   154,    -1,
       7,   157,  1140,    -1,    11,    12,    13,    14,    -1,    -1,
      17,    -1,   962,    -1,   964,  1148,  1008,    -1,   968,    26,
      -1,    -1,    29,    30,    31,    -1,   976,   147,   148,    36,
     150,   151,   152,    10,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    17,    -1,  1036,    -1,    53,    54,    55,    -1,
      27,    26,  1044,    -1,    29,    30,    31,    34,    -1,    -1,
      37,    -1,    69,    38,    -1,    -1,  1057,    42,    43,    -1,
      -1,    -1,    -1,  1211,    -1,    52,    -1,    -1,    53,    54,
      55,    88,    10,    -1,    -1,    -1,    63,    -1,    -1,    66,
      18,    68,  1084,    -1,  1085,    -1,    -1,    -1,  1090,    27,
    1092,    78,  1094,    80,    -1,    82,    34,    84,    -1,    -1,
      -1,    -1,    -1,    90,    -1,    -1,  1066,    -1,    46,   147,
     148,    -1,   150,   151,   152,   153,   154,  1077,    -1,   157,
      -1,  1123,    -1,    -1,    -1,    63,  1086,    -1,    66,    -1,
      -1,    -1,  1133,  1281,  1135,    -1,    -1,  1138,    -1,    -1,
      78,     9,    80,    -1,    82,    -1,    84,    -1,    -1,    17,
      -1,  1152,    90,    -1,    -1,    -1,    -1,    -1,    26,    -1,
      -1,    29,    30,    31,    -1,    -1,  1168,    -1,  1170,    -1,
      38,  1131,    -1,    -1,    -1,    43,  1136,    -1,    -1,  1354,
       9,  1141,    -1,  1185,    -1,    53,    54,    55,    17,    -1,
      -1,    -1,    -1,    -1,    -1,  1196,  1197,    26,    -1,    -1,
      29,    30,    31,    -1,    -1,    -1,    -1,  1208,  1351,    38,
     357,   358,    -1,    -1,   361,   362,  1359,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    53,    54,    55,   374,    -1,    -1,
      -1,    -1,    -1,    -1,   381,    -1,    -1,   384,    -1,    -1,
     387,    -1,   132,    -1,   134,  1205,   136,   137,   395,   139,
      -1,   141,   142,  1213,   144,   145,   146,   147,   148,   149,
     150,   151,   152,    -1,   154,    17,   156,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    26,    -1,  1278,    29,    30,    31,
      -1,    -1,    -1,    -1,    -1,    -1,   176,    -1,   178,   179,
     180,    43,   182,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    53,    54,    55,    -1,    -1,    -1,    -1,  1310,    -1,
    1312,    -1,    -1,    -1,   146,   147,   148,  1319,   150,   151,
     152,   153,   154,  1283,  1284,  1285,  1286,    -1,  1288,  1289,
      -1,  1291,  1292,  1293,  1294,    17,  1296,    -1,    -1,    -1,
      -1,   231,    -1,   233,    26,    -1,    -1,    29,    30,    31,
     240,    -1,   242,    -1,    -1,    -1,   246,   247,    -1,    -1,
      -1,    -1,   252,   253,   254,   255,    -1,   257,   258,   259,
      52,    53,    54,    55,    -1,    -1,    -1,    -1,   268,    -1,
      -1,    -1,    -1,   273,   274,    -1,   276,    -1,    -1,    -1,
      -1,    17,   282,    -1,    -1,    -1,   286,   287,    -1,    -1,
      26,   291,   292,    29,    30,    31,   296,   297,    -1,    -1,
      -1,    -1,    -1,    -1,   304,    -1,    -1,    43,   308,   147,
     148,    -1,   150,   151,   152,   153,   154,    53,    54,    55,
      -1,    -1,    -1,    -1,   324,   325,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   333,   334,   335,    -1,   337,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   107,   108,
     109,   110,   111,   112,   113,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
      -1,    -1,   141,   142,   143,   144,   145,   417,   418,    -1,
      -1,    -1,    -1,    -1,   424,   425,    -1,   427,    -1,    -1,
      -1,   431,   432,   433,    -1,   435,    92,    93,    94,    95,
      96,    97,    98,    99,   100,    -1,   446,    -1,    -1,   449,
     450,    -1,   452,    -1,   454,   455,    17,   457,   458,    -1,
     460,    -1,   462,   463,   464,    26,    -1,   467,    29,    30,
      31,    -1,   472,   473,    -1,    -1,   476,   477,    -1,    -1,
      -1,    -1,    -1,   483,   484,    -1,   486,   487,    -1,   489,
     490,    -1,    53,    54,    55,    -1,    -1,    -1,    -1,   499,
     500,   501,    -1,    -1,   504,    -1,    -1,    -1,    -1,    -1,
     510,   511,   512,   513,    -1,    -1,   516,    -1,   518,    -1,
     520,    -1,   522,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   301,   302,    -1,   304,
      -1,    -1,   612,    -1,    -1,    -1,    -1,    -1,    -1,   619,
      -1,    -1,   622,    -1,   624,    -1,   626,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   649,
     650,   651,    -1,    -1,   654,   655,   656,   657,   658,    -1,
      -1,   661,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   673,   674,   675,   676,    -1,   678,    -1,
     680,    -1,   682,    -1,   684,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   701,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   713,    -1,   715,    -1,    -1,   718,    -1,
     720,    -1,   722,    -1,   724,    -1,   726,    -1,    -1,   729,
      -1,   731,   732,    -1,    -1,   735,    -1,    -1,   738,    -1,
      -1,    -1,    -1,   743,    -1,    -1,    -1,   747,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   466,    -1,    -1,    -1,    -1,   471,    -1,    -1,    -1,
      -1,    -1,    -1,   478,   479,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   496,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   842,    -1,   844,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   852,    -1,    -1,   855,    -1,    -1,   858,    -1,
     860,    -1,   862,   863,    -1,    -1,    -1,    -1,   868,    -1,
      -1,   871,    -1,    -1,   874,   875,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   887,    -1,   889,
      -1,   891,    -1,    -1,    -1,   895,   896,   897,    -1,    -1,
     900,   901,    -1,   903,   904,    -1,   906,    -1,    -1,   909,
      -1,    -1,   912,   913,    -1,    -1,   916,   917,   918,    -1,
      -1,   921,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   929,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   668,   669,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   984,    -1,    -1,   987,   988,   989,
      -1,    -1,    -1,    -1,   994,   995,    -1,    -1,    -1,    -1,
    1000,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1011,    -1,    -1,    -1,    -1,  1016,  1017,  1018,    -1,
    1020,  1021,  1022,  1023,    -1,  1025,    -1,  1027,    -1,    -1,
      -1,    -1,    -1,  1033,  1034,  1035,    -1,  1037,    -1,  1039,
      -1,    -1,  1042,    -1,    -1,    -1,  1046,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   195,
      -1,   197,    -1,   199,    -1,    -1,   202,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1089,
      -1,  1091,    -1,  1093,    -1,    -1,    -1,  1097,  1098,    -1,
      -1,    -1,  1102,  1103,    -1,  1105,    -1,    -1,  1108,    -1,
    1110,    -1,    -1,  1113,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1121,    -1,    -1,    -1,    -1,  1126,    -1,  1128,  1129,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   849,   850,   851,   852,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1171,  1172,    -1,  1174,  1175,    -1,  1177,  1178,    -1,
      -1,   876,    -1,  1183,  1184,    -1,  1186,    -1,    -1,    -1,
      -1,    -1,    -1,  1193,  1194,    -1,   322,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   339,    -1,    -1,  1216,  1217,  1218,    -1,
     346,    -1,   348,   349,    -1,    -1,  1226,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   364,    -1,
      -1,    -1,    -1,   369,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1251,  1252,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1260,  1261,  1262,  1263,  1264,  1265,    -1,    -1,  1268,    -1,
    1270,    -1,  1272,    -1,    -1,    -1,    -1,  1277,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   990,    -1,    -1,   993,    -1,
      -1,    -1,    -1,   998,    -1,    -1,    -1,    -1,    -1,  1309,
      -1,  1311,    -1,    -1,  1009,    -1,    -1,    -1,  1318,   445,
    1320,    -1,    -1,    -1,    -1,  1325,    -1,    -1,    -1,  1329,
     456,    -1,    -1,  1333,    -1,    -1,    -1,    -1,  1338,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   475,
      -1,    -1,    -1,    -1,    -1,   481,   482,    -1,    -1,   485,
      -1,    -1,   488,    -1,    -1,    -1,    -1,    -1,    -1,   495,
      -1,    -1,   498,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1380,    -1,    -1,  1383,  1384,    -1,    -1,    -1,  1388,   515,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   524,    -1,
      -1,  1096,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   537,    -1,    -1,    -1,   541,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   557,    -1,    -1,    -1,    -1,    -1,    -1,   564,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   609,    -1,  1180,    -1,    -1,    -1,   615,
      -1,   617,    -1,    -1,    -1,   621,    -1,    -1,    -1,    -1,
    1195,   627,    -1,    -1,    -1,    -1,    -1,    -1,   634,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   659,    -1,    -1,   662,   663,   664,   665,
      -1,    -1,    -1,    -1,    -1,    -1,   672,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   681,    -1,   683,    -1,   685,
     686,   687,   688,   689,   690,   691,    -1,    -1,    -1,    -1,
     696,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1274,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1302,    -1,    -1,
      -1,  1306,  1307,    -1,  1309,    -1,  1311,    -1,    -1,    -1,
    1315,  1316,    -1,  1318,    -1,  1320,    -1,  1322,  1323,    -1,
    1325,  1326,  1327,    -1,  1329,    -1,  1331,    -1,  1333,  1334,
     766,  1336,   768,  1338,  1339,    -1,  1341,    -1,  1343,    -1,
      -1,    -1,    -1,    -1,    -1,   781,    -1,   783,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1382,  1383,    -1,
    1385,    -1,  1387,  1388,    -1,  1390,  1391,  1392,  1393,  1394,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   835,
      -1,    -1,   838,    -1,    -1,    -1,  1411,    -1,  1413,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   854,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   877,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   885,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   894,    -1,
      -1,    -1,    -1,   899,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   911,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   928,    -1,   930,   931,   932,   933,    -1,    -1,
      -1,    -1,   938,   939,    -1,   941,    -1,   943,    -1,    -1,
     946,    -1,   948,   949,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   977,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1015,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1030,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1040,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1055,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1073,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1083,    -1,    -1,
      -1,  1087,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1114,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1139,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1151,    -1,  1153,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1191,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1204,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1212,    -1,  1214,  1215,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1258,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1266,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1282,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1290,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1298,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1345,
    1346,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1386,    -1,    -1,  1389,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1412
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
     429,   430,   431,   433,   435,   436,   442,   443,   448,   453,
     456,   459,   462,   472,   480,   482,   485,   487,   497,   499,
     500,   503,   505,   506,     0,   158,   159,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     146,   204,   240,   204,   310,   204,   337,   337,   204,   344,
     204,   353,   337,   204,   373,   337,   353,   337,   353,   337,
     353,   337,   353,   204,   444,   204,   473,   332,   333,   334,
     335,   358,   384,   385,   395,   399,   403,   429,   442,   448,
     453,   456,   459,   462,   481,   483,   484,   204,   488,   488,
     488,    40,   489,   490,   162,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   354,   203,   354,   203,   354,
     203,   203,   406,   203,   203,   160,   161,   202,   203,   203,
     203,   203,   203,   201,   147,   148,   150,   151,   152,   153,
     154,   167,   168,   169,   171,   173,   174,   177,   179,   180,
     204,   241,    61,   311,   313,    19,    28,    56,   304,   309,
     320,   321,   340,   351,   366,   367,   432,   434,   437,   439,
     440,   441,   320,   340,   432,   434,   171,   345,   341,   360,
     361,   363,   364,   365,   366,   367,     3,    25,   374,   375,
      70,   307,   309,   320,   386,   393,   449,   450,   451,   452,
      23,   237,   355,   356,   202,    71,   397,   454,   455,   202,
      72,   401,   457,   458,   202,    73,   405,   460,   461,    50,
      51,   237,   408,   409,   411,   412,   202,    64,   445,   446,
     331,   332,   333,   358,   383,   392,   395,   430,   431,   433,
     442,   453,   463,   470,   471,   474,   475,   476,   505,    74,
      75,   477,   478,   489,   489,   489,    41,   491,   492,   166,
     179,   149,   167,   178,   171,   203,   238,   203,   305,   308,
     201,   201,   203,   201,   203,   201,   201,   203,   203,   203,
     203,   203,   203,   203,   354,   203,   201,   201,   203,   371,
     201,   201,   203,   203,   203,   201,   203,   258,   203,   201,
     203,   203,   201,   203,   203,   201,   203,   203,   201,   258,
     258,   201,   203,   258,   201,   203,   201,   201,   202,   203,
     203,   201,   203,   203,   203,   203,   201,   202,   172,   175,
     176,   180,   179,    32,    33,   168,   204,   242,   243,   244,
     246,   247,   202,    62,   311,   316,   317,   341,   202,   202,
     204,   314,   310,   351,    57,   322,   323,     6,    16,    81,
     267,   268,   270,   271,   273,   336,   347,   348,   204,   368,
     368,    48,   414,   417,   414,   351,   336,   414,   414,   171,
     346,   202,   370,   368,   368,     9,   237,   377,   379,   202,
     204,   376,   310,   393,   273,   387,   414,   368,   237,   237,
     204,   357,   267,   414,   368,   267,   414,   368,   267,   414,
     368,   179,   183,   185,   204,   410,   408,   204,   413,   414,
     368,   337,   403,   459,   466,   383,   399,   448,   456,   464,
     465,   466,   468,   505,   179,   479,   491,    25,   419,   420,
     491,    47,   507,   510,   166,   179,   203,   203,   201,   201,
     305,   308,   203,   306,   201,   203,   203,   312,   203,   203,
     203,   325,   201,   201,   202,   203,   201,   203,   203,   203,
     201,   203,   203,   202,   203,   203,   203,   342,   203,   203,
     203,   258,   371,   201,   258,   203,   203,   201,   202,   203,
     203,   258,   258,   202,   202,   203,   203,   202,   203,   203,
     202,   203,   203,   154,   156,   182,   184,   188,   191,   202,
     258,   202,   203,   203,   203,   203,   467,   467,   467,   203,
     469,   203,   206,   167,   202,   203,   203,   201,   203,   206,
     201,   202,   243,   246,   204,   245,   204,   248,   237,   319,
     168,   318,   316,   237,   315,   202,   311,   336,    58,    59,
     326,   328,   202,   179,   324,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   274,
     275,   276,   267,   204,   349,   320,   340,   320,   340,   267,
     204,   418,   267,   336,   267,   267,     7,    11,   237,   259,
     263,   202,   267,   320,   340,   320,   340,   204,   380,   202,
     311,   387,   274,   267,   320,   393,   267,   397,   267,   401,
     267,   405,   179,   186,   179,   192,   267,   437,   439,   440,
     441,   447,    26,    29,    30,    31,    53,    54,    55,   207,
     209,   210,   211,   212,   213,   214,   215,   218,   219,   220,
     222,   223,   228,   230,   233,   234,   237,   249,   250,   466,
     207,   466,   468,   207,   179,   419,    38,   379,   415,   422,
     426,   427,   204,   421,    45,   504,   202,   488,   203,   202,
     202,   306,   202,   306,   312,   203,   202,   201,   167,   202,
     203,   203,   203,   203,   203,   203,   438,   203,   438,   202,
     203,   202,   202,   202,   202,   258,   258,   201,   202,   203,
     203,   362,   203,   362,   203,   203,   202,   203,   202,   203,
     202,   202,   202,   202,   202,   202,   202,   184,   182,   155,
     157,   180,   181,   189,   192,   197,   198,   199,   202,   203,
     203,   201,   203,   208,   201,   203,   208,   201,   203,   208,
     201,   203,   208,   201,   203,   208,   201,   208,   201,   467,
     467,   469,   203,   201,   203,   498,   201,   203,   327,   201,
     203,   246,   311,   204,   329,   179,   150,   151,   152,   277,
     277,   277,   170,   172,   350,   351,   347,   202,   351,   202,
     419,   204,   265,   351,     7,    12,   257,   262,   347,   369,
     202,   351,   202,   170,   381,   311,   277,   393,   179,   187,
     192,   179,   193,   181,   200,   370,   211,   237,   249,   214,
     218,   237,   249,   204,   216,   222,   228,   233,   204,   221,
     228,   233,   168,   224,   233,   168,   231,   183,   204,   235,
     146,   205,   415,   380,    43,   207,   422,   426,   495,   496,
     202,   380,   237,   202,   370,   414,   485,   499,   501,   419,
     327,   258,   258,   258,   203,   258,   171,   202,   438,   438,
     203,   416,   203,   362,   201,   203,   362,   203,   378,   258,
     202,   184,   200,   181,   190,   197,   193,   202,   208,   203,
     208,   202,   203,   208,   203,   208,   208,   202,   203,   208,
     208,   203,   208,   203,   203,   202,   203,   486,   203,   498,
     498,   206,   201,   203,   202,   203,   202,   202,   202,   202,
     257,   202,   202,   237,   422,   426,   202,   171,   266,   202,
     265,   369,   202,     7,    11,    12,    13,   255,   256,   382,
     202,   202,   179,   194,   195,   217,   219,   222,   228,   233,
     228,   233,   233,   233,   168,   225,   168,   232,   183,   204,
     236,    42,   207,   422,   426,   493,   494,   495,   202,   381,
     318,   204,   428,   203,   379,   426,   508,   511,   203,   203,
     203,   258,   258,   416,   416,   258,   203,   201,   258,   203,
     167,   197,   202,   203,   203,   203,   203,   208,   208,   208,
     208,   203,   203,   229,   202,   486,   486,   203,   206,   201,
     206,   203,   202,   327,   504,   201,   203,   509,     8,   278,
     281,   279,   281,   280,   281,   202,   266,   265,   278,   179,
     196,   197,   222,   228,   233,   228,   233,   233,   233,   168,
     226,   259,   202,   495,   166,     7,    11,    12,    13,    14,
      69,   423,   424,   425,   202,   201,   380,   207,   426,   511,
     202,   203,   269,   201,   206,   201,   203,   272,   201,   258,
     203,   203,   388,   197,   203,   203,   203,   208,   203,   206,
     202,   327,   201,   501,   203,   509,   509,     7,    11,    12,
      13,    14,    69,    88,   207,   251,   252,   253,   254,   260,
     264,   278,   302,   204,   282,   282,   207,   280,   282,   202,
     266,    36,   207,   302,   389,   390,   228,   233,   233,   233,
     168,   227,   202,   265,   202,   381,   201,   201,   269,   206,
     203,   203,   272,   203,   258,   203,   206,   206,   201,   203,
     202,   203,   261,   203,   502,   258,   265,   265,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   283,   286,
     295,   295,   106,   114,   139,   140,   288,   291,   295,   202,
     389,   204,   391,   233,   266,   202,   207,   495,   504,   202,
     202,   203,   261,   203,   203,   203,   203,   202,   203,   203,
     293,   203,   203,   203,   203,   202,   203,   206,   327,   258,
     206,   201,   266,   202,    20,    22,   237,   252,   284,   296,
     297,   300,   301,   284,    21,   237,   252,   285,   298,   299,
     300,   285,   237,   252,   287,   300,   237,   260,   294,   300,
     202,   237,   289,   296,   300,   284,   237,   290,   298,   300,
     290,   237,   292,   300,   202,   501,   258,   258,   258,   203,
     258,   201,   203,   258,   201,   258,   258,   203,   258,   201,
     203,   258,   258,   258,   203,   258,   258,   258,   203,   258,
     258,   203,   258,   258,   258,   203,   258,   258,   258,   258,
     202,   202,   252,   300,   168,   252,   179,   252,   300,   168,
     252,   252,   260,   300,   300,   502,   258,   203,   258,   203,
     258,   202,   258,   203,   258,   202,   258,   258,   258,   258,
     258,   252,   257,   252,   258,   202,   258
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
     367,   368,   369,   370,   371,   371,   371,   372,   373,   374,
     375,   375,   376,   377,   378,   378,   379,   380,   381,   382,
     383,   383,   384,   385,   386,   386,   387,   388,   388,   388,
     388,   388,   389,   390,   391,   392,   392,   393,   394,   394,
     394,   395,   396,   396,   397,   398,   398,   399,   400,   401,
     402,   402,   403,   404,   405,   406,   406,   406,   406,   406,
     407,   407,   408,   409,   410,   410,   411,   412,   413,   414,
     415,   415,   416,   416,   416,   416,   417,   418,   419,   420,
     421,   422,   423,   424,   425,   425,   425,   425,   425,   425,
     426,   427,   428,   429,   429,   429,   430,   430,   431,   432,
     432,   433,   434,   434,   435,   436,   437,   438,   438,   439,
     440,   441,   442,   443,   444,   445,   446,   447,   447,   447,
     447,   448,   449,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   458,   459,   460,   461,   462,   463,   463,   464,
     464,   464,   464,   465,   465,   466,   466,   467,   467,   467,
     468,   468,   469,   469,   469,   469,   470,   471,   472,   473,
     474,   474,   474,   474,   474,   474,   474,   474,   474,   474,
     474,   475,   476,   477,   478,   478,   479,   479,   479,   480,
     481,   481,   482,   483,   483,   483,   483,   483,   483,   483,
     483,   483,   483,   483,   483,   483,   483,   484,   484,   484,
     484,   484,   484,   484,   485,   486,   486,   486,   486,   486,
     486,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   498,   498,   498,   498,   499,   500,
     501,   501,   501,   501,   502,   502,   502,   502,   503,   504,
     505,   506,   507,   508,   508,   509,   509,   509,   509,   510,
     511
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
       1,     1,     1,     7,     0,     3,     3,     1,     1,     5,
       1,     1,     1,     7,     0,     3,     1,     1,     1,     1,
       1,     1,     8,    10,     1,     1,    10,     0,     3,     5,
       3,     2,     5,     1,     1,     1,     1,     5,     1,     1,
       1,     8,     1,     1,     5,     1,     1,     8,     1,     5,
       1,     1,     8,     1,     5,     0,     3,     5,     3,     3,
       1,     1,     4,     1,     1,     1,     4,     1,     1,     7,
       1,     1,     0,     3,     3,     3,     1,     1,     5,     1,
       1,     9,     1,     5,     1,     1,     1,     1,     1,     1,
       7,     1,     1,     1,     1,     1,     1,     1,    10,     1,
       1,    10,     1,     1,    10,    10,     7,     0,     2,     9,
       7,     9,    10,     1,     1,     8,     1,     1,     1,     1,
       1,    10,     1,     1,     6,     8,     1,    10,     6,     1,
      10,     6,     1,    10,     6,     1,     6,     3,     1,     2,
       2,     2,     2,     1,     1,     1,     1,     0,     3,     2,
       1,     1,     0,     3,     3,     2,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     1,     1,     1,     2,     3,     4,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,    13,     0,     3,     3,     3,     5,
       3,     2,     1,     1,     4,     1,     4,     1,     4,     1,
       4,     1,    11,     0,     3,     3,     3,     2,     1,    19,
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
