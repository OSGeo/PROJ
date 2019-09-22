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
#define YYLAST   3309

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  164
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  350
/* YYNRULES -- Number of rules.  */
#define YYNRULES  687
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1411

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
     553,   553,   555,   557,   564,   564,   564,   566,   573,   574,
     575,   576,   578,   585,   592,   593,   594,   596,   598,   598,
     598,   598,   598,   598,   598,   598,   598,   601,   601,   601,
     603,   603,   605,   605,   605,   607,   612,   618,   623,   626,
     629,   630,   631,   632,   633,   634,   635,   636,   637,   640,
     641,   642,   643,   644,   645,   646,   647,   650,   651,   652,
     653,   654,   655,   656,   657,   660,   661,   664,   665,   666,
     667,   672,   673,   674,   675,   676,   677,   678,   679,   680,
     683,   684,   685,   686,   689,   690,   691,   692,   695,   696,
     699,   700,   705,   706,   709,   710,   711,   712,   715,   716,
     717,   718,   719,   720,   721,   722,   723,   724,   725,   726,
     727,   728,   729,   730,   731,   732,   733,   734,   735,   736,
     737,   738,   739,   740,   741,   742,   743,   744,   745,   746,
     747,   748,   749,   750,   752,   755,   757,   759,   761,   763,
     765,   781,   781,   783,   791,   792,   794,   795,   797,   805,
     806,   808,   810,   812,   817,   818,   820,   822,   824,   826,
     828,   830,   832,   837,   841,   843,   846,   849,   850,   851,
     853,   854,   856,   861,   862,   864,   864,   866,   870,   870,
     870,   872,   872,   874,   882,   891,   899,   909,   910,   912,
     914,   914,   916,   916,   919,   920,   924,   930,   931,   932,
     934,   934,   936,   938,   940,   944,   949,   949,   951,   954,
     955,   959,   964,   964,   964,   966,   968,   969,   970,   971,
     973,   976,   978,   982,   988,   988,   992,   992,   993,   993,
     995,  1000,  1001,  1002,  1003,  1004,  1006,  1012,  1017,  1023,
    1025,  1027,  1029,  1033,  1039,  1040,  1041,  1043,  1045,  1047,
    1051,  1051,  1053,  1055,  1060,  1061,  1063,  1065,  1067,  1069,
    1073,  1073,  1075,  1081,  1088,  1088,  1091,  1098,  1099,  1100,
    1101,  1102,  1104,  1108,  1110,  1112,  1112,  1116,  1121,  1121,
    1121,  1125,  1130,  1130,  1132,  1136,  1136,  1140,  1145,  1147,
    1151,  1151,  1155,  1160,  1162,  1166,  1167,  1168,  1169,  1170,
    1172,  1172,  1174,  1177,  1179,  1179,  1181,  1183,  1185,  1189,
    1195,  1196,  1197,  1198,  1200,  1202,  1206,  1211,  1213,  1216,
    1221,  1225,  1231,  1231,  1231,  1231,  1231,  1231,  1235,  1240,
    1242,  1247,  1247,  1248,  1250,  1250,  1252,  1259,  1259,  1261,
    1268,  1268,  1270,  1277,  1284,  1289,  1290,  1291,  1293,  1299,
    1304,  1312,  1318,  1320,  1322,  1328,  1330,  1330,  1331,  1331,
    1335,  1341,  1341,  1343,  1348,  1354,  1359,  1365,  1370,  1375,
    1381,  1386,  1391,  1397,  1402,  1407,  1413,  1413,  1414,  1414,
    1415,  1415,  1416,  1416,  1417,  1417,  1418,  1418,  1421,  1421,
    1423,  1424,  1425,  1427,  1429,  1433,  1436,  1436,  1439,  1440,
    1441,  1443,  1447,  1448,  1450,  1452,  1452,  1453,  1453,  1454,
    1454,  1454,  1455,  1456,  1456,  1457,  1457,  1458,  1458,  1460,
    1460,  1461,  1461,  1462,  1463,  1463,  1467,  1471,  1472,  1475,
    1480,  1481,  1482,  1483,  1484,  1485,  1486,  1488,  1490,  1492,
    1495,  1497,  1499,  1501,  1503,  1505,  1507,  1509,  1511,  1513,
    1518,  1522,  1523,  1526,  1531,  1532,  1533,  1534,  1535,  1537,
    1542,  1547,  1548,  1551,  1557,  1557,  1557,  1557,  1559,  1560,
    1561,  1562,  1564,  1566,  1571,  1577,  1579,  1584,  1585,  1588,
    1594,  1594,  1596,  1597,  1598,  1599,  1601,  1603
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

#define YYPACT_NINF -1187

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-1187)))

#define YYTABLE_NINF -626

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     820, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187,
   -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187,
   -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187,
   -1187, -1187, -1187, -1187, -1187, -1187, -1187,   147, -1187, -1187,
   -1187,   213, -1187, -1187, -1187,   213, -1187, -1187, -1187, -1187,
   -1187, -1187,   213,   213, -1187,   213, -1187,   213, -1187,   213,
   -1187,   213, -1187, -1187, -1187,   213, -1187,   213, -1187,   213,
   -1187,   213, -1187,   213, -1187,   213, -1187,   213, -1187,   213,
   -1187, -1187, -1187, -1187, -1187, -1187, -1187,   213, -1187, -1187,
   -1187, -1187, -1187, -1187,   213, -1187,   213, -1187,   213, -1187,
     213, -1187,   213, -1187,   213, -1187, -1187, -1187,    19,    19,
      19,    19,    19,    19,    19,    19,    19,    19,    19,    19,
      19,    19,    19,    19,    19,    19,   928,    19,    19,    19,
     164, -1187, -1187,    17, -1187,    17, -1187,    17,    17, -1187,
      17, -1187,    17,    17, -1187,    17,    17,    17,    17,    17,
      17,    17,    17,    17, -1187,    17, -1187,    17, -1187, -1187,
   -1187, -1187,    78, -1187, -1187, -1187, -1187, -1187,    94,   167,
     185, -1187, -1187, -1187, -1187,   345, -1187,    17, -1187,    17,
      17,    17, -1187,    17,   213, -1187,  1290,   154,    97,    97,
     680,   330,   412,   301,   499,   113,   345,   304,   345,   312,
     345,   160,   255,   345,   231,  1501, -1187, -1187, -1187,   487,
     100, -1187, -1187,   100, -1187, -1187,   100, -1187, -1187,   382,
     928, -1187, -1187, -1187, -1187, -1187, -1187, -1187,   385, -1187,
   -1187, -1187, -1187,   292,   299,   317,   680, -1187,    17, -1187,
      17,   213, -1187, -1187, -1187, -1187,   213,    17,   213,    17,
   -1187,   213,   213,    17,    17, -1187, -1187, -1187, -1187,    17,
      17,    17,    17, -1187,    17,    17,    17, -1187, -1187, -1187,
   -1187,   213,   213, -1187, -1187,    17,   213, -1187, -1187,   213,
      17,    17, -1187,    17, -1187, -1187,   213, -1187,    17,    17,
     213, -1187, -1187,    17,    17,   213, -1187, -1187,    17,    17,
     213, -1187, -1187,    17,    17,   213, -1187, -1187,    17,    17,
     213,    17,   213, -1187, -1187,    17,   213, -1187,    17, -1187,
   -1187, -1187, -1187,   213, -1187,    17,   213,    17,    17,    17,
      17,    17, -1187,    17,   213,   345, -1187,   459,   385, -1187,
   -1187,   329,   345,   157,   345,   345,    19,    19,   102,   422,
     112,    19,    19,   442,   442,   102,   112,   442,   442,   680,
     345,   470,    19,    19,   243,   345,    19,    19,   242,   496,
     442,    19,   486, -1187,   486,    19,   496,   442,    19,   496,
     442,    19,   496,   442,    19, -1187, -1187,   669,   107, -1187,
      19,   442,    19,  1501,   385,   164, -1187,    19,   382,   164,
   -1187,   505,   164, -1187,   382,   493,   928, -1187,   385, -1187,
   -1187, -1187, -1187, -1187, -1187, -1187, -1187,    17,    17,   213,
   -1187,   213, -1187, -1187,    17,    17,   213,    17, -1187, -1187,
   -1187,    17,    17,    17, -1187,    17,   213, -1187, -1187, -1187,
   -1187, -1187, -1187, -1187,   213,   345,    17,   213, -1187,    17,
      17, -1187,    17,   213,    17,    17,   345,    17,    17, -1187,
      17, -1187,    17,    17,    17, -1187,    17,    17,   213, -1187,
   -1187,    17,    17,    17,   213,   345,    17,    17,    17,    17,
   -1187,   345,   345,    17,    17,   345,    17,    17,   345,    17,
      17, -1187, -1187,   363, -1187,   345,    17, -1187,   345,    17,
      17,    17,   317,   345, -1187,   345,    17, -1187,    17,   213,
      17, -1187,    17,   213,   345, -1187,   489,   514,    19,    19,
   -1187, -1187,   486, -1187,  1298,   506,   486,   345,   154,   112,
     565,   345,   385,  1217, -1187,   496,    19,    90,    90,   496,
      19,   496,   112, -1187,   496,   496,   327,   345,   496,    90,
      90, -1187, -1187,    19,   345,   154,   496,  1258, -1187,   496,
     364, -1187, -1187, -1187, -1187,   496,   108, -1187,   496,   245,
   -1187,   496,    72, -1187, -1187,   385, -1187, -1187,   385, -1187,
   -1187, -1187,   496,   412,  1402,   345,   385, -1187, -1187,   505,
     694,   345,    19,   534,  1206,   345,    19, -1187,    17, -1187,
   -1187,   345, -1187,   345, -1187,    17, -1187,   345,    17, -1187,
      17, -1187,    17,   345, -1187, -1187, -1187,   213, -1187,   317,
     345, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187,
   -1187, -1187, -1187, -1187, -1187,    17,    17,    17, -1187, -1187,
      17,    17,    17,    17,    17,   345, -1187,    17,   345,   345,
     345,   345, -1187, -1187,    17,    17,   213, -1187,   345,    17,
      17,    17,    17, -1187,    17, -1187,    17,   345,    17,   345,
      17,    17,   345,    17,   345,    17,   345,    17,   386,   427,
   -1187,   635,   345, -1187, -1187, -1187, -1187,    17, -1187, -1187,
   -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187,    17,
     213,    17,   213, -1187,    17,   213,    17,   213,    17,   213,
      17,   213,    17, -1187,   213,    17, -1187, -1187,    17, -1187,
   -1187, -1187,   213,    17,    17,   213,    17,   213, -1187, -1187,
      17, -1187,   213, -1187, -1187,    17,   514, -1187, -1187, -1187,
   -1187, -1187, -1187,   140, -1187,    19,   385, -1187,   398,   398,
     398,   459,   102,   131,   345,   102,   345, -1187,   505, -1187,
   -1187, -1187, -1187, -1187, -1187,    19, -1187,   102,   132,   345,
     102,   345,   459,   568, -1187,   398, -1187,   242,   345, -1187,
     345, -1187,   345, -1187,   345, -1187,   385, -1187, -1187,   385,
     385, -1187,   437, -1187, -1187, -1187, -1187,   470,   315,   572,
     754, -1187,    19,   527, -1187,    19,   323, -1187,  1298,   115,
   -1187,  1298,   474, -1187,   669, -1187,   462, -1187,   688,   345,
      19, -1187, -1187,    19, -1187,  1298,   486,   345,   151,   101,
   -1187, -1187, -1187,    17, -1187,    17, -1187, -1187, -1187, -1187,
      17,    17,    17,    17,   680,   345,    17,    17,    17, -1187,
      17, -1187,    17, -1187,    17,    17, -1187, -1187,    17, -1187,
     213,    17,    17, -1187,    17, -1187, -1187,    17,    17,    17,
      17, -1187, -1187, -1187, -1187, -1187,   449,   437, -1187,   635,
     385, -1187,    17, -1187,    17, -1187,    17, -1187,    17, -1187,
   -1187,   345,    17,    17,    17, -1187,   345,    17,    17, -1187,
      17,    17, -1187,    17, -1187, -1187,    17, -1187,   345, -1187,
   -1187,    17,    17,    17,   213,    17, -1187,    17,    17,   345,
   -1187, -1187, -1187, -1187, -1187, -1187,   345,    17,    17,   345,
     345,   345,   345,   390, -1187, -1187, -1187,   345, -1187, -1187,
     345,   179,   345,   680,   345, -1187,    19,   390, -1187, -1187,
     345,   733,   345,   345,   345, -1187, -1187,   385, -1187, -1187,
   -1187,   345, -1187,   457, -1187, -1187,   323, -1187,   115, -1187,
   -1187, -1187,   115, -1187, -1187,  1298, -1187,  1298,   669, -1187,
   -1187, -1187,   991, -1187,   928, -1187,   459,    19, -1187,    17,
     114,   505, -1187, -1187,    17,    17,    17,    17, -1187, -1187,
      17,    17,    17, -1187, -1187,    17, -1187,    17,    17, -1187,
   -1187, -1187, -1187, -1187, -1187,   213,    17, -1187,    17, -1187,
   -1187, -1187,  1034, -1187,   345,    17,    17,    17, -1187,    17,
      17,    17,    17, -1187,    17, -1187,    17, -1187, -1187,   345,
      17,   345,    17, -1187,    17,   534,   213, -1187,    17, -1187,
     596,   596,   596, -1187, -1187, -1187, -1187,   345,   680, -1187,
      19, -1187,   596,  1148, -1187, -1187,   190,   591,   557,   115,
   -1187, -1187, -1187, -1187,  1298,   376,   345, -1187, -1187, -1187,
     287,   345,   213,    19,   972,   345, -1187,    17,   213,    17,
     213,    17,   213, -1187,    17,    17,    17,   204,  1148, -1187,
      17,    17, -1187,    17, -1187, -1187,    17, -1187,    17, -1187,
   -1187, -1187, -1187, -1187, -1187, -1187, -1187,    17, -1187,   213,
   -1187,   151,    17, -1187,    17,    17, -1187,   906, -1187,    19,
   -1187,    19,   566, -1187,    19,   345,   680,  1240, -1187, -1187,
     591,   557,   557, -1187,  1298,   345,    19,   345,   459, -1187,
   -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187,
   -1187, -1187,   213, -1187,   213,    17,    17, -1187,    17,    17,
   -1187,    17,    17, -1187,    17, -1187, -1187,    17,    17,   213,
      17, -1187, -1187, -1187, -1187,   345, -1187,    17,    17,    17,
      19,    19, -1187, -1187,  1544,  1736, -1187,  1584,   345,  1080,
   -1187, -1187,    19,   557, -1187,   680,   345,   899,   345,   345,
      17,    17,    17, -1187, -1187, -1187, -1187, -1187, -1187, -1187,
      17, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187,
   -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187,
   -1187, -1187, -1187, -1187, -1187,    17,    17, -1187, -1187, -1187,
   -1187, -1187,   345, -1187,    17,    17,    17,    17,    17,    17,
     345, -1187,    17, -1187,    17, -1187,    17, -1187,    17, -1187,
   -1187,    17,   213, -1187, -1187,   680,   345,   417,   417,   625,
     625, -1187,   605,   137,   345,   447,   417,   404,   404, -1187,
     476, -1187,   345, -1187, -1187,   151,    17, -1187, -1187, -1187,
      17,    17, -1187,    17,   213,    17,   213, -1187, -1187,    17,
      17, -1187,    17,   213,    17, -1187,    17,    17, -1187,    17,
      17,    17, -1187,    17, -1187,    17, -1187,    17,    17, -1187,
      17, -1187,    17,    17, -1187,    17, -1187,    17, -1187,   345,
     345, -1187, -1187,   605, -1187,  1298,   379, -1187,   385, -1187,
   -1187,   605, -1187,  1298,   379, -1187, -1187, -1187,   379, -1187,
   -1187, -1187,   103, -1187, -1187,   476, -1187, -1187, -1187,   476,
   -1187, -1187, -1187, -1187,    17, -1187,    17,    17,    17,    17,
     345,    17,    17,   345,    17,    17,    17,    17,    17, -1187,
   -1187,   379, -1187,   490, -1187, -1187, -1187,   379, -1187, -1187,
   -1187, -1187, -1187, -1187, -1187,    17,   345,    17, -1187, -1187,
   -1187
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   412,   401,   390,   400,   161,   424,   447,   392,   475,
     478,   593,   637,   672,   675,   500,   493,   351,   552,   485,
     482,   490,   488,   604,   659,   391,   414,   425,   393,   413,
     476,   480,   479,   501,   486,   483,   491,     0,     4,     5,
       2,     0,    13,   341,   342,     0,   576,   380,   378,   379,
     381,   382,     0,     0,     3,     0,    12,     0,   578,     0,
      11,     0,   580,   460,   461,     0,    14,     0,   582,     0,
      15,     0,   584,     0,    16,     0,   586,     0,    17,     0,
     577,   533,   531,   532,   534,   535,   579,     0,   581,   583,
     585,   587,    19,    18,     0,     7,     0,     8,     0,     9,
       0,    10,     0,     6,     0,     1,    73,    74,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    78,   162,     0,   352,     0,   389,     0,     0,   402,
       0,   415,     0,     0,   448,     0,     0,   416,     0,   416,
       0,   416,     0,   495,   553,     0,   594,     0,   605,   619,
     606,   620,   607,   608,   622,   609,   610,   611,   612,   613,
     614,   615,   616,   617,   618,     0,   602,     0,   638,     0,
       0,     0,   643,     0,     0,    77,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    75,    76,   601,     0,
       0,   626,   628,     0,   650,   652,     0,   660,   662,     0,
       0,    40,    20,    37,    38,    39,    41,    42,     0,   163,
      21,    22,    26,     0,    25,    35,     0,   164,   154,   356,
       0,     0,   439,   440,   364,   395,     0,     0,     0,     0,
     394,     0,     0,     0,     0,   537,   540,   538,   541,     0,
       0,     0,     0,   403,     0,   416,     0,   426,   427,   428,
     429,     0,     0,   451,   450,   444,     0,   565,   465,     0,
       0,     0,   464,     0,   561,   562,     0,   421,   190,   417,
       0,   477,   568,     0,     0,     0,   484,   571,     0,     0,
       0,   489,   574,     0,     0,     0,   507,   503,   190,   190,
       0,   190,     0,   494,   555,     0,     0,   588,     0,   589,
     596,   597,   603,     0,   640,     0,     0,     0,     0,     0,
       0,     0,   645,     0,     0,     0,    34,    27,     0,    33,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   418,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   499,   498,     0,     0,   496,
       0,     0,     0,     0,     0,     0,   627,     0,     0,     0,
     651,     0,     0,   661,     0,     0,     0,   642,     0,    29,
      31,    28,    36,   168,   171,   165,   166,   155,   158,     0,
     160,     0,   153,   360,     0,   346,     0,     0,   343,   348,
     357,   354,     0,     0,   366,   370,     0,   406,   217,   407,
     388,   204,   205,   206,     0,     0,     0,     0,   441,     0,
       0,   514,     0,     0,     0,     0,     0,     0,     0,   404,
     397,   411,     0,     0,     0,   456,   190,   444,     0,   443,
     452,   190,     0,     0,     0,     0,     0,     0,   190,   190,
     422,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    50,   504,    48,   505,     0,   190,   508,     0,     0,
       0,   590,   598,     0,   641,     0,     0,   517,   654,     0,
       0,   686,    80,     0,     0,    32,     0,     0,     0,     0,
     345,   350,     0,   349,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   383,     0,     0,     0,     0,     0,
       0,     0,     0,   385,     0,     0,     0,     0,     0,     0,
       0,   445,   446,     0,     0,     0,     0,     0,   462,     0,
       0,   191,   419,   420,   481,     0,     0,   487,     0,     0,
     492,     0,     0,    44,    58,     0,    45,    49,     0,   502,
     497,   506,     0,     0,     0,     0,   599,   595,   639,     0,
       0,     0,     0,     0,     0,     0,     0,   644,   156,   159,
     169,     0,   172,     0,   362,   346,   361,     0,   346,   358,
     354,   353,     0,     0,   375,   376,   371,     0,   363,   367,
       0,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,     0,     0,     0,   387,   408,
       0,     0,   545,     0,   545,     0,   515,     0,     0,     0,
       0,     0,   199,   198,   190,   190,     0,   396,     0,     0,
     431,     0,   431,   457,     0,   449,     0,     0,     0,     0,
       0,   190,     0,   190,     0,   190,     0,   190,    48,     0,
      59,     0,     0,   556,   557,   558,   559,     0,   174,   100,
     133,   136,   144,   148,    98,   592,    82,    88,    89,    93,
       0,    85,     0,    92,    85,     0,    85,     0,    85,     0,
      85,     0,    85,    84,     0,   590,   575,   600,   630,   529,
     649,   658,     0,   654,   654,     0,    80,     0,   653,   518,
     373,   673,     0,    81,   674,     0,     0,   167,   170,   347,
     359,   344,   355,     0,   384,     0,   368,   365,     0,     0,
       0,    27,     0,     0,     0,     0,     0,   536,     0,   539,
     386,   542,   543,   399,   398,     0,   423,     0,     0,     0,
       0,     0,    27,     0,   463,     0,   560,     0,     0,   566,
       0,   569,     0,   572,     0,    46,     0,    43,    68,     0,
       0,    53,    71,    55,    66,    67,   551,     0,     0,     0,
       0,    91,     0,     0,   117,     0,     0,   118,     0,     0,
     119,     0,     0,   120,     0,    83,     0,   591,     0,     0,
       0,   655,   656,     0,   657,     0,     0,     0,     0,     0,
     676,   678,   157,     0,   377,   373,   369,   232,   233,   234,
     190,   190,   190,   190,     0,     0,   545,   190,   190,   544,
     545,   549,   510,   202,     0,   431,   197,   196,   190,   442,
       0,   190,   190,   430,   431,   437,   458,   454,     0,   190,
     190,   563,   567,   570,   573,    52,    48,    71,    60,     0,
       0,    70,   190,    96,    85,    94,     0,    90,    85,    87,
     101,     0,    85,    85,    85,   134,     0,    85,    85,   137,
       0,    85,   145,     0,   149,   150,     0,    79,     0,   647,
     636,   630,   630,    80,     0,    80,   629,     0,     0,     0,
     374,   516,   666,   667,   664,   665,     0,     0,     0,     0,
       0,     0,     0,     0,   410,    24,   405,     0,   547,   546,
       0,     0,     0,     0,     0,   435,     0,     0,   432,   434,
       0,     0,     0,     0,     0,    47,    69,     0,    54,    57,
      72,     0,    95,     0,    86,    99,     0,   121,     0,   122,
     123,   132,     0,   124,   125,     0,   126,     0,     0,   173,
     631,   632,     0,   633,     0,   635,    27,     0,   648,     0,
       0,     0,   677,   372,     0,     0,     0,   190,   548,   550,
     190,   510,   510,   509,   203,   190,   436,     0,   190,   438,
     188,   186,   185,   187,   459,     0,   190,   453,     0,   564,
      64,    56,     0,   554,     0,   102,   103,   104,   105,    85,
      85,    85,    85,   138,     0,   146,   142,   151,   152,     0,
      80,     0,     0,   530,   373,     0,     0,   681,   682,   680,
       0,     0,     0,   409,   513,   511,   512,     0,     0,   433,
       0,   455,     0,     0,    63,    97,     0,     0,     0,     0,
     127,   128,   129,   130,     0,     0,     0,   147,   634,   646,
       0,     0,     0,     0,     0,     0,   238,   208,     0,    80,
       0,   214,     0,   192,   190,     0,   467,    65,     0,    61,
     106,   107,   108,   109,   110,   111,    85,   139,     0,   143,
     141,   527,   522,   523,   524,   525,   526,   373,   520,     0,
     528,     0,     0,   685,   682,   682,   679,     0,   207,     0,
     212,     0,     0,   213,     0,     0,     0,     0,   466,    62,
       0,     0,     0,   131,     0,     0,     0,     0,    27,   684,
     683,   183,   180,   179,   182,   200,   181,   201,   211,   340,
     175,   177,     0,   176,     0,   208,    80,   239,     0,     0,
     216,   214,     0,   189,   190,   473,   471,    80,    80,     0,
     112,   113,   114,   115,   140,     0,   519,   194,   668,   190,
       0,     0,   210,   209,     0,     0,   215,     0,     0,     0,
     468,   470,     0,     0,   135,     0,     0,     0,     0,     0,
       0,   194,   241,   298,   299,   300,   301,   302,   303,   304,
     243,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   245,   247,   329,   330,   331,
     332,   333,     0,   240,   265,   292,   272,   274,   276,   278,
       0,   271,   288,   184,    80,   474,   373,   116,   190,   521,
     671,    80,     0,   663,   687,     0,     0,     0,     0,     0,
       0,   235,     0,     0,     0,     0,     0,     0,     0,   237,
       0,   469,     0,   195,   670,     0,   190,   193,   335,   339,
     190,   190,   242,   190,     0,   190,     0,   244,   337,   190,
     190,   246,   190,     0,   190,   248,   190,   190,   266,   190,
     190,   190,   293,   190,   236,   190,   273,   190,   190,   275,
     190,   277,   190,   190,   279,   190,   289,   190,   472,     0,
       0,   249,   256,     0,   253,     0,     0,   255,     0,   257,
     264,     0,   261,     0,     0,   263,   267,   270,     0,   268,
     294,   297,     0,   295,   280,     0,   282,   283,   284,     0,
     286,   287,   290,   291,   668,   178,   190,   190,     0,   190,
       0,   190,   190,     0,   190,   190,   190,   190,   190,   669,
     252,     0,   250,     0,   254,   338,   260,     0,   258,   336,
     262,   269,   296,   281,   285,   190,     0,   190,   251,   334,
     259
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1187, -1187, -1187,  -219,  -226,  -180, -1187,  -126,  -132,   296,
   -1187, -1187, -1187, -1187, -1187, -1187,  -223,  -334,  -589,   -38,
    -715,  -585, -1187, -1187, -1187, -1187, -1187, -1187, -1187,  -528,
    -245, -1187, -1187, -1187,  -831, -1187, -1187,  -238,   -41,  1769,
     915,   -44, -1187,  -707,  -567,  -577, -1187, -1187,  -147, -1187,
   -1187,  -144, -1187, -1187, -1187,  -142,  -298, -1187, -1187,  -741,
   -1187, -1187, -1187, -1187, -1187,  -708, -1187, -1187, -1187, -1187,
    -553, -1187, -1187, -1187,     0, -1187, -1187, -1187, -1187, -1187,
     144, -1187, -1187,  -470, -1187, -1187,  -698, -1187, -1187,  -819,
   -1187, -1187, -1187, -1187,  -908,  1902,  -408, -1186,  -531, -1187,
   -1187, -1187,  -904, -1002,   -40, -1187,  -483, -1187, -1187,  -484,
    -278,   133, -1187, -1187,  -513,  -943, -1187,  -440,  -939,  -726,
   -1187,  -959,  -581, -1187, -1187, -1187, -1187,  -593, -1187, -1187,
   -1187, -1187,  -641,  -572, -1187,  -639, -1187,  -680, -1187,  -427,
   -1187,   715,  -397,  -162,   522,  -395,    52,  -184,  -308,   117,
   -1187, -1187, -1187,   197, -1187,   -92, -1187,  -149, -1187, -1187,
   -1187, -1187, -1187, -1187,  -804, -1187, -1187, -1187, -1187,   609,
     610,   612,   613,  -246,   582, -1187, -1187,   -93,    88, -1187,
   -1187, -1187, -1187, -1187,  -529, -1187, -1187, -1187,     2, -1187,
     454,   -42, -1187, -1187, -1187,   624, -1187, -1187, -1187,  -575,
   -1187, -1187, -1187,   559,   560,   291,  -191,     7,   293, -1187,
   -1187, -1187, -1187, -1187, -1187, -1187,  -356,  -769,  -883, -1187,
   -1187,   633,   636, -1187,   207, -1187,  -433, -1187, -1187, -1187,
    -179, -1187,   641, -1187,  -148, -1187,   647, -1187,  -160, -1187,
     648, -1187,  -158, -1187, -1187,   387, -1187, -1187, -1187, -1187,
   -1187,   488,  -325, -1187, -1187,  -360, -1187, -1187,  -729, -1187,
   -1187, -1187,  -774, -1187, -1187,   651, -1187, -1187,   589, -1187,
     592, -1187, -1187,   199,  -558,   209,   211,   218,   676, -1187,
   -1187, -1187, -1187, -1187,   679, -1187, -1187, -1187, -1187,   684,
   -1187, -1187,   687, -1187, -1187,   689, -1187, -1187,   697,  -168,
    -303,   119, -1187, -1187, -1187, -1187, -1187, -1187, -1187, -1187,
   -1187, -1187,   827, -1187,   511,  -233, -1187,  -107,  -203, -1187,
   -1187,   -48, -1187,    43, -1187, -1187, -1187,  -785, -1187, -1187,
   -1187,   515,   -17,   839, -1187, -1187,   518, -1061,  -525, -1187,
    -923,   850, -1187, -1187, -1187,   -77, -1187,  -416, -1187,  -232
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   228,   606,   230,   866,   231,   844,
     232,   233,   409,   410,   234,   339,   235,   236,   880,   575,
     492,   576,   493,   679,   876,   577,   791,   958,   578,   792,
     879,  1021,  1022,  1098,   793,   794,   795,   881,   108,   208,
     372,   448,   908,   595,   733,   801,   696,   697,   698,   699,
     700,   701,   702,   891,  1024,   703,   704,   705,   896,   706,
     707,   900,  1034,  1108,  1185,   708,  1076,   709,   903,  1036,
     710,   711,   906,  1039,   478,   342,    41,   133,   238,   417,
     418,   419,   601,   420,   421,   603,   713,   714,  1159,  1301,
    1161,  1162,  1014,  1015,   859,   373,   655,  1163,  1206,   860,
     656,  1164,   854,  1005,   440,   441,  1128,   442,   443,  1133,
     444,   635,   636,   637,   840,  1087,  1089,  1091,  1088,  1168,
    1252,  1302,  1311,  1253,  1318,  1260,  1326,  1331,  1261,  1336,
    1284,  1322,  1254,  1303,  1304,  1312,  1313,  1305,  1306,  1166,
      42,   245,   344,   523,    44,   345,   246,   135,   240,   527,
     241,   431,   610,   425,   426,   607,   605,   247,   248,   435,
     436,   620,   531,   616,   827,   617,   835,    46,    47,    48,
      49,    50,    51,   445,   137,    52,    53,   249,   427,   547,
      55,   140,   264,   460,   446,   447,   640,   845,   250,    57,
     142,   196,   289,   290,   481,    58,    59,   266,   267,   769,
     268,   269,   270,   251,   252,   449,   862,   922,   365,    61,
     145,   275,   276,   471,   467,   952,   722,   664,   867,  1016,
      62,    63,    64,   281,   475,  1138,  1178,  1179,  1266,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,   203,    79,   309,   310,   495,   311,   312,
     498,   923,   942,   453,   647,   927,   509,   730,   723,  1117,
    1118,  1119,   724,   725,  1044,    80,    81,    82,   253,    83,
     254,    84,    85,   255,   754,   256,   257,   258,    86,    87,
     155,   315,   316,   687,    88,   283,   284,   285,   286,    89,
     294,   295,    90,   299,   300,    91,   304,   305,    92,    93,
     318,   585,    94,   157,   322,   323,   503,    95,   175,    96,
     176,   177,   924,   211,   212,   819,    98,   179,   325,   326,
     505,   327,   184,   333,   334,   913,   914,   726,   727,    99,
     214,   215,   591,   925,   101,   217,   218,   926,  1208,   102,
     732,   319,   104,   512,   830,   831,  1048,  1085,   513,  1049
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      40,   335,    56,   411,   109,   336,   229,    60,   468,   338,
     328,   110,   111,   330,   112,   282,   113,   695,   114,   824,
     115,   180,   181,   721,   116,   997,   117,   520,   118,   521,
     119,   929,   120,   915,   121,   424,   122,   317,   123,   298,
     259,   508,  1007,   303,   912,   280,   124,   599,   959,   293,
     681,   917,    45,   125,   918,   126,  1094,   127,   263,   128,
    1147,   129,   892,   130,   132,   134,   136,   136,   139,   141,
     136,   144,   136,   141,   136,   141,   136,   141,   136,   141,
     154,   156,   183,   178,   178,   178,   756,   771,    54,   911,
     501,   474,   790,   785,     1,   893,   260,  1321,   897,   904,
     885,     1,   889,  1042,   340,   889,     1,   198,   889,   200,
     456,   889,  1090,  1092,   889,   412,   242,  1155,   437,  1096,
       5,    15,  1082,   465,     5,   243,   507,   804,   438,   807,
       5,   810,     5,   813,  1174,   815,   287,   437,   437,   856,
     182,   688,   237,   220,   857,     2,   244,   105,     5,     5,
      17,  1155,   719,   244,     5,     4,  1095,    17,   307,  1299,
      33,   415,     2,   432,   491,   329,   131,  1002,   331,   693,
      26,   502,     4,    19,    29,     7,  1386,    26,   324,   324,
     185,    29,    26,   472,  1165,   515,    29,   514,   465,   473,
      12,  1064,  1157,  1092,   439,   288,     5,  1040,    34,   451,
     346,   239,   308,  1268,   182,   347,   983,   349,   985,    15,
     351,   352,  1001,   439,   439,   239,  1047,   719,   239,   423,
     612,   691,  1026,   360,   848,   317,  1157,   459,    24,   718,
     362,   363,  1099,   302,  1339,   366,   841,   842,   367,   861,
    1081,  -621,  1187,   692,   693,   371,   279,   666,    33,   375,
     894,   910,   465,   898,   378,  1027,   901,  -623,  1029,   381,
       5,   877,   869,  1037,   384,  1189,   832,  1139,   889,   387,
     889,   390,     5,  1296,   889,   392,   586,    10,   474,   265,
     944,   715,   394,   613,  1272,   397,  1210,  1211,   937,   950,
     957,   955,   940,   406,  1111,   314,   649,   416,  1112,  1113,
    1114,  1115,   430,   134,   273,   306,   307,   962,  1160,   619,
    1124,   964,    21,  1145,  1122,   967,   969,   970,  1160,  1307,
     973,   974,   470,   134,   976,  1100,   274,  1329,    31,    32,
    -624,   480,     5,  1078,   652,     2,   482,    36,   653,   485,
       5,   688,   488,   494,     5,     4,   497,   789,  -625,   688,
     433,   329,   678,   504,   331,   680,  1116,   455,  1101,  1103,
     -59,   413,   414,   717,   466,   -59,   -59,   -59,   462,    19,
     694,   889,   106,   107,   479,   292,   692,   693,   518,    21,
     519,   671,  1130,   652,   297,   524,  1151,   653,   641,   643,
    1152,  1153,  1154,   746,    34,   532,     5,   856,   852,    10,
     659,   661,   857,   533,    36,  1169,   536,     5,  1172,   675,
    1028,   670,   540,  1030,   677,  1031,   317,   411,   673,  1032,
     244,     5,  1271,   332,  1151,  1308,  1299,   553,  1152,  1153,
    1154,   242,  1180,   557,     5,   833,   520,  1298,   411,  1299,
     243,   506,   337,   739,   642,   644,   741,   510,  1156,   -30,
      31,    32,  1070,  1071,  1072,  1073,   660,   662,   680,  1193,
    1310,  1310,  1292,  1317,     5,   868,   222,  1298,   592,  1299,
    1200,  1201,   596,   521,   600,   602,   131,   221,   222,   434,
     223,   224,   225,   226,   227,  1406,  1156,   690,   691,   735,
     451,     5,   639,     5,     7,   638,   646,   856,  1299,   645,
     688,   648,   857,     5,   650,   651,   206,   207,   658,   663,
     692,   693,   438,  1102,  1104,  1105,  1106,  1123,   573,   669,
     574,   413,   604,   836,  1376,   672,   609,  1379,   674,  1143,
     507,   676,  1381,   221,    10,  1384,   223,   224,   225,  1385,
     511,   -51,   682,   574,     5,   789,   654,   414,   729,   837,
     838,   839,   178,   688,  1255,   244,  1262,  1291,   691,    17,
    1158,   320,   321,   875,  1294,  1170,   680,   878,   423,   277,
    1176,   147,  1405,   149,  1086,   151,   745,   153,  1407,   731,
     692,   693,   573,     5,   712,    31,    32,  1181,  1182,  1183,
     712,   491,   688,   787,   712,   689,   690,   691,   870,  1314,
    1314,   689,  1319,  1323,  1086,  1328,   574,  1333,  1333,   907,
    1337,   693,  1151,   226,   227,   765,  1152,  1153,  1154,   692,
     693,   694,     5,   614,   615,   843,   928,  1299,   899,   239,
     423,   902,  1151,   408,  1046,   960,  1152,  1153,  1154,   956,
    1270,   786,     5,   450,   692,   693,  1308,  1299,  1332,  1332,
    1267,   883,   411,   463,   464,   886,   680,   878,   887,   799,
     598,   802,   477,  1377,   805,  1025,   808,  1109,   811,   484,
     814,  1382,   487,   816,  1156,   490,  1055,  1056,   980,   981,
    1276,   820,  1192,   500,   823,  1387,   825,  1196,   789,  1388,
     668,   828,  1171,   138,  1156,  1334,   143,   465,   146,  1315,
     148,   834,   150,   465,   152,     5,   821,   822,  1149,  1150,
    1177,     5,   935,  1327,   688,    43,   278,   689,   690,   691,
     688,   853,   608,   689,   690,   691,   719,   742,  1046,   789,
     909,   720,   719,   919,  1020,   158,   159,   720,   160,   161,
    1010,   692,   693,   694,  1011,  1012,  1013,   692,   693,   694,
     162,   271,   272,   847,   846,   491,  1008,   850,   890,   163,
     552,   895,   164,   667,   789,  1041,  1264,   165,   858,   855,
     905,     5,   864,   166,   167,   496,   663,   168,   261,   663,
     688,   262,   683,   221,   690,   691,   223,   224,   225,   226,
     227,   787,   684,   788,   685,  1033,  1063,  1035,   884,   680,
     888,   686,   169,   888,   882,   170,   888,   692,   693,   888,
     171,  1004,   888,   172,   411,   173,   131,   221,   712,   946,
     223,   224,   225,   174,     1,     2,   920,    97,   221,   222,
       3,   223,   224,   225,   817,     4,   396,     5,     6,   100,
    1097,   452,   454,   400,     7,   457,   458,     8,   403,  1389,
     103,   992,  1125,     0,     9,    10,     0,    11,   476,    12,
       0,     0,     0,     0,    13,   483,    14,     0,   486,    15,
       0,   489,    16,   984,     0,   680,     0,     0,     0,   499,
      17,     0,     0,    18,     0,    19,    20,    21,    22,     0,
       0,     0,     0,     0,  1107,     0,    23,    24,     0,    25,
      26,    27,   853,    28,    29,    30,    31,    32,    33,     0,
      34,    35,    36,  1151,  1086,     0,     5,  1152,  1153,  1154,
    1155,     0,     0,     5,     0,   688,  1004,     0,   689,   690,
     691,     0,   688,     0,  1038,   689,   690,   691,     3,     0,
       0,  1000,   720,  1043,   731,     0,     6,     0,     0,     0,
       0,     0,   692,   693,   694,     8,     0,     0,     0,   692,
     693,   694,     9,     0,  1184,    11,   888,     0,   888,     0,
       0,     0,   888,     0,  1060,  1156,     0,     0,     0,     0,
      16,   465,   712,     0,     0,     0,     0,     0,     0,     5,
       0,    18,     0,     0,    20,  1157,    22,     0,   688,     0,
       0,   689,   690,   691,  1004,  1083,     0,    25,     5,    27,
     719,    28,     0,    30,     0,     0,   853,   688,     0,    35,
     689,   690,   691,     0,     0,   692,   693,   694,     0,     0,
       0,     0,     0,     0,   720,     0,     0,     0,     0,   663,
       0,  1121,     0,     0,   692,   693,   694,  1129,   186,  1131,
     187,  1134,   188,   189,     0,   190,     0,   191,   192,     0,
     193,   194,   195,   197,   195,   199,   195,   201,   202,   888,
     204,     0,   205,  1004,     0,     0,     0,     0,  1146,     0,
       0,     0,     0,     0,   712,  1167,     0,  1167,     0,     0,
    1167,     0,   209,     0,   210,   213,   216,     5,   219,     0,
       0,     0,   853,     0,     0,     0,   688,     0,     0,   689,
     690,   691,     0,     0,     0,     0,  1175,     0,     0,     0,
       0,  1190,     0,  1191,     0,  1380,     0,   712,     0,     0,
       0,     0,   712,   692,   693,   694,     0,   712,  1202,     0,
       0,     0,     0,  1004,     0,     0,   853,   853,     0,     0,
       0,     0,     0,   341,     0,   343,     0,     0,  1265,     0,
       0,     0,   348,     0,   350,  1378,     0,     0,   353,   354,
       0,     0,     0,  1383,   355,   356,   357,   358,     0,   359,
     195,   361,   221,   222,     0,   223,   224,   225,   226,   227,
     364,     0,   788,     0,     0,   368,   369,     0,   370,   712,
       0,     0,     0,     0,   374,     0,     0,   712,   376,   377,
       0,     0,     0,   379,   380,     0,     0,     0,   382,   383,
       0,     0,     0,     5,     0,     0,   388,     0,     0,     0,
     391,  1295,   688,   393,     0,   689,   690,   691,     0,     0,
     395,     0,   398,   399,   401,   402,   404,  1151,   405,     0,
       0,  1152,  1153,  1154,  1155,     0,     0,     5,     0,   692,
     693,   694,     0,  1345,     0,  1348,   688,     0,     0,   689,
     690,   691,  1353,     0,     0,     0,  1175,  1300,  1300,  1309,
    1309,     0,  1316,  1320,     0,  1325,  1300,  1330,  1330,     0,
    1335,     0,     0,   692,   693,   694,   221,     0,     0,   223,
     224,   225,   226,   227,     0,     0,   788,     0,     0,  1156,
     621,   622,   623,   624,   625,   626,   627,   628,   629,   630,
     631,   632,   633,   634,     0,     0,     0,     0,     0,  1157,
       0,     0,   516,   517,     0,     0,     0,     0,     0,   343,
     522,     0,   525,     0,     0,     0,   526,   528,   529,     0,
     530,   621,   622,   623,   624,   625,   626,   627,   628,   629,
       0,   535,     0,     0,   537,   538,     0,   539,     0,   541,
     542,     0,   544,   545,     0,   546,     0,   548,   549,   550,
       0,     0,   364,     0,     0,     0,     0,   555,   556,     0,
       0,   559,   560,     0,     0,     0,     0,     0,   565,   566,
       0,   568,   569,     0,   571,   572,     0,     0,     0,     0,
       0,     0,     3,     0,   582,   583,   584,     0,     0,     5,
       6,   589,     0,   590,     0,   593,     0,   594,   688,     8,
       0,   689,   690,   691,     0,     0,     9,   131,   221,   222,
       0,   223,   224,   225,   226,   227,   221,   222,    14,   223,
     224,   225,   226,   227,    16,   692,   693,   694,     0,     0,
       0,     0,     0,     0,     0,    18,     0,     0,    20,     0,
      22,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    25,     0,    27,     0,    28,     0,    30,     0,     0,
       0,     0,     0,    35,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     3,     0,   736,     0,     0,     0,     0,     0,     6,
     522,     0,     0,   522,     0,   526,     0,   743,     8,     0,
       0,     0,     0,     0,     0,     9,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,     0,
     748,   749,   750,    16,     0,   751,   752,   753,   755,   753,
       0,     0,   758,     0,    18,     0,     0,    20,     0,    22,
       0,     0,     0,     0,   767,   768,   770,   768,     0,   772,
      25,   773,    27,   775,    28,   777,    30,     0,     0,     0,
       0,     0,    35,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   797,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   798,     0,   800,     0,     0,   803,
       0,   806,     0,   809,     0,   812,     0,   812,     0,     0,
     584,     0,     0,   818,     0,     0,     0,     0,   590,   590,
       0,   594,     0,     0,     0,   826,     0,     0,     0,     0,
     829,  1212,  1213,  1214,  1215,  1216,  1217,  1218,  1219,  1220,
    1221,  1222,  1223,  1224,  1225,  1226,  1227,  1228,  1229,  1230,
    1231,  1232,  1233,  1234,  1235,  1236,  1237,  1238,  1239,  1240,
    1241,  1242,  1243,  1244,  1245,  1246,  1247,  1248,  1249,  1250,
    1251,  1256,  1213,  1214,  1215,  1216,  1217,  1218,  1219,  1257,
    1221,  1222,  1223,  1224,  1225,  1226,  1227,  1228,  1229,  1230,
    1231,  1232,  1233,  1234,  1235,  1236,  1237,  1238,  1239,  1240,
    1241,  1242,  1243,  1244,  1258,  1259,  1247,  1248,  1249,  1250,
    1251,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   743,     0,
     826,     0,     0,     0,     0,     0,     0,     0,   933,     0,
       0,   753,     0,     0,     0,   753,     0,   941,     0,   943,
     768,     0,     0,     0,     0,     0,   947,     0,     0,   768,
       0,     0,   951,   773,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   812,
       0,   963,     0,   812,     0,     0,     0,   966,   968,   812,
       0,     0,   972,   812,     0,   975,   812,     0,   977,     0,
       0,   978,     0,     0,     0,     0,   818,   818,   982,     0,
     594,     0,   986,   987,     0,     0,     0,     0,     0,     0,
       0,     0,   990,   991,  1213,  1214,  1215,  1216,  1217,  1218,
    1219,     0,  1221,  1222,  1223,  1224,  1225,  1226,  1227,  1228,
    1229,  1230,  1231,  1232,  1233,  1234,  1235,  1236,  1237,  1238,
    1239,  1240,  1241,  1242,  1243,  1244,     0,     0,  1247,  1248,
    1249,  1250,  1251,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1045,     0,     0,     0,     0,  1050,
    1051,  1052,     0,     0,     0,     0,   941,   941,     0,     0,
       0,     0,  1058,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1062,     0,     0,     0,     0,     0,     0,
    1066,  1067,  1068,     0,  1069,   812,   812,   812,     0,  1074,
       0,  1075,     0,     0,     0,   594,     0,  1080,     0,   826,
       0,     0,     0,  1084,     0,   291,     0,   296,     0,   301,
       0,     0,   313,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1127,     0,   594,     0,  1132,     0,     0,     0,
    1136,  1137,     0,     0,     0,  1140,  1141,     0,  1142,     0,
       0,   812,     0,  1144,     0,     0,     0,     0,     0,     0,
       0,     0,   826,     0,     0,     0,     0,  1148,     0,  1084,
    1084,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1127,   594,     0,  1194,  1195,     0,  1132,  1197,     0,     0,
       0,     0,  1199,   594,     0,  1203,     0,     0,     0,     0,
       0,     0,  1205,  1207,   407,     0,     0,     0,     0,     0,
       0,   422,     0,   428,   429,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1275,  1205,  1277,     0,   461,
       0,     0,     0,     0,   469,  1278,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1279,  1280,     0,     0,     0,     0,     0,     0,     0,  1282,
    1283,  1285,  1286,  1287,  1288,     0,     0,  1290,     0,   594,
       0,   826,     0,     0,     0,     0,   594,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     385,   386,     0,   389,   534,     0,     0,     0,  1343,     0,
    1346,     0,     0,     0,     0,   543,     0,  1351,     0,  1354,
       0,     0,     0,     0,  1358,     0,     0,     0,  1362,     0,
       0,     0,  1365,     0,   558,     0,     0,  1369,     0,     0,
     563,   564,     0,     0,   567,     0,     0,   570,     0,     0,
       0,     0,     0,     0,   579,     0,     0,   581,     0,     0,
       0,     0,   587,     0,   588,     0,     0,     0,     0,     0,
       0,     0,     0,   597,     0,     0,     0,     0,     0,  1207,
       0,     0,  1391,  1393,     0,     0,   611,  1397,     0,     0,
     618,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   657,     0,     0,     0,
       0,     0,     0,   665,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   716,     0,     0,     0,     0,     0,
     728,     0,     0,     0,   734,     0,     0,     0,   551,     0,
     737,     0,   738,   554,     0,     0,   740,     0,     0,     0,
     561,   562,   744,     0,     0,     0,     0,     0,     0,   747,
       0,     0,     0,     0,     0,     0,     0,     0,   580,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   757,     0,     0,   759,   760,   761,
     762,     0,     0,     0,     0,     0,     0,   766,     0,     0,
       0,     0,     0,     0,     0,     0,   774,     0,   776,     0,
       0,   779,     0,   781,     0,   783,     0,     0,     0,     0,
       0,   796,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   849,     0,   851,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   863,     0,
     865,     0,     0,     0,     0,     0,     0,   871,     0,   872,
       0,   873,     0,   874,     0,     0,   763,   764,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   778,     0,   780,     0,   782,     0,   784,
       0,     0,     0,     0,     0,     0,     0,     0,   916,     0,
       0,     0,     0,     0,     0,     0,   921,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   936,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     965,     0,     0,     0,     0,   971,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   979,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   988,     0,
       0,     0,     0,     0,     0,   989,     0,     0,   993,   994,
     995,   996,     0,     0,     0,     0,   998,     0,     0,   999,
       0,  1003,     0,  1006,     0,     0,     0,     0,     0,  1009,
       0,  1017,  1018,  1019,     0,     0,     0,     0,     0,     0,
    1023,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   930,   931,   932,   934,     0,     0,     0,   938,
     939,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     945,     0,     0,   948,   949,     0,     0,     0,     0,     0,
       0,   953,   954,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   961,     0,     0,     0,     0,     0,
       0,     0,     0,  1065,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1077,     0,
    1079,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1093,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1110,     0,     0,     0,     0,
    1120,     0,     0,     0,  1126,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1053,
       0,     0,  1054,     0,  1173,     0,     0,  1057,     0,     0,
    1059,     0,     0,     0,  1186,     0,  1188,     0,  1061,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1204,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1263,     0,     0,
       0,     0,     0,     0,     0,  1269,     0,  1273,  1274,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1135,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1281,     0,     0,     0,     0,     0,     0,     0,  1289,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1297,     0,     0,     0,     0,
       0,     0,     0,  1324,     0,     0,     0,     0,     0,     0,
       0,  1338,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1198,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1209,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1374,  1375,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1395,
       0,     0,  1399,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1293,     0,     0,     0,     0,  1409,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1340,     0,
       0,     0,  1341,  1342,     0,  1344,     0,  1347,     0,     0,
       0,  1349,  1350,     0,  1352,     0,  1355,     0,  1356,  1357,
       0,  1359,  1360,  1361,     0,  1363,     0,  1364,     0,  1366,
    1367,     0,  1368,     0,  1370,  1371,     0,  1372,     0,  1373,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1390,  1392,
       0,  1394,     0,  1396,  1398,     0,  1400,  1401,  1402,  1403,
    1404,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1408,     0,  1410
};

static const yytype_int16 yycheck[] =
{
       0,   220,     0,   337,    45,   228,   186,     0,   364,   235,
     213,    52,    53,   216,    55,   194,    57,   584,    59,   726,
      61,   128,   129,   590,    65,   933,    67,   424,    69,   424,
      71,   835,    73,   818,    75,   343,    77,   205,    79,   199,
     189,   401,   946,   201,   818,   194,    87,   517,   879,   197,
     578,   820,     0,    94,   823,    96,  1058,    98,   190,   100,
    1121,   102,   803,   104,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   130,   127,   128,   129,   644,   662,     0,   818,
     393,   369,   681,   678,     4,   803,   189,  1283,   806,   814,
     798,     4,   800,   986,   236,   803,     4,   149,   806,   151,
     356,   809,  1051,  1052,   812,   338,    19,    14,     6,  1062,
      17,    49,  1045,     9,    17,    28,    25,   704,    16,   706,
      17,   708,    17,   710,  1136,   712,    23,     6,     6,     7,
      40,    26,   186,   184,    12,     5,    56,     0,    17,    17,
      60,    14,    38,    56,    17,    15,  1060,    60,    51,    22,
      88,   341,     5,   347,   387,   213,   147,   941,   216,    54,
      80,   394,    15,    65,    84,    24,  1362,    80,    78,    78,
     163,    84,    80,   367,  1127,   408,    84,   406,     9,   368,
      39,  1022,    89,  1132,    82,   195,    17,   982,    90,    48,
     241,    61,   202,  1205,    40,   246,   913,   248,   915,    49,
     251,   252,   941,    82,    82,    61,   990,    38,    61,    62,
     528,    31,   963,   265,   753,   393,    89,   359,    77,   589,
     271,   272,  1063,    73,  1295,   276,   749,   750,   279,   768,
    1044,   163,  1146,    53,    54,   286,   194,   555,    88,   290,
     803,   818,     9,   806,   295,   963,   809,   163,   966,   300,
      17,   789,   775,   978,   305,  1148,   736,  1098,   966,   310,
     968,   312,    17,  1275,   972,   316,   502,    35,   556,   191,
     855,   584,   323,   529,  1207,   326,  1190,  1191,   846,   864,
     879,   876,   850,   334,     7,    64,   542,   341,    11,    12,
      13,    14,   346,   347,     3,    50,    51,   884,  1127,   532,
    1084,   888,    67,  1117,  1083,   892,   893,   894,  1137,  1278,
     897,   898,   366,   367,   901,  1066,    25,  1286,    86,    87,
     163,   375,    17,  1040,     7,     5,   376,    92,    11,   379,
      17,    26,   382,   387,    17,    15,   390,   681,   163,    26,
     348,   399,   575,   397,   402,   578,    69,   355,  1066,  1067,
     156,    32,    33,   586,   364,   161,   162,   163,   361,    65,
      55,  1069,   159,   160,   374,    71,    53,    54,   419,    67,
     421,   560,  1089,     7,    72,   426,     7,    11,   537,   538,
      11,    12,    13,   619,    90,   436,    17,     7,   758,    35,
     549,   550,    12,   444,    92,  1131,   447,    17,  1134,   569,
     963,   560,   453,   966,   572,   968,   584,   751,   566,   972,
      56,    17,  1207,    41,     7,    21,    22,   468,    11,    12,
      13,    19,  1140,   474,    17,   743,   833,    20,   772,    22,
      28,   398,   150,   605,   537,   538,   608,   404,    69,   150,
      86,    87,  1029,  1030,  1031,  1032,   549,   550,   681,  1166,
    1279,  1280,  1266,  1282,    17,   773,   149,    20,   509,    22,
    1177,  1178,   513,   868,   518,   519,   147,   148,   149,    57,
     151,   152,   153,   154,   155,  1393,    69,    30,    31,   596,
      48,    17,   536,    17,    24,   535,   540,     7,    22,   539,
      26,   541,    12,    17,   544,   545,   161,   162,   548,   553,
      53,    54,    16,  1066,  1067,  1068,  1069,  1084,   155,   559,
     157,    32,   522,   746,  1343,   565,   526,  1346,   568,  1106,
      25,   571,  1351,   148,    35,  1354,   151,   152,   153,  1358,
      47,   155,   582,   157,    17,   879,   546,    33,   592,   151,
     152,   153,   596,    26,  1195,    56,  1197,  1264,    31,    60,
    1127,    74,    75,   786,  1271,  1132,   789,   790,    62,    70,
    1137,   117,  1391,   119,     8,   121,   617,   123,  1397,    45,
      53,    54,   155,    17,   584,    86,    87,  1140,  1141,  1142,
     590,   814,    26,   156,   594,    29,    30,    31,   777,  1279,
    1280,    29,  1282,  1283,     8,  1285,   157,  1287,  1288,   147,
    1290,    54,     7,   154,   155,   656,    11,    12,    13,    53,
      54,    55,    17,    58,    59,   751,   829,    22,   808,    61,
      62,   811,     7,   337,   990,   880,    11,    12,    13,   877,
    1207,   679,    17,   352,    53,    54,    21,    22,  1287,  1288,
    1203,   798,   986,   362,   363,   799,   879,   880,   800,   700,
     516,   702,   371,  1343,   705,   963,   707,  1075,   709,   378,
     711,  1351,   381,   714,    69,   384,  1001,  1002,   911,   912,
    1211,   722,  1165,   392,   725,  1365,   727,  1171,  1022,  1369,
     557,   732,  1132,   111,    69,  1288,   114,     9,   116,  1280,
     118,   745,   120,     9,   122,    17,   723,   724,  1124,  1125,
    1137,    17,   844,  1285,    26,     0,   194,    29,    30,    31,
      26,   765,   525,    29,    30,    31,    38,   610,  1084,  1063,
      42,    43,    38,   825,   957,   126,   126,    43,   126,   126,
       7,    53,    54,    55,    11,    12,    13,    53,    54,    55,
     126,   192,   192,   753,   752,   978,   947,   755,   802,   126,
     467,   805,   126,   556,  1098,   984,  1199,   126,   768,   767,
     814,    17,   770,   126,   126,   388,   820,   126,   189,   823,
      26,   189,   583,   148,    30,    31,   151,   152,   153,   154,
     155,   156,   583,   158,   583,   975,  1022,   977,   798,  1022,
     800,   583,   126,   803,   797,   126,   806,    53,    54,   809,
     126,   943,   812,   126,  1148,   126,   147,   148,   818,   860,
     151,   152,   153,   126,     4,     5,   826,     0,   148,   149,
      10,   151,   152,   153,   715,    15,   325,    17,    18,     0,
    1063,   353,   354,   328,    24,   357,   358,    27,   330,  1374,
       0,   928,  1084,    -1,    34,    35,    -1,    37,   370,    39,
      -1,    -1,    -1,    -1,    44,   377,    46,    -1,   380,    49,
      -1,   383,    52,   914,    -1,  1098,    -1,    -1,    -1,   391,
      60,    -1,    -1,    63,    -1,    65,    66,    67,    68,    -1,
      -1,    -1,    -1,    -1,  1074,    -1,    76,    77,    -1,    79,
      80,    81,   946,    83,    84,    85,    86,    87,    88,    -1,
      90,    91,    92,     7,     8,    -1,    17,    11,    12,    13,
      14,    -1,    -1,    17,    -1,    26,  1058,    -1,    29,    30,
      31,    -1,    26,    -1,   978,    29,    30,    31,    10,    -1,
      -1,   941,    43,   987,    45,    -1,    18,    -1,    -1,    -1,
      -1,    -1,    53,    54,    55,    27,    -1,    -1,    -1,    53,
      54,    55,    34,    -1,  1144,    37,   966,    -1,   968,    -1,
      -1,    -1,   972,    -1,  1015,    69,    -1,    -1,    -1,    -1,
      52,     9,   982,    -1,    -1,    -1,    -1,    -1,    -1,    17,
      -1,    63,    -1,    -1,    66,    89,    68,    -1,    26,    -1,
      -1,    29,    30,    31,  1136,  1046,    -1,    79,    17,    81,
      38,    83,    -1,    85,    -1,    -1,  1060,    26,    -1,    91,
      29,    30,    31,    -1,    -1,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    -1,    -1,    -1,    -1,  1083,
      -1,  1082,    -1,    -1,    53,    54,    55,  1088,   133,  1090,
     135,  1092,   137,   138,    -1,   140,    -1,   142,   143,    -1,
     145,   146,   147,   148,   149,   150,   151,   152,   153,  1069,
     155,    -1,   157,  1205,    -1,    -1,    -1,    -1,  1119,    -1,
      -1,    -1,    -1,    -1,  1084,  1129,    -1,  1131,    -1,    -1,
    1134,    -1,   177,    -1,   179,   180,   181,    17,   183,    -1,
      -1,    -1,  1146,    -1,    -1,    -1,    26,    -1,    -1,    29,
      30,    31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,
      -1,  1162,    -1,  1164,    -1,  1348,    -1,  1127,    -1,    -1,
      -1,    -1,  1132,    53,    54,    55,    -1,  1137,  1179,    -1,
      -1,    -1,    -1,  1275,    -1,    -1,  1190,  1191,    -1,    -1,
      -1,    -1,    -1,   238,    -1,   240,    -1,    -1,  1202,    -1,
      -1,    -1,   247,    -1,   249,  1345,    -1,    -1,   253,   254,
      -1,    -1,    -1,  1353,   259,   260,   261,   262,    -1,   264,
     265,   266,   148,   149,    -1,   151,   152,   153,   154,   155,
     275,    -1,   158,    -1,    -1,   280,   281,    -1,   283,  1199,
      -1,    -1,    -1,    -1,   289,    -1,    -1,  1207,   293,   294,
      -1,    -1,    -1,   298,   299,    -1,    -1,    -1,   303,   304,
      -1,    -1,    -1,    17,    -1,    -1,   311,    -1,    -1,    -1,
     315,  1272,    26,   318,    -1,    29,    30,    31,    -1,    -1,
     325,    -1,   327,   328,   329,   330,   331,     7,   333,    -1,
      -1,    11,    12,    13,    14,    -1,    -1,    17,    -1,    53,
      54,    55,    -1,  1304,    -1,  1306,    26,    -1,    -1,    29,
      30,    31,  1313,    -1,    -1,    -1,    36,  1277,  1278,  1279,
    1280,    -1,  1282,  1283,    -1,  1285,  1286,  1287,  1288,    -1,
    1290,    -1,    -1,    53,    54,    55,   148,    -1,    -1,   151,
     152,   153,   154,   155,    -1,    -1,   158,    -1,    -1,    69,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,    -1,    -1,    -1,    -1,    -1,    89,
      -1,    -1,   417,   418,    -1,    -1,    -1,    -1,    -1,   424,
     425,    -1,   427,    -1,    -1,    -1,   431,   432,   433,    -1,
     435,    93,    94,    95,    96,    97,    98,    99,   100,   101,
      -1,   446,    -1,    -1,   449,   450,    -1,   452,    -1,   454,
     455,    -1,   457,   458,    -1,   460,    -1,   462,   463,   464,
      -1,    -1,   467,    -1,    -1,    -1,    -1,   472,   473,    -1,
      -1,   476,   477,    -1,    -1,    -1,    -1,    -1,   483,   484,
      -1,   486,   487,    -1,   489,   490,    -1,    -1,    -1,    -1,
      -1,    -1,    10,    -1,   499,   500,   501,    -1,    -1,    17,
      18,   506,    -1,   508,    -1,   510,    -1,   512,    26,    27,
      -1,    29,    30,    31,    -1,    -1,    34,   147,   148,   149,
      -1,   151,   152,   153,   154,   155,   148,   149,    46,   151,
     152,   153,   154,   155,    52,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    66,    -1,
      68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    79,    -1,    81,    -1,    83,    -1,    85,    -1,    -1,
      -1,    -1,    -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    10,    -1,   598,    -1,    -1,    -1,    -1,    -1,    18,
     605,    -1,    -1,   608,    -1,   610,    -1,   612,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,
     635,   636,   637,    52,    -1,   640,   641,   642,   643,   644,
      -1,    -1,   647,    -1,    63,    -1,    -1,    66,    -1,    68,
      -1,    -1,    -1,    -1,   659,   660,   661,   662,    -1,   664,
      79,   666,    81,   668,    83,   670,    85,    -1,    -1,    -1,
      -1,    -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   687,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   699,    -1,   701,    -1,    -1,   704,
      -1,   706,    -1,   708,    -1,   710,    -1,   712,    -1,    -1,
     715,    -1,    -1,   718,    -1,    -1,    -1,    -1,   723,   724,
      -1,   726,    -1,    -1,    -1,   730,    -1,    -1,    -1,    -1,
     735,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   833,    -1,
     835,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   843,    -1,
      -1,   846,    -1,    -1,    -1,   850,    -1,   852,    -1,   854,
     855,    -1,    -1,    -1,    -1,    -1,   861,    -1,    -1,   864,
      -1,    -1,   867,   868,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   884,
      -1,   886,    -1,   888,    -1,    -1,    -1,   892,   893,   894,
      -1,    -1,   897,   898,    -1,   900,   901,    -1,   903,    -1,
      -1,   906,    -1,    -1,    -1,    -1,   911,   912,   913,    -1,
     915,    -1,   917,   918,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   927,   928,   108,   109,   110,   111,   112,   113,
     114,    -1,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,    -1,    -1,   142,   143,
     144,   145,   146,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   989,    -1,    -1,    -1,    -1,   994,
     995,   996,    -1,    -1,    -1,    -1,  1001,  1002,    -1,    -1,
      -1,    -1,  1007,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1018,    -1,    -1,    -1,    -1,    -1,    -1,
    1025,  1026,  1027,    -1,  1029,  1030,  1031,  1032,    -1,  1034,
      -1,  1036,    -1,    -1,    -1,  1040,    -1,  1042,    -1,  1044,
      -1,    -1,    -1,  1048,    -1,   196,    -1,   198,    -1,   200,
      -1,    -1,   203,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1087,    -1,  1089,    -1,  1091,    -1,    -1,    -1,
    1095,  1096,    -1,    -1,    -1,  1100,  1101,    -1,  1103,    -1,
      -1,  1106,    -1,  1108,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1117,    -1,    -1,    -1,    -1,  1122,    -1,  1124,
    1125,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1165,  1166,    -1,  1168,  1169,    -1,  1171,  1172,    -1,    -1,
      -1,    -1,  1177,  1178,    -1,  1180,    -1,    -1,    -1,    -1,
      -1,    -1,  1187,  1188,   335,    -1,    -1,    -1,    -1,    -1,
      -1,   342,    -1,   344,   345,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1210,  1211,  1212,    -1,   360,
      -1,    -1,    -1,    -1,   365,  1220,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1245,  1246,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1254,
    1255,  1256,  1257,  1258,  1259,    -1,    -1,  1262,    -1,  1264,
      -1,  1266,    -1,    -1,    -1,    -1,  1271,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     308,   309,    -1,   311,   445,    -1,    -1,    -1,  1303,    -1,
    1305,    -1,    -1,    -1,    -1,   456,    -1,  1312,    -1,  1314,
      -1,    -1,    -1,    -1,  1319,    -1,    -1,    -1,  1323,    -1,
      -1,    -1,  1327,    -1,   475,    -1,    -1,  1332,    -1,    -1,
     481,   482,    -1,    -1,   485,    -1,    -1,   488,    -1,    -1,
      -1,    -1,    -1,    -1,   495,    -1,    -1,   498,    -1,    -1,
      -1,    -1,   503,    -1,   505,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   514,    -1,    -1,    -1,    -1,    -1,  1374,
      -1,    -1,  1377,  1378,    -1,    -1,   527,  1382,    -1,    -1,
     531,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   547,    -1,    -1,    -1,
      -1,    -1,    -1,   554,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   585,    -1,    -1,    -1,    -1,    -1,
     591,    -1,    -1,    -1,   595,    -1,    -1,    -1,   466,    -1,
     601,    -1,   603,   471,    -1,    -1,   607,    -1,    -1,    -1,
     478,   479,   613,    -1,    -1,    -1,    -1,    -1,    -1,   620,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   496,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   645,    -1,    -1,   648,   649,   650,
     651,    -1,    -1,    -1,    -1,    -1,    -1,   658,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   667,    -1,   669,    -1,
      -1,   672,    -1,   674,    -1,   676,    -1,    -1,    -1,    -1,
      -1,   682,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   754,    -1,   756,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   769,    -1,
     771,    -1,    -1,    -1,    -1,    -1,    -1,   778,    -1,   780,
      -1,   782,    -1,   784,    -1,    -1,   654,   655,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   671,    -1,   673,    -1,   675,    -1,   677,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   819,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   827,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   845,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     891,    -1,    -1,    -1,    -1,   896,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   908,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   919,    -1,
      -1,    -1,    -1,    -1,    -1,   926,    -1,    -1,   929,   930,
     931,   932,    -1,    -1,    -1,    -1,   937,    -1,    -1,   940,
      -1,   942,    -1,   944,    -1,    -1,    -1,    -1,    -1,   950,
      -1,   952,   953,   954,    -1,    -1,    -1,    -1,    -1,    -1,
     961,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   840,   841,   842,   843,    -1,    -1,    -1,   847,
     848,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     858,    -1,    -1,   861,   862,    -1,    -1,    -1,    -1,    -1,
      -1,   869,   870,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   882,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1024,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1039,    -1,
    1041,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1057,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1076,    -1,    -1,    -1,    -1,
    1081,    -1,    -1,    -1,  1085,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   997,
      -1,    -1,  1000,    -1,  1135,    -1,    -1,  1005,    -1,    -1,
    1008,    -1,    -1,    -1,  1145,    -1,  1147,    -1,  1016,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1185,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1198,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1206,    -1,  1208,  1209,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1094,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1252,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1260,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1276,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1284,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1292,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1174,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1189,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1339,  1340,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1380,
      -1,    -1,  1383,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1268,    -1,    -1,    -1,    -1,  1406,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1296,    -1,
      -1,    -1,  1300,  1301,    -1,  1303,    -1,  1305,    -1,    -1,
      -1,  1309,  1310,    -1,  1312,    -1,  1314,    -1,  1316,  1317,
      -1,  1319,  1320,  1321,    -1,  1323,    -1,  1325,    -1,  1327,
    1328,    -1,  1330,    -1,  1332,  1333,    -1,  1335,    -1,  1337,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1376,  1377,
      -1,  1379,    -1,  1381,  1382,    -1,  1384,  1385,  1386,  1387,
    1388,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1405,    -1,  1407
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     4,     5,    10,    15,    17,    18,    24,    27,    34,
      35,    37,    39,    44,    46,    49,    52,    60,    63,    65,
      66,    67,    68,    76,    77,    79,    80,    81,    83,    84,
      85,    86,    87,    88,    90,    91,    92,   165,   166,   167,
     238,   240,   304,   305,   308,   310,   331,   332,   333,   334,
     335,   336,   339,   340,   342,   344,   352,   353,   359,   360,
     371,   373,   384,   385,   386,   393,   394,   395,   396,   397,
     398,   399,   400,   401,   402,   403,   404,   405,   406,   408,
     429,   430,   431,   433,   435,   436,   442,   443,   448,   453,
     456,   459,   462,   463,   466,   471,   473,   476,   480,   493,
     497,   498,   503,   505,   506,     0,   159,   160,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   147,   205,   241,   205,   311,   205,   338,   338,   205,
     345,   205,   354,   338,   205,   374,   338,   354,   338,   354,
     338,   354,   338,   354,   205,   444,   205,   467,   333,   334,
     335,   336,   359,   385,   386,   396,   400,   404,   429,   442,
     448,   453,   456,   459,   462,   472,   474,   475,   205,   481,
     481,   481,    40,   485,   486,   163,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   355,   204,   355,   204,
     355,   204,   204,   407,   204,   204,   161,   162,   203,   204,
     204,   477,   478,   204,   494,   495,   204,   499,   500,   204,
     202,   148,   149,   151,   152,   153,   154,   155,   168,   169,
     170,   172,   174,   175,   178,   180,   181,   205,   242,    61,
     312,   314,    19,    28,    56,   305,   310,   321,   322,   341,
     352,   367,   368,   432,   434,   437,   439,   440,   441,   321,
     341,   432,   434,   172,   346,   342,   361,   362,   364,   365,
     366,   367,   368,     3,    25,   375,   376,    70,   308,   310,
     321,   387,   394,   449,   450,   451,   452,    23,   238,   356,
     357,   203,    71,   398,   454,   455,   203,    72,   402,   457,
     458,   203,    73,   406,   460,   461,    50,    51,   238,   409,
     410,   412,   413,   203,    64,   445,   446,   463,   464,   505,
      74,    75,   468,   469,    78,   482,   483,   485,   482,   485,
     482,   485,    41,   487,   488,   167,   180,   150,   168,   179,
     172,   204,   239,   204,   306,   309,   202,   202,   204,   202,
     204,   202,   202,   204,   204,   204,   204,   204,   204,   204,
     355,   204,   202,   202,   204,   372,   202,   202,   204,   204,
     204,   202,   204,   259,   204,   202,   204,   204,   202,   204,
     204,   202,   204,   204,   202,   259,   259,   202,   204,   259,
     202,   204,   202,   204,   202,   204,   478,   202,   204,   204,
     495,   204,   204,   500,   204,   204,   202,   203,   173,   176,
     177,   181,   180,    32,    33,   169,   205,   243,   244,   245,
     247,   248,   203,    62,   312,   317,   318,   342,   203,   203,
     205,   315,   311,   352,    57,   323,   324,     6,    16,    82,
     268,   269,   271,   272,   274,   337,   348,   349,   205,   369,
     369,    48,   415,   417,   415,   352,   337,   415,   415,   172,
     347,   203,   371,   369,   369,     9,   238,   378,   380,   203,
     205,   377,   311,   394,   274,   388,   415,   369,   238,   238,
     205,   358,   268,   415,   369,   268,   415,   369,   268,   415,
     369,   180,   184,   186,   205,   411,   409,   205,   414,   415,
     369,   464,   180,   470,   205,   484,   487,    25,   419,   420,
     487,    47,   507,   512,   167,   180,   204,   204,   202,   202,
     306,   309,   204,   307,   202,   204,   204,   313,   204,   204,
     204,   326,   202,   202,   203,   204,   202,   204,   204,   204,
     202,   204,   204,   203,   204,   204,   204,   343,   204,   204,
     204,   259,   372,   202,   259,   204,   204,   202,   203,   204,
     204,   259,   259,   203,   203,   204,   204,   203,   204,   204,
     203,   204,   204,   155,   157,   183,   185,   189,   192,   203,
     259,   203,   204,   204,   204,   465,   168,   203,   203,   204,
     204,   496,   202,   204,   204,   207,   202,   203,   244,   247,
     205,   246,   205,   249,   238,   320,   169,   319,   317,   238,
     316,   203,   312,   337,    58,    59,   327,   329,   203,   180,
     325,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   275,   276,   277,   268,   205,
     350,   321,   341,   321,   341,   268,   205,   418,   268,   337,
     268,   268,     7,    11,   238,   260,   264,   203,   268,   321,
     341,   321,   341,   205,   381,   203,   312,   388,   275,   268,
     321,   394,   268,   398,   268,   402,   268,   406,   180,   187,
     180,   193,   268,   437,   439,   440,   441,   447,    26,    29,
      30,    31,    53,    54,    55,   208,   210,   211,   212,   213,
     214,   215,   216,   219,   220,   221,   223,   224,   229,   231,
     234,   235,   238,   250,   251,   464,   203,   180,   419,    38,
      43,   208,   380,   422,   426,   427,   491,   492,   203,   205,
     421,    45,   504,   208,   203,   481,   204,   203,   203,   307,
     203,   307,   313,   204,   203,   202,   168,   203,   204,   204,
     204,   204,   204,   204,   438,   204,   438,   203,   204,   203,
     203,   203,   203,   259,   259,   202,   203,   204,   204,   363,
     204,   363,   204,   204,   203,   204,   203,   204,   259,   203,
     259,   203,   259,   203,   259,   185,   183,   156,   158,   181,
     182,   190,   193,   198,   199,   200,   203,   204,   204,   202,
     204,   209,   202,   204,   209,   202,   204,   209,   202,   204,
     209,   202,   204,   209,   202,   209,   202,   465,   204,   479,
     202,   496,   496,   202,   207,   202,   204,   328,   202,   204,
     508,   509,   247,   312,   205,   330,   180,   151,   152,   153,
     278,   278,   278,   171,   173,   351,   352,   238,   348,   203,
     352,   203,   419,   205,   266,   352,     7,    12,   238,   258,
     263,   348,   370,   203,   352,   203,   171,   382,   312,   278,
     394,   203,   203,   203,   203,   180,   188,   193,   180,   194,
     182,   201,   371,   212,   238,   250,   215,   219,   238,   250,
     205,   217,   223,   229,   234,   205,   222,   229,   234,   169,
     225,   234,   169,   232,   184,   205,   236,   147,   206,    42,
     208,   422,   426,   489,   490,   491,   203,   381,   381,   319,
     238,   203,   371,   415,   476,   497,   501,   419,   482,   328,
     259,   259,   259,   204,   259,   172,   203,   438,   259,   259,
     438,   204,   416,   204,   363,   259,   202,   204,   259,   259,
     363,   204,   379,   259,   259,   185,   201,   182,   191,   198,
     194,   259,   209,   204,   209,   203,   204,   209,   204,   209,
     209,   203,   204,   209,   209,   204,   209,   204,   204,   203,
     479,   479,   204,   207,   202,   207,   204,   204,   203,   203,
     204,   204,   509,   203,   203,   203,   203,   258,   203,   203,
     238,   422,   426,   203,   172,   267,   203,   266,   370,   203,
       7,    11,    12,    13,   256,   257,   383,   203,   203,   203,
     180,   195,   196,   203,   218,   220,   223,   229,   234,   229,
     234,   234,   234,   169,   226,   169,   233,   184,   205,   237,
     491,   167,   382,   205,   428,   204,   380,   426,   510,   513,
     204,   204,   204,   259,   259,   416,   416,   259,   204,   259,
     202,   259,   204,   168,   198,   203,   204,   204,   204,   204,
     209,   209,   209,   209,   204,   204,   230,   203,   207,   203,
     204,   328,   504,   202,   204,   511,     8,   279,   282,   280,
     282,   281,   282,   203,   267,   266,   279,   180,   197,   198,
     223,   229,   234,   229,   234,   234,   234,   169,   227,   260,
     203,     7,    11,    12,    13,    14,    69,   423,   424,   425,
     203,   202,   381,   208,   426,   513,   203,   204,   270,   202,
     207,   202,   204,   273,   202,   259,   204,   204,   389,   198,
     204,   204,   204,   209,   204,   328,   202,   501,   204,   511,
     511,     7,    11,    12,    13,    14,    69,    89,   208,   252,
     253,   254,   255,   261,   265,   279,   303,   205,   283,   283,
     208,   281,   283,   203,   267,    36,   208,   303,   390,   391,
     229,   234,   234,   234,   169,   228,   203,   266,   203,   382,
     202,   202,   270,   207,   204,   204,   273,   204,   259,   204,
     207,   207,   202,   204,   203,   204,   262,   204,   502,   259,
     266,   266,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   284,   287,   296,   296,   107,   115,   140,   141,
     289,   292,   296,   203,   390,   205,   392,   234,   267,   203,
     208,   491,   504,   203,   203,   204,   262,   204,   204,   204,
     204,   203,   204,   204,   294,   204,   204,   204,   204,   203,
     204,   207,   328,   259,   207,   202,   267,   203,    20,    22,
     238,   253,   285,   297,   298,   301,   302,   285,    21,   238,
     253,   286,   299,   300,   301,   286,   238,   253,   288,   301,
     238,   261,   295,   301,   203,   238,   290,   297,   301,   285,
     238,   291,   299,   301,   291,   238,   293,   301,   203,   501,
     259,   259,   259,   204,   259,   202,   204,   259,   202,   259,
     259,   204,   259,   202,   204,   259,   259,   259,   204,   259,
     259,   259,   204,   259,   259,   204,   259,   259,   259,   204,
     259,   259,   259,   259,   203,   203,   253,   301,   169,   253,
     180,   253,   301,   169,   253,   253,   261,   301,   301,   502,
     259,   204,   259,   204,   259,   203,   259,   204,   259,   203,
     259,   259,   259,   259,   259,   253,   258,   253,   259,   203,
     259
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
     265,   265,   266,   267,   268,   268,   268,   269,   270,   270,
     270,   270,   271,   272,   273,   273,   273,   274,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   276,   276,   276,
     277,   277,   278,   278,   278,   279,   280,   281,   282,   283,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   285,
     285,   285,   285,   285,   285,   285,   285,   286,   286,   286,
     286,   286,   286,   286,   286,   287,   287,   288,   288,   288,
     288,   289,   289,   289,   289,   289,   289,   289,   289,   289,
     290,   290,   290,   290,   291,   291,   291,   291,   292,   292,
     293,   293,   294,   294,   295,   295,   295,   295,   296,   296,
     296,   296,   296,   296,   296,   296,   296,   296,   296,   296,
     296,   296,   296,   296,   296,   296,   296,   296,   296,   296,
     296,   296,   296,   296,   296,   296,   296,   296,   296,   296,
     296,   296,   296,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   304,   305,   306,   306,   307,   307,   308,   309,
     309,   310,   311,   312,   313,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   325,   325,
     326,   326,   327,   328,   328,   329,   329,   330,   331,   331,
     331,   332,   332,   333,   334,   335,   336,   337,   337,   338,
     339,   339,   340,   340,   341,   341,   342,   343,   343,   343,
     344,   344,   345,   346,   347,   348,   349,   349,   350,   351,
     351,   352,   353,   353,   353,   354,   355,   355,   355,   355,
     356,   357,   358,   359,   360,   360,   361,   361,   361,   361,
     362,   363,   363,   363,   363,   363,   364,   365,   366,   367,
     368,   369,   370,   371,   372,   372,   372,   373,   374,   375,
     376,   376,   377,   378,   379,   379,   380,   381,   382,   383,
     384,   384,   385,   386,   387,   387,   388,   389,   389,   389,
     389,   389,   390,   391,   392,   393,   393,   394,   395,   395,
     395,   396,   397,   397,   398,   399,   399,   400,   401,   402,
     403,   403,   404,   405,   406,   407,   407,   407,   407,   407,
     408,   408,   409,   410,   411,   411,   412,   413,   414,   415,
     416,   416,   416,   416,   417,   418,   419,   420,   421,   422,
     423,   424,   425,   425,   425,   425,   425,   425,   426,   427,
     428,   429,   429,   429,   430,   430,   431,   432,   432,   433,
     434,   434,   435,   436,   437,   438,   438,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   447,   447,   447,
     448,   449,   449,   450,   451,   452,   453,   454,   455,   456,
     457,   458,   459,   460,   461,   462,   463,   463,   463,   463,
     463,   463,   463,   463,   463,   463,   463,   463,   464,   464,
     465,   465,   465,   466,   467,   468,   469,   469,   470,   470,
     470,   471,   472,   472,   473,   474,   474,   474,   474,   474,
     474,   474,   474,   474,   474,   474,   474,   474,   474,   475,
     475,   475,   475,   475,   475,   475,   476,   477,   477,   478,
     479,   479,   479,   479,   479,   479,   479,   480,   481,   482,
     483,   484,   485,   486,   487,   488,   489,   490,   491,   492,
     493,   494,   494,   495,   496,   496,   496,   496,   496,   497,
     498,   499,   499,   500,   501,   501,   501,   501,   502,   502,
     502,   502,   503,   504,   505,   506,   507,   508,   508,   509,
     510,   510,   511,   511,   511,   511,   512,   513
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
       7,     0,     3,     5,     3,     3,     9,     7,     9,     1,
       1,     1,     1,     7,     0,     3,     3,     1,     1,     5,
       1,     1,     1,     7,     0,     3,     1,     1,     1,     1,
       1,     1,     8,    10,     1,     1,    10,     0,     3,     5,
       3,     2,     5,     1,     1,     1,     1,     5,     1,     1,
       1,     8,     1,     1,     5,     1,     1,     8,     1,     5,
       1,     1,     8,     1,     5,     0,     3,     5,     3,     3,
       1,     1,     4,     1,     1,     1,     4,     1,     1,     7,
       0,     3,     3,     3,     1,     1,     5,     1,     1,     9,
       1,     5,     1,     1,     1,     1,     1,     1,     7,     1,
       1,     1,     1,     1,     1,     1,    10,     1,     1,    10,
       1,     1,    10,    10,     7,     0,     3,     3,     9,     7,
       9,    10,     1,     1,     9,     1,     1,     1,     1,     1,
      10,     1,     1,     7,     9,     1,    10,     7,     1,    10,
       7,     1,    10,     7,     1,     9,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     3,     2,     1,     1,     4,     1,     1,     1,     2,
       3,     4,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,     3,     1,     8,
       0,     3,     3,     3,     5,     3,     2,     1,     1,     4,
       1,     1,     4,     1,     4,     1,     4,     1,     4,     1,
       4,     3,     1,     6,     0,     3,     3,     3,     2,     1,
       4,     3,     1,    16,     1,     1,     1,     1,     0,     6,
       3,     2,     1,     1,     9,     1,     4,     3,     1,     6,
       1,     1,     0,     3,     3,     2,     1,     7
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
