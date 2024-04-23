/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

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

/* First part of user prologue.  */

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



# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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
    T_AXISMINVALUE = 335,
    T_AXISMAXVALUE = 336,
    T_RANGEMEANING = 337,
    T_exact = 338,
    T_wraparound = 339,
    T_GEODETICCRS = 340,
    T_GEODETICDATUM = 341,
    T_PROJECTEDCRS = 342,
    T_PRIMEMERIDIAN = 343,
    T_GEOGRAPHICCRS = 344,
    T_TRF = 345,
    T_VERTICALCRS = 346,
    T_VERTICALDATUM = 347,
    T_VRF = 348,
    T_TIMEDATUM = 349,
    T_TEMPORALQUANTITY = 350,
    T_ENGINEERINGDATUM = 351,
    T_ENGINEERINGCRS = 352,
    T_PARAMETRICDATUM = 353,
    T_AFFINE = 354,
    T_CARTESIAN = 355,
    T_CYLINDRICAL = 356,
    T_ELLIPSOIDAL = 357,
    T_LINEAR = 358,
    T_PARAMETRIC = 359,
    T_POLAR = 360,
    T_SPHERICAL = 361,
    T_VERTICAL = 362,
    T_TEMPORAL = 363,
    T_TEMPORALCOUNT = 364,
    T_TEMPORALMEASURE = 365,
    T_ORDINAL = 366,
    T_TEMPORALDATETIME = 367,
    T_NORTH = 368,
    T_NORTHNORTHEAST = 369,
    T_NORTHEAST = 370,
    T_EASTNORTHEAST = 371,
    T_EAST = 372,
    T_EASTSOUTHEAST = 373,
    T_SOUTHEAST = 374,
    T_SOUTHSOUTHEAST = 375,
    T_SOUTH = 376,
    T_SOUTHSOUTHWEST = 377,
    T_SOUTHWEST = 378,
    T_WESTSOUTHWEST = 379,
    T_WEST = 380,
    T_WESTNORTHWEST = 381,
    T_NORTHWEST = 382,
    T_NORTHNORTHWEST = 383,
    T_UP = 384,
    T_DOWN = 385,
    T_GEOCENTRICX = 386,
    T_GEOCENTRICY = 387,
    T_GEOCENTRICZ = 388,
    T_COLUMNPOSITIVE = 389,
    T_COLUMNNEGATIVE = 390,
    T_ROWPOSITIVE = 391,
    T_ROWNEGATIVE = 392,
    T_DISPLAYRIGHT = 393,
    T_DISPLAYLEFT = 394,
    T_DISPLAYUP = 395,
    T_DISPLAYDOWN = 396,
    T_FORWARD = 397,
    T_AFT = 398,
    T_PORT = 399,
    T_STARBOARD = 400,
    T_CLOCKWISE = 401,
    T_COUNTERCLOCKWISE = 402,
    T_TOWARDS = 403,
    T_AWAYFROM = 404,
    T_FUTURE = 405,
    T_PAST = 406,
    T_UNSPECIFIED = 407,
    T_STRING = 408,
    T_UNSIGNED_INTEGER_DIFFERENT_ONE_TWO_THREE = 409
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



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

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
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
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
#define YYLAST   3231

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  170
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  368
/* YYNRULES -- Number of rules.  */
#define YYNRULES  735
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1501

#define YYUNDEFTOK  2
#define YYMAXUTOK   409


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     166,   168,     2,   160,   169,   161,   155,     2,     2,   157,
     158,   159,     2,     2,     2,     2,     2,     2,   162,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   156,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   163,     2,     2,     2,     2,     2,
     164,   165,     2,   167,     2,     2,     2,     2,     2,     2,
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
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   213,   213,   213,   213,   213,   213,   213,   214,   214,
     214,   215,   218,   218,   219,   219,   219,   220,   222,   222,
     226,   230,   230,   232,   234,   236,   236,   238,   238,   240,
     242,   244,   246,   248,   248,   250,   250,   252,   252,   252,
     252,   254,   254,   258,   260,   264,   265,   266,   268,   268,
     270,   272,   274,   276,   280,   281,   284,   285,   287,   289,
     291,   294,   295,   296,   298,   300,   302,   302,   304,   307,
     308,   310,   310,   315,   315,   317,   317,   319,   321,   323,
     327,   328,   331,   332,   333,   335,   335,   336,   339,   340,
     344,   345,   346,   350,   351,   352,   353,   355,   359,   361,
     364,   366,   369,   370,   371,   372,   373,   374,   375,   376,
     377,   378,   379,   380,   381,   382,   383,   386,   387,   388,
     389,   390,   391,   392,   393,   394,   395,   396,   397,   398,
     399,   400,   404,   406,   408,   412,   417,   419,   421,   423,
     425,   429,   434,   435,   437,   439,   441,   445,   449,   451,
     451,   453,   453,   458,   463,   464,   465,   466,   467,   468,
     469,   471,   473,   475,   475,   477,   477,   479,   481,   483,
     485,   487,   489,   493,   495,   499,   499,   502,   505,   510,
     510,   510,   510,   510,   513,   518,   518,   518,   518,   521,
     525,   526,   528,   544,   548,   549,   551,   551,   553,   553,
     559,   559,   561,   563,   570,   570,   570,   570,   572,   579,
     580,   581,   582,   584,   591,   592,   593,   594,   596,   603,
     610,   611,   612,   614,   616,   616,   616,   616,   616,   616,
     616,   616,   616,   618,   618,   620,   620,   622,   622,   622,
     624,   629,   635,   640,   643,   646,   647,   648,   649,   650,
     651,   652,   653,   654,   657,   658,   659,   660,   661,   662,
     663,   664,   667,   668,   669,   670,   671,   672,   673,   674,
     677,   678,   681,   682,   683,   684,   685,   689,   690,   691,
     692,   693,   694,   695,   696,   697,   700,   701,   702,   703,
     706,   707,   708,   709,   712,   713,   716,   717,   718,   723,
     724,   727,   728,   729,   730,   731,   734,   735,   736,   737,
     738,   739,   740,   741,   742,   743,   744,   745,   746,   747,
     748,   749,   750,   751,   752,   753,   754,   755,   756,   757,
     758,   759,   760,   761,   762,   763,   764,   765,   766,   767,
     768,   769,   771,   774,   776,   778,   780,   782,   785,   786,
     787,   788,   790,   791,   792,   793,   794,   796,   797,   799,
     800,   802,   803,   804,   804,   806,   822,   822,   824,   832,
     833,   835,   836,   838,   846,   847,   849,   851,   853,   858,
     859,   861,   863,   865,   867,   869,   871,   873,   878,   882,
     884,   887,   890,   891,   892,   894,   895,   897,   902,   903,
     905,   905,   907,   911,   911,   911,   913,   913,   915,   923,
     932,   940,   950,   951,   953,   955,   955,   957,   957,   960,
     961,   965,   971,   972,   973,   975,   975,   977,   979,   981,
     985,   990,   990,   992,   995,   996,  1001,  1002,  1004,  1009,
    1009,  1009,  1011,  1013,  1014,  1015,  1016,  1017,  1018,  1019,
    1020,  1022,  1025,  1027,  1029,  1032,  1034,  1034,  1034,  1038,
    1044,  1044,  1048,  1048,  1049,  1049,  1051,  1056,  1057,  1058,
    1059,  1060,  1062,  1068,  1073,  1079,  1081,  1083,  1085,  1089,
    1095,  1096,  1097,  1099,  1101,  1103,  1107,  1107,  1109,  1111,
    1116,  1117,  1118,  1120,  1122,  1124,  1126,  1130,  1130,  1132,
    1138,  1145,  1145,  1148,  1155,  1156,  1157,  1158,  1159,  1161,
    1162,  1163,  1165,  1169,  1171,  1173,  1173,  1177,  1182,  1182,
    1182,  1186,  1191,  1191,  1193,  1197,  1197,  1199,  1200,  1201,
    1202,  1206,  1211,  1213,  1217,  1217,  1221,  1226,  1228,  1232,
    1233,  1234,  1235,  1236,  1238,  1238,  1240,  1243,  1245,  1245,
    1247,  1249,  1251,  1255,  1261,  1262,  1263,  1264,  1266,  1268,
    1272,  1277,  1279,  1282,  1288,  1289,  1290,  1292,  1296,  1302,
    1302,  1302,  1302,  1302,  1302,  1306,  1311,  1313,  1318,  1318,
    1319,  1321,  1321,  1323,  1330,  1330,  1332,  1339,  1339,  1341,
    1348,  1355,  1360,  1361,  1362,  1364,  1370,  1375,  1383,  1389,
    1391,  1393,  1404,  1405,  1406,  1408,  1410,  1410,  1411,  1411,
    1415,  1421,  1421,  1423,  1428,  1434,  1439,  1445,  1450,  1455,
    1461,  1466,  1471,  1477,  1482,  1487,  1493,  1493,  1494,  1494,
    1495,  1495,  1496,  1496,  1497,  1497,  1498,  1498,  1501,  1501,
    1503,  1504,  1505,  1507,  1509,  1513,  1516,  1516,  1519,  1520,
    1521,  1523,  1527,  1528,  1530,  1532,  1532,  1533,  1533,  1534,
    1534,  1534,  1535,  1536,  1536,  1537,  1537,  1538,  1538,  1540,
    1540,  1541,  1541,  1542,  1543,  1543,  1547,  1551,  1552,  1555,
    1560,  1561,  1562,  1563,  1564,  1565,  1566,  1568,  1570,  1572,
    1575,  1577,  1579,  1581,  1583,  1585,  1587,  1589,  1591,  1593,
    1598,  1602,  1603,  1606,  1611,  1612,  1613,  1614,  1615,  1617,
    1622,  1627,  1628,  1631,  1637,  1637,  1637,  1637,  1639,  1640,
    1641,  1642,  1644,  1646,  1651,  1657,  1659,  1664,  1665,  1668,
    1676,  1677,  1678,  1679,  1681,  1683
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
  "\"ORDER\"", "\"ANCHOR\"", "\"ANCHOREPOCH\"", "\"CONVERSION\"",
  "\"METHOD\"", "\"REMARK\"", "\"GEOGCRS\"", "\"BASEGEOGCRS\"",
  "\"SCOPE\"", "\"AREA\"", "\"BBOX\"", "\"CITATION\"", "\"URI\"",
  "\"VERTCRS\"", "\"VDATUM\"", "\"GEOIDMODEL\"", "\"COMPOUNDCRS\"",
  "\"PARAMETERFILE\"", "\"COORDINATEOPERATION\"", "\"SOURCECRS\"",
  "\"TARGETCRS\"", "\"INTERPOLATIONCRS\"", "\"OPERATIONACCURACY\"",
  "\"CONCATENATEDOPERATION\"", "\"STEP\"", "\"BOUNDCRS\"",
  "\"ABRIDGEDTRANSFORMATION\"", "\"DERIVINGCONVERSION\"", "\"TDATUM\"",
  "\"CALENDAR\"", "\"TIMEORIGIN\"", "\"TIMECRS\"", "\"VERTICALEXTENT\"",
  "\"TIMEEXTENT\"", "\"USAGE\"", "\"DYNAMIC\"", "\"FRAMEEPOCH\"",
  "\"MODEL\"", "\"VELOCITYGRID\"", "\"ENSEMBLE\"", "\"MEMBER\"",
  "\"ENSEMBLEACCURACY\"", "\"DERIVEDPROJCRS\"", "\"BASEPROJCRS\"",
  "\"EDATUM\"", "\"ENGCRS\"", "\"PDATUM\"", "\"PARAMETRICCRS\"",
  "\"PARAMETRICUNIT\"", "\"BASEVERTCRS\"", "\"BASEENGCRS\"",
  "\"BASEPARAMCRS\"", "\"BASETIMECRS\"", "\"EPOCH\"", "\"COORDEPOCH\"",
  "\"COORDINATEMETADATA\"", "\"POINTMOTIONOPERATION\"", "\"VERSION\"",
  "\"AXISMINVALUE\"", "\"AXISMAXVALUE\"", "\"RANGEMEANING\"", "\"exact\"",
  "\"wraparound\"", "\"GEODETICCRS\"", "\"GEODETICDATUM\"",
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
  "axis_range_opt_separator_identifier_list",
  "opt_separator_axis_range_opt_separator_identifier_list",
  "axis_minimum_value", "axis_minimum_value_keyword", "axis_maximum_value",
  "axis_maximum_value_keyword", "axis_range_meaning",
  "axis_range_meaning_keyword", "axis_range_meaning_value", "cs_unit",
  "datum_ensemble", "geodetic_datum_ensemble_without_pm",
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
  "opt_separator_datum_anchor_anchor_epoch_identifier_list",
  "datum_anchor", "datum_anchor_keyword", "datum_anchor_description",
  "datum_anchor_epoch", "datum_anchor_epoch_keyword", "anchor_epoch",
  "projected_crs", "projected_crs_keyword", "base_geodetic_crs",
  "base_static_geodetic_crs",
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
  "opt_geoid_model_id_list_opt_separator_scope_extent_identifier_remark",
  "geoid_model_id", "geoid_model_keyword", "geoid_model_name",
  "vertical_crs_keyword", "vertical_reference_frame",
  "vertical_reference_frame_keyword", "engineering_crs",
  "engineering_crs_keyword", "engineering_datum",
  "engineering_datum_keyword",
  "opt_separator_datum_anchor_identifier_list", "parametric_crs",
  "parametric_crs_keyword", "parametric_datum", "parametric_datum_keyword",
  "temporal_crs", "temporal_crs_keyword", "temporal_datum",
  "opt_separator_temporal_datum_end", "temporal_datum_keyword",
  "temporal_origin", "temporal_origin_keyword",
  "temporal_origin_description", "calendar", "calendar_keyword",
  "calendar_identifier", "deriving_conversion",
  "opt_separator_parameter_or_parameter_file_identifier_list",
  "deriving_conversion_keyword", "deriving_conversion_name",
  "operation_method", "operation_method_keyword", "operation_method_name",
  "operation_parameter", "opt_separator_parameter_unit_identifier_list",
  "parameter_unit", "length_or_angle_or_scale_or_time_or_parametric_unit",
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
  "derived_crs_name", "base_projected_crs",
  "base_projected_crs_opt_separator_cs_identifier",
  "base_projected_crs_keyword", "base_geodetic_geographic_crs",
  "derived_vertical_crs", "base_vertical_crs", "base_static_vertical_crs",
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
  "opt_end_abridged_coordinate_transformation",
  "abridged_transformation_keyword", "abridged_transformation_parameter", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
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
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,    46,    69,    49,    50,    51,
      43,    45,    58,    84,    90,    91,    40,    93,    41,    44
};
# endif

#define YYPACT_NINF (-1271)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-676)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    2601, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271,
   -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271,
   -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271,
   -1271, -1271, -1271, -1271, -1271, -1271, -1271,   107, -1271, -1271,
   -1271,   393, -1271, -1271, -1271,   393, -1271, -1271, -1271, -1271,
   -1271, -1271,   393,   393, -1271,   393, -1271,   -35,   393, -1271,
     393, -1271,   393, -1271, -1271, -1271,   393, -1271,   393, -1271,
     393, -1271,   393, -1271,   393, -1271,   393, -1271,   393, -1271,
     393, -1271, -1271, -1271, -1271, -1271, -1271, -1271,   393, -1271,
   -1271, -1271, -1271, -1271, -1271,   393, -1271,   393, -1271,   393,
   -1271,   393, -1271,   393, -1271,   393, -1271, -1271, -1271,    -6,
      -6,    -6,    -6,    -6, -1271,    81,    -6,    -6,    -6,    -6,
      -6,    -6,    -6,    -6,    -6,    -6,    -6,    -6,    -6,  1008,
      -6,    -6,    -6,   150, -1271, -1271,   -35, -1271,   -35, -1271,
     -35,   -35, -1271,   -35, -1271, -1271, -1271,   393, -1271,   -35,
     -35, -1271,   -35,   -35,   -35,   -35,   -35,   -35,   -35,   -35,
     -35, -1271,   -35, -1271,   -35, -1271, -1271, -1271, -1271,   -15,
   -1271, -1271, -1271, -1271, -1271,    37,    44,    76, -1271, -1271,
   -1271, -1271,   477, -1271,   -35, -1271,   -35,   -35,   -35, -1271,
     -35,   393,   771,   272,   109,   109,   453,    -6,   213,   466,
     336,   764,   455,   477,   259,   217,   477,   215,   477,    68,
     267,   477,   350,  1122, -1271, -1271, -1271,   590,   152, -1271,
   -1271,   152, -1271, -1271,   152, -1271, -1271,   310,  1008, -1271,
   -1271, -1271, -1271, -1271, -1271, -1271,   719, -1271, -1271, -1271,
   -1271,   276,   299,   269,   453, -1271,   -35, -1271,   -35,   393,
   -1271, -1271, -1271, -1271,   393,   -35,   393,   -35, -1271,   393,
     393,   -35,   -35, -1271, -1271, -1271, -1271,   -35,   -35,   -35,
     -35, -1271,   -35, -1271,   -35,   -35,   -35, -1271, -1271, -1271,
   -1271,   393,   393, -1271, -1271,   -35,   393, -1271, -1271,   393,
     -35,   -35, -1271,   -35, -1271, -1271,   393, -1271, -1271,   -35,
     -35,   393,   -35,   393, -1271, -1271,   -35,   -35,   393,   -35,
     -35, -1271, -1271,   -35,   -35,   393, -1271, -1271,   -35,   -35,
     393, -1271, -1271,   -35,   -35,   393,   -35,   393, -1271, -1271,
     -35,   393, -1271,   -35, -1271, -1271, -1271, -1271,   393, -1271,
     -35,   393,   -35,   -35,   -35,   -35,   -35, -1271,   -35,   393,
     477, -1271,   508,   719, -1271, -1271,   423,   477,   297,   477,
     477,    -6,    -6,    85,   448,   120,    -6,    -6,   492,   492,
      85,   120,   492,   492,   453,   508,   477,   523,    -6,    -6,
     256,   477,    -6,    -6,    86,   547,   492,    -6,   551, -1271,
     116,    -6,   551,   719,   547,   492,    -6, -1271,   551,   547,
     492,    -6,   547,   492,    -6, -1271, -1271,   555,   171, -1271,
      -6,   492,    -6,  1122,   719,   150, -1271,    -6,   310,   150,
   -1271,   560,   150, -1271,   310,   540,  1008, -1271,   719, -1271,
   -1271, -1271, -1271, -1271, -1271, -1271, -1271,   -35,   -35,   393,
   -1271,   393, -1271, -1271,   -35,   -35,   393,   -35, -1271, -1271,
   -1271,   -35,   -35,   -35, -1271,   -35,   393, -1271, -1271, -1271,
   -1271, -1271, -1271,   393,   477,   -35, -1271,   -35,   -35, -1271,
     -35,   393,   -35,   -35,   477,   -35,   -35, -1271,   -35,   -35,
     453,   477, -1271,   -35,   -35,   -35, -1271,   -35,   -35,   393,
   -1271, -1271,   -35,   -35,   -35,   393,   477,   -35,   -35,   -35,
     -35,   -35, -1271,   477,   -35,   269,   477,   477,   -35,   -35,
     -35,   477,   -35,   -35,   477,   -35,   -35, -1271, -1271,    64,
   -1271,   477,   -35, -1271,   477,   -35,   -35,   -35,   269,   477,
   -1271,   477,   -35, -1271,   -35,   393,   -35, -1271,   -35,   393,
     477, -1271,   563,   564,    -6,    -6, -1271, -1271,   551, -1271,
    1199,   537,   551,   477,   272,   120,   632,   477,   719,  1438,
   -1271,   547,    74,    74,   547,    -6,   547,   120, -1271,   547,
     547,   326,   477,   548, -1271, -1271, -1271,   547,    74,    74,
   -1271, -1271,    -6,   477,   272,   547,  1308, -1271,   547,    52,
   -1271, -1271,   551, -1271, -1271,   719, -1271, -1271,   547,   285,
   -1271, -1271,   547,   308, -1271,   547,    87, -1271, -1271,   719,
   -1271, -1271,   719, -1271, -1271, -1271,   547,   466,  1669,   477,
     719, -1271, -1271,   560,  1296,   477,    -6,   558,  1446,   477,
      -6, -1271,   -35, -1271, -1271,   477, -1271,   477, -1271,   -35,
   -1271,   477,   -35, -1271,   -35, -1271,   -35,   477, -1271, -1271,
   -1271,   393, -1271,   269,   477, -1271, -1271, -1271, -1271, -1271,
   -1271, -1271, -1271, -1271,   -35, -1271, -1271, -1271, -1271,   -35,
     -35,   -35, -1271,   -35,   -35,   -35,   -35,   477, -1271,   -35,
     477,   477,   477,   477, -1271, -1271,   -35,   -35,   393, -1271,
   -1271, -1271,   -35,   393,   477,   -35,   -35,   -35,   -35, -1271,
     -35, -1271,   -35,   477,   -35,   477,   -35,   -35,   -35, -1271,
     477,   -35,   477,   -35,   477,   -35,   118,   471, -1271,   339,
     477, -1271, -1271, -1271, -1271,   -35, -1271, -1271, -1271, -1271,
   -1271, -1271, -1271, -1271, -1271, -1271, -1271,   -35,   393,   -35,
     393, -1271,   -35,   393,   -35,   393,   -35,   393,   -35,   393,
     -35, -1271,   393,   -35, -1271, -1271,   -35, -1271, -1271, -1271,
     393,   -35,   -35,   393,   -35,   393, -1271, -1271,   -35, -1271,
     393, -1271, -1271,   -35,   564, -1271, -1271, -1271, -1271, -1271,
   -1271,   286, -1271,    -6,   719, -1271,   531,   531,   531,   531,
      85,    94,   477,    85,   477, -1271,   560, -1271, -1271, -1271,
   -1271, -1271, -1271,    -6, -1271,    -6, -1271,    85,   113,   477,
      85,   477,   508,   634, -1271,   531, -1271,    86,   477, -1271,
   -1271,   477, -1271,   477, -1271,   477, -1271,   719, -1271, -1271,
     719,   719, -1271,   452, -1271, -1271, -1271, -1271,   523,   112,
     586,   457, -1271,    -6,   432, -1271,    -6,   268, -1271,  1199,
     362, -1271,  1199,   490, -1271,   555, -1271,   489, -1271,  1263,
     477,    -6, -1271, -1271,    -6, -1271,  1199,   551,   477,   329,
      97, -1271, -1271, -1271,   -35, -1271,   -35, -1271, -1271, -1271,
   -1271,   -35,   -35,   -35,   -35,   -35,   -35,   -35, -1271,   -35,
   -1271,   -35, -1271,   -35,   -35,   -35,   -35, -1271,   -35,   -35,
   -1271,   -35, -1271, -1271,   -35,   -35,   -35,   -35, -1271, -1271,
   -1271, -1271, -1271,   495,   452, -1271,   339,   719, -1271,   -35,
   -1271,   -35, -1271,   -35, -1271,   -35, -1271, -1271,   477,   -35,
     -35,   -35, -1271,   477,   -35,   -35, -1271,   -35,   -35, -1271,
     -35, -1271, -1271,   -35, -1271,   477, -1271, -1271,   -35,   -35,
     -35,   393,   -35, -1271,   -35,   -35,   477, -1271, -1271, -1271,
   -1271, -1271, -1271,   477,   -35,   -35,   477,   477,   477,   477,
     477,   477, -1271, -1271,   477,   288,   477,   453,   453,   477,
   -1271,   548, -1271, -1271,   477,   454,   477,   477,   477, -1271,
   -1271,   719, -1271, -1271, -1271,  1349,   477, -1271,   367, -1271,
   -1271,   268, -1271,   362, -1271, -1271, -1271,   362, -1271, -1271,
    1199, -1271,  1199,   555, -1271, -1271, -1271,  1426, -1271,  1008,
   -1271,   508,    -6, -1271,   -35,  1307,   477,   560, -1271, -1271,
     -35,   -35,   -35,   -35, -1271, -1271,   -35,   -35,   -35, -1271,
   -1271,   -35,   -35, -1271,   -35, -1271, -1271, -1271, -1271, -1271,
     -35, -1271,   393,   -35, -1271,   -35, -1271, -1271, -1271,  1066,
   -1271, -1271,   393, -1271,   477,   -35,   -35,   -35, -1271,   -35,
     -35,   -35,   -35, -1271,   -35, -1271,   -35, -1271, -1271,   477,
     -35,   477,   -35, -1271,   -35,   558, -1271,   393,   -35,   -35,
   -1271,   654,   654,   654,   654, -1271, -1271, -1271,   477,   477,
   -1271, -1271,    -6, -1271,   654,   694, -1271,  1308, -1271,   387,
     651,   616,   362, -1271, -1271, -1271, -1271,  1199,   519,   477,
   -1271, -1271, -1271,   722,   477,   477,   393,    -6, -1271, -1271,
   -1271,   -35,   393,   -35,   393,   -35,   -35,   393, -1271, -1271,
     -35,   -35,   462,   694, -1271,   -35,   -35, -1271,   -35, -1271,
   -1271,   -35, -1271,   -35, -1271, -1271, -1271, -1271, -1271, -1271,
   -1271, -1271,   -35,   -35, -1271,   393, -1271, -1271,   329,   -35,
     539, -1271,    -6,   883, -1271,    -6, -1271,   730, -1271,    -6,
     453,  1053, -1271, -1271,   651,   616,   616, -1271,  1199, -1271,
   -1271,    -6,   477,   508, -1271, -1271, -1271, -1271, -1271, -1271,
   -1271, -1271, -1271, -1271, -1271,   393, -1271,   393,   -35, -1271,
     -35, -1271,   -35,   -35,   -35, -1271,   -35,   -35,   -35, -1271,
   -1271,   -35,   -35,   393,   -35, -1271, -1271, -1271, -1271,   477,
     -35,   -35,   -35,    -6,    -6,  1446,  2745, -1271, -1271,  1474,
   -1271,  2117,   477,  1437, -1271,  1437, -1271,    -6,   616, -1271,
     453,   477,  1071,   477,   477,   -35,   -35, -1271, -1271, -1271,
   -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271,
   -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271,
   -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271,
   -1271, -1271, -1271, -1271,   -35,   -35,   -35,   -35,   -35,   477,
   -1271,   -35,   -35,   -35,   -35,   -35,   477, -1271,   -35, -1271,
     -35, -1271,   -35, -1271,   -35, -1271,   -35, -1271, -1271,   -35,
     393, -1271, -1271,   453,   477,   292,   477,   520,   520,   606,
     606, -1271,   304,   619,   520,   604,   604, -1271,   141, -1271,
   -1271,   477, -1271, -1271,   329,   -35, -1271, -1271, -1271, -1271,
     -35,   -35, -1271,   -35,   393, -1271,   -35,   393,   -35,   393,
   -1271, -1271,   -35,   -35, -1271,   -35,   393,   -35, -1271, -1271,
     -35,   -35, -1271,   -35,   393,   -35, -1271,   -35,   -35, -1271,
     -35, -1271,   -35, -1271,   -35,   -35, -1271,   -35, -1271,   -35,
     -35, -1271,   -35, -1271,   -35, -1271, -1271,   477,   477, -1271,
     636, -1271,    70, -1271,   719,    75, -1271,  1199, -1271,  1199,
   -1271, -1271,   502, -1271,  1199,   532, -1271, -1271, -1271,   502,
   -1271,  1199,   532, -1271, -1271, -1271,   287, -1271, -1271,   406,
   -1271, -1271, -1271,   406, -1271, -1271, -1271, -1271,   -35, -1271,
     -35,   -35,   -35,   477,   -35,   477,   477,   -35,   -35,   -35,
     -35,   -35,   -35,   477,   -35,   -35,   -35,   -35, -1271,    75,
   -1271, -1271, -1271, -1271,    95, -1271, -1271, -1271, -1271,   532,
   -1271,   441, -1271, -1271,   532, -1271, -1271, -1271, -1271, -1271,
   -1271,   -35, -1271,   -35,   393,   -35,   477,   -35,    95, -1271,
   -1271,   647, -1271, -1271, -1271,   -35, -1271, -1271,   477, -1271,
   -1271
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,   439,   426,   415,   425,   161,   460,   483,   417,   515,
     518,   643,   687,   722,   725,   544,   537,   376,   599,   525,
     522,   534,   532,   654,   709,   416,   441,   461,   418,   440,
     516,   520,   519,   545,   526,   523,   535,     0,     4,     5,
       2,     0,    13,   366,   367,     0,   626,   405,   403,   404,
     406,   407,     0,     0,     3,     0,    12,   436,     0,   628,
       0,    11,     0,   630,   497,   498,     0,    14,     0,   632,
       0,    15,     0,   634,     0,    16,     0,   636,     0,    17,
       0,   627,   580,   578,   579,   581,   582,   629,     0,   631,
     633,   635,   637,    19,    18,     0,     7,     0,     8,     0,
       9,     0,    10,     0,     6,     0,     1,    73,    74,     0,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    78,   162,     0,   377,     0,   414,
       0,     0,   427,     0,   431,   432,   437,     0,   442,     0,
       0,   484,     0,     0,   443,     0,   527,     0,   527,     0,
     539,   600,     0,   644,     0,   655,   669,   656,   670,   657,
     658,   672,   659,   660,   661,   662,   663,   664,   665,   666,
     667,   668,     0,   652,     0,   688,     0,     0,     0,   693,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    75,    76,   651,     0,     0,   676,
     678,     0,   700,   702,     0,   710,   712,     0,     0,    40,
      20,    37,    38,    39,    41,    42,     0,   163,    21,    22,
      26,     0,    25,    35,     0,   164,   154,   381,     0,     0,
     475,   476,   389,   420,     0,     0,     0,     0,   419,     0,
       0,     0,     0,   584,   587,   585,   588,     0,     0,     0,
       0,   428,     0,   433,     0,   443,     0,   462,   463,   464,
     465,     0,     0,   487,   486,   480,     0,   615,   502,     0,
       0,     0,   501,     0,   611,   612,     0,   452,   455,   190,
     444,     0,   445,     0,   517,   618,     0,     0,     0,   190,
     528,   524,   621,     0,     0,     0,   533,   624,     0,     0,
       0,   551,   547,   190,   190,     0,   190,     0,   538,   605,
       0,     0,   638,     0,   639,   646,   647,   653,     0,   690,
       0,     0,     0,     0,     0,     0,     0,   695,     0,     0,
       0,    34,    27,     0,    33,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    27,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   447,
       0,     0,     0,     0,     0,     0,     0,   529,     0,     0,
       0,     0,     0,     0,     0,   543,   542,     0,     0,   540,
       0,     0,     0,     0,     0,     0,   677,     0,     0,     0,
     701,     0,     0,   711,     0,     0,     0,   692,     0,    29,
      31,    28,    36,   168,   171,   165,   166,   155,   158,     0,
     160,     0,   153,   385,     0,   371,     0,     0,   368,   373,
     382,   379,     0,     0,   391,   395,     0,   223,   413,   204,
     205,   206,   207,     0,     0,     0,   477,     0,     0,   558,
       0,     0,     0,     0,     0,     0,     0,   429,   422,   190,
       0,     0,   438,     0,     0,     0,   493,   190,   480,     0,
     479,   488,   190,     0,     0,     0,     0,     0,     0,   190,
     190,   446,   453,     0,   190,   456,     0,     0,     0,     0,
     190,     0,     0,     0,     0,     0,     0,    50,   548,    48,
     549,     0,   190,   552,     0,     0,     0,   640,   648,     0,
     691,     0,     0,   561,   704,     0,     0,   734,    80,     0,
       0,    32,     0,     0,     0,     0,   370,   375,     0,   374,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     408,     0,     0,     0,     0,     0,     0,     0,   410,     0,
       0,     0,     0,     0,   435,    24,   430,     0,     0,     0,
     481,   482,     0,     0,     0,     0,     0,   499,     0,     0,
     191,   449,     0,   451,   448,   457,   454,   521,     0,     0,
     530,   531,     0,     0,   536,     0,     0,    44,    58,     0,
      45,    49,     0,   546,   541,   550,     0,     0,     0,     0,
     649,   645,   689,     0,     0,     0,     0,     0,     0,     0,
       0,   694,   156,   159,   169,     0,   172,     0,   387,   371,
     386,     0,   371,   383,   379,   378,     0,     0,   400,   401,
     396,     0,   388,   392,     0,   224,   225,   226,   227,   228,
     229,   230,   231,   232,     0,   233,   234,   235,   236,     0,
       0,     0,   412,     0,   592,     0,   592,     0,   559,     0,
       0,     0,     0,     0,   199,   198,   190,   190,     0,   421,
     197,   196,   190,     0,     0,     0,   467,     0,   467,   494,
       0,   485,     0,     0,     0,     0,     0,   190,   190,   458,
       0,   190,     0,   190,     0,   190,    48,     0,    59,     0,
       0,   606,   607,   608,   609,     0,   174,   100,   133,   136,
     144,   148,    98,   642,    82,    88,    89,    93,     0,    85,
       0,    92,    85,     0,    85,     0,    85,     0,    85,     0,
      85,    84,     0,   640,   625,   650,   680,   576,   699,   708,
       0,   704,   704,     0,    80,     0,   703,   562,   398,   723,
       0,    81,   724,     0,     0,   167,   170,   372,   384,   369,
     380,     0,   409,     0,   393,   390,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   583,     0,   586,   411,   589,
     590,   424,   423,     0,   434,     0,   459,     0,     0,     0,
       0,     0,    27,     0,   500,     0,   610,     0,     0,   450,
     616,     0,   619,     0,   622,     0,    46,     0,    43,    68,
       0,     0,    53,    71,    55,    66,    67,   598,     0,     0,
       0,     0,    91,     0,     0,   117,     0,     0,   118,     0,
       0,   119,     0,     0,   120,     0,    83,     0,   641,     0,
       0,     0,   705,   706,     0,   707,     0,     0,     0,     0,
       0,   726,   728,   157,     0,   402,   398,   394,   237,   238,
     239,   190,   190,   190,   190,   592,   190,   190,   591,   592,
     596,   554,   202,     0,     0,   467,   190,   478,   190,   190,
     466,   467,   473,   495,   490,     0,   190,   190,   613,   617,
     620,   623,    52,    48,    71,    60,     0,     0,    70,   602,
      96,    85,    94,     0,    90,    85,    87,   101,     0,    85,
      85,    85,   134,     0,    85,    85,   137,     0,    85,   145,
       0,   149,   150,     0,    79,     0,   697,   686,   680,   680,
      80,     0,    80,   679,     0,     0,     0,   399,   560,   716,
     717,   714,   715,     0,   730,     0,     0,     0,     0,     0,
       0,     0,   594,   593,     0,     0,     0,     0,     0,     0,
     471,     0,   468,   470,     0,     0,     0,     0,     0,    47,
      69,     0,    54,    57,    72,     0,     0,    95,     0,    86,
      99,     0,   121,     0,   122,   123,   132,     0,   124,   125,
       0,   126,     0,     0,   173,   681,   682,     0,   683,     0,
     685,    27,     0,   698,     0,     0,     0,     0,   727,   397,
       0,     0,     0,     0,   595,   597,   190,   554,   554,   553,
     203,   190,   190,   472,   190,   474,   188,   186,   185,   187,
     190,   496,     0,   190,   489,     0,   614,    64,    56,     0,
     604,   603,     0,   601,     0,   102,   103,   104,   105,    85,
      85,    85,    85,   138,     0,   146,   142,   151,   152,     0,
      80,     0,   564,   577,   398,     0,   733,     0,   730,   730,
     729,     0,     0,     0,     0,   557,   555,   556,     0,     0,
     469,   491,     0,   492,     0,     0,    63,     0,    97,     0,
       0,     0,     0,   127,   128,   129,   130,     0,     0,     0,
     147,   684,   696,     0,     0,     0,     0,     0,   732,   731,
     243,   214,     0,   209,     0,    80,   220,     0,   192,   189,
       0,   504,    65,     0,    61,   106,   107,   108,   109,   110,
     111,    85,   139,     0,   143,   141,   574,   569,   570,   571,
     572,   573,   190,   190,   567,     0,   563,   575,     0,     0,
       0,   213,     0,     0,   208,     0,   218,     0,   219,     0,
       0,     0,   503,    62,     0,     0,     0,   131,     0,   566,
     565,     0,     0,    27,   183,   180,   179,   182,   200,   181,
     201,   217,   365,   175,   177,     0,   176,     0,   215,   244,
       0,   212,   209,    80,     0,   222,   220,     0,   190,   513,
     508,    80,   509,     0,   112,   113,   114,   115,   140,     0,
     194,   718,   190,     0,     0,     0,     0,   211,   210,     0,
     221,     0,     0,     0,   505,     0,   507,     0,     0,   135,
       0,     0,     0,     0,     0,     0,   194,   216,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   339,   340,   341,   299,   246,   248,   250,   252,     0,
     245,   270,   278,   280,   282,   284,     0,   277,   294,   184,
     509,   511,   509,   514,   398,   116,   190,   568,   721,    80,
       0,   713,   735,     0,     0,     0,     0,     0,     0,     0,
       0,   240,     0,     0,     0,     0,     0,   242,     0,   506,
     510,     0,   195,   720,     0,   190,   193,   347,   358,   360,
     190,   352,   300,   352,     0,   302,   190,     0,   190,     0,
     241,   343,   190,   190,   247,   190,     0,   190,   249,   345,
     190,   190,   251,   190,     0,   190,   253,   190,   352,   271,
     352,   273,   190,   279,   190,   190,   281,   190,   283,   190,
     190,   285,   190,   295,   352,   298,   512,     0,     0,   301,
       0,   305,     0,   303,     0,     0,   348,     0,   349,     0,
     254,   261,     0,   258,     0,     0,   260,   262,   269,     0,
     266,     0,     0,   268,   272,   276,     0,   274,   286,     0,
     288,   289,   290,     0,   292,   293,   296,   297,   718,   178,
     190,   190,   352,     0,   190,     0,     0,   190,   190,     0,
     190,   190,   190,     0,   190,   352,   190,   190,   719,     0,
     353,   354,   304,   346,     0,   350,   357,   359,   257,     0,
     255,     0,   259,   265,     0,   263,   344,   267,   275,   287,
     291,   190,   362,   190,     0,   190,     0,   190,     0,   355,
     351,     0,   256,   342,   264,   190,   363,   364,     0,   356,
     361
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1271, -1271, -1271,  -223,  -241,  -130, -1271,   298,  -172,   323,
   -1271, -1271, -1271, -1271, -1271, -1271,  -104,  -349,  -642,   -40,
    -760,  -636, -1271, -1271, -1271, -1271, -1271, -1271, -1271,  -568,
    -238, -1271, -1271, -1271,  -867, -1271, -1271,  -228,   -45,  1509,
     497,  1984, -1271,  -745,  -503,  -589, -1271, -1271,  -145, -1271,
   -1271,  -139, -1271, -1271, -1271,  -138,  -287, -1271, -1271,  -802,
   -1271, -1271, -1271, -1271, -1271,  -781, -1271, -1271, -1271, -1271,
    -746, -1271, -1271, -1271,   664, -1271, -1271, -1271, -1271, -1271,
     179, -1271, -1271,  -496, -1271, -1271,  -768, -1271, -1271,  -551,
   -1271, -1271, -1271, -1271,  -572,  1695,  -394, -1219,  -528, -1271,
   -1271, -1271,  -764,  -930,   -41,  -258,  -468, -1271, -1271, -1271,
   -1271,  -470,  -373,   162, -1271, -1271,  -342,  -999,  -343,  -424,
    -977,  -641, -1271,  -924,  -575, -1271, -1271, -1271, -1271,  -573,
   -1271, -1271, -1271, -1271,  -949,  -565, -1271,  -595, -1271, -1239,
   -1271,  -828, -1270, -1259, -1271, -1216, -1271,  -718, -1271, -1271,
    -793, -1271,   780,  -416,  -121,   588,  -428,    65,  -235,  -338,
     147, -1271, -1271, -1271,   242, -1271,   -71, -1271,  -127, -1271,
   -1271, -1271, -1271, -1271, -1271,  -854, -1271, -1271, -1271, -1271,
     669,   685,   688,   693,  -296,   480, -1271, -1271,  -149,    69,
   -1271, -1271, -1271, -1271, -1271,  -109, -1271, -1271, -1271, -1271,
      39, -1271,   465,   549,   621, -1271, -1271,   438, -1271, -1271,
     702, -1271, -1271, -1271,  -647, -1271, -1271, -1271,   635,   637,
       4,  -148,     9,   371, -1271, -1271, -1271, -1271, -1271, -1271,
   -1271,  -362,  -797,  -951, -1271, -1271,   709,   721, -1271,   275,
   -1271,  -935,  -848, -1271, -1271, -1271,  -197, -1271,   733, -1271,
    -174, -1271,   705,   736, -1271,  -169, -1271,   738, -1271,  -175,
   -1271, -1271,   464, -1271, -1271, -1271, -1271, -1271,   516,  -263,
   -1271, -1271,  -376, -1271, -1271,  -778, -1271, -1271, -1271, -1271,
    -756, -1271, -1271,   741, -1271, -1271,   684, -1271,   686, -1271,
   -1271,   266,  -617,   281,   284,   290,   757, -1271, -1271, -1271,
   -1271, -1271, -1271,   763, -1271, -1271, -1271, -1271,   774, -1271,
   -1271,   775, -1271, -1271,   776, -1271, -1271,   779,  -181,  -358,
     140, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271, -1271,
   -1271,   912, -1271,   577,  -147, -1271,   -95,  -210, -1271, -1271,
     -72, -1271,   155, -1271, -1271, -1271,  -819, -1271, -1271, -1271,
     578,    47,   920, -1271, -1271,   591, -1115,  -516, -1271, -1028,
     933, -1271, -1271, -1271,   -25,  -269, -1271, -1271
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   236,   640,   238,   903,   239,   480,
     240,   241,   429,   430,   242,   354,   243,   244,   917,   609,
     518,   610,   519,   717,   913,   611,   832,   992,   612,   833,
     916,  1058,  1059,  1143,   834,   835,   836,   918,   109,   216,
     388,   466,   945,   629,   771,   842,   734,   735,   736,   737,
     738,   739,   740,   928,  1064,   741,   742,   743,   933,   744,
     745,   937,  1074,  1153,  1229,   746,  1119,   747,   940,  1076,
     748,   749,   943,  1079,   499,   357,    41,   136,   246,   437,
     438,   439,   635,   440,   441,   637,   751,   752,  1202,  1203,
    1204,  1205,  1051,  1052,   897,   389,   687,  1206,  1251,   693,
     688,  1207,   893,  1041,   458,   459,  1174,   460,  1171,   461,
     462,  1178,   463,   669,   670,   671,   881,  1133,  1131,  1136,
    1134,  1210,  1299,  1364,  1372,  1300,  1379,  1306,  1383,  1388,
    1307,  1393,  1326,  1352,  1294,  1365,  1366,  1373,  1374,  1367,
    1354,  1355,  1401,  1356,  1357,  1358,  1359,  1483,  1484,  1498,
    1208,    42,   253,   359,   549,    44,   360,   254,   138,   248,
     553,   249,   451,   644,   445,   446,   641,   639,   255,   256,
     455,   456,   654,   557,   650,   868,   651,   876,    46,    47,
      48,    49,    50,    51,   464,   140,    52,    53,   257,   447,
     572,    55,   143,   272,   478,   465,   147,   274,   481,    56,
     258,    58,   149,   203,   300,   301,   503,   302,   303,   506,
      59,    60,   276,   277,   809,   278,   279,   280,   259,   260,
     467,   899,   959,   381,    62,   152,   285,   286,   492,   488,
     986,   760,   700,   904,  1053,    63,    64,    65,   291,   496,
    1182,  1246,  1222,  1223,  1314,    66,    67,    68,    69,    70,
      71,    72,   206,    73,    74,    75,    76,    77,    78,    79,
     211,    80,   324,   325,   521,   326,   327,   524,   960,   976,
     471,   679,   964,   535,   768,   761,  1124,  1163,  1164,  1165,
     762,   763,  1084,    81,    82,    83,   261,    84,   262,    85,
      86,   263,   792,   264,   265,   266,    87,    88,   162,   330,
     996,   331,   725,    89,   293,   294,   295,   296,    90,   307,
     308,    91,   314,   315,    92,   319,   320,    93,    94,   333,
     619,    95,   164,   337,   338,   529,    96,   182,    97,   183,
     184,   961,   219,   220,   860,    99,   186,   340,   341,   531,
     342,   191,   348,   349,   950,   951,   764,   765,   100,   222,
     223,   625,   962,   102,   225,   226,   963,  1253,   103,   770,
     334,   105,   538,   871,   872,  1026,   539,  1089
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     110,   692,   353,   431,   292,   350,   146,   111,   112,    61,
     113,   343,   495,   116,   345,   117,   547,   118,   489,   865,
     444,   119,   966,   120,   271,   121,   431,   122,   546,   123,
     306,   124,   332,   125,   318,   126,   187,   188,   313,    57,
     952,   894,   929,   127,   719,   534,   268,   633,  1042,   993,
     128,   811,   129,  1192,   130,   527,   131,  1126,   132,   794,
     133,   190,   237,   930,   954,    45,   934,   955,   267,    54,
    1082,   922,   355,   926,   290,   474,   926,   831,     1,   926,
     826,   948,   926,  1403,  1198,   926,  1353,   144,    10,     1,
    1375,  1375,     5,  1380,  1385,   941,  1390,  1390,   931,  1394,
     144,   935,   197,   949,   938,  1141,  1351,   106,  1425,   252,
    1427,     5,     5,     1,  1132,   733,  1132,  1137,    15,   144,
     690,   759,    10,   533,  1437,   691,   144,   452,   250,     5,
       5,   252,   351,     5,   114,    17,   457,    15,   251,   726,
     298,  1440,   317,  1440,    31,    32,   228,   134,   493,   344,
    1348,  1349,   346,   845,  -671,   848,  1349,   851,     5,   854,
      26,   856,    33,  1347,    29,  1200,   252,  1440,   732,   145,
      17,    26,  1462,  1448,  1212,    29,   339,  1482,    31,    32,
    1452,    33,   145,  1442,  1441,  1478,  1441,   494,     5,  1444,
    1456,   189,  1106,   189,  1457,    26,  1066,  1037,  1080,    29,
    1137,   145,   477,   540,   361,  1018,  -673,  1020,   145,   362,
    1441,   364,   495,  -674,   366,   367,   646,  1067,     2,  1038,
    1069,  1348,  1349,   322,  1320,   607,   435,   608,     4,  1397,
    1125,   339,   332,   926,     5,   926,   378,   379,  1144,   926,
     297,   382,  1232,  1481,   383,  -675,   702,   756,   979,   432,
    1218,   387,  1068,  1077,   984,  1070,   391,  1071,   393,   647,
     753,  1072,   914,   396,   595,   486,   289,   275,   971,  1088,
     401,   681,   974,     5,   991,   404,  1183,   989,   873,   -51,
     407,   608,   410,    21,     5,     5,   412,   620,   312,   505,
    1301,     2,  1308,   414,  1194,   726,   417,   486,  1195,  1196,
    1197,     4,     2,   517,   426,     5,  1198,  1145,   575,     5,
     528,  1194,     4,    36,  1347,  1195,  1196,  1197,   321,   322,
    1316,     5,   730,   731,   541,    19,  1347,   757,  1146,  1148,
    1169,   305,   997,   684,   247,  1121,   999,   685,  1140,   283,
    1002,  1004,  1005,     5,   926,  1008,  1009,   344,   247,  1011,
     346,    19,   347,   507,     7,    34,   947,  1199,   511,   247,
     443,   514,   284,  1147,  1149,  1150,  1151,  1348,  1349,    12,
     830,   468,  1348,  1349,  1199,  1339,    21,  1340,   469,     5,
    1213,    34,   484,   485,  1348,  1349,   483,  1200,  1221,   726,
    1176,   498,   707,  1345,   544,  1310,   545,  1312,   728,   729,
     509,   550,   453,  1224,  1368,   513,    36,    24,   516,   473,
    1386,   558,   784,   674,   676,   329,   526,   731,   559,   729,
     891,   730,   731,     5,   230,   711,   565,  1230,  1347,   696,
     698,   715,   352,  1319,   713,   673,   675,   332,  1225,  1226,
    1227,   730,   731,   874,   582,   882,   883,   884,   690,     5,
     586,   695,   697,   691,   653,   -30,   433,   434,   546,   726,
    1341,  1046,   706,   431,   729,  1047,  1048,  1049,  1238,  1255,
    1256,     5,     5,   906,     5,   905,  1244,   547,   297,   298,
    1113,  1114,  1115,  1116,   726,   250,   730,   731,   728,   729,
     626,   709,  1060,   229,   630,   251,   231,   232,   233,   234,
     235,   828,  1315,   829,  1381,   716,   454,     5,   718,  1194,
    1395,   730,   731,  1195,  1196,  1197,   755,   726,   777,     5,
     672,   779,  1086,   677,  1347,   680,   684,  1194,   682,   683,
     685,  1195,  1196,  1197,  1214,   773,   694,     5,  1217,  1194,
    1361,   469,  1347,  1195,  1196,  1197,  1194,   705,     7,     5,
    1195,  1196,  1197,  1198,   115,   690,     5,   710,   107,   108,
     691,   712,  1187,   457,   714,     5,   726,   830,     5,   727,
     728,   729,  1199,   532,  1343,   720,   134,   229,   230,   536,
     231,   232,   233,   234,   235,   154,   533,   156,   537,   158,
    1199,   160,   141,   730,   731,   732,   433,   150,   434,   153,
     443,   155,  1199,   157,   769,   159,   783,   229,   230,  1199,
     231,   232,   233,  1194,   828,   718,   727,  1195,  1196,  1197,
     907,     5,  1062,     5,   -59,  1369,  1347,  1369,  1347,   -59,
     -59,   -59,   607,   192,  1200,   193,     5,   194,   195,  1361,
     196,  1347,   944,   803,   214,   215,   198,   199,   805,   200,
     201,   202,   204,   205,   207,   205,   209,   210,   608,   212,
     965,   213,  1130,  1087,    40,   335,   336,  1201,   234,   235,
    1211,   731,   431,   479,  1215,   428,  1199,   827,  1220,   994,
     877,   217,   887,   218,   221,   224,   990,   227,   878,   879,
     880,   648,   649,   840,   920,   843,   247,   443,   846,   898,
     849,   923,   852,   924,   855,   730,   731,   857,   134,   229,
     830,  1065,   231,   232,   233,   861,  1348,  1349,   864,   936,
     866,   632,   939,   912,  1154,   869,   718,   915,  1324,  1156,
    1496,  1497,  1257,  1157,  1158,  1159,  1160,  1061,  1130,     5,
    1389,  1389,  1311,   356,  1237,   358,  1240,     5,   704,  1318,
    1135,   517,   363,  1216,   365,  1376,   830,   726,   368,   369,
     727,   728,   729,  1391,   370,   371,   372,   373,  1384,   374,
    1495,   375,   202,   377,  1096,  1097,  1363,  1363,  1371,  1371,
      43,  1378,   380,  1363,   730,   731,   732,   384,   385,   288,
     386,   780,  1161,   642,   830,   956,  1081,   390,   165,   392,
      10,  1015,  1016,   394,   395,  1040,  1040,   398,   862,   863,
     399,   400,   718,   915,   166,   402,   403,   167,  1105,  1128,
    1129,   252,   168,   408,   376,    17,   310,   411,   501,   885,
     413,   169,   889,  1044,   281,   287,   282,   415,   170,   418,
     419,   421,   422,   424,   431,   425,   895,   919,   229,   901,
     171,   231,   232,   233,   234,   235,    31,    32,   829,   581,
     703,  1447,   172,   208,  1450,   173,   299,   174,  1451,   309,
     175,  1454,   522,   229,   323,  1455,   231,   232,   233,   269,
    1073,   270,  1075,   721,   470,   472,   176,  1057,   475,   476,
    1194,  1130,   177,   858,  1195,  1196,  1197,  1198,   722,  1486,
       5,   723,   497,   178,   179,   180,  1019,   724,   181,   517,
     726,   508,    98,   727,   728,   729,   512,   416,  1485,   515,
     101,   420,  1458,  1487,   134,   229,   230,   525,   231,   232,
     233,   234,   235,   104,   542,   543,   423,   730,   731,   732,
    1028,   358,   548,     0,   551,     0,     0,     0,   552,   554,
     555,     0,   556,  1199,     0,   718,     0,     0,     0,     0,
       0,     0,   561,     0,   562,   563,     0,   564,     0,   566,
     567,     0,   569,   570,     0,   571,   573,     0,  1200,     0,
     577,   578,   579,     0,     0,   380,     0,  1152,     0,     0,
     584,   585,     0,     0,   588,   589,     0,     0,   592,     0,
       0,  1142,     0,     0,     0,   598,   599,  1102,  1040,   602,
     603,     0,   605,   606,     0,     0,     0,  1107,     3,     0,
       0,     0,   616,   617,   618,     0,     6,     0,     0,   623,
       0,   624,     0,   627,     0,   628,     8,     0,     0,   718,
       0,     0,  1127,     9,   487,     0,    11,     0,     0,     0,
       0,     0,     0,     0,   500,     0,   504,     0,  1228,     0,
    1194,    16,   510,     0,  1195,  1196,  1197,  1198,     0,     0,
       5,     0,    18,     0,     0,    20,     0,    22,  1040,     0,
     726,  1168,     0,   727,   728,   729,     0,  1172,     5,  1175,
    1219,     0,  1179,    25,     0,    27,     0,    28,   726,    30,
       0,   727,   728,   729,     0,    35,     0,   730,   731,   732,
       0,     0,     0,     0,     0,   758,     0,   769,     0,     0,
    1191,     0,     0,  1199,     0,   730,   731,   732,     0,   774,
       0,     0,     3,     0,     0,     0,   548,     0,     0,   548,
       6,   552,     0,   781,     0,     0,     0,     0,  1200,     0,
       8,  1040,     0,     0,     0,     0,     0,     9,     0,     0,
    1233,   786,  1234,     0,     0,     0,   787,   788,   789,    14,
     790,   791,   793,   791,     0,    16,   796,     0,  1247,     0,
       0,     0,     0,     0,     0,     0,    18,     0,     0,    20,
       0,    22,   807,   808,   810,   808,     0,   812,     0,   813,
       0,   815,     0,   817,     0,     0,     0,    25,     0,    27,
       0,    28,   638,    30,     0,     0,   643,     0,     0,    35,
     229,   230,   838,   231,   232,   233,   234,   235,     0,     0,
     829,     0,     0,     0,   839,   686,   841,     0,     0,   844,
       0,   847,     0,   850,     0,   853,     0,   853,     0,     0,
     618,     0,     0,   859,     0,     0,   708,     0,   624,   624,
       0,   628,     0,     0,     0,   867,     0,     0,     0,     0,
     870,     0,   486,     0,     0,  1344,     0,  1445,     0,  1446,
       5,     0,   750,     0,  1449,     0,     0,     0,   750,     0,
     726,  1453,   750,   727,   728,   729,     0,     0,     0,     0,
    1443,     0,   757,     0,     0,   486,   946,   758,     0,  1404,
       0,     0,  1407,     5,  1409,     0,   486,   730,   731,   732,
       0,  1414,     0,   726,     5,     0,   727,   728,   729,  1421,
       0,     0,     0,     0,   726,   757,     0,   727,   728,   729,
     758,     0,     0,     0,     0,     0,   757,     0,     0,     0,
     730,   731,   732,   229,   230,     0,   231,   232,   233,   234,
     235,   730,   731,   732,     0,   457,     5,     0,     0,     0,
       0,   781,     0,   867,     0,     0,   726,     0,     0,   727,
     728,   729,   791,     0,     0,     0,   791,     0,   975,     0,
     977,   978,   808,     0,     0,   981,     0,     0,   808,     0,
       0,   985,   813,   730,   731,   732,     0,   655,   656,   657,
     658,   659,   660,   661,   662,   663,   995,     0,   853,     0,
     998,     0,   853,     0,     0,     0,  1001,  1003,   853,     0,
       0,  1007,   853,     0,  1010,   853,     0,  1012,     0,  1491,
    1013,     0,     0,     5,     0,   859,   859,  1017,     0,   628,
       0,  1021,  1022,   726,     5,   886,   727,   728,   729,     0,
       0,  1025,  1027,     5,   726,     0,     0,   727,   728,   729,
     758,     0,   896,   726,  1219,     0,   727,   728,   729,     0,
     730,   731,   732,     0,     0,     0,     0,     0,     0,     0,
       0,   730,   731,   732,     0,     0,     0,     0,     0,     0,
     730,   731,   732,   921,     0,   925,     0,     0,   925,     0,
       0,   925,     0,     0,   925,     0,     0,   925,     0,     0,
       0,  1085,     0,   750,     0,     0,     0,  1091,  1092,  1093,
    1094,   957,     0,     0,   975,   975,     0,   655,   656,   657,
     658,   659,   660,   661,   662,   663,   664,   665,   666,   667,
     668,     0,  1104,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1109,  1110,  1111,     0,  1112,   853,   853,   853,
       0,  1117,     0,  1118,     0,     0,     0,   628,     0,  1123,
       0,   867,     0,     0,     0,  1025,  1025,  1295,  1258,  1259,
    1260,  1261,  1262,  1263,  1264,  1296,  1265,  1266,  1267,  1268,
    1269,  1270,  1271,  1272,  1273,  1274,  1275,  1276,  1277,  1278,
    1279,  1280,  1281,  1282,  1283,  1284,  1285,  1286,  1287,  1288,
    1297,  1298,  1289,  1290,  1291,  1292,  1293,     0,  1170,     0,
    1173,     0,   628,  1177,     0,     0,     0,  1180,  1181,  1036,
       0,     0,  1184,  1185,     0,  1186,     0,     0,   853,  1050,
    1188,     0,     0,     0,     0,     0,     0,     0,     0,   750,
       0,     0,     0,     0,     0,   925,  1193,   925,     0,     0,
       0,   925,     0,     0,     0,     0,     0,     0,     0,     3,
       0,   750,     0,     0,     0,     0,     5,     6,     0,   750,
       0,     0,     0,     0,     0,     0,   726,     8,     0,   727,
     728,   729,     0,     0,     9,  1235,     0,  1236,     0,  1173,
     628,  1239,   304,  1177,  1241,   311,    14,   316,  1243,  1245,
     328,  1248,    16,   730,   731,   732,     0,  1250,  1252,     0,
       0,     0,     0,    18,     0,     0,    20,     0,    22,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1323,  1250,    25,     0,    27,     0,    28,     0,
      30,     0,     0,     0,     0,     0,    35,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   925,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1162,     0,     0,
       0,  1325,  1327,  1328,  1329,  1330,     0,     0,  1332,  1333,
    1334,  1335,  1336,     0,     0,  1338,     0,  1245,     0,  1245,
       0,   867,     0,     0,     0,     0,   628,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   750,     0,     0,   750,     0,     0,
       0,   750,     0,     0,     0,   750,     0,     0,  1400,     0,
    1402,     0,     0,  1405,     0,     0,     0,     0,     0,   427,
       0,     0,  1412,     0,  1415,     0,   442,     0,   448,   449,
    1419,     0,  1422,     0,     0,  1400,     0,  1426,     0,     0,
       0,  1429,     0,     0,     0,   482,  1433,     0,     0,     0,
     490,  1400,     0,     0,     0,     0,     0,     0,     0,   750,
       0,     0,     0,     0,     0,     0,     0,   750,     0,   750,
       0,     0,     0,     0,     0,     0,   750,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1252,     0,  1459,     0,  1400,
       0,  1464,     0,     0,     0,  1469,  1471,     0,     0,  1474,
       0,     0,  1400,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   560,     0,     0,     0,     0,  1488,     0,
       0,     0,     0,   568,     0,     0,     0,     0,     0,  1350,
     576,  1362,  1362,  1370,  1370,     0,  1377,  1382,  1362,  1387,
    1387,     0,  1392,     0,   397,   587,     0,     0,     0,     0,
       0,     0,   593,     0,     0,   596,   597,     0,   405,   406,
     601,   409,     0,   604,     0,     0,     0,     0,     0,     0,
     613,     0,     0,   615,     0,     0,     0,     0,   621,     0,
     622,     0,     0,     0,     0,     0,     0,     0,     0,   631,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   645,     0,     0,     0,   652,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   689,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   701,   135,   137,   139,   139,   142,     0,     0,
     148,   139,   151,   139,   148,   139,   148,   139,   148,   139,
     148,   161,   163,     0,   185,   185,   185,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   754,     0,
       0,     0,     0,     0,   766,     0,     0,     0,   772,     0,
       0,     0,     0,     0,   775,     0,   776,     0,     0,     0,
     778,     0,     0,     0,     0,     0,   782,     0,     0,     0,
       0,     0,     0,   785,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   574,     0,   245,     0,     0,     0,
       0,   273,   580,     0,     0,     0,   795,   583,     0,   797,
     798,   799,   800,     0,   590,   591,     0,     0,     0,   594,
       0,     0,     0,   806,     0,   600,     0,     0,     0,     0,
       0,     0,   814,     0,   816,     0,     0,   614,     0,   820,
       0,   822,     0,   824,     0,     0,     0,     0,     0,   837,
    1302,  1258,  1259,  1260,  1261,  1262,  1263,  1264,  1303,  1265,
    1266,  1267,  1268,  1269,  1270,  1271,  1272,  1273,  1274,  1275,
    1276,  1277,  1278,  1279,  1280,  1281,  1282,  1283,  1284,  1285,
    1286,  1287,  1288,  1304,  1305,  1289,  1290,  1291,  1292,  1293,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   888,     0,   890,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   900,     0,
     902,     0,     0,     0,     0,     0,     0,   908,     0,     0,
     909,     0,   910,     0,   911,     0,     0,     0,     0,     0,
     436,     0,     0,     0,     0,   450,   137,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   491,   137,     0,   953,
       0,     0,     0,     0,     0,   502,     0,   958,     0,     0,
       0,   801,   802,     0,     0,     0,     0,   804,     0,     0,
       0,   520,     0,     0,   523,     0,     0,     0,     0,     0,
       0,   530,   818,   819,     0,     0,   821,     0,   823,     0,
     825,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1000,     0,     0,
       0,     0,  1006,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1014,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1023,     0,     0,     0,     0,
       0,     0,  1024,     0,     0,  1029,  1030,  1031,  1032,  1033,
    1034,     0,     0,  1035,     0,  1039,     0,     0,  1043,     0,
       0,     0,     0,  1045,     0,  1054,  1055,  1056,     0,     0,
       0,     0,     0,     0,     0,  1063,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   634,   636,
       0,     0,     0,     0,     0,  1090,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   678,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   699,     0,     0,     0,
       0,     0,     0,  1108,     0,     0,   967,   968,   969,   970,
       0,   972,   973,     0,     0,     0,     0,     0,  1120,     0,
    1122,   980,     0,   982,   983,     0,     0,     0,     0,     0,
       0,   987,   988,     0,     0,     1,     2,  1138,  1139,     0,
     767,     3,     0,     0,   185,     0,     4,     0,     5,     6,
       0,     0,     0,     0,     0,     0,     7,     0,  1155,     8,
       0,     0,     0,  1166,  1167,     0,     9,    10,     0,    11,
       0,    12,     0,     0,     0,     0,    13,     0,    14,     0,
       0,    15,     0,     0,    16,     0,     0,     0,     0,     0,
       0,     0,    17,     0,     0,    18,     0,    19,    20,    21,
      22,     0,     0,     0,     0,     0,     0,     0,    23,    24,
       0,     0,     0,     0,     0,     0,    25,    26,    27,     0,
      28,    29,    30,    31,    32,    33,     0,    34,    35,    36,
       0,  1231,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1095,     0,     0,     0,     0,  1098,  1099,  1249,  1100,
       0,     0,     0,     0,     0,  1101,     0,     0,  1103,     0,
       0,  1309,     0,     0,     0,     0,     0,     0,     0,     0,
    1317,     0,  1321,  1322,     0,     0,     0,   875,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   892,     0,   892,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1331,     0,
       0,     0,     0,     0,     0,  1337,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   927,     0,     0,
     932,     0,     0,  1346,     0,  1360,     0,     0,     0,   942,
       0,     0,     0,     0,     0,   699,     0,     0,   699,     0,
    1396,     0,     0,     0,     0,     0,     0,  1189,  1190,  1258,
    1259,  1260,  1261,  1262,  1263,  1264,     0,  1265,  1266,  1267,
    1268,  1269,  1270,  1271,  1272,  1273,  1274,  1275,  1276,  1277,
    1278,  1279,  1280,  1281,  1282,  1283,  1284,  1285,  1286,  1287,
    1288,     0,     0,  1289,  1290,  1291,  1292,  1293,     0,     0,
       0,     0,     0,     0,     0,     0,  1438,  1439,     0,     0,
       0,     0,     0,  1242,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1254,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1463,     0,  1466,  1467,     0,     0,     0,     0,
       0,     0,  1476,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1493,     0,  1078,     0,     0,
       0,     0,     0,     0,     0,     0,  1083,  1500,     0,     0,
       0,  1342,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1398,     0,     0,     0,     0,  1399,     0,     0,     0,     0,
       0,  1406,     0,  1408,     0,     0,     0,  1410,  1411,     0,
    1413,     0,  1416,     0,     0,  1417,  1418,     0,  1420,     0,
    1423,     0,  1424,     0,     0,     0,     0,  1428,     0,  1430,
    1431,     0,  1432,     0,  1434,  1435,   892,  1436,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   699,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1460,  1461,     0,     0,  1465,
       0,     0,  1468,  1470,     0,  1472,  1473,  1475,     0,  1477,
       0,  1479,  1480,     0,     0,     0,  1209,     0,     0,  1209,
       0,     0,     0,  1209,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   892,  1489,     0,  1490,     0,
    1492,     0,  1494,     0,     0,     0,     0,     0,     0,     0,
    1499,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   892,   892,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1313
};

static const yytype_int16 yycheck[] =
{
      45,   573,   243,   352,   201,   228,   115,    52,    53,     0,
      55,   221,   385,    58,   224,    60,   444,    62,   380,   764,
     358,    66,   876,    68,   196,    70,   375,    72,   444,    74,
     204,    76,   213,    78,   209,    80,   131,   132,   207,     0,
     859,   805,   844,    88,   612,   421,   195,   543,   978,   916,
      95,   698,    97,  1168,    99,   413,   101,  1085,   103,   676,
     105,   133,   192,   844,   861,     0,   847,   864,   195,     0,
    1021,   839,   244,   841,   201,   371,   844,   719,     4,   847,
     716,   859,   850,  1353,    14,   853,  1325,     6,    36,     4,
    1329,  1330,    17,  1332,  1333,   855,  1335,  1336,   844,  1338,
       6,   847,   147,   859,   850,  1104,  1325,     0,  1378,    57,
    1380,    17,    17,     4,  1091,   618,  1093,  1094,    50,     6,
       7,   624,    36,    26,  1394,    12,     6,   362,    19,    17,
      17,    57,   236,    17,   169,    61,    16,    50,    29,    27,
      24,  1400,    74,  1402,    92,    93,   191,   153,   383,   221,
      80,    81,   224,   742,   169,   744,    81,   746,    17,   748,
      86,   750,    94,    22,    90,    95,    57,  1426,    56,    88,
      61,    86,  1442,  1412,  1173,    90,    79,    82,    92,    93,
    1419,    94,    88,  1402,  1400,  1455,  1402,   384,    17,  1405,
    1429,    41,  1059,    41,  1433,    86,   998,   975,  1017,    90,
    1177,    88,   374,   426,   249,   950,   169,   952,    88,   254,
    1426,   256,   585,   169,   259,   260,   554,   998,     5,   975,
    1001,    80,    81,    52,  1252,   161,   356,   163,    15,  1344,
    1084,    79,   413,  1001,    17,  1003,   281,   282,  1105,  1007,
      23,   286,  1193,  1459,   289,   169,   584,   623,   895,   353,
    1180,   296,   998,  1013,   901,  1001,   301,  1003,   303,   555,
     618,  1007,   830,   308,   505,     9,   201,   198,   885,  1025,
     315,   567,   889,    17,   916,   320,  1143,   913,   774,   161,
     325,   163,   327,    68,    17,    17,   331,   528,    73,   393,
    1239,     5,  1241,   338,     7,    27,   341,     9,    11,    12,
      13,    15,     5,   407,   349,    17,    14,  1109,   480,    17,
     414,     7,    15,    98,    22,    11,    12,    13,    51,    52,
    1250,    17,    54,    55,   428,    66,    22,    39,  1109,  1110,
    1127,    72,   921,     7,    62,  1080,   925,    11,  1102,     3,
     929,   930,   931,    17,  1112,   934,   935,   419,    62,   938,
     422,    66,    42,   394,    25,    96,   859,    70,   399,    62,
      63,   402,    26,  1109,  1110,  1111,  1112,    80,    81,    40,
     719,   367,    80,    81,    70,  1310,    68,  1312,    49,    17,
    1173,    96,   378,   379,    80,    81,   377,    95,  1181,    27,
    1135,   387,   589,  1323,   439,  1243,   441,  1245,    31,    32,
     396,   446,   363,  1184,  1328,   401,    98,    78,   404,   370,
    1334,   456,   653,   562,   563,    65,   412,    55,   463,    32,
     796,    54,    55,    17,   155,   599,   471,  1191,    22,   578,
     579,   606,   156,  1252,   603,   562,   563,   618,  1184,  1185,
    1186,    54,    55,   781,   489,   787,   788,   789,     7,    17,
     495,   578,   579,    12,   558,   156,    33,    34,   874,    27,
    1314,     7,   589,   812,    32,    11,    12,    13,  1213,  1233,
    1234,    17,    17,   815,    17,   813,  1221,   905,    23,    24,
    1069,  1070,  1071,  1072,    27,    19,    54,    55,    31,    32,
     535,   595,   995,   154,   539,    29,   157,   158,   159,   160,
     161,   162,  1248,   164,  1332,   609,    58,    17,   612,     7,
    1338,    54,    55,    11,    12,    13,   620,    27,   639,    17,
     561,   642,  1025,   564,    22,   566,     7,     7,   569,   570,
      11,    11,    12,    13,  1175,   630,   577,    17,  1179,     7,
      20,    49,    22,    11,    12,    13,     7,   588,    25,    17,
      11,    12,    13,    14,    57,     7,    17,   598,   165,   166,
      12,   602,  1151,    16,   605,    17,    27,   916,    17,    30,
      31,    32,    70,   418,  1319,   616,   153,   154,   155,   424,
     157,   158,   159,   160,   161,   120,    26,   122,    48,   124,
      70,   126,   112,    54,    55,    56,    33,   117,    34,   119,
      63,   121,    70,   123,    46,   125,   651,   154,   155,    70,
     157,   158,   159,     7,   162,   719,    30,    11,    12,    13,
     817,    17,   995,    17,   162,    21,    22,    21,    22,   167,
     168,   169,   161,   136,    95,   138,    17,   140,   141,    20,
     143,    22,   153,   688,   167,   168,   149,   150,   693,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   163,   162,
     870,   164,     8,  1025,     0,    75,    76,  1170,   160,   161,
    1173,    55,  1021,   375,  1177,   352,    70,   717,  1181,   917,
     784,   184,   791,   186,   187,   188,   914,   190,   157,   158,
     159,    59,    60,   738,   839,   740,    62,    63,   743,   808,
     745,   840,   747,   841,   749,    54,    55,   752,   153,   154,
    1059,   998,   157,   158,   159,   760,    80,    81,   763,   849,
     765,   542,   852,   827,  1118,   770,   830,   831,  1256,     7,
      83,    84,  1235,    11,    12,    13,    14,   995,     8,    17,
    1335,  1336,  1245,   246,  1212,   248,  1216,    17,   586,  1252,
    1093,   855,   255,  1177,   257,  1330,  1105,    27,   261,   262,
      30,    31,    32,  1336,   267,   268,   269,   270,  1333,   272,
    1488,   274,   275,   276,  1037,  1038,  1327,  1328,  1329,  1330,
       0,  1332,   285,  1334,    54,    55,    56,   290,   291,   201,
     293,   644,    70,   551,  1143,   866,  1019,   300,   129,   302,
      36,   948,   949,   306,   307,   977,   978,   310,   761,   762,
     313,   314,   916,   917,   129,   318,   319,   129,  1059,  1088,
    1089,    57,   129,   326,   275,    61,   205,   330,   390,   790,
     333,   129,   793,   981,   199,    71,   199,   340,   129,   342,
     343,   344,   345,   346,  1193,   348,   807,   838,   154,   810,
     129,   157,   158,   159,   160,   161,    92,    93,   164,   488,
     585,  1412,   129,   158,  1415,   129,   202,   129,  1419,   205,
     129,  1422,   408,   154,   210,  1426,   157,   158,   159,   195,
    1010,   195,  1012,   617,   368,   369,   129,   991,   372,   373,
       7,     8,   129,   753,    11,    12,    13,    14,   617,  1471,
      17,   617,   386,   129,   129,   129,   951,   617,   129,  1013,
      27,   395,     0,    30,    31,    32,   400,   340,  1469,   403,
       0,   343,  1438,  1474,   153,   154,   155,   411,   157,   158,
     159,   160,   161,     0,   437,   438,   345,    54,    55,    56,
     965,   444,   445,    -1,   447,    -1,    -1,    -1,   451,   452,
     453,    -1,   455,    70,    -1,  1059,    -1,    -1,    -1,    -1,
      -1,    -1,   465,    -1,   467,   468,    -1,   470,    -1,   472,
     473,    -1,   475,   476,    -1,   478,   479,    -1,    95,    -1,
     483,   484,   485,    -1,    -1,   488,    -1,  1117,    -1,    -1,
     493,   494,    -1,    -1,   497,   498,    -1,    -1,   501,    -1,
      -1,  1105,    -1,    -1,    -1,   508,   509,  1052,  1180,   512,
     513,    -1,   515,   516,    -1,    -1,    -1,  1062,    10,    -1,
      -1,    -1,   525,   526,   527,    -1,    18,    -1,    -1,   532,
      -1,   534,    -1,   536,    -1,   538,    28,    -1,    -1,  1143,
      -1,    -1,  1087,    35,   380,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   390,    -1,   392,    -1,  1188,    -1,
       7,    53,   398,    -1,    11,    12,    13,    14,    -1,    -1,
      17,    -1,    64,    -1,    -1,    67,    -1,    69,  1250,    -1,
      27,  1126,    -1,    30,    31,    32,    -1,  1132,    17,  1134,
      37,    -1,  1137,    85,    -1,    87,    -1,    89,    27,    91,
      -1,    30,    31,    32,    -1,    97,    -1,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    44,    -1,    46,    -1,    -1,
    1165,    -1,    -1,    70,    -1,    54,    55,    56,    -1,   632,
      -1,    -1,    10,    -1,    -1,    -1,   639,    -1,    -1,   642,
      18,   644,    -1,   646,    -1,    -1,    -1,    -1,    95,    -1,
      28,  1323,    -1,    -1,    -1,    -1,    -1,    35,    -1,    -1,
    1205,   664,  1207,    -1,    -1,    -1,   669,   670,   671,    47,
     673,   674,   675,   676,    -1,    53,   679,    -1,  1223,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    64,    -1,    -1,    67,
      -1,    69,   695,   696,   697,   698,    -1,   700,    -1,   702,
      -1,   704,    -1,   706,    -1,    -1,    -1,    85,    -1,    87,
      -1,    89,   548,    91,    -1,    -1,   552,    -1,    -1,    97,
     154,   155,   725,   157,   158,   159,   160,   161,    -1,    -1,
     164,    -1,    -1,    -1,   737,   571,   739,    -1,    -1,   742,
      -1,   744,    -1,   746,    -1,   748,    -1,   750,    -1,    -1,
     753,    -1,    -1,   756,    -1,    -1,   592,    -1,   761,   762,
      -1,   764,    -1,    -1,    -1,   768,    -1,    -1,    -1,    -1,
     773,    -1,     9,    -1,    -1,  1320,    -1,  1407,    -1,  1409,
      17,    -1,   618,    -1,  1414,    -1,    -1,    -1,   624,    -1,
      27,  1421,   628,    30,    31,    32,    -1,    -1,    -1,    -1,
    1404,    -1,    39,    -1,    -1,     9,    43,    44,    -1,  1354,
      -1,    -1,  1357,    17,  1359,    -1,     9,    54,    55,    56,
      -1,  1366,    -1,    27,    17,    -1,    30,    31,    32,  1374,
      -1,    -1,    -1,    -1,    27,    39,    -1,    30,    31,    32,
      44,    -1,    -1,    -1,    -1,    -1,    39,    -1,    -1,    -1,
      54,    55,    56,   154,   155,    -1,   157,   158,   159,   160,
     161,    54,    55,    56,    -1,    16,    17,    -1,    -1,    -1,
      -1,   874,    -1,   876,    -1,    -1,    27,    -1,    -1,    30,
      31,    32,   885,    -1,    -1,    -1,   889,    -1,   891,    -1,
     893,   894,   895,    -1,    -1,   898,    -1,    -1,   901,    -1,
      -1,   904,   905,    54,    55,    56,    -1,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   919,    -1,   921,    -1,
     923,    -1,   925,    -1,    -1,    -1,   929,   930,   931,    -1,
      -1,   934,   935,    -1,   937,   938,    -1,   940,    -1,  1484,
     943,    -1,    -1,    17,    -1,   948,   949,   950,    -1,   952,
      -1,   954,   955,    27,    17,   791,    30,    31,    32,    -1,
      -1,   964,   965,    17,    27,    -1,    -1,    30,    31,    32,
      44,    -1,   808,    27,    37,    -1,    30,    31,    32,    -1,
      54,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    54,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,
      54,    55,    56,   839,    -1,   841,    -1,    -1,   844,    -1,
      -1,   847,    -1,    -1,   850,    -1,    -1,   853,    -1,    -1,
      -1,  1024,    -1,   859,    -1,    -1,    -1,  1030,  1031,  1032,
    1033,   867,    -1,    -1,  1037,  1038,    -1,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,    -1,  1055,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1065,  1066,  1067,    -1,  1069,  1070,  1071,  1072,
      -1,  1074,    -1,  1076,    -1,    -1,    -1,  1080,    -1,  1082,
      -1,  1084,    -1,    -1,    -1,  1088,  1089,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,    -1,  1131,    -1,
    1133,    -1,  1135,  1136,    -1,    -1,    -1,  1140,  1141,   975,
      -1,    -1,  1145,  1146,    -1,  1148,    -1,    -1,  1151,   985,
    1153,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   995,
      -1,    -1,    -1,    -1,    -1,  1001,  1169,  1003,    -1,    -1,
      -1,  1007,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    10,
      -1,  1017,    -1,    -1,    -1,    -1,    17,    18,    -1,  1025,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    28,    -1,    30,
      31,    32,    -1,    -1,    35,  1208,    -1,  1210,    -1,  1212,
    1213,  1214,   203,  1216,  1217,   206,    47,   208,  1221,  1222,
     211,  1224,    53,    54,    55,    56,    -1,  1230,  1231,    -1,
      -1,    -1,    -1,    64,    -1,    -1,    67,    -1,    69,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1255,  1256,    85,    -1,    87,    -1,    89,    -1,
      91,    -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1112,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1123,    -1,    -1,
      -1,  1294,  1295,  1296,  1297,  1298,    -1,    -1,  1301,  1302,
    1303,  1304,  1305,    -1,    -1,  1308,    -1,  1310,    -1,  1312,
      -1,  1314,    -1,    -1,    -1,    -1,  1319,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1170,    -1,    -1,  1173,    -1,    -1,
      -1,  1177,    -1,    -1,    -1,  1181,    -1,    -1,  1351,    -1,
    1353,    -1,    -1,  1356,    -1,    -1,    -1,    -1,    -1,   350,
      -1,    -1,  1365,    -1,  1367,    -1,   357,    -1,   359,   360,
    1373,    -1,  1375,    -1,    -1,  1378,    -1,  1380,    -1,    -1,
      -1,  1384,    -1,    -1,    -1,   376,  1389,    -1,    -1,    -1,
     381,  1394,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1235,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1243,    -1,  1245,
      -1,    -1,    -1,    -1,    -1,    -1,  1252,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1438,    -1,  1440,    -1,  1442,
      -1,  1444,    -1,    -1,    -1,  1448,  1449,    -1,    -1,  1452,
      -1,    -1,  1455,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   464,    -1,    -1,    -1,    -1,  1481,    -1,
      -1,    -1,    -1,   474,    -1,    -1,    -1,    -1,    -1,  1325,
     481,  1327,  1328,  1329,  1330,    -1,  1332,  1333,  1334,  1335,
    1336,    -1,  1338,    -1,   309,   496,    -1,    -1,    -1,    -1,
      -1,    -1,   503,    -1,    -1,   506,   507,    -1,   323,   324,
     511,   326,    -1,   514,    -1,    -1,    -1,    -1,    -1,    -1,
     521,    -1,    -1,   524,    -1,    -1,    -1,    -1,   529,    -1,
     531,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   540,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   553,    -1,    -1,    -1,   557,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   572,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   583,   109,   110,   111,   112,   113,    -1,    -1,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,    -1,   130,   131,   132,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   619,    -1,
      -1,    -1,    -1,    -1,   625,    -1,    -1,    -1,   629,    -1,
      -1,    -1,    -1,    -1,   635,    -1,   637,    -1,    -1,    -1,
     641,    -1,    -1,    -1,    -1,    -1,   647,    -1,    -1,    -1,
      -1,    -1,    -1,   654,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   479,    -1,   192,    -1,    -1,    -1,
      -1,   197,   487,    -1,    -1,    -1,   677,   492,    -1,   680,
     681,   682,   683,    -1,   499,   500,    -1,    -1,    -1,   504,
      -1,    -1,    -1,   694,    -1,   510,    -1,    -1,    -1,    -1,
      -1,    -1,   703,    -1,   705,    -1,    -1,   522,    -1,   710,
      -1,   712,    -1,   714,    -1,    -1,    -1,    -1,    -1,   720,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   792,    -1,   794,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   809,    -1,
     811,    -1,    -1,    -1,    -1,    -1,    -1,   818,    -1,    -1,
     821,    -1,   823,    -1,   825,    -1,    -1,    -1,    -1,    -1,
     356,    -1,    -1,    -1,    -1,   361,   362,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   382,   383,    -1,   860,
      -1,    -1,    -1,    -1,    -1,   391,    -1,   868,    -1,    -1,
      -1,   686,   687,    -1,    -1,    -1,    -1,   692,    -1,    -1,
      -1,   407,    -1,    -1,   410,    -1,    -1,    -1,    -1,    -1,
      -1,   417,   707,   708,    -1,    -1,   711,    -1,   713,    -1,
     715,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   928,    -1,    -1,
      -1,    -1,   933,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   945,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   956,    -1,    -1,    -1,    -1,
      -1,    -1,   963,    -1,    -1,   966,   967,   968,   969,   970,
     971,    -1,    -1,   974,    -1,   976,    -1,    -1,   979,    -1,
      -1,    -1,    -1,   984,    -1,   986,   987,   988,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   996,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   544,   545,
      -1,    -1,    -1,    -1,    -1,  1026,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   565,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   582,    -1,    -1,    -1,
      -1,    -1,    -1,  1064,    -1,    -1,   881,   882,   883,   884,
      -1,   886,   887,    -1,    -1,    -1,    -1,    -1,  1079,    -1,
    1081,   896,    -1,   898,   899,    -1,    -1,    -1,    -1,    -1,
      -1,   906,   907,    -1,    -1,     4,     5,  1098,  1099,    -1,
     626,    10,    -1,    -1,   630,    -1,    15,    -1,    17,    18,
      -1,    -1,    -1,    -1,    -1,    -1,    25,    -1,  1119,    28,
      -1,    -1,    -1,  1124,  1125,    -1,    35,    36,    -1,    38,
      -1,    40,    -1,    -1,    -1,    -1,    45,    -1,    47,    -1,
      -1,    50,    -1,    -1,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    -1,    -1,    64,    -1,    66,    67,    68,
      69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,    78,
      -1,    -1,    -1,    -1,    -1,    -1,    85,    86,    87,    -1,
      89,    90,    91,    92,    93,    94,    -1,    96,    97,    98,
      -1,  1192,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1036,    -1,    -1,    -1,    -1,  1041,  1042,  1229,  1044,
      -1,    -1,    -1,    -1,    -1,  1050,    -1,    -1,  1053,    -1,
      -1,  1242,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1251,    -1,  1253,  1254,    -1,    -1,    -1,   783,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   803,    -1,   805,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1299,    -1,
      -1,    -1,    -1,    -1,    -1,  1306,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   843,    -1,    -1,
     846,    -1,    -1,  1324,    -1,  1326,    -1,    -1,    -1,   855,
      -1,    -1,    -1,    -1,    -1,   861,    -1,    -1,   864,    -1,
    1341,    -1,    -1,    -1,    -1,    -1,    -1,  1162,  1163,   114,
     115,   116,   117,   118,   119,   120,    -1,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,    -1,    -1,   148,   149,   150,   151,   152,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1397,  1398,    -1,    -1,
      -1,    -1,    -1,  1218,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1232,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1443,    -1,  1445,  1446,    -1,    -1,    -1,    -1,
      -1,    -1,  1453,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1486,    -1,  1013,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1022,  1498,    -1,    -1,
      -1,  1316,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1345,    -1,    -1,    -1,    -1,  1350,    -1,    -1,    -1,    -1,
      -1,  1356,    -1,  1358,    -1,    -1,    -1,  1362,  1363,    -1,
    1365,    -1,  1367,    -1,    -1,  1370,  1371,    -1,  1373,    -1,
    1375,    -1,  1377,    -1,    -1,    -1,    -1,  1382,    -1,  1384,
    1385,    -1,  1387,    -1,  1389,  1390,  1102,  1392,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1127,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1440,  1441,    -1,    -1,  1444,
      -1,    -1,  1447,  1448,    -1,  1450,  1451,  1452,    -1,  1454,
      -1,  1456,  1457,    -1,    -1,    -1,  1172,    -1,    -1,  1175,
      -1,    -1,    -1,  1179,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1191,  1481,    -1,  1483,    -1,
    1485,    -1,  1487,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1495,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1233,  1234,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1247
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     4,     5,    10,    15,    17,    18,    25,    28,    35,
      36,    38,    40,    45,    47,    50,    53,    61,    64,    66,
      67,    68,    69,    77,    78,    85,    86,    87,    89,    90,
      91,    92,    93,    94,    96,    97,    98,   171,   172,   173,
     244,   246,   321,   322,   325,   327,   348,   349,   350,   351,
     352,   353,   356,   357,   359,   361,   369,   370,   371,   380,
     381,   392,   394,   405,   406,   407,   415,   416,   417,   418,
     419,   420,   421,   423,   424,   425,   426,   427,   428,   429,
     431,   453,   454,   455,   457,   459,   460,   466,   467,   473,
     478,   481,   484,   487,   488,   491,   496,   498,   501,   505,
     518,   522,   523,   528,   530,   531,     0,   165,   166,   208,
     208,   208,   208,   208,   169,   210,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   153,   211,   247,   211,   328,   211,
     355,   355,   211,   362,     6,    88,   365,   366,   211,   372,
     355,   211,   395,   355,   372,   355,   372,   355,   372,   355,
     372,   211,   468,   211,   492,   350,   351,   352,   353,   380,
     406,   407,   418,   423,   427,   453,   466,   473,   478,   481,
     484,   487,   497,   499,   500,   211,   506,   506,   506,    41,
     510,   511,   210,   210,   210,   210,   210,   208,   210,   210,
     210,   210,   210,   373,   210,   210,   422,   210,   422,   210,
     210,   430,   210,   210,   167,   168,   209,   210,   210,   502,
     503,   210,   519,   520,   210,   524,   525,   210,   208,   154,
     155,   157,   158,   159,   160,   161,   174,   175,   176,   178,
     180,   181,   184,   186,   187,   211,   248,    62,   329,   331,
      19,    29,    57,   322,   327,   338,   339,   358,   370,   388,
     389,   456,   458,   461,   463,   464,   465,   338,   358,   456,
     458,   178,   363,   211,   367,   359,   382,   383,   385,   386,
     387,   388,   389,     3,    26,   396,   397,    71,   325,   327,
     338,   408,   416,   474,   475,   476,   477,    23,    24,   244,
     374,   375,   377,   378,   209,    72,   420,   479,   480,   244,
     374,   209,    73,   425,   482,   483,   209,    74,   429,   485,
     486,    51,    52,   244,   432,   433,   435,   436,   209,    65,
     469,   471,   488,   489,   530,    75,    76,   493,   494,    79,
     507,   508,   510,   507,   510,   507,   510,    42,   512,   513,
     173,   186,   156,   174,   185,   178,   210,   245,   210,   323,
     326,   208,   208,   210,   208,   210,   208,   208,   210,   210,
     210,   210,   210,   210,   210,   210,   373,   210,   208,   208,
     210,   393,   208,   208,   210,   210,   210,   208,   210,   265,
     210,   208,   210,   208,   210,   210,   208,   265,   210,   210,
     210,   208,   210,   210,   208,   265,   265,   208,   210,   265,
     208,   210,   208,   210,   208,   210,   503,   208,   210,   210,
     520,   210,   210,   525,   210,   210,   208,   209,   179,   182,
     183,   187,   186,    33,    34,   175,   211,   249,   250,   251,
     253,   254,   209,    63,   329,   334,   335,   359,   209,   209,
     211,   332,   328,   370,    58,   340,   341,    16,   274,   275,
     277,   279,   280,   282,   354,   365,   211,   390,   390,    49,
     438,   440,   438,   370,   354,   438,   438,   178,   364,   177,
     179,   368,   209,   392,   390,   390,     9,   244,   399,   401,
     209,   211,   398,   328,   416,   282,   409,   438,   390,   244,
     244,   377,   211,   376,   244,   186,   379,   274,   438,   390,
     244,   274,   438,   390,   274,   438,   390,   186,   190,   192,
     211,   434,   432,   211,   437,   438,   390,   489,   186,   495,
     211,   509,   512,    26,   442,   443,   512,    48,   532,   536,
     173,   186,   210,   210,   208,   208,   323,   326,   210,   324,
     208,   210,   210,   330,   210,   210,   210,   343,   208,   208,
     209,   210,   210,   210,   210,   208,   210,   210,   209,   210,
     210,   210,   360,   210,   265,   178,   209,   210,   210,   210,
     265,   393,   208,   265,   210,   210,   208,   209,   210,   210,
     265,   265,   210,   209,   265,   174,   209,   209,   210,   210,
     265,   209,   210,   210,   209,   210,   210,   161,   163,   189,
     191,   195,   198,   209,   265,   209,   210,   210,   210,   490,
     174,   209,   209,   210,   210,   521,   208,   210,   210,   213,
     208,   209,   250,   253,   211,   252,   211,   255,   244,   337,
     175,   336,   334,   244,   333,   209,   329,   354,    59,    60,
     344,   346,   209,   186,   342,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   283,
     284,   285,   274,   338,   358,   338,   358,   274,   211,   441,
     274,   354,   274,   274,     7,    11,   244,   266,   270,   209,
       7,    12,   264,   269,   274,   338,   358,   338,   358,   211,
     402,   209,   329,   409,   283,   274,   338,   416,   244,   186,
     274,   420,   274,   425,   274,   429,   186,   193,   186,   199,
     274,   461,   463,   464,   465,   472,    27,    30,    31,    32,
      54,    55,    56,   214,   216,   217,   218,   219,   220,   221,
     222,   225,   226,   227,   229,   230,   235,   237,   240,   241,
     244,   256,   257,   489,   209,   186,   442,    39,    44,   214,
     401,   445,   450,   451,   516,   517,   209,   211,   444,    46,
     529,   214,   209,   506,   210,   209,   209,   324,   209,   324,
     330,   210,   209,   208,   174,   209,   210,   210,   210,   210,
     210,   210,   462,   210,   462,   209,   210,   209,   209,   209,
     209,   265,   265,   208,   265,   208,   209,   210,   210,   384,
     210,   384,   210,   210,   209,   210,   209,   210,   265,   265,
     209,   265,   209,   265,   209,   265,   191,   189,   162,   164,
     187,   188,   196,   199,   204,   205,   206,   209,   210,   210,
     208,   210,   215,   208,   210,   215,   208,   210,   215,   208,
     210,   215,   208,   210,   215,   208,   215,   208,   490,   210,
     504,   208,   521,   521,   208,   213,   208,   210,   345,   208,
     210,   533,   534,   253,   329,   211,   347,   186,   157,   158,
     159,   286,   286,   286,   286,   370,   244,   365,   209,   370,
     209,   442,   211,   272,   272,   370,   244,   264,   365,   391,
     209,   370,   209,   177,   403,   329,   286,   416,   209,   209,
     209,   209,   186,   194,   199,   186,   200,   188,   207,   392,
     218,   244,   256,   221,   225,   244,   256,   211,   223,   229,
     235,   240,   211,   228,   235,   240,   175,   231,   240,   175,
     238,   190,   211,   242,   153,   212,    43,   214,   445,   450,
     514,   515,   516,   209,   402,   402,   336,   244,   209,   392,
     438,   501,   522,   526,   442,   507,   345,   265,   265,   265,
     265,   462,   265,   265,   462,   210,   439,   210,   210,   384,
     265,   210,   265,   265,   384,   210,   400,   265,   265,   191,
     207,   188,   197,   204,   200,   210,   470,   215,   210,   215,
     209,   210,   215,   210,   215,   215,   209,   210,   215,   215,
     210,   215,   210,   210,   209,   504,   504,   210,   213,   208,
     213,   210,   210,   209,   209,   210,   535,   210,   534,   209,
     209,   209,   209,   209,   209,   209,   244,   445,   450,   209,
     178,   273,   273,   209,   391,   209,     7,    11,    12,    13,
     244,   262,   263,   404,   209,   209,   209,   186,   201,   202,
     214,   275,   282,   209,   224,   226,   229,   235,   240,   235,
     240,   240,   240,   175,   232,   175,   239,   190,   211,   243,
     516,   173,   403,   211,   452,   210,   214,   401,   450,   537,
     209,   210,   210,   210,   210,   265,   439,   439,   265,   265,
     265,   265,   208,   265,   210,   174,   204,   208,   209,   210,
     210,   210,   210,   215,   215,   215,   215,   210,   210,   236,
     209,   213,   209,   210,   446,   345,   529,   208,   535,   535,
       8,   288,   290,   287,   290,   288,   289,   290,   209,   209,
     272,   287,   186,   203,   204,   229,   235,   240,   235,   240,
     240,   240,   175,   233,   266,   209,     7,    11,    12,    13,
      14,    70,   244,   447,   448,   449,   209,   209,   208,   402,
     210,   278,   208,   210,   276,   208,   213,   210,   281,   208,
     210,   210,   410,   204,   210,   210,   210,   215,   210,   265,
     265,   208,   526,   210,     7,    11,    12,    13,    14,    70,
      95,   214,   258,   259,   260,   261,   267,   271,   320,   211,
     291,   214,   287,   320,   291,   214,   289,   291,   273,    37,
     214,   320,   412,   413,   235,   240,   240,   240,   175,   234,
     272,   209,   403,   208,   208,   210,   210,   276,   213,   210,
     281,   210,   265,   210,   213,   210,   411,   208,   210,   209,
     210,   268,   210,   527,   265,   272,   272,   214,   114,   115,
     116,   117,   118,   119,   120,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   148,
     149,   150,   151,   152,   304,   113,   121,   146,   147,   292,
     295,   304,   113,   121,   146,   147,   297,   300,   304,   209,
     412,   214,   412,   211,   414,   240,   273,   209,   214,   516,
     529,   209,   209,   210,   268,   210,   302,   210,   210,   210,
     210,   209,   210,   210,   210,   210,   210,   209,   210,   411,
     411,   345,   265,   213,   208,   273,   209,    22,    80,    81,
     244,   267,   303,   309,   310,   311,   313,   314,   315,   316,
     209,    20,   244,   259,   293,   305,   306,   309,   293,    21,
     244,   259,   294,   307,   308,   309,   294,   244,   259,   296,
     309,   311,   244,   298,   305,   309,   293,   244,   299,   307,
     309,   299,   244,   301,   309,   311,   209,   526,   265,   265,
     210,   312,   210,   312,   208,   210,   265,   208,   265,   208,
     265,   265,   210,   265,   208,   210,   265,   265,   265,   210,
     265,   208,   210,   265,   265,   312,   210,   312,   265,   210,
     265,   265,   265,   210,   265,   265,   265,   312,   209,   209,
     313,   315,   267,   186,   315,   175,   175,   259,   309,   175,
     259,   259,   309,   175,   259,   259,   309,   309,   527,   210,
     265,   265,   312,   209,   210,   265,   209,   209,   265,   210,
     265,   210,   265,   265,   210,   265,   209,   265,   312,   265,
     265,   315,    82,   317,   318,   259,   264,   259,   210,   265,
     265,   208,   265,   209,   265,   317,    83,    84,   319,   265,
     209
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int16 yyr1[] =
{
       0,   170,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   172,   172,   172,   172,   172,   172,   173,   173,
     174,   175,   175,   176,   177,   178,   178,   179,   179,   180,
     181,   182,   183,   184,   184,   185,   185,   186,   186,   186,
     186,   187,   187,   188,   189,   190,   190,   190,   191,   191,
     192,   193,   194,   195,   196,   196,   197,   197,   198,   199,
     200,   201,   201,   201,   202,   203,   204,   204,   205,   206,
     206,   207,   207,   208,   208,   209,   209,   210,   211,   212,
     213,   213,   214,   214,   214,   215,   215,   215,   216,   216,
     217,   217,   217,   218,   218,   218,   218,   219,   220,   221,
     222,   223,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   236,   237,   238,   239,   240,   241,   242,
     242,   243,   243,   244,   245,   245,   245,   245,   245,   245,
     245,   246,   247,   248,   248,   249,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   258,   259,   260,   261,
     261,   261,   261,   261,   262,   263,   263,   263,   263,   264,
     265,   265,   266,   267,   268,   268,   269,   269,   270,   270,
     271,   271,   272,   273,   274,   274,   274,   274,   275,   276,
     276,   276,   276,   277,   278,   278,   278,   278,   279,   280,
     281,   281,   281,   282,   283,   283,   283,   283,   283,   283,
     283,   283,   283,   284,   284,   285,   285,   286,   286,   286,
     287,   288,   289,   290,   291,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   293,   293,   293,   293,   293,   293,
     293,   293,   294,   294,   294,   294,   294,   294,   294,   294,
     295,   295,   296,   296,   296,   296,   296,   297,   297,   297,
     297,   297,   297,   297,   297,   297,   298,   298,   298,   298,
     299,   299,   299,   299,   300,   300,   301,   301,   301,   302,
     302,   303,   303,   303,   303,   303,   304,   304,   304,   304,
     304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
     304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
     304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
     304,   304,   305,   306,   307,   308,   309,   310,   311,   311,
     311,   311,   312,   312,   312,   312,   312,   313,   314,   315,
     316,   317,   318,   319,   319,   320,   321,   321,   322,   323,
     323,   324,   324,   325,   326,   326,   327,   328,   329,   330,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   342,   342,   342,   343,   343,   344,   345,   345,
     346,   346,   347,   348,   348,   348,   349,   349,   350,   351,
     352,   353,   354,   354,   355,   356,   356,   357,   357,   358,
     358,   359,   360,   360,   360,   361,   361,   362,   363,   364,
     365,   366,   366,   367,   368,   368,   369,   369,   370,   371,
     371,   371,   372,   373,   373,   373,   373,   373,   373,   373,
     373,   374,   375,   376,   377,   378,   379,   379,   379,   380,
     381,   381,   382,   382,   382,   382,   383,   384,   384,   384,
     384,   384,   385,   386,   387,   388,   389,   390,   391,   392,
     393,   393,   393,   394,   395,   396,   397,   397,   398,   399,
     400,   400,   400,   401,   402,   403,   404,   405,   405,   406,
     407,   408,   408,   409,   410,   410,   410,   410,   410,   411,
     411,   411,   412,   413,   414,   415,   415,   416,   417,   417,
     417,   418,   419,   419,   420,   421,   421,   422,   422,   422,
     422,   423,   424,   425,   426,   426,   427,   428,   429,   430,
     430,   430,   430,   430,   431,   431,   432,   433,   434,   434,
     435,   436,   437,   438,   439,   439,   439,   439,   440,   441,
     442,   443,   444,   445,   446,   446,   446,   447,   448,   449,
     449,   449,   449,   449,   449,   450,   451,   452,   453,   453,
     453,   454,   454,   455,   456,   456,   457,   458,   458,   459,
     460,   461,   462,   462,   462,   463,   464,   465,   466,   467,
     468,   469,   470,   470,   470,   471,   472,   472,   472,   472,
     473,   474,   474,   475,   476,   477,   478,   479,   480,   481,
     482,   483,   484,   485,   486,   487,   488,   488,   488,   488,
     488,   488,   488,   488,   488,   488,   488,   488,   489,   489,
     490,   490,   490,   491,   492,   493,   494,   494,   495,   495,
     495,   496,   497,   497,   498,   499,   499,   499,   499,   499,
     499,   499,   499,   499,   499,   499,   499,   499,   499,   500,
     500,   500,   500,   500,   500,   500,   501,   502,   502,   503,
     504,   504,   504,   504,   504,   504,   504,   505,   506,   507,
     508,   509,   510,   511,   512,   513,   514,   515,   516,   517,
     518,   519,   519,   520,   521,   521,   521,   521,   521,   522,
     523,   524,   524,   525,   526,   526,   526,   526,   527,   527,
     527,   527,   528,   529,   530,   531,   532,   533,   533,   534,
     535,   535,   535,   535,   536,   537
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
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
       1,     3,     2,     1,     2,     4,     2,     1,     1,     3,
       1,     3,     1,     3,     1,     3,     2,     4,     2,     2,
       2,     4,     2,     2,     1,     3,     2,     2,     1,     0,
       2,     2,     1,     2,     4,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     6,     1,     4,     1,     4,     1,     2,     2,
       4,     6,     0,     3,     3,     5,     7,     4,     1,     4,
       1,     4,     1,     1,     1,     1,     1,     1,     7,     5,
       3,     0,     3,     7,     3,     3,     1,     1,     5,     0,
       3,     1,     1,     1,     4,     1,     1,     1,     5,     1,
       4,     1,     1,     2,     3,     0,     2,     5,     0,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     8,    10,
       8,    10,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     9,     0,     3,     3,     1,     1,     1,     1,     1,
       6,     1,     1,     1,     4,     2,     1,     3,     7,     1,
       1,     1,     1,     0,     2,     2,     4,     3,     5,     5,
       7,     4,     1,     1,     4,     1,     1,     2,     3,    10,
       1,     1,     1,     1,     1,     1,     7,     0,     3,     5,
       3,     3,     9,     7,     9,     1,     1,     1,     1,     7,
       0,     3,     3,     1,     1,     5,     1,     1,     1,     7,
       0,     3,     3,     1,     1,     1,     1,     1,     1,     8,
      10,     1,     1,    10,     0,     3,     5,     3,     2,     0,
       3,     2,     5,     1,     1,     1,     1,     5,     1,     1,
       1,     8,     1,     1,     5,     1,     1,     0,     2,     3,
       5,     8,     1,     5,     1,     1,     8,     1,     5,     0,
       3,     5,     3,     3,     1,     1,     4,     1,     1,     1,
       4,     1,     1,     7,     0,     3,     3,     3,     1,     1,
       5,     1,     1,     7,     0,     3,     3,     1,     5,     1,
       1,     1,     1,     1,     1,     7,     1,     1,     1,     1,
       1,     1,     1,    10,     1,     1,    10,     1,     1,    10,
      10,     7,     0,     3,     3,     9,     7,     9,    10,     1,
       1,     9,     0,     2,     2,     1,     1,     1,     1,     1,
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
       3,     2,     1,     1,     9,     1,     4,     3,     1,     4,
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

#define YYBACKUP(Token, Value)                                    \
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


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, pj_wkt2_parse_context *context)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (context);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, pj_wkt2_parse_context *context)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep, context);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule, pj_wkt2_parse_context *context)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
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
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
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
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return (YYPTRDIFF_T)(yystpcpy (yyres, yystr) - yyres);
  else
    return yystrlen (yystr);
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
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

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
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
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
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
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
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
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
          ++yyp;
          ++yyformat;
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
    /* int yynerrs; */

    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

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
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
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
  /* yynerrs = 0; */
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = (YYPTRDIFF_T)(yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

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
      /* ++yynerrs; */
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
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

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


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
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
                  yystos[+*yyssp], yyvsp, context);
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
