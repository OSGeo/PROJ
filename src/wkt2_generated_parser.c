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
    T_UNSIGNED_INTEGER_DIFFERENT_ONE_TWO_THREE = 403,
    T_EPSG_CODE_FOR_INTERPOLATION = 404
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
#define YYLAST   3025

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  165
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  354
/* YYNRULES -- Number of rules.  */
#define YYNRULES  698
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1438

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
       0,   209,   209,   209,   209,   209,   209,   209,   210,   210,
     210,   211,   214,   214,   215,   215,   215,   216,   218,   218,
     222,   226,   226,   228,   230,   232,   232,   234,   234,   236,
     238,   240,   242,   244,   244,   246,   246,   248,   248,   248,
     248,   250,   250,   254,   256,   260,   261,   262,   264,   264,
     266,   268,   270,   272,   276,   277,   280,   281,   283,   285,
     287,   290,   291,   292,   294,   296,   298,   298,   300,   303,
     304,   306,   306,   311,   311,   313,   313,   315,   317,   319,
     323,   324,   327,   328,   329,   331,   331,   332,   335,   336,
     340,   341,   342,   346,   347,   348,   349,   351,   355,   357,
     360,   362,   365,   366,   367,   368,   369,   370,   371,   372,
     373,   374,   375,   376,   377,   378,   379,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   400,   402,   404,   408,   413,   415,   417,   419,
     421,   425,   430,   431,   433,   435,   437,   441,   445,   447,
     447,   449,   449,   454,   459,   460,   461,   462,   463,   464,
     465,   467,   469,   471,   471,   473,   473,   475,   477,   479,
     481,   483,   485,   489,   491,   495,   495,   498,   501,   506,
     506,   506,   506,   506,   509,   514,   514,   514,   514,   517,
     521,   522,   524,   540,   544,   545,   547,   547,   549,   549,
     555,   555,   557,   559,   566,   566,   566,   566,   568,   575,
     576,   577,   578,   580,   587,   588,   589,   590,   592,   599,
     606,   607,   608,   610,   612,   612,   612,   612,   612,   612,
     612,   612,   612,   614,   614,   616,   616,   618,   618,   618,
     620,   625,   631,   636,   639,   642,   643,   644,   645,   646,
     647,   648,   649,   650,   653,   654,   655,   656,   657,   658,
     659,   660,   663,   664,   665,   666,   667,   668,   669,   670,
     673,   674,   677,   678,   679,   680,   685,   686,   687,   688,
     689,   690,   691,   692,   693,   696,   697,   698,   699,   702,
     703,   704,   705,   708,   709,   712,   713,   718,   719,   722,
     723,   724,   725,   728,   729,   730,   731,   732,   733,   734,
     735,   736,   737,   738,   739,   740,   741,   742,   743,   744,
     745,   746,   747,   748,   749,   750,   751,   752,   753,   754,
     755,   756,   757,   758,   759,   760,   761,   762,   763,   765,
     768,   770,   772,   774,   776,   778,   794,   794,   796,   804,
     805,   807,   808,   810,   818,   819,   821,   823,   825,   830,
     831,   833,   835,   837,   839,   841,   843,   845,   850,   854,
     856,   859,   862,   863,   864,   866,   867,   869,   874,   875,
     877,   877,   879,   883,   883,   883,   885,   885,   887,   895,
     904,   912,   922,   923,   925,   927,   927,   929,   929,   932,
     933,   937,   943,   944,   945,   947,   947,   949,   951,   953,
     957,   962,   962,   964,   967,   968,   973,   974,   976,   981,
     981,   981,   983,   985,   986,   987,   988,   990,   993,   995,
     999,  1005,  1005,  1009,  1009,  1010,  1010,  1012,  1017,  1018,
    1019,  1020,  1021,  1023,  1029,  1034,  1040,  1042,  1044,  1046,
    1050,  1056,  1057,  1058,  1060,  1062,  1064,  1068,  1068,  1070,
    1072,  1077,  1078,  1079,  1081,  1083,  1085,  1087,  1091,  1091,
    1093,  1099,  1106,  1106,  1109,  1116,  1117,  1118,  1119,  1120,
    1122,  1126,  1128,  1130,  1130,  1134,  1139,  1139,  1139,  1143,
    1148,  1148,  1150,  1154,  1154,  1158,  1163,  1165,  1169,  1169,
    1173,  1178,  1180,  1184,  1185,  1186,  1187,  1188,  1190,  1190,
    1192,  1195,  1197,  1197,  1199,  1201,  1203,  1207,  1213,  1214,
    1215,  1216,  1218,  1220,  1224,  1229,  1231,  1234,  1241,  1242,
    1243,  1245,  1249,  1255,  1255,  1255,  1255,  1255,  1255,  1259,
    1264,  1266,  1271,  1271,  1272,  1274,  1274,  1276,  1283,  1283,
    1285,  1292,  1292,  1294,  1301,  1308,  1313,  1314,  1315,  1317,
    1323,  1328,  1336,  1342,  1344,  1346,  1352,  1354,  1354,  1355,
    1355,  1359,  1365,  1365,  1367,  1372,  1378,  1383,  1389,  1394,
    1399,  1405,  1410,  1415,  1421,  1426,  1431,  1437,  1437,  1438,
    1438,  1439,  1439,  1440,  1440,  1441,  1441,  1442,  1442,  1445,
    1445,  1447,  1448,  1449,  1451,  1453,  1457,  1460,  1460,  1463,
    1464,  1465,  1467,  1471,  1472,  1474,  1476,  1476,  1477,  1477,
    1478,  1478,  1478,  1479,  1480,  1480,  1481,  1481,  1482,  1482,
    1484,  1484,  1485,  1485,  1486,  1487,  1487,  1491,  1495,  1496,
    1499,  1504,  1505,  1506,  1507,  1508,  1509,  1510,  1512,  1514,
    1516,  1519,  1521,  1523,  1525,  1527,  1529,  1531,  1533,  1535,
    1537,  1542,  1546,  1547,  1550,  1555,  1556,  1557,  1558,  1559,
    1561,  1566,  1571,  1572,  1575,  1581,  1581,  1581,  1581,  1583,
    1584,  1585,  1586,  1588,  1590,  1595,  1601,  1603,  1608,  1609,
    1612,  1620,  1621,  1622,  1623,  1625,  1627,  1632,  1633
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
  "\"EPSG code for interpolation CRS\"", "'.'", "'E'", "'1'", "'2'", "'3'",
  "'+'", "'-'", "':'", "'T'", "'Z'", "'['", "'('", "']'", "')'", "','",
  "$accept", "input", "datum", "crs", "period", "number",
  "signed_numeric_literal_with_sign", "signed_numeric_literal",
  "unsigned_numeric_literal", "opt_sign", "approximate_numeric_literal",
  "mantissa", "exponent", "signed_integer", "exact_numeric_literal",
  "opt_period_unsigned_integer", "unsigned_integer", "sign", "colon",
  "hyphen", "datetime", "opt_24_hour_clock", "year", "month", "day",
  "_24_hour_clock", "opt_colon_minute_colon_second_time_zone_designator",
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
  "operation_parameter", "operation_parameter_body", "parameter_unit",
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
  "opt_end_abridged_coordinate_transformation",
  "abridged_transformation_keyword", "abridged_transformation_parameter",
  "abridged_transformation_parameter_body", YY_NULLPTR
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

#define YYPACT_NINF (-1247)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-637)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    2593, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247,
   -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247,
   -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247,
   -1247, -1247, -1247, -1247, -1247, -1247, -1247,    89, -1247, -1247,
   -1247,   367, -1247, -1247, -1247,   367, -1247, -1247, -1247, -1247,
   -1247, -1247,   367,   367, -1247,   367, -1247,   -88,   367, -1247,
     367, -1247,   367, -1247, -1247, -1247,   367, -1247,   367, -1247,
     367, -1247,   367, -1247,   367, -1247,   367, -1247,   367, -1247,
     367, -1247, -1247, -1247, -1247, -1247, -1247, -1247,   367, -1247,
   -1247, -1247, -1247, -1247, -1247,   367, -1247,   367, -1247,   367,
   -1247,   367, -1247,   367, -1247,   367, -1247, -1247, -1247,   -43,
     -43,   -43,   -43,   -43, -1247,    59,   -43,   -43,   -43,   -43,
     -43,   -43,   -43,   -43,   -43,   -43,   -43,   -43,   -43,  1013,
     -43,   -43,   -43,    78, -1247, -1247,   -88, -1247,   -88, -1247,
     -88,   -88, -1247,   -88, -1247, -1247, -1247,   367, -1247,   -88,
     -88, -1247,   -88,   -88,   -88,   -88,   -88,   -88,   -88,   -88,
     -88, -1247,   -88, -1247,   -88, -1247, -1247, -1247, -1247,   -29,
   -1247, -1247, -1247, -1247, -1247,    -2,    44,    74, -1247, -1247,
   -1247, -1247,   287, -1247,   -88, -1247,   -88,   -88,   -88, -1247,
     -88,   367,   874,   134,    87,    87,   604,   -43,    56,   289,
     203,   699,   356,   287,    23,   287,    81,   287,    97,   308,
     287,   241,  1444, -1247, -1247, -1247,   458,    71, -1247, -1247,
      71, -1247, -1247,    71, -1247, -1247,   293,  1013, -1247, -1247,
   -1247, -1247, -1247, -1247, -1247,   527, -1247, -1247, -1247, -1247,
     213,   221,   231,   604, -1247,   -88, -1247,   -88,   367, -1247,
   -1247, -1247, -1247,   367,   -88,   367,   -88, -1247,   367,   367,
     -88,   -88, -1247, -1247, -1247, -1247,   -88,   -88,   -88,   -88,
   -1247,   -88, -1247,   -88,   -88,   -88, -1247, -1247, -1247, -1247,
     367,   367, -1247, -1247,   -88,   367, -1247, -1247,   367,   -88,
     -88, -1247,   -88, -1247, -1247,   367, -1247,   -88,   -88,   367,
   -1247, -1247,   -88,   -88,   367, -1247, -1247,   -88,   -88,   367,
   -1247, -1247,   -88,   -88,   367, -1247, -1247,   -88,   -88,   367,
     -88,   367, -1247, -1247,   -88,   367, -1247,   -88, -1247, -1247,
   -1247, -1247,   367, -1247,   -88,   367,   -88,   -88,   -88,   -88,
     -88, -1247,   -88,   367,   287, -1247,   395,   527, -1247, -1247,
     459,   287,   160,   287,   287,   -43,   -43,    42,   332,    57,
     -43,   -43,   359,   359,    42,    57,   359,   359,   604,   395,
     287,   443,   -43,   -43,   285,   287,   -43,   -43,   279,   462,
     359,   -43,   469, -1247,   469,   -43,   462,   359,   -43,   462,
     359,   -43,   462,   359,   -43, -1247, -1247,   584,   149, -1247,
     -43,   359,   -43,  1444,   527,    78, -1247,   -43,   293,    78,
   -1247,   468,    78, -1247,   293,   473,  1013, -1247,   527, -1247,
   -1247, -1247, -1247, -1247, -1247, -1247, -1247,   -88,   -88,   367,
   -1247,   367, -1247, -1247,   -88,   -88,   367,   -88, -1247, -1247,
   -1247,   -88,   -88,   -88, -1247,   -88,   367, -1247, -1247, -1247,
   -1247, -1247, -1247,   367,   287,   -88, -1247,   -88,   -88, -1247,
     -88,   367,   -88,   -88,   287,   -88,   -88, -1247,   -88,   -88,
     604,   287, -1247,   -88,   -88,   -88, -1247,   -88,   -88,   367,
   -1247, -1247,   -88,   -88,   -88,   367,   287,   -88,   -88,   -88,
     -88, -1247,   287,   287,   -88,   -88,   287,   -88,   -88,   287,
     -88,   -88, -1247, -1247,   380, -1247,   287,   -88, -1247,   287,
     -88,   -88,   -88,   231,   287, -1247,   287,   -88, -1247,   -88,
     367,   -88, -1247,   -88,   367,   287, -1247,   512,   472,   -43,
     -43, -1247, -1247,   469, -1247,   818,   485,   469,   287,   134,
      57,   515,   287,   527,  1261, -1247,   462,    76,    76,   462,
     -43,   462,    57, -1247,   462,   462,   425,   287,   470, -1247,
   -1247, -1247,   462,    76,    76, -1247, -1247,   -43,   287,   134,
     462,  1017, -1247,   462,   327, -1247, -1247, -1247, -1247,   462,
      86, -1247,   462,   198, -1247,   462,    20, -1247, -1247,   527,
   -1247, -1247,   527, -1247, -1247, -1247,   462,   289,  1580,   287,
     527, -1247, -1247,   468,   835,   287,   -43,   511,  1730,   287,
     -43, -1247,   -88, -1247, -1247,   287, -1247,   287, -1247,   -88,
   -1247,   287,   -88, -1247,   -88, -1247,   -88,   287, -1247, -1247,
   -1247,   367, -1247,   231,   287, -1247, -1247, -1247, -1247, -1247,
   -1247, -1247, -1247, -1247,   -88, -1247, -1247, -1247, -1247,   -88,
     -88,   -88, -1247,   -88,   -88,   -88,   -88,   287, -1247,   -88,
     287,   287,   287,   287, -1247, -1247,   -88,   -88,   367, -1247,
   -1247, -1247,   -88,   367,   287,   -88,   -88,   -88,   -88, -1247,
     -88, -1247,   -88,   287,   -88,   287,   -88,   -88,   287,   -88,
     287,   -88,   287,   -88,   408,   460, -1247,   543,   287, -1247,
   -1247, -1247, -1247,   -88, -1247, -1247, -1247, -1247, -1247, -1247,
   -1247, -1247, -1247, -1247, -1247,   -88,   367,   -88,   367, -1247,
     -88,   367,   -88,   367,   -88,   367,   -88,   367,   -88, -1247,
     367,   -88, -1247, -1247,   -88, -1247, -1247, -1247,   367,   -88,
     -88,   367,   -88,   367, -1247, -1247,   -88, -1247,   367, -1247,
   -1247,   -88,   472, -1247, -1247, -1247, -1247, -1247, -1247,    62,
   -1247,   -43,   527, -1247,   451,   451,   451,   451,    42,    58,
     287,    42,   287, -1247,   468, -1247, -1247, -1247, -1247, -1247,
   -1247,   -43, -1247,   -43, -1247,    42,   281,   287,    42,   287,
     395,   520, -1247,   451, -1247,   279,   287, -1247,   287, -1247,
     287, -1247,   287, -1247,   527, -1247, -1247,   527,   527, -1247,
     402, -1247, -1247, -1247, -1247,   443,    61,   603,   301, -1247,
     -43,   398, -1247,   -43,   275, -1247,   818,   128, -1247,   818,
     319, -1247,   584, -1247,   423, -1247,  1222,   287,   310, -1247,
   -1247,   -43, -1247,   818,   469,   287,   247,    85, -1247, -1247,
   -1247,   -88, -1247,   -88, -1247, -1247, -1247, -1247,   -88,   -88,
     -88,   -88,   -88,   -88,   -88, -1247,   -88, -1247,   -88, -1247,
     -88,   -88,   -88,   -88, -1247,   -88,   -88, -1247,   -88, -1247,
   -1247,   -88,   -88,   -88,   -88, -1247, -1247, -1247, -1247, -1247,
     476,   402, -1247,   543,   527, -1247,   -88, -1247,   -88, -1247,
     -88, -1247,   -88, -1247, -1247,   287,   -88,   -88,   -88, -1247,
     287,   -88,   -88, -1247,   -88,   -88, -1247,   -88, -1247, -1247,
     -88, -1247,   287, -1247, -1247,   -88,   -88,   -88,   367,   -88,
   -1247,   -88,   -88,   287,   -88,   287, -1247, -1247, -1247, -1247,
   -1247, -1247,   287,   -88,   -88,   287,   287,   287,   287,   287,
     287, -1247, -1247,   287,   352,   287,   604,   604,   287, -1247,
     470, -1247, -1247,   287,   528,   287,   287,   287, -1247, -1247,
     527, -1247, -1247, -1247,   287, -1247,   148, -1247, -1247,   275,
   -1247,   128, -1247, -1247, -1247,   128, -1247, -1247,   818, -1247,
     818,   584, -1247, -1247, -1247,  1562, -1247,  1013, -1247,   395,
     395, -1247,   -43, -1247,   -88,  1531,   287,   468, -1247, -1247,
     -88,   -88,   -88,   -88, -1247, -1247,   -88,   -88,   -88, -1247,
   -1247,   -88,   -88, -1247,   -88, -1247, -1247, -1247, -1247, -1247,
     -88, -1247,   367,   -88, -1247,   -88, -1247, -1247, -1247,   678,
   -1247,   287,   -88,   -88,   -88, -1247,   -88,   -88,   -88,   -88,
   -1247,   -88, -1247,   -88, -1247, -1247,   287,   -88,   287,   -88,
     -88, -1247,   -88,   511, -1247,   367,   -88,   -88, -1247,   633,
     633,   633,   633, -1247, -1247, -1247,   287,   287, -1247, -1247,
     -43, -1247,   633,   761, -1247, -1247,   430,   547,   589,   128,
   -1247, -1247, -1247, -1247,   818,   497,   287, -1247, -1247, -1247,
     261, -1247,   488,   287,   367,   442, -1247, -1247, -1247,   -88,
     367,   -88,   367,   -88,   -88,   367, -1247, -1247,   -88,   -88,
     414,   761, -1247,   -88,   -88, -1247,   -88, -1247, -1247,   -88,
   -1247,   -88, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247,
     -88, -1247,   367,   -88, -1247,   247,   -88,   -88,   287,  1430,
   -1247,   -43,   713, -1247,   -43, -1247,   989, -1247,   -43,   604,
    1113, -1247, -1247,   547,   589,   589, -1247,   818, -1247,   -43,
   -1247,   287,   395,   395, -1247, -1247, -1247, -1247, -1247, -1247,
   -1247, -1247, -1247, -1247, -1247, -1247,   367, -1247,   367,   -88,
   -1247,   -88, -1247,   -88,   -88,   -88, -1247,   -88,   -88,   -88,
   -1247, -1247,   -88,   -88,   367,   -88, -1247, -1247, -1247, -1247,
     287,   -88,   -88,   -88,   -88,   -43,   -43,  1730,  2698, -1247,
   -1247,  2579, -1247,  2619,   287,  1678, -1247, -1247,   -43,   589,
   -1247,   604,   287,  1720,   287, -1247, -1247,   -88,   -88, -1247,
   -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247,
   -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247,
   -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247, -1247,
   -1247, -1247, -1247, -1247, -1247, -1247,   -88,   -88,   -88,   -88,
     -88,   287, -1247,   -88,   -88,   -88,   -88,   -88,   287, -1247,
     -88, -1247,   -88, -1247,   -88, -1247,   -88, -1247, -1247,   -88,
     367, -1247,   604,   287,    68,   287,   637,   637,   330,   330,
   -1247,   573,   494,   637,   508,   508, -1247,   403, -1247,   287,
   -1247, -1247,   247,   -88, -1247, -1247,   -88,   -88, -1247,   -88,
     367, -1247, -1247,   -88,   -88, -1247,   -88,   367,   -88, -1247,
   -1247,   -88,   -88, -1247,   -88,   367,   -88, -1247,   -88,   -88,
   -1247,   -88,   -88, -1247,   -88,   -88, -1247,   -88, -1247,   -88,
     -88, -1247,   -88, -1247,   -88, -1247,   287,   287, -1247, -1247,
      95, -1247,   527, -1247, -1247,   573, -1247,   818,   541, -1247,
   -1247, -1247,   573, -1247,   818,   541, -1247, -1247, -1247,   541,
   -1247, -1247,   403, -1247, -1247, -1247,   403, -1247, -1247, -1247,
   -1247,   -88, -1247,   -88,   287,   -88,   -88,   -88,   -88,   -88,
     -88,   287,   -88,   -88,   -88,   -88, -1247, -1247, -1247, -1247,
     541, -1247,   427, -1247, -1247,   541, -1247, -1247, -1247, -1247,
   -1247, -1247,   -88,   287,   -88, -1247, -1247, -1247
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,   419,   406,   395,   405,   161,   431,   454,   397,   483,
     486,   604,   648,   683,   686,   508,   501,   356,   563,   493,
     490,   498,   496,   615,   670,   396,   421,   432,   398,   420,
     484,   488,   487,   509,   494,   491,   499,     0,     4,     5,
       2,     0,    13,   346,   347,     0,   587,   385,   383,   384,
     386,   387,     0,     0,     3,     0,    12,   416,     0,   589,
       0,    11,     0,   591,   468,   469,     0,    14,     0,   593,
       0,    15,     0,   595,     0,    16,     0,   597,     0,    17,
       0,   588,   544,   542,   543,   545,   546,   590,     0,   592,
     594,   596,   598,    19,    18,     0,     7,     0,     8,     0,
       9,     0,    10,     0,     6,     0,     1,    73,    74,     0,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    78,   162,     0,   357,     0,   394,
       0,     0,   407,     0,   411,   412,   417,     0,   422,     0,
       0,   455,     0,     0,   423,     0,   423,     0,   423,     0,
     503,   564,     0,   605,     0,   616,   630,   617,   631,   618,
     619,   633,   620,   621,   622,   623,   624,   625,   626,   627,
     628,   629,     0,   613,     0,   649,     0,     0,     0,   654,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    75,    76,   612,     0,     0,   637,   639,
       0,   661,   663,     0,   671,   673,     0,     0,    40,    20,
      37,    38,    39,    41,    42,     0,   163,    21,    22,    26,
       0,    25,    35,     0,   164,   154,   361,     0,     0,   446,
     447,   369,   400,     0,     0,     0,     0,   399,     0,     0,
       0,     0,   548,   551,   549,   552,     0,     0,     0,     0,
     408,     0,   413,     0,   423,     0,   433,   434,   435,   436,
       0,     0,   458,   457,   451,     0,   576,   473,     0,     0,
       0,   472,     0,   572,   573,     0,   428,   190,   424,     0,
     485,   579,     0,     0,     0,   492,   582,     0,     0,     0,
     497,   585,     0,     0,     0,   515,   511,   190,   190,     0,
     190,     0,   502,   566,     0,     0,   599,     0,   600,   607,
     608,   614,     0,   651,     0,     0,     0,     0,     0,     0,
       0,   656,     0,     0,     0,    34,    27,     0,    33,    23,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   425,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   507,   506,     0,     0,   504,
       0,     0,     0,     0,     0,     0,   638,     0,     0,     0,
     662,     0,     0,   672,     0,     0,     0,   653,     0,    29,
      31,    28,    36,   168,   171,   165,   166,   155,   158,     0,
     160,     0,   153,   365,     0,   351,     0,     0,   348,   353,
     362,   359,     0,     0,   371,   375,     0,   223,   393,   204,
     205,   206,   207,     0,     0,     0,   448,     0,     0,   522,
       0,     0,     0,     0,     0,     0,     0,   409,   402,   190,
       0,     0,   418,     0,     0,     0,   464,   190,   451,     0,
     450,   459,   190,     0,     0,     0,     0,     0,     0,   190,
     190,   429,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    50,   512,    48,   513,     0,   190,   516,     0,
       0,     0,   601,   609,     0,   652,     0,     0,   525,   665,
       0,     0,   695,    80,     0,     0,    32,     0,     0,     0,
       0,   350,   355,     0,   354,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   388,     0,     0,     0,     0,
       0,     0,     0,   390,     0,     0,     0,     0,     0,   415,
      24,   410,     0,     0,     0,   452,   453,     0,     0,     0,
       0,     0,   470,     0,     0,   191,   426,   427,   489,     0,
       0,   495,     0,     0,   500,     0,     0,    44,    58,     0,
      45,    49,     0,   510,   505,   514,     0,     0,     0,     0,
     610,   606,   650,     0,     0,     0,     0,     0,     0,     0,
       0,   655,   156,   159,   169,     0,   172,     0,   367,   351,
     366,     0,   351,   363,   359,   358,     0,     0,   380,   381,
     376,     0,   368,   372,     0,   224,   225,   226,   227,   228,
     229,   230,   231,   232,     0,   233,   234,   235,   236,     0,
       0,     0,   392,     0,   556,     0,   556,     0,   523,     0,
       0,     0,     0,     0,   199,   198,   190,   190,     0,   401,
     197,   196,   190,     0,     0,     0,   438,     0,   438,   465,
       0,   456,     0,     0,     0,     0,     0,   190,     0,   190,
       0,   190,     0,   190,    48,     0,    59,     0,     0,   567,
     568,   569,   570,     0,   174,   100,   133,   136,   144,   148,
      98,   603,    82,    88,    89,    93,     0,    85,     0,    92,
      85,     0,    85,     0,    85,     0,    85,     0,    85,    84,
       0,   601,   586,   611,   641,   540,   660,   669,     0,   665,
     665,     0,    80,     0,   664,   526,   378,   684,     0,    81,
     685,     0,     0,   167,   170,   352,   364,   349,   360,     0,
     389,     0,   373,   370,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   547,     0,   550,   391,   553,   554,   404,
     403,     0,   414,     0,   430,     0,     0,     0,     0,     0,
      27,     0,   471,     0,   571,     0,     0,   577,     0,   580,
       0,   583,     0,    46,     0,    43,    68,     0,     0,    53,
      71,    55,    66,    67,   562,     0,     0,     0,     0,    91,
       0,     0,   117,     0,     0,   118,     0,     0,   119,     0,
       0,   120,     0,    83,     0,   602,     0,     0,     0,   666,
     667,     0,   668,     0,     0,     0,     0,     0,   687,   689,
     157,     0,   382,   378,   374,   237,   238,   239,   190,   190,
     190,   190,   556,   190,   190,   555,   556,   560,   518,   202,
       0,     0,   438,   190,   449,   190,   190,   437,   438,   444,
     466,   461,     0,   190,   190,   574,   578,   581,   584,    52,
      48,    71,    60,     0,     0,    70,   190,    96,    85,    94,
       0,    90,    85,    87,   101,     0,    85,    85,    85,   134,
       0,    85,    85,   137,     0,    85,   145,     0,   149,   150,
       0,    79,     0,   658,   647,   641,   641,    80,     0,    80,
     640,     0,     0,     0,     0,     0,   379,   524,   677,   678,
     675,   676,     0,   691,     0,     0,     0,     0,     0,     0,
       0,   558,   557,     0,     0,     0,     0,     0,     0,   442,
       0,   439,   441,     0,     0,     0,     0,     0,    47,    69,
       0,    54,    57,    72,     0,    95,     0,    86,    99,     0,
     121,     0,   122,   123,   132,     0,   124,   125,     0,   126,
       0,     0,   173,   642,   643,     0,   644,     0,   646,    27,
      27,   527,     0,   659,     0,     0,     0,     0,   688,   377,
       0,     0,     0,     0,   559,   561,   190,   518,   518,   517,
     203,   190,   190,   443,   190,   445,   188,   186,   185,   187,
     190,   467,     0,   190,   460,     0,   575,    64,    56,     0,
     565,     0,   102,   103,   104,   105,    85,    85,    85,    85,
     138,     0,   146,   142,   151,   152,     0,    80,     0,   378,
       0,   541,   378,     0,   694,     0,   691,   691,   690,     0,
       0,     0,     0,   521,   519,   520,     0,     0,   440,   462,
       0,   463,     0,     0,    63,    97,     0,     0,     0,     0,
     127,   128,   129,   130,     0,     0,     0,   147,   645,   657,
       0,   529,     0,     0,     0,     0,   693,   692,   243,   214,
       0,   209,     0,    80,   220,     0,   192,   189,     0,   475,
      65,     0,    61,   106,   107,   108,   109,   110,   111,    85,
     139,     0,   143,   141,   538,   533,   534,   535,   536,   537,
     378,   531,     0,   378,   539,     0,     0,     0,     0,     0,
     213,     0,     0,   208,     0,   218,     0,   219,     0,     0,
       0,   474,    62,     0,     0,     0,   131,     0,   530,     0,
     528,     0,    27,    27,   696,   183,   180,   179,   182,   200,
     181,   201,   217,   345,   175,   177,     0,   176,     0,   215,
     244,     0,   212,   209,    80,     0,   222,   220,     0,   190,
     481,   479,    80,    80,     0,   112,   113,   114,   115,   140,
       0,   194,   679,   190,   190,     0,     0,     0,     0,   211,
     210,     0,   221,     0,     0,     0,   476,   478,     0,     0,
     135,     0,     0,     0,     0,   698,   697,     0,   194,   216,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,   297,   246,   248,   250,
     252,     0,   245,   270,   277,   279,   281,   283,     0,   276,
     293,   184,    80,   482,   378,   116,   190,   532,   682,    80,
       0,   674,     0,     0,     0,     0,     0,     0,     0,     0,
     240,     0,     0,     0,     0,     0,   242,     0,   477,     0,
     195,   681,     0,   190,   193,   344,   190,   190,   298,   190,
       0,   241,   340,   190,   190,   247,   190,     0,   190,   249,
     342,   190,   190,   251,   190,     0,   190,   253,   190,   190,
     271,   190,   190,   278,   190,   190,   280,   190,   282,   190,
     190,   284,   190,   294,   190,   480,     0,     0,   299,   302,
       0,   300,     0,   254,   261,     0,   258,     0,     0,   260,
     262,   269,     0,   266,     0,     0,   268,   272,   275,     0,
     273,   285,     0,   287,   288,   289,     0,   291,   292,   295,
     296,   679,   178,   190,     0,   190,   190,     0,   190,   190,
     190,     0,   190,   190,   190,   190,   680,   301,   343,   257,
       0,   255,     0,   259,   265,     0,   263,   341,   267,   274,
     286,   290,   190,     0,   190,   256,   339,   264
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1247, -1247, -1247,  -222,  -229,  -191, -1247,   276,  -194,   309,
   -1247, -1247, -1247, -1247, -1247, -1247,   -58,  -227,  -653,   -33,
    -784,  -651, -1247, -1247, -1247, -1247, -1247, -1247, -1247,  -547,
    -230, -1247, -1247, -1247,  -852, -1247, -1247,  -223,  1233,  1229,
     -57,  1273, -1247,  -731,  -577,  -654, -1247, -1247,  -140, -1247,
   -1247,  -130, -1247, -1247, -1247,  -117,  -273, -1247, -1247,  -783,
   -1247, -1247, -1247, -1247, -1247,  -787, -1247, -1247, -1247, -1247,
    -641, -1247, -1247, -1247,   114, -1247, -1247, -1247, -1247, -1247,
     178, -1247, -1247,  -488, -1247, -1247,  -666, -1247, -1247,  -598,
   -1247, -1247, -1247, -1247,  -555,  1591,  -388, -1246,  -524, -1247,
   -1247, -1247,  -747,  -934,   -36, -1247,  -474, -1247, -1247, -1247,
   -1247,  -464,  -340,   152, -1247, -1247,  -445, -1027,  -336,  -415,
   -1012,  -637, -1247,  -804,  -569, -1247, -1247, -1247, -1247,  -570,
   -1247, -1247, -1247, -1247,  -590,  -565, -1247,  -662, -1247,  -413,
   -1247,  -803, -1247,   748,  -416,  -143,   550,  -426,    33,   -65,
    -326,   129, -1247, -1247, -1247,   224, -1247,   -82, -1247,   -67,
   -1247, -1247, -1247, -1247, -1247, -1247,  -836, -1247, -1247, -1247,
   -1247,   635,   636,   641,   642,  -303,   505, -1247, -1247,   -74,
      31, -1247, -1247, -1247, -1247, -1247,  -108, -1247, -1247, -1247,
   -1247,    10, -1247,   564,  -105, -1247, -1247, -1247,   647, -1247,
   -1247, -1247,  -636, -1247, -1247, -1247,   580,   590,   546,  -177,
       4,   312, -1247, -1247, -1247, -1247, -1247, -1247, -1247,  -368,
    -809,  -862, -1247, -1247,   659,   663, -1247,   223, -1247,  -425,
   -1247, -1247, -1247,  -187, -1247,   672, -1247,  -184, -1247,   674,
   -1247,  -181, -1247,   675, -1247,  -189, -1247, -1247,   409, -1247,
   -1247, -1247, -1247, -1247,   797,  -345, -1247, -1247,  -383, -1247,
   -1247,  -782, -1247,  -296, -1247, -1247,  -786, -1247, -1247,   679,
   -1247, -1247,   621, -1247,   622, -1247, -1247,   226,  -600,   232,
     242,   243,   691, -1247, -1247, -1247, -1247, -1247,   709, -1247,
   -1247, -1247, -1247,   717, -1247, -1247,   718, -1247, -1247,   719,
   -1247, -1247,   724,  -188,  -351,   111, -1247, -1247, -1247, -1247,
   -1247, -1247, -1247, -1247, -1247, -1247,   859, -1247,   537,  -233,
   -1247,  -116,  -211, -1247, -1247,  -103, -1247,    98, -1247, -1247,
   -1247,  -814, -1247, -1247, -1247,   525,   -23,   876, -1247, -1247,
     540, -1098,  -521, -1247, -1028,   881, -1247, -1247, -1247,   -62,
    -338, -1247, -1247, -1247
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   235,   620,   237,   880,   238,   470,
     239,   240,   419,   420,   241,   348,   242,   243,   894,   589,
     503,   590,   504,   695,   890,   591,   809,   971,   592,   810,
     893,  1038,  1039,  1121,   811,   812,   813,   895,   109,   215,
     382,   456,   922,   609,   749,   819,   712,   713,   714,   715,
     716,   717,   718,   905,  1041,   719,   720,   721,   910,   722,
     723,   914,  1051,  1131,  1210,   724,  1096,   725,   917,  1053,
     726,   727,   920,  1056,   489,   351,    41,   136,   245,   427,
     428,   429,   615,   430,   431,   617,   729,   730,  1183,  1184,
    1185,  1186,  1031,  1032,   874,   383,   667,  1187,  1232,   673,
     668,  1188,   870,  1021,   448,   449,  1153,   450,  1150,   451,
     452,  1157,   453,   649,   650,   651,   858,  1111,  1109,  1114,
    1112,  1191,  1281,  1335,  1343,  1282,  1350,  1288,  1353,  1358,
    1289,  1363,  1305,  1328,  1276,  1336,  1337,  1344,  1345,  1338,
    1330,  1189,    42,   252,   353,   534,    44,   354,   253,   138,
     247,   538,   248,   441,   624,   435,   436,   621,   619,   254,
     255,   445,   446,   634,   542,   630,   845,   631,   853,    46,
      47,    48,    49,    50,    51,   454,   140,    52,    53,   256,
     437,   557,    55,   143,   271,   468,   455,   147,   273,   471,
      56,   257,    58,   149,   203,   298,   299,   492,    59,    60,
     275,   276,   787,   277,   278,   279,   258,   259,   457,   876,
     938,   375,    62,   152,   284,   285,   482,   478,   965,   738,
     680,   881,  1033,    63,    64,    65,   290,   486,  1161,  1203,
    1204,  1294,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,   210,    80,   318,   319,
     506,   320,   321,   509,   939,   955,   461,   659,   943,   520,
     746,   739,   933,  1140,  1141,  1142,   740,   741,  1062,    81,
      82,    83,   260,    84,   261,    85,    86,   262,   770,   263,
     264,   265,    87,    88,   162,   324,   325,   703,    89,   292,
     293,   294,   295,    90,   303,   304,    91,   308,   309,    92,
     313,   314,    93,    94,   327,   599,    95,   164,   331,   332,
     514,    96,   182,    97,   183,   184,   940,   218,   219,   837,
      99,   186,   334,   335,   516,   336,   191,   342,   343,   927,
     928,   742,   743,   100,   221,   222,   605,   941,   102,   224,
     225,   942,  1234,   103,   748,   328,   105,   523,   848,   849,
    1006,   524,  1067,  1148
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     115,   236,   270,   672,    61,   344,   479,   146,   532,   337,
      57,   842,   339,   347,   291,   187,   188,   945,   531,   312,
     302,   711,   929,  1022,   326,   307,   434,   737,   519,   932,
     190,    54,   934,    45,   907,  1104,   871,   911,   906,   485,
     613,   972,   789,   803,   808,   697,     1,  1171,   918,   349,
     926,   205,   512,   207,   925,  1119,   772,  1110,  1327,  1110,
    1115,     2,   464,   144,   144,   144,   822,     2,   825,    15,
     828,     4,   831,   447,   833,     5,   114,     4,     5,   192,
       1,   193,  1179,   194,   195,     5,   196,   704,    19,   106,
    1325,     1,   198,   199,   301,   200,   201,   202,   204,   202,
     206,   202,   208,   209,   134,   211,   249,   212,    33,  1179,
     518,   189,     5,    34,    40,   250,   710,   338,   189,   421,
     340,   267,    26,   246,  1403,  1193,    29,   216,   266,   217,
     220,   223,   251,   226,   289,  -632,    17,  1059,  1060,   145,
     145,   145,   421,   251,  1115,     5,    15,    17,    21,   333,
     899,    19,   903,   306,   704,   903,    26,  1181,   903,   425,
      29,   903,  -634,   333,   903,     2,     5,    26,  1018,   370,
     311,    29,  1017,    36,   467,     4,    34,   345,   706,   707,
     908,  1057,   709,   912,  1181,    33,   915,  1084,   350,  1044,
     352,   484,  1046,  1043,   525,   246,   996,   357,   998,   359,
     316,   708,   709,   362,   363,  1300,   282,  1054,  -635,   364,
     365,   366,   367,   626,   368,   326,   369,   202,   371,  1066,
     734,   246,   433,  1101,  1366,  1199,  1103,   374,   283,   274,
     485,  1122,   378,   379,   288,   380,   958,   627,  -636,   968,
     970,   384,   963,   682,   975,   386,   387,   731,   977,   661,
     389,   390,   980,   982,   983,   392,   393,   986,   987,   924,
     891,   989,   950,   398,   850,    21,   953,   401,  1134,  1162,
     403,     7,  1135,  1136,  1137,  1138,   560,   405,     5,   408,
     409,   411,   412,   414,   600,   415,    12,   144,   670,   422,
      36,   442,     5,   671,   476,   459,  1147,  1296,     5,  1124,
    1126,   704,     5,  1123,  1168,   323,   338,  1170,   249,   340,
    1213,  1214,   483,   903,    10,   903,   297,   250,     5,   903,
     859,   860,   861,   317,    24,     5,  1098,   704,   708,   709,
    1139,   706,   707,  1118,   341,  1045,     5,  1175,  1047,   502,
    1048,  1176,  1177,  1178,  1049,   704,   513,     5,   883,  1194,
     493,  1340,  1325,   496,   708,   709,   499,  1202,   315,   316,
     526,   476,    10,   145,   346,    31,    32,   443,  1323,     5,
     527,   528,   -30,     5,   463,   473,  1205,   352,   533,   296,
     536,   229,  1155,   251,   537,   539,   540,   687,   541,   444,
     735,   868,  1090,  1091,  1092,  1093,   689,   693,   546,  1180,
     547,   548,   691,   549,   762,   551,   552,   459,   554,   555,
     326,   556,   558,    31,    32,     5,   562,   563,   564,  1299,
       5,   374,  1211,   903,   704,  1325,   569,   570,  1064,   707,
     573,   574,   664,   851,   670,   531,   665,   579,   580,   671,
     582,   583,     5,   585,   586,  1125,  1127,  1128,  1129,   213,
     214,   708,   709,   596,   597,   598,   532,   134,  1319,   931,
     603,   707,   604,  1220,   607,   882,   608,     7,  1237,  1238,
     807,  1226,  1227,   654,   656,  1166,   755,   670,   447,   757,
     653,   655,   671,   708,   709,   633,     5,     5,   477,   676,
     678,   423,   424,   518,   751,  1134,   675,   677,   490,  1135,
    1136,  1137,  1138,  1339,   664,   424,   517,   686,   665,  1356,
     652,     5,   521,   657,  1332,   660,  1325,  1195,   662,   663,
     522,  1198,  1206,  1207,  1208,     5,   674,   107,   108,  1340,
    1325,   694,   329,   330,   696,  1026,   587,   685,   588,  1027,
    1028,  1029,   733,   688,   423,     5,   690,   433,  1175,   692,
     233,   234,  1176,  1177,  1178,   752,   747,  1139,     5,   805,
     698,  1318,   533,   421,   -51,   533,   588,   537,  1321,   759,
     921,   -59,  1182,   628,   629,  1192,   -59,   -59,   -59,  1196,
    1175,   246,   433,  1201,  1176,  1177,  1178,   764,  1295,   134,
       5,  1146,   765,   766,   767,  1325,   768,   769,   771,   769,
     708,   709,   774,   855,   856,   857,   134,   228,   884,   229,
    1180,   230,   231,   232,   233,   234,   587,   141,   785,   786,
     788,   786,   150,   790,   153,   791,   155,   793,   157,   795,
     159,  1283,   705,  1290,   588,   913,   944,  1065,   916,   696,
    1239,  1108,  1180,   709,  1175,   469,   815,   618,  1176,  1177,
    1178,   623,  1359,  1359,     5,   418,  1298,  1332,   816,  1325,
     818,   864,   804,   821,   973,   824,   807,   827,   969,   830,
     666,   830,  1074,  1075,   598,   228,   897,   836,   875,   230,
     231,   232,   604,   604,   154,   608,   156,   900,   158,   844,
     160,   228,   993,   994,   847,   230,   231,   232,   233,   234,
     805,   901,   806,  1042,   854,   612,  1180,  1132,  1334,  1334,
    1342,  1342,   728,  1349,  1303,  1334,   839,   840,   728,  1219,
    1175,  1108,   728,   684,  1176,  1177,  1178,  1179,  1106,  1107,
       5,   134,   228,  1222,    10,  1113,   230,   231,   232,   704,
    1347,  1197,   705,   706,   707,  1361,   889,  1354,    43,   696,
     892,   287,   228,   758,   229,   251,   230,   231,   232,    17,
     622,   935,  1020,  1020,   165,   166,   708,   709,   710,   286,
     167,   168,   421,   421,   502,  1058,   169,  1405,   862,   280,
    1408,   866,  1180,  1024,  1409,    31,    32,  1412,   170,   281,
     566,  1413,   171,   683,   759,   872,   844,  1050,   878,  1052,
    1292,   172,  1181,   173,   174,   769,  1143,   507,   175,   769,
    1083,   954,   807,   956,   957,   786,   268,   269,   960,   896,
     176,   786,  1432,   699,   964,   791,   228,  1434,   229,   700,
     230,   231,   232,   233,   234,   696,   892,   806,   177,   701,
     702,   830,   835,   976,   476,   830,   178,   179,   180,   979,
     981,   830,     5,   181,   985,   830,   807,   988,   830,    98,
     990,   704,   410,   991,   705,   706,   707,  1433,   836,   836,
     995,   406,   608,   735,   999,  1000,   101,  1002,   736,   413,
    1416,   104,  1008,   863,     0,     0,  1005,  1007,   708,   709,
     710,  1329,     0,     0,   807,  1346,  1346,     0,  1351,  1355,
     873,  1360,  1360,  1130,  1364,     0,     0,   458,     0,   228,
       0,     0,  1037,   230,   231,   232,   233,   234,   474,   475,
     806,     0,     0,     0,     0,     0,     0,   488,     0,     0,
     898,     0,   902,   502,   495,   902,     0,   498,   902,     0,
     501,   902,     0,     0,   902,   421,   421,  1063,   511,     0,
     728,     0,     0,  1069,  1070,  1071,  1072,     0,   936,     0,
     954,   954,  1406,     0,     0,  1020,   228,     0,   229,  1410,
     230,   231,   232,   233,   234,     0,  1209,     0,  1082,  1414,
       0,   696,     0,  1415,     0,  1086,  1087,  1088,     0,  1089,
     830,   830,   830,     0,  1094,     0,  1095,  1108,     0,     0,
     608,     0,  1100,  1102,     0,   844,     5,     0,     0,  1005,
    1005,     0,     0,     0,     0,   704,     0,     0,   705,   706,
     707,   134,   228,     3,   229,  1120,   230,   231,   232,   233,
     234,     6,     0,     0,     0,     0,     0,  1020,     0,     0,
       8,     0,   708,   709,   710,     0,     0,     9,     0,     0,
      11,     0,  1149,     0,  1152,     0,   608,  1156,     0,     0,
       0,  1159,  1160,   696,     0,    16,  1163,  1164,  1016,  1165,
       0,     0,   830,     0,  1167,     0,    18,     0,  1030,    20,
       0,    22,     0,   844,     0,     0,   844,     0,     0,  1172,
    1173,     0,    25,   902,    27,   902,    28,     0,    30,   902,
       0,     0,     0,     0,    35,     0,     0,     0,  1020,   728,
     635,   636,   637,   638,   639,   640,   641,   642,   643,   728,
    1175,     0,     0,     0,  1176,  1177,  1178,  1179,     0,     0,
       5,     0,  1217,     0,  1218,     0,  1152,   608,  1221,   704,
    1156,  1223,   705,   706,   707,  1225,   608,     0,  1229,  1200,
       0,     0,     0,     0,  1231,  1233,     0,     0,     0,   460,
     462,     0,     0,   465,   466,     0,   708,   709,   710,     0,
       0,     0,     0,     0,     0,     0,     0,   487,     0,     0,
    1302,  1231,  1180,     0,   494,     0,  1407,   497,     0,     0,
     500,     0,     0,  1411,     0,     0,     0,     0,   510,     0,
       0,     0,  1181,   902,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   936,     0,     0,     0,     0,  1304,
    1306,  1307,  1308,  1309,     0,     0,  1311,  1312,  1313,  1314,
    1315,   476,     0,  1317,     0,   608,     0,   844,     0,     5,
       0,     0,   608,     0,     0,     0,     0,     0,   704,     0,
       0,   705,   706,   707,     0,     0,     0,     0,     0,     0,
     735,     0,     0,   728,   923,   736,   728,     0,     0,     0,
     728,     0,  1370,     0,   728,   708,   709,   710,   110,  1375,
       0,  1378,     0,     0,     0,   111,   112,  1382,   113,  1385,
       0,   116,     0,   117,  1389,   118,     0,  1392,     0,   119,
       0,   120,  1396,   121,     0,   122,     0,   123,     0,   124,
       0,   125,     0,   126,  1404,     0,     0,     0,     0,     0,
       0,   127,     0,     0,     0,     0,     0,     0,   128,     0,
     129,   728,   130,     0,   131,     0,   132,     0,   133,   728,
       0,     0,     0,     0,  1233,     0,     0,   728,     0,  1420,
    1422,     0,     0,  1425,   635,   636,   637,   638,   639,   640,
     641,   642,   643,   644,   645,   646,   647,   648,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     197,     0,   135,   137,   139,   139,   142,     0,     0,   148,
     139,   151,   139,   148,   139,   148,   139,   148,   139,   148,
     161,   163,     0,   185,   185,   185,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1326,     0,
    1333,  1333,  1341,  1341,   227,  1348,  1352,  1333,  1357,  1357,
       0,  1362,   300,     0,   305,     0,   310,  1175,     0,   322,
       0,  1176,  1177,  1178,  1179,     0,     0,     5,     0,     0,
       0,     0,     0,     0,     3,     0,   704,     0,     0,   705,
     706,   707,     6,     0,     0,   244,     0,     0,     0,     0,
     272,     8,     0,     0,     0,     0,     0,     0,     9,     0,
       0,   355,     0,   708,   709,   710,   356,     0,   358,     0,
      14,   360,   361,     0,     0,     0,    16,     0,     0,  1180,
       0,     0,     0,     0,     0,     0,     0,    18,     0,     0,
      20,     0,    22,   372,   373,     0,     0,     0,   376,  1181,
       0,   377,     0,    25,     0,    27,     0,    28,   381,    30,
       0,     0,   385,     0,     0,    35,     0,   388,     0,     0,
     476,     0,   391,     0,     0,     0,     0,   394,     5,     0,
       0,     0,   397,     0,   400,     0,     0,   704,   402,     0,
     705,   706,   707,     0,     0,   404,     0,     0,   407,   735,
       0,     0,     0,   417,     0,     0,   416,     0,     0,     5,
     432,     0,   438,   439,   708,   709,   710,     0,   704,     0,
       3,   705,   706,   707,     0,     0,     0,     5,     6,   472,
       0,     0,     0,     0,   480,   736,   704,     8,     0,   705,
     706,   707,     0,     0,     9,   708,   709,   710,     0,     0,
       0,     0,     0,   426,     0,     0,    14,     0,   440,   137,
       0,     0,    16,   708,   709,   710,     0,     0,     0,     0,
       0,     0,     0,    18,     0,     0,    20,     0,    22,   481,
     137,     0,     0,     0,     0,     0,     0,     0,   491,    25,
       0,    27,   529,    28,   530,    30,     0,     0,     0,   535,
     505,    35,     0,   508,     0,     0,     0,     0,     0,   543,
     515,     0,     0,   545,     0,     0,   544,     0,     0,     0,
       0,     0,     0,   553,   550,     5,     0,     0,     0,     0,
     561,     0,     0,     0,   704,     0,     0,   705,   706,   707,
       0,     0,   567,     0,  1200,   572,     0,     0,   571,     0,
       0,   577,   578,     0,     0,   581,     0,     0,   584,     0,
       0,   708,   709,   710,     0,   593,     0,     5,   595,     0,
       0,     0,     0,   601,     0,   602,   704,     5,     0,   705,
     706,   707,     0,   606,   611,     0,   704,   610,     0,   705,
     706,   707,     0,   736,     0,   747,     0,   625,     0,     0,
       0,   632,     0,   708,   709,   710,     0,     0,     0,     0,
       0,     0,     0,   708,   709,   710,   669,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   681,     0,     0,
       0,     0,   614,   616,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   658,     0,     0,     0,     0,   732,     0,
       0,     0,     0,     0,   744,     0,     0,     0,   750,     0,
     679,     0,     0,     0,   753,     0,   754,     0,     0,     0,
     756,     0,     0,     0,     0,     0,   760,     0,     0,     0,
       0,     0,     0,   763,   761,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   745,
       0,     0,     0,   185,     0,     0,   773,     0,     0,   775,
     776,   777,   778,     0,     0,     0,     0,     0,     0,     0,
       0,   781,     0,   784,     0,     0,   783,     0,   395,   396,
       0,   399,   792,     0,   794,     0,     0,   797,     0,   799,
       0,   801,     0,     0,     0,     0,     0,   814,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   817,
       0,   820,     0,     0,   823,     0,   826,     0,   829,     0,
     832,     0,     0,   834,     0,     0,     0,     0,     0,     0,
       0,   838,     0,     0,   841,     0,   843,     0,     0,     0,
       0,   846,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   865,
       0,   867,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   877,     0,   879,     0,
       0,     0,     0,     0,     0,   885,     0,   886,     0,   887,
       0,   888,     0,     0,   852,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   869,     0,   869,     0,     0,     0,
     559,     0,     0,     0,     0,     0,   930,     0,   565,     0,
       0,     0,     0,   568,   937,     0,     0,     0,     0,     0,
     575,   576,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   904,     0,     0,   909,     0,   594,     0,
       0,     0,     0,     0,     0,   919,     0,     0,     0,     0,
       0,   679,     0,     0,   679,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   978,     0,     0,     0,     0,   984,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   992,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   997,  1001,     0,  1003,     0,     0,     0,     0,     0,
       0,  1004,     0,     0,  1009,  1010,  1011,  1012,  1013,  1014,
       0,     0,  1015,     0,  1019,     0,     0,  1023,     0,     0,
       0,     0,  1025,     0,  1034,  1035,  1036,     0,     0,     0,
       0,     0,     0,  1040,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1068,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   779,   780,     0,
       0,     0,     0,   782,  1055,  1080,     0,     0,     0,     0,
    1085,     0,     0,     0,     0,  1061,     0,     0,   796,     0,
     798,     0,   800,     0,   802,  1097,     0,  1099,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1105,     0,
       0,     0,     0,     0,     0,  1116,  1117,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1133,     0,     0,     0,     0,
       0,     0,  1144,     0,     0,     0,     0,  1145,     0,     0,
       0,     0,     0,  1151,     0,  1154,     0,     0,  1158,     0,
       0,     0,     0,   869,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1169,     0,  1174,   679,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1212,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1215,
       0,  1216,     0,     0,  1190,     0,     0,  1190,     0,     0,
       0,  1190,     0,     0,     0,     0,     0,  1228,     0,  1230,
       0,     0,   869,     0,     0,     0,     0,     0,     0,   946,
     947,   948,   949,  1291,   951,   952,     0,     0,     0,     0,
       0,  1297,     0,  1301,   959,     0,   961,   962,     0,     0,
       0,     0,     0,     0,   966,   967,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   974,   869,   869,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1293,     0,     0,     0,     0,     0,     0,     0,     0,
    1310,     0,     0,     0,     0,     0,     0,  1316,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1324,  1322,  1331,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1365,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1372,     0,     0,     0,     0,     0,     0,
    1377,     0,     0,     0,     0,     0,     0,     0,  1384,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1401,  1402,     1,     2,     0,
       0,     0,     0,     3,     0,     0,     0,  1073,     4,     0,
       5,     6,  1076,  1077,     0,  1078,     0,     7,     0,     0,
       8,  1079,     0,     0,  1081,     0,     0,     9,    10,     0,
      11,     0,    12,  1418,     0,     0,     0,    13,     0,    14,
    1427,     0,    15,     0,     0,    16,     0,     0,     0,     0,
       0,     0,     0,    17,     0,     0,    18,     0,    19,    20,
      21,    22,  1436,     0,     0,     0,     0,     0,     0,    23,
      24,     0,    25,    26,    27,     0,    28,    29,    30,    31,
      32,    33,     0,    34,    35,    36,  1277,  1240,  1241,  1242,
    1243,  1244,  1245,  1246,  1278,  1247,  1248,  1249,  1250,  1251,
    1252,  1253,  1254,  1255,  1256,  1257,  1258,  1259,  1260,  1261,
    1262,  1263,  1264,  1265,  1266,  1267,  1268,  1269,  1270,  1279,
    1280,  1271,  1272,  1273,  1274,  1275,  1284,  1240,  1241,  1242,
    1243,  1244,  1245,  1246,  1285,  1247,  1248,  1249,  1250,  1251,
    1252,  1253,  1254,  1255,  1256,  1257,  1258,  1259,  1260,  1261,
    1262,  1263,  1264,  1265,  1266,  1267,  1268,  1269,  1270,  1286,
    1287,  1271,  1272,  1273,  1274,  1275,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1224,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1235,  1236,  1240,  1241,  1242,  1243,
    1244,  1245,  1246,     0,  1247,  1248,  1249,  1250,  1251,  1252,
    1253,  1254,  1255,  1256,  1257,  1258,  1259,  1260,  1261,  1262,
    1263,  1264,  1265,  1266,  1267,  1268,  1269,  1270,     0,     0,
    1271,  1272,  1273,  1274,  1275,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1320,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1367,     0,     0,  1368,  1369,     0,
    1371,     0,     0,     0,  1373,  1374,     0,  1376,     0,  1379,
       0,     0,  1380,  1381,     0,  1383,     0,  1386,     0,  1387,
    1388,     0,  1390,  1391,     0,  1393,  1394,     0,  1395,     0,
    1397,  1398,     0,  1399,     0,  1400,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1417,     0,  1419,  1421,     0,  1423,
    1424,  1426,     0,  1428,  1429,  1430,  1431,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1435,     0,  1437
};

static const yytype_int16 yycheck[] =
{
      57,   192,   196,   558,     0,   227,   374,   115,   434,   220,
       0,   742,   223,   242,   201,   131,   132,   853,   434,   208,
     204,   598,   836,   957,   212,   206,   352,   604,   411,   838,
     133,     0,   841,     0,   821,  1063,   783,   824,   821,   379,
     528,   893,   678,   694,   697,   592,     4,  1145,   832,   243,
     836,   156,   403,   158,   836,  1082,   656,  1069,  1304,  1071,
    1072,     5,   365,     6,     6,     6,   720,     5,   722,    49,
     724,    15,   726,    16,   728,    17,   164,    15,    17,   136,
       4,   138,    14,   140,   141,    17,   143,    26,    65,     0,
      22,     4,   149,   150,    71,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   147,   162,    19,   164,    88,    14,
      25,    40,    17,    90,     0,    28,    55,   220,    40,   346,
     223,   195,    80,    61,  1370,  1152,    84,   184,   195,   186,
     187,   188,    56,   190,   201,   164,    60,   999,  1000,    82,
      82,    82,   369,    56,  1156,    17,    49,    60,    67,    78,
     816,    65,   818,    72,    26,   821,    80,    89,   824,   350,
      84,   827,   164,    78,   830,     5,    17,    80,   954,   274,
      73,    84,   954,    92,   368,    15,    90,   235,    30,    31,
     821,   995,    54,   824,    89,    88,   827,  1039,   245,   976,
     247,   378,   979,   976,   416,    61,   927,   254,   929,   256,
      51,    53,    54,   260,   261,  1233,     3,   991,   164,   266,
     267,   268,   269,   539,   271,   403,   273,   274,   275,  1005,
     603,    61,    62,  1059,  1322,  1159,  1062,   284,    25,   198,
     570,  1083,   289,   290,   201,   292,   872,   540,   164,   890,
     893,   298,   878,   569,   898,   302,   303,   598,   902,   552,
     307,   308,   906,   907,   908,   312,   313,   911,   912,   836,
     807,   915,   862,   320,   752,    67,   866,   324,     7,  1121,
     327,    24,    11,    12,    13,    14,   470,   334,    17,   336,
     337,   338,   339,   340,   513,   342,    39,     6,     7,   347,
      92,   356,    17,    12,     9,    48,  1105,  1231,    17,  1086,
    1087,    26,    17,  1086,  1140,    64,   409,  1143,    19,   412,
    1172,  1173,   377,   979,    35,   981,   202,    28,    17,   985,
     765,   766,   767,   209,    77,    17,  1057,    26,    53,    54,
      69,    30,    31,  1080,    41,   976,    17,     7,   979,   397,
     981,    11,    12,    13,   985,    26,   404,    17,   793,  1152,
     386,    21,    22,   389,    53,    54,   392,  1160,    50,    51,
     418,     9,    35,    82,   151,    86,    87,   357,  1302,    17,
     427,   428,   151,    17,   364,   371,  1163,   434,   435,    23,
     437,   150,  1113,    56,   441,   442,   443,   574,   445,    57,
      38,   774,  1046,  1047,  1048,  1049,   580,   586,   455,    69,
     457,   458,   583,   460,   633,   462,   463,    48,   465,   466,
     598,   468,   469,    86,    87,    17,   473,   474,   475,  1233,
      17,   478,  1169,  1089,    26,    22,   483,   484,  1005,    31,
     487,   488,     7,   759,     7,   851,    11,   494,   495,    12,
     497,   498,    17,   500,   501,  1086,  1087,  1088,  1089,   162,
     163,    53,    54,   510,   511,   512,   882,   147,  1294,   149,
     517,    31,   519,  1194,   521,   791,   523,    24,  1215,  1216,
     697,  1202,  1203,   547,   548,  1129,   619,     7,    16,   622,
     547,   548,    12,    53,    54,   543,    17,    17,   374,   563,
     564,    32,    33,    25,   610,     7,   563,   564,   384,    11,
      12,    13,    14,  1307,     7,    33,   408,   574,    11,  1313,
     546,    17,   414,   549,    20,   551,    22,  1154,   554,   555,
      47,  1158,  1163,  1164,  1165,    17,   562,   160,   161,    21,
      22,   589,    74,    75,   592,     7,   156,   573,   158,    11,
      12,    13,   600,   579,    32,    17,   582,    62,     7,   585,
     155,   156,    11,    12,    13,   612,    45,    69,    17,   157,
     596,  1292,   619,   790,   156,   622,   158,   624,  1299,   626,
     147,   157,  1149,    58,    59,  1152,   162,   163,   164,  1156,
       7,    61,    62,  1160,    11,    12,    13,   644,  1229,   147,
      17,   149,   649,   650,   651,    22,   653,   654,   655,   656,
      53,    54,   659,   152,   153,   154,   147,   148,   795,   150,
      69,   152,   153,   154,   155,   156,   156,   112,   675,   676,
     677,   678,   117,   680,   119,   682,   121,   684,   123,   686,
     125,  1221,    29,  1223,   158,   826,   847,  1005,   829,   697,
    1217,     8,    69,    54,     7,   369,   703,   533,    11,    12,
      13,   537,  1314,  1315,    17,   346,  1233,    20,   715,    22,
     717,   769,   695,   720,   894,   722,   893,   724,   891,   726,
     556,   728,  1017,  1018,   731,   148,   816,   734,   786,   152,
     153,   154,   739,   740,   120,   742,   122,   817,   124,   746,
     126,   148,   925,   926,   751,   152,   153,   154,   155,   156,
     157,   818,   159,   976,   762,   527,    69,  1095,  1306,  1307,
    1308,  1309,   598,  1311,  1238,  1313,   739,   740,   604,  1193,
       7,     8,   608,   571,    11,    12,    13,    14,  1066,  1067,
      17,   147,   148,  1197,    35,  1071,   152,   153,   154,    26,
    1309,  1156,    29,    30,    31,  1315,   804,  1312,     0,   807,
     808,   201,   148,   624,   150,    56,   152,   153,   154,    60,
     536,   843,   956,   957,   129,   129,    53,    54,    55,    70,
     129,   129,   999,  1000,   832,   997,   129,  1375,   768,   199,
    1378,   771,    69,   960,  1382,    86,    87,  1385,   129,   199,
     478,  1389,   129,   570,   851,   785,   853,   988,   788,   990,
    1225,   129,    89,   129,   129,   862,  1102,   398,   129,   866,
    1039,   868,  1039,   870,   871,   872,   195,   195,   875,   815,
     129,   878,  1420,   597,   881,   882,   148,  1425,   150,   597,
     152,   153,   154,   155,   156,   893,   894,   159,   129,   597,
     597,   898,   731,   900,     9,   902,   129,   129,   129,   906,
     907,   908,    17,   129,   911,   912,  1083,   914,   915,     0,
     917,    26,   337,   920,    29,    30,    31,  1422,   925,   926,
     927,   334,   929,    38,   931,   932,     0,   934,    43,   339,
    1401,     0,   944,   769,    -1,    -1,   943,   944,    53,    54,
      55,  1304,    -1,    -1,  1121,  1308,  1309,    -1,  1311,  1312,
     786,  1314,  1315,  1094,  1317,    -1,    -1,   361,    -1,   148,
      -1,    -1,   970,   152,   153,   154,   155,   156,   372,   373,
     159,    -1,    -1,    -1,    -1,    -1,    -1,   381,    -1,    -1,
     816,    -1,   818,   991,   388,   821,    -1,   391,   824,    -1,
     394,   827,    -1,    -1,   830,  1172,  1173,  1004,   402,    -1,
     836,    -1,    -1,  1010,  1011,  1012,  1013,    -1,   844,    -1,
    1017,  1018,  1375,    -1,    -1,  1159,   148,    -1,   150,  1382,
     152,   153,   154,   155,   156,    -1,  1167,    -1,  1035,  1392,
      -1,  1039,    -1,  1396,    -1,  1042,  1043,  1044,    -1,  1046,
    1047,  1048,  1049,    -1,  1051,    -1,  1053,     8,    -1,    -1,
    1057,    -1,  1059,  1060,    -1,  1062,    17,    -1,    -1,  1066,
    1067,    -1,    -1,    -1,    -1,    26,    -1,    -1,    29,    30,
      31,   147,   148,    10,   150,  1083,   152,   153,   154,   155,
     156,    18,    -1,    -1,    -1,    -1,    -1,  1231,    -1,    -1,
      27,    -1,    53,    54,    55,    -1,    -1,    34,    -1,    -1,
      37,    -1,  1109,    -1,  1111,    -1,  1113,  1114,    -1,    -1,
      -1,  1118,  1119,  1121,    -1,    52,  1123,  1124,   954,  1126,
      -1,    -1,  1129,    -1,  1131,    -1,    63,    -1,   964,    66,
      -1,    68,    -1,  1140,    -1,    -1,  1143,    -1,    -1,  1146,
    1147,    -1,    79,   979,    81,   981,    83,    -1,    85,   985,
      -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,  1302,   995,
      93,    94,    95,    96,    97,    98,    99,   100,   101,  1005,
       7,    -1,    -1,    -1,    11,    12,    13,    14,    -1,    -1,
      17,    -1,  1189,    -1,  1191,    -1,  1193,  1194,  1195,    26,
    1197,  1198,    29,    30,    31,  1202,  1203,    -1,  1205,    36,
      -1,    -1,    -1,    -1,  1211,  1212,    -1,    -1,    -1,   362,
     363,    -1,    -1,   366,   367,    -1,    53,    54,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   380,    -1,    -1,
    1237,  1238,    69,    -1,   387,    -1,  1377,   390,    -1,    -1,
     393,    -1,    -1,  1384,    -1,    -1,    -1,    -1,   401,    -1,
      -1,    -1,    89,  1089,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1100,    -1,    -1,    -1,    -1,  1276,
    1277,  1278,  1279,  1280,    -1,    -1,  1283,  1284,  1285,  1286,
    1287,     9,    -1,  1290,    -1,  1292,    -1,  1294,    -1,    17,
      -1,    -1,  1299,    -1,    -1,    -1,    -1,    -1,    26,    -1,
      -1,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    -1,    -1,  1149,    42,    43,  1152,    -1,    -1,    -1,
    1156,    -1,  1329,    -1,  1160,    53,    54,    55,    45,  1336,
      -1,  1338,    -1,    -1,    -1,    52,    53,  1344,    55,  1346,
      -1,    58,    -1,    60,  1351,    62,    -1,  1354,    -1,    66,
      -1,    68,  1359,    70,    -1,    72,    -1,    74,    -1,    76,
      -1,    78,    -1,    80,  1372,    -1,    -1,    -1,    -1,    -1,
      -1,    88,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,
      97,  1217,    99,    -1,   101,    -1,   103,    -1,   105,  1225,
      -1,    -1,    -1,    -1,  1401,    -1,    -1,  1233,    -1,  1406,
    1407,    -1,    -1,  1410,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     147,    -1,   109,   110,   111,   112,   113,    -1,    -1,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,    -1,   130,   131,   132,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1304,    -1,
    1306,  1307,  1308,  1309,   191,  1311,  1312,  1313,  1314,  1315,
      -1,  1317,   203,    -1,   205,    -1,   207,     7,    -1,   210,
      -1,    11,    12,    13,    14,    -1,    -1,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    10,    -1,    26,    -1,    -1,    29,
      30,    31,    18,    -1,    -1,   192,    -1,    -1,    -1,    -1,
     197,    27,    -1,    -1,    -1,    -1,    -1,    -1,    34,    -1,
      -1,   248,    -1,    53,    54,    55,   253,    -1,   255,    -1,
      46,   258,   259,    -1,    -1,    -1,    52,    -1,    -1,    69,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,
      66,    -1,    68,   280,   281,    -1,    -1,    -1,   285,    89,
      -1,   288,    -1,    79,    -1,    81,    -1,    83,   295,    85,
      -1,    -1,   299,    -1,    -1,    91,    -1,   304,    -1,    -1,
       9,    -1,   309,    -1,    -1,    -1,    -1,   314,    17,    -1,
      -1,    -1,   319,    -1,   321,    -1,    -1,    26,   325,    -1,
      29,    30,    31,    -1,    -1,   332,    -1,    -1,   335,    38,
      -1,    -1,    -1,   344,    -1,    -1,   343,    -1,    -1,    17,
     351,    -1,   353,   354,    53,    54,    55,    -1,    26,    -1,
      10,    29,    30,    31,    -1,    -1,    -1,    17,    18,   370,
      -1,    -1,    -1,    -1,   375,    43,    26,    27,    -1,    29,
      30,    31,    -1,    -1,    34,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,   350,    -1,    -1,    46,    -1,   355,   356,
      -1,    -1,    52,    53,    54,    55,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    63,    -1,    -1,    66,    -1,    68,   376,
     377,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   385,    79,
      -1,    81,   429,    83,   431,    85,    -1,    -1,    -1,   436,
     397,    91,    -1,   400,    -1,    -1,    -1,    -1,    -1,   446,
     407,    -1,    -1,   454,    -1,    -1,   453,    -1,    -1,    -1,
      -1,    -1,    -1,   464,   461,    17,    -1,    -1,    -1,    -1,
     471,    -1,    -1,    -1,    26,    -1,    -1,    29,    30,    31,
      -1,    -1,   479,    -1,    36,   486,    -1,    -1,   485,    -1,
      -1,   492,   493,    -1,    -1,   496,    -1,    -1,   499,    -1,
      -1,    53,    54,    55,    -1,   506,    -1,    17,   509,    -1,
      -1,    -1,    -1,   514,    -1,   516,    26,    17,    -1,    29,
      30,    31,    -1,   520,   525,    -1,    26,   524,    -1,    29,
      30,    31,    -1,    43,    -1,    45,    -1,   538,    -1,    -1,
      -1,   542,    -1,    53,    54,    55,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    53,    54,    55,   557,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   568,    -1,    -1,
      -1,    -1,   529,   530,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   550,    -1,    -1,    -1,    -1,   599,    -1,
      -1,    -1,    -1,    -1,   605,    -1,    -1,    -1,   609,    -1,
     567,    -1,    -1,    -1,   615,    -1,   617,    -1,    -1,    -1,
     621,    -1,    -1,    -1,    -1,    -1,   627,    -1,    -1,    -1,
      -1,    -1,    -1,   634,   631,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   606,
      -1,    -1,    -1,   610,    -1,    -1,   657,    -1,    -1,   660,
     661,   662,   663,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   668,    -1,   674,    -1,    -1,   673,    -1,   317,   318,
      -1,   320,   683,    -1,   685,    -1,    -1,   688,    -1,   690,
      -1,   692,    -1,    -1,    -1,    -1,    -1,   698,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   716,
      -1,   718,    -1,    -1,   721,    -1,   723,    -1,   725,    -1,
     727,    -1,    -1,   730,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   738,    -1,    -1,   741,    -1,   743,    -1,    -1,    -1,
      -1,   748,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   770,
      -1,   772,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   787,    -1,   789,    -1,
      -1,    -1,    -1,    -1,    -1,   796,    -1,   798,    -1,   800,
      -1,   802,    -1,    -1,   761,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   781,    -1,   783,    -1,    -1,    -1,
     469,    -1,    -1,    -1,    -1,    -1,   837,    -1,   477,    -1,
      -1,    -1,    -1,   482,   845,    -1,    -1,    -1,    -1,    -1,
     489,   490,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   820,    -1,    -1,   823,    -1,   507,    -1,
      -1,    -1,    -1,    -1,    -1,   832,    -1,    -1,    -1,    -1,
      -1,   838,    -1,    -1,   841,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   905,    -1,    -1,    -1,    -1,   910,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   922,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   928,   933,    -1,   935,    -1,    -1,    -1,    -1,    -1,
      -1,   942,    -1,    -1,   945,   946,   947,   948,   949,   950,
      -1,    -1,   953,    -1,   955,    -1,    -1,   958,    -1,    -1,
      -1,    -1,   963,    -1,   965,   966,   967,    -1,    -1,    -1,
      -1,    -1,    -1,   974,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1006,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   666,   667,    -1,
      -1,    -1,    -1,   672,   991,  1032,    -1,    -1,    -1,    -1,
    1041,    -1,    -1,    -1,    -1,  1002,    -1,    -1,   687,    -1,
     689,    -1,   691,    -1,   693,  1056,    -1,  1058,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1065,    -1,
      -1,    -1,    -1,    -1,    -1,  1076,  1077,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1096,    -1,    -1,    -1,    -1,
      -1,    -1,  1103,    -1,    -1,    -1,    -1,  1104,    -1,    -1,
      -1,    -1,    -1,  1110,    -1,  1112,    -1,    -1,  1115,    -1,
      -1,    -1,    -1,  1080,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1142,    -1,  1148,  1105,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1171,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1186,
      -1,  1188,    -1,    -1,  1151,    -1,    -1,  1154,    -1,    -1,
      -1,  1158,    -1,    -1,    -1,    -1,    -1,  1204,    -1,  1210,
      -1,    -1,  1169,    -1,    -1,    -1,    -1,    -1,    -1,   858,
     859,   860,   861,  1224,   863,   864,    -1,    -1,    -1,    -1,
      -1,  1232,    -1,  1234,   873,    -1,   875,   876,    -1,    -1,
      -1,    -1,    -1,    -1,   883,   884,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   896,  1215,  1216,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1228,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1281,    -1,    -1,    -1,    -1,    -1,    -1,  1288,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1303,  1300,  1305,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1319,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1330,    -1,    -1,    -1,    -1,    -1,    -1,
    1337,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1345,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1366,  1367,     4,     5,    -1,
      -1,    -1,    -1,    10,    -1,    -1,    -1,  1016,    15,    -1,
      17,    18,  1021,  1022,    -1,  1024,    -1,    24,    -1,    -1,
      27,  1030,    -1,    -1,  1033,    -1,    -1,    34,    35,    -1,
      37,    -1,    39,  1404,    -1,    -1,    -1,    44,    -1,    46,
    1411,    -1,    49,    -1,    -1,    52,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    -1,    -1,    63,    -1,    65,    66,
      67,    68,  1433,    -1,    -1,    -1,    -1,    -1,    -1,    76,
      77,    -1,    79,    80,    81,    -1,    83,    84,    85,    86,
      87,    88,    -1,    90,    91,    92,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1199,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1213,  1214,   108,   109,   110,   111,
     112,   113,   114,    -1,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,    -1,    -1,
     142,   143,   144,   145,   146,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1296,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1323,    -1,    -1,  1326,  1327,    -1,
    1329,    -1,    -1,    -1,  1333,  1334,    -1,  1336,    -1,  1338,
      -1,    -1,  1341,  1342,    -1,  1344,    -1,  1346,    -1,  1348,
    1349,    -1,  1351,  1352,    -1,  1354,  1355,    -1,  1357,    -1,
    1359,  1360,    -1,  1362,    -1,  1364,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1403,    -1,  1405,  1406,    -1,  1408,
    1409,  1410,    -1,  1412,  1413,  1414,  1415,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1432,    -1,  1434
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     4,     5,    10,    15,    17,    18,    24,    27,    34,
      35,    37,    39,    44,    46,    49,    52,    60,    63,    65,
      66,    67,    68,    76,    77,    79,    80,    81,    83,    84,
      85,    86,    87,    88,    90,    91,    92,   166,   167,   168,
     239,   241,   307,   308,   311,   313,   334,   335,   336,   337,
     338,   339,   342,   343,   345,   347,   355,   356,   357,   363,
     364,   375,   377,   388,   389,   390,   397,   398,   399,   400,
     401,   402,   403,   404,   405,   406,   407,   408,   409,   410,
     412,   434,   435,   436,   438,   440,   441,   447,   448,   453,
     458,   461,   464,   467,   468,   471,   476,   478,   481,   485,
     498,   502,   503,   508,   510,   511,     0,   160,   161,   203,
     203,   203,   203,   203,   164,   205,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   147,   206,   242,   206,   314,   206,
     341,   341,   206,   348,     6,    82,   351,   352,   206,   358,
     341,   206,   378,   341,   358,   341,   358,   341,   358,   341,
     358,   206,   449,   206,   472,   336,   337,   338,   339,   363,
     389,   390,   400,   404,   408,   434,   447,   453,   458,   461,
     464,   467,   477,   479,   480,   206,   486,   486,   486,    40,
     490,   491,   205,   205,   205,   205,   205,   203,   205,   205,
     205,   205,   205,   359,   205,   359,   205,   359,   205,   205,
     411,   205,   205,   162,   163,   204,   205,   205,   482,   483,
     205,   499,   500,   205,   504,   505,   205,   203,   148,   150,
     152,   153,   154,   155,   156,   169,   170,   171,   173,   175,
     176,   179,   181,   182,   206,   243,    61,   315,   317,    19,
      28,    56,   308,   313,   324,   325,   344,   356,   371,   372,
     437,   439,   442,   444,   445,   446,   324,   344,   437,   439,
     173,   349,   206,   353,   345,   365,   366,   368,   369,   370,
     371,   372,     3,    25,   379,   380,    70,   311,   313,   324,
     391,   398,   454,   455,   456,   457,    23,   239,   360,   361,
     204,    71,   402,   459,   460,   204,    72,   406,   462,   463,
     204,    73,   410,   465,   466,    50,    51,   239,   413,   414,
     416,   417,   204,    64,   450,   451,   468,   469,   510,    74,
      75,   473,   474,    78,   487,   488,   490,   487,   490,   487,
     490,    41,   492,   493,   168,   181,   151,   169,   180,   173,
     205,   240,   205,   309,   312,   203,   203,   205,   203,   205,
     203,   203,   205,   205,   205,   205,   205,   205,   205,   205,
     359,   205,   203,   203,   205,   376,   203,   203,   205,   205,
     205,   203,   205,   260,   205,   203,   205,   205,   203,   205,
     205,   203,   205,   205,   203,   260,   260,   203,   205,   260,
     203,   205,   203,   205,   203,   205,   483,   203,   205,   205,
     500,   205,   205,   505,   205,   205,   203,   204,   174,   177,
     178,   182,   181,    32,    33,   170,   206,   244,   245,   246,
     248,   249,   204,    62,   315,   320,   321,   345,   204,   204,
     206,   318,   314,   356,    57,   326,   327,    16,   269,   270,
     272,   274,   275,   277,   340,   351,   206,   373,   373,    48,
     419,   421,   419,   356,   340,   419,   419,   173,   350,   172,
     174,   354,   204,   375,   373,   373,     9,   239,   382,   384,
     204,   206,   381,   314,   398,   277,   392,   419,   373,   239,
     239,   206,   362,   269,   419,   373,   269,   419,   373,   269,
     419,   373,   181,   185,   187,   206,   415,   413,   206,   418,
     419,   373,   469,   181,   475,   206,   489,   492,    25,   423,
     424,   492,    47,   512,   516,   168,   181,   205,   205,   203,
     203,   309,   312,   205,   310,   203,   205,   205,   316,   205,
     205,   205,   329,   203,   203,   204,   205,   205,   205,   205,
     203,   205,   205,   204,   205,   205,   205,   346,   205,   260,
     173,   204,   205,   205,   205,   260,   376,   203,   260,   205,
     205,   203,   204,   205,   205,   260,   260,   204,   204,   205,
     205,   204,   205,   205,   204,   205,   205,   156,   158,   184,
     186,   190,   193,   204,   260,   204,   205,   205,   205,   470,
     169,   204,   204,   205,   205,   501,   203,   205,   205,   208,
     203,   204,   245,   248,   206,   247,   206,   250,   239,   323,
     170,   322,   320,   239,   319,   204,   315,   340,    58,    59,
     330,   332,   204,   181,   328,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   278,
     279,   280,   269,   324,   344,   324,   344,   269,   206,   422,
     269,   340,   269,   269,     7,    11,   239,   261,   265,   204,
       7,    12,   259,   264,   269,   324,   344,   324,   344,   206,
     385,   204,   315,   392,   278,   269,   324,   398,   269,   402,
     269,   406,   269,   410,   181,   188,   181,   194,   269,   442,
     444,   445,   446,   452,    26,    29,    30,    31,    53,    54,
      55,   209,   211,   212,   213,   214,   215,   216,   217,   220,
     221,   222,   224,   225,   230,   232,   235,   236,   239,   251,
     252,   469,   204,   181,   423,    38,    43,   209,   384,   426,
     431,   432,   496,   497,   204,   206,   425,    45,   509,   209,
     204,   486,   205,   204,   204,   310,   204,   310,   316,   205,
     204,   203,   169,   204,   205,   205,   205,   205,   205,   205,
     443,   205,   443,   204,   205,   204,   204,   204,   204,   260,
     260,   203,   260,   203,   204,   205,   205,   367,   205,   367,
     205,   205,   204,   205,   204,   205,   260,   204,   260,   204,
     260,   204,   260,   186,   184,   157,   159,   182,   183,   191,
     194,   199,   200,   201,   204,   205,   205,   203,   205,   210,
     203,   205,   210,   203,   205,   210,   203,   205,   210,   203,
     205,   210,   203,   210,   203,   470,   205,   484,   203,   501,
     501,   203,   208,   203,   205,   331,   203,   205,   513,   514,
     248,   315,   206,   333,   181,   152,   153,   154,   281,   281,
     281,   281,   356,   239,   351,   204,   356,   204,   423,   206,
     267,   267,   356,   239,   259,   351,   374,   204,   356,   204,
     172,   386,   315,   281,   398,   204,   204,   204,   204,   181,
     189,   194,   181,   195,   183,   202,   375,   213,   239,   251,
     216,   220,   239,   251,   206,   218,   224,   230,   235,   206,
     223,   230,   235,   170,   226,   235,   170,   233,   185,   206,
     237,   147,   207,    42,   209,   426,   431,   494,   495,   496,
     204,   149,   385,   427,   385,   322,   239,   204,   375,   419,
     481,   502,   506,   423,   487,   331,   260,   260,   260,   260,
     443,   260,   260,   443,   205,   420,   205,   205,   367,   260,
     205,   260,   260,   367,   205,   383,   260,   260,   186,   202,
     183,   192,   199,   195,   260,   210,   205,   210,   204,   205,
     210,   205,   210,   210,   204,   205,   210,   210,   205,   210,
     205,   205,   204,   484,   484,   205,   208,   203,   208,   205,
     205,   204,   205,   204,   204,   205,   515,   205,   514,   204,
     204,   204,   204,   204,   204,   204,   239,   426,   431,   204,
     173,   268,   268,   204,   374,   204,     7,    11,    12,    13,
     239,   257,   258,   387,   204,   204,   204,   181,   196,   197,
     204,   219,   221,   224,   230,   235,   230,   235,   235,   235,
     170,   227,   170,   234,   185,   206,   238,   496,   168,   386,
     386,   206,   433,   205,   209,   384,   431,   517,   204,   205,
     205,   205,   205,   260,   420,   420,   260,   260,   260,   260,
     203,   260,   205,   169,   199,   204,   205,   205,   205,   205,
     210,   210,   210,   210,   205,   205,   231,   204,   208,   204,
     205,   331,   205,   331,   509,   203,   515,   515,     8,   283,
     285,   282,   285,   283,   284,   285,   204,   204,   267,   282,
     181,   198,   199,   224,   230,   235,   230,   235,   235,   235,
     170,   228,   261,   204,     7,    11,    12,    13,    14,    69,
     428,   429,   430,   428,   204,   203,   149,   385,   518,   205,
     273,   203,   205,   271,   203,   208,   205,   276,   203,   205,
     205,   393,   199,   205,   205,   205,   210,   205,   331,   203,
     331,   506,   205,   205,   204,     7,    11,    12,    13,    14,
      69,    89,   209,   253,   254,   255,   256,   262,   266,   306,
     206,   286,   209,   282,   306,   286,   209,   284,   286,   268,
      36,   209,   306,   394,   395,   230,   235,   235,   235,   170,
     229,   267,   204,   386,   386,   203,   203,   205,   205,   271,
     208,   205,   276,   205,   260,   205,   208,   208,   203,   205,
     204,   205,   263,   205,   507,   260,   260,   267,   267,   209,
     108,   109,   110,   111,   112,   113,   114,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   142,   143,   144,   145,   146,   299,   107,   115,   140,
     141,   287,   290,   299,   107,   115,   140,   141,   292,   295,
     299,   204,   394,   206,   396,   235,   268,   204,   209,   496,
     509,   204,   205,   263,   205,   297,   205,   205,   205,   205,
     204,   205,   205,   205,   205,   205,   204,   205,   208,   331,
     260,   208,   203,   268,   204,    22,   239,   262,   298,   304,
     305,   204,    20,   239,   254,   288,   300,   301,   304,   288,
      21,   239,   254,   289,   302,   303,   304,   289,   239,   254,
     291,   304,   239,   293,   300,   304,   288,   239,   294,   302,
     304,   294,   239,   296,   304,   204,   506,   260,   260,   260,
     205,   260,   203,   260,   260,   205,   260,   203,   205,   260,
     260,   260,   205,   260,   203,   205,   260,   260,   260,   205,
     260,   260,   205,   260,   260,   260,   205,   260,   260,   260,
     260,   204,   204,   262,   181,   254,   304,   170,   254,   254,
     304,   170,   254,   254,   304,   304,   507,   260,   204,   260,
     205,   260,   205,   260,   260,   205,   260,   204,   260,   260,
     260,   260,   254,   259,   254,   260,   204,   260
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
     357,   357,   358,   359,   359,   359,   359,   360,   361,   362,
     363,   364,   364,   365,   365,   365,   365,   366,   367,   367,
     367,   367,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   376,   376,   377,   378,   379,   380,   380,   381,
     382,   383,   383,   383,   384,   385,   386,   387,   388,   388,
     389,   390,   391,   391,   392,   393,   393,   393,   393,   393,
     394,   395,   396,   397,   397,   398,   399,   399,   399,   400,
     401,   401,   402,   403,   403,   404,   405,   406,   407,   407,
     408,   409,   410,   411,   411,   411,   411,   411,   412,   412,
     413,   414,   415,   415,   416,   417,   418,   419,   420,   420,
     420,   420,   421,   422,   423,   424,   425,   426,   427,   427,
     427,   428,   429,   430,   430,   430,   430,   430,   430,   431,
     432,   433,   434,   434,   434,   435,   435,   436,   437,   437,
     438,   439,   439,   440,   441,   442,   443,   443,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   452,   452,
     452,   453,   454,   454,   455,   456,   457,   458,   459,   460,
     461,   462,   463,   464,   465,   466,   467,   468,   468,   468,
     468,   468,   468,   468,   468,   468,   468,   468,   468,   469,
     469,   470,   470,   470,   471,   472,   473,   474,   474,   475,
     475,   475,   476,   477,   477,   478,   479,   479,   479,   479,
     479,   479,   479,   479,   479,   479,   479,   479,   479,   479,
     480,   480,   480,   480,   480,   480,   480,   481,   482,   482,
     483,   484,   484,   484,   484,   484,   484,   484,   485,   486,
     487,   488,   489,   490,   491,   492,   493,   494,   495,   496,
     497,   498,   499,   499,   500,   501,   501,   501,   501,   501,
     502,   503,   504,   504,   505,   506,   506,   506,   506,   507,
     507,   507,   507,   508,   509,   510,   511,   512,   513,   513,
     514,   515,   515,   515,   515,   516,   517,   518,   518
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
       3,     3,     1,     1,     5,     1,     1,     4,     6,     4,
       6,     1,     5,     1,     1,     1,     1,     1,     1,     7,
       1,     1,     1,     1,     1,     1,     1,    10,     1,     1,
      10,     1,     1,    10,    10,     7,     0,     3,     3,     9,
       7,     9,    10,     1,     1,     9,     1,     1,     1,     1,
       1,    10,     1,     1,     7,     9,     1,    10,     7,     1,
      10,     7,     1,    10,     7,     1,     9,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     3,     2,     1,     1,     4,     1,     1,     1,
       2,     3,     4,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     3,     1,
       8,     0,     3,     3,     3,     5,     3,     2,     1,     1,
       4,     1,     1,     4,     1,     4,     1,     4,     1,     4,
       1,     4,     3,     1,     6,     0,     3,     3,     3,     2,
       1,     4,     3,     1,    16,     1,     1,     1,     1,     0,
       6,     3,     2,     1,     1,     9,     1,     4,     3,     1,
       4,     0,     3,     3,     2,     1,     4,     4,     4
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
