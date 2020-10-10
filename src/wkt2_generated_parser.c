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
#define YYLAST   3132

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  164
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  353
/* YYNRULES -- Number of rules.  */
#define YYNRULES  694
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1427

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
    1070,  1075,  1076,  1078,  1080,  1082,  1084,  1088,  1088,  1090,
    1096,  1103,  1103,  1106,  1113,  1114,  1115,  1116,  1117,  1119,
    1123,  1125,  1127,  1127,  1131,  1136,  1136,  1136,  1140,  1145,
    1145,  1147,  1151,  1151,  1155,  1160,  1162,  1166,  1166,  1170,
    1175,  1177,  1181,  1182,  1183,  1184,  1185,  1187,  1187,  1189,
    1192,  1194,  1194,  1196,  1198,  1200,  1204,  1210,  1211,  1212,
    1213,  1215,  1217,  1221,  1226,  1228,  1231,  1236,  1240,  1246,
    1246,  1246,  1246,  1246,  1246,  1250,  1255,  1257,  1262,  1262,
    1263,  1265,  1265,  1267,  1274,  1274,  1276,  1283,  1283,  1285,
    1292,  1299,  1304,  1305,  1306,  1308,  1314,  1319,  1327,  1333,
    1335,  1337,  1343,  1345,  1345,  1346,  1346,  1350,  1356,  1356,
    1358,  1363,  1369,  1374,  1380,  1385,  1390,  1396,  1401,  1406,
    1412,  1417,  1422,  1428,  1428,  1429,  1429,  1430,  1430,  1431,
    1431,  1432,  1432,  1433,  1433,  1436,  1436,  1438,  1439,  1440,
    1442,  1444,  1448,  1451,  1451,  1454,  1455,  1456,  1458,  1462,
    1463,  1465,  1467,  1467,  1468,  1468,  1469,  1469,  1469,  1470,
    1471,  1471,  1472,  1472,  1473,  1473,  1475,  1475,  1476,  1476,
    1477,  1478,  1478,  1482,  1486,  1487,  1490,  1495,  1496,  1497,
    1498,  1499,  1500,  1501,  1503,  1505,  1507,  1510,  1512,  1514,
    1516,  1518,  1520,  1522,  1524,  1526,  1528,  1533,  1537,  1538,
    1541,  1546,  1547,  1548,  1549,  1550,  1552,  1557,  1562,  1563,
    1566,  1572,  1572,  1572,  1572,  1574,  1575,  1576,  1577,  1579,
    1581,  1586,  1592,  1594,  1599,  1600,  1603,  1609,  1609,  1611,
    1612,  1613,  1614,  1616,  1618
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

#define YYTABLE_NINF -633

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    2573, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232,   114, -1232, -1232,
   -1232,   231, -1232, -1232, -1232,   231, -1232, -1232, -1232, -1232,
   -1232, -1232,   231,   231, -1232,   231, -1232,   -75,   231, -1232,
     231, -1232,   231, -1232, -1232, -1232,   231, -1232,   231, -1232,
     231, -1232,   231, -1232,   231, -1232,   231, -1232,   231, -1232,
     231, -1232, -1232, -1232, -1232, -1232, -1232, -1232,   231, -1232,
   -1232, -1232, -1232, -1232, -1232,   231, -1232,   231, -1232,   231,
   -1232,   231, -1232,   231, -1232,   231, -1232, -1232, -1232,    33,
      33,    33,    33,    33, -1232,    81,    33,    33,    33,    33,
      33,    33,    33,    33,    33,    33,    33,    33,    33,   879,
      33,    33,    33,   117, -1232, -1232,   -75, -1232,   -75, -1232,
     -75,   -75, -1232,   -75, -1232, -1232, -1232,   231, -1232,   -75,
     -75, -1232,   -75,   -75,   -75,   -75,   -75,   -75,   -75,   -75,
     -75, -1232,   -75, -1232,   -75, -1232, -1232, -1232, -1232,    28,
   -1232, -1232, -1232, -1232, -1232,    97,   168,   186, -1232, -1232,
   -1232, -1232,   284, -1232,   -75, -1232,   -75,   -75,   -75, -1232,
     -75,   231,  1038,   176,    66,    66,   713,    33,   287,   179,
     157,   766,   290,   284,   244,   284,   413,   284,   105,   286,
     284,   297,  1406, -1232, -1232, -1232,   428,   101, -1232, -1232,
     101, -1232, -1232,   101, -1232, -1232,   331,   879, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232,   728, -1232, -1232, -1232, -1232,
     265,   278,   293,   713, -1232,   -75, -1232,   -75,   231, -1232,
   -1232, -1232, -1232,   231,   -75,   231,   -75, -1232,   231,   231,
     -75,   -75, -1232, -1232, -1232, -1232,   -75,   -75,   -75,   -75,
   -1232,   -75, -1232,   -75,   -75,   -75, -1232, -1232, -1232, -1232,
     231,   231, -1232, -1232,   -75,   231, -1232, -1232,   231,   -75,
     -75, -1232,   -75, -1232, -1232,   231, -1232,   -75,   -75,   231,
   -1232, -1232,   -75,   -75,   231, -1232, -1232,   -75,   -75,   231,
   -1232, -1232,   -75,   -75,   231, -1232, -1232,   -75,   -75,   231,
     -75,   231, -1232, -1232,   -75,   231, -1232,   -75, -1232, -1232,
   -1232, -1232,   231, -1232,   -75,   231,   -75,   -75,   -75,   -75,
     -75, -1232,   -75,   231,   284, -1232,   476,   728, -1232, -1232,
     387,   284,   283,   284,   284,    33,    33,    59,   404,   258,
      33,    33,   441,   441,    59,   258,   441,   441,   713,   476,
     284,   452,    33,    33,   349,   284,    33,    33,    88,   477,
     441,    33,   479, -1232,   479,    33,   477,   441,    33,   477,
     441,    33,   477,   441,    33, -1232, -1232,   682,    74, -1232,
      33,   441,    33,  1406,   728,   117, -1232,    33,   331,   117,
   -1232,   487,   117, -1232,   331,   469,   879, -1232,   728, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232,   -75,   -75,   231,
   -1232,   231, -1232, -1232,   -75,   -75,   231,   -75, -1232, -1232,
   -1232,   -75,   -75,   -75, -1232,   -75,   231, -1232, -1232, -1232,
   -1232, -1232, -1232,   231,   284,   -75, -1232,   -75,   -75, -1232,
     -75,   231,   -75,   -75,   284,   -75,   -75, -1232,   -75,   -75,
     713,   284, -1232,   -75,   -75,   -75, -1232,   -75,   -75,   231,
   -1232, -1232,   -75,   -75,   -75,   231,   284,   -75,   -75,   -75,
     -75, -1232,   284,   284,   -75,   -75,   284,   -75,   -75,   284,
     -75,   -75, -1232, -1232,   116, -1232,   284,   -75, -1232,   284,
     -75,   -75,   -75,   293,   284, -1232,   284,   -75, -1232,   -75,
     231,   -75, -1232,   -75,   231,   284, -1232,   488,   511,    33,
      33, -1232, -1232,   479, -1232,  1192,   486,   479,   284,   176,
     258,   614,   284,   728,  1482, -1232,   477,    56,    56,   477,
      33,   477,   258, -1232,   477,   477,   148,   284,   345, -1232,
   -1232, -1232,   477,    56,    56, -1232, -1232,    33,   284,   176,
     477,  1503, -1232,   477,   277, -1232, -1232, -1232, -1232,   477,
      -1, -1232,   477,    -2, -1232,   477,    22, -1232, -1232,   728,
   -1232, -1232,   728, -1232, -1232, -1232,   477,   179,  1669,   284,
     728, -1232, -1232,   487,  1210,   284,    33,   523,  1412,   284,
      33, -1232,   -75, -1232, -1232,   284, -1232,   284, -1232,   -75,
   -1232,   284,   -75, -1232,   -75, -1232,   -75,   284, -1232, -1232,
   -1232,   231, -1232,   293,   284, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232,   -75, -1232, -1232, -1232, -1232,   -75,
     -75,   -75, -1232,   -75,   -75,   -75,   -75,   284, -1232,   -75,
     284,   284,   284,   284, -1232, -1232,   -75,   -75,   231, -1232,
   -1232, -1232,   -75,   231,   284,   -75,   -75,   -75,   -75, -1232,
     -75, -1232,   -75,   284,   -75,   284,   -75,   -75,   284,   -75,
     284,   -75,   284,   -75,   159,   401, -1232,   767,   284, -1232,
   -1232, -1232, -1232,   -75, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232,   -75,   231,   -75,   231, -1232,
     -75,   231,   -75,   231,   -75,   231,   -75,   231,   -75, -1232,
     231,   -75, -1232, -1232,   -75, -1232, -1232, -1232,   231,   -75,
     -75,   231,   -75,   231, -1232, -1232,   -75, -1232,   231, -1232,
   -1232,   -75,   511, -1232, -1232, -1232, -1232, -1232, -1232,   257,
   -1232,    33,   728, -1232,   586,   586,   586,   586,    59,   196,
     284,    59,   284, -1232,   487, -1232, -1232, -1232, -1232, -1232,
   -1232,    33, -1232,    33, -1232,    59,   141,   284,    59,   284,
     476,   631, -1232,   586, -1232,    88,   284, -1232,   284, -1232,
     284, -1232,   284, -1232,   728, -1232, -1232,   728,   728, -1232,
     415, -1232, -1232, -1232, -1232,   452,   145,   548,   528, -1232,
      33,   547, -1232,    33,   322, -1232,  1192,   325, -1232,  1192,
     267, -1232,   682, -1232,   439, -1232,   980,   284,    33, -1232,
   -1232,    33, -1232,  1192,   479,   284,   282,    42, -1232, -1232,
   -1232,   -75, -1232,   -75, -1232, -1232, -1232, -1232,   -75,   -75,
     -75,   -75,   -75,   -75,   -75, -1232,   -75, -1232,   -75, -1232,
     -75,   -75,   -75,   -75, -1232,   -75,   -75, -1232,   -75, -1232,
   -1232,   -75,   -75,   -75,   -75, -1232, -1232, -1232, -1232, -1232,
     433,   415, -1232,   767,   728, -1232,   -75, -1232,   -75, -1232,
     -75, -1232,   -75, -1232, -1232,   284,   -75,   -75,   -75, -1232,
     284,   -75,   -75, -1232,   -75,   -75, -1232,   -75, -1232, -1232,
     -75, -1232,   284, -1232, -1232,   -75,   -75,   -75,   231,   -75,
   -1232,   -75,   -75,   284, -1232, -1232, -1232, -1232, -1232, -1232,
     284,   -75,   -75,   284,   284,   284,   284,   284,   284, -1232,
   -1232,   284,   259,   284,   713,   713,   284, -1232,   345, -1232,
   -1232,   284,   456,   284,   284,   284, -1232, -1232,   728, -1232,
   -1232, -1232,   284, -1232,   212, -1232, -1232,   322, -1232,   325,
   -1232, -1232, -1232,   325, -1232, -1232,  1192, -1232,  1192,   682,
   -1232, -1232, -1232,  1052, -1232,   879, -1232,   476,    33, -1232,
     -75,   167,   487, -1232, -1232,   -75,   -75,   -75,   -75, -1232,
   -1232,   -75,   -75,   -75, -1232, -1232,   -75,   -75, -1232,   -75,
   -1232, -1232, -1232, -1232, -1232, -1232,   231,   -75, -1232,   -75,
   -1232, -1232, -1232,  1008, -1232,   284,   -75,   -75,   -75, -1232,
     -75,   -75,   -75,   -75, -1232,   -75, -1232,   -75, -1232, -1232,
     284,   -75,   284,   -75, -1232,   -75,   523,   231, -1232,   -75,
   -1232,   595,   595,   595,   595, -1232, -1232, -1232,   284,   284,
   -1232,    33, -1232,   595,  1080, -1232, -1232,   315,   622,   553,
     325, -1232, -1232, -1232, -1232,  1192,   376,   284, -1232, -1232,
   -1232,   775,   284,   231,    33,  1141,   284, -1232,   -75,   231,
     -75,   231,   -75,   -75,   231, -1232, -1232,   -75,   -75,   525,
    1080, -1232,   -75,   -75, -1232,   -75, -1232, -1232,   -75, -1232,
     -75, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,   -75,
   -1232,   231, -1232,   282,   -75, -1232,   -75,   -75, -1232,   696,
   -1232,    33,   688, -1232,    33, -1232,   974, -1232,    33,   713,
    1086, -1232, -1232,   622,   553,   553, -1232,  1192,   284,    33,
     284,   476, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232,   231, -1232,   231,   -75, -1232,
     -75, -1232,   -75,   -75,   -75, -1232,   -75,   -75,   -75, -1232,
   -1232,   -75,   -75,   231,   -75, -1232, -1232, -1232, -1232,   284,
   -1232,   -75,   -75,   -75,    33,    33,  1412,  2101, -1232, -1232,
    2559, -1232,  2630,   284,   478, -1232, -1232,    33,   553, -1232,
     713,   284,  1469,   284,   284,   -75,   -75, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232,   -75,   -75,   -75,   -75,   -75,   284,
   -1232,   -75,   -75,   -75,   -75,   -75,   284, -1232,   -75, -1232,
     -75, -1232,   -75, -1232,   -75, -1232, -1232,   -75,   231, -1232,
   -1232,   713,   284,    55,   284,   563,   563,   723,   723, -1232,
     741,   457,   563,   593,   593, -1232,   530, -1232,   284, -1232,
   -1232,   282,   -75, -1232, -1232,   -75,   -75, -1232,   -75,   231,
   -1232, -1232,   -75,   -75, -1232,   -75,   231,   -75, -1232, -1232,
     -75,   -75, -1232,   -75,   231,   -75, -1232,   -75,   -75, -1232,
     -75,   -75, -1232,   -75,   -75, -1232,   -75, -1232,   -75,   -75,
   -1232,   -75, -1232,   -75, -1232,   284,   284, -1232, -1232,    92,
   -1232,   728, -1232, -1232,   741, -1232,  1192,   763, -1232, -1232,
   -1232,   741, -1232,  1192,   763, -1232, -1232, -1232,   763, -1232,
   -1232,   530, -1232, -1232, -1232,   530, -1232, -1232, -1232, -1232,
     -75, -1232,   -75,   284,   -75,   -75,   -75,   -75,   -75,   -75,
     284,   -75,   -75,   -75,   -75, -1232, -1232, -1232, -1232,   763,
   -1232,   577, -1232, -1232,   763, -1232, -1232, -1232, -1232, -1232,
   -1232,   -75,   284,   -75, -1232, -1232, -1232
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   419,   406,   395,   405,   161,   431,   454,   397,   482,
     485,   600,   644,   679,   682,   507,   500,   356,   559,   492,
     489,   497,   495,   611,   666,   396,   421,   432,   398,   420,
     483,   487,   486,   508,   493,   490,   498,     0,     4,     5,
       2,     0,    13,   346,   347,     0,   583,   385,   383,   384,
     386,   387,     0,     0,     3,     0,    12,   416,     0,   585,
       0,    11,     0,   587,   467,   468,     0,    14,     0,   589,
       0,    15,     0,   591,     0,    16,     0,   593,     0,    17,
       0,   584,   540,   538,   539,   541,   542,   586,     0,   588,
     590,   592,   594,    19,    18,     0,     7,     0,     8,     0,
       9,     0,    10,     0,     6,     0,     1,    73,    74,     0,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    78,   162,     0,   357,     0,   394,
       0,     0,   407,     0,   411,   412,   417,     0,   422,     0,
       0,   455,     0,     0,   423,     0,   423,     0,   423,     0,
     502,   560,     0,   601,     0,   612,   626,   613,   627,   614,
     615,   629,   616,   617,   618,   619,   620,   621,   622,   623,
     624,   625,     0,   609,     0,   645,     0,     0,     0,   650,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    75,    76,   608,     0,     0,   633,   635,
       0,   657,   659,     0,   667,   669,     0,     0,    40,    20,
      37,    38,    39,    41,    42,     0,   163,    21,    22,    26,
       0,    25,    35,     0,   164,   154,   361,     0,     0,   446,
     447,   369,   400,     0,     0,     0,     0,   399,     0,     0,
       0,     0,   544,   547,   545,   548,     0,     0,     0,     0,
     408,     0,   413,     0,   423,     0,   433,   434,   435,   436,
       0,     0,   458,   457,   451,     0,   572,   472,     0,     0,
       0,   471,     0,   568,   569,     0,   428,   190,   424,     0,
     484,   575,     0,     0,     0,   491,   578,     0,     0,     0,
     496,   581,     0,     0,     0,   514,   510,   190,   190,     0,
     190,     0,   501,   562,     0,     0,   595,     0,   596,   603,
     604,   610,     0,   647,     0,     0,     0,     0,     0,     0,
       0,   652,     0,     0,     0,    34,    27,     0,    33,    23,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   425,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   506,   505,     0,     0,   503,
       0,     0,     0,     0,     0,     0,   634,     0,     0,     0,
     658,     0,     0,   668,     0,     0,     0,   649,     0,    29,
      31,    28,    36,   168,   171,   165,   166,   155,   158,     0,
     160,     0,   153,   365,     0,   351,     0,     0,   348,   353,
     362,   359,     0,     0,   371,   375,     0,   223,   393,   204,
     205,   206,   207,     0,     0,     0,   448,     0,     0,   521,
       0,     0,     0,     0,     0,     0,     0,   409,   402,   190,
       0,     0,   418,     0,     0,     0,   463,   190,   451,     0,
     450,   459,   190,     0,     0,     0,     0,     0,     0,   190,
     190,   429,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    50,   511,    48,   512,     0,   190,   515,     0,
       0,     0,   597,   605,     0,   648,     0,     0,   524,   661,
       0,     0,   693,    80,     0,     0,    32,     0,     0,     0,
       0,   350,   355,     0,   354,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   388,     0,     0,     0,     0,
       0,     0,     0,   390,     0,     0,     0,     0,     0,   415,
      24,   410,     0,     0,     0,   452,   453,     0,     0,     0,
       0,     0,   469,     0,     0,   191,   426,   427,   488,     0,
       0,   494,     0,     0,   499,     0,     0,    44,    58,     0,
      45,    49,     0,   509,   504,   513,     0,     0,     0,     0,
     606,   602,   646,     0,     0,     0,     0,     0,     0,     0,
       0,   651,   156,   159,   169,     0,   172,     0,   367,   351,
     366,     0,   351,   363,   359,   358,     0,     0,   380,   381,
     376,     0,   368,   372,     0,   224,   225,   226,   227,   228,
     229,   230,   231,   232,     0,   233,   234,   235,   236,     0,
       0,     0,   392,     0,   552,     0,   552,     0,   522,     0,
       0,     0,     0,     0,   199,   198,   190,   190,     0,   401,
     197,   196,   190,     0,     0,     0,   438,     0,   438,   464,
       0,   456,     0,     0,     0,     0,     0,   190,     0,   190,
       0,   190,     0,   190,    48,     0,    59,     0,     0,   563,
     564,   565,   566,     0,   174,   100,   133,   136,   144,   148,
      98,   599,    82,    88,    89,    93,     0,    85,     0,    92,
      85,     0,    85,     0,    85,     0,    85,     0,    85,    84,
       0,   597,   582,   607,   637,   536,   656,   665,     0,   661,
     661,     0,    80,     0,   660,   525,   378,   680,     0,    81,
     681,     0,     0,   167,   170,   352,   364,   349,   360,     0,
     389,     0,   373,   370,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   543,     0,   546,   391,   549,   550,   404,
     403,     0,   414,     0,   430,     0,     0,     0,     0,     0,
      27,     0,   470,     0,   567,     0,     0,   573,     0,   576,
       0,   579,     0,    46,     0,    43,    68,     0,     0,    53,
      71,    55,    66,    67,   558,     0,     0,     0,     0,    91,
       0,     0,   117,     0,     0,   118,     0,     0,   119,     0,
       0,   120,     0,    83,     0,   598,     0,     0,     0,   662,
     663,     0,   664,     0,     0,     0,     0,     0,   683,   685,
     157,     0,   382,   378,   374,   237,   238,   239,   190,   190,
     190,   190,   552,   190,   190,   551,   552,   556,   517,   202,
       0,     0,   438,   190,   449,   190,   190,   437,   438,   444,
     465,   461,     0,   190,   190,   570,   574,   577,   580,    52,
      48,    71,    60,     0,     0,    70,   190,    96,    85,    94,
       0,    90,    85,    87,   101,     0,    85,    85,    85,   134,
       0,    85,    85,   137,     0,    85,   145,     0,   149,   150,
       0,    79,     0,   654,   643,   637,   637,    80,     0,    80,
     636,     0,     0,     0,   379,   523,   673,   674,   671,   672,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   554,
     553,     0,     0,     0,     0,     0,     0,   442,     0,   439,
     441,     0,     0,     0,     0,     0,    47,    69,     0,    54,
      57,    72,     0,    95,     0,    86,    99,     0,   121,     0,
     122,   123,   132,     0,   124,   125,     0,   126,     0,     0,
     173,   638,   639,     0,   640,     0,   642,    27,     0,   655,
       0,     0,     0,   684,   377,     0,     0,     0,     0,   555,
     557,   190,   517,   517,   516,   203,   190,   190,   443,   190,
     445,   188,   186,   185,   187,   466,     0,   190,   460,     0,
     571,    64,    56,     0,   561,     0,   102,   103,   104,   105,
      85,    85,    85,    85,   138,     0,   146,   142,   151,   152,
       0,    80,     0,     0,   537,   378,     0,     0,   688,   689,
     687,     0,     0,     0,     0,   520,   518,   519,     0,     0,
     440,     0,   462,     0,     0,    63,    97,     0,     0,     0,
       0,   127,   128,   129,   130,     0,     0,     0,   147,   641,
     653,     0,     0,     0,     0,     0,     0,   243,   214,     0,
     209,     0,    80,   220,     0,   192,   189,     0,   474,    65,
       0,    61,   106,   107,   108,   109,   110,   111,    85,   139,
       0,   143,   141,   534,   529,   530,   531,   532,   533,   378,
     527,     0,   535,     0,     0,   692,   689,   689,   686,     0,
     213,     0,     0,   208,     0,   218,     0,   219,     0,     0,
       0,   473,    62,     0,     0,     0,   131,     0,     0,     0,
       0,    27,   691,   690,   183,   180,   179,   182,   200,   181,
     201,   217,   345,   175,   177,     0,   176,     0,   215,   244,
       0,   212,   209,    80,     0,   222,   220,     0,   190,   480,
     478,    80,    80,     0,   112,   113,   114,   115,   140,     0,
     526,   194,   675,   190,     0,     0,     0,     0,   211,   210,
       0,   221,     0,     0,     0,   475,   477,     0,     0,   135,
       0,     0,     0,     0,     0,     0,   194,   216,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   297,   246,   248,   250,   252,     0,
     245,   270,   277,   279,   281,   283,     0,   276,   293,   184,
      80,   481,   378,   116,   190,   528,   678,    80,     0,   670,
     694,     0,     0,     0,     0,     0,     0,     0,     0,   240,
       0,     0,     0,     0,     0,   242,     0,   476,     0,   195,
     677,     0,   190,   193,   344,   190,   190,   298,   190,     0,
     241,   340,   190,   190,   247,   190,     0,   190,   249,   342,
     190,   190,   251,   190,     0,   190,   253,   190,   190,   271,
     190,   190,   278,   190,   190,   280,   190,   282,   190,   190,
     284,   190,   294,   190,   479,     0,     0,   299,   302,     0,
     300,     0,   254,   261,     0,   258,     0,     0,   260,   262,
     269,     0,   266,     0,     0,   268,   272,   275,     0,   273,
     285,     0,   287,   288,   289,     0,   291,   292,   295,   296,
     675,   178,   190,     0,   190,   190,     0,   190,   190,   190,
       0,   190,   190,   190,   190,   676,   301,   343,   257,     0,
     255,     0,   259,   265,     0,   263,   341,   267,   274,   286,
     290,   190,     0,   190,   256,   339,   264
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1232, -1232, -1232,  -222,  -226,  -189, -1232,   247,  -195,   289,
   -1232, -1232, -1232, -1232, -1232, -1232,  -196,  -342,  -662,   -56,
    -783,  -637, -1232, -1232, -1232, -1232, -1232, -1232, -1232,  -569,
    -252, -1232, -1232, -1232,  -849, -1232, -1232,  -247,  1222,  1220,
     -57,  1263, -1232,  -708,  -589,  -607, -1232, -1232,  -169, -1232,
   -1232,  -168, -1232, -1232, -1232,  -167,  -321, -1232, -1232,  -791,
   -1232, -1232, -1232, -1232, -1232,  -788, -1232, -1232, -1232, -1232,
    -655, -1232, -1232, -1232,   108, -1232, -1232, -1232, -1232, -1232,
     130, -1232, -1232,  -503, -1232, -1232,  -489, -1232, -1232, -1222,
   -1232, -1232, -1232, -1232,  -552,  1709,  -427, -1231,  -565, -1232,
   -1232, -1232,  -746,  -909,   -36, -1232,  -516, -1232, -1232, -1232,
   -1232,  -518,  -334,    99, -1232, -1232,  -315, -1007,  -385,  -466,
   -1008,  -980, -1232,  -942,  -614, -1232, -1232, -1232, -1232,  -600,
   -1232, -1232, -1232, -1232,  -606,  -611, -1232,  -583, -1232,  -776,
   -1232,  -761, -1232,   711,  -412,  -190,   514,  -417,    29,  -245,
    -310,   107, -1232, -1232, -1232,   193, -1232,  -110, -1232,   -77,
   -1232, -1232, -1232, -1232, -1232, -1232,  -835, -1232, -1232, -1232,
   -1232,   635,   637,   638,   640,  -283,   531, -1232, -1232,   -91,
      41, -1232, -1232, -1232, -1232, -1232,  -107, -1232, -1232, -1232,
   -1232,    10, -1232,   502,  -104, -1232, -1232, -1232,   642, -1232,
   -1232, -1232,  -620, -1232, -1232, -1232,   580,   584,   510,  -174,
       2,   312, -1232, -1232, -1232, -1232, -1232, -1232, -1232,  -367,
    -798,  -966, -1232, -1232,   662,   671, -1232,   232, -1232,  -411,
   -1232, -1232, -1232,  -182, -1232,   675, -1232,  -178, -1232,   677,
   -1232,  -186, -1232,   679, -1232,  -187, -1232, -1232,   414, -1232,
   -1232, -1232, -1232, -1232,   969,  -289, -1232, -1232,  -379, -1232,
   -1232,  -785, -1232, -1232, -1232,  -775, -1232, -1232,   687, -1232,
   -1232,   625, -1232,   628, -1232, -1232,   230,  -618,   234,   241,
     242,   699, -1232, -1232, -1232, -1232, -1232,   717, -1232, -1232,
   -1232, -1232,   718, -1232, -1232,   719, -1232, -1232,   727, -1232,
   -1232,   738,  -188,  -344,   109, -1232, -1232, -1232, -1232, -1232,
   -1232, -1232, -1232, -1232, -1232,   842, -1232,   539,  -179, -1232,
    -119,  -209, -1232, -1232,   -86, -1232,   115, -1232, -1232, -1232,
    -808, -1232, -1232, -1232,   549,    16,   887, -1232, -1232,   551,
   -1083,  -502, -1232,  -988,   892, -1232, -1232, -1232,   -49, -1232,
    -375, -1232,  -200
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   235,   620,   237,   880,   238,   470,
     239,   240,   419,   420,   241,   348,   242,   243,   894,   589,
     503,   590,   504,   695,   890,   591,   809,   969,   592,   810,
     893,  1032,  1033,  1110,   811,   812,   813,   895,   109,   215,
     382,   456,   922,   609,   749,   819,   712,   713,   714,   715,
     716,   717,   718,   905,  1035,   719,   720,   721,   910,   722,
     723,   914,  1045,  1120,  1199,   724,  1087,   725,   917,  1047,
     726,   727,   920,  1050,   489,   351,    41,   136,   245,   427,
     428,   429,   615,   430,   431,   617,   729,   730,  1172,  1173,
    1174,  1175,  1025,  1026,   874,   383,   667,  1176,  1221,   673,
     668,  1177,   870,  1016,   448,   449,  1143,   450,  1140,   451,
     452,  1147,   453,   649,   650,   651,   858,  1100,  1098,  1103,
    1101,  1180,  1269,  1324,  1332,  1270,  1339,  1276,  1342,  1347,
    1277,  1352,  1294,  1317,  1264,  1325,  1326,  1333,  1334,  1327,
    1319,  1178,    42,   252,   353,   534,    44,   354,   253,   138,
     247,   538,   248,   441,   624,   435,   436,   621,   619,   254,
     255,   445,   446,   634,   542,   630,   845,   631,   853,    46,
      47,    48,    49,    50,    51,   454,   140,    52,    53,   256,
     437,   557,    55,   143,   271,   468,   455,   147,   273,   471,
      56,   257,    58,   149,   203,   298,   299,   492,    59,    60,
     275,   276,   787,   277,   278,   279,   258,   259,   457,   876,
     936,   375,    62,   152,   284,   285,   482,   478,   963,   738,
     680,   881,  1027,    63,    64,    65,   290,   486,  1151,  1192,
    1193,  1282,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,   210,    80,   318,   319,
     506,   320,   321,   509,   937,   953,   461,   659,   941,   520,
     746,   739,  1129,  1130,  1131,   740,   741,  1055,    81,    82,
      83,   260,    84,   261,    85,    86,   262,   770,   263,   264,
     265,    87,    88,   162,   324,   325,   703,    89,   292,   293,
     294,   295,    90,   303,   304,    91,   308,   309,    92,   313,
     314,    93,    94,   327,   599,    95,   164,   331,   332,   514,
      96,   182,    97,   183,   184,   938,   218,   219,   837,    99,
     186,   334,   335,   516,   336,   191,   342,   343,   927,   928,
     742,   743,   100,   221,   222,   605,   939,   102,   224,   225,
     940,  1223,   103,   748,   328,   105,   523,   848,   849,  1059,
    1096,   524,  1060
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     115,   270,    61,   236,   421,   344,   672,   479,   146,   711,
      57,   337,   187,   188,   339,   737,   347,   532,   943,   291,
     307,   312,   531,   697,   326,   613,   302,   421,   929,    45,
     906,  1053,   519,   907,   842,   808,   911,   871,   772,   345,
     931,    54,   434,   932,   970,   485,  1017,   190,   349,   918,
    1160,   925,   205,  1099,   207,  1099,  1104,   803,   789,   512,
       1,   926,  1316,     1,    19,    21,  1108,   518,  1093,  1168,
       1,    15,     5,  1323,  1323,  1331,  1331,  1314,  1338,   192,
    1323,   193,   464,   194,   195,   249,   196,   144,   114,    34,
      36,     5,   198,   199,   250,   200,   201,   202,   204,   202,
     206,   202,   208,   209,   267,   211,  1168,   212,    40,     5,
      33,   442,   251,   822,   106,   825,    17,   828,   266,   831,
     333,   833,   251,    10,   289,   316,    17,   216,  1392,   217,
     220,   223,   483,   226,   338,  1182,    26,   340,  1104,    26,
      29,   189,  1394,    29,  1170,  1397,    26,   144,   670,  1398,
      29,   422,  1401,   671,    15,   664,  1402,   189,     5,   665,
     282,   425,     5,   145,  1184,     5,   908,  1012,  1187,   912,
     370,   704,   915,   467,    31,    32,   476,  1013,   311,   333,
     134,  1170,   283,  1037,  1075,  1051,  1038,  1421,   350,  1040,
     352,  -628,  1423,    33,   525,  1203,   484,   357,   249,   359,
     710,   502,   144,   362,   363,   735,  1048,   250,   513,   364,
     365,   366,   367,     5,   368,   326,   369,   202,   371,   994,
    1092,   996,   526,   145,   734,  1111,  1058,   374,  1355,   626,
     288,   968,   378,   379,  1288,   380,   485,   246,   891,   274,
    1188,   384,   706,   707,   948,   386,   387,   924,   951,   850,
     389,   390,   956,   966,   731,   392,   393,   627,   961,   682,
    -630,  1152,     2,   398,   144,   708,   709,   401,   476,   661,
     403,   587,     4,   588,   447,   560,     5,   405,   145,   408,
     409,   411,   412,   414,     5,   415,  1112,   600,     2,  1113,
    1115,   973,     2,   704,  1158,   975,  1134,   735,     4,   978,
     980,   981,     4,     5,   984,   985,     7,     5,   987,    19,
     297,  1284,    10,   296,   -51,   301,   588,   317,   246,  1039,
    1136,    12,  1041,   338,  1042,  1107,   340,   899,  1043,   903,
     459,  -631,   903,   251,    34,   903,   315,   316,   903,     5,
     145,   903,     5,  1089,   246,   433,   707,   633,   704,  -632,
     493,   704,   670,   496,  1328,   807,   499,   671,   476,    24,
    1345,   323,     5,    31,    32,  1194,     5,   443,   708,   709,
     527,   528,   341,   473,   463,   708,   709,   352,   533,   709,
     536,  1183,  1312,   664,   537,   539,   540,   665,   541,  1191,
     107,   108,   687,   694,  1145,   868,   696,   691,   546,   693,
     547,   548,   689,   549,   733,   551,   552,   762,   554,   555,
     326,   556,   558,  1201,  1287,   346,   562,   563,   564,   423,
     424,   374,  1114,  1116,  1117,  1118,   569,   570,   -30,   755,
     573,   574,   757,  1081,  1082,  1083,  1084,   579,   580,   531,
     582,   583,   229,   585,   586,   213,   214,  1308,   421,   851,
     859,   860,   861,   596,   597,   598,   654,   656,  1225,  1226,
     603,   444,   604,  1021,   607,   532,   608,  1022,  1023,  1024,
     653,   655,   676,   678,     5,  1209,     7,  1321,   883,  1314,
      21,   882,   477,  1215,  1216,   306,   675,   677,   903,   459,
     903,   751,   490,   447,   903,     5,     5,   686,  1195,  1196,
    1197,   696,   329,   330,   704,    36,  1135,   705,   706,   707,
     652,  1156,   518,   657,  1189,   660,   522,  1318,   662,   663,
     423,  1335,  1335,   517,  1340,  1344,   674,  1349,  1349,   521,
    1353,   708,   709,   710,   134,   228,   229,   685,   230,   231,
     232,   233,   234,   688,   424,     5,   690,     5,   433,   692,
    1171,   807,  1314,  1181,   704,   752,   587,  1185,   706,   707,
     698,  1190,   533,  1283,     5,   533,   854,   537,   747,   759,
    1164,   805,  1307,   704,  1165,  1166,  1167,   705,   707,  1310,
       5,   708,   709,  1321,   670,  1314,   921,   764,  1395,   671,
     588,   903,   765,   766,   767,  1399,   768,   769,   771,   769,
     708,   709,   774,  1097,  1271,  1403,  1278,   709,   889,  1404,
       5,   696,   892,   884,  1329,  1314,   469,  1227,   785,   786,
     788,   786,   154,   790,   156,   791,   158,   793,   160,   795,
     233,   234,  1169,  1286,  1057,   418,   502,   913,   942,   804,
     916,   618,   971,   141,   967,   623,   815,   897,   150,   900,
     153,   901,   155,  1036,   157,   421,   159,   612,   816,  1121,
     818,  1292,   864,   821,   666,   824,  1208,   827,  1211,   830,
     684,   830,   628,   629,   598,   708,   709,   836,  1102,   875,
    1186,   -59,   604,   604,  1336,   608,   -59,   -59,   -59,   844,
    1343,   807,   246,   433,   847,  1164,  1097,   696,   892,  1165,
    1166,  1167,  1168,  1164,  1350,     5,   728,  1165,  1166,  1167,
    1168,    43,   728,     5,   704,   287,   728,   705,   706,   707,
    1348,  1348,   704,  1066,  1067,   705,   706,   707,  1057,   622,
    1164,   758,   807,   933,  1165,  1166,  1167,   855,   856,   857,
       5,   708,   709,   710,  1329,  1314,   991,   992,  1164,   708,
     709,   710,  1165,  1166,  1167,   839,   840,  1169,     5,  1015,
    1015,  1162,  1163,  1314,   165,  1169,   166,   167,   807,   168,
    1164,   169,  1031,  1052,  1165,  1166,  1167,  1170,   862,   280,
       5,   866,  1123,   281,  1019,  1170,  1124,  1125,  1126,  1127,
     566,   170,  1169,   502,   759,   872,   844,  1044,   878,  1046,
     171,    10,   683,  1280,   172,   769,   173,  1074,   174,   769,
    1169,   952,   507,   954,   955,   786,   175,   896,   958,   421,
     268,   786,   251,   269,   962,   791,    17,   699,   176,   134,
     228,   700,  1169,   230,   231,   232,   286,   696,   701,   702,
     835,   830,    98,   974,  1128,   830,   177,   178,   179,   977,
     979,   830,    31,    32,   983,   830,   180,   986,   830,  1422,
     988,   228,   229,   989,   230,   231,   232,   181,   836,   836,
     993,   458,   608,   406,   997,   998,   228,   863,  1109,   230,
     231,   232,   474,   475,  1001,  1002,   410,   101,  1405,     3,
     413,   488,   104,  1003,   873,  1137,  1119,     6,   495,     0,
       0,   498,     0,     0,   501,     0,     8,     0,     0,     0,
       0,     0,   511,     9,   696,   228,    11,     0,   230,   231,
     232,   233,   234,   805,   898,   806,   902,     0,     0,   902,
       0,    16,   902,     0,     0,   902,     0,     0,   902,     0,
       0,     0,    18,  1056,   728,    20,     0,    22,  1061,  1062,
    1063,  1064,   934,     0,  1015,   952,   952,     0,    25,     0,
      27,     0,    28,     0,    30,     0,     0,     0,  1198,     0,
      35,     0,  1073,     0,     0,     0,     0,     0,     0,  1077,
    1078,  1079,  1097,  1080,   830,   830,   830,     0,  1085,   476,
    1086,     5,     0,     0,   608,     0,  1091,     5,   844,     0,
     704,     0,  1095,   705,   706,   707,   704,     0,     0,   705,
     706,   707,     0,     0,     0,     0,     0,     0,   735,     0,
       0,     0,   923,   736,     0,  1015,     0,   708,   709,   710,
       0,     0,     0,   708,   709,   710,     0,     0,     0,     0,
       0,  1139,     0,  1142,     0,   608,  1146,     0,     0,     0,
    1149,  1150,     0,     0,     0,  1153,  1154,     0,  1155,     0,
    1011,   830,     0,  1157,     0,     0,     0,     0,     0,     5,
       0,     0,   844,     0,     0,     0,     0,  1161,   704,  1095,
    1095,   705,   706,   707,     0,   902,     0,   902,     0,     0,
       0,   902,     0,  1164,     0,   736,  1015,  1165,  1166,  1167,
    1168,   728,     0,     5,     0,   708,   709,   710,     0,     0,
       0,     0,   704,     0,     0,   705,   706,   707,     0,     0,
       0,  1206,  1189,  1207,     0,  1142,   608,  1210,     0,  1146,
    1212,     0,     0,     0,  1214,   608,     0,  1218,     0,   708,
     709,   710,     0,     0,  1220,  1222,     0,     0,     0,     0,
     476,     0,     0,     0,     0,  1169,   228,   229,     5,   230,
     231,   232,   233,   234,     0,  1393,   806,   704,  1291,  1220,
     705,   706,   707,     0,     0,  1170,     0,  1396,     0,   735,
       0,     0,     0,     0,  1400,   134,   228,   229,   902,   230,
     231,   232,   233,   234,   708,   709,   710,     0,     0,     0,
       0,     0,     0,   728,     0,     0,     0,  1293,  1295,  1296,
    1297,  1298,     0,     0,  1300,  1301,  1302,  1303,  1304,   476,
       0,  1306,     0,   608,     0,   844,     0,     5,   228,     0,
     608,   230,   231,   232,   233,   234,   704,     0,   806,   705,
     706,   707,     0,     0,     0,     0,     0,   728,   735,     0,
     728,     0,     0,   736,   728,     0,     0,     0,   728,     0,
       0,  1359,     0,   708,   709,   710,     0,   110,  1364,     0,
    1367,     0,     0,     0,   111,   112,  1371,   113,  1374,     0,
     116,     0,   117,  1378,   118,     0,  1381,     0,   119,     0,
     120,  1385,   121,     0,   122,     0,   123,     0,   124,     0,
     125,     0,   126,     0,     0,     0,     0,     0,     0,     0,
     127,     0,     0,     0,   728,     0,     0,   128,     0,   129,
       0,   130,   728,   131,     0,   132,     0,   133,     0,     0,
     728,   460,   462,  1222,     0,   465,   466,     0,  1409,  1411,
     228,   229,  1414,   230,   231,   232,   233,   234,     0,   487,
       0,     0,     0,     0,     0,     0,   494,     0,     0,   497,
       0,     0,   500,     0,     0,     0,     0,     0,     0,   197,
     510,     0,   135,   137,   139,   139,   142,     0,     0,   148,
     139,   151,   139,   148,   139,   148,   139,   148,   139,   148,
     161,   163,     0,   185,   185,   185,     0,     0,     0,     0,
       0,  1315,     0,  1322,  1322,  1330,  1330,     0,  1337,  1341,
    1322,  1346,  1346,   227,  1351,     0,     3,     0,     0,     0,
       0,     0,     0,   300,     6,   305,     0,   310,     0,     5,
     322,     0,     0,     8,     0,     0,     0,     0,   704,     0,
       9,   705,   706,   707,     0,     0,     0,     0,     0,     0,
       0,     0,    14,     0,     0,   244,     0,     0,    16,     0,
     272,     0,     0,     0,     0,   708,   709,   710,     0,    18,
     355,     0,    20,     0,    22,   356,     0,   358,     0,     0,
     360,   361,     0,     0,     0,    25,     5,    27,     0,    28,
       0,    30,     0,     0,     0,   704,     0,    35,   705,   706,
     707,     0,   372,   373,     0,     0,     0,   376,     0,     0,
     377,     0,   736,     0,   747,     0,     0,   381,     0,     0,
       0,   385,   708,   709,   710,     0,   388,     0,     0,     0,
       0,   391,     0,     0,     0,     0,   394,     0,     0,     0,
       0,   397,     0,   400,     0,     0,     0,   402,     0,     0,
       0,     0,     0,     0,   404,     0,     0,   407,     0,     0,
       0,     0,     0,     0,   417,   416,     0,     0,     0,     0,
       0,   432,     0,   438,   439,   635,   636,   637,   638,   639,
     640,   641,   642,   643,   644,   645,   646,   647,   648,     0,
     472,     0,     0,     0,     0,   480,   635,   636,   637,   638,
     639,   640,   641,   642,   643,     0,     0,     0,     0,     0,
       0,     0,     0,   426,     0,     0,     0,     0,   440,   137,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   481,
     137,     0,     0,     0,     0,     0,     0,     0,   491,     0,
       0,   529,     0,   530,     0,     0,     0,     0,   535,     0,
     505,     0,     0,   508,     0,     0,     0,     0,   543,     0,
     515,     0,     0,     0,   545,   544,     0,     0,     0,     3,
       0,     0,     0,   550,   553,     0,     5,     6,     0,     0,
       0,   561,     0,     0,     0,   704,     8,     0,   705,   706,
     707,   567,     0,     9,     0,     0,   572,   571,     0,     0,
       0,     0,   577,   578,     0,    14,   581,     0,     0,   584,
       0,    16,   708,   709,   710,     0,   593,     0,     0,   595,
       0,     0,    18,     0,   601,    20,   602,    22,     0,     0,
       0,     0,   606,     0,     0,   611,   610,     0,    25,     0,
      27,     0,    28,     0,    30,     0,     0,     0,   625,     0,
      35,     0,   632,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   669,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   681,     0,
       0,     0,   614,   616,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   658,     0,     0,     0,     0,     0,   732,
       0,     0,     0,     0,     0,   744,     0,     0,     0,   750,
     679,     0,     0,     0,     0,   753,     0,   754,     0,     0,
       0,   756,     0,     0,     0,     0,     0,   760,     0,     0,
       0,     0,     0,   761,   763,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   745,
       0,     0,     0,   185,     0,     0,     0,   773,     0,     0,
     775,   776,   777,   778,     0,     0,     0,     0,     0,     0,
     781,     0,     0,     0,   784,   783,     0,     0,     0,     0,
       0,     0,     0,   792,     0,   794,     0,     0,   797,     0,
     799,     0,   801,     0,     0,     0,     0,     0,   814,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   817,     0,
     820,     0,     0,   823,     0,   826,     0,   829,     0,   832,
       0,     0,   834,     0,     0,     0,     0,     0,     0,     0,
     838,     0,     0,   841,     0,   843,     0,     0,     0,     0,
     846,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     865,     0,   867,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   877,     0,   879,
       0,     0,     0,     0,     0,     0,   885,     0,   886,     0,
     887,     0,   888,     0,   852,     0,   395,   396,     0,   399,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   869,     0,   869,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   930,     0,     0,
       0,     0,     0,     0,     0,   935,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   904,     0,     0,   909,     0,     0,     0,
       0,     0,     0,     0,     0,   919,     0,     0,     0,     0,
       0,   679,     0,     0,   679,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   976,     0,     0,     0,     0,
     982,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   990,     0,     0,     0,     0,     0,     0,     0,
     995,     0,     0,   999,     0,     0,     0,     0,     0,     0,
    1000,     0,     0,  1004,  1005,  1006,  1007,  1008,  1009,     0,
       0,  1010,     0,  1014,     0,     0,  1018,     0,   559,     0,
       0,  1020,     0,  1028,  1029,  1030,   565,     0,     0,     0,
       0,   568,  1034,     0,     0,     0,     0,     0,   575,   576,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1228,
    1229,  1230,  1231,  1232,  1233,  1234,   594,  1235,  1236,  1237,
    1238,  1239,  1240,  1241,  1242,  1243,  1244,  1245,  1246,  1247,
    1248,  1249,  1250,  1251,  1252,  1253,  1254,  1255,  1256,  1257,
    1258,     0,     0,  1259,  1260,  1261,  1262,  1263,  1071,     0,
       0,     0,  1049,     0,     0,  1076,     0,     0,     0,     0,
       0,  1054,     0,     0,     0,     0,     0,     0,     0,     0,
    1088,     0,  1090,     0,     0,     0,     0,     0,     0,  1094,
       0,     0,     0,     0,     0,     0,     0,     0,  1105,  1106,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1122,     0,     0,
       0,     0,  1132,     0,     0,  1133,  1138,     0,     0,     0,
       0,  1141,     0,  1144,     0,     0,  1148,     0,     0,     0,
       0,     0,     0,     0,   869,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1159,     0,     0,     0,   679,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   779,   780,     0,  1200,     0,
    1202,   782,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   796,  1204,   798,  1205,
     800,     0,   802,     0,  1179,     0,     0,  1179,     0,     0,
       0,  1179,     0,     0,     0,  1217,     0,     0,     0,  1219,
       0,     0,   869,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1279,     0,     0,     0,     0,     0,     0,
       0,  1285,     0,  1289,  1290,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   869,   869,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1281,     0,     0,     0,     0,     0,     0,     0,     0,  1299,
       0,     0,     0,     0,     0,     0,  1305,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1311,     0,  1313,     0,  1320,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1354,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1361,     0,     0,     0,     0,     0,     0,  1366,     0,
       0,     0,     0,     0,     0,     0,  1373,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   944,   945,   946,
     947,     0,   949,   950,     0,  1390,  1391,     1,     2,     0,
       0,     0,   957,     3,   959,   960,     0,     0,     4,     0,
       5,     6,   964,   965,     0,     0,     0,     7,     0,     0,
       8,     0,     0,     0,     0,   972,     0,     9,    10,     0,
      11,     0,    12,  1407,     0,     0,     0,    13,     0,    14,
    1416,     0,    15,     0,     0,    16,     0,     0,     0,     0,
       0,     0,     0,    17,     0,     0,    18,     0,    19,    20,
      21,    22,  1425,     0,     0,     0,     0,     0,     0,    23,
      24,     0,    25,    26,    27,     0,    28,    29,    30,    31,
      32,    33,     0,    34,    35,    36,  1265,  1228,  1229,  1230,
    1231,  1232,  1233,  1234,  1266,  1235,  1236,  1237,  1238,  1239,
    1240,  1241,  1242,  1243,  1244,  1245,  1246,  1247,  1248,  1249,
    1250,  1251,  1252,  1253,  1254,  1255,  1256,  1257,  1258,  1267,
    1268,  1259,  1260,  1261,  1262,  1263,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1065,     0,     0,     0,     0,  1068,  1069,     0,  1070,     0,
       0,     0,     0,     0,     0,     0,  1072,  1272,  1228,  1229,
    1230,  1231,  1232,  1233,  1234,  1273,  1235,  1236,  1237,  1238,
    1239,  1240,  1241,  1242,  1243,  1244,  1245,  1246,  1247,  1248,
    1249,  1250,  1251,  1252,  1253,  1254,  1255,  1256,  1257,  1258,
    1274,  1275,  1259,  1260,  1261,  1262,  1263,     0,     0,     0,
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1213,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1224,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1309,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1356,     0,     0,  1357,  1358,     0,  1360,     0,     0,
       0,  1362,  1363,     0,  1365,     0,  1368,     0,     0,  1369,
    1370,     0,  1372,     0,  1375,     0,  1376,  1377,     0,  1379,
    1380,     0,  1382,  1383,     0,  1384,     0,  1386,  1387,     0,
    1388,     0,  1389,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1406,     0,  1408,  1410,     0,  1412,  1413,  1415,     0,
    1417,  1418,  1419,  1420,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1424,     0,  1426
};

static const yytype_int16 yycheck[] =
{
      57,   196,     0,   192,   346,   227,   558,   374,   115,   598,
       0,   220,   131,   132,   223,   604,   242,   434,   853,   201,
     206,   208,   434,   592,   212,   528,   204,   369,   836,     0,
     821,   997,   411,   821,   742,   697,   824,   783,   656,   235,
     838,     0,   352,   841,   893,   379,   955,   133,   243,   832,
    1133,   836,   156,  1061,   158,  1063,  1064,   694,   678,   403,
       4,   836,  1293,     4,    65,    67,  1073,    25,  1056,    14,
       4,    49,    17,  1295,  1296,  1297,  1298,    22,  1300,   136,
    1302,   138,   365,   140,   141,    19,   143,     6,   163,    90,
      92,    17,   149,   150,    28,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   195,   162,    14,   164,     0,    17,
      88,   356,    56,   720,     0,   722,    60,   724,   195,   726,
      78,   728,    56,    35,   201,    51,    60,   184,  1359,   186,
     187,   188,   377,   190,   220,  1142,    80,   223,  1146,    80,
      84,    40,  1364,    84,    89,  1367,    80,     6,     7,  1371,
      84,   347,  1374,    12,    49,     7,  1378,    40,    17,    11,
       3,   350,    17,    82,  1144,    17,   821,   952,  1148,   824,
     274,    26,   827,   368,    86,    87,     9,   952,    73,    78,
     147,    89,    25,   974,  1033,   993,   974,  1409,   245,   977,
     247,   163,  1414,    88,   416,  1161,   378,   254,    19,   256,
      55,   397,     6,   260,   261,    38,   989,    28,   404,   266,
     267,   268,   269,    17,   271,   403,   273,   274,   275,   927,
    1055,   929,   418,    82,   603,  1074,  1001,   284,  1311,   539,
     201,   893,   289,   290,  1222,   292,   570,    61,   807,   198,
    1149,   298,    30,    31,   862,   302,   303,   836,   866,   752,
     307,   308,   872,   890,   598,   312,   313,   540,   878,   569,
     163,  1110,     5,   320,     6,    53,    54,   324,     9,   552,
     327,   155,    15,   157,    16,   470,    17,   334,    82,   336,
     337,   338,   339,   340,    17,   342,  1077,   513,     5,  1077,
    1078,   898,     5,    26,  1129,   902,  1094,    38,    15,   906,
     907,   908,    15,    17,   911,   912,    24,    17,   915,    65,
     202,  1220,    35,    23,   155,    71,   157,   209,    61,   974,
    1095,    39,   977,   409,   979,  1071,   412,   816,   983,   818,
      48,   163,   821,    56,    90,   824,    50,    51,   827,    17,
      82,   830,    17,  1051,    61,    62,    31,   543,    26,   163,
     386,    26,     7,   389,  1296,   697,   392,    12,     9,    77,
    1302,    64,    17,    86,    87,  1153,    17,   357,    53,    54,
     427,   428,    41,   371,   364,    53,    54,   434,   435,    54,
     437,  1142,  1291,     7,   441,   442,   443,    11,   445,  1150,
     159,   160,   574,   589,  1102,   774,   592,   583,   455,   586,
     457,   458,   580,   460,   600,   462,   463,   633,   465,   466,
     598,   468,   469,  1159,  1222,   150,   473,   474,   475,    32,
      33,   478,  1077,  1078,  1079,  1080,   483,   484,   150,   619,
     487,   488,   622,  1040,  1041,  1042,  1043,   494,   495,   851,
     497,   498,   149,   500,   501,   161,   162,  1282,   790,   759,
     765,   766,   767,   510,   511,   512,   547,   548,  1204,  1205,
     517,    57,   519,     7,   521,   882,   523,    11,    12,    13,
     547,   548,   563,   564,    17,  1183,    24,    20,   793,    22,
      67,   791,   374,  1191,  1192,    72,   563,   564,   977,    48,
     979,   610,   384,    16,   983,    17,    17,   574,  1153,  1154,
    1155,   697,    74,    75,    26,    92,  1095,    29,    30,    31,
     546,  1118,    25,   549,    36,   551,    47,  1293,   554,   555,
      32,  1297,  1298,   408,  1300,  1301,   562,  1303,  1304,   414,
    1306,    53,    54,    55,   147,   148,   149,   573,   151,   152,
     153,   154,   155,   579,    33,    17,   582,    17,    62,   585,
    1139,   893,    22,  1142,    26,   612,   155,  1146,    30,    31,
     596,  1150,   619,  1218,    17,   622,   762,   624,    45,   626,
       7,   156,  1280,    26,    11,    12,    13,    29,    31,  1287,
      17,    53,    54,    20,     7,    22,   147,   644,  1364,    12,
     157,  1080,   649,   650,   651,  1371,   653,   654,   655,   656,
      53,    54,   659,     8,  1210,  1381,  1212,    54,   804,  1385,
      17,   807,   808,   795,    21,    22,   369,  1206,   675,   676,
     677,   678,   120,   680,   122,   682,   124,   684,   126,   686,
     154,   155,    69,  1222,  1001,   346,   832,   826,   847,   695,
     829,   533,   894,   112,   891,   537,   703,   816,   117,   817,
     119,   818,   121,   974,   123,   997,   125,   527,   715,  1086,
     717,  1226,   769,   720,   556,   722,  1182,   724,  1186,   726,
     571,   728,    58,    59,   731,    53,    54,   734,  1063,   786,
    1146,   156,   739,   740,  1298,   742,   161,   162,   163,   746,
    1301,  1033,    61,    62,   751,     7,     8,   893,   894,    11,
      12,    13,    14,     7,  1304,    17,   598,    11,    12,    13,
      14,     0,   604,    17,    26,   201,   608,    29,    30,    31,
    1303,  1304,    26,  1012,  1013,    29,    30,    31,  1095,   536,
       7,   624,  1074,   843,    11,    12,    13,   151,   152,   153,
      17,    53,    54,    55,    21,    22,   925,   926,     7,    53,
      54,    55,    11,    12,    13,   739,   740,    69,    17,   954,
     955,  1136,  1137,    22,   129,    69,   129,   129,  1110,   129,
       7,   129,   968,   995,    11,    12,    13,    89,   768,   199,
      17,   771,     7,   199,   958,    89,    11,    12,    13,    14,
     478,   129,    69,   989,   851,   785,   853,   986,   788,   988,
     129,    35,   570,  1214,   129,   862,   129,  1033,   129,   866,
      69,   868,   398,   870,   871,   872,   129,   815,   875,  1161,
     195,   878,    56,   195,   881,   882,    60,   597,   129,   147,
     148,   597,    69,   151,   152,   153,    70,  1033,   597,   597,
     731,   898,     0,   900,    69,   902,   129,   129,   129,   906,
     907,   908,    86,    87,   911,   912,   129,   914,   915,  1411,
     917,   148,   149,   920,   151,   152,   153,   129,   925,   926,
     927,   361,   929,   334,   931,   932,   148,   769,  1074,   151,
     152,   153,   372,   373,   941,   942,   337,     0,  1390,    10,
     339,   381,     0,   942,   786,  1095,  1085,    18,   388,    -1,
      -1,   391,    -1,    -1,   394,    -1,    27,    -1,    -1,    -1,
      -1,    -1,   402,    34,  1110,   148,    37,    -1,   151,   152,
     153,   154,   155,   156,   816,   158,   818,    -1,    -1,   821,
      -1,    52,   824,    -1,    -1,   827,    -1,    -1,   830,    -1,
      -1,    -1,    63,  1000,   836,    66,    -1,    68,  1005,  1006,
    1007,  1008,   844,    -1,  1149,  1012,  1013,    -1,    79,    -1,
      81,    -1,    83,    -1,    85,    -1,    -1,    -1,  1157,    -1,
      91,    -1,  1029,    -1,    -1,    -1,    -1,    -1,    -1,  1036,
    1037,  1038,     8,  1040,  1041,  1042,  1043,    -1,  1045,     9,
    1047,    17,    -1,    -1,  1051,    -1,  1053,    17,  1055,    -1,
      26,    -1,  1059,    29,    30,    31,    26,    -1,    -1,    29,
      30,    31,    -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,
      -1,    -1,    42,    43,    -1,  1220,    -1,    53,    54,    55,
      -1,    -1,    -1,    53,    54,    55,    -1,    -1,    -1,    -1,
      -1,  1098,    -1,  1100,    -1,  1102,  1103,    -1,    -1,    -1,
    1107,  1108,    -1,    -1,    -1,  1112,  1113,    -1,  1115,    -1,
     952,  1118,    -1,  1120,    -1,    -1,    -1,    -1,    -1,    17,
      -1,    -1,  1129,    -1,    -1,    -1,    -1,  1134,    26,  1136,
    1137,    29,    30,    31,    -1,   977,    -1,   979,    -1,    -1,
      -1,   983,    -1,     7,    -1,    43,  1291,    11,    12,    13,
      14,   993,    -1,    17,    -1,    53,    54,    55,    -1,    -1,
      -1,    -1,    26,    -1,    -1,    29,    30,    31,    -1,    -1,
      -1,  1178,    36,  1180,    -1,  1182,  1183,  1184,    -1,  1186,
    1187,    -1,    -1,    -1,  1191,  1192,    -1,  1194,    -1,    53,
      54,    55,    -1,    -1,  1201,  1202,    -1,    -1,    -1,    -1,
       9,    -1,    -1,    -1,    -1,    69,   148,   149,    17,   151,
     152,   153,   154,   155,    -1,  1361,   158,    26,  1225,  1226,
      29,    30,    31,    -1,    -1,    89,    -1,  1366,    -1,    38,
      -1,    -1,    -1,    -1,  1373,   147,   148,   149,  1080,   151,
     152,   153,   154,   155,    53,    54,    55,    -1,    -1,    -1,
      -1,    -1,    -1,  1095,    -1,    -1,    -1,  1264,  1265,  1266,
    1267,  1268,    -1,    -1,  1271,  1272,  1273,  1274,  1275,     9,
      -1,  1278,    -1,  1280,    -1,  1282,    -1,    17,   148,    -1,
    1287,   151,   152,   153,   154,   155,    26,    -1,   158,    29,
      30,    31,    -1,    -1,    -1,    -1,    -1,  1139,    38,    -1,
    1142,    -1,    -1,    43,  1146,    -1,    -1,    -1,  1150,    -1,
      -1,  1318,    -1,    53,    54,    55,    -1,    45,  1325,    -1,
    1327,    -1,    -1,    -1,    52,    53,  1333,    55,  1335,    -1,
      58,    -1,    60,  1340,    62,    -1,  1343,    -1,    66,    -1,
      68,  1348,    70,    -1,    72,    -1,    74,    -1,    76,    -1,
      78,    -1,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      88,    -1,    -1,    -1,  1206,    -1,    -1,    95,    -1,    97,
      -1,    99,  1214,   101,    -1,   103,    -1,   105,    -1,    -1,
    1222,   362,   363,  1390,    -1,   366,   367,    -1,  1395,  1396,
     148,   149,  1399,   151,   152,   153,   154,   155,    -1,   380,
      -1,    -1,    -1,    -1,    -1,    -1,   387,    -1,    -1,   390,
      -1,    -1,   393,    -1,    -1,    -1,    -1,    -1,    -1,   147,
     401,    -1,   109,   110,   111,   112,   113,    -1,    -1,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,    -1,   130,   131,   132,    -1,    -1,    -1,    -1,
      -1,  1293,    -1,  1295,  1296,  1297,  1298,    -1,  1300,  1301,
    1302,  1303,  1304,   191,  1306,    -1,    10,    -1,    -1,    -1,
      -1,    -1,    -1,   203,    18,   205,    -1,   207,    -1,    17,
     210,    -1,    -1,    27,    -1,    -1,    -1,    -1,    26,    -1,
      34,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,   192,    -1,    -1,    52,    -1,
     197,    -1,    -1,    -1,    -1,    53,    54,    55,    -1,    63,
     248,    -1,    66,    -1,    68,   253,    -1,   255,    -1,    -1,
     258,   259,    -1,    -1,    -1,    79,    17,    81,    -1,    83,
      -1,    85,    -1,    -1,    -1,    26,    -1,    91,    29,    30,
      31,    -1,   280,   281,    -1,    -1,    -1,   285,    -1,    -1,
     288,    -1,    43,    -1,    45,    -1,    -1,   295,    -1,    -1,
      -1,   299,    53,    54,    55,    -1,   304,    -1,    -1,    -1,
      -1,   309,    -1,    -1,    -1,    -1,   314,    -1,    -1,    -1,
      -1,   319,    -1,   321,    -1,    -1,    -1,   325,    -1,    -1,
      -1,    -1,    -1,    -1,   332,    -1,    -1,   335,    -1,    -1,
      -1,    -1,    -1,    -1,   344,   343,    -1,    -1,    -1,    -1,
      -1,   351,    -1,   353,   354,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,    -1,
     370,    -1,    -1,    -1,    -1,   375,    93,    94,    95,    96,
      97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   350,    -1,    -1,    -1,    -1,   355,   356,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   376,
     377,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   385,    -1,
      -1,   429,    -1,   431,    -1,    -1,    -1,    -1,   436,    -1,
     397,    -1,    -1,   400,    -1,    -1,    -1,    -1,   446,    -1,
     407,    -1,    -1,    -1,   454,   453,    -1,    -1,    -1,    10,
      -1,    -1,    -1,   461,   464,    -1,    17,    18,    -1,    -1,
      -1,   471,    -1,    -1,    -1,    26,    27,    -1,    29,    30,
      31,   479,    -1,    34,    -1,    -1,   486,   485,    -1,    -1,
      -1,    -1,   492,   493,    -1,    46,   496,    -1,    -1,   499,
      -1,    52,    53,    54,    55,    -1,   506,    -1,    -1,   509,
      -1,    -1,    63,    -1,   514,    66,   516,    68,    -1,    -1,
      -1,    -1,   520,    -1,    -1,   525,   524,    -1,    79,    -1,
      81,    -1,    83,    -1,    85,    -1,    -1,    -1,   538,    -1,
      91,    -1,   542,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   557,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   568,    -1,
      -1,    -1,   529,   530,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   550,    -1,    -1,    -1,    -1,    -1,   599,
      -1,    -1,    -1,    -1,    -1,   605,    -1,    -1,    -1,   609,
     567,    -1,    -1,    -1,    -1,   615,    -1,   617,    -1,    -1,
      -1,   621,    -1,    -1,    -1,    -1,    -1,   627,    -1,    -1,
      -1,    -1,    -1,   631,   634,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   606,
      -1,    -1,    -1,   610,    -1,    -1,    -1,   657,    -1,    -1,
     660,   661,   662,   663,    -1,    -1,    -1,    -1,    -1,    -1,
     668,    -1,    -1,    -1,   674,   673,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   683,    -1,   685,    -1,    -1,   688,    -1,
     690,    -1,   692,    -1,    -1,    -1,    -1,    -1,   698,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   716,    -1,
     718,    -1,    -1,   721,    -1,   723,    -1,   725,    -1,   727,
      -1,    -1,   730,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     738,    -1,    -1,   741,    -1,   743,    -1,    -1,    -1,    -1,
     748,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     770,    -1,   772,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   787,    -1,   789,
      -1,    -1,    -1,    -1,    -1,    -1,   796,    -1,   798,    -1,
     800,    -1,   802,    -1,   761,    -1,   317,   318,    -1,   320,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   781,    -1,   783,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   837,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   845,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   820,    -1,    -1,   823,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   832,    -1,    -1,    -1,    -1,
      -1,   838,    -1,    -1,   841,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   905,    -1,    -1,    -1,    -1,
     910,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   922,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     928,    -1,    -1,   933,    -1,    -1,    -1,    -1,    -1,    -1,
     940,    -1,    -1,   943,   944,   945,   946,   947,   948,    -1,
      -1,   951,    -1,   953,    -1,    -1,   956,    -1,   469,    -1,
      -1,   961,    -1,   963,   964,   965,   477,    -1,    -1,    -1,
      -1,   482,   972,    -1,    -1,    -1,    -1,    -1,   489,   490,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     109,   110,   111,   112,   113,   114,   507,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,    -1,    -1,   142,   143,   144,   145,   146,  1026,    -1,
      -1,    -1,   989,    -1,    -1,  1035,    -1,    -1,    -1,    -1,
      -1,   998,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1050,    -1,  1052,    -1,    -1,    -1,    -1,    -1,    -1,  1057,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1068,  1069,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1087,    -1,    -1,
      -1,    -1,  1092,    -1,    -1,  1093,  1096,    -1,    -1,    -1,
      -1,  1099,    -1,  1101,    -1,    -1,  1104,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1071,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1131,    -1,    -1,    -1,  1094,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   666,   667,    -1,  1158,    -1,
    1160,   672,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   687,  1175,   689,  1177,
     691,    -1,   693,    -1,  1141,    -1,    -1,  1144,    -1,    -1,
      -1,  1148,    -1,    -1,    -1,  1193,    -1,    -1,    -1,  1199,
      -1,    -1,  1159,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1213,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1221,    -1,  1223,  1224,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1204,  1205,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1217,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1269,
      -1,    -1,    -1,    -1,    -1,    -1,  1276,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1288,    -1,  1292,    -1,  1294,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1308,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1319,    -1,    -1,    -1,    -1,    -1,    -1,  1326,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1334,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   858,   859,   860,
     861,    -1,   863,   864,    -1,  1355,  1356,     4,     5,    -1,
      -1,    -1,   873,    10,   875,   876,    -1,    -1,    15,    -1,
      17,    18,   883,   884,    -1,    -1,    -1,    24,    -1,    -1,
      27,    -1,    -1,    -1,    -1,   896,    -1,    34,    35,    -1,
      37,    -1,    39,  1393,    -1,    -1,    -1,    44,    -1,    46,
    1400,    -1,    49,    -1,    -1,    52,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    -1,    -1,    63,    -1,    65,    66,
      67,    68,  1422,    -1,    -1,    -1,    -1,    -1,    -1,    76,
      77,    -1,    79,    80,    81,    -1,    83,    84,    85,    86,
      87,    88,    -1,    90,    91,    92,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1011,    -1,    -1,    -1,    -1,  1016,  1017,    -1,  1019,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1027,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,    -1,    -1,    -1,
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
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1188,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1203,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1284,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1312,    -1,    -1,  1315,  1316,    -1,  1318,    -1,    -1,
      -1,  1322,  1323,    -1,  1325,    -1,  1327,    -1,    -1,  1330,
    1331,    -1,  1333,    -1,  1335,    -1,  1337,  1338,    -1,  1340,
    1341,    -1,  1343,  1344,    -1,  1346,    -1,  1348,  1349,    -1,
    1351,    -1,  1353,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1392,    -1,  1394,  1395,    -1,  1397,  1398,  1399,    -1,
    1401,  1402,  1403,  1404,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1421,    -1,  1423
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
     203,     7,    11,    12,    13,   256,   257,   386,   203,   203,
     203,   180,   195,   196,   203,   218,   220,   223,   229,   234,
     229,   234,   234,   234,   169,   226,   169,   233,   184,   205,
     237,   494,   167,   385,   205,   431,   204,   383,   429,   513,
     516,   204,   204,   204,   204,   259,   419,   419,   259,   259,
     259,   202,   259,   204,   168,   198,   203,   204,   204,   204,
     204,   209,   209,   209,   209,   204,   204,   230,   203,   207,
     203,   204,   330,   507,   202,   204,   514,     8,   282,   284,
     281,   284,   282,   283,   284,   203,   203,   266,   281,   180,
     197,   198,   223,   229,   234,   229,   234,   234,   234,   169,
     227,   260,   203,     7,    11,    12,    13,    14,    69,   426,
     427,   428,   203,   202,   384,   208,   429,   516,   203,   204,
     272,   202,   204,   270,   202,   207,   204,   275,   202,   204,
     204,   392,   198,   204,   204,   204,   209,   204,   330,   202,
     504,   204,   514,   514,     7,    11,    12,    13,    14,    69,
      89,   208,   252,   253,   254,   255,   261,   265,   305,   205,
     285,   208,   281,   305,   285,   208,   283,   285,   267,    36,
     208,   305,   393,   394,   229,   234,   234,   234,   169,   228,
     203,   266,   203,   385,   202,   202,   204,   204,   270,   207,
     204,   275,   204,   259,   204,   207,   207,   202,   204,   203,
     204,   262,   204,   505,   259,   266,   266,   208,   108,   109,
     110,   111,   112,   113,   114,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   142,
     143,   144,   145,   146,   298,   107,   115,   140,   141,   286,
     289,   298,   107,   115,   140,   141,   291,   294,   298,   203,
     393,   205,   395,   234,   267,   203,   208,   494,   507,   203,
     203,   204,   262,   204,   296,   204,   204,   204,   204,   203,
     204,   204,   204,   204,   204,   203,   204,   207,   330,   259,
     207,   202,   267,   203,    22,   238,   261,   297,   303,   304,
     203,    20,   238,   253,   287,   299,   300,   303,   287,    21,
     238,   253,   288,   301,   302,   303,   288,   238,   253,   290,
     303,   238,   292,   299,   303,   287,   238,   293,   301,   303,
     293,   238,   295,   303,   203,   504,   259,   259,   259,   204,
     259,   202,   259,   259,   204,   259,   202,   204,   259,   259,
     259,   204,   259,   202,   204,   259,   259,   259,   204,   259,
     259,   204,   259,   259,   259,   204,   259,   259,   259,   259,
     203,   203,   261,   180,   253,   303,   169,   253,   253,   303,
     169,   253,   253,   303,   303,   505,   259,   203,   259,   204,
     259,   204,   259,   259,   204,   259,   203,   259,   259,   259,
     259,   253,   258,   253,   259,   203,   259
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
     381,   382,   382,   383,   384,   385,   386,   387,   387,   388,
     389,   390,   390,   391,   392,   392,   392,   392,   392,   393,
     394,   395,   396,   396,   397,   398,   398,   398,   399,   400,
     400,   401,   402,   402,   403,   404,   405,   406,   406,   407,
     408,   409,   410,   410,   410,   410,   410,   411,   411,   412,
     413,   414,   414,   415,   416,   417,   418,   419,   419,   419,
     419,   420,   421,   422,   423,   424,   425,   426,   427,   428,
     428,   428,   428,   428,   428,   429,   430,   431,   432,   432,
     432,   433,   433,   434,   435,   435,   436,   437,   437,   438,
     439,   440,   441,   441,   441,   442,   443,   444,   445,   446,
     447,   448,   449,   450,   450,   450,   450,   451,   452,   452,
     453,   454,   455,   456,   457,   458,   459,   460,   461,   462,
     463,   464,   465,   466,   466,   466,   466,   466,   466,   466,
     466,   466,   466,   466,   466,   467,   467,   468,   468,   468,
     469,   470,   471,   472,   472,   473,   473,   473,   474,   475,
     475,   476,   477,   477,   477,   477,   477,   477,   477,   477,
     477,   477,   477,   477,   477,   477,   478,   478,   478,   478,
     478,   478,   478,   479,   480,   480,   481,   482,   482,   482,
     482,   482,   482,   482,   483,   484,   485,   486,   487,   488,
     489,   490,   491,   492,   493,   494,   495,   496,   497,   497,
     498,   499,   499,   499,   499,   499,   500,   501,   502,   502,
     503,   504,   504,   504,   504,   505,   505,   505,   505,   506,
     507,   508,   509,   510,   511,   511,   512,   513,   513,   514,
     514,   514,   514,   515,   516
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
       7,     0,     3,     1,     1,     1,     1,     1,     1,     8,
      10,     1,     1,    10,     0,     3,     5,     3,     2,     5,
       1,     1,     1,     1,     5,     1,     1,     1,     8,     1,
       1,     5,     1,     1,     8,     1,     5,     1,     1,     8,
       1,     5,     0,     3,     5,     3,     3,     1,     1,     4,
       1,     1,     1,     4,     1,     1,     7,     0,     3,     3,
       3,     1,     1,     5,     1,     1,     9,     1,     5,     1,
       1,     1,     1,     1,     1,     7,     1,     1,     1,     1,
       1,     1,     1,    10,     1,     1,    10,     1,     1,    10,
      10,     7,     0,     3,     3,     9,     7,     9,    10,     1,
       1,     9,     1,     1,     1,     1,     1,    10,     1,     1,
       7,     9,     1,    10,     7,     1,    10,     7,     1,    10,
       7,     1,     9,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     3,     2,
       1,     1,     4,     1,     1,     1,     2,     3,     4,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     3,     1,     8,     0,     3,     3,
       3,     5,     3,     2,     1,     1,     4,     1,     1,     4,
       1,     4,     1,     4,     1,     4,     1,     4,     3,     1,
       6,     0,     3,     3,     3,     2,     1,     4,     3,     1,
      16,     1,     1,     1,     1,     0,     6,     3,     2,     1,
       1,     9,     1,     4,     3,     1,     6,     1,     1,     0,
       3,     3,     2,     1,     7
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
