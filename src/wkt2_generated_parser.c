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
#define YYLAST   3662

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  163
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  349
/* YYNRULES -- Number of rules.  */
#define YYNRULES  699
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1419

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
    1246,  1248,  1253,  1253,  1254,  1256,  1256,  1258,  1265,  1265,
    1267,  1274,  1274,  1276,  1283,  1290,  1295,  1296,  1298,  1304,
    1309,  1317,  1323,  1325,  1327,  1332,  1334,  1334,  1335,  1335,
    1339,  1345,  1345,  1347,  1350,  1354,  1359,  1365,  1368,  1373,
    1379,  1382,  1387,  1393,  1396,  1401,  1406,  1407,  1410,  1411,
    1412,  1414,  1416,  1416,  1418,  1418,  1420,  1421,  1422,  1424,
    1424,  1426,  1427,  1428,  1429,  1431,  1433,  1435,  1437,  1439,
    1439,  1439,  1440,  1440,  1440,  1442,  1442,  1442,  1442,  1444,
    1446,  1448,  1452,  1455,  1455,  1458,  1459,  1460,  1462,  1466,
    1467,  1469,  1471,  1471,  1472,  1472,  1473,  1473,  1473,  1474,
    1475,  1475,  1476,  1476,  1477,  1477,  1479,  1479,  1480,  1480,
    1481,  1482,  1482,  1486,  1493,  1494,  1495,  1496,  1497,  1498,
    1499,  1501,  1503,  1505,  1507,  1509,  1511,  1513,  1515,  1517,
    1519,  1524,  1531,  1532,  1533,  1534,  1535,  1537,  1542,  1550,
    1550,  1550,  1550,  1552,  1553,  1554,  1555,  1557,  1559,  1564,
    1570,  1572,  1579,  1579,  1581,  1582,  1583,  1584,  1586,  1588
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

#define YYPACT_NINF -1182

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-1182)))

#define YYTABLE_NINF -653

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     826, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182,
   -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182,
   -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182,
   -1182, -1182, -1182, -1182, -1182, -1182, -1182,   125, -1182, -1182,
   -1182,   229, -1182, -1182, -1182,   229, -1182, -1182, -1182, -1182,
   -1182, -1182,   229,   229, -1182,   229, -1182,   229, -1182,   229,
   -1182,   229, -1182, -1182, -1182,   229, -1182,   229, -1182,   229,
   -1182,   229, -1182,   229, -1182,   229, -1182,   229, -1182,   229,
   -1182, -1182, -1182, -1182, -1182, -1182, -1182,   229, -1182, -1182,
   -1182, -1182, -1182,   229, -1182,   229, -1182,   229, -1182,   229,
   -1182,   229, -1182,   229, -1182, -1182, -1182,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,   678,    61,    61,    61,   206,
   -1182, -1182,   102, -1182,   102, -1182,   102,   102, -1182,   102,
   -1182,   102,   102, -1182,   102,   102,   102,   102,   102,   102,
     102,   102,   102, -1182,   102, -1182,   102, -1182, -1182, -1182,
   -1182,   127, -1182, -1182, -1182, -1182, -1182,   145,   157,   171,
   -1182, -1182, -1182, -1182,   435, -1182,   102, -1182,   102,   102,
     102, -1182,   102,   229, -1182,  1485,   277,   140,   140,   587,
     316,   148,   137,   446,   122,   435,   388,   435,   391,   435,
      60,   331,   435,   302,   493, -1182, -1182, -1182,   532,   206,
     206,   206,   335,   678, -1182, -1182, -1182, -1182, -1182, -1182,
   -1182,   598, -1182, -1182, -1182, -1182,   246,   253,   259,   587,
   -1182,   102, -1182,   102,   229, -1182, -1182, -1182, -1182,   229,
     102,   229,   102, -1182,   229,   229,   102,   102, -1182, -1182,
   -1182, -1182,   102,   102,   102,   102, -1182,   102,   102,   102,
   -1182, -1182, -1182, -1182,   229,   229, -1182, -1182,   102,   229,
   -1182, -1182,   229,   102,   102, -1182,   102, -1182, -1182,   229,
   -1182,   102,   102,   229, -1182, -1182,   102,   102,   229, -1182,
   -1182,   102,   102,   229, -1182, -1182,   102,   102,   229, -1182,
   -1182,   102,   102,   229,   102,   229, -1182, -1182,   102,   229,
   -1182, -1182, -1182, -1182, -1182,   229, -1182, -1182, -1182, -1182,
   -1182, -1182,   435, -1182,   102,   102, -1182, -1182, -1182, -1182,
   -1182, -1182,   229,   102,   102,   102, -1182,   102,   229,   435,
   -1182,   474,   598, -1182, -1182,   533,   435,   108,   435,   435,
      61,    61,   104,   360,   144,    61,    61,   373,   373,   104,
     144,   373,   373,   587,   435,   406,    61,    61,   426,    61,
      61,   219,   432,   373,    61,   458, -1182,   458,    61,   432,
     373,    61,   432,   373,    61,   432,   373,    61, -1182, -1182,
     462,   114, -1182,    61,   373,    61,    61, -1182,   428,   565,
     598,   335,   464,   335,   452,   678, -1182,   598, -1182, -1182,
   -1182, -1182, -1182, -1182, -1182, -1182,   102,   102,   229, -1182,
     229, -1182, -1182,   102,   102,   229,   102, -1182, -1182, -1182,
     102,   102,   102, -1182,   102,   229, -1182, -1182, -1182, -1182,
   -1182, -1182, -1182,   229,   435,   102,   229, -1182,   102,   102,
   -1182,   102,   229,   102,   102,   435,   102,   102, -1182,   102,
   -1182,   102,   102,   102, -1182,   435,   102,   229, -1182,   102,
     102,   102,   229,   435,   102,   102,   102,   102, -1182,   435,
     435,   102,   102,   435,   102,   102,   435,   102,   102, -1182,
   -1182,   397, -1182,   435,   102, -1182,   435,   102,   102,   102,
   -1182, -1182,   102, -1182, -1182, -1182, -1182, -1182,   102,   102,
     102,   102,   259,   435,   102, -1182,   102,   229,   102, -1182,
     102,   229,   435, -1182,   461,   471,    61,    61, -1182, -1182,
     458, -1182,   912,   455,   458,   435,   277,   144,   589,   435,
     598,  1487, -1182,   432,    61,   274,   274,   432,    61,   432,
     144, -1182,   432,   432,   217,   435,   432,   274,   274, -1182,
     221, -1182,    61,   435,   277,   432,  1515, -1182,   432,   380,
   -1182, -1182, -1182, -1182,   432,    72, -1182,   432,    91, -1182,
     432,    85, -1182, -1182,   598, -1182, -1182,   598, -1182, -1182,
   -1182,   432,   148,   287,  1335, -1182, -1182, -1182,   749, -1182,
     925, -1182,   598, -1182,   464,   105,    61,   465,   435,    61,
   -1182,   102, -1182, -1182,   435, -1182,   435, -1182,   102, -1182,
     435,   102, -1182,   102, -1182,   102,   435, -1182, -1182, -1182,
     229, -1182,   259,   435, -1182, -1182, -1182, -1182, -1182, -1182,
   -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182,   102,   102,
     102, -1182, -1182,   102,   102,   102,   102,   102,   435, -1182,
     102,   435,   435,   435,   435, -1182, -1182,   102,   102,   229,
   -1182,   435,   102,   102,   102,   102, -1182, -1182,   102, -1182,
     102,   435,   102,   435,   102,   435,   435,   435,   435,   435,
     435,   435,   448,   368, -1182,   961,   435, -1182, -1182, -1182,
   -1182,   102, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182,
   -1182, -1182, -1182,   102,   229,   102,   229, -1182,   102,   229,
     102,   229,   102,   229,   102,   229,   102, -1182,   229,   102,
   -1182,   102,   102, -1182, -1182,   102, -1182,   229,   102, -1182,
   -1182,   229, -1182,   102, -1182,   229, -1182,   102,   471, -1182,
   -1182, -1182, -1182, -1182, -1182,   117, -1182,    61,   598, -1182,
     441,   441,   441,   474,   104,   111,   435,   104,   435, -1182,
     464, -1182, -1182, -1182, -1182, -1182, -1182,    61, -1182,   104,
     123,   435,   104,   435,   474,   593, -1182,   441, -1182,   219,
   -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182,   598, -1182,
   -1182,   598,   598, -1182,   383, -1182, -1182, -1182, -1182,   406,
     289,   499,   701, -1182,    61,   646, -1182,    61,   310, -1182,
     912,    95, -1182,   912,   343, -1182,   462, -1182,   398, -1182,
   -1182, -1182,   105,    61,   959,   435,    61,   458,   435,    80,
     464, -1182,   102, -1182,   102, -1182, -1182, -1182, -1182,   102,
     102,   102,   102,   587,   435,   102, -1182, -1182,   102, -1182,
     102, -1182,   102,   102, -1182, -1182, -1182,   229,   102, -1182,
   -1182,   102, -1182, -1182,   102,   102,   102,   435, -1182,   399,
     383, -1182,   961,   598, -1182,   435, -1182,   102, -1182,   102,
   -1182,   102, -1182, -1182,   435,   102,   102,   102, -1182,   435,
     102,   102, -1182,   102,   102, -1182,   102, -1182, -1182,   102,
   -1182,   435,   102,   102, -1182, -1182,   102,   102,   102,   229,
   -1182,   102, -1182, -1182, -1182, -1182, -1182, -1182,   435,   102,
     435,   435,   435,   435,   529, -1182, -1182, -1182,   435,   435,
     105,   587,   435,    61,   139,   435,   556,   435,   435, -1182,
   -1182, -1182,   598, -1182, -1182, -1182, -1182, -1182,   438, -1182,
   -1182,   310, -1182,    95, -1182, -1182, -1182,    95, -1182, -1182,
     912, -1182,   912,   462, -1182,  1000,   435,   474, -1182, -1182,
   -1182,   912,    61,   102,   105, -1182,   102,   102,   102,   102,
   -1182, -1182,   102, -1182,   102, -1182,   102, -1182, -1182, -1182,
   -1182, -1182, -1182, -1182,   229,   102, -1182,   102, -1182, -1182,
    1203,   435,   102,   102,   102, -1182,   102,   102,   102,   102,
   -1182,   102, -1182,   102, -1182, -1182,   435, -1182, -1182,   102,
     102,   102,   229,   102, -1182,   102,   435, -1182,   102,   465,
     229, -1182,   102, -1182,   539,   539,   539, -1182,   353,   435,
     435,   587,    61, -1182,   539,   695, -1182, -1182,   447,   607,
     507,    95, -1182, -1182, -1182, -1182,   912,   389,   435, -1182,
   -1182, -1182,  1523, -1182,   678, -1182,   141, -1182,   435,   229,
      61,  1136,   435, -1182,   102,   229,   102,   229,   102,   229,
     102,   102,   102, -1182, -1182,   102,   102,   102,   364,   695,
   -1182,   102,   102, -1182,   102, -1182, -1182,   102, -1182,   102,
   -1182, -1182,   102,   435, -1182, -1182, -1182, -1182, -1182, -1182,
     102, -1182,   229, -1182,    80,   102, -1182,   102,   102, -1182,
     917, -1182,    61, -1182,    61,   615, -1182,    61, -1182, -1182,
   -1182,   435,   587,   970, -1182, -1182,   607,   507,   507, -1182,
     912, -1182, -1182,   435,    61,   435,   474, -1182, -1182, -1182,
   -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182,
     229, -1182,   229,   102,   102, -1182,   102,   102, -1182,   102,
     102, -1182,   102, -1182, -1182,   102,   102,   229,   102, -1182,
   -1182, -1182, -1182,   435, -1182,   102,   102,   102,    61,    61,
   -1182, -1182,  1204,  1401, -1182,  1361,   435,  1015, -1182, -1182,
      61,   507, -1182,   587,   435,  1210,   435,   435,   102,   102,
     102, -1182, -1182, -1182, -1182, -1182, -1182, -1182,   102, -1182,
   -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182,
   -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182,
   -1182, -1182, -1182,   102,   102, -1182, -1182, -1182, -1182, -1182,
     435, -1182,   102,   102,   102,   102,   102,   102,   435, -1182,
     102, -1182,   102, -1182,   102, -1182,   102, -1182, -1182,   102,
     229, -1182, -1182,   587,   435,   501,   501,   567,   567, -1182,
     372,   323,   435,   358,   501,   411,   411, -1182,   332, -1182,
     435, -1182, -1182,    80,   102, -1182, -1182, -1182,   102,   102,
   -1182,   102,   229,   102,   229, -1182, -1182,   102,   102, -1182,
     102,   229,   102, -1182,   102,   102, -1182,   102,   102,   102,
   -1182,   102, -1182,   102, -1182,   102,   102, -1182,   102, -1182,
     102,   102, -1182,   102, -1182,   102, -1182,   435,   435, -1182,
   -1182,   372, -1182,   912,   407, -1182,   598, -1182, -1182,   372,
   -1182,   912,   407, -1182, -1182, -1182,   407, -1182, -1182, -1182,
     149, -1182, -1182,   332, -1182, -1182, -1182,   332, -1182, -1182,
   -1182, -1182,   102, -1182,   102,   102,   102,   102,   435,   102,
     102,   435,   102,   102,   102,   102,   102, -1182, -1182,   407,
   -1182,   139, -1182, -1182, -1182,   407, -1182, -1182, -1182, -1182,
   -1182, -1182, -1182,   102,   435,   102, -1182, -1182, -1182
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   423,   412,   401,   411,   172,   435,   456,   403,   484,
     487,   607,   661,   687,   690,   509,   502,   362,   562,   494,
     491,   499,   497,   631,   677,   402,   425,   436,   404,   424,
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
     427,     0,   504,   563,     0,   608,     0,   632,   646,   633,
     647,   634,   635,   649,   636,   637,   638,   639,   640,   641,
     642,   643,   644,   645,     0,   629,     0,   662,     0,     0,
       0,   664,     0,     0,    88,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    86,    87,   628,     0,     0,
       0,     0,     0,     0,    51,    31,    48,    49,    50,    52,
      53,     0,   174,    32,    33,    37,     0,    36,    46,     0,
     175,   165,   367,     0,     0,   449,   450,   375,   406,     0,
       0,     0,     0,   405,     0,     0,     0,     0,   548,   551,
     549,   552,     0,     0,     0,     0,   414,     0,   427,     0,
     437,   438,   439,   440,     0,     0,   460,   459,     0,     0,
     575,   474,     0,     0,     0,   473,     0,   571,   572,     0,
     432,   201,   428,     0,   486,   578,     0,     0,     0,   493,
     581,     0,     0,     0,   498,   584,     0,     0,     0,   516,
     512,   201,   201,     0,   201,     0,   503,   565,     0,     0,
     620,   615,   616,   610,   606,     0,   611,   621,   617,   618,
     613,   614,     0,   587,     0,     0,   609,   612,   619,   623,
     624,   630,     0,     0,     0,     0,   666,     0,     0,     0,
      45,    38,     0,    44,    34,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   429,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   508,   507,
       0,     0,   505,     0,     0,     0,     0,   585,     0,     0,
       0,     0,     0,     0,     0,     0,   663,     0,    40,    42,
      39,    47,   179,   182,   176,   177,   166,   169,     0,   171,
       0,   164,   371,     0,   357,     0,     0,   354,   359,   368,
     365,     0,     0,   377,   381,     0,   417,   228,   418,   399,
     215,   216,   217,     0,     0,     0,     0,   451,     0,     0,
     525,     0,     0,     0,     0,     0,     0,     0,   415,   408,
     422,     0,     0,     0,   465,     0,   201,     0,   461,   201,
       0,     0,     0,     0,     0,     0,   201,   201,   433,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    61,
     513,    59,   514,     0,   201,   517,     0,     0,     0,     0,
     594,   595,   596,   592,   599,   593,   600,   586,   596,   596,
     601,    91,   625,     0,     0,   528,     0,     0,     0,   698,
      91,     0,     0,    43,     0,     0,     0,     0,   356,   361,
       0,   360,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   394,     0,     0,     0,     0,     0,     0,     0,
       0,   396,     0,     0,     0,     0,     0,     0,     0,   453,
       0,   454,     0,     0,     0,     0,     0,   471,     0,     0,
     202,   430,   431,   490,     0,     0,   496,     0,     0,   501,
       0,     0,    55,    69,     0,    56,    60,     0,   511,   506,
     515,     0,     0,     0,     0,   605,   588,   589,     0,   590,
       0,   591,   626,   622,     0,     0,     0,     0,     0,     0,
     665,   167,   170,   180,     0,   183,     0,   373,   357,   372,
       0,   357,   369,   365,   364,     0,     0,   386,   387,   382,
       0,   374,   378,     0,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,     0,     0,
       0,   398,   419,     0,     0,   556,     0,   556,     0,   526,
       0,     0,     0,     0,     0,   210,   209,   201,   201,     0,
     407,     0,     0,   442,     0,   442,   455,   466,     0,   458,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    59,     0,    70,     0,     0,   566,   567,   568,
     569,     0,   185,   111,   144,   147,   155,   159,   109,   598,
      93,    99,   100,   104,     0,    96,     0,   103,    96,     0,
      96,     0,    96,     0,    96,     0,    96,    95,     0,   596,
     604,   596,   601,    92,   627,     0,   540,     0,   672,   519,
     520,     0,   529,   384,   688,     0,   689,     0,     0,   178,
     181,   358,   370,   355,   366,     0,   395,     0,   379,   376,
       0,     0,     0,    38,     0,     0,     0,     0,     0,   547,
       0,   550,   397,   553,   554,   410,   409,     0,   434,     0,
       0,     0,     0,     0,    38,     0,   472,     0,   570,     0,
     573,   576,   577,   579,   580,   582,   583,    57,     0,    54,
      79,     0,     0,    64,    82,    66,    77,    78,   561,     0,
       0,     0,     0,   102,     0,     0,   128,     0,     0,   129,
       0,     0,   130,     0,     0,   131,     0,    94,     0,   597,
     603,   602,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   168,     0,   388,   384,   380,   243,   244,   245,   201,
     201,   201,   201,     0,     0,   556,   557,   555,   556,   559,
       0,   213,     0,   442,   208,   207,   452,     0,   443,   445,
     441,   442,   447,   467,   463,     0,   201,     0,    63,    59,
      82,    71,     0,     0,    81,     0,   107,    96,   105,     0,
     101,    96,    98,   112,     0,    96,    96,    96,   145,     0,
      96,    96,   148,     0,    96,   156,     0,   160,   161,     0,
      90,     0,   654,     0,   670,   676,   672,   672,    91,     0,
     671,     0,   385,   527,   681,   682,   679,   680,     0,     0,
       0,     0,     0,     0,     0,   421,    35,   416,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   574,
      58,    80,     0,    65,    68,    83,   564,   106,     0,    97,
     110,     0,   132,     0,   133,   134,   143,     0,   135,   136,
       0,   137,     0,     0,   184,     0,     0,    38,   673,   674,
     675,     0,     0,     0,     0,   383,     0,     0,     0,   201,
     558,   560,   521,   214,   201,   446,     0,   444,   448,   199,
     197,   196,   198,   468,     0,   201,   462,     0,    75,    67,
       0,     0,   113,   114,   115,   116,    96,    96,    96,    96,
     149,     0,   157,   153,   162,   163,     0,   668,   660,   654,
     654,    91,     0,    91,   653,     0,     0,   541,   384,     0,
       0,   693,   694,   692,     0,     0,     0,   420,     0,     0,
       0,     0,     0,   464,     0,     0,    74,   108,     0,     0,
       0,     0,   138,   139,   140,   141,     0,     0,     0,   158,
     655,   656,     0,   657,     0,   659,     0,   669,     0,     0,
       0,     0,     0,   249,   219,     0,    91,     0,   225,     0,
     384,   521,   521,   518,   203,   201,     0,   476,    76,     0,
      72,   117,   118,   119,   120,   121,   122,    96,   150,     0,
     154,   152,    91,     0,   538,   533,   534,   535,   536,   537,
     384,   531,     0,   539,     0,     0,   697,   694,   694,   691,
       0,   218,     0,   223,     0,     0,   224,     0,   524,   522,
     523,     0,     0,     0,   475,    73,     0,     0,     0,   142,
       0,   658,   667,     0,     0,     0,    38,   696,   695,   194,
     191,   190,   193,   211,   192,   212,   222,   351,   186,   188,
       0,   187,     0,   219,    91,   250,     0,     0,   227,   225,
       0,   200,   201,   482,   480,    91,    91,     0,   123,   124,
     125,   126,   151,     0,   530,   205,   683,   201,     0,     0,
     221,   220,     0,     0,   226,     0,     0,     0,   477,   479,
       0,     0,   146,     0,     0,     0,     0,     0,     0,   205,
     252,   309,   310,   311,   312,   313,   314,   315,   254,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   339,   256,   258,   340,   341,   342,   343,   344,
       0,   251,   276,   303,   283,   285,   287,   289,     0,   282,
     299,   195,    91,   483,   384,   127,   201,   532,   686,    91,
       0,   678,   699,     0,     0,     0,     0,     0,     0,   246,
       0,     0,     0,     0,     0,     0,     0,   248,     0,   478,
       0,   206,   685,     0,   201,   204,   346,   350,   201,   201,
     253,   201,     0,   201,     0,   255,   348,   201,   201,   257,
     201,     0,   201,   259,   201,   201,   277,   201,   201,   201,
     304,   201,   247,   201,   284,   201,   201,   286,   201,   288,
     201,   201,   290,   201,   300,   201,   481,     0,     0,   260,
     267,     0,   264,     0,     0,   266,     0,   268,   275,     0,
     272,     0,     0,   274,   278,   281,     0,   279,   305,   308,
       0,   306,   291,     0,   293,   294,   295,     0,   297,   298,
     301,   302,   683,   189,   201,   201,     0,   201,     0,   201,
     201,     0,   201,   201,   201,   201,   201,   684,   263,     0,
     261,     0,   265,   349,   271,     0,   269,   347,   273,   280,
     307,   292,   296,   201,     0,   201,   262,   345,   270
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1182, -1182, -1182,  -212,  -220,  -181, -1182,  -187,  -178,   240,
   -1182, -1182, -1182, -1182, -1182, -1182,  -216,  -334,  -639,  -106,
    -731,  -606, -1182, -1182, -1182, -1182, -1182, -1182, -1182,  -551,
    -282, -1182, -1182, -1182,  -824, -1182, -1182,  -265,   -12,  1750,
     946,  2452, -1182,  -496,  -585,  -545, -1182, -1182,  -173, -1182,
   -1182,  -171, -1182, -1182, -1182,  -160,  -300, -1182, -1182,  -778,
   -1182, -1182, -1182, -1182, -1182,  -745, -1182, -1182, -1182, -1182,
    -749, -1182, -1182, -1182,     0, -1182, -1182, -1182, -1182, -1182,
     147, -1182, -1182,  -487, -1182, -1182,  -722, -1182, -1182,  -802,
   -1182, -1182, -1182, -1182,  -917,  1400,  -393, -1181,  -537, -1182,
   -1182, -1182,  -914,  -954,    82, -1182,  -481, -1182, -1182,  -485,
    -310,   132, -1182, -1182,  -646,  -974, -1182,  -434,  -940,  -692,
   -1182, -1028,  -582, -1182, -1182, -1182, -1182,  -588, -1182, -1182,
   -1182, -1182,  -581,  -590, -1182,  -631, -1182,  -844, -1182,  -433,
   -1182,   714,  -403,   -78,   717,  -405,    12,   -35,  -328,    99,
   -1182, -1182, -1182,   191, -1182,  -255, -1182,  -153, -1182, -1182,
   -1182, -1182, -1182, -1182,  -818, -1182, -1182, -1182,   524,   -14,
      -7,   622,   626,  -268,   -71, -1182, -1182,  -120,    54, -1182,
   -1182, -1182, -1182, -1182,  -448, -1182, -1182, -1182,     6, -1182,
     575,   -47, -1182, -1182, -1182,    -5, -1182, -1182, -1182,  -615,
   -1182, -1182, -1182,   549,   568,   500,  -183,     2,   204, -1182,
   -1182, -1182, -1182, -1182, -1182, -1182,  -358,  -762,  -893, -1182,
    -125,   642,   644, -1182,   209, -1182,  -430, -1182, -1182,   577,
       3, -1182,    -1, -1182,  -165, -1182,  -103, -1182,  -175, -1182,
    -111, -1182,  -172, -1182, -1182,   393, -1182, -1182, -1182, -1182,
   -1182,   579,  -734,  -372, -1182, -1182,  -572, -1182, -1182,  -740,
   -1182, -1182, -1182,  -758, -1182, -1182,   657,   582,   583,   605,
     591,   608, -1182, -1182,   205,  -593,   207,   208,   213,    11,
   -1182, -1182, -1182, -1182, -1182,   -98, -1182, -1182, -1182, -1182,
      17, -1182, -1182,  -100, -1182, -1182,  -104, -1182, -1182,   673,
   -1182, -1182, -1182,  -347,  -431,   210,    74, -1182, -1182, -1182,
   -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182, -1182,
   -1182, -1182,   807,  -287, -1182,  -112,   410, -1182,   250, -1182,
   -1182, -1182,  -945, -1182, -1182,  -164,   809, -1182, -1024,  -569,
   -1182,  -932,  -122, -1182, -1182, -1182,  -356, -1182,  -267
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   221,   619,   223,   873,   224,   853,
     225,   226,   408,   409,   227,   343,   228,   229,   883,   584,
     490,   585,   491,   693,   879,   586,   803,   953,   587,   804,
     882,  1009,  1010,  1099,   805,   806,   807,   884,   107,   207,
     375,   447,   911,   601,   733,   813,   710,   711,   712,   713,
     714,   715,   716,   894,  1011,   717,   718,   719,   899,   720,
     721,   903,  1021,  1109,  1193,   722,  1068,   723,   906,  1023,
     724,   725,   909,  1026,   476,   346,    41,   132,   231,   416,
     417,   418,   614,   419,   420,   616,   727,   728,  1167,  1309,
    1169,  1170,  1003,  1004,   866,   376,   668,  1171,  1214,   867,
     669,  1172,   862,   994,   439,   440,  1131,   441,   442,  1136,
     443,   648,   649,   650,   849,  1084,  1086,  1088,  1085,  1176,
    1260,  1310,  1319,  1261,  1326,  1268,  1334,  1339,  1269,  1344,
    1292,  1330,  1262,  1311,  1312,  1320,  1321,  1313,  1314,  1174,
      42,   238,   348,   531,   271,   349,   239,   134,   233,   535,
     234,   430,   623,   424,   425,   620,   618,   273,   241,   434,
     435,   633,   539,   629,   838,   630,   844,    46,    47,    48,
      49,    50,    51,   444,   136,    52,    53,   242,   426,   555,
      55,   139,   257,   459,   445,   446,   653,   854,   243,    57,
     141,   195,   282,   283,   479,    58,    59,   259,   260,   781,
     261,   262,   263,   244,   245,   448,   869,   924,   465,    61,
     144,   268,   269,   469,   466,   947,   737,   678,   874,  1005,
      62,    63,    64,   274,   473,  1144,  1186,  1187,  1274,    65,
     275,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,   202,    79,   302,   303,   493,   304,   305,
     496,   925,   738,  1049,   452,   660,   516,   517,   743,   739,
    1120,  1121,  1122,   740,   741,  1038,    80,    81,    82,   246,
      83,   247,    84,    85,   248,   766,   249,   250,   251,    86,
      87,   154,   308,   309,   701,    88,   276,   277,   278,   279,
      89,   287,   288,    90,   292,   293,    91,   297,   298,    92,
     322,   507,   508,   502,   595,   510,   599,   323,   324,    93,
     156,   325,   326,   327,   331,   332,   513,    94,   174,    95,
     175,   176,   926,   976,    97,   178,   182,   183,   337,   338,
    1031,  1032,   918,   919,    98,   835,   927,   100,   928,  1216,
     101,   745,   102,   103,   520,  1042,  1082,   521,  1043
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      40,   339,    60,    66,   222,   340,    56,   410,   342,   709,
     467,   256,    45,   730,   166,   179,   180,   989,   529,   423,
     528,   172,   165,   291,   608,   171,   930,   169,   296,   996,
    1033,   286,   735,   108,   240,   252,   695,   895,   612,   137,
     109,   110,   142,   111,   145,   112,   147,   113,   149,   114,
     151,   344,   509,   115,    54,   116,   802,   117,   954,   118,
     783,   119,   472,   120,   768,   121,   897,   122,   253,   901,
     896,   913,   904,   900,   921,   123,   917,   596,   597,   314,
    1097,   124,   328,   125,  1035,   126,   797,   127,   888,   128,
     892,   129,   455,   892,   916,   907,   892,  1095,   912,   892,
    1155,   197,   892,   199,     7,  1087,  1089,  1079,     1,    15,
    1329,   157,     5,     2,   464,   850,   851,   436,   158,    12,
     161,   702,     2,     4,   164,   104,   411,  1112,   450,   436,
     864,     5,     4,   295,    15,   865,   168,    19,  1096,     5,
     266,   876,   170,   736,     1,   280,   864,    33,  1114,   707,
     436,   865,  1115,  1116,  1117,  1118,  1173,    24,    21,   235,
     437,    34,   267,  1163,   414,   300,     5,   235,   236,   232,
     422,   213,    33,   816,   489,   819,   236,   822,   232,   825,
    1013,   827,    36,    26,   512,   458,  1056,    29,  1182,  1394,
     311,   523,   438,   522,   281,  1089,   237,   312,   860,   313,
      17,   301,   467,   316,   438,   272,   992,   130,   625,  1015,
    1119,   364,  1017,  1014,  1018,   320,  1016,  1030,  1019,    26,
    1078,   321,   350,    29,   665,   438,  1041,   351,   666,   353,
     464,  1100,   355,   356,     5,  1029,   680,  1165,     5,   892,
    1195,   892,  1024,   952,   258,   892,   181,   729,   942,   915,
     880,   731,   366,   367,    10,   472,   945,   369,  1315,  1276,
     370,   841,   938,  1197,   184,   939,  1337,   374,   929,   626,
    1279,   378,  1138,   950,   503,  1145,   381,   511,     1,  1347,
    1101,   384,   662,  1280,  1218,  1219,   387,   500,   500,  -648,
    1092,   390,   602,   393,   501,   501,   504,   395,   829,   506,
     830,   505,  1153,   396,    31,    32,     5,  -650,  1091,  1103,
    1105,  1106,  1107,  1102,  1104,   702,   431,   856,  1125,  -651,
     400,     2,    10,  1127,   632,   499,   405,     5,  1168,  1304,
     237,     4,   868,  -652,    17,   470,   702,  1163,   232,   892,
       5,  1168,   957,   237,   708,  1307,   959,    17,     5,     5,
     962,   964,   965,    26,  1307,   968,   969,    29,   432,   971,
       5,   801,   464,   706,   707,   454,   307,   461,   692,   702,
       5,   694,    31,    32,   471,     5,   336,   477,  1306,  1159,
    1307,   299,   300,  1160,  1161,  1162,   734,   105,   106,     5,
    1028,   736,   654,   656,  1307,   341,   665,  1189,  1190,  1191,
     666,  1188,   -41,   689,   672,   674,   526,   215,   527,   691,
     687,  1165,   758,   532,  1159,    10,   684,   433,  1160,  1161,
    1162,   450,   980,   540,     5,   655,   657,   842,     5,   410,
       7,   541,  1316,  1307,   544,   464,   237,   673,   675,   528,
     548,  1164,  1177,  1322,  1322,  1180,  1327,  1331,   437,  1336,
     410,  1341,  1341,    19,  1345,   562,  1300,   875,    21,   285,
     566,   480,  1275,   290,   483,    31,    32,   486,   704,   705,
     529,  1062,  1063,  1064,  1065,     5,  1164,    34,   705,   694,
      16,    10,    36,   500,  1414,  1318,  1318,   500,  1325,   515,
     501,   706,   707,   412,   501,   504,  1126,   747,   506,   519,
     706,   707,   237,     3,   413,   606,    17,  1385,  1159,   609,
     744,     6,  1160,  1161,  1162,  1390,   270,   422,     5,   -70,
       8,  1306,   582,  1307,   -70,   -70,   -70,     9,   703,  1395,
     617,    31,    32,  1396,   622,  1073,   864,  1075,   799,    14,
     751,   865,   845,   753,   910,  1166,     5,  1083,   801,  1384,
    1178,   582,  1387,   583,   667,   583,    18,  1389,  1184,    20,
    1392,   707,  1149,   999,  1393,   412,   413,  1000,  1001,  1002,
    1164,    25,   685,    27,  1159,    28,   852,    30,  1160,  1161,
    1162,   407,   878,    35,     5,   694,   881,   798,  1316,  1307,
    1133,   846,   847,   848,   726,   205,   206,  1413,   726,     9,
     726,   955,   -62,  1415,   583,   272,   329,   330,   130,   214,
     489,    14,   216,   217,   218,   951,  1151,    16,   757,   333,
     334,   335,  1263,  1083,  1270,   651,  1040,   219,   220,   658,
    1278,   661,     5,    22,   663,   664,  1164,   886,   671,   902,
     889,   702,   905,   410,   703,   704,   705,   627,   628,    30,
     683,   514,   890,   518,   232,   422,   686,   777,  1012,   688,
     706,   707,   690,     5,  1340,  1340,   694,   881,   706,   707,
     708,   611,   702,   696,  1110,   936,   801,   705,  1201,   130,
     214,   215,  1284,   216,   217,   218,   219,   220,     3,  1208,
    1209,   146,  1200,   148,  1204,   150,     6,   152,   682,   706,
     707,  1179,   811,  1335,   814,     8,  1323,   817,  1342,   820,
    1185,   823,     9,   826,    43,    11,   828,    44,     5,  1139,
    1140,   801,   754,  1040,   621,   833,  1036,   702,   310,   836,
      16,   704,   705,   839,   214,   215,  1008,   216,   217,   218,
     264,    18,  1070,  1071,    20,   214,    22,   159,   216,   217,
     218,   160,   978,   979,   706,   707,    25,   489,    27,   265,
      28,   997,    30,   993,   676,   801,     5,   162,    35,   163,
     855,  1157,  1158,   858,   681,   702,  1299,  1272,   703,   704,
     705,   315,   167,  1302,   494,   863,   317,   318,   871,  1020,
    1055,  1022,   877,   254,   694,   319,   255,   697,   173,   698,
     699,    16,   706,   707,   708,   700,   831,    96,   732,    99,
     887,   885,   891,  1397,  1128,   891,     0,    22,   891,     0,
       0,   891,   410,     0,   891,     0,     0,     0,     0,     0,
       1,     2,     0,     0,   726,     0,     3,   922,     0,  1098,
       0,     4,   214,     5,     6,   216,   217,   218,   219,   220,
       7,     0,   800,     8,     0,   943,   449,     0,     0,     0,
       9,    10,  1113,    11,     0,    12,   462,   463,     0,     0,
      13,     0,    14,   993,   475,    15,     0,     0,    16,     0,
       0,   482,     0,   694,   485,  1108,    17,   488,     0,    18,
       0,    19,    20,    21,    22,   498,     0,     0,     0,     0,
       0,     0,    23,    24,    25,    26,    27,   981,    28,    29,
      30,    31,    32,    33,     0,    34,    35,    36,     0,     0,
       0,     0,     0,     0,  1159,  1083,     0,     0,  1160,  1161,
    1162,  1163,     0,     0,     5,     0,   451,   453,     0,     0,
     456,   457,     5,   702,     0,     0,   703,   704,   705,     0,
       0,   702,   474,     0,   703,   704,   705,     0,     0,   481,
       0,   891,   484,   891,   993,   487,     0,   891,   464,  1192,
     706,   707,   708,   497,     0,   726,     5,  1159,   706,   707,
     708,  1160,  1161,  1162,  1163,   702,  1164,     5,   703,   704,
     705,     0,  1052,     0,     0,     0,   702,   736,     0,   703,
     704,   705,   914,     0,     0,  1165,  1183,     0,     0,   464,
       0,     0,   706,   707,   708,     0,     0,     5,     0,     0,
    1074,     0,     0,   706,   707,   708,   702,     0,  1080,   703,
     704,   705,     5,     0,     0,   993,     0,     0,   736,  1164,
       0,   702,  1027,   914,   703,   704,   705,     0,  1090,     0,
       0,  1183,     0,   706,   707,   708,     0,     0,  1165,   214,
     215,   891,   216,   217,   218,   219,   220,  1124,   706,   707,
     708,     0,   726,  1132,     0,  1134,     0,  1137,   185,     0,
     186,   726,   187,   188,     0,   189,     0,   190,   191,     0,
     192,   193,   194,   196,   194,   198,   194,   200,   201,     0,
     203,     0,   204,     0,     0,   993,     0,     0,   214,     0,
    1154,   216,   217,   218,   219,   220,   799,     0,   800,     0,
       0,     0,   208,     0,   209,   210,   211,     0,   212,     0,
     726,     0,     0,     0,     0,   726,     0,     0,     0,     0,
    1388,     0,     0,   726,     0,   464,     0,     0,     0,     0,
       0,     0,     0,     5,     0,     0,     0,     0,  1198,     0,
    1199,     0,   702,     0,     0,   703,   704,   705,     0,     0,
       0,     0,  1386,     0,   736,  1210,     0,   345,     0,   347,
    1391,     0,     0,     0,     0,     0,   352,     0,   354,   706,
     707,   708,   357,   358,     0,     0,     0,     0,   359,   360,
     361,   362,     0,   363,   194,   365,     0,   726,     0,     0,
       0,     0,     0,     0,   368,   726,     0,     0,     0,   371,
     372,     0,   373,     0,     0,     0,     0,     5,   377,     0,
       0,     0,   379,   380,     0,     0,   702,   382,   383,   703,
     704,   705,   385,   386,     0,     0,     0,     0,     0,     0,
     391,     0,     0,   914,   394,   744,     0,     0,     0,     0,
       0,     0,     0,   706,   707,   708,     0,     0,  1303,     0,
     398,   399,     0,     0,     0,     0,     0,     0,     0,   401,
     402,   403,     0,   404,     0,  1308,  1308,  1317,  1317,     0,
    1324,  1328,     0,  1333,  1308,  1338,  1338,     0,  1343,     0,
    1353,     0,  1356,     0,     0,     0,     0,     0,     0,  1361,
    1220,  1221,  1222,  1223,  1224,  1225,  1226,  1227,  1228,  1229,
    1230,  1231,  1232,  1233,  1234,  1235,  1236,  1237,  1238,  1239,
    1240,  1241,  1242,  1243,  1244,  1245,  1246,  1247,  1248,  1249,
    1250,  1251,  1252,  1253,  1254,  1255,  1256,  1257,  1258,  1259,
     214,   215,     5,   216,   217,   218,   219,   220,     0,     0,
     800,   702,   524,   525,   703,   704,   705,     0,     0,   347,
     530,     0,   533,     0,     0,     0,   534,   536,   537,     0,
     538,     0,     0,     0,     0,     0,     0,    16,   706,   707,
     708,   543,     0,     0,   545,   546,     0,   547,     0,   549,
     550,     0,   552,   553,     0,   554,     0,   556,   557,   558,
       0,     0,   560,     0,     0,     0,   564,   565,     0,     0,
     568,   569,     0,     0,     0,     0,     0,   574,   575,     0,
     577,   578,     0,   580,   581,     0,     0,     0,     0,     0,
       0,     0,     0,   591,   592,   593,     0,     0,   594,     0,
       0,     0,     0,     0,   594,   594,   598,   600,     0,     0,
     604,     0,   605,     0,   607,     0,   600,  1264,  1221,  1222,
    1223,  1224,  1225,  1226,  1227,  1265,  1229,  1230,  1231,  1232,
    1233,  1234,  1235,  1236,  1237,  1238,  1239,  1240,  1241,  1242,
    1243,  1244,  1245,  1246,  1247,  1248,  1249,  1250,  1251,  1252,
    1266,  1267,  1255,  1256,  1257,  1258,  1259,     0,  1221,  1222,
    1223,  1224,  1225,  1226,  1227,     0,  1229,  1230,  1231,  1232,
    1233,  1234,  1235,  1236,  1237,  1238,  1239,  1240,  1241,  1242,
    1243,  1244,  1245,  1246,  1247,  1248,  1249,  1250,  1251,  1252,
       5,     0,  1255,  1256,  1257,  1258,  1259,     0,     0,   702,
       0,     0,   703,   704,   705,     0,     0,   748,     0,     0,
       0,     0,     0,     0,   530,     0,   914,   530,     0,   534,
       0,   755,     0,     0,     0,     0,   706,   707,   708,   634,
     635,   636,   637,   638,   639,   640,   641,   642,   643,   644,
     645,   646,   647,     0,   760,   761,   762,     0,     0,   763,
     764,   765,   767,   765,     0,     0,   770,   634,   635,   636,
     637,   638,   639,   640,   641,   642,     0,     0,   779,   780,
     782,   780,     0,     0,   784,     0,   785,     0,   787,     0,
     789,   130,   214,   215,     0,   216,   217,   218,   219,   220,
       0,     0,     0,     0,     0,     0,     0,   809,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   810,
       0,   812,     0,     0,   815,     0,   818,     0,   821,     0,
     824,     0,   824,     0,     0,   594,     0,   594,   598,     0,
       0,   832,     0,     0,   834,     0,     0,     0,     0,   837,
       0,     0,     0,   840,     0,     0,     0,     0,     0,     0,
       0,   388,   389,     0,   392,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   755,     0,
     837,     0,     0,     0,     0,     0,     0,     0,   934,     0,
       0,   765,     0,     0,   765,     0,   940,     0,   941,   780,
       0,     0,     0,     0,   944,     0,     0,   780,     0,     0,
     946,   785,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   824,     0,   958,     0,   824,     0,     0,
       0,   961,   963,   824,     0,     0,   967,   824,     0,   970,
     824,     0,   972,     0,     0,   973,     0,     0,   975,   977,
       0,     0,   834,   834,   600,     0,   561,   982,     0,   563,
       0,     0,     0,     0,     0,   984,   570,   571,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   589,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1039,
       0,     0,  1044,  1045,  1046,     0,     0,     0,  1048,     0,
       0,     0,  1051,     0,     0,   284,     0,   289,     0,   294,
       0,     0,   306,  1054,     0,     0,     0,     0,  1058,  1059,
    1060,     0,  1061,   824,   824,   824,     0,  1066,     0,  1067,
       0,     0,     0,     0,     0,   975,   975,  1072,     0,   600,
       0,  1076,     0,     0,   837,     0,     0,     0,  1081,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1130,     0,   600,     0,  1135,     0,   837,  1048,  1048,     0,
       0,     0,  1142,  1143,     0,     0,     0,  1146,  1147,     0,
    1148,     0,     0,   824,     0,  1150,     0,     0,   600,     0,
       0,     0,     0,     0,     0,     0,   837,   775,   776,     0,
       0,  1156,   397,  1081,  1081,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   406,
       0,     0,     0,     0,     0,     0,   421,     0,   427,   428,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   460,     0,     0,     0,     0,  1130,
     600,     0,  1202,  1203,     0,  1135,  1205,     0,     0,     0,
       0,  1207,   600,     0,  1211,     0,     0,     0,     0,     0,
       0,  1213,  1215,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1283,  1213,  1285,     0,     0,     0,
       0,     0,     0,     0,  1286,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   542,     0,     0,     0,     0,  1287,
    1288,     0,     0,     0,     0,   551,     0,     0,  1290,  1291,
    1293,  1294,  1295,  1296,     0,   559,  1298,     0,   600,     0,
     837,     0,     0,   567,     0,   600,     0,     0,     0,   572,
     573,     0,     0,   576,     0,     0,   579,     0,     0,     0,
       0,     0,     0,   588,     0,     0,   590,     0,     0,   931,
     932,   933,   935,     0,     0,     0,     0,  1351,     0,  1354,
       0,     0,     0,   603,     0,     0,  1359,     0,  1362,     0,
       0,     0,   610,  1366,     0,     0,   948,  1370,     0,     0,
       0,  1373,     0,     0,     0,   624,  1377,     0,     0,   631,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   670,     0,     0,     0,     0,
       0,     0,     0,   679,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1215,     0,
       0,  1399,  1401,     0,     0,     0,  1405,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   746,     0,
       0,     0,     0,     0,   749,     0,   750,     0,     0,     0,
     752,     0,     0,     0,     0,     0,   756,     0,     0,     0,
       0,     0,     0,   759,     0,     0,     0,     0,     0,  1047,
       0,     0,     0,     0,  1050,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1053,     0,     0,   769,     0,
       0,   771,   772,   773,   774,     0,     0,     0,     0,     0,
       0,   778,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   786,     0,   788,     0,   790,   791,   792,   793,   794,
     795,   796,     0,     0,     0,     0,   808,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1141,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   857,     0,   859,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   870,     0,   872,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   131,
     133,   135,   135,   138,   140,   135,   143,   135,   140,   135,
     140,   135,   140,   135,   140,   153,   155,     0,   177,   177,
     177,     0,  1206,     0,     0,   920,     0,     0,   923,     0,
       0,     0,     0,     0,     0,     0,     0,  1217,     0,     0,
       0,     0,     0,     0,   937,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   949,     0,     0,
       0,     0,     0,     0,     0,   956,     0,   230,     0,     0,
       0,     0,     0,     0,   960,     0,     0,     0,     0,   966,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   974,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1301,     0,   983,     0,
     985,   986,   987,   988,     0,     0,     0,     0,   990,   991,
       0,     0,   995,     0,     0,   998,     0,  1006,  1007,     0,
       0,     0,     0,     0,  1348,     0,     0,     0,  1349,  1350,
       0,  1352,     0,  1355,     0,     0,     0,  1357,  1358,     0,
    1360,     0,  1363,     0,  1364,  1365,  1034,  1367,  1368,  1369,
       0,  1371,     0,  1372,     0,  1374,  1375,     0,  1376,     0,
    1378,  1379,     0,  1380,     0,  1381,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1057,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1069,     0,     0,     0,
       0,     0,     0,     0,  1398,  1400,  1077,  1402,     0,  1404,
    1406,     0,  1408,  1409,  1410,  1411,  1412,   415,     0,  1093,
    1094,     0,   429,   133,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1416,     0,  1418,     0,     0,  1111,     0,
       0,   468,   133,     0,     0,     0,     0,     0,  1123,     0,
     478,     0,  1129,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   492,     0,     0,   495,     0,     0,   135,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1152,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1181,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1194,     0,  1196,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1212,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1271,     0,     0,     0,
       0,     0,     0,     0,  1277,     0,  1281,  1282,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   613,   615,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   652,     0,     0,     0,
     659,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1289,     0,     0,     0,   677,     0,     0,     0,  1297,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1305,     0,     0,     0,     0,     0,
       0,     0,  1332,     0,     0,     0,     0,     0,     0,     0,
    1346,     0,     0,     0,     0,     0,     0,     0,   742,     0,
       0,   177,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1382,  1383,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1403,     0,
       0,  1407,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1417,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   843,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   861,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   893,     0,     0,   898,
       0,     0,     0,     0,     0,     0,     0,     0,   908,     0,
       0,     0,     0,     0,     0,   677,     0,     0,   677,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   861,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1025,     0,     0,     0,     0,
       0,     0,     0,     0,  1037,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   861,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   677,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1175,     0,  1175,     0,     0,  1175,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   861,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     861,   861,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1273
};

static const yytype_int16 yycheck[] =
{
       0,   213,     0,     0,   185,   221,     0,   341,   228,   594,
     368,   189,     0,   598,   125,   127,   128,   934,   423,   347,
     423,   125,   125,   198,   520,   125,   844,   125,   200,   943,
     975,   196,   604,    45,   187,   188,   587,   815,   525,   110,
      52,    53,   113,    55,   115,    57,   117,    59,   119,    61,
     121,   229,   399,    65,     0,    67,   695,    69,   882,    71,
     675,    73,   372,    75,   657,    77,   815,    79,   188,   818,
     815,   833,   821,   818,   836,    87,   834,   508,   509,   204,
    1054,    93,   204,    95,   977,    97,   692,    99,   810,   101,
     812,   103,   360,   815,   834,   826,   818,  1051,   832,   821,
    1124,   148,   824,   150,    24,  1045,  1046,  1039,     4,    49,
    1291,   125,    17,     5,     9,   761,   762,     6,   125,    39,
     125,    26,     5,    15,   125,     0,   342,  1072,    48,     6,
       7,    17,    15,    73,    49,    12,   125,    65,  1052,    17,
       3,   787,   125,    38,     4,    23,     7,    87,     7,    54,
       6,    12,    11,    12,    13,    14,  1130,    77,    67,    19,
      16,    89,    25,    14,   345,    51,    17,    19,    28,    61,
      62,   183,    87,   718,   390,   720,    28,   722,    61,   724,
     958,   726,    91,    79,   400,   363,  1010,    83,  1142,  1370,
     204,   407,    81,   405,   194,  1135,    56,   204,   770,   204,
      60,   201,   560,   204,    81,   193,   940,   146,   536,   958,
      69,   258,   961,   958,   963,   204,   961,   975,   967,    79,
    1038,   204,   234,    83,     7,    81,   984,   239,    11,   241,
       9,  1055,   244,   245,    17,   975,   564,    88,    17,   961,
    1154,   963,   973,   882,   190,   967,    40,   594,   863,   834,
     801,   598,   264,   265,    35,   565,   871,   269,  1286,  1213,
     272,   748,   855,  1156,   162,   858,  1294,   279,   840,   537,
    1215,   283,  1090,   879,   399,  1099,   288,   399,     4,  1303,
    1058,   293,   550,  1215,  1198,  1199,   298,   398,   399,   162,
    1048,   303,   512,   305,   398,   399,   399,   309,   729,   399,
     731,   399,  1120,   315,    85,    86,    17,   162,  1048,  1058,
    1059,  1060,  1061,  1058,  1059,    26,   351,   765,  1080,   162,
     332,     5,    35,  1081,   540,   396,   338,    17,  1130,  1283,
      56,    15,   780,   162,    60,   370,    26,    14,    61,  1061,
      17,  1143,   887,    56,    55,    22,   891,    60,    17,    17,
     895,   896,   897,    79,    22,   900,   901,    83,   352,   904,
      17,   695,     9,    53,    54,   359,    64,   365,   584,    26,
      17,   587,    85,    86,   371,    17,    41,   377,    20,     7,
      22,    50,    51,    11,    12,    13,   602,   158,   159,    17,
     975,    38,   545,   546,    22,   149,     7,  1146,  1147,  1148,
      11,  1146,   149,   578,   557,   558,   418,   148,   420,   581,
     575,    88,   632,   425,     7,    35,   569,    57,    11,    12,
      13,    48,   918,   435,    17,   545,   546,   755,    17,   763,
      24,   443,    21,    22,   446,     9,    56,   557,   558,   842,
     452,    69,  1134,  1287,  1288,  1137,  1290,  1291,    16,  1293,
     784,  1295,  1296,    65,  1298,   467,  1274,   785,    67,    71,
     472,   379,  1211,    72,   382,    85,    86,   385,    30,    31,
     875,  1016,  1017,  1018,  1019,    17,    69,    89,    31,   695,
      52,    35,    91,   594,  1401,  1287,  1288,   598,  1290,    25,
     594,    53,    54,    32,   598,   598,  1081,   609,   598,    47,
      53,    54,    56,    10,    33,   517,    60,  1351,     7,   521,
      45,    18,    11,    12,    13,  1359,    70,    62,    17,   155,
      27,    20,   154,    22,   160,   161,   162,    34,    29,  1373,
     530,    85,    86,  1377,   534,  1031,     7,  1033,   155,    46,
     618,    12,   758,   621,   146,  1130,    17,     8,   882,  1351,
    1135,   154,  1354,   156,   554,   156,    63,  1359,  1143,    66,
    1362,    54,  1107,     7,  1366,    32,    33,    11,    12,    13,
      69,    78,   569,    80,     7,    82,   763,    84,    11,    12,
      13,   341,   798,    90,    17,   801,   802,   693,    21,    22,
    1086,   150,   151,   152,   594,   160,   161,  1399,   598,    34,
     600,   883,   154,  1405,   156,   593,    74,    75,   146,   147,
     826,    46,   150,   151,   152,   880,  1112,    52,   630,   209,
     210,   211,  1203,     8,  1205,   543,   984,   153,   154,   547,
    1215,   549,    17,    68,   552,   553,    69,   810,   556,   820,
     811,    26,   823,   977,    29,    30,    31,    58,    59,    84,
     568,   401,   812,   403,    61,    62,   574,   669,   958,   577,
      53,    54,   580,    17,  1295,  1296,   882,   883,    53,    54,
      55,   524,    26,   591,  1067,   853,  1010,    31,  1174,   146,
     147,   148,  1219,   150,   151,   152,   153,   154,    10,  1185,
    1186,   116,  1173,   118,  1179,   120,    18,   122,   566,    53,
      54,  1135,   714,  1293,   716,    27,  1288,   719,  1296,   721,
    1143,   723,    34,   725,     0,    37,   728,     0,    17,  1091,
    1092,  1055,   623,  1081,   533,   737,   981,    26,   204,   741,
      52,    30,    31,   745,   147,   148,   952,   150,   151,   152,
     191,    63,  1029,  1030,    66,   147,    68,   125,   150,   151,
     152,   125,   916,   917,    53,    54,    78,   973,    80,   191,
      82,   944,    84,   941,   560,  1099,    17,   125,    90,   125,
     764,  1127,  1128,   767,   565,    26,  1272,  1207,    29,    30,
      31,   204,   125,  1279,   391,   779,   204,   204,   782,   970,
    1010,   972,   789,   188,  1010,   204,   188,   592,   125,   592,
     592,    52,    53,    54,    55,   592,   732,     0,   598,     0,
     810,   809,   812,  1382,  1081,   815,    -1,    68,   818,    -1,
      -1,   821,  1156,    -1,   824,    -1,    -1,    -1,    -1,    -1,
       4,     5,    -1,    -1,   834,    -1,    10,   837,    -1,  1055,
      -1,    15,   147,    17,    18,   150,   151,   152,   153,   154,
      24,    -1,   157,    27,    -1,   867,   356,    -1,    -1,    -1,
      34,    35,  1074,    37,    -1,    39,   366,   367,    -1,    -1,
      44,    -1,    46,  1051,   374,    49,    -1,    -1,    52,    -1,
      -1,   381,    -1,  1099,   384,  1066,    60,   387,    -1,    63,
      -1,    65,    66,    67,    68,   395,    -1,    -1,    -1,    -1,
      -1,    -1,    76,    77,    78,    79,    80,   919,    82,    83,
      84,    85,    86,    87,    -1,    89,    90,    91,    -1,    -1,
      -1,    -1,    -1,    -1,     7,     8,    -1,    -1,    11,    12,
      13,    14,    -1,    -1,    17,    -1,   357,   358,    -1,    -1,
     361,   362,    17,    26,    -1,    -1,    29,    30,    31,    -1,
      -1,    26,   373,    -1,    29,    30,    31,    -1,    -1,   380,
      -1,   961,   383,   963,  1142,   386,    -1,   967,     9,  1150,
      53,    54,    55,   394,    -1,   975,    17,     7,    53,    54,
      55,    11,    12,    13,    14,    26,    69,    17,    29,    30,
      31,    -1,  1004,    -1,    -1,    -1,    26,    38,    -1,    29,
      30,    31,    43,    -1,    -1,    88,    36,    -1,    -1,     9,
      -1,    -1,    53,    54,    55,    -1,    -1,    17,    -1,    -1,
    1032,    -1,    -1,    53,    54,    55,    26,    -1,  1040,    29,
      30,    31,    17,    -1,    -1,  1213,    -1,    -1,    38,    69,
      -1,    26,    42,    43,    29,    30,    31,    -1,  1048,    -1,
      -1,    36,    -1,    53,    54,    55,    -1,    -1,    88,   147,
     148,  1061,   150,   151,   152,   153,   154,  1079,    53,    54,
      55,    -1,  1072,  1085,    -1,  1087,    -1,  1089,   132,    -1,
     134,  1081,   136,   137,    -1,   139,    -1,   141,   142,    -1,
     144,   145,   146,   147,   148,   149,   150,   151,   152,    -1,
     154,    -1,   156,    -1,    -1,  1283,    -1,    -1,   147,    -1,
    1122,   150,   151,   152,   153,   154,   155,    -1,   157,    -1,
      -1,    -1,   176,    -1,   178,   179,   180,    -1,   182,    -1,
    1130,    -1,    -1,    -1,    -1,  1135,    -1,    -1,    -1,    -1,
    1356,    -1,    -1,  1143,    -1,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,  1170,    -1,
    1172,    -1,    26,    -1,    -1,    29,    30,    31,    -1,    -1,
      -1,    -1,  1353,    -1,    38,  1187,    -1,   231,    -1,   233,
    1361,    -1,    -1,    -1,    -1,    -1,   240,    -1,   242,    53,
      54,    55,   246,   247,    -1,    -1,    -1,    -1,   252,   253,
     254,   255,    -1,   257,   258,   259,    -1,  1207,    -1,    -1,
      -1,    -1,    -1,    -1,   268,  1215,    -1,    -1,    -1,   273,
     274,    -1,   276,    -1,    -1,    -1,    -1,    17,   282,    -1,
      -1,    -1,   286,   287,    -1,    -1,    26,   291,   292,    29,
      30,    31,   296,   297,    -1,    -1,    -1,    -1,    -1,    -1,
     304,    -1,    -1,    43,   308,    45,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    53,    54,    55,    -1,    -1,  1280,    -1,
     324,   325,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   333,
     334,   335,    -1,   337,    -1,  1285,  1286,  1287,  1288,    -1,
    1290,  1291,    -1,  1293,  1294,  1295,  1296,    -1,  1298,    -1,
    1312,    -1,  1314,    -1,    -1,    -1,    -1,    -1,    -1,  1321,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     147,   148,    17,   150,   151,   152,   153,   154,    -1,    -1,
     157,    26,   416,   417,    29,    30,    31,    -1,    -1,   423,
     424,    -1,   426,    -1,    -1,    -1,   430,   431,   432,    -1,
     434,    -1,    -1,    -1,    -1,    -1,    -1,    52,    53,    54,
      55,   445,    -1,    -1,   448,   449,    -1,   451,    -1,   453,
     454,    -1,   456,   457,    -1,   459,    -1,   461,   462,   463,
      -1,    -1,   466,    -1,    -1,    -1,   470,   471,    -1,    -1,
     474,   475,    -1,    -1,    -1,    -1,    -1,   481,   482,    -1,
     484,   485,    -1,   487,   488,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   497,   498,   499,    -1,    -1,   502,    -1,
      -1,    -1,    -1,    -1,   508,   509,   510,   511,    -1,    -1,
     514,    -1,   516,    -1,   518,    -1,   520,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,    -1,   107,   108,
     109,   110,   111,   112,   113,    -1,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
      17,    -1,   141,   142,   143,   144,   145,    -1,    -1,    26,
      -1,    -1,    29,    30,    31,    -1,    -1,   611,    -1,    -1,
      -1,    -1,    -1,    -1,   618,    -1,    43,   621,    -1,   623,
      -1,   625,    -1,    -1,    -1,    -1,    53,    54,    55,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,    -1,   648,   649,   650,    -1,    -1,   653,
     654,   655,   656,   657,    -1,    -1,   660,    92,    93,    94,
      95,    96,    97,    98,    99,   100,    -1,    -1,   672,   673,
     674,   675,    -1,    -1,   678,    -1,   680,    -1,   682,    -1,
     684,   146,   147,   148,    -1,   150,   151,   152,   153,   154,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   701,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   713,
      -1,   715,    -1,    -1,   718,    -1,   720,    -1,   722,    -1,
     724,    -1,   726,    -1,    -1,   729,    -1,   731,   732,    -1,
      -1,   735,    -1,    -1,   738,    -1,    -1,    -1,    -1,   743,
      -1,    -1,    -1,   747,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   301,   302,    -1,   304,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   842,    -1,
     844,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   852,    -1,
      -1,   855,    -1,    -1,   858,    -1,   860,    -1,   862,   863,
      -1,    -1,    -1,    -1,   868,    -1,    -1,   871,    -1,    -1,
     874,   875,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   887,    -1,   889,    -1,   891,    -1,    -1,
      -1,   895,   896,   897,    -1,    -1,   900,   901,    -1,   903,
     904,    -1,   906,    -1,    -1,   909,    -1,    -1,   912,   913,
      -1,    -1,   916,   917,   918,    -1,   466,   921,    -1,   469,
      -1,    -1,    -1,    -1,    -1,   929,   476,   477,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   494,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   983,
      -1,    -1,   986,   987,   988,    -1,    -1,    -1,   992,    -1,
      -1,    -1,   996,    -1,    -1,   195,    -1,   197,    -1,   199,
      -1,    -1,   202,  1007,    -1,    -1,    -1,    -1,  1012,  1013,
    1014,    -1,  1016,  1017,  1018,  1019,    -1,  1021,    -1,  1023,
      -1,    -1,    -1,    -1,    -1,  1029,  1030,  1031,    -1,  1033,
      -1,  1035,    -1,    -1,  1038,    -1,    -1,    -1,  1042,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1084,    -1,  1086,    -1,  1088,    -1,  1090,  1091,  1092,    -1,
      -1,    -1,  1096,  1097,    -1,    -1,    -1,  1101,  1102,    -1,
    1104,    -1,    -1,  1107,    -1,  1109,    -1,    -1,  1112,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1120,   667,   668,    -1,
      -1,  1125,   322,  1127,  1128,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   339,
      -1,    -1,    -1,    -1,    -1,    -1,   346,    -1,   348,   349,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   364,    -1,    -1,    -1,    -1,  1173,
    1174,    -1,  1176,  1177,    -1,  1179,  1180,    -1,    -1,    -1,
      -1,  1185,  1186,    -1,  1188,    -1,    -1,    -1,    -1,    -1,
      -1,  1195,  1196,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1218,  1219,  1220,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1228,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   444,    -1,    -1,    -1,    -1,  1253,
    1254,    -1,    -1,    -1,    -1,   455,    -1,    -1,  1262,  1263,
    1264,  1265,  1266,  1267,    -1,   465,  1270,    -1,  1272,    -1,
    1274,    -1,    -1,   473,    -1,  1279,    -1,    -1,    -1,   479,
     480,    -1,    -1,   483,    -1,    -1,   486,    -1,    -1,    -1,
      -1,    -1,    -1,   493,    -1,    -1,   496,    -1,    -1,   849,
     850,   851,   852,    -1,    -1,    -1,    -1,  1311,    -1,  1313,
      -1,    -1,    -1,   513,    -1,    -1,  1320,    -1,  1322,    -1,
      -1,    -1,   522,  1327,    -1,    -1,   876,  1331,    -1,    -1,
      -1,  1335,    -1,    -1,    -1,   535,  1340,    -1,    -1,   539,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   555,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   563,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1382,    -1,
      -1,  1385,  1386,    -1,    -1,    -1,  1390,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   608,    -1,
      -1,    -1,    -1,    -1,   614,    -1,   616,    -1,    -1,    -1,
     620,    -1,    -1,    -1,    -1,    -1,   626,    -1,    -1,    -1,
      -1,    -1,    -1,   633,    -1,    -1,    -1,    -1,    -1,   989,
      -1,    -1,    -1,    -1,   994,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1005,    -1,    -1,   658,    -1,
      -1,   661,   662,   663,   664,    -1,    -1,    -1,    -1,    -1,
      -1,   671,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   681,    -1,   683,    -1,   685,   686,   687,   688,   689,
     690,   691,    -1,    -1,    -1,    -1,   696,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1095,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   766,    -1,   768,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   781,    -1,   783,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,    -1,   126,   127,
     128,    -1,  1182,    -1,    -1,   835,    -1,    -1,   838,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1197,    -1,    -1,
      -1,    -1,    -1,    -1,   854,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   877,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   885,    -1,   185,    -1,    -1,
      -1,    -1,    -1,    -1,   894,    -1,    -1,    -1,    -1,   899,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   911,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1276,    -1,   928,    -1,
     930,   931,   932,   933,    -1,    -1,    -1,    -1,   938,   939,
      -1,    -1,   942,    -1,    -1,   945,    -1,   947,   948,    -1,
      -1,    -1,    -1,    -1,  1304,    -1,    -1,    -1,  1308,  1309,
      -1,  1311,    -1,  1313,    -1,    -1,    -1,  1317,  1318,    -1,
    1320,    -1,  1322,    -1,  1324,  1325,   976,  1327,  1328,  1329,
      -1,  1331,    -1,  1333,    -1,  1335,  1336,    -1,  1338,    -1,
    1340,  1341,    -1,  1343,    -1,  1345,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1011,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1026,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1384,  1385,  1036,  1387,    -1,  1389,
    1390,    -1,  1392,  1393,  1394,  1395,  1396,   345,    -1,  1049,
    1050,    -1,   350,   351,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1413,    -1,  1415,    -1,    -1,  1068,    -1,
      -1,   369,   370,    -1,    -1,    -1,    -1,    -1,  1078,    -1,
     378,    -1,  1082,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   390,    -1,    -1,   393,    -1,    -1,   396,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1113,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1141,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1153,    -1,  1155,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1193,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1206,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1214,    -1,  1216,  1217,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   526,   527,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   544,    -1,    -1,    -1,
     548,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1260,    -1,    -1,    -1,   562,    -1,    -1,    -1,  1268,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1284,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1292,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1300,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   606,    -1,
      -1,   609,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1347,  1348,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1388,    -1,
      -1,  1391,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1414,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   757,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   777,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   814,    -1,    -1,   817,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   826,    -1,
      -1,    -1,    -1,    -1,    -1,   833,    -1,    -1,   836,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   943,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   973,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   982,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1052,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1080,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1132,    -1,  1134,    -1,    -1,  1137,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1154,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1198,  1199,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1210
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
     203,   203,   203,   203,   354,   203,   201,   201,   203,   201,
     201,   203,   203,   203,   201,   203,   258,   203,   201,   203,
     203,   201,   203,   203,   201,   203,   203,   201,   258,   258,
     201,   203,   258,   201,   203,   201,   201,   202,   203,   203,
     201,   203,   203,   203,   203,   201,   202,   172,   175,   176,
     180,   179,    32,    33,   168,   204,   242,   243,   244,   246,
     247,   202,    62,   311,   316,   317,   341,   202,   202,   204,
     314,   310,   351,    57,   322,   323,     6,    16,    81,   267,
     268,   270,   271,   273,   336,   347,   348,   204,   368,   368,
      48,   414,   417,   414,   351,   336,   414,   414,   171,   346,
     202,   370,   368,   368,     9,   371,   377,   379,   204,   376,
     310,   393,   273,   387,   414,   368,   237,   237,   204,   357,
     267,   414,   368,   267,   414,   368,   267,   414,   368,   179,
     183,   185,   204,   410,   408,   204,   413,   414,   368,   337,
     403,   459,   466,   383,   399,   448,   456,   464,   465,   466,
     468,   505,   179,   479,   491,    25,   419,   420,   491,    47,
     507,   510,   166,   179,   203,   203,   201,   201,   305,   308,
     203,   306,   201,   203,   203,   312,   203,   203,   203,   325,
     201,   201,   202,   203,   201,   203,   203,   203,   201,   203,
     203,   202,   203,   203,   203,   342,   203,   203,   203,   202,
     203,   258,   201,   258,   203,   203,   201,   202,   203,   203,
     258,   258,   202,   202,   203,   203,   202,   203,   203,   202,
     203,   203,   154,   156,   182,   184,   188,   191,   202,   258,
     202,   203,   203,   203,   203,   467,   467,   467,   203,   469,
     203,   206,   167,   202,   203,   203,   201,   203,   206,   201,
     202,   243,   246,   204,   245,   204,   248,   237,   319,   168,
     318,   316,   237,   315,   202,   311,   336,    58,    59,   326,
     328,   202,   179,   324,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   274,   275,
     276,   267,   204,   349,   320,   340,   320,   340,   267,   204,
     418,   267,   336,   267,   267,     7,    11,   237,   259,   263,
     202,   267,   320,   340,   320,   340,   371,   204,   380,   202,
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
     203,   203,   362,   201,   203,   362,   203,   378,   258,   202,
     184,   200,   181,   190,   197,   193,   202,   208,   203,   208,
     202,   203,   208,   203,   208,   208,   202,   203,   208,   208,
     203,   208,   203,   203,   202,   203,   486,   203,   498,   498,
     206,   201,   203,   202,   203,   202,   202,   202,   202,   257,
     202,   202,   415,   171,   266,   202,   265,   369,   202,     7,
      11,    12,    13,   255,   256,   382,   202,   202,   179,   194,
     195,   217,   219,   222,   228,   233,   228,   233,   233,   233,
     168,   225,   168,   232,   183,   204,   236,    42,   207,   422,
     426,   493,   494,   495,   202,   381,   318,   204,   428,   203,
     379,   426,   508,   511,   203,   203,   203,   258,   203,   416,
     258,   203,   201,   258,   203,   167,   197,   202,   203,   203,
     203,   203,   208,   208,   208,   208,   203,   203,   229,   202,
     486,   486,   203,   206,   201,   206,   203,   202,   327,   504,
     201,   203,   509,     8,   278,   281,   279,   281,   280,   281,
     237,   422,   426,   202,   202,   266,   265,   278,   179,   196,
     197,   222,   228,   233,   228,   233,   233,   233,   168,   226,
     259,   202,   495,   166,     7,    11,    12,    13,    14,    69,
     423,   424,   425,   202,   201,   380,   207,   426,   511,   202,
     203,   269,   201,   206,   201,   203,   272,   201,   327,   416,
     416,   258,   203,   203,   388,   197,   203,   203,   203,   208,
     203,   206,   202,   327,   201,   501,   203,   509,   509,     7,
      11,    12,    13,    14,    69,    88,   207,   251,   252,   253,
     254,   260,   264,   278,   302,   204,   282,   282,   207,   280,
     282,   202,   266,    36,   207,   302,   389,   390,   228,   233,
     233,   233,   168,   227,   202,   265,   202,   381,   201,   201,
     269,   206,   203,   203,   272,   203,   258,   203,   206,   206,
     201,   203,   202,   203,   261,   203,   502,   258,   265,   265,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     283,   286,   295,   295,   106,   114,   139,   140,   288,   291,
     295,   202,   389,   204,   391,   233,   266,   202,   207,   495,
     504,   202,   202,   203,   261,   203,   203,   203,   203,   202,
     203,   203,   293,   203,   203,   203,   203,   202,   203,   206,
     327,   258,   206,   201,   266,   202,    20,    22,   237,   252,
     284,   296,   297,   300,   301,   284,    21,   237,   252,   285,
     298,   299,   300,   285,   237,   252,   287,   300,   237,   260,
     294,   300,   202,   237,   289,   296,   300,   284,   237,   290,
     298,   300,   290,   237,   292,   300,   202,   501,   258,   258,
     258,   203,   258,   201,   203,   258,   201,   258,   258,   203,
     258,   201,   203,   258,   258,   258,   203,   258,   258,   258,
     203,   258,   258,   203,   258,   258,   258,   203,   258,   258,
     258,   258,   202,   202,   252,   300,   168,   252,   179,   252,
     300,   168,   252,   252,   260,   300,   300,   502,   258,   203,
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
     433,   434,   434,   435,   436,   437,   438,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   447,   447,   447,
     448,   449,   449,   450,   451,   452,   453,   454,   455,   456,
     457,   458,   459,   460,   461,   462,   463,   463,   464,   464,
     464,   464,   465,   465,   466,   466,   467,   467,   467,   468,
     468,   469,   469,   469,   469,   470,   471,   472,   473,   474,
     474,   474,   474,   474,   474,   474,   474,   474,   474,   474,
     475,   476,   477,   478,   478,   479,   479,   479,   480,   481,
     481,   482,   483,   483,   483,   483,   483,   483,   483,   483,
     483,   483,   483,   483,   483,   483,   484,   484,   484,   484,
     484,   484,   484,   485,   486,   486,   486,   486,   486,   486,
     486,   487,   488,   489,   490,   491,   492,   493,   494,   495,
     496,   497,   498,   498,   498,   498,   498,   499,   500,   501,
     501,   501,   501,   502,   502,   502,   502,   503,   504,   505,
     506,   507,   508,   508,   509,   509,   509,   509,   510,   511
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
      10,     1,     1,    10,    10,     7,     0,     2,     9,     7,
       9,    10,     1,     1,     8,     1,     1,     1,     1,     1,
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
