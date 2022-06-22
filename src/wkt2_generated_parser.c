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
#define YYLAST   3195

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  165
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  355
/* YYNRULES -- Number of rules.  */
#define YYNRULES  701
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1442

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
    1122,  1123,  1124,  1126,  1130,  1132,  1134,  1134,  1138,  1143,
    1143,  1143,  1147,  1152,  1152,  1154,  1158,  1158,  1162,  1167,
    1169,  1173,  1173,  1177,  1182,  1184,  1188,  1189,  1190,  1191,
    1192,  1194,  1194,  1196,  1199,  1201,  1201,  1203,  1205,  1207,
    1211,  1217,  1218,  1219,  1220,  1222,  1224,  1228,  1233,  1235,
    1238,  1245,  1246,  1247,  1249,  1253,  1259,  1259,  1259,  1259,
    1259,  1259,  1263,  1268,  1270,  1275,  1275,  1276,  1278,  1278,
    1280,  1287,  1287,  1289,  1296,  1296,  1298,  1305,  1312,  1317,
    1318,  1319,  1321,  1327,  1332,  1340,  1346,  1348,  1350,  1356,
    1358,  1358,  1359,  1359,  1363,  1369,  1369,  1371,  1376,  1382,
    1387,  1393,  1398,  1403,  1409,  1414,  1419,  1425,  1430,  1435,
    1441,  1441,  1442,  1442,  1443,  1443,  1444,  1444,  1445,  1445,
    1446,  1446,  1449,  1449,  1451,  1452,  1453,  1455,  1457,  1461,
    1464,  1464,  1467,  1468,  1469,  1471,  1475,  1476,  1478,  1480,
    1480,  1481,  1481,  1482,  1482,  1482,  1483,  1484,  1484,  1485,
    1485,  1486,  1486,  1488,  1488,  1489,  1489,  1490,  1491,  1491,
    1495,  1499,  1500,  1503,  1508,  1509,  1510,  1511,  1512,  1513,
    1514,  1516,  1518,  1520,  1523,  1525,  1527,  1529,  1531,  1533,
    1535,  1537,  1539,  1541,  1546,  1550,  1551,  1554,  1559,  1560,
    1561,  1562,  1563,  1565,  1570,  1575,  1576,  1579,  1585,  1585,
    1585,  1585,  1587,  1588,  1589,  1590,  1592,  1594,  1599,  1605,
    1607,  1612,  1613,  1616,  1624,  1625,  1626,  1627,  1629,  1631,
    1636,  1637
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
  "opt_geoid_model_id_list_opt_separator_scope_extent_identifier_remark",
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

#define YYPACT_NINF (-1226)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-640)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1431, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226,
   -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226,
   -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226,
   -1226, -1226, -1226, -1226, -1226, -1226, -1226,    90, -1226, -1226,
   -1226,   275, -1226, -1226, -1226,   275, -1226, -1226, -1226, -1226,
   -1226, -1226,   275,   275, -1226,   275, -1226,    27,   275, -1226,
     275, -1226,   275, -1226, -1226, -1226,   275, -1226,   275, -1226,
     275, -1226,   275, -1226,   275, -1226,   275, -1226,   275, -1226,
     275, -1226, -1226, -1226, -1226, -1226, -1226, -1226,   275, -1226,
   -1226, -1226, -1226, -1226, -1226,   275, -1226,   275, -1226,   275,
   -1226,   275, -1226,   275, -1226,   275, -1226, -1226, -1226,    81,
      81,    81,    81,    81, -1226,    39,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    81,  1229,
      81,    81,    81,   233, -1226, -1226,    27, -1226,    27, -1226,
      27,    27, -1226,    27, -1226, -1226, -1226,   275, -1226,    27,
      27, -1226,    27,    27,    27,    27,    27,    27,    27,    27,
      27, -1226,    27, -1226,    27, -1226, -1226, -1226, -1226,   124,
   -1226, -1226, -1226, -1226, -1226,   174,   194,   212, -1226, -1226,
   -1226, -1226,   339, -1226,    27, -1226,    27,    27,    27, -1226,
      27,   275,  1273,   266,   524,   524,   682,    81,   341,   326,
     271,   423,   172,   339,   260,   339,   247,   339,   216,    74,
     339,   327,  1333, -1226, -1226, -1226,   460,    82, -1226, -1226,
      82, -1226, -1226,    82, -1226, -1226,   369,  1229, -1226, -1226,
   -1226, -1226, -1226, -1226, -1226,   491, -1226, -1226, -1226, -1226,
     291,   306,   315,   682, -1226,    27, -1226,    27,   275, -1226,
   -1226, -1226, -1226,   275,    27,   275,    27, -1226,   275,   275,
      27,    27, -1226, -1226, -1226, -1226,    27,    27,    27,    27,
   -1226,    27, -1226,    27,    27,    27, -1226, -1226, -1226, -1226,
     275,   275, -1226, -1226,    27,   275, -1226, -1226,   275,    27,
      27, -1226,    27, -1226, -1226,   275, -1226,    27,    27,   275,
   -1226, -1226,    27,    27,   275, -1226, -1226,    27,    27,   275,
   -1226, -1226,    27,    27,   275, -1226, -1226,    27,    27,   275,
      27,   275, -1226, -1226,    27,   275, -1226,    27, -1226, -1226,
   -1226, -1226,   275, -1226,    27,   275,    27,    27,    27,    27,
      27, -1226,    27,   275,   339, -1226,   422,   491, -1226, -1226,
     598,   339,   113,   339,   339,    81,    81,    54,   420,    88,
      81,    81,   437,   437,    54,    88,   437,   437,   682,   422,
     339,   472,    81,    81,   136,   339,    81,    81,   307,   501,
     437,    81,   490, -1226,   490,    81,   501,   437,    81,   501,
     437,    81,   501,   437,    81, -1226, -1226,   560,    93, -1226,
      81,   437,    81,  1333,   491,   233, -1226,    81,   369,   233,
   -1226,   523,   233, -1226,   369,   479,  1229, -1226,   491, -1226,
   -1226, -1226, -1226, -1226, -1226, -1226, -1226,    27,    27,   275,
   -1226,   275, -1226, -1226,    27,    27,   275,    27, -1226, -1226,
   -1226,    27,    27,    27, -1226,    27,   275, -1226, -1226, -1226,
   -1226, -1226, -1226,   275,   339,    27, -1226,    27,    27, -1226,
      27,   275,    27,    27,   339,    27,    27, -1226,    27,    27,
     682,   339, -1226,    27,    27,    27, -1226,    27,    27,   275,
   -1226, -1226,    27,    27,    27,   275,   339,    27,    27,    27,
      27, -1226,   339,   339,    27,    27,   339,    27,    27,   339,
      27,    27, -1226, -1226,   264, -1226,   339,    27, -1226,   339,
      27,    27,    27,   315,   339, -1226,   339,    27, -1226,    27,
     275,    27, -1226,    27,   275,   339, -1226,   532,   537,    81,
      81, -1226, -1226,   490, -1226,  1134,   478,   490,   339,   266,
      88,   555,   339,   491,  1105, -1226,   501,    83,    83,   501,
      81,   501,    88, -1226,   501,   501,   513,   339,   130, -1226,
   -1226, -1226,   501,    83,    83, -1226, -1226,    81,   339,   266,
     501,  1679, -1226,   501,   426, -1226, -1226, -1226, -1226,   501,
     242, -1226,   501,   256, -1226,   501,    70, -1226, -1226,   491,
   -1226, -1226,   491, -1226, -1226, -1226,   501,   326,  1521,   339,
     491, -1226, -1226,   523,  1287,   339,    81,   511,  1890,   339,
      81, -1226,    27, -1226, -1226,   339, -1226,   339, -1226,    27,
   -1226,   339,    27, -1226,    27, -1226,    27,   339, -1226, -1226,
   -1226,   275, -1226,   315,   339, -1226, -1226, -1226, -1226, -1226,
   -1226, -1226, -1226, -1226,    27, -1226, -1226, -1226, -1226,    27,
      27,    27, -1226,    27,    27,    27,    27,   339, -1226,    27,
     339,   339,   339,   339, -1226, -1226,    27,    27,   275, -1226,
   -1226, -1226,    27,   275,   339,    27,    27,    27,    27, -1226,
      27, -1226,    27,   339,    27,   339,    27,    27,   339,    27,
     339,    27,   339,    27,   466,   425, -1226,   881,   339, -1226,
   -1226, -1226, -1226,    27, -1226, -1226, -1226, -1226, -1226, -1226,
   -1226, -1226, -1226, -1226, -1226,    27,   275,    27,   275, -1226,
      27,   275,    27,   275,    27,   275,    27,   275,    27, -1226,
     275,    27, -1226, -1226,    27, -1226, -1226, -1226,   275,    27,
      27,   275,    27,   275, -1226, -1226,    27, -1226,   275, -1226,
   -1226,    27,   537, -1226, -1226, -1226, -1226, -1226, -1226,   263,
   -1226,    81,   491, -1226,   588,   588,   588,   588,    54,   170,
     339,    54,   339, -1226,   523, -1226, -1226, -1226, -1226, -1226,
   -1226,    81, -1226,    81, -1226,    54,   149,   339,    54,   339,
     422,   619, -1226,   588, -1226,   307,   339, -1226,   339, -1226,
     339, -1226,   339, -1226,   491, -1226, -1226,   491,   491, -1226,
     444, -1226, -1226, -1226, -1226,   472,   240,   599,   739, -1226,
      81,   469, -1226,    81,   556, -1226,  1134,   152, -1226,  1134,
     335, -1226,   560, -1226,   470, -1226,   989,   339,   485, -1226,
   -1226,    81, -1226,  1134,   490,   339,   439,    48, -1226, -1226,
   -1226,    27, -1226,    27, -1226, -1226, -1226, -1226,    27,    27,
      27,    27,    27,    27,    27, -1226,    27, -1226,    27, -1226,
      27,    27,    27,    27, -1226,    27,    27, -1226,    27, -1226,
   -1226,    27,    27,    27,    27, -1226, -1226, -1226, -1226, -1226,
     484,   444, -1226,   881,   491, -1226,    27, -1226,    27, -1226,
      27, -1226,    27, -1226, -1226,   339,    27,    27,    27, -1226,
     339,    27,    27, -1226,    27,    27, -1226,    27, -1226, -1226,
      27, -1226,   339, -1226, -1226,    27,    27,    27,   275,    27,
   -1226,    27,    27,   339,    27,   339, -1226, -1226, -1226, -1226,
   -1226, -1226,   339,    27,    27,   339,   339,   339,   339,   339,
     339, -1226, -1226,   339,   163,   339,   682,   682,   339, -1226,
     130, -1226, -1226,   339,   795,   339,   339,   339, -1226, -1226,
     491, -1226, -1226, -1226,   339, -1226,   507, -1226, -1226,   556,
   -1226,   152, -1226, -1226, -1226,   152, -1226, -1226,  1134, -1226,
    1134,   560, -1226, -1226, -1226,  1697, -1226,  1229, -1226,   422,
     422, -1226,    81, -1226,    27,  1608,   339,   523, -1226, -1226,
      27,    27,    27,    27, -1226, -1226,    27,    27,    27, -1226,
   -1226,    27,    27, -1226,    27, -1226, -1226, -1226, -1226, -1226,
      27, -1226,   275,    27, -1226,    27, -1226, -1226, -1226,   923,
   -1226,   339,    27,    27,    27, -1226,    27,    27,    27,    27,
   -1226,    27, -1226,    27, -1226, -1226,   339,    27,   339,    27,
      27, -1226,    27,   511, -1226,   275,    27,    27, -1226,   628,
     628,   628,   628, -1226, -1226, -1226,   339,   339, -1226, -1226,
      81, -1226,   628,  1009, -1226, -1226,   488,   677,   597,   152,
   -1226, -1226, -1226, -1226,  1134,   540,   339, -1226, -1226, -1226,
     323, -1226,   304,   339,   275,   515, -1226, -1226, -1226,    27,
     275,    27,   275,    27,    27,   275, -1226, -1226,    27,    27,
     288,  1009, -1226,    27,    27, -1226,    27, -1226, -1226,    27,
   -1226,    27, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226,
      27, -1226,   275,    27, -1226,   439,    27,    27,   339,  1163,
   -1226,    81,   919, -1226,    81, -1226,   911, -1226,    81,   682,
    1097, -1226, -1226,   677,   597,   597, -1226,  1134, -1226,    81,
   -1226,   339,   422,   422, -1226, -1226, -1226, -1226, -1226, -1226,
   -1226, -1226, -1226, -1226, -1226, -1226,   275, -1226,   275,    27,
   -1226,    27, -1226,    27,    27,    27, -1226,    27,    27,    27,
   -1226, -1226,    27,    27,   275,    27, -1226, -1226, -1226, -1226,
     339,    27,    27,    27,    27,    81,    81,  1890,  1767, -1226,
   -1226,  1562, -1226,  1728,   339,  1731, -1226,  1731, -1226,    81,
     597, -1226,   682,   339,   835,   339, -1226, -1226,    27,    27,
   -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226,
   -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226,
   -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226, -1226,
   -1226, -1226, -1226, -1226, -1226, -1226, -1226,    27,    27,    27,
      27,    27,   339, -1226,    27,    27,    27,    27,    27,   339,
   -1226,    27, -1226,    27, -1226,    27, -1226,    27, -1226,    27,
   -1226, -1226,    27,   275, -1226,   682,   339,   118,   339,   546,
     546,   412,   412, -1226,   578,   554,   546,   510,   510, -1226,
     373, -1226, -1226,   339, -1226, -1226,   439,    27, -1226, -1226,
      27,    27, -1226,    27,   275, -1226, -1226,    27,    27, -1226,
      27,   275,    27, -1226, -1226,    27,    27, -1226,    27,   275,
      27, -1226,    27,    27, -1226,    27,    27, -1226,    27,    27,
   -1226,    27, -1226,    27,    27, -1226,    27, -1226,    27, -1226,
     339,   339, -1226, -1226,   140, -1226,   491, -1226, -1226,   578,
   -1226,  1134,   280, -1226, -1226, -1226,   578, -1226,  1134,   280,
   -1226, -1226, -1226,   280, -1226, -1226,   373, -1226, -1226, -1226,
     373, -1226, -1226, -1226, -1226,    27, -1226,    27,   339,    27,
      27,    27,    27,    27,    27,   339,    27,    27,    27,    27,
   -1226, -1226, -1226, -1226,   280, -1226,   477, -1226, -1226,   280,
   -1226, -1226, -1226, -1226, -1226, -1226,    27,   339,    27, -1226,
   -1226, -1226
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,   419,   406,   395,   405,   161,   431,   454,   397,   486,
     489,   607,   651,   686,   689,   511,   504,   356,   566,   496,
     493,   501,   499,   618,   673,   396,   421,   432,   398,   420,
     487,   491,   490,   512,   497,   494,   502,     0,     4,     5,
       2,     0,    13,   346,   347,     0,   590,   385,   383,   384,
     386,   387,     0,     0,     3,     0,    12,   416,     0,   592,
       0,    11,     0,   594,   468,   469,     0,    14,     0,   596,
       0,    15,     0,   598,     0,    16,     0,   600,     0,    17,
       0,   591,   547,   545,   546,   548,   549,   593,     0,   595,
     597,   599,   601,    19,    18,     0,     7,     0,     8,     0,
       9,     0,    10,     0,     6,     0,     1,    73,    74,     0,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    78,   162,     0,   357,     0,   394,
       0,     0,   407,     0,   411,   412,   417,     0,   422,     0,
       0,   455,     0,     0,   423,     0,   423,     0,   423,     0,
     506,   567,     0,   608,     0,   619,   633,   620,   634,   621,
     622,   636,   623,   624,   625,   626,   627,   628,   629,   630,
     631,   632,     0,   616,     0,   652,     0,     0,     0,   657,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    75,    76,   615,     0,     0,   640,   642,
       0,   664,   666,     0,   674,   676,     0,     0,    40,    20,
      37,    38,    39,    41,    42,     0,   163,    21,    22,    26,
       0,    25,    35,     0,   164,   154,   361,     0,     0,   446,
     447,   369,   400,     0,     0,     0,     0,   399,     0,     0,
       0,     0,   551,   554,   552,   555,     0,     0,     0,     0,
     408,     0,   413,     0,   423,     0,   433,   434,   435,   436,
       0,     0,   458,   457,   451,     0,   579,   473,     0,     0,
       0,   472,     0,   575,   576,     0,   428,   190,   424,     0,
     488,   582,     0,     0,     0,   495,   585,     0,     0,     0,
     500,   588,     0,     0,     0,   518,   514,   190,   190,     0,
     190,     0,   505,   569,     0,     0,   602,     0,   603,   610,
     611,   617,     0,   654,     0,     0,     0,     0,     0,     0,
       0,   659,     0,     0,     0,    34,    27,     0,    33,    23,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   425,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   510,   509,     0,     0,   507,
       0,     0,     0,     0,     0,     0,   641,     0,     0,     0,
     665,     0,     0,   675,     0,     0,     0,   656,     0,    29,
      31,    28,    36,   168,   171,   165,   166,   155,   158,     0,
     160,     0,   153,   365,     0,   351,     0,     0,   348,   353,
     362,   359,     0,     0,   371,   375,     0,   223,   393,   204,
     205,   206,   207,     0,     0,     0,   448,     0,     0,   525,
       0,     0,     0,     0,     0,     0,     0,   409,   402,   190,
       0,     0,   418,     0,     0,     0,   464,   190,   451,     0,
     450,   459,   190,     0,     0,     0,     0,     0,     0,   190,
     190,   429,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    50,   515,    48,   516,     0,   190,   519,     0,
       0,     0,   604,   612,     0,   655,     0,     0,   528,   668,
       0,     0,   698,    80,     0,     0,    32,     0,     0,     0,
       0,   350,   355,     0,   354,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   388,     0,     0,     0,     0,
       0,     0,     0,   390,     0,     0,     0,     0,     0,   415,
      24,   410,     0,     0,     0,   452,   453,     0,     0,     0,
       0,     0,   470,     0,     0,   191,   426,   427,   492,     0,
       0,   498,     0,     0,   503,     0,     0,    44,    58,     0,
      45,    49,     0,   513,   508,   517,     0,     0,     0,     0,
     613,   609,   653,     0,     0,     0,     0,     0,     0,     0,
       0,   658,   156,   159,   169,     0,   172,     0,   367,   351,
     366,     0,   351,   363,   359,   358,     0,     0,   380,   381,
     376,     0,   368,   372,     0,   224,   225,   226,   227,   228,
     229,   230,   231,   232,     0,   233,   234,   235,   236,     0,
       0,     0,   392,     0,   559,     0,   559,     0,   526,     0,
       0,     0,     0,     0,   199,   198,   190,   190,     0,   401,
     197,   196,   190,     0,     0,     0,   438,     0,   438,   465,
       0,   456,     0,     0,     0,     0,     0,   190,     0,   190,
       0,   190,     0,   190,    48,     0,    59,     0,     0,   570,
     571,   572,   573,     0,   174,   100,   133,   136,   144,   148,
      98,   606,    82,    88,    89,    93,     0,    85,     0,    92,
      85,     0,    85,     0,    85,     0,    85,     0,    85,    84,
       0,   604,   589,   614,   644,   543,   663,   672,     0,   668,
     668,     0,    80,     0,   667,   529,   378,   687,     0,    81,
     688,     0,     0,   167,   170,   352,   364,   349,   360,     0,
     389,     0,   373,   370,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   550,     0,   553,   391,   556,   557,   404,
     403,     0,   414,     0,   430,     0,     0,     0,     0,     0,
      27,     0,   471,     0,   574,     0,     0,   580,     0,   583,
       0,   586,     0,    46,     0,    43,    68,     0,     0,    53,
      71,    55,    66,    67,   565,     0,     0,     0,     0,    91,
       0,     0,   117,     0,     0,   118,     0,     0,   119,     0,
       0,   120,     0,    83,     0,   605,     0,     0,     0,   669,
     670,     0,   671,     0,     0,     0,     0,     0,   690,   692,
     157,     0,   382,   378,   374,   237,   238,   239,   190,   190,
     190,   190,   559,   190,   190,   558,   559,   563,   521,   202,
       0,     0,   438,   190,   449,   190,   190,   437,   438,   444,
     466,   461,     0,   190,   190,   577,   581,   584,   587,    52,
      48,    71,    60,     0,     0,    70,   190,    96,    85,    94,
       0,    90,    85,    87,   101,     0,    85,    85,    85,   134,
       0,    85,    85,   137,     0,    85,   145,     0,   149,   150,
       0,    79,     0,   661,   650,   644,   644,    80,     0,    80,
     643,     0,     0,     0,     0,     0,   379,   527,   680,   681,
     678,   679,     0,   694,     0,     0,     0,     0,     0,     0,
       0,   561,   560,     0,     0,     0,     0,     0,     0,   442,
       0,   439,   441,     0,     0,     0,     0,     0,    47,    69,
       0,    54,    57,    72,     0,    95,     0,    86,    99,     0,
     121,     0,   122,   123,   132,     0,   124,   125,     0,   126,
       0,     0,   173,   645,   646,     0,   647,     0,   649,    27,
      27,   530,     0,   662,     0,     0,     0,     0,   691,   377,
       0,     0,     0,     0,   562,   564,   190,   521,   521,   520,
     203,   190,   190,   443,   190,   445,   188,   186,   185,   187,
     190,   467,     0,   190,   460,     0,   578,    64,    56,     0,
     568,     0,   102,   103,   104,   105,    85,    85,    85,    85,
     138,     0,   146,   142,   151,   152,     0,    80,     0,   378,
       0,   544,   378,     0,   697,     0,   694,   694,   693,     0,
       0,     0,     0,   524,   522,   523,     0,     0,   440,   462,
       0,   463,     0,     0,    63,    97,     0,     0,     0,     0,
     127,   128,   129,   130,     0,     0,     0,   147,   648,   660,
       0,   532,     0,     0,     0,     0,   696,   695,   243,   214,
       0,   209,     0,    80,   220,     0,   192,   189,     0,   475,
      65,     0,    61,   106,   107,   108,   109,   110,   111,    85,
     139,     0,   143,   141,   541,   536,   537,   538,   539,   540,
     378,   534,     0,   378,   542,     0,     0,     0,     0,     0,
     213,     0,     0,   208,     0,   218,     0,   219,     0,     0,
       0,   474,    62,     0,     0,     0,   131,     0,   533,     0,
     531,     0,    27,    27,   699,   183,   180,   179,   182,   200,
     181,   201,   217,   345,   175,   177,     0,   176,     0,   215,
     244,     0,   212,   209,    80,     0,   222,   220,     0,   190,
     484,   479,    80,   480,     0,   112,   113,   114,   115,   140,
       0,   194,   682,   190,   190,     0,     0,     0,     0,   211,
     210,     0,   221,     0,     0,     0,   476,     0,   478,     0,
       0,   135,     0,     0,     0,     0,   701,   700,     0,   194,
     216,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   297,   246,   248,
     250,   252,     0,   245,   270,   277,   279,   281,   283,     0,
     276,   293,   184,   480,   482,   480,   485,   378,   116,   190,
     535,   685,    80,     0,   677,     0,     0,     0,     0,     0,
       0,     0,     0,   240,     0,     0,     0,     0,     0,   242,
       0,   477,   481,     0,   195,   684,     0,   190,   193,   344,
     190,   190,   298,   190,     0,   241,   340,   190,   190,   247,
     190,     0,   190,   249,   342,   190,   190,   251,   190,     0,
     190,   253,   190,   190,   271,   190,   190,   278,   190,   190,
     280,   190,   282,   190,   190,   284,   190,   294,   190,   483,
       0,     0,   299,   302,     0,   300,     0,   254,   261,     0,
     258,     0,     0,   260,   262,   269,     0,   266,     0,     0,
     268,   272,   275,     0,   273,   285,     0,   287,   288,   289,
       0,   291,   292,   295,   296,   682,   178,   190,     0,   190,
     190,     0,   190,   190,   190,     0,   190,   190,   190,   190,
     683,   301,   343,   257,     0,   255,     0,   259,   265,     0,
     263,   341,   267,   274,   286,   290,   190,     0,   190,   256,
     339,   264
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1226, -1226, -1226,  -222,  -229,  -191, -1226,   285,  -184,   313,
   -1226, -1226, -1226, -1226, -1226, -1226,   -71,  -344,  -651,   -23,
    -755,  -647, -1226, -1226, -1226, -1226, -1226, -1226, -1226,  -554,
    -219, -1226, -1226, -1226,  -857, -1226, -1226,  -199,  1310,  1596,
     -57,   -56, -1226,  -714,  -577,  -543, -1226, -1226,  -116, -1226,
   -1226,  -114, -1226, -1226, -1226,   -95,  -248, -1226, -1226,  -778,
   -1226, -1226, -1226, -1226, -1226,  -673, -1226, -1226, -1226, -1226,
    -619, -1226, -1226, -1226,   508, -1226, -1226, -1226, -1226, -1226,
     211, -1226, -1226,  -504, -1226, -1226,  -710, -1226, -1226,  -472,
   -1226, -1226, -1226, -1226,  -555,  1757,  -351, -1225,  -492, -1226,
   -1226, -1226,  -767,  -922,   147, -1226,  -444, -1226, -1226, -1226,
   -1226,  -442,  -340,   195, -1226, -1226,  -160, -1002,  -303,  -385,
    -956,  -542, -1226,  -988,  -538, -1226, -1226, -1226, -1226,  -534,
   -1226, -1226, -1226, -1226,  -553,  -529, -1226,  -583, -1226,  -596,
   -1226,  -987, -1226,   787,  -404,  -310,   587,  -426,    37,  -185,
    -320,   165, -1226, -1226, -1226,   255, -1226,   -42, -1226,   -49,
   -1226, -1226, -1226, -1226, -1226, -1226,  -838, -1226, -1226, -1226,
   -1226,   674,   687,   688,   691,  -280,   779, -1226, -1226,   -72,
      41, -1226, -1226, -1226, -1226, -1226,  -108, -1226, -1226, -1226,
   -1226,    10, -1226,   564,  -106, -1226, -1226, -1226,   697, -1226,
   -1226, -1226,  -638, -1226, -1226, -1226,   632,   634,   876,  -133,
       4,   368, -1226, -1226, -1226, -1226, -1226, -1226, -1226,  -368,
    -815,  -911, -1226, -1226,   718,   719, -1226,   283, -1226,  -589,
    -528, -1226, -1226, -1226,  -192, -1226,   727, -1226,  -173, -1226,
     730, -1226,  -186, -1226,   733, -1226,  -197, -1226, -1226,   475,
   -1226, -1226, -1226, -1226, -1226,   286,  -260, -1226, -1226,  -378,
   -1226, -1226,  -792, -1226,  -235, -1226, -1226,  -758, -1226, -1226,
     747, -1226, -1226,   684, -1226,   686, -1226, -1226,   287,  -608,
     296,   297,   298,   754, -1226, -1226, -1226, -1226, -1226,   756,
   -1226, -1226, -1226, -1226,   768, -1226, -1226,   772, -1226, -1226,
     776, -1226, -1226,   777,  -183,  -354,   177, -1226, -1226, -1226,
   -1226, -1226, -1226, -1226, -1226, -1226, -1226,   909, -1226,   577,
    -166, -1226,  -113,  -206, -1226, -1226,  -111, -1226,   -27, -1226,
   -1226, -1226,  -802, -1226, -1226, -1226,   575,    23,   913, -1226,
   -1226,   576, -1103,  -489, -1226, -1012,   918, -1226, -1226, -1226,
     -22,  -287, -1226, -1226, -1226
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
    1185,  1186,  1031,  1032,   874,   383,   667,  1187,  1233,   673,
     668,  1188,   870,  1021,   448,   449,  1153,   450,  1150,   451,
     452,  1157,   453,   649,   650,   651,   858,  1111,  1109,  1114,
    1112,  1191,  1282,  1339,  1347,  1283,  1354,  1289,  1357,  1362,
    1290,  1367,  1308,  1332,  1277,  1340,  1341,  1348,  1349,  1342,
    1334,  1189,    42,   252,   353,   534,    44,   354,   253,   138,
     247,   538,   248,   441,   624,   435,   436,   621,   619,   254,
     255,   445,   446,   634,   542,   630,   845,   631,   853,    46,
      47,    48,    49,    50,    51,   454,   140,    52,    53,   256,
     437,   557,    55,   143,   271,   468,   455,   147,   273,   471,
      56,   257,    58,   149,   203,   298,   299,   492,    59,    60,
     275,   276,   787,   277,   278,   279,   258,   259,   457,   876,
     938,   375,    62,   152,   284,   285,   482,   478,   965,   738,
     680,   881,  1033,    63,    64,    65,   290,   486,  1161,  1228,
    1203,  1204,  1297,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,   210,    80,   318,
     319,   506,   320,   321,   509,   939,   955,   461,   659,   943,
     520,   746,   739,   933,  1140,  1141,  1142,   740,   741,  1062,
      81,    82,    83,   260,    84,   261,    85,    86,   262,   770,
     263,   264,   265,    87,    88,   162,   324,   325,   703,    89,
     292,   293,   294,   295,    90,   303,   304,    91,   308,   309,
      92,   313,   314,    93,    94,   327,   599,    95,   164,   331,
     332,   514,    96,   182,    97,   183,   184,   940,   218,   219,
     837,    99,   186,   334,   335,   516,   336,   191,   342,   343,
     927,   928,   742,   743,   100,   221,   222,   605,   941,   102,
     224,   225,   942,  1235,   103,   748,   328,   105,   523,   848,
     849,  1006,   524,  1067,  1148
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     115,   236,   421,   672,    61,   344,   479,   146,   532,   291,
      57,   312,   270,   347,   337,   945,   871,   339,   187,   188,
     307,   711,   190,   932,   613,   421,   934,   737,   842,   326,
     531,   302,   434,   519,   929,  1022,   972,    45,   697,   485,
     789,    54,  1171,   906,   925,   144,   808,   803,   772,   512,
     205,  1104,   207,   135,   137,   139,   139,   142,     1,   349,
     148,   139,   151,   139,   148,   139,   148,   139,   148,   139,
     148,   161,   163,   518,   185,   185,   185,   918,   926,   192,
    1119,   193,  1331,   194,   195,   464,   196,     1,  1059,  1060,
     106,     5,   198,   199,   144,   200,   201,   202,   204,   202,
     206,   202,   208,   209,   447,   211,   899,   212,   903,   338,
       5,   903,   340,  1110,   903,  1110,  1115,   903,     2,    15,
     903,   145,   189,   267,   315,   316,   333,   216,     4,   217,
     220,   223,  1179,   226,    26,     5,   244,   670,    29,   251,
    1329,   272,   671,    17,   316,   476,   266,     5,   907,  1407,
    1193,   911,   289,     5,  1179,   144,   670,     5,    33,   425,
     333,   671,  1017,    26,   345,  1194,     5,    29,   370,     5,
     145,   442,   476,  1202,   246,   433,   144,   822,   704,   825,
       5,   828,  1084,   831,   467,   833,   484,     5,   350,     5,
     352,   114,   483,  1057,   525,   296,  1018,   357,  1043,   359,
    1115,   735,   908,   362,   363,   912,   709,  1181,   915,   364,
     365,   366,   367,   996,   368,   998,   369,   202,   371,   626,
     326,  1101,  1303,  1370,  1103,   734,  1122,   374,   134,  1181,
     485,   145,   378,   379,   958,   380,  1054,  1199,   288,   274,
     963,   384,   970,   968,   731,   386,   387,  1066,   850,   682,
     389,   390,   145,   891,   950,   392,   393,     5,   953,   924,
     627,  1213,  1214,   398,  1162,    15,   704,   401,     2,   903,
     403,   903,   661,   189,   282,   903,   422,   405,     4,   408,
     409,   411,   412,   414,   600,   415,   560,  1175,  -635,   311,
    1147,  1176,  1177,  1178,   426,   710,   283,     5,   338,   440,
     137,   340,  1168,  1044,    33,  1170,  1046,    19,  1123,   755,
    1299,  1134,   757,  1118,    21,  1135,  1136,  1137,  1138,   306,
     481,   137,  1343,    21,   246,    19,   502,   246,  1360,   491,
    1134,   301,    34,   513,  1135,  1136,  1137,  1138,  -637,    36,
       5,   505,    10,  1098,   508,   249,     2,   526,    36,  1180,
      34,   515,     5,   807,   250,   975,     4,  1045,  -638,   977,
    1047,   704,  1048,   980,   982,   983,  1049,   443,   986,   987,
     527,   528,   989,  1139,   463,   473,  -639,   352,   533,   903,
     536,   517,   687,  1327,   537,   539,   540,   521,   541,   693,
       5,   323,  1139,    31,    32,  1329,   868,   691,   546,  1155,
     547,   548,  1211,   549,   762,   551,   552,   689,   554,   555,
     341,   556,   558,  1124,  1126,   326,   562,   563,   564,  1175,
     587,   374,   588,  1176,  1177,  1178,   569,   570,  1064,     5,
     573,   574,  1302,  1344,  1329,   107,   108,   579,   580,   851,
     582,   583,   346,   585,   586,   -59,   421,   531,  1238,  1239,
     -59,   -59,   -59,   596,   597,   598,   532,   -30,    10,  1323,
     603,    10,   604,     7,   607,   229,   608,  1125,  1127,  1128,
    1129,   882,   633,   614,   616,   654,   656,   444,    12,   251,
    1220,  1180,   251,    17,   670,   459,     5,   459,  1226,   671,
    1205,   676,   678,   286,   658,   704,     7,   751,   653,   655,
     707,   213,   214,  1090,  1091,  1092,  1093,     5,    40,    31,
      32,   679,    31,    32,   675,   677,    24,   447,   694,   707,
     664,   696,   708,   709,   665,   686,   522,     5,     1,   733,
       5,  1344,  1329,   493,   329,   330,   496,   706,   707,   499,
     433,   708,   709,   249,  1206,  1207,  1208,   664,   518,   807,
     745,   665,   250,  1175,   185,   752,   747,  1176,  1177,  1178,
     708,   709,   533,     5,   423,   533,  1336,   537,  1329,   759,
     424,     5,  1182,     5,  1336,  1192,  1329,   233,   234,  1196,
     251,   587,   704,  1201,    17,  1175,  1166,   764,  1325,  1176,
    1177,  1178,   765,   766,   767,     5,   768,   769,   771,   769,
    1329,   805,   774,   884,    26,   859,   860,   861,    29,   708,
     709,  1298,  1195,   628,   629,  1180,  1198,   921,   785,   786,
     788,   786,   -51,   790,   588,   791,   696,   793,   705,   795,
     423,   424,   134,   883,   931,   913,  1108,  1065,   916,   228,
    1240,   944,   588,   230,   231,   232,   815,  1180,   460,   462,
    1294,   709,   465,   466,   469,   421,   421,  1301,   816,   418,
     818,   864,   134,   821,  1146,   824,   487,   827,  1284,   830,
    1291,   830,   804,   494,   598,   973,   497,   836,   875,   500,
     246,   433,   604,   604,   154,   608,   156,   510,   158,   844,
     160,   854,   969,   652,   847,   807,   657,  1293,   660,  1295,
     897,   662,   663,   900,  1321,   852,  1322,   134,   228,   674,
     297,  1333,   230,   231,   232,  1350,  1350,   317,  1355,  1359,
     685,  1364,  1364,   901,  1368,   869,   688,   869,  1042,   690,
     708,   709,   692,   889,  1363,  1363,   696,   892,   612,   807,
     855,   856,   857,   698,  1132,   134,   228,  1306,   229,  1219,
     230,   231,   232,   233,   234,  1222,     5,  1074,  1075,   993,
     994,   502,   839,   840,   904,   704,   684,   909,  1113,   706,
     707,  1197,  1020,  1020,  1351,  1058,   919,   807,   862,  1106,
    1107,   866,   679,  1410,  1365,   679,  1358,    43,   287,   758,
    1414,   622,   708,   709,   759,   872,   844,  1050,   878,  1052,
    1418,   935,  1026,   165,  1419,   769,  1027,  1028,  1029,   769,
    1083,   954,     5,   956,   957,   786,   166,   167,   960,   896,
     168,   786,   696,   892,   964,   791,   169,  1024,   421,   421,
     228,   280,   229,   281,   230,   231,   232,  1338,  1338,  1346,
    1346,   830,  1353,   976,  1338,   830,   566,   170,   171,   979,
     981,   830,     5,   683,   985,   830,   172,   988,   830,   173,
     990,   704,   174,   991,   705,   706,   707,  1143,   836,   836,
     995,  1437,   608,   507,   999,  1000,   175,  1002,   736,   268,
     747,   269,   477,   176,   699,   177,  1005,  1007,   708,   709,
     710,   141,   490,   700,   701,   702,   150,   178,   153,  1037,
     155,   179,   157,  1130,   159,   180,   181,  1409,   835,    98,
    1412,   406,   410,   101,  1413,   413,  1420,  1416,   104,  1108,
     502,  1417,  1008,     0,     0,     0,  1175,  1108,     5,     0,
    1176,  1177,  1178,  1179,     0,  1055,     5,   704,     0,     0,
     705,   706,   707,     0,     0,   704,  1061,  1063,   705,   706,
     707,     0,  1436,  1069,  1070,  1071,  1072,  1438,     0,     0,
     954,   954,     0,     0,   708,   709,   710,     0,   696,     0,
       0,     0,   708,   709,   710,  1020,  1209,     0,  1082,     0,
       0,     0,     0,     0,     0,  1086,  1087,  1088,  1180,  1089,
     830,   830,   830,     0,  1094,     0,  1095,     0,   476,     0,
     608,     0,  1100,  1102,     0,   844,     5,     0,  1181,  1005,
    1005,     0,  1120,     0,     0,   704,     0,     0,   705,   706,
     707,     0,     0,     0,   869,     0,     0,   735,     0,   228,
       0,   923,   736,   230,   231,   232,   233,   234,   805,     0,
     806,   618,   708,   709,   710,   623,     0,     0,  1020,   679,
     696,     0,  1149,     0,  1152,     0,   608,  1156,     0,     0,
       0,  1159,  1160,     0,   666,     0,  1163,  1164,     0,  1165,
       0,   228,   830,   229,  1167,   230,   231,   232,   233,   234,
       0,     0,   806,   844,     0,     0,   844,     0,     0,  1172,
    1173,     0,     0,     0,     0,  1190,     0,     0,  1190,     0,
       0,     0,  1190,     0,  1175,     0,   728,     0,  1176,  1177,
    1178,  1179,   728,   869,     5,     0,   728,     0,     0,     0,
       0,  1020,     0,   704,     0,     0,   705,   706,   707,     0,
       0,     0,  1217,  1200,  1218,     0,  1152,   608,  1221,     0,
    1156,  1223,     0,     0,     0,  1225,  1227,     0,  1230,     0,
     708,   709,   710,     0,  1232,  1234,     0,   228,     0,   869,
     869,   230,   231,   232,   233,   234,  1180,     0,   806,     0,
    1175,     0,     0,  1296,  1176,  1177,  1178,  1179,     0,     0,
       5,  1305,  1232,     0,     0,     0,  1181,     0,     0,   704,
    1411,     0,   705,   706,   707,     0,     0,  1415,   635,   636,
     637,   638,   639,   640,   641,   642,   643,   644,   645,   646,
     647,   648,     0,     0,     0,     0,   708,   709,   710,     0,
    1307,  1309,  1310,  1311,  1312,     0,     0,  1314,  1315,  1316,
    1317,  1318,  1180,     0,  1320,     0,  1227,   458,  1227,     3,
     844,     0,     0,     0,     0,   608,     0,     6,   474,   475,
       0,     0,  1181,     0,     0,     0,     8,   488,     0,     0,
       0,     0,     0,     9,   495,     0,    11,   498,     0,     0,
     501,     0,     0,     0,     0,     0,  1374,   863,   511,     0,
       0,    16,   228,  1379,   229,  1382,   230,   231,   232,   233,
     234,  1386,    18,  1389,   873,    20,   476,    22,  1393,     0,
       0,  1396,     0,     0,     5,  1408,  1400,     0,    25,     0,
      27,     0,    28,   704,    30,     0,   705,   706,   707,     0,
      35,     0,     0,     0,   898,   735,   902,     0,     0,   902,
     736,     0,   902,     0,     0,   902,     0,     0,   902,     0,
     708,   709,   710,     3,   728,     0,     0,     0,  1234,     0,
       0,     6,   936,  1424,  1426,   110,     0,  1429,     0,     0,
       8,     0,   111,   112,     0,   113,     0,     9,   116,     0,
     117,     0,   118,     0,     0,     0,   119,     0,   120,    14,
     121,     0,   122,     0,   123,    16,   124,     0,   125,     0,
     126,     0,     0,     0,     0,     0,    18,     0,   127,    20,
       0,    22,     0,     0,     0,   128,     0,   129,     0,   130,
       0,   131,    25,   132,    27,   133,    28,     0,    30,     0,
     134,   228,     0,   229,    35,   230,   231,   232,   233,   234,
       0,     0,     0,     0,     0,     1,     2,     0,     0,     0,
       0,     3,     0,     0,     0,     0,     4,     0,     5,     6,
       0,     0,     0,     0,     0,     7,     0,   197,     8,     0,
       0,     0,  1016,     0,     0,     9,    10,     0,    11,     0,
      12,     0,  1030,     0,     0,    13,     0,    14,     0,     0,
      15,     0,     0,    16,     0,     0,     0,   902,     0,   902,
       0,    17,     0,   902,    18,     0,    19,    20,    21,    22,
       0,   227,     0,   728,     0,     0,     0,    23,    24,     0,
      25,    26,    27,   728,    28,    29,    30,    31,    32,    33,
       0,    34,    35,    36,     0,     0,     0,     0,     0,     0,
       0,     3,     0,     0,     0,     0,     0,     0,     5,     6,
       0,     0,     0,     0,     0,     0,     0,   704,     8,     0,
     705,   706,   707,     0,     0,     9,     0,     0,   355,     0,
       0,     0,     0,   356,     0,   358,     0,    14,   360,   361,
       0,     0,     0,    16,   708,   709,   710,     0,     0,     0,
       0,     0,     0,     0,    18,     0,     0,    20,     0,    22,
     372,   373,     0,     0,     0,   376,     0,   902,   377,     0,
      25,     0,    27,     0,    28,   381,    30,     0,   936,   385,
       0,     0,    35,     0,   388,     0,     0,   476,     0,   391,
       0,     0,     0,     0,   394,     5,     0,     0,     0,   397,
       0,   400,     0,     0,   704,   402,     0,   705,   706,   707,
       0,     0,   404,     0,     0,   407,   735,     0,     0,     0,
       0,     0,     0,   416,     0,     0,     0,   728,     0,     0,
     728,   708,   709,   710,   728,     0,     0,     0,   728,  1278,
    1241,  1242,  1243,  1244,  1245,  1246,  1247,  1279,  1248,  1249,
    1250,  1251,  1252,  1253,  1254,  1255,  1256,  1257,  1258,  1259,
    1260,  1261,  1262,  1263,  1264,  1265,  1266,  1267,  1268,  1269,
    1270,  1271,  1280,  1281,  1272,  1273,  1274,  1275,  1276,     0,
       0,     0,     0,     0,     5,     0,     0,     0,     0,     0,
       0,     0,     0,   704,     0,   728,   705,   706,   707,     0,
       0,     0,     0,   728,     0,   728,     0,     0,     0,   529,
     736,   530,   728,     0,     0,     0,   535,     0,     5,     0,
     708,   709,   710,     0,     0,     0,   543,   704,     0,     0,
     705,   706,   707,   544,     0,     0,     0,  1200,     0,     0,
       0,   550,   635,   636,   637,   638,   639,   640,   641,   642,
     643,     0,     0,     0,   708,   709,   710,     0,     0,   567,
       0,     0,     0,     0,     0,   571,     0,     0,     0,   300,
       0,   305,     0,   310,     0,     0,   322,     0,     0,     0,
       0,     0,     0,     0,     0,  1330,     0,  1337,  1337,  1345,
    1345,     0,  1352,  1356,  1337,  1361,  1361,     0,  1366,     0,
     606,     0,     0,     0,   610,  1285,  1241,  1242,  1243,  1244,
    1245,  1246,  1247,  1286,  1248,  1249,  1250,  1251,  1252,  1253,
    1254,  1255,  1256,  1257,  1258,  1259,  1260,  1261,  1262,  1263,
    1264,  1265,  1266,  1267,  1268,  1269,  1270,  1271,  1287,  1288,
    1272,  1273,  1274,  1275,  1276,  1241,  1242,  1243,  1244,  1245,
    1246,  1247,     0,  1248,  1249,  1250,  1251,  1252,  1253,  1254,
    1255,  1256,  1257,  1258,  1259,  1260,  1261,  1262,  1263,  1264,
    1265,  1266,  1267,  1268,  1269,  1270,  1271,     5,     0,  1272,
    1273,  1274,  1275,  1276,     0,     0,   704,     0,     0,   705,
     706,   707,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     417,   761,     0,   708,   709,   710,     0,   432,     0,   438,
     439,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   472,     0,     0,     0,
       0,   480,     0,     0,     0,     0,     0,     0,   781,     0,
       0,     0,     0,   783,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   817,     0,   820,     0,
       0,   823,     0,   826,     0,   829,     0,   832,     0,     0,
     834,     0,     0,     0,     0,     0,     0,     0,   838,     0,
     545,   841,     0,   843,     0,     0,     0,     0,   846,     0,
     553,     0,     0,     0,     0,     0,     0,   561,     0,     0,
       0,     0,     0,     0,   395,   396,     0,   399,     0,     0,
       0,     0,   572,     0,     0,     0,     0,     0,   577,   578,
       0,     0,   581,     0,     0,   584,     0,     0,     0,     0,
       0,     0,   593,     0,     0,   595,     0,     0,     0,     0,
     601,     0,   602,     0,     0,     0,     0,     0,     0,     0,
       0,   611,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   625,     0,     0,     0,   632,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   669,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   681,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   732,     0,     0,     0,     0,
       0,   744,     0,     0,     0,   750,     0,     0,     0,     0,
       0,   753,     0,   754,     0,     0,     0,   756,     0,     0,
       0,     0,     0,   760,     0,     0,   559,     0,     0,     0,
     763,     0,     0,     0,   565,     0,     0,     0,   997,   568,
       0,     0,     0,     0,     0,     0,   575,   576,     0,     0,
       0,     0,     0,   773,     0,     0,   775,   776,   777,   778,
       0,     0,     0,     0,   594,     0,     0,     0,     0,     0,
     784,     0,     0,     0,     0,     0,     0,     0,     0,   792,
       0,   794,     0,     0,   797,     0,   799,     0,   801,     0,
       0,     0,     0,     0,   814,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1080,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   865,     0,   867,     0,
       0,     0,     0,     0,     0,  1105,     0,     0,     0,     0,
       0,     0,     0,   877,     0,   879,     0,     0,     0,     0,
       0,     0,   885,     0,   886,     0,   887,     0,   888,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1145,     0,     0,     0,     0,     0,
    1151,     0,  1154,   779,   780,  1158,     0,     0,     0,   782,
       0,     0,     0,   930,     0,     0,     0,     0,     0,     0,
       0,   937,     0,     0,   796,     0,   798,     0,   800,     0,
     802,     0,  1169,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1215,     0,  1216,     0,
       0,   978,     0,     0,     0,     0,   984,     0,     0,     0,
       0,     0,     0,     0,  1229,     0,     0,     0,   992,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1001,
       0,  1003,     0,     0,     0,     0,     0,     0,  1004,     0,
       0,  1009,  1010,  1011,  1012,  1013,  1014,     0,     0,  1015,
       0,  1019,     0,     0,  1023,     0,     0,     0,     0,  1025,
       0,  1034,  1035,  1036,     0,     0,     0,     0,     0,     0,
    1040,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1068,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1326,     0,   946,   947,   948,   949,     0,
     951,   952,     0,     0,     0,     0,     0,     0,     0,     0,
     959,     0,   961,   962,     0,     0,     0,  1085,     0,     0,
     966,   967,     0,     0,  1376,     0,     0,     0,     0,     0,
       0,  1381,  1097,   974,  1099,     0,     0,     0,     0,  1388,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1116,  1117,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1133,     0,     0,     0,     0,     0,     0,  1144,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1174,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1212,     0,     0,
       0,     0,     0,  1073,     0,     0,     0,     0,  1076,  1077,
       0,  1078,     0,     0,     0,     0,     0,  1079,     0,     0,
    1081,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1231,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1292,     0,     0,     0,     0,     0,     0,     0,     0,  1300,
       0,  1304,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1313,     0,
       0,     0,     0,     0,     0,  1319,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1328,     0,  1335,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1369,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1224,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1405,  1406,     0,     0,
    1236,  1237,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1422,     0,     0,     0,     0,     0,
       0,  1431,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1440,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1324,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1371,     0,     0,  1372,  1373,     0,
    1375,     0,     0,     0,  1377,  1378,     0,  1380,     0,  1383,
       0,     0,  1384,  1385,     0,  1387,     0,  1390,     0,  1391,
    1392,     0,  1394,  1395,     0,  1397,  1398,     0,  1399,     0,
    1401,  1402,     0,  1403,     0,  1404,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1421,     0,  1423,  1425,     0,  1427,
    1428,  1430,     0,  1432,  1433,  1434,  1435,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1439,     0,  1441
};

static const yytype_int16 yycheck[] =
{
      57,   192,   346,   558,     0,   227,   374,   115,   434,   201,
       0,   208,   196,   242,   220,   853,   783,   223,   131,   132,
     206,   598,   133,   838,   528,   369,   841,   604,   742,   212,
     434,   204,   352,   411,   836,   957,   893,     0,   592,   379,
     678,     0,  1145,   821,   836,     6,   697,   694,   656,   403,
     156,  1063,   158,   109,   110,   111,   112,   113,     4,   243,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,    25,   130,   131,   132,   832,   836,   136,
    1082,   138,  1307,   140,   141,   365,   143,     4,   999,  1000,
       0,    17,   149,   150,     6,   152,   153,   154,   155,   156,
     157,   158,   159,   160,    16,   162,   816,   164,   818,   220,
      17,   821,   223,  1069,   824,  1071,  1072,   827,     5,    49,
     830,    82,    40,   195,    50,    51,    78,   184,    15,   186,
     187,   188,    14,   190,    80,    17,   192,     7,    84,    56,
      22,   197,    12,    60,    51,     9,   195,    17,   821,  1374,
    1152,   824,   201,    17,    14,     6,     7,    17,    88,   350,
      78,    12,   954,    80,   235,  1152,    17,    84,   274,    17,
      82,   356,     9,  1160,    61,    62,     6,   720,    26,   722,
      17,   724,  1039,   726,   368,   728,   378,    17,   245,    17,
     247,   164,   377,   995,   416,    23,   954,   254,   976,   256,
    1156,    38,   821,   260,   261,   824,    54,    89,   827,   266,
     267,   268,   269,   927,   271,   929,   273,   274,   275,   539,
     403,  1059,  1234,  1326,  1062,   603,  1083,   284,   147,    89,
     570,    82,   289,   290,   872,   292,   991,  1159,   201,   198,
     878,   298,   893,   890,   598,   302,   303,  1005,   752,   569,
     307,   308,    82,   807,   862,   312,   313,    17,   866,   836,
     540,  1172,  1173,   320,  1121,    49,    26,   324,     5,   979,
     327,   981,   552,    40,     3,   985,   347,   334,    15,   336,
     337,   338,   339,   340,   513,   342,   470,     7,   164,    73,
    1105,    11,    12,    13,   350,    55,    25,    17,   409,   355,
     356,   412,  1140,   976,    88,  1143,   979,    65,  1086,   619,
    1232,     7,   622,  1080,    67,    11,    12,    13,    14,    72,
     376,   377,  1310,    67,    61,    65,   397,    61,  1316,   385,
       7,    71,    90,   404,    11,    12,    13,    14,   164,    92,
      17,   397,    35,  1057,   400,    19,     5,   418,    92,    69,
      90,   407,    17,   697,    28,   898,    15,   976,   164,   902,
     979,    26,   981,   906,   907,   908,   985,   357,   911,   912,
     427,   428,   915,    69,   364,   371,   164,   434,   435,  1089,
     437,   408,   574,  1305,   441,   442,   443,   414,   445,   586,
      17,    64,    69,    86,    87,    22,   774,   583,   455,  1113,
     457,   458,  1169,   460,   633,   462,   463,   580,   465,   466,
      41,   468,   469,  1086,  1087,   598,   473,   474,   475,     7,
     156,   478,   158,    11,    12,    13,   483,   484,  1005,    17,
     487,   488,  1234,    21,    22,   160,   161,   494,   495,   759,
     497,   498,   151,   500,   501,   157,   790,   851,  1215,  1216,
     162,   163,   164,   510,   511,   512,   882,   151,    35,  1297,
     517,    35,   519,    24,   521,   150,   523,  1086,  1087,  1088,
    1089,   791,   543,   529,   530,   547,   548,    57,    39,    56,
    1194,    69,    56,    60,     7,    48,    17,    48,  1202,    12,
    1163,   563,   564,    70,   550,    26,    24,   610,   547,   548,
      31,   162,   163,  1046,  1047,  1048,  1049,    17,     0,    86,
      87,   567,    86,    87,   563,   564,    77,    16,   589,    31,
       7,   592,    53,    54,    11,   574,    47,    17,     4,   600,
      17,    21,    22,   386,    74,    75,   389,    30,    31,   392,
      62,    53,    54,    19,  1163,  1164,  1165,     7,    25,   893,
     606,    11,    28,     7,   610,   612,    45,    11,    12,    13,
      53,    54,   619,    17,    32,   622,    20,   624,    22,   626,
      33,    17,  1149,    17,    20,  1152,    22,   155,   156,  1156,
      56,   156,    26,  1160,    60,     7,  1129,   644,  1302,    11,
      12,    13,   649,   650,   651,    17,   653,   654,   655,   656,
      22,   157,   659,   795,    80,   765,   766,   767,    84,    53,
      54,  1230,  1154,    58,    59,    69,  1158,   147,   675,   676,
     677,   678,   156,   680,   158,   682,   697,   684,    29,   686,
      32,    33,   147,   793,   149,   826,     8,  1005,   829,   148,
    1217,   847,   158,   152,   153,   154,   703,    69,   362,   363,
    1227,    54,   366,   367,   369,   999,  1000,  1234,   715,   346,
     717,   769,   147,   720,   149,   722,   380,   724,  1221,   726,
    1223,   728,   695,   387,   731,   894,   390,   734,   786,   393,
      61,    62,   739,   740,   120,   742,   122,   401,   124,   746,
     126,   762,   891,   546,   751,  1039,   549,  1225,   551,  1227,
     816,   554,   555,   817,  1293,   761,  1295,   147,   148,   562,
     202,  1307,   152,   153,   154,  1311,  1312,   209,  1314,  1315,
     573,  1317,  1318,   818,  1320,   781,   579,   783,   976,   582,
      53,    54,   585,   804,  1317,  1318,   807,   808,   527,  1083,
     152,   153,   154,   596,  1095,   147,   148,  1239,   150,  1193,
     152,   153,   154,   155,   156,  1197,    17,  1017,  1018,   925,
     926,   832,   739,   740,   820,    26,   571,   823,  1071,    30,
      31,  1156,   956,   957,  1312,   997,   832,  1121,   768,  1066,
    1067,   771,   838,  1379,  1318,   841,  1315,     0,   201,   624,
    1386,   536,    53,    54,   851,   785,   853,   988,   788,   990,
    1396,   843,     7,   129,  1400,   862,    11,    12,    13,   866,
    1039,   868,    17,   870,   871,   872,   129,   129,   875,   815,
     129,   878,   893,   894,   881,   882,   129,   960,  1172,  1173,
     148,   199,   150,   199,   152,   153,   154,  1309,  1310,  1311,
    1312,   898,  1314,   900,  1316,   902,   478,   129,   129,   906,
     907,   908,    17,   570,   911,   912,   129,   914,   915,   129,
     917,    26,   129,   920,    29,    30,    31,  1102,   925,   926,
     927,  1426,   929,   398,   931,   932,   129,   934,    43,   195,
      45,   195,   374,   129,   597,   129,   943,   944,    53,    54,
      55,   112,   384,   597,   597,   597,   117,   129,   119,   970,
     121,   129,   123,  1094,   125,   129,   129,  1379,   731,     0,
    1382,   334,   337,     0,  1386,   339,  1405,  1389,     0,     8,
     991,  1393,   944,    -1,    -1,    -1,     7,     8,    17,    -1,
      11,    12,    13,    14,    -1,   991,    17,    26,    -1,    -1,
      29,    30,    31,    -1,    -1,    26,  1002,  1004,    29,    30,
      31,    -1,  1424,  1010,  1011,  1012,  1013,  1429,    -1,    -1,
    1017,  1018,    -1,    -1,    53,    54,    55,    -1,  1039,    -1,
      -1,    -1,    53,    54,    55,  1159,  1167,    -1,  1035,    -1,
      -1,    -1,    -1,    -1,    -1,  1042,  1043,  1044,    69,  1046,
    1047,  1048,  1049,    -1,  1051,    -1,  1053,    -1,     9,    -1,
    1057,    -1,  1059,  1060,    -1,  1062,    17,    -1,    89,  1066,
    1067,    -1,  1083,    -1,    -1,    26,    -1,    -1,    29,    30,
      31,    -1,    -1,    -1,  1080,    -1,    -1,    38,    -1,   148,
      -1,    42,    43,   152,   153,   154,   155,   156,   157,    -1,
     159,   533,    53,    54,    55,   537,    -1,    -1,  1232,  1105,
    1121,    -1,  1109,    -1,  1111,    -1,  1113,  1114,    -1,    -1,
      -1,  1118,  1119,    -1,   556,    -1,  1123,  1124,    -1,  1126,
      -1,   148,  1129,   150,  1131,   152,   153,   154,   155,   156,
      -1,    -1,   159,  1140,    -1,    -1,  1143,    -1,    -1,  1146,
    1147,    -1,    -1,    -1,    -1,  1151,    -1,    -1,  1154,    -1,
      -1,    -1,  1158,    -1,     7,    -1,   598,    -1,    11,    12,
      13,    14,   604,  1169,    17,    -1,   608,    -1,    -1,    -1,
      -1,  1305,    -1,    26,    -1,    -1,    29,    30,    31,    -1,
      -1,    -1,  1189,    36,  1191,    -1,  1193,  1194,  1195,    -1,
    1197,  1198,    -1,    -1,    -1,  1202,  1203,    -1,  1205,    -1,
      53,    54,    55,    -1,  1211,  1212,    -1,   148,    -1,  1215,
    1216,   152,   153,   154,   155,   156,    69,    -1,   159,    -1,
       7,    -1,    -1,  1229,    11,    12,    13,    14,    -1,    -1,
      17,  1238,  1239,    -1,    -1,    -1,    89,    -1,    -1,    26,
    1381,    -1,    29,    30,    31,    -1,    -1,  1388,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,    -1,    -1,    53,    54,    55,    -1,
    1277,  1278,  1279,  1280,  1281,    -1,    -1,  1284,  1285,  1286,
    1287,  1288,    69,    -1,  1291,    -1,  1293,   361,  1295,    10,
    1297,    -1,    -1,    -1,    -1,  1302,    -1,    18,   372,   373,
      -1,    -1,    89,    -1,    -1,    -1,    27,   381,    -1,    -1,
      -1,    -1,    -1,    34,   388,    -1,    37,   391,    -1,    -1,
     394,    -1,    -1,    -1,    -1,    -1,  1333,   769,   402,    -1,
      -1,    52,   148,  1340,   150,  1342,   152,   153,   154,   155,
     156,  1348,    63,  1350,   786,    66,     9,    68,  1355,    -1,
      -1,  1358,    -1,    -1,    17,  1376,  1363,    -1,    79,    -1,
      81,    -1,    83,    26,    85,    -1,    29,    30,    31,    -1,
      91,    -1,    -1,    -1,   816,    38,   818,    -1,    -1,   821,
      43,    -1,   824,    -1,    -1,   827,    -1,    -1,   830,    -1,
      53,    54,    55,    10,   836,    -1,    -1,    -1,  1405,    -1,
      -1,    18,   844,  1410,  1411,    45,    -1,  1414,    -1,    -1,
      27,    -1,    52,    53,    -1,    55,    -1,    34,    58,    -1,
      60,    -1,    62,    -1,    -1,    -1,    66,    -1,    68,    46,
      70,    -1,    72,    -1,    74,    52,    76,    -1,    78,    -1,
      80,    -1,    -1,    -1,    -1,    -1,    63,    -1,    88,    66,
      -1,    68,    -1,    -1,    -1,    95,    -1,    97,    -1,    99,
      -1,   101,    79,   103,    81,   105,    83,    -1,    85,    -1,
     147,   148,    -1,   150,    91,   152,   153,   154,   155,   156,
      -1,    -1,    -1,    -1,    -1,     4,     5,    -1,    -1,    -1,
      -1,    10,    -1,    -1,    -1,    -1,    15,    -1,    17,    18,
      -1,    -1,    -1,    -1,    -1,    24,    -1,   147,    27,    -1,
      -1,    -1,   954,    -1,    -1,    34,    35,    -1,    37,    -1,
      39,    -1,   964,    -1,    -1,    44,    -1,    46,    -1,    -1,
      49,    -1,    -1,    52,    -1,    -1,    -1,   979,    -1,   981,
      -1,    60,    -1,   985,    63,    -1,    65,    66,    67,    68,
      -1,   191,    -1,   995,    -1,    -1,    -1,    76,    77,    -1,
      79,    80,    81,  1005,    83,    84,    85,    86,    87,    88,
      -1,    90,    91,    92,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    10,    -1,    -1,    -1,    -1,    -1,    -1,    17,    18,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,    27,    -1,
      29,    30,    31,    -1,    -1,    34,    -1,    -1,   248,    -1,
      -1,    -1,    -1,   253,    -1,   255,    -1,    46,   258,   259,
      -1,    -1,    -1,    52,    53,    54,    55,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    63,    -1,    -1,    66,    -1,    68,
     280,   281,    -1,    -1,    -1,   285,    -1,  1089,   288,    -1,
      79,    -1,    81,    -1,    83,   295,    85,    -1,  1100,   299,
      -1,    -1,    91,    -1,   304,    -1,    -1,     9,    -1,   309,
      -1,    -1,    -1,    -1,   314,    17,    -1,    -1,    -1,   319,
      -1,   321,    -1,    -1,    26,   325,    -1,    29,    30,    31,
      -1,    -1,   332,    -1,    -1,   335,    38,    -1,    -1,    -1,
      -1,    -1,    -1,   343,    -1,    -1,    -1,  1149,    -1,    -1,
    1152,    53,    54,    55,  1156,    -1,    -1,    -1,  1160,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,    -1,
      -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    26,    -1,  1217,    29,    30,    31,    -1,
      -1,    -1,    -1,  1225,    -1,  1227,    -1,    -1,    -1,   429,
      43,   431,  1234,    -1,    -1,    -1,   436,    -1,    17,    -1,
      53,    54,    55,    -1,    -1,    -1,   446,    26,    -1,    -1,
      29,    30,    31,   453,    -1,    -1,    -1,    36,    -1,    -1,
      -1,   461,    93,    94,    95,    96,    97,    98,    99,   100,
     101,    -1,    -1,    -1,    53,    54,    55,    -1,    -1,   479,
      -1,    -1,    -1,    -1,    -1,   485,    -1,    -1,    -1,   203,
      -1,   205,    -1,   207,    -1,    -1,   210,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1307,    -1,  1309,  1310,  1311,
    1312,    -1,  1314,  1315,  1316,  1317,  1318,    -1,  1320,    -1,
     520,    -1,    -1,    -1,   524,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   108,   109,   110,   111,   112,
     113,   114,    -1,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,    17,    -1,   142,
     143,   144,   145,   146,    -1,    -1,    26,    -1,    -1,    29,
      30,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     344,   631,    -1,    53,    54,    55,    -1,   351,    -1,   353,
     354,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   370,    -1,    -1,    -1,
      -1,   375,    -1,    -1,    -1,    -1,    -1,    -1,   668,    -1,
      -1,    -1,    -1,   673,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   716,    -1,   718,    -1,
      -1,   721,    -1,   723,    -1,   725,    -1,   727,    -1,    -1,
     730,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   738,    -1,
     454,   741,    -1,   743,    -1,    -1,    -1,    -1,   748,    -1,
     464,    -1,    -1,    -1,    -1,    -1,    -1,   471,    -1,    -1,
      -1,    -1,    -1,    -1,   317,   318,    -1,   320,    -1,    -1,
      -1,    -1,   486,    -1,    -1,    -1,    -1,    -1,   492,   493,
      -1,    -1,   496,    -1,    -1,   499,    -1,    -1,    -1,    -1,
      -1,    -1,   506,    -1,    -1,   509,    -1,    -1,    -1,    -1,
     514,    -1,   516,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   525,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   538,    -1,    -1,    -1,   542,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   557,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   568,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   599,    -1,    -1,    -1,    -1,
      -1,   605,    -1,    -1,    -1,   609,    -1,    -1,    -1,    -1,
      -1,   615,    -1,   617,    -1,    -1,    -1,   621,    -1,    -1,
      -1,    -1,    -1,   627,    -1,    -1,   469,    -1,    -1,    -1,
     634,    -1,    -1,    -1,   477,    -1,    -1,    -1,   928,   482,
      -1,    -1,    -1,    -1,    -1,    -1,   489,   490,    -1,    -1,
      -1,    -1,    -1,   657,    -1,    -1,   660,   661,   662,   663,
      -1,    -1,    -1,    -1,   507,    -1,    -1,    -1,    -1,    -1,
     674,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   683,
      -1,   685,    -1,    -1,   688,    -1,   690,    -1,   692,    -1,
      -1,    -1,    -1,    -1,   698,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1032,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   770,    -1,   772,    -1,
      -1,    -1,    -1,    -1,    -1,  1065,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   787,    -1,   789,    -1,    -1,    -1,    -1,
      -1,    -1,   796,    -1,   798,    -1,   800,    -1,   802,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1104,    -1,    -1,    -1,    -1,    -1,
    1110,    -1,  1112,   666,   667,  1115,    -1,    -1,    -1,   672,
      -1,    -1,    -1,   837,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   845,    -1,    -1,   687,    -1,   689,    -1,   691,    -1,
     693,    -1,  1142,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1186,    -1,  1188,    -1,
      -1,   905,    -1,    -1,    -1,    -1,   910,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1204,    -1,    -1,    -1,   922,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   933,
      -1,   935,    -1,    -1,    -1,    -1,    -1,    -1,   942,    -1,
      -1,   945,   946,   947,   948,   949,   950,    -1,    -1,   953,
      -1,   955,    -1,    -1,   958,    -1,    -1,    -1,    -1,   963,
      -1,   965,   966,   967,    -1,    -1,    -1,    -1,    -1,    -1,
     974,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1006,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1303,    -1,   858,   859,   860,   861,    -1,
     863,   864,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     873,    -1,   875,   876,    -1,    -1,    -1,  1041,    -1,    -1,
     883,   884,    -1,    -1,  1334,    -1,    -1,    -1,    -1,    -1,
      -1,  1341,  1056,   896,  1058,    -1,    -1,    -1,    -1,  1349,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1076,  1077,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1096,    -1,    -1,    -1,    -1,    -1,    -1,  1103,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1148,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1171,    -1,    -1,
      -1,    -1,    -1,  1016,    -1,    -1,    -1,    -1,  1021,  1022,
      -1,  1024,    -1,    -1,    -1,    -1,    -1,  1030,    -1,    -1,
    1033,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1210,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1224,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1233,
      -1,  1235,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1282,    -1,
      -1,    -1,    -1,    -1,    -1,  1289,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1306,    -1,  1308,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1323,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1199,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1370,  1371,    -1,    -1,
    1213,  1214,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1408,    -1,    -1,    -1,    -1,    -1,
      -1,  1415,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1437,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1299,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1327,    -1,    -1,  1330,  1331,    -1,
    1333,    -1,    -1,    -1,  1337,  1338,    -1,  1340,    -1,  1342,
      -1,    -1,  1345,  1346,    -1,  1348,    -1,  1350,    -1,  1352,
    1353,    -1,  1355,  1356,    -1,  1358,  1359,    -1,  1361,    -1,
    1363,  1364,    -1,  1366,    -1,  1368,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1407,    -1,  1409,  1410,    -1,  1412,
    1413,  1414,    -1,  1416,  1417,  1418,  1419,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1436,    -1,  1438
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
     364,   375,   377,   388,   389,   390,   398,   399,   400,   401,
     402,   403,   404,   405,   406,   407,   408,   409,   410,   411,
     413,   435,   436,   437,   439,   441,   442,   448,   449,   454,
     459,   462,   465,   468,   469,   472,   477,   479,   482,   486,
     499,   503,   504,   509,   511,   512,     0,   160,   161,   203,
     203,   203,   203,   203,   164,   205,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   147,   206,   242,   206,   314,   206,
     341,   341,   206,   348,     6,    82,   351,   352,   206,   358,
     341,   206,   378,   341,   358,   341,   358,   341,   358,   341,
     358,   206,   450,   206,   473,   336,   337,   338,   339,   363,
     389,   390,   401,   405,   409,   435,   448,   454,   459,   462,
     465,   468,   478,   480,   481,   206,   487,   487,   487,    40,
     491,   492,   205,   205,   205,   205,   205,   203,   205,   205,
     205,   205,   205,   359,   205,   359,   205,   359,   205,   205,
     412,   205,   205,   162,   163,   204,   205,   205,   483,   484,
     205,   500,   501,   205,   505,   506,   205,   203,   148,   150,
     152,   153,   154,   155,   156,   169,   170,   171,   173,   175,
     176,   179,   181,   182,   206,   243,    61,   315,   317,    19,
      28,    56,   308,   313,   324,   325,   344,   356,   371,   372,
     438,   440,   443,   445,   446,   447,   324,   344,   438,   440,
     173,   349,   206,   353,   345,   365,   366,   368,   369,   370,
     371,   372,     3,    25,   379,   380,    70,   311,   313,   324,
     391,   399,   455,   456,   457,   458,    23,   239,   360,   361,
     204,    71,   403,   460,   461,   204,    72,   407,   463,   464,
     204,    73,   411,   466,   467,    50,    51,   239,   414,   415,
     417,   418,   204,    64,   451,   452,   469,   470,   511,    74,
      75,   474,   475,    78,   488,   489,   491,   488,   491,   488,
     491,    41,   493,   494,   168,   181,   151,   169,   180,   173,
     205,   240,   205,   309,   312,   203,   203,   205,   203,   205,
     203,   203,   205,   205,   205,   205,   205,   205,   205,   205,
     359,   205,   203,   203,   205,   376,   203,   203,   205,   205,
     205,   203,   205,   260,   205,   203,   205,   205,   203,   205,
     205,   203,   205,   205,   203,   260,   260,   203,   205,   260,
     203,   205,   203,   205,   203,   205,   484,   203,   205,   205,
     501,   205,   205,   506,   205,   205,   203,   204,   174,   177,
     178,   182,   181,    32,    33,   170,   206,   244,   245,   246,
     248,   249,   204,    62,   315,   320,   321,   345,   204,   204,
     206,   318,   314,   356,    57,   326,   327,    16,   269,   270,
     272,   274,   275,   277,   340,   351,   206,   373,   373,    48,
     420,   422,   420,   356,   340,   420,   420,   173,   350,   172,
     174,   354,   204,   375,   373,   373,     9,   239,   382,   384,
     204,   206,   381,   314,   399,   277,   392,   420,   373,   239,
     239,   206,   362,   269,   420,   373,   269,   420,   373,   269,
     420,   373,   181,   185,   187,   206,   416,   414,   206,   419,
     420,   373,   470,   181,   476,   206,   490,   493,    25,   424,
     425,   493,    47,   513,   517,   168,   181,   205,   205,   203,
     203,   309,   312,   205,   310,   203,   205,   205,   316,   205,
     205,   205,   329,   203,   203,   204,   205,   205,   205,   205,
     203,   205,   205,   204,   205,   205,   205,   346,   205,   260,
     173,   204,   205,   205,   205,   260,   376,   203,   260,   205,
     205,   203,   204,   205,   205,   260,   260,   204,   204,   205,
     205,   204,   205,   205,   204,   205,   205,   156,   158,   184,
     186,   190,   193,   204,   260,   204,   205,   205,   205,   471,
     169,   204,   204,   205,   205,   502,   203,   205,   205,   208,
     203,   204,   245,   248,   206,   247,   206,   250,   239,   323,
     170,   322,   320,   239,   319,   204,   315,   340,    58,    59,
     330,   332,   204,   181,   328,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   278,
     279,   280,   269,   324,   344,   324,   344,   269,   206,   423,
     269,   340,   269,   269,     7,    11,   239,   261,   265,   204,
       7,    12,   259,   264,   269,   324,   344,   324,   344,   206,
     385,   204,   315,   392,   278,   269,   324,   399,   269,   403,
     269,   407,   269,   411,   181,   188,   181,   194,   269,   443,
     445,   446,   447,   453,    26,    29,    30,    31,    53,    54,
      55,   209,   211,   212,   213,   214,   215,   216,   217,   220,
     221,   222,   224,   225,   230,   232,   235,   236,   239,   251,
     252,   470,   204,   181,   424,    38,    43,   209,   384,   427,
     432,   433,   497,   498,   204,   206,   426,    45,   510,   209,
     204,   487,   205,   204,   204,   310,   204,   310,   316,   205,
     204,   203,   169,   204,   205,   205,   205,   205,   205,   205,
     444,   205,   444,   204,   205,   204,   204,   204,   204,   260,
     260,   203,   260,   203,   204,   205,   205,   367,   205,   367,
     205,   205,   204,   205,   204,   205,   260,   204,   260,   204,
     260,   204,   260,   186,   184,   157,   159,   182,   183,   191,
     194,   199,   200,   201,   204,   205,   205,   203,   205,   210,
     203,   205,   210,   203,   205,   210,   203,   205,   210,   203,
     205,   210,   203,   210,   203,   471,   205,   485,   203,   502,
     502,   203,   208,   203,   205,   331,   203,   205,   514,   515,
     248,   315,   206,   333,   181,   152,   153,   154,   281,   281,
     281,   281,   356,   239,   351,   204,   356,   204,   424,   206,
     267,   267,   356,   239,   259,   351,   374,   204,   356,   204,
     172,   386,   315,   281,   399,   204,   204,   204,   204,   181,
     189,   194,   181,   195,   183,   202,   375,   213,   239,   251,
     216,   220,   239,   251,   206,   218,   224,   230,   235,   206,
     223,   230,   235,   170,   226,   235,   170,   233,   185,   206,
     237,   147,   207,    42,   209,   427,   432,   495,   496,   497,
     204,   149,   385,   428,   385,   322,   239,   204,   375,   420,
     482,   503,   507,   424,   488,   331,   260,   260,   260,   260,
     444,   260,   260,   444,   205,   421,   205,   205,   367,   260,
     205,   260,   260,   367,   205,   383,   260,   260,   186,   202,
     183,   192,   199,   195,   260,   210,   205,   210,   204,   205,
     210,   205,   210,   210,   204,   205,   210,   210,   205,   210,
     205,   205,   204,   485,   485,   205,   208,   203,   208,   205,
     205,   204,   205,   204,   204,   205,   516,   205,   515,   204,
     204,   204,   204,   204,   204,   204,   239,   427,   432,   204,
     173,   268,   268,   204,   374,   204,     7,    11,    12,    13,
     239,   257,   258,   387,   204,   204,   204,   181,   196,   197,
     204,   219,   221,   224,   230,   235,   230,   235,   235,   235,
     170,   227,   170,   234,   185,   206,   238,   497,   168,   386,
     386,   206,   434,   205,   209,   384,   432,   518,   204,   205,
     205,   205,   205,   260,   421,   421,   260,   260,   260,   260,
     203,   260,   205,   169,   199,   204,   205,   205,   205,   205,
     210,   210,   210,   210,   205,   205,   231,   204,   208,   204,
     205,   331,   205,   331,   510,   203,   516,   516,     8,   283,
     285,   282,   285,   283,   284,   285,   204,   204,   267,   282,
     181,   198,   199,   224,   230,   235,   230,   235,   235,   235,
     170,   228,   261,   204,     7,    11,    12,    13,    14,    69,
     429,   430,   431,   429,   204,   203,   149,   385,   519,   205,
     273,   203,   205,   271,   203,   208,   205,   276,   203,   205,
     205,   393,   199,   205,   205,   205,   210,   205,   331,   203,
     331,   507,   205,   205,   204,     7,    11,    12,    13,    14,
      69,    89,   209,   253,   254,   255,   256,   262,   266,   306,
     206,   286,   209,   282,   306,   286,   209,   284,   286,   268,
      36,   209,   306,   395,   396,   230,   235,   235,   235,   170,
     229,   267,   204,   386,   386,   203,   203,   205,   205,   271,
     208,   205,   276,   205,   260,   205,   208,   205,   394,   203,
     205,   204,   205,   263,   205,   508,   260,   260,   267,   267,
     209,   108,   109,   110,   111,   112,   113,   114,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   142,   143,   144,   145,   146,   299,   107,   115,
     140,   141,   287,   290,   299,   107,   115,   140,   141,   292,
     295,   299,   204,   395,   209,   395,   206,   397,   235,   268,
     204,   209,   497,   510,   204,   205,   263,   205,   297,   205,
     205,   205,   205,   204,   205,   205,   205,   205,   205,   204,
     205,   394,   394,   331,   260,   208,   203,   268,   204,    22,
     239,   262,   298,   304,   305,   204,    20,   239,   254,   288,
     300,   301,   304,   288,    21,   239,   254,   289,   302,   303,
     304,   289,   239,   254,   291,   304,   239,   293,   300,   304,
     288,   239,   294,   302,   304,   294,   239,   296,   304,   204,
     507,   260,   260,   260,   205,   260,   203,   260,   260,   205,
     260,   203,   205,   260,   260,   260,   205,   260,   203,   205,
     260,   260,   260,   205,   260,   260,   205,   260,   260,   260,
     205,   260,   260,   260,   260,   204,   204,   262,   181,   254,
     304,   170,   254,   254,   304,   170,   254,   254,   304,   304,
     508,   260,   204,   260,   205,   260,   205,   260,   260,   205,
     260,   204,   260,   260,   260,   260,   254,   259,   254,   260,
     204,   260
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
     394,   394,   394,   395,   396,   397,   398,   398,   399,   400,
     400,   400,   401,   402,   402,   403,   404,   404,   405,   406,
     407,   408,   408,   409,   410,   411,   412,   412,   412,   412,
     412,   413,   413,   414,   415,   416,   416,   417,   418,   419,
     420,   421,   421,   421,   421,   422,   423,   424,   425,   426,
     427,   428,   428,   428,   429,   430,   431,   431,   431,   431,
     431,   431,   432,   433,   434,   435,   435,   435,   436,   436,
     437,   438,   438,   439,   440,   440,   441,   442,   443,   444,
     444,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   453,   453,   453,   454,   455,   455,   456,   457,   458,
     459,   460,   461,   462,   463,   464,   465,   466,   467,   468,
     469,   469,   469,   469,   469,   469,   469,   469,   469,   469,
     469,   469,   470,   470,   471,   471,   471,   472,   473,   474,
     475,   475,   476,   476,   476,   477,   478,   478,   479,   480,
     480,   480,   480,   480,   480,   480,   480,   480,   480,   480,
     480,   480,   480,   481,   481,   481,   481,   481,   481,   481,
     482,   483,   483,   484,   485,   485,   485,   485,   485,   485,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   500,   501,   502,   502,
     502,   502,   502,   503,   504,   505,   505,   506,   507,   507,
     507,   507,   508,   508,   508,   508,   509,   510,   511,   512,
     513,   514,   514,   515,   516,   516,   516,   516,   517,   518,
     519,   519
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
       0,     3,     2,     5,     1,     1,     1,     1,     5,     1,
       1,     1,     8,     1,     1,     5,     1,     1,     8,     1,
       5,     1,     1,     8,     1,     5,     0,     3,     5,     3,
       3,     1,     1,     4,     1,     1,     1,     4,     1,     1,
       7,     0,     3,     3,     3,     1,     1,     5,     1,     1,
       4,     6,     4,     6,     1,     5,     1,     1,     1,     1,
       1,     1,     7,     1,     1,     1,     1,     1,     1,     1,
      10,     1,     1,    10,     1,     1,    10,    10,     7,     0,
       3,     3,     9,     7,     9,    10,     1,     1,     9,     1,
       1,     1,     1,     1,    10,     1,     1,     7,     9,     1,
      10,     7,     1,    10,     7,     1,    10,     7,     1,     9,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     3,     2,     1,     1,     4,
       1,     1,     1,     2,     3,     4,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     3,     1,     8,     0,     3,     3,     3,     5,     3,
       2,     1,     1,     4,     1,     1,     4,     1,     4,     1,
       4,     1,     4,     1,     4,     3,     1,     6,     0,     3,
       3,     3,     2,     1,     4,     3,     1,    16,     1,     1,
       1,     1,     0,     6,     3,     2,     1,     1,     9,     1,
       4,     3,     1,     4,     0,     3,     3,     2,     1,     4,
       4,     4
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
