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
    T_GEODETICCRS = 335,
    T_GEODETICDATUM = 336,
    T_PROJECTEDCRS = 337,
    T_PRIMEMERIDIAN = 338,
    T_GEOGRAPHICCRS = 339,
    T_TRF = 340,
    T_VERTICALCRS = 341,
    T_VERTICALDATUM = 342,
    T_VRF = 343,
    T_TIMEDATUM = 344,
    T_TEMPORALQUANTITY = 345,
    T_ENGINEERINGDATUM = 346,
    T_ENGINEERINGCRS = 347,
    T_PARAMETRICDATUM = 348,
    T_AFFINE = 349,
    T_CARTESIAN = 350,
    T_CYLINDRICAL = 351,
    T_ELLIPSOIDAL = 352,
    T_LINEAR = 353,
    T_PARAMETRIC = 354,
    T_POLAR = 355,
    T_SPHERICAL = 356,
    T_VERTICAL = 357,
    T_TEMPORAL = 358,
    T_TEMPORALCOUNT = 359,
    T_TEMPORALMEASURE = 360,
    T_ORDINAL = 361,
    T_TEMPORALDATETIME = 362,
    T_NORTH = 363,
    T_NORTHNORTHEAST = 364,
    T_NORTHEAST = 365,
    T_EASTNORTHEAST = 366,
    T_EAST = 367,
    T_EASTSOUTHEAST = 368,
    T_SOUTHEAST = 369,
    T_SOUTHSOUTHEAST = 370,
    T_SOUTH = 371,
    T_SOUTHSOUTHWEST = 372,
    T_SOUTHWEST = 373,
    T_WESTSOUTHWEST = 374,
    T_WEST = 375,
    T_WESTNORTHWEST = 376,
    T_NORTHWEST = 377,
    T_NORTHNORTHWEST = 378,
    T_UP = 379,
    T_DOWN = 380,
    T_GEOCENTRICX = 381,
    T_GEOCENTRICY = 382,
    T_GEOCENTRICZ = 383,
    T_COLUMNPOSITIVE = 384,
    T_COLUMNNEGATIVE = 385,
    T_ROWPOSITIVE = 386,
    T_ROWNEGATIVE = 387,
    T_DISPLAYRIGHT = 388,
    T_DISPLAYLEFT = 389,
    T_DISPLAYUP = 390,
    T_DISPLAYDOWN = 391,
    T_FORWARD = 392,
    T_AFT = 393,
    T_PORT = 394,
    T_STARBOARD = 395,
    T_CLOCKWISE = 396,
    T_COUNTERCLOCKWISE = 397,
    T_TOWARDS = 398,
    T_AWAYFROM = 399,
    T_FUTURE = 400,
    T_PAST = 401,
    T_UNSPECIFIED = 402,
    T_STRING = 403,
    T_UNSIGNED_INTEGER_DIFFERENT_ONE_TWO_THREE = 404
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
#define YYLAST   2796

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  165
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  358
/* YYNRULES -- Number of rules.  */
#define YYNRULES  712
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1455

#define YYUNDEFTOK  2
#define YYMAXUTOK   404


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
     161,   163,     2,   155,   164,   156,   150,     2,     2,   152,
     153,   154,     2,     2,     2,     2,     2,     2,   157,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   151,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   158,     2,     2,     2,     2,     2,
     159,   160,     2,   162,     2,     2,     2,     2,     2,     2,
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
     145,   146,   147,   148,   149
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   208,   208,   208,   208,   208,   208,   208,   209,   209,
     209,   210,   213,   213,   214,   214,   214,   215,   217,   217,
     221,   225,   225,   227,   229,   231,   231,   233,   233,   235,
     237,   239,   241,   243,   243,   245,   245,   247,   247,   247,
     247,   249,   249,   253,   255,   259,   260,   261,   263,   263,
     265,   267,   269,   271,   275,   276,   279,   280,   282,   284,
     286,   289,   290,   291,   293,   295,   297,   297,   299,   302,
     303,   305,   305,   310,   310,   312,   312,   314,   316,   318,
     322,   323,   326,   327,   328,   330,   330,   331,   334,   335,
     339,   340,   341,   345,   346,   347,   348,   350,   354,   356,
     359,   361,   364,   365,   366,   367,   368,   369,   370,   371,
     372,   373,   374,   375,   376,   377,   378,   381,   382,   383,
     384,   385,   386,   387,   388,   389,   390,   391,   392,   393,
     394,   395,   399,   401,   403,   407,   412,   414,   416,   418,
     420,   424,   429,   430,   432,   434,   436,   440,   444,   446,
     446,   448,   448,   453,   458,   459,   460,   461,   462,   463,
     464,   466,   468,   470,   470,   472,   472,   474,   476,   478,
     480,   482,   484,   488,   490,   494,   494,   497,   500,   505,
     505,   505,   505,   505,   508,   513,   513,   513,   513,   516,
     520,   521,   523,   539,   543,   544,   546,   546,   548,   548,
     554,   554,   556,   558,   565,   565,   565,   565,   567,   574,
     575,   576,   577,   579,   586,   587,   588,   589,   591,   598,
     605,   606,   607,   609,   611,   611,   611,   611,   611,   611,
     611,   611,   611,   613,   613,   615,   615,   617,   617,   617,
     619,   624,   630,   635,   638,   641,   642,   643,   644,   645,
     646,   647,   648,   649,   652,   653,   654,   655,   656,   657,
     658,   659,   662,   663,   664,   665,   666,   667,   668,   669,
     672,   673,   676,   677,   678,   679,   684,   685,   686,   687,
     688,   689,   690,   691,   692,   695,   696,   697,   698,   701,
     702,   703,   704,   707,   708,   711,   712,   717,   718,   721,
     722,   723,   724,   727,   728,   729,   730,   731,   732,   733,
     734,   735,   736,   737,   738,   739,   740,   741,   742,   743,
     744,   745,   746,   747,   748,   749,   750,   751,   752,   753,
     754,   755,   756,   757,   758,   759,   760,   761,   762,   764,
     767,   769,   771,   773,   775,   777,   793,   793,   795,   803,
     804,   806,   807,   809,   817,   818,   820,   822,   824,   829,
     830,   832,   834,   836,   838,   840,   842,   844,   849,   853,
     855,   858,   861,   862,   863,   865,   866,   868,   873,   874,
     876,   876,   878,   882,   882,   882,   884,   884,   886,   894,
     903,   911,   921,   922,   924,   926,   926,   928,   928,   931,
     932,   936,   942,   943,   944,   946,   946,   948,   950,   952,
     956,   961,   961,   963,   966,   967,   972,   973,   975,   980,
     980,   980,   982,   984,   985,   986,   987,   988,   989,   990,
     991,   993,   996,   998,  1000,  1003,  1005,  1005,  1005,  1009,
    1015,  1015,  1019,  1019,  1020,  1020,  1022,  1027,  1028,  1029,
    1030,  1031,  1033,  1039,  1044,  1050,  1052,  1054,  1056,  1060,
    1066,  1067,  1068,  1070,  1072,  1074,  1078,  1078,  1080,  1082,
    1087,  1088,  1089,  1091,  1093,  1095,  1097,  1101,  1101,  1103,
    1109,  1116,  1116,  1119,  1126,  1127,  1128,  1129,  1130,  1132,
    1133,  1134,  1136,  1140,  1142,  1144,  1144,  1148,  1153,  1153,
    1153,  1157,  1162,  1162,  1164,  1168,  1168,  1170,  1171,  1172,
    1173,  1177,  1182,  1184,  1188,  1188,  1192,  1197,  1199,  1203,
    1204,  1205,  1206,  1207,  1209,  1209,  1211,  1214,  1216,  1216,
    1218,  1220,  1222,  1226,  1232,  1233,  1234,  1235,  1237,  1239,
    1243,  1248,  1250,  1253,  1259,  1260,  1261,  1263,  1267,  1273,
    1273,  1273,  1273,  1273,  1273,  1277,  1282,  1284,  1289,  1289,
    1290,  1292,  1292,  1294,  1301,  1301,  1303,  1310,  1310,  1312,
    1319,  1326,  1331,  1332,  1333,  1335,  1341,  1346,  1354,  1360,
    1362,  1364,  1370,  1372,  1372,  1373,  1373,  1377,  1383,  1383,
    1385,  1390,  1396,  1401,  1407,  1412,  1417,  1423,  1428,  1433,
    1439,  1444,  1449,  1455,  1455,  1456,  1456,  1457,  1457,  1458,
    1458,  1459,  1459,  1460,  1460,  1463,  1463,  1465,  1466,  1467,
    1469,  1471,  1475,  1478,  1478,  1481,  1482,  1483,  1485,  1489,
    1490,  1492,  1494,  1494,  1495,  1495,  1496,  1496,  1496,  1497,
    1498,  1498,  1499,  1499,  1500,  1500,  1502,  1502,  1503,  1503,
    1504,  1505,  1505,  1509,  1513,  1514,  1517,  1522,  1523,  1524,
    1525,  1526,  1527,  1528,  1530,  1532,  1534,  1537,  1539,  1541,
    1543,  1545,  1547,  1549,  1551,  1553,  1555,  1560,  1564,  1565,
    1568,  1573,  1574,  1575,  1576,  1577,  1579,  1584,  1589,  1590,
    1593,  1599,  1599,  1599,  1599,  1601,  1602,  1603,  1604,  1606,
    1608,  1613,  1619,  1621,  1626,  1627,  1630,  1638,  1639,  1640,
    1641,  1643,  1645
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
      46,    69,    49,    50,    51,    43,    45,    58,    84,    90,
      91,    40,    93,    41,    44
};
# endif

#define YYPACT_NINF (-1262)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-653)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    2135, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262,
   -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262,
   -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262,
   -1262, -1262, -1262, -1262, -1262, -1262, -1262,    91, -1262, -1262,
   -1262,   274, -1262, -1262, -1262,   274, -1262, -1262, -1262, -1262,
   -1262, -1262,   274,   274, -1262,   274, -1262,   -44,   274, -1262,
     274, -1262,   274, -1262, -1262, -1262,   274, -1262,   274, -1262,
     274, -1262,   274, -1262,   274, -1262,   274, -1262,   274, -1262,
     274, -1262, -1262, -1262, -1262, -1262, -1262, -1262,   274, -1262,
   -1262, -1262, -1262, -1262, -1262,   274, -1262,   274, -1262,   274,
   -1262,   274, -1262,   274, -1262,   274, -1262, -1262, -1262,    46,
      46,    46,    46,    46, -1262,    79,    46,    46,    46,    46,
      46,    46,    46,    46,    46,    46,    46,    46,    46,   743,
      46,    46,    46,   224, -1262, -1262,   -44, -1262,   -44, -1262,
     -44,   -44, -1262,   -44, -1262, -1262, -1262,   274, -1262,   -44,
     -44, -1262,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,
     -44, -1262,   -44, -1262,   -44, -1262, -1262, -1262, -1262,    37,
   -1262, -1262, -1262, -1262, -1262,   154,   173,   180, -1262, -1262,
   -1262, -1262,   244, -1262,   -44, -1262,   -44,   -44,   -44, -1262,
     -44,   274,  1256,   210,   410,   410,   517,    46,   358,   292,
     108,   281,   437,   244,   275,   160,   244,   272,   244,   113,
     246,   244,   264,  1614, -1262, -1262, -1262,   417,   105, -1262,
   -1262,   105, -1262, -1262,   105, -1262, -1262,   349,   743, -1262,
   -1262, -1262, -1262, -1262, -1262, -1262,   352, -1262, -1262, -1262,
   -1262,   248,   311,   323,   517, -1262,   -44, -1262,   -44,   274,
   -1262, -1262, -1262, -1262,   274,   -44,   274,   -44, -1262,   274,
     274,   -44,   -44, -1262, -1262, -1262, -1262,   -44,   -44,   -44,
     -44, -1262,   -44, -1262,   -44,   -44,   -44, -1262, -1262, -1262,
   -1262,   274,   274, -1262, -1262,   -44,   274, -1262, -1262,   274,
     -44,   -44, -1262,   -44, -1262, -1262,   274, -1262, -1262,   -44,
     -44,   274,   -44,   274, -1262, -1262,   -44,   -44,   274,   -44,
     -44, -1262, -1262,   -44,   -44,   274, -1262, -1262,   -44,   -44,
     274, -1262, -1262,   -44,   -44,   274,   -44,   274, -1262, -1262,
     -44,   274, -1262,   -44, -1262, -1262, -1262, -1262,   274, -1262,
     -44,   274,   -44,   -44,   -44,   -44,   -44, -1262,   -44,   274,
     244, -1262,   373,   352, -1262, -1262,   479,   244,   287,   244,
     244,    46,    46,    69,   427,   143,    46,    46,   448,   448,
      69,   143,   448,   448,   517,   373,   244,   478,    46,    46,
     165,   244,    46,    46,   109,   502,   448,    46,   519, -1262,
      89,    46,   519,   352,   502,   448,    46, -1262,   519,   502,
     448,    46,   502,   448,    46, -1262, -1262,   566,   218, -1262,
      46,   448,    46,  1614,   352,   224, -1262,    46,   349,   224,
   -1262,   513,   224, -1262,   349,   494,   743, -1262,   352, -1262,
   -1262, -1262, -1262, -1262, -1262, -1262, -1262,   -44,   -44,   274,
   -1262,   274, -1262, -1262,   -44,   -44,   274,   -44, -1262, -1262,
   -1262,   -44,   -44,   -44, -1262,   -44,   274, -1262, -1262, -1262,
   -1262, -1262, -1262,   274,   244,   -44, -1262,   -44,   -44, -1262,
     -44,   274,   -44,   -44,   244,   -44,   -44, -1262,   -44,   -44,
     517,   244, -1262,   -44,   -44,   -44, -1262,   -44,   -44,   274,
   -1262, -1262,   -44,   -44,   -44,   274,   244,   -44,   -44,   -44,
     -44,   -44, -1262,   244,   -44,   323,   244,   244,   -44,   -44,
     -44,   244,   -44,   -44,   244,   -44,   -44, -1262, -1262,   316,
   -1262,   244,   -44, -1262,   244,   -44,   -44,   -44,   323,   244,
   -1262,   244,   -44, -1262,   -44,   274,   -44, -1262,   -44,   274,
     244, -1262,   532,   552,    46,    46, -1262, -1262,   519, -1262,
    1066,   506,   519,   244,   210,   143,   564,   244,   352,  1810,
   -1262,   502,   100,   100,   502,    46,   502,   143, -1262,   502,
     502,   297,   244,   536, -1262, -1262, -1262,   502,   100,   100,
   -1262, -1262,    46,   244,   210,   502,  1429, -1262,   502,    82,
   -1262, -1262,   519, -1262, -1262,   352, -1262, -1262,   502,    85,
   -1262, -1262,   502,   159, -1262,   502,    67, -1262, -1262,   352,
   -1262, -1262,   352, -1262, -1262, -1262,   502,   292,  1692,   244,
     352, -1262, -1262,   513,  1300,   244,    46,   545,  1270,   244,
      46, -1262,   -44, -1262, -1262,   244, -1262,   244, -1262,   -44,
   -1262,   244,   -44, -1262,   -44, -1262,   -44,   244, -1262, -1262,
   -1262,   274, -1262,   323,   244, -1262, -1262, -1262, -1262, -1262,
   -1262, -1262, -1262, -1262,   -44, -1262, -1262, -1262, -1262,   -44,
     -44,   -44, -1262,   -44,   -44,   -44,   -44,   244, -1262,   -44,
     244,   244,   244,   244, -1262, -1262,   -44,   -44,   274, -1262,
   -1262, -1262,   -44,   274,   244,   -44,   -44,   -44,   -44, -1262,
     -44, -1262,   -44,   244,   -44,   244,   -44,   -44,   -44, -1262,
     244,   -44,   244,   -44,   244,   -44,   361,   452, -1262,   378,
     244, -1262, -1262, -1262, -1262,   -44, -1262, -1262, -1262, -1262,
   -1262, -1262, -1262, -1262, -1262, -1262, -1262,   -44,   274,   -44,
     274, -1262,   -44,   274,   -44,   274,   -44,   274,   -44,   274,
     -44, -1262,   274,   -44, -1262, -1262,   -44, -1262, -1262, -1262,
     274,   -44,   -44,   274,   -44,   274, -1262, -1262,   -44, -1262,
     274, -1262, -1262,   -44,   552, -1262, -1262, -1262, -1262, -1262,
   -1262,   137, -1262,    46,   352, -1262,    26,    26,    26,    26,
      69,    72,   244,    69,   244, -1262,   513, -1262, -1262, -1262,
   -1262, -1262, -1262,    46, -1262,    46, -1262,    69,   131,   244,
      69,   244,   373,   574, -1262,    26, -1262,   109,   244, -1262,
   -1262,   244, -1262,   244, -1262,   244, -1262,   352, -1262, -1262,
     352,   352, -1262,   447, -1262, -1262, -1262, -1262,   478,   247,
     590,   799, -1262,    46,   544, -1262,    46,   467, -1262,  1066,
     213, -1262,  1066,   376, -1262,   566, -1262,   482, -1262,  1170,
     244,    46, -1262, -1262,    46, -1262,  1066,   519,   244,   146,
      68, -1262, -1262, -1262,   -44, -1262,   -44, -1262, -1262, -1262,
   -1262,   -44,   -44,   -44,   -44,   -44,   -44,   -44, -1262,   -44,
   -1262,   -44, -1262,   -44,   -44,   -44,   -44, -1262,   -44,   -44,
   -1262,   -44, -1262, -1262,   -44,   -44,   -44,   -44, -1262, -1262,
   -1262, -1262, -1262,   468,   447, -1262,   378,   352, -1262,   -44,
   -1262,   -44, -1262,   -44, -1262,   -44, -1262, -1262,   244,   -44,
     -44,   -44, -1262,   244,   -44,   -44, -1262,   -44,   -44, -1262,
     -44, -1262, -1262,   -44, -1262,   244, -1262, -1262,   -44,   -44,
     -44,   274,   -44, -1262,   -44,   -44,   244, -1262, -1262, -1262,
   -1262, -1262, -1262,   244,   -44,   -44,   244,   244,   244,   244,
     244,   244, -1262, -1262,   244,   151,   244,   517,   517,   244,
   -1262,   536, -1262, -1262,   244,   598,   244,   244,   244, -1262,
   -1262,   352, -1262, -1262, -1262,   244, -1262,   411, -1262, -1262,
     467, -1262,   213, -1262, -1262, -1262,   213, -1262, -1262,  1066,
   -1262,  1066,   566, -1262, -1262, -1262,  1236, -1262,   743, -1262,
     373,    46, -1262,   -44,   873,   244,   513, -1262, -1262,   -44,
     -44,   -44,   -44, -1262, -1262,   -44,   -44,   -44, -1262, -1262,
     -44,   -44, -1262,   -44, -1262, -1262, -1262, -1262, -1262,   -44,
   -1262,   274,   -44, -1262,   -44, -1262, -1262, -1262,   781, -1262,
     244,   -44,   -44,   -44, -1262,   -44,   -44,   -44,   -44, -1262,
     -44, -1262,   -44, -1262, -1262,   244,   -44,   244,   -44, -1262,
     -44,   545, -1262,   274,   -44,   -44, -1262,   634,   634,   634,
     634, -1262, -1262, -1262,   244,   244, -1262, -1262,    46, -1262,
     634,   986, -1262, -1262,   269,   597,   589,   213, -1262, -1262,
   -1262, -1262,  1066,   348,   244, -1262, -1262, -1262,   533,   244,
     244,   274,    46, -1262, -1262, -1262,   -44,   274,   -44,   274,
     -44,   -44,   274, -1262, -1262,   -44,   -44,   214,   986, -1262,
     -44,   -44, -1262,   -44, -1262, -1262,   -44, -1262,   -44, -1262,
   -1262, -1262, -1262, -1262, -1262, -1262, -1262,   -44,   -44, -1262,
     274, -1262, -1262,   146,   -44,  1539, -1262,    46,  1443, -1262,
      46, -1262,  1073, -1262,    46,   517,  1527, -1262, -1262,   597,
     589,   589, -1262,  1066, -1262, -1262,    46,   244,   373, -1262,
   -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262,
     274, -1262,   274,   -44, -1262,   -44, -1262,   -44,   -44,   -44,
   -1262,   -44,   -44,   -44, -1262, -1262,   -44,   -44,   274,   -44,
   -1262, -1262, -1262, -1262,   244,   -44,   -44,   -44,    46,    46,
    1270,  2585, -1262, -1262,  2448, -1262,  2649,   244,  1698, -1262,
    1698, -1262,    46,   589, -1262,   517,   244,  1371,   244,   244,
     -44,   -44, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262,
   -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262,
   -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262,
   -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262,   -44,
     -44,   -44,   -44,   -44,   244, -1262,   -44,   -44,   -44,   -44,
     -44,   244, -1262,   -44, -1262,   -44, -1262,   -44, -1262,   -44,
   -1262,   -44, -1262, -1262,   -44,   274, -1262, -1262,   517,   244,
     118,   244,   503,   503,   718,   718, -1262,   751,   535,   503,
     673,   673, -1262,   353, -1262, -1262,   244, -1262, -1262,   146,
     -44, -1262, -1262,   -44,   -44, -1262,   -44,   274, -1262, -1262,
     -44,   -44, -1262,   -44,   274,   -44, -1262, -1262,   -44,   -44,
   -1262,   -44,   274,   -44, -1262,   -44,   -44, -1262,   -44,   -44,
   -1262,   -44,   -44, -1262,   -44, -1262,   -44,   -44, -1262,   -44,
   -1262,   -44, -1262,   244,   244, -1262, -1262,    63, -1262,   352,
   -1262, -1262,   751, -1262,  1066,   551, -1262, -1262, -1262,   751,
   -1262,  1066,   551, -1262, -1262, -1262,   551, -1262, -1262,   353,
   -1262, -1262, -1262,   353, -1262, -1262, -1262, -1262,   -44, -1262,
     -44,   244,   -44,   -44,   -44,   -44,   -44,   -44,   244,   -44,
     -44,   -44,   -44, -1262, -1262, -1262, -1262,   551, -1262,   441,
   -1262, -1262,   551, -1262, -1262, -1262, -1262, -1262, -1262,   -44,
     244,   -44, -1262, -1262, -1262
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,   419,   406,   395,   405,   161,   440,   463,   397,   495,
     498,   620,   664,   699,   702,   524,   517,   356,   579,   505,
     502,   514,   512,   631,   686,   396,   421,   441,   398,   420,
     496,   500,   499,   525,   506,   503,   515,     0,     4,     5,
       2,     0,    13,   346,   347,     0,   603,   385,   383,   384,
     386,   387,     0,     0,     3,     0,    12,   416,     0,   605,
       0,    11,     0,   607,   477,   478,     0,    14,     0,   609,
       0,    15,     0,   611,     0,    16,     0,   613,     0,    17,
       0,   604,   560,   558,   559,   561,   562,   606,     0,   608,
     610,   612,   614,    19,    18,     0,     7,     0,     8,     0,
       9,     0,    10,     0,     6,     0,     1,    73,    74,     0,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    78,   162,     0,   357,     0,   394,
       0,     0,   407,     0,   411,   412,   417,     0,   422,     0,
       0,   464,     0,     0,   423,     0,   507,     0,   507,     0,
     519,   580,     0,   621,     0,   632,   646,   633,   647,   634,
     635,   649,   636,   637,   638,   639,   640,   641,   642,   643,
     644,   645,     0,   629,     0,   665,     0,     0,     0,   670,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    75,    76,   628,     0,     0,   653,
     655,     0,   677,   679,     0,   687,   689,     0,     0,    40,
      20,    37,    38,    39,    41,    42,     0,   163,    21,    22,
      26,     0,    25,    35,     0,   164,   154,   361,     0,     0,
     455,   456,   369,   400,     0,     0,     0,     0,   399,     0,
       0,     0,     0,   564,   567,   565,   568,     0,     0,     0,
       0,   408,     0,   413,     0,   423,     0,   442,   443,   444,
     445,     0,     0,   467,   466,   460,     0,   592,   482,     0,
       0,     0,   481,     0,   588,   589,     0,   432,   435,   190,
     424,     0,   425,     0,   497,   595,     0,     0,     0,   190,
     508,   504,   598,     0,     0,     0,   513,   601,     0,     0,
       0,   531,   527,   190,   190,     0,   190,     0,   518,   582,
       0,     0,   615,     0,   616,   623,   624,   630,     0,   667,
       0,     0,     0,     0,     0,     0,     0,   672,     0,     0,
       0,    34,    27,     0,    33,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    27,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   427,
       0,     0,     0,     0,     0,     0,     0,   509,     0,     0,
       0,     0,     0,     0,     0,   523,   522,     0,     0,   520,
       0,     0,     0,     0,     0,     0,   654,     0,     0,     0,
     678,     0,     0,   688,     0,     0,     0,   669,     0,    29,
      31,    28,    36,   168,   171,   165,   166,   155,   158,     0,
     160,     0,   153,   365,     0,   351,     0,     0,   348,   353,
     362,   359,     0,     0,   371,   375,     0,   223,   393,   204,
     205,   206,   207,     0,     0,     0,   457,     0,     0,   538,
       0,     0,     0,     0,     0,     0,     0,   409,   402,   190,
       0,     0,   418,     0,     0,     0,   473,   190,   460,     0,
     459,   468,   190,     0,     0,     0,     0,     0,     0,   190,
     190,   426,   433,     0,   190,   436,     0,     0,     0,     0,
     190,     0,     0,     0,     0,     0,     0,    50,   528,    48,
     529,     0,   190,   532,     0,     0,     0,   617,   625,     0,
     668,     0,     0,   541,   681,     0,     0,   711,    80,     0,
       0,    32,     0,     0,     0,     0,   350,   355,     0,   354,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     388,     0,     0,     0,     0,     0,     0,     0,   390,     0,
       0,     0,     0,     0,   415,    24,   410,     0,     0,     0,
     461,   462,     0,     0,     0,     0,     0,   479,     0,     0,
     191,   429,     0,   431,   428,   437,   434,   501,     0,     0,
     510,   511,     0,     0,   516,     0,     0,    44,    58,     0,
      45,    49,     0,   526,   521,   530,     0,     0,     0,     0,
     626,   622,   666,     0,     0,     0,     0,     0,     0,     0,
       0,   671,   156,   159,   169,     0,   172,     0,   367,   351,
     366,     0,   351,   363,   359,   358,     0,     0,   380,   381,
     376,     0,   368,   372,     0,   224,   225,   226,   227,   228,
     229,   230,   231,   232,     0,   233,   234,   235,   236,     0,
       0,     0,   392,     0,   572,     0,   572,     0,   539,     0,
       0,     0,     0,     0,   199,   198,   190,   190,     0,   401,
     197,   196,   190,     0,     0,     0,   447,     0,   447,   474,
       0,   465,     0,     0,     0,     0,     0,   190,   190,   438,
       0,   190,     0,   190,     0,   190,    48,     0,    59,     0,
       0,   583,   584,   585,   586,     0,   174,   100,   133,   136,
     144,   148,    98,   619,    82,    88,    89,    93,     0,    85,
       0,    92,    85,     0,    85,     0,    85,     0,    85,     0,
      85,    84,     0,   617,   602,   627,   657,   556,   676,   685,
       0,   681,   681,     0,    80,     0,   680,   542,   378,   700,
       0,    81,   701,     0,     0,   167,   170,   352,   364,   349,
     360,     0,   389,     0,   373,   370,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   563,     0,   566,   391,   569,
     570,   404,   403,     0,   414,     0,   439,     0,     0,     0,
       0,     0,    27,     0,   480,     0,   587,     0,     0,   430,
     593,     0,   596,     0,   599,     0,    46,     0,    43,    68,
       0,     0,    53,    71,    55,    66,    67,   578,     0,     0,
       0,     0,    91,     0,     0,   117,     0,     0,   118,     0,
       0,   119,     0,     0,   120,     0,    83,     0,   618,     0,
       0,     0,   682,   683,     0,   684,     0,     0,     0,     0,
       0,   703,   705,   157,     0,   382,   378,   374,   237,   238,
     239,   190,   190,   190,   190,   572,   190,   190,   571,   572,
     576,   534,   202,     0,     0,   447,   190,   458,   190,   190,
     446,   447,   453,   475,   470,     0,   190,   190,   590,   594,
     597,   600,    52,    48,    71,    60,     0,     0,    70,   190,
      96,    85,    94,     0,    90,    85,    87,   101,     0,    85,
      85,    85,   134,     0,    85,    85,   137,     0,    85,   145,
       0,   149,   150,     0,    79,     0,   674,   663,   657,   657,
      80,     0,    80,   656,     0,     0,     0,   379,   540,   693,
     694,   691,   692,     0,   707,     0,     0,     0,     0,     0,
       0,     0,   574,   573,     0,     0,     0,     0,     0,     0,
     451,     0,   448,   450,     0,     0,     0,     0,     0,    47,
      69,     0,    54,    57,    72,     0,    95,     0,    86,    99,
       0,   121,     0,   122,   123,   132,     0,   124,   125,     0,
     126,     0,     0,   173,   658,   659,     0,   660,     0,   662,
      27,     0,   675,     0,     0,     0,     0,   704,   377,     0,
       0,     0,     0,   575,   577,   190,   534,   534,   533,   203,
     190,   190,   452,   190,   454,   188,   186,   185,   187,   190,
     476,     0,   190,   469,     0,   591,    64,    56,     0,   581,
       0,   102,   103,   104,   105,    85,    85,    85,    85,   138,
       0,   146,   142,   151,   152,     0,    80,     0,   544,   557,
     378,     0,   710,     0,   707,   707,   706,     0,     0,     0,
       0,   537,   535,   536,     0,     0,   449,   471,     0,   472,
       0,     0,    63,    97,     0,     0,     0,     0,   127,   128,
     129,   130,     0,     0,     0,   147,   661,   673,     0,     0,
       0,     0,     0,   709,   708,   243,   214,     0,   209,     0,
      80,   220,     0,   192,   189,     0,   484,    65,     0,    61,
     106,   107,   108,   109,   110,   111,    85,   139,     0,   143,
     141,   554,   549,   550,   551,   552,   553,   190,   190,   547,
       0,   543,   555,     0,     0,     0,   213,     0,     0,   208,
       0,   218,     0,   219,     0,     0,     0,   483,    62,     0,
       0,     0,   131,     0,   546,   545,     0,     0,    27,   183,
     180,   179,   182,   200,   181,   201,   217,   345,   175,   177,
       0,   176,     0,   215,   244,     0,   212,   209,    80,     0,
     222,   220,     0,   190,   493,   488,    80,   489,     0,   112,
     113,   114,   115,   140,     0,   194,   695,   190,     0,     0,
       0,     0,   211,   210,     0,   221,     0,     0,     0,   485,
       0,   487,     0,     0,   135,     0,     0,     0,     0,     0,
       0,   194,   216,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   297,
     246,   248,   250,   252,     0,   245,   270,   277,   279,   281,
     283,     0,   276,   293,   184,   489,   491,   489,   494,   378,
     116,   190,   548,   698,    80,     0,   690,   712,     0,     0,
       0,     0,     0,     0,     0,     0,   240,     0,     0,     0,
       0,     0,   242,     0,   486,   490,     0,   195,   697,     0,
     190,   193,   344,   190,   190,   298,   190,     0,   241,   340,
     190,   190,   247,   190,     0,   190,   249,   342,   190,   190,
     251,   190,     0,   190,   253,   190,   190,   271,   190,   190,
     278,   190,   190,   280,   190,   282,   190,   190,   284,   190,
     294,   190,   492,     0,     0,   299,   302,     0,   300,     0,
     254,   261,     0,   258,     0,     0,   260,   262,   269,     0,
     266,     0,     0,   268,   272,   275,     0,   273,   285,     0,
     287,   288,   289,     0,   291,   292,   295,   296,   695,   178,
     190,     0,   190,   190,     0,   190,   190,   190,     0,   190,
     190,   190,   190,   696,   301,   343,   257,     0,   255,     0,
     259,   265,     0,   263,   341,   267,   274,   286,   290,   190,
       0,   190,   256,   339,   264
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1262, -1262, -1262,  -223,  -215,  -189, -1262,   271,  -186,   304,
   -1262, -1262, -1262, -1262, -1262, -1262,   -71,  -336,  -677,   -58,
    -803,  -665, -1262, -1262, -1262, -1262, -1262, -1262, -1262,  -579,
    -256, -1262, -1262, -1262,  -894, -1262, -1262,  -250,  1319,  1306,
     -57,   -56, -1262,  -730,  -575,   115, -1262, -1262,  -166, -1262,
   -1262,  -154, -1262, -1262, -1262,  -153,  -294, -1262, -1262,  -824,
   -1262, -1262, -1262, -1262, -1262,  -672, -1262, -1262, -1262, -1262,
     341, -1262, -1262, -1262,   110, -1262, -1262, -1262, -1262, -1262,
     166, -1262, -1262,  -516, -1262, -1262,  -725, -1262, -1262,  -341,
   -1262, -1262, -1262, -1262,  -572,   984,  -403, -1261,  -539, -1262,
   -1262, -1262,  -782,  -946,   -10, -1262,  -486, -1262, -1262, -1262,
   -1262,  -485,  -354,   150, -1262, -1262,  -455, -1010,  -348,  -427,
    -966,  -812, -1262,  -866,  -582, -1262, -1262, -1262, -1262,  -580,
   -1262, -1262, -1262, -1262,  -941,  -576, -1262,  -633, -1262,  -414,
   -1262,  -897, -1262,   754,  -429,  -222,   554,  -425,    45,  -275,
    -317,   122, -1262, -1262, -1262,   206, -1262,   -99, -1262,  -177,
   -1262, -1262, -1262, -1262, -1262, -1262,  -859, -1262, -1262, -1262,
   -1262,   640,   641,   643,   645,  -289,   625, -1262, -1262,   -80,
      40, -1262, -1262, -1262, -1262, -1262,  -108, -1262, -1262, -1262,
   -1262,     4, -1262,   457,   500,   571, -1262, -1262,   387, -1262,
   -1262,   650, -1262, -1262, -1262,  -654, -1262, -1262, -1262,   581,
     583,   -48,  -198,     2,   298, -1262, -1262, -1262, -1262, -1262,
   -1262, -1262,  -374,  -826,  -972, -1262, -1262,   656,   660, -1262,
     208, -1262,  -711,  -638, -1262, -1262, -1262,  -192, -1262,   669,
   -1262,  -190, -1262,   642,   672, -1262,  -178, -1262,   674, -1262,
    -183, -1262, -1262,   396, -1262, -1262, -1262, -1262, -1262,   306,
    -383, -1262, -1262,  -391, -1262, -1262,  -809, -1262, -1262, -1262,
   -1262,  -715, -1262, -1262,   677, -1262, -1262,   614, -1262,   618,
   -1262, -1262,   198,  -630,   201,   207,   216,   710, -1262, -1262,
   -1262, -1262, -1262,   713, -1262, -1262, -1262, -1262,   720, -1262,
   -1262,   721, -1262, -1262,   722, -1262, -1262,   726,  -188,  -376,
     103, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262, -1262,
   -1262,   858, -1262,   520,  -272, -1262,  -119,  -213, -1262, -1262,
    -112, -1262,   148, -1262, -1262, -1262,  -823, -1262, -1262, -1262,
     526,   -38,   862, -1262, -1262,   525, -1116,  -547, -1262,  -993,
     875, -1262, -1262, -1262,   -89,  -352, -1262, -1262
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   236,   640,   238,   903,   239,   480,
     240,   241,   429,   430,   242,   354,   243,   244,   917,   609,
     518,   610,   519,   717,   913,   611,   832,   992,   612,   833,
     916,  1057,  1058,  1138,   834,   835,   836,   918,   109,   216,
     388,   466,   945,   629,   771,   842,   734,   735,   736,   737,
     738,   739,   740,   928,  1060,   741,   742,   743,   933,   744,
     745,   937,  1070,  1148,  1224,   746,  1114,   747,   940,  1072,
     748,   749,   943,  1075,   499,   357,    41,   136,   246,   437,
     438,   439,   635,   440,   441,   637,   751,   752,  1197,  1198,
    1199,  1200,  1050,  1051,   897,   389,   687,  1201,  1246,   693,
     688,  1202,   893,  1040,   458,   459,  1169,   460,  1166,   461,
     462,  1173,   463,   669,   670,   671,   881,  1128,  1126,  1131,
    1129,  1205,  1294,  1352,  1360,  1295,  1367,  1301,  1370,  1375,
    1302,  1380,  1321,  1345,  1289,  1353,  1354,  1361,  1362,  1355,
    1347,  1203,    42,   253,   359,   549,    44,   360,   254,   138,
     248,   553,   249,   451,   644,   445,   446,   641,   639,   255,
     256,   455,   456,   654,   557,   650,   868,   651,   876,    46,
      47,    48,    49,    50,    51,   464,   140,    52,    53,   257,
     447,   572,    55,   143,   272,   478,   465,   147,   274,   481,
      56,   258,    58,   149,   203,   300,   301,   503,   302,   303,
     506,    59,    60,   276,   277,   809,   278,   279,   280,   259,
     260,   467,   899,   959,   381,    62,   152,   285,   286,   492,
     488,   986,   760,   700,   904,  1052,    63,    64,    65,   291,
     496,  1177,  1241,  1217,  1218,  1309,    66,    67,    68,    69,
      70,    71,    72,   206,    73,    74,    75,    76,    77,    78,
      79,   211,    80,   324,   325,   521,   326,   327,   524,   960,
     976,   471,   679,   964,   535,   768,   761,  1119,  1158,  1159,
    1160,   762,   763,  1080,    81,    82,    83,   261,    84,   262,
      85,    86,   263,   792,   264,   265,   266,    87,    88,   162,
     330,   331,   725,    89,   293,   294,   295,   296,    90,   307,
     308,    91,   314,   315,    92,   319,   320,    93,    94,   333,
     619,    95,   164,   337,   338,   529,    96,   182,    97,   183,
     184,   961,   219,   220,   860,    99,   186,   340,   341,   531,
     342,   191,   348,   349,   950,   951,   764,   765,   100,   222,
     223,   625,   962,   102,   225,   226,   963,  1248,   103,   770,
     334,   105,   538,   871,   872,  1025,   539,  1085
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     115,   692,    61,   237,    57,   350,   489,   146,   343,   292,
     271,   345,   187,   188,   306,   546,   431,   966,   267,   547,
     929,   190,   993,   894,   290,   332,   318,   633,   353,   313,
     534,   495,  1041,   719,   865,   954,   952,   527,   955,   431,
      54,   444,   831,   733,   811,    45,   794,  1187,  1078,   759,
     948,   826,   941,   135,   137,   139,   139,   142,   355,  1344,
     148,   139,   151,   139,   148,   139,   148,   139,   148,   139,
     148,   161,   163,     1,   185,   185,   185,  1193,   144,   192,
       5,   193,   474,   194,   195,   144,   196,   452,  1121,     5,
    1136,   106,   198,   199,   533,   200,   201,   202,   204,   205,
     207,   205,   209,   210,     1,   212,     5,   213,   493,   344,
      40,   283,   346,   298,   922,   268,   926,    15,    10,   926,
     114,  1127,   926,  1127,  1132,   926,  1420,   217,   926,   218,
     221,   224,  1193,   227,   284,     5,   245,   144,   690,   252,
    1342,   273,     2,   691,   949,    10,   189,   339,     5,   144,
      26,    19,     4,  1195,    29,   145,    33,   252,  1207,   457,
     486,    17,   145,    15,  1102,   351,  1036,   435,     5,    31,
      32,     7,   930,  1062,   486,   934,    34,     5,   878,   879,
     880,    26,     5,   297,   339,    29,    12,   317,   477,   356,
     757,   358,   494,  1076,   134,   469,    31,    32,   363,   247,
     365,  -648,    33,   540,   368,   369,  1132,  1139,  1195,  1073,
     370,   371,   372,   373,   145,   374,  1227,   375,   202,   377,
    1017,  1120,  1019,  1383,    24,   332,   145,    21,   380,  1213,
       5,   495,   756,   384,   385,     5,   386,   646,   275,   991,
     726,   979,   753,   390,  1178,   392,   289,   984,   989,   394,
     395,   914,    36,   398,  1315,   971,   399,   400,   873,   974,
    1037,   402,   403,     5,     5,   189,   647,   702,   731,   408,
     322,  1208,   247,   411,   726,   926,   413,   926,   681,  1216,
    1140,   926,   432,   415,   947,   418,   419,   421,   422,   424,
     595,   425,     2,  1296,   575,  1303,  1164,   321,   322,  1311,
     436,   729,     4,   732,   684,   450,   137,   344,   685,  1084,
     346,   250,   299,   620,     5,   309,  1135,    10,  -650,   468,
     323,   251,   505,   730,   731,  1063,   491,   137,  1065,   329,
     484,   485,   882,   883,   884,   502,   517,  -651,   252,   498,
      21,    19,    17,   528,  -652,   312,  1116,   305,   509,   247,
     443,   520,   287,   513,   523,   684,   516,   541,  1209,   685,
     906,   530,  1212,     2,   526,    36,    34,   453,    31,    32,
       5,   -59,  1340,     4,   473,  1342,   -59,   -59,   -59,   483,
     542,   543,   926,   830,   507,   673,   675,   358,   548,   511,
     551,   347,   514,     5,   552,   554,   555,   707,   556,   352,
    1171,   695,   697,   726,  1225,   891,   214,   215,   561,   711,
     562,   563,   706,   564,     1,   566,   567,   777,   569,   570,
     779,   571,   573,   715,  1314,   713,   577,   578,   579,   250,
     332,   380,  1141,  1143,   107,   108,   584,   585,   784,   251,
     588,   589,   728,   729,   592,   546,  1250,  1251,   690,  1082,
    1336,   598,   599,   691,     5,   602,   603,  1356,   605,   606,
     297,   298,   -30,  1373,   874,   730,   731,   252,   616,   617,
     618,    17,   607,   230,   608,   623,   431,   624,  1233,   627,
     547,   628,   674,   676,     5,   454,  1239,   653,   634,   636,
     487,    26,   335,   336,   726,    29,   905,   469,   696,   698,
     500,   229,   504,     7,   231,   232,   233,  1219,   510,   678,
    1189,   773,   433,   434,  1190,  1191,  1192,   -51,   457,   608,
       5,   730,   731,  1349,   709,  1342,   699,   229,   234,   235,
     231,   232,   233,   234,   235,   828,     5,   829,   716,   533,
    1151,   718,   537,   690,  1152,  1153,  1154,  1155,   691,   755,
       5,   672,     5,     5,   677,  1349,   680,  1342,  1189,   682,
     683,     5,  1190,  1191,  1192,   433,   532,   694,     5,   443,
     767,   726,   536,  1194,   185,   774,   729,   154,   705,   156,
     830,   158,   548,   160,  1338,   548,   434,   552,   710,   781,
    1196,   769,   712,  1206,  1334,   714,  1335,  1210,   730,   731,
    1305,  1215,  1307,  1156,   828,  1045,   720,   786,   607,  1046,
    1047,  1048,   787,   788,   789,     5,   790,   791,   793,   791,
     727,  1194,   796,   648,   649,   907,   608,   134,   229,   230,
     944,   231,   232,   233,   234,   235,   247,   443,   807,   808,
     810,   808,  1125,   812,   731,   813,   479,   815,   718,   817,
    1083,   730,   731,  1092,  1093,  1252,   428,   965,   638,   827,
     936,   994,   643,   939,   990,  1306,   229,   230,   838,   231,
     232,   233,  1313,   920,   470,   472,  1014,  1015,   475,   476,
     839,   686,   841,   887,   431,   844,   923,   847,   924,   850,
       5,   853,   497,   853,  1357,  1342,   618,  1376,  1376,   859,
     898,   508,   708,  1061,   624,   624,   512,   628,   632,   515,
    1149,   867,  1319,   877,   134,   229,   870,   525,   231,   232,
     233,  1232,   830,   862,   863,  1189,  1235,   875,   750,  1190,
    1191,  1192,  1123,  1124,   750,     5,   704,   141,   750,  1357,
    1342,  1130,   150,  1364,   153,  1211,   155,   892,   157,   892,
     159,  1378,  1371,     3,    43,   288,   912,   642,  1189,   718,
     915,     6,  1190,  1191,  1192,   830,   780,   956,     5,   165,
     166,     8,   167,  1342,   168,   376,   310,   501,     9,   169,
     281,    11,   282,  1043,   517,   170,   581,   927,  1194,   171,
     932,  1039,  1039,   703,   885,  1077,    16,   889,   172,   942,
     208,   173,   830,   174,   522,   699,   175,    18,   699,   269,
      20,   895,    22,   270,   901,   721,     5,   781,   722,   867,
    1069,  1194,  1071,    25,   723,    27,   726,    28,   791,    30,
     728,   729,   791,   724,   975,    35,   977,   978,   808,   176,
     919,   981,   177,  1101,   808,   718,   915,   985,   813,   178,
     179,   180,   431,   730,   731,   181,   858,   845,    98,   848,
     416,   851,   101,   854,   853,   856,   997,  1450,   853,   420,
     423,  1433,  1000,  1002,   853,   104,  1027,  1006,   853,     0,
    1009,   853,   486,  1011,     0,     0,  1012,     0,     0,     0,
       5,   859,   859,  1016,     0,   628,     0,  1020,  1021,     0,
     726,   886,     0,   727,   728,   729,  1346,  1024,  1026,     0,
    1363,  1363,   757,  1368,  1372,     0,  1377,  1377,   896,  1381,
    1056,     0,     0,  1147,     0,     0,     0,   730,   731,   732,
     229,   230,     0,   231,   232,   233,   234,   235,     0,     0,
     829,   517,     0,     0,     0,     0,     0,     0,     0,   921,
       0,   925,     0,     0,   925,     0,  1074,   925,     0,     0,
     925,     0,     0,   925,     0,  1079,  1081,     0,     0,   750,
       0,     0,  1087,  1088,  1089,  1090,     0,   957,  1423,   975,
     975,  1351,  1351,  1359,  1359,  1427,  1366,   718,  1351,  1039,
       0,     0,     0,     0,  1223,  1431,     0,  1100,     0,  1432,
       0,     0,     0,     0,  1104,  1105,  1106,     0,  1107,   853,
     853,   853,     0,  1112,     0,  1113,     0,     0,     0,   628,
       0,  1118,     0,   867,     0,     0,     0,  1024,  1024,     0,
    1137,     0,     0,     0,     0,     0,   996,     0,     0,     0,
     998,     0,   892,     0,  1001,  1003,  1004,     0,     0,  1007,
    1008,  1422,     0,  1010,  1425,     0,     0,     0,  1426,  1039,
       0,  1429,     0,     0,     0,  1430,   699,   718,     0,  1165,
       0,  1168,     0,   628,  1172,     0,     0,     0,  1175,  1176,
       0,  1125,     0,  1179,  1180,  1035,  1181,     0,     0,   853,
       5,  1183,     0,     0,     0,  1049,  1449,     0,     0,     0,
     726,  1451,     0,   727,   728,   729,     0,  1188,     0,     0,
     925,  1204,   925,     0,  1204,     0,   925,     0,  1204,     0,
       0,     0,     0,     0,     0,     0,   750,   730,   731,   732,
     892,     0,  1039,     0,   750,   229,     0,     0,   231,   232,
     233,   234,   235,     0,     0,   829,  1230,     0,  1231,     0,
    1168,   628,  1234,     0,  1172,  1236,     0,     0,     0,  1238,
    1240,     0,  1243,     0,     0,     0,     0,     0,  1245,  1247,
       0,     0,   892,   892,     0,     0,     0,     0,     0,   486,
    1108,  1109,  1110,  1111,     0,   931,  1308,     5,   935,     0,
       0,   938,     0,  1318,  1245,     0,     0,   726,     0,     0,
     727,   728,   729,     0,     0,  1424,     0,     0,     0,   757,
       0,     0,  1428,   946,   758,   229,   230,   925,   231,   232,
     233,   234,   235,     0,   730,   731,   732,     0,  1157,     0,
       0,     0,  1320,  1322,  1323,  1324,  1325,     0,     0,  1327,
    1328,  1329,  1330,  1331,     0,     0,  1333,     0,  1240,     0,
    1240,     0,   867,     5,     0,     0,     0,   628,     0,     0,
       0,  1182,     0,   726,     0,     0,   727,   728,   729,     0,
       0,     0,     0,     0,     0,   750,     0,     0,   750,     0,
     758,     0,   750,     0,     0,     0,   750,     5,     0,  1387,
     730,   731,   732,   397,     0,     0,  1392,   726,  1395,     0,
     727,   728,   729,     0,  1399,     0,  1402,   405,   406,   486,
     409,  1406,     0,     0,  1409,     0,     0,     5,  1421,  1413,
       0,     0,     0,     0,   730,   731,   732,   726,     0,     0,
     727,   728,   729,     0,     0,     0,     0,     0,  1064,   757,
     750,  1066,     0,  1067,   758,     0,     0,  1068,   750,     0,
     750,     0,     0,     0,   730,   731,   732,   750,     0,     0,
       0,  1247,     0,     0,   110,     0,  1437,  1439,     0,     0,
    1442,   111,   112,     0,   113,     0,     0,   116,     0,   117,
       0,   118,     0,     0,     0,   119,     0,   120,     5,   121,
       0,   122,     0,   123,     0,   124,     0,   125,   726,   126,
       0,   727,   728,   729,   134,   229,   230,   127,   231,   232,
     233,   234,   235,     0,   128,   758,   129,   769,   130,     0,
     131,     0,   132,     0,   133,   730,   731,   732,     0,     0,
    1343,     0,  1350,  1350,  1358,  1358,     0,  1365,  1369,  1350,
    1374,  1374,     0,  1379,     0,  1142,  1144,  1145,  1146,     0,
    1189,  1125,     0,     0,  1190,  1191,  1192,  1193,     0,     0,
       5,     0,     0,   574,     0,     0,   197,     0,     0,     0,
     726,   580,     0,   727,   728,   729,   583,     0,     0,     0,
       0,     0,     0,   590,   591,     0,     0,     0,   594,     0,
       0,     0,     0,     0,   600,     0,     0,   730,   731,   732,
       0,     0,     0,     0,     0,     0,   614,     0,     0,   304,
     228,     0,   311,  1194,   316,     0,     0,   328,     0,     0,
    1220,  1221,  1222,   655,   656,   657,   658,   659,   660,   661,
     662,   663,     0,  1195,  1189,     0,     0,     0,  1190,  1191,
    1192,  1193,     0,     0,     5,     0,  1189,     0,     0,     0,
    1190,  1191,  1192,  1193,   726,     0,     5,   727,   728,   729,
       0,     0,     0,     0,  1214,     0,   726,     0,   361,   727,
     728,   729,     0,   362,     0,   364,     0,     0,   366,   367,
       0,   730,   731,   732,  1310,     0,     0,     0,     0,     0,
       0,     0,     0,   730,   731,   732,     0,  1194,     0,     0,
     378,   379,     0,     0,     0,   382,     0,     0,   383,  1194,
       0,     0,     0,     0,     0,   387,     0,  1195,     0,     0,
     391,     0,   393,     0,     3,     0,     0,   396,     0,  1195,
       0,     0,     6,     0,   401,     0,     0,     0,     0,   404,
       0,     0,     8,     0,   407,     0,   410,     0,     0,     9,
     412,     0,     0,     0,     0,     0,   427,   414,     0,     0,
     417,    14,     0,   442,     0,   448,   449,    16,   426,     0,
     801,   802,     0,     0,     0,     0,   804,     0,    18,     0,
       0,    20,   482,    22,     0,     0,     0,   490,     0,     0,
       0,   818,   819,     0,    25,   821,    27,   823,    28,   825,
      30,     0,     3,     0,     0,     0,    35,     0,     0,     5,
       6,     0,     0,     0,     0,     5,     0,     0,     0,   726,
       8,     0,   727,   728,   729,   726,     0,     9,   727,   728,
     729,     0,     0,     0,     0,  1214,     0,     0,     0,    14,
       0,     0,     0,     0,     0,    16,   730,   731,   732,     0,
       0,     0,   730,   731,   732,     0,    18,     0,   544,    20,
     545,    22,     0,     0,     0,   550,     0,     0,     0,     0,
     560,     0,    25,     0,    27,   558,    28,     0,    30,     0,
     568,     0,   559,     0,    35,     0,     0,   576,     0,     0,
     565,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   587,     0,     0,     0,     0,     0,   582,   593,
       0,     0,   596,   597,   586,     0,     0,   601,     0,     0,
     604,     0,     0,     0,     0,     0,     0,   613,     0,     0,
     615,     0,     0,     0,     0,   621,     0,   622,     0,     0,
       0,     0,     0,     0,     0,     0,   631,     0,     0,     0,
       0,     0,     0,     0,   626,     0,     0,     0,   630,   645,
       0,     0,     0,   652,     0,   967,   968,   969,   970,     0,
     972,   973,     0,     0,     0,     0,     0,     0,   689,     0,
     980,     0,   982,   983,     0,     0,     0,     0,     0,   701,
     987,   988,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   995,   655,   656,   657,   658,   659,   660,
     661,   662,   663,   664,   665,   666,   667,   668,     0,     0,
       0,     0,     0,     0,     0,   754,     0,     0,     0,     0,
       0,   766,     0,     0,     0,   772,     0,     0,     0,     0,
       0,   775,     0,   776,     0,     0,     0,   778,     0,     0,
       0,     0,     0,   782,     0,     0,     0,     0,     0,     0,
     785,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     783,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   795,     0,     0,   797,   798,   799,   800,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     806,     0,     0,     0,     0,     0,     0,   803,     0,   814,
       0,   816,   805,     0,     0,     0,   820,     0,   822,  1091,
     824,     0,     0,     0,  1094,  1095,   837,  1096,     0,     0,
       0,     0,     0,  1097,     0,     0,  1099,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   840,     0,   843,
       0,     0,   846,     0,   849,     0,   852,     0,   855,     0,
       0,   857,     0,     0,     0,     0,     0,     0,     0,   861,
       0,     0,   864,     0,   866,     0,     0,     0,     0,   869,
       0,     0,     0,     0,     0,     0,     0,     0,   888,     0,
     890,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   900,     0,   902,     0,     0,
       0,     0,     0,     0,   908,     0,     0,   909,     0,   910,
       0,   911,     0,     0,     0,     0,     0,     0,     0,     1,
       2,  1184,  1185,     0,     0,     3,     0,     0,     0,     0,
       4,     0,     5,     6,     0,     0,     0,     0,     0,     0,
       7,     0,     0,     8,     0,     0,   953,     0,     0,     0,
       9,    10,     0,    11,   958,    12,     0,     0,     0,     0,
      13,     0,    14,     0,     0,    15,     0,     0,    16,     0,
       0,     0,     0,     0,     0,     0,    17,  1237,     0,    18,
       0,    19,    20,    21,    22,     0,     0,     0,     0,     0,
       0,  1249,    23,    24,     0,    25,    26,    27,     0,    28,
      29,    30,    31,    32,    33,     0,    34,    35,    36,     0,
       0,     0,     0,     0,   999,     0,     0,     0,     0,  1005,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1013,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1022,     0,     0,     0,     0,     0,     0,  1023,
    1018,     0,  1028,  1029,  1030,  1031,  1032,  1033,     0,     0,
    1034,     0,  1038,     0,     0,  1042,     0,     0,     0,     0,
    1044,     0,  1053,  1054,  1055,  1337,     0,     0,     0,     0,
       0,  1059,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1384,     0,     0,  1385,  1386,     0,
    1388,  1086,     0,     0,  1390,  1391,     0,  1393,     0,  1396,
       0,     0,  1397,  1398,     0,  1400,     0,  1403,     0,  1404,
    1405,     0,  1407,  1408,     0,  1410,  1411,     0,  1412,     0,
    1414,  1415,     0,  1416,     0,  1417,  1103,     0,     0,     0,
    1098,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1115,     0,  1117,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1133,  1134,  1122,     0,  1434,     0,  1436,  1438,     0,  1440,
    1441,  1443,     0,  1445,  1446,  1447,  1448,     0,     0,     0,
    1150,     0,     0,     0,     0,  1161,  1162,     0,     0,     0,
       0,     0,     0,  1452,     0,  1454,     0,     0,     0,     0,
    1163,     0,     0,     0,     0,     0,  1167,     0,  1170,     0,
       0,  1174,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1186,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1226,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1228,
       0,  1229,     0,     0,     0,     0,     0,     0,     0,     0,
    1244,     0,     0,     0,     0,     0,     0,  1242,     0,     0,
       0,     0,     0,  1304,     0,     0,     0,     0,     0,     0,
       0,     0,  1312,     0,  1316,  1317,  1290,  1253,  1254,  1255,
    1256,  1257,  1258,  1259,  1291,  1260,  1261,  1262,  1263,  1264,
    1265,  1266,  1267,  1268,  1269,  1270,  1271,  1272,  1273,  1274,
    1275,  1276,  1277,  1278,  1279,  1280,  1281,  1282,  1283,  1292,
    1293,  1284,  1285,  1286,  1287,  1288,     0,     0,     0,     0,
    1326,     0,     0,     0,     0,     0,     0,  1332,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1341,     0,  1348,     0,     0,
       0,     0,     0,     0,  1339,     0,     0,     0,     0,     0,
       0,     0,  1382,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1389,     0,     0,     0,
       0,     0,     0,  1394,     0,     0,     0,     0,     0,     0,
       0,  1401,     0,     0,     0,     0,     0,     0,     0,  1418,
    1419,     0,     0,     0,  1253,  1254,  1255,  1256,  1257,  1258,
    1259,     0,  1260,  1261,  1262,  1263,  1264,  1265,  1266,  1267,
    1268,  1269,  1270,  1271,  1272,  1273,  1274,  1275,  1276,  1277,
    1278,  1279,  1280,  1281,  1282,  1283,     0,  1435,  1284,  1285,
    1286,  1287,  1288,     0,  1444,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1453,  1297,  1253,  1254,
    1255,  1256,  1257,  1258,  1259,  1298,  1260,  1261,  1262,  1263,
    1264,  1265,  1266,  1267,  1268,  1269,  1270,  1271,  1272,  1273,
    1274,  1275,  1276,  1277,  1278,  1279,  1280,  1281,  1282,  1283,
    1299,  1300,  1284,  1285,  1286,  1287,  1288
};

static const yytype_int16 yycheck[] =
{
      57,   573,     0,   192,     0,   228,   380,   115,   221,   201,
     196,   224,   131,   132,   204,   444,   352,   876,   195,   444,
     844,   133,   916,   805,   201,   213,   209,   543,   243,   207,
     421,   385,   978,   612,   764,   861,   859,   413,   864,   375,
       0,   358,   719,   618,   698,     0,   676,  1163,  1020,   624,
     859,   716,   855,   109,   110,   111,   112,   113,   244,  1320,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,     4,   130,   131,   132,    14,     6,   136,
      17,   138,   371,   140,   141,     6,   143,   362,  1081,    17,
    1100,     0,   149,   150,    26,   152,   153,   154,   155,   156,
     157,   158,   159,   160,     4,   162,    17,   164,   383,   221,
       0,     3,   224,    24,   839,   195,   841,    50,    36,   844,
     164,  1087,   847,  1089,  1090,   850,  1387,   184,   853,   186,
     187,   188,    14,   190,    26,    17,   192,     6,     7,    57,
      22,   197,     5,    12,   859,    36,    41,    79,    17,     6,
      81,    66,    15,    90,    85,    83,    89,    57,  1168,    16,
       9,    61,    83,    50,  1058,   236,   975,   356,    17,    87,
      88,    25,   844,   997,     9,   847,    91,    17,   152,   153,
     154,    81,    17,    23,    79,    85,    40,    74,   374,   246,
      39,   248,   384,  1016,   148,    49,    87,    88,   255,    62,
     257,   164,    89,   426,   261,   262,  1172,  1101,    90,  1012,
     267,   268,   269,   270,    83,   272,  1188,   274,   275,   276,
     950,  1080,   952,  1339,    78,   413,    83,    68,   285,  1175,
      17,   585,   623,   290,   291,    17,   293,   554,   198,   916,
      27,   895,   618,   300,  1138,   302,   201,   901,   913,   306,
     307,   830,    93,   310,  1247,   885,   313,   314,   774,   889,
     975,   318,   319,    17,    17,    41,   555,   584,    55,   326,
      52,  1168,    62,   330,    27,  1000,   333,  1002,   567,  1176,
    1104,  1006,   353,   340,   859,   342,   343,   344,   345,   346,
     505,   348,     5,  1234,   480,  1236,  1122,    51,    52,  1245,
     356,    32,    15,    56,     7,   361,   362,   419,    11,  1024,
     422,    19,   202,   528,    17,   205,  1098,    36,   164,   367,
     210,    29,   393,    54,    55,   997,   382,   383,  1000,    65,
     378,   379,   787,   788,   789,   391,   407,   164,    57,   387,
      68,    66,    61,   414,   164,    73,  1076,    72,   396,    62,
      63,   407,    71,   401,   410,     7,   404,   428,  1170,    11,
     815,   417,  1174,     5,   412,    93,    91,   363,    87,    88,
      17,   157,  1318,    15,   370,    22,   162,   163,   164,   377,
     437,   438,  1107,   719,   394,   562,   563,   444,   445,   399,
     447,    42,   402,    17,   451,   452,   453,   589,   455,   151,
    1130,   578,   579,    27,  1186,   796,   162,   163,   465,   599,
     467,   468,   589,   470,     4,   472,   473,   639,   475,   476,
     642,   478,   479,   606,  1247,   603,   483,   484,   485,    19,
     618,   488,  1104,  1105,   160,   161,   493,   494,   653,    29,
     497,   498,    31,    32,   501,   874,  1228,  1229,     7,  1024,
    1309,   508,   509,    12,    17,   512,   513,  1323,   515,   516,
      23,    24,   151,  1329,   781,    54,    55,    57,   525,   526,
     527,    61,   156,   150,   158,   532,   812,   534,  1208,   536,
     905,   538,   562,   563,    17,    58,  1216,   558,   544,   545,
     380,    81,    75,    76,    27,    85,   813,    49,   578,   579,
     390,   149,   392,    25,   152,   153,   154,  1179,   398,   565,
       7,   630,    33,    34,    11,    12,    13,   156,    16,   158,
      17,    54,    55,    20,   595,    22,   582,   149,   155,   156,
     152,   153,   154,   155,   156,   157,    17,   159,   609,    26,
       7,   612,    48,     7,    11,    12,    13,    14,    12,   620,
      17,   561,    17,    17,   564,    20,   566,    22,     7,   569,
     570,    17,    11,    12,    13,    33,   418,   577,    17,    63,
     626,    27,   424,    70,   630,   632,    32,   120,   588,   122,
     916,   124,   639,   126,  1314,   642,    34,   644,   598,   646,
    1165,    46,   602,  1168,  1305,   605,  1307,  1172,    54,    55,
    1238,  1176,  1240,    70,   157,     7,   616,   664,   156,    11,
      12,    13,   669,   670,   671,    17,   673,   674,   675,   676,
      30,    70,   679,    59,    60,   817,   158,   148,   149,   150,
     148,   152,   153,   154,   155,   156,    62,    63,   695,   696,
     697,   698,     8,   700,    55,   702,   375,   704,   719,   706,
    1024,    54,    55,  1036,  1037,  1230,   352,   870,   548,   717,
     849,   917,   552,   852,   914,  1240,   149,   150,   725,   152,
     153,   154,  1247,   839,   368,   369,   948,   949,   372,   373,
     737,   571,   739,   791,  1020,   742,   840,   744,   841,   746,
      17,   748,   386,   750,    21,    22,   753,  1330,  1331,   756,
     808,   395,   592,   997,   761,   762,   400,   764,   542,   403,
    1113,   768,  1251,   784,   148,   149,   773,   411,   152,   153,
     154,  1207,  1058,   761,   762,     7,  1211,   783,   618,    11,
      12,    13,  1084,  1085,   624,    17,   586,   112,   628,    21,
      22,  1089,   117,  1325,   119,  1172,   121,   803,   123,   805,
     125,  1331,  1328,    10,     0,   201,   827,   551,     7,   830,
     831,    18,    11,    12,    13,  1101,   644,   866,    17,   129,
     129,    28,   129,    22,   129,   275,   205,   390,    35,   129,
     199,    38,   199,   981,   855,   129,   488,   843,    70,   129,
     846,   977,   978,   585,   790,  1018,    53,   793,   129,   855,
     158,   129,  1138,   129,   408,   861,   129,    64,   864,   195,
      67,   807,    69,   195,   810,   617,    17,   874,   617,   876,
    1009,    70,  1011,    80,   617,    82,    27,    84,   885,    86,
      31,    32,   889,   617,   891,    92,   893,   894,   895,   129,
     838,   898,   129,  1058,   901,   916,   917,   904,   905,   129,
     129,   129,  1188,    54,    55,   129,   753,   742,     0,   744,
     340,   746,     0,   748,   921,   750,   923,  1439,   925,   343,
     345,  1418,   929,   930,   931,     0,   965,   934,   935,    -1,
     937,   938,     9,   940,    -1,    -1,   943,    -1,    -1,    -1,
      17,   948,   949,   950,    -1,   952,    -1,   954,   955,    -1,
      27,   791,    -1,    30,    31,    32,  1320,   964,   965,    -1,
    1324,  1325,    39,  1327,  1328,    -1,  1330,  1331,   808,  1333,
     991,    -1,    -1,  1112,    -1,    -1,    -1,    54,    55,    56,
     149,   150,    -1,   152,   153,   154,   155,   156,    -1,    -1,
     159,  1012,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   839,
      -1,   841,    -1,    -1,   844,    -1,  1012,   847,    -1,    -1,
     850,    -1,    -1,   853,    -1,  1021,  1023,    -1,    -1,   859,
      -1,    -1,  1029,  1030,  1031,  1032,    -1,   867,  1392,  1036,
    1037,  1322,  1323,  1324,  1325,  1399,  1327,  1058,  1329,  1175,
      -1,    -1,    -1,    -1,  1183,  1409,    -1,  1054,    -1,  1413,
      -1,    -1,    -1,    -1,  1061,  1062,  1063,    -1,  1065,  1066,
    1067,  1068,    -1,  1070,    -1,  1072,    -1,    -1,    -1,  1076,
      -1,  1078,    -1,  1080,    -1,    -1,    -1,  1084,  1085,    -1,
    1101,    -1,    -1,    -1,    -1,    -1,   921,    -1,    -1,    -1,
     925,    -1,  1098,    -1,   929,   930,   931,    -1,    -1,   934,
     935,  1392,    -1,   938,  1395,    -1,    -1,    -1,  1399,  1245,
      -1,  1402,    -1,    -1,    -1,  1406,  1122,  1138,    -1,  1126,
      -1,  1128,    -1,  1130,  1131,    -1,    -1,    -1,  1135,  1136,
      -1,     8,    -1,  1140,  1141,   975,  1143,    -1,    -1,  1146,
      17,  1148,    -1,    -1,    -1,   985,  1437,    -1,    -1,    -1,
      27,  1442,    -1,    30,    31,    32,    -1,  1164,    -1,    -1,
    1000,  1167,  1002,    -1,  1170,    -1,  1006,    -1,  1174,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1016,    54,    55,    56,
    1186,    -1,  1318,    -1,  1024,   149,    -1,    -1,   152,   153,
     154,   155,   156,    -1,    -1,   159,  1203,    -1,  1205,    -1,
    1207,  1208,  1209,    -1,  1211,  1212,    -1,    -1,    -1,  1216,
    1217,    -1,  1219,    -1,    -1,    -1,    -1,    -1,  1225,  1226,
      -1,    -1,  1228,  1229,    -1,    -1,    -1,    -1,    -1,     9,
    1065,  1066,  1067,  1068,    -1,   844,  1242,    17,   847,    -1,
      -1,   850,    -1,  1250,  1251,    -1,    -1,    27,    -1,    -1,
      30,    31,    32,    -1,    -1,  1394,    -1,    -1,    -1,    39,
      -1,    -1,  1401,    43,    44,   149,   150,  1107,   152,   153,
     154,   155,   156,    -1,    54,    55,    56,    -1,  1118,    -1,
      -1,    -1,  1289,  1290,  1291,  1292,  1293,    -1,    -1,  1296,
    1297,  1298,  1299,  1300,    -1,    -1,  1303,    -1,  1305,    -1,
    1307,    -1,  1309,    17,    -1,    -1,    -1,  1314,    -1,    -1,
      -1,  1146,    -1,    27,    -1,    -1,    30,    31,    32,    -1,
      -1,    -1,    -1,    -1,    -1,  1165,    -1,    -1,  1168,    -1,
      44,    -1,  1172,    -1,    -1,    -1,  1176,    17,    -1,  1346,
      54,    55,    56,   309,    -1,    -1,  1353,    27,  1355,    -1,
      30,    31,    32,    -1,  1361,    -1,  1363,   323,   324,     9,
     326,  1368,    -1,    -1,  1371,    -1,    -1,    17,  1389,  1376,
      -1,    -1,    -1,    -1,    54,    55,    56,    27,    -1,    -1,
      30,    31,    32,    -1,    -1,    -1,    -1,    -1,   997,    39,
    1230,  1000,    -1,  1002,    44,    -1,    -1,  1006,  1238,    -1,
    1240,    -1,    -1,    -1,    54,    55,    56,  1247,    -1,    -1,
      -1,  1418,    -1,    -1,    45,    -1,  1423,  1424,    -1,    -1,
    1427,    52,    53,    -1,    55,    -1,    -1,    58,    -1,    60,
      -1,    62,    -1,    -1,    -1,    66,    -1,    68,    17,    70,
      -1,    72,    -1,    74,    -1,    76,    -1,    78,    27,    80,
      -1,    30,    31,    32,   148,   149,   150,    88,   152,   153,
     154,   155,   156,    -1,    95,    44,    97,    46,    99,    -1,
     101,    -1,   103,    -1,   105,    54,    55,    56,    -1,    -1,
    1320,    -1,  1322,  1323,  1324,  1325,    -1,  1327,  1328,  1329,
    1330,  1331,    -1,  1333,    -1,  1104,  1105,  1106,  1107,    -1,
       7,     8,    -1,    -1,    11,    12,    13,    14,    -1,    -1,
      17,    -1,    -1,   479,    -1,    -1,   147,    -1,    -1,    -1,
      27,   487,    -1,    30,    31,    32,   492,    -1,    -1,    -1,
      -1,    -1,    -1,   499,   500,    -1,    -1,    -1,   504,    -1,
      -1,    -1,    -1,    -1,   510,    -1,    -1,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,   522,    -1,    -1,   203,
     191,    -1,   206,    70,   208,    -1,    -1,   211,    -1,    -1,
    1179,  1180,  1181,    94,    95,    96,    97,    98,    99,   100,
     101,   102,    -1,    90,     7,    -1,    -1,    -1,    11,    12,
      13,    14,    -1,    -1,    17,    -1,     7,    -1,    -1,    -1,
      11,    12,    13,    14,    27,    -1,    17,    30,    31,    32,
      -1,    -1,    -1,    -1,    37,    -1,    27,    -1,   249,    30,
      31,    32,    -1,   254,    -1,   256,    -1,    -1,   259,   260,
      -1,    54,    55,    56,  1243,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    55,    56,    -1,    70,    -1,    -1,
     281,   282,    -1,    -1,    -1,   286,    -1,    -1,   289,    70,
      -1,    -1,    -1,    -1,    -1,   296,    -1,    90,    -1,    -1,
     301,    -1,   303,    -1,    10,    -1,    -1,   308,    -1,    90,
      -1,    -1,    18,    -1,   315,    -1,    -1,    -1,    -1,   320,
      -1,    -1,    28,    -1,   325,    -1,   327,    -1,    -1,    35,
     331,    -1,    -1,    -1,    -1,    -1,   350,   338,    -1,    -1,
     341,    47,    -1,   357,    -1,   359,   360,    53,   349,    -1,
     686,   687,    -1,    -1,    -1,    -1,   692,    -1,    64,    -1,
      -1,    67,   376,    69,    -1,    -1,    -1,   381,    -1,    -1,
      -1,   707,   708,    -1,    80,   711,    82,   713,    84,   715,
      86,    -1,    10,    -1,    -1,    -1,    92,    -1,    -1,    17,
      18,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,    27,
      28,    -1,    30,    31,    32,    27,    -1,    35,    30,    31,
      32,    -1,    -1,    -1,    -1,    37,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,    54,    55,    56,    -1,
      -1,    -1,    54,    55,    56,    -1,    64,    -1,   439,    67,
     441,    69,    -1,    -1,    -1,   446,    -1,    -1,    -1,    -1,
     464,    -1,    80,    -1,    82,   456,    84,    -1,    86,    -1,
     474,    -1,   463,    -1,    92,    -1,    -1,   481,    -1,    -1,
     471,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   496,    -1,    -1,    -1,    -1,    -1,   489,   503,
      -1,    -1,   506,   507,   495,    -1,    -1,   511,    -1,    -1,
     514,    -1,    -1,    -1,    -1,    -1,    -1,   521,    -1,    -1,
     524,    -1,    -1,    -1,    -1,   529,    -1,   531,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   540,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   535,    -1,    -1,    -1,   539,   553,
      -1,    -1,    -1,   557,    -1,   881,   882,   883,   884,    -1,
     886,   887,    -1,    -1,    -1,    -1,    -1,    -1,   572,    -1,
     896,    -1,   898,   899,    -1,    -1,    -1,    -1,    -1,   583,
     906,   907,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   919,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   619,    -1,    -1,    -1,    -1,
      -1,   625,    -1,    -1,    -1,   629,    -1,    -1,    -1,    -1,
      -1,   635,    -1,   637,    -1,    -1,    -1,   641,    -1,    -1,
      -1,    -1,    -1,   647,    -1,    -1,    -1,    -1,    -1,    -1,
     654,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     651,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   677,    -1,    -1,   680,   681,   682,   683,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     694,    -1,    -1,    -1,    -1,    -1,    -1,   688,    -1,   703,
      -1,   705,   693,    -1,    -1,    -1,   710,    -1,   712,  1035,
     714,    -1,    -1,    -1,  1040,  1041,   720,  1043,    -1,    -1,
      -1,    -1,    -1,  1049,    -1,    -1,  1052,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   738,    -1,   740,
      -1,    -1,   743,    -1,   745,    -1,   747,    -1,   749,    -1,
      -1,   752,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   760,
      -1,    -1,   763,    -1,   765,    -1,    -1,    -1,    -1,   770,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   792,    -1,
     794,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   809,    -1,   811,    -1,    -1,
      -1,    -1,    -1,    -1,   818,    -1,    -1,   821,    -1,   823,
      -1,   825,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,
       5,  1157,  1158,    -1,    -1,    10,    -1,    -1,    -1,    -1,
      15,    -1,    17,    18,    -1,    -1,    -1,    -1,    -1,    -1,
      25,    -1,    -1,    28,    -1,    -1,   860,    -1,    -1,    -1,
      35,    36,    -1,    38,   868,    40,    -1,    -1,    -1,    -1,
      45,    -1,    47,    -1,    -1,    50,    -1,    -1,    53,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    61,  1213,    -1,    64,
      -1,    66,    67,    68,    69,    -1,    -1,    -1,    -1,    -1,
      -1,  1227,    77,    78,    -1,    80,    81,    82,    -1,    84,
      85,    86,    87,    88,    89,    -1,    91,    92,    93,    -1,
      -1,    -1,    -1,    -1,   928,    -1,    -1,    -1,    -1,   933,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   945,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   956,    -1,    -1,    -1,    -1,    -1,    -1,   963,
     951,    -1,   966,   967,   968,   969,   970,   971,    -1,    -1,
     974,    -1,   976,    -1,    -1,   979,    -1,    -1,    -1,    -1,
     984,    -1,   986,   987,   988,  1311,    -1,    -1,    -1,    -1,
      -1,   995,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1340,    -1,    -1,  1343,  1344,    -1,
    1346,  1025,    -1,    -1,  1350,  1351,    -1,  1353,    -1,  1355,
      -1,    -1,  1358,  1359,    -1,  1361,    -1,  1363,    -1,  1365,
    1366,    -1,  1368,  1369,    -1,  1371,  1372,    -1,  1374,    -1,
    1376,  1377,    -1,  1379,    -1,  1381,  1060,    -1,    -1,    -1,
    1051,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1075,    -1,  1077,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1094,  1095,  1083,    -1,  1420,    -1,  1422,  1423,    -1,  1425,
    1426,  1427,    -1,  1429,  1430,  1431,  1432,    -1,    -1,    -1,
    1114,    -1,    -1,    -1,    -1,  1119,  1120,    -1,    -1,    -1,
      -1,    -1,    -1,  1449,    -1,  1451,    -1,    -1,    -1,    -1,
    1121,    -1,    -1,    -1,    -1,    -1,  1127,    -1,  1129,    -1,
      -1,  1132,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1160,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1187,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1200,
      -1,  1202,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1224,    -1,    -1,    -1,    -1,    -1,    -1,  1218,    -1,    -1,
      -1,    -1,    -1,  1237,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1246,    -1,  1248,  1249,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,    -1,    -1,    -1,    -1,
    1294,    -1,    -1,    -1,    -1,    -1,    -1,  1301,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1319,    -1,  1321,    -1,    -1,
      -1,    -1,    -1,    -1,  1315,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1336,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1347,    -1,    -1,    -1,
      -1,    -1,    -1,  1354,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1362,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1383,
    1384,    -1,    -1,    -1,   109,   110,   111,   112,   113,   114,
     115,    -1,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,    -1,  1421,   143,   144,
     145,   146,   147,    -1,  1428,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1450,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     4,     5,    10,    15,    17,    18,    25,    28,    35,
      36,    38,    40,    45,    47,    50,    53,    61,    64,    66,
      67,    68,    69,    77,    78,    80,    81,    82,    84,    85,
      86,    87,    88,    89,    91,    92,    93,   166,   167,   168,
     239,   241,   307,   308,   311,   313,   334,   335,   336,   337,
     338,   339,   342,   343,   345,   347,   355,   356,   357,   366,
     367,   378,   380,   391,   392,   393,   401,   402,   403,   404,
     405,   406,   407,   409,   410,   411,   412,   413,   414,   415,
     417,   439,   440,   441,   443,   445,   446,   452,   453,   458,
     463,   466,   469,   472,   473,   476,   481,   483,   486,   490,
     503,   507,   508,   513,   515,   516,     0,   160,   161,   203,
     203,   203,   203,   203,   164,   205,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   148,   206,   242,   206,   314,   206,
     341,   341,   206,   348,     6,    83,   351,   352,   206,   358,
     341,   206,   381,   341,   358,   341,   358,   341,   358,   341,
     358,   206,   454,   206,   477,   336,   337,   338,   339,   366,
     392,   393,   404,   409,   413,   439,   452,   458,   463,   466,
     469,   472,   482,   484,   485,   206,   491,   491,   491,    41,
     495,   496,   205,   205,   205,   205,   205,   203,   205,   205,
     205,   205,   205,   359,   205,   205,   408,   205,   408,   205,
     205,   416,   205,   205,   162,   163,   204,   205,   205,   487,
     488,   205,   504,   505,   205,   509,   510,   205,   203,   149,
     150,   152,   153,   154,   155,   156,   169,   170,   171,   173,
     175,   176,   179,   181,   182,   206,   243,    62,   315,   317,
      19,    29,    57,   308,   313,   324,   325,   344,   356,   374,
     375,   442,   444,   447,   449,   450,   451,   324,   344,   442,
     444,   173,   349,   206,   353,   345,   368,   369,   371,   372,
     373,   374,   375,     3,    26,   382,   383,    71,   311,   313,
     324,   394,   402,   459,   460,   461,   462,    23,    24,   239,
     360,   361,   363,   364,   204,    72,   406,   464,   465,   239,
     360,   204,    73,   411,   467,   468,   204,    74,   415,   470,
     471,    51,    52,   239,   418,   419,   421,   422,   204,    65,
     455,   456,   473,   474,   515,    75,    76,   478,   479,    79,
     492,   493,   495,   492,   495,   492,   495,    42,   497,   498,
     168,   181,   151,   169,   180,   173,   205,   240,   205,   309,
     312,   203,   203,   205,   203,   205,   203,   203,   205,   205,
     205,   205,   205,   205,   205,   205,   359,   205,   203,   203,
     205,   379,   203,   203,   205,   205,   205,   203,   205,   260,
     205,   203,   205,   203,   205,   205,   203,   260,   205,   205,
     205,   203,   205,   205,   203,   260,   260,   203,   205,   260,
     203,   205,   203,   205,   203,   205,   488,   203,   205,   205,
     505,   205,   205,   510,   205,   205,   203,   204,   174,   177,
     178,   182,   181,    33,    34,   170,   206,   244,   245,   246,
     248,   249,   204,    63,   315,   320,   321,   345,   204,   204,
     206,   318,   314,   356,    58,   326,   327,    16,   269,   270,
     272,   274,   275,   277,   340,   351,   206,   376,   376,    49,
     424,   426,   424,   356,   340,   424,   424,   173,   350,   172,
     174,   354,   204,   378,   376,   376,     9,   239,   385,   387,
     204,   206,   384,   314,   402,   277,   395,   424,   376,   239,
     239,   363,   206,   362,   239,   181,   365,   269,   424,   376,
     239,   269,   424,   376,   269,   424,   376,   181,   185,   187,
     206,   420,   418,   206,   423,   424,   376,   474,   181,   480,
     206,   494,   497,    26,   428,   429,   497,    48,   517,   521,
     168,   181,   205,   205,   203,   203,   309,   312,   205,   310,
     203,   205,   205,   316,   205,   205,   205,   329,   203,   203,
     204,   205,   205,   205,   205,   203,   205,   205,   204,   205,
     205,   205,   346,   205,   260,   173,   204,   205,   205,   205,
     260,   379,   203,   260,   205,   205,   203,   204,   205,   205,
     260,   260,   205,   204,   260,   169,   204,   204,   205,   205,
     260,   204,   205,   205,   204,   205,   205,   156,   158,   184,
     186,   190,   193,   204,   260,   204,   205,   205,   205,   475,
     169,   204,   204,   205,   205,   506,   203,   205,   205,   208,
     203,   204,   245,   248,   206,   247,   206,   250,   239,   323,
     170,   322,   320,   239,   319,   204,   315,   340,    59,    60,
     330,   332,   204,   181,   328,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   278,
     279,   280,   269,   324,   344,   324,   344,   269,   206,   427,
     269,   340,   269,   269,     7,    11,   239,   261,   265,   204,
       7,    12,   259,   264,   269,   324,   344,   324,   344,   206,
     388,   204,   315,   395,   278,   269,   324,   402,   239,   181,
     269,   406,   269,   411,   269,   415,   181,   188,   181,   194,
     269,   447,   449,   450,   451,   457,    27,    30,    31,    32,
      54,    55,    56,   209,   211,   212,   213,   214,   215,   216,
     217,   220,   221,   222,   224,   225,   230,   232,   235,   236,
     239,   251,   252,   474,   204,   181,   428,    39,    44,   209,
     387,   431,   436,   437,   501,   502,   204,   206,   430,    46,
     514,   209,   204,   491,   205,   204,   204,   310,   204,   310,
     316,   205,   204,   203,   169,   204,   205,   205,   205,   205,
     205,   205,   448,   205,   448,   204,   205,   204,   204,   204,
     204,   260,   260,   203,   260,   203,   204,   205,   205,   370,
     205,   370,   205,   205,   204,   205,   204,   205,   260,   260,
     204,   260,   204,   260,   204,   260,   186,   184,   157,   159,
     182,   183,   191,   194,   199,   200,   201,   204,   205,   205,
     203,   205,   210,   203,   205,   210,   203,   205,   210,   203,
     205,   210,   203,   205,   210,   203,   210,   203,   475,   205,
     489,   203,   506,   506,   203,   208,   203,   205,   331,   203,
     205,   518,   519,   248,   315,   206,   333,   181,   152,   153,
     154,   281,   281,   281,   281,   356,   239,   351,   204,   356,
     204,   428,   206,   267,   267,   356,   239,   259,   351,   377,
     204,   356,   204,   172,   389,   315,   281,   402,   204,   204,
     204,   204,   181,   189,   194,   181,   195,   183,   202,   378,
     213,   239,   251,   216,   220,   239,   251,   206,   218,   224,
     230,   235,   206,   223,   230,   235,   170,   226,   235,   170,
     233,   185,   206,   237,   148,   207,    43,   209,   431,   436,
     499,   500,   501,   204,   388,   388,   322,   239,   204,   378,
     424,   486,   507,   511,   428,   492,   331,   260,   260,   260,
     260,   448,   260,   260,   448,   205,   425,   205,   205,   370,
     260,   205,   260,   260,   370,   205,   386,   260,   260,   186,
     202,   183,   192,   199,   195,   260,   210,   205,   210,   204,
     205,   210,   205,   210,   210,   204,   205,   210,   210,   205,
     210,   205,   205,   204,   489,   489,   205,   208,   203,   208,
     205,   205,   204,   204,   205,   520,   205,   519,   204,   204,
     204,   204,   204,   204,   204,   239,   431,   436,   204,   173,
     268,   268,   204,   377,   204,     7,    11,    12,    13,   239,
     257,   258,   390,   204,   204,   204,   181,   196,   197,   204,
     219,   221,   224,   230,   235,   230,   235,   235,   235,   170,
     227,   170,   234,   185,   206,   238,   501,   168,   389,   206,
     438,   205,   209,   387,   436,   522,   204,   205,   205,   205,
     205,   260,   425,   425,   260,   260,   260,   260,   203,   260,
     205,   169,   199,   204,   205,   205,   205,   205,   210,   210,
     210,   210,   205,   205,   231,   204,   208,   204,   205,   432,
     331,   514,   203,   520,   520,     8,   283,   285,   282,   285,
     283,   284,   285,   204,   204,   267,   282,   181,   198,   199,
     224,   230,   235,   230,   235,   235,   235,   170,   228,   261,
     204,     7,    11,    12,    13,    14,    70,   239,   433,   434,
     435,   204,   204,   203,   388,   205,   273,   203,   205,   271,
     203,   208,   205,   276,   203,   205,   205,   396,   199,   205,
     205,   205,   210,   205,   260,   260,   203,   511,   205,     7,
      11,    12,    13,    14,    70,    90,   209,   253,   254,   255,
     256,   262,   266,   306,   206,   286,   209,   282,   306,   286,
     209,   284,   286,   268,    37,   209,   306,   398,   399,   230,
     235,   235,   235,   170,   229,   267,   204,   389,   203,   203,
     205,   205,   271,   208,   205,   276,   205,   260,   205,   208,
     205,   397,   203,   205,   204,   205,   263,   205,   512,   260,
     267,   267,   209,   109,   110,   111,   112,   113,   114,   115,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   143,   144,   145,   146,   147,   299,
     108,   116,   141,   142,   287,   290,   299,   108,   116,   141,
     142,   292,   295,   299,   204,   398,   209,   398,   206,   400,
     235,   268,   204,   209,   501,   514,   204,   204,   205,   263,
     205,   297,   205,   205,   205,   205,   204,   205,   205,   205,
     205,   205,   204,   205,   397,   397,   331,   260,   208,   203,
     268,   204,    22,   239,   262,   298,   304,   305,   204,    20,
     239,   254,   288,   300,   301,   304,   288,    21,   239,   254,
     289,   302,   303,   304,   289,   239,   254,   291,   304,   239,
     293,   300,   304,   288,   239,   294,   302,   304,   294,   239,
     296,   304,   204,   511,   260,   260,   260,   205,   260,   203,
     260,   260,   205,   260,   203,   205,   260,   260,   260,   205,
     260,   203,   205,   260,   260,   260,   205,   260,   260,   205,
     260,   260,   260,   205,   260,   260,   260,   260,   204,   204,
     262,   181,   254,   304,   170,   254,   254,   304,   170,   254,
     254,   304,   304,   512,   260,   204,   260,   205,   260,   205,
     260,   260,   205,   260,   204,   260,   260,   260,   260,   254,
     259,   254,   260,   204,   260
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int16 yyr1[] =
{
       0,   165,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   167,   167,   167,   167,   167,   167,   168,   168,
     169,   170,   170,   171,   172,   173,   173,   174,   174,   175,
     176,   177,   178,   179,   179,   180,   180,   181,   181,   181,
     181,   182,   182,   183,   184,   185,   185,   185,   186,   186,
     187,   188,   189,   190,   191,   191,   192,   192,   193,   194,
     195,   196,   196,   196,   197,   198,   199,   199,   200,   201,
     201,   202,   202,   203,   203,   204,   204,   205,   206,   207,
     208,   208,   209,   209,   209,   210,   210,   210,   211,   211,
     212,   212,   212,   213,   213,   213,   213,   214,   215,   216,
     217,   218,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   231,   232,   233,   234,   235,   236,   237,
     237,   238,   238,   239,   240,   240,   240,   240,   240,   240,
     240,   241,   242,   243,   243,   244,   244,   245,   246,   247,
     248,   249,   250,   251,   252,   253,   253,   254,   255,   256,
     256,   256,   256,   256,   257,   258,   258,   258,   258,   259,
     260,   260,   261,   262,   263,   263,   264,   264,   265,   265,
     266,   266,   267,   268,   269,   269,   269,   269,   270,   271,
     271,   271,   271,   272,   273,   273,   273,   273,   274,   275,
     276,   276,   276,   277,   278,   278,   278,   278,   278,   278,
     278,   278,   278,   279,   279,   280,   280,   281,   281,   281,
     282,   283,   284,   285,   286,   287,   287,   287,   287,   287,
     287,   287,   287,   287,   288,   288,   288,   288,   288,   288,
     288,   288,   289,   289,   289,   289,   289,   289,   289,   289,
     290,   290,   291,   291,   291,   291,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   293,   293,   293,   293,   294,
     294,   294,   294,   295,   295,   296,   296,   297,   297,   298,
     298,   298,   298,   299,   299,   299,   299,   299,   299,   299,
     299,   299,   299,   299,   299,   299,   299,   299,   299,   299,
     299,   299,   299,   299,   299,   299,   299,   299,   299,   299,
     299,   299,   299,   299,   299,   299,   299,   299,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   307,   308,   309,
     309,   310,   310,   311,   312,   312,   313,   314,   315,   316,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   328,   328,   329,   329,   330,   331,   331,
     332,   332,   333,   334,   334,   334,   335,   335,   336,   337,
     338,   339,   340,   340,   341,   342,   342,   343,   343,   344,
     344,   345,   346,   346,   346,   347,   347,   348,   349,   350,
     351,   352,   352,   353,   354,   354,   355,   355,   356,   357,
     357,   357,   358,   359,   359,   359,   359,   359,   359,   359,
     359,   360,   361,   362,   363,   364,   365,   365,   365,   366,
     367,   367,   368,   368,   368,   368,   369,   370,   370,   370,
     370,   370,   371,   372,   373,   374,   375,   376,   377,   378,
     379,   379,   379,   380,   381,   382,   383,   383,   384,   385,
     386,   386,   386,   387,   388,   389,   390,   391,   391,   392,
     393,   394,   394,   395,   396,   396,   396,   396,   396,   397,
     397,   397,   398,   399,   400,   401,   401,   402,   403,   403,
     403,   404,   405,   405,   406,   407,   407,   408,   408,   408,
     408,   409,   410,   411,   412,   412,   413,   414,   415,   416,
     416,   416,   416,   416,   417,   417,   418,   419,   420,   420,
     421,   422,   423,   424,   425,   425,   425,   425,   426,   427,
     428,   429,   430,   431,   432,   432,   432,   433,   434,   435,
     435,   435,   435,   435,   435,   436,   437,   438,   439,   439,
     439,   440,   440,   441,   442,   442,   443,   444,   444,   445,
     446,   447,   448,   448,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   457,   457,   457,   458,   459,   459,
     460,   461,   462,   463,   464,   465,   466,   467,   468,   469,
     470,   471,   472,   473,   473,   473,   473,   473,   473,   473,
     473,   473,   473,   473,   473,   474,   474,   475,   475,   475,
     476,   477,   478,   479,   479,   480,   480,   480,   481,   482,
     482,   483,   484,   484,   484,   484,   484,   484,   484,   484,
     484,   484,   484,   484,   484,   484,   485,   485,   485,   485,
     485,   485,   485,   486,   487,   487,   488,   489,   489,   489,
     489,   489,   489,   489,   490,   491,   492,   493,   494,   495,
     496,   497,   498,   499,   500,   501,   502,   503,   504,   504,
     505,   506,   506,   506,   506,   506,   507,   508,   509,   509,
     510,   511,   511,   511,   511,   512,   512,   512,   512,   513,
     514,   515,   516,   517,   518,   518,   519,   520,   520,   520,
     520,   521,   522
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
       1,     9,     1,     4,     3,     1,     4,     0,     3,     3,
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
    int yynerrs;

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
  yynerrs = 0;
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
