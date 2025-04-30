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
    T_DEFININGTRANSFORMATION = 340,
    T_GEODETICCRS = 341,
    T_GEODETICDATUM = 342,
    T_PROJECTEDCRS = 343,
    T_PRIMEMERIDIAN = 344,
    T_GEOGRAPHICCRS = 345,
    T_TRF = 346,
    T_VERTICALCRS = 347,
    T_VERTICALDATUM = 348,
    T_VRF = 349,
    T_TIMEDATUM = 350,
    T_TEMPORALQUANTITY = 351,
    T_ENGINEERINGDATUM = 352,
    T_ENGINEERINGCRS = 353,
    T_PARAMETRICDATUM = 354,
    T_AFFINE = 355,
    T_CARTESIAN = 356,
    T_CYLINDRICAL = 357,
    T_ELLIPSOIDAL = 358,
    T_LINEAR = 359,
    T_PARAMETRIC = 360,
    T_POLAR = 361,
    T_SPHERICAL = 362,
    T_VERTICAL = 363,
    T_TEMPORAL = 364,
    T_TEMPORALCOUNT = 365,
    T_TEMPORALMEASURE = 366,
    T_ORDINAL = 367,
    T_TEMPORALDATETIME = 368,
    T_NORTH = 369,
    T_NORTHNORTHEAST = 370,
    T_NORTHEAST = 371,
    T_EASTNORTHEAST = 372,
    T_EAST = 373,
    T_EASTSOUTHEAST = 374,
    T_SOUTHEAST = 375,
    T_SOUTHSOUTHEAST = 376,
    T_SOUTH = 377,
    T_SOUTHSOUTHWEST = 378,
    T_SOUTHWEST = 379,
    T_WESTSOUTHWEST = 380,
    T_WEST = 381,
    T_WESTNORTHWEST = 382,
    T_NORTHWEST = 383,
    T_NORTHNORTHWEST = 384,
    T_UP = 385,
    T_DOWN = 386,
    T_GEOCENTRICX = 387,
    T_GEOCENTRICY = 388,
    T_GEOCENTRICZ = 389,
    T_COLUMNPOSITIVE = 390,
    T_COLUMNNEGATIVE = 391,
    T_ROWPOSITIVE = 392,
    T_ROWNEGATIVE = 393,
    T_DISPLAYRIGHT = 394,
    T_DISPLAYLEFT = 395,
    T_DISPLAYUP = 396,
    T_DISPLAYDOWN = 397,
    T_FORWARD = 398,
    T_AFT = 399,
    T_PORT = 400,
    T_STARBOARD = 401,
    T_CLOCKWISE = 402,
    T_COUNTERCLOCKWISE = 403,
    T_TOWARDS = 404,
    T_AWAYFROM = 405,
    T_FUTURE = 406,
    T_PAST = 407,
    T_UNSPECIFIED = 408,
    T_STRING = 409,
    T_UNSIGNED_INTEGER_DIFFERENT_ONE_TWO_THREE = 410
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
#define YYLAST   4007

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  171
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  375
/* YYNRULES -- Number of rules.  */
#define YYNRULES  750
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1533

#define YYUNDEFTOK  2
#define YYMAXUTOK   410


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
     167,   169,     2,   161,   170,   162,   156,     2,     2,   158,
     159,   160,     2,     2,     2,     2,     2,     2,   163,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   157,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   164,     2,     2,     2,     2,     2,
     165,   166,     2,   168,     2,     2,     2,     2,     2,     2,
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
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   214,   214,   214,   214,   214,   214,   214,   215,   215,
     215,   216,   219,   219,   220,   220,   220,   221,   223,   223,
     227,   231,   231,   233,   235,   237,   237,   239,   239,   241,
     243,   245,   247,   249,   249,   251,   251,   253,   253,   253,
     253,   255,   255,   259,   261,   265,   266,   267,   269,   269,
     271,   273,   275,   277,   281,   282,   285,   286,   288,   290,
     292,   295,   296,   297,   299,   301,   303,   303,   305,   308,
     309,   311,   311,   316,   316,   318,   318,   320,   322,   324,
     328,   329,   332,   333,   334,   336,   336,   337,   340,   341,
     345,   346,   347,   351,   352,   353,   354,   356,   360,   362,
     365,   367,   370,   371,   372,   373,   374,   375,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   387,   388,   389,
     390,   391,   392,   393,   394,   395,   396,   397,   398,   399,
     400,   401,   405,   407,   409,   413,   418,   420,   422,   424,
     426,   430,   435,   436,   438,   440,   442,   446,   450,   452,
     452,   454,   454,   459,   464,   465,   466,   467,   468,   469,
     470,   472,   474,   476,   476,   478,   478,   480,   482,   484,
     486,   488,   490,   494,   496,   500,   500,   503,   506,   511,
     511,   511,   511,   511,   514,   519,   519,   519,   519,   522,
     526,   527,   529,   545,   549,   550,   552,   552,   554,   554,
     560,   560,   562,   564,   571,   571,   571,   571,   573,   575,
     582,   589,   590,   591,   592,   594,   595,   598,   600,   603,
     604,   605,   606,   607,   609,   610,   611,   612,   614,   621,
     622,   623,   624,   626,   633,   640,   641,   642,   644,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   648,   648,
     650,   650,   652,   652,   652,   654,   659,   665,   670,   673,
     676,   677,   678,   679,   680,   681,   682,   683,   684,   687,
     688,   689,   690,   691,   692,   693,   694,   697,   698,   699,
     700,   701,   702,   703,   704,   707,   708,   711,   712,   713,
     714,   715,   719,   720,   721,   722,   723,   724,   725,   726,
     727,   730,   731,   732,   733,   736,   737,   738,   739,   742,
     743,   746,   747,   748,   753,   754,   757,   758,   759,   760,
     761,   764,   765,   766,   767,   768,   769,   770,   771,   772,
     773,   774,   775,   776,   777,   778,   779,   780,   781,   782,
     783,   784,   785,   786,   787,   788,   789,   790,   791,   792,
     793,   794,   795,   796,   797,   798,   799,   801,   804,   806,
     808,   810,   812,   815,   816,   817,   818,   820,   821,   822,
     823,   824,   826,   827,   829,   830,   832,   833,   834,   834,
     836,   852,   852,   854,   862,   863,   865,   866,   868,   876,
     877,   879,   881,   883,   888,   889,   891,   893,   895,   897,
     899,   901,   903,   908,   912,   914,   917,   920,   921,   922,
     924,   925,   927,   932,   933,   935,   935,   937,   941,   941,
     941,   943,   943,   945,   953,   962,   970,   980,   981,   983,
     985,   985,   987,   987,   990,   991,   995,  1001,  1002,  1003,
    1005,  1005,  1007,  1009,  1011,  1015,  1020,  1020,  1022,  1025,
    1026,  1031,  1032,  1034,  1039,  1039,  1039,  1041,  1043,  1044,
    1045,  1046,  1047,  1048,  1049,  1050,  1052,  1055,  1057,  1059,
    1062,  1064,  1064,  1064,  1068,  1074,  1074,  1078,  1078,  1079,
    1079,  1081,  1086,  1087,  1088,  1089,  1090,  1092,  1098,  1103,
    1109,  1111,  1113,  1115,  1119,  1125,  1126,  1127,  1129,  1131,
    1133,  1137,  1137,  1139,  1141,  1146,  1147,  1148,  1150,  1152,
    1154,  1156,  1160,  1160,  1162,  1168,  1175,  1175,  1178,  1185,
    1186,  1187,  1188,  1189,  1191,  1192,  1193,  1195,  1199,  1201,
    1203,  1203,  1207,  1212,  1212,  1212,  1216,  1221,  1221,  1223,
    1227,  1227,  1229,  1230,  1231,  1232,  1236,  1241,  1243,  1247,
    1247,  1251,  1256,  1258,  1262,  1263,  1264,  1265,  1266,  1268,
    1268,  1270,  1273,  1275,  1275,  1277,  1279,  1281,  1285,  1291,
    1292,  1293,  1294,  1296,  1298,  1302,  1307,  1309,  1312,  1318,
    1319,  1320,  1322,  1326,  1332,  1332,  1332,  1332,  1332,  1332,
    1336,  1341,  1343,  1348,  1348,  1349,  1351,  1351,  1353,  1360,
    1360,  1362,  1369,  1369,  1371,  1378,  1385,  1390,  1391,  1392,
    1394,  1400,  1405,  1413,  1419,  1421,  1423,  1434,  1435,  1436,
    1438,  1440,  1440,  1441,  1441,  1445,  1451,  1451,  1453,  1458,
    1464,  1469,  1475,  1480,  1485,  1491,  1496,  1501,  1507,  1512,
    1517,  1523,  1523,  1524,  1524,  1525,  1525,  1526,  1526,  1527,
    1527,  1528,  1528,  1531,  1531,  1533,  1534,  1535,  1537,  1539,
    1543,  1546,  1546,  1549,  1550,  1551,  1553,  1557,  1558,  1560,
    1562,  1562,  1563,  1563,  1564,  1564,  1564,  1565,  1566,  1566,
    1567,  1567,  1568,  1568,  1570,  1570,  1571,  1571,  1572,  1573,
    1573,  1577,  1581,  1582,  1585,  1590,  1591,  1592,  1593,  1594,
    1595,  1596,  1598,  1600,  1602,  1605,  1607,  1609,  1611,  1613,
    1615,  1617,  1619,  1621,  1623,  1628,  1632,  1633,  1636,  1641,
    1642,  1643,  1644,  1645,  1647,  1652,  1657,  1658,  1661,  1667,
    1667,  1667,  1667,  1669,  1670,  1671,  1672,  1674,  1676,  1681,
    1687,  1689,  1694,  1695,  1698,  1706,  1707,  1708,  1709,  1711,
    1713
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
  "\"wraparound\"", "\"DEFININGTRANSFORMATION\"", "\"GEODETICCRS\"",
  "\"GEODETICDATUM\"", "\"PROJECTEDCRS\"", "\"PRIMEMERIDIAN\"",
  "\"GEOGRAPHICCRS\"", "\"TRF\"", "\"VERTICALCRS\"", "\"VERTICALDATUM\"",
  "\"VRF\"", "\"TIMEDATUM\"", "\"TEMPORALQUANTITY\"",
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
  "coordinate_system_defining_transformation_scope_extent_identifier_remark",
  "spatial_cs_scope_extent_identifier_remark",
  "spatial_cs_defining_transformation_scope_extent_identifier_remark",
  "opt_separator_spatial_axis_list_opt_separator_cs_unit_scope_extent_identifier_remark",
  "opt_defining_transformation_separator_scope_extent_identifier_remark",
  "defining_transformation", "defining_transformation_name",
  "no_opt_defining_transformation_separator_scope_extent_identifier_remark",
  "opt_separator_spatial_axis_list_opt_defining_transformation_opt_separator_cs_unit_scope_extent_identifier_remark",
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
  "opt_prime_meridian_coordinate_system_defining_transformation_scope_extent_identifier_remark",
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
     405,   406,   407,   408,   409,   410,    46,    69,    49,    50,
      51,    43,    45,    58,    84,    90,    91,    40,    93,    41,
      44
};
# endif

#define YYPACT_NINF (-1313)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-691)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    2384, -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313,
   -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313,
   -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313,
   -1313, -1313, -1313, -1313, -1313, -1313, -1313,   144, -1313, -1313,
   -1313,   395, -1313, -1313, -1313,   395, -1313, -1313, -1313, -1313,
   -1313, -1313,   395,   395, -1313,   395, -1313,    35,   395, -1313,
     395, -1313,   395, -1313, -1313, -1313,   395, -1313,   395, -1313,
     395, -1313,   395, -1313,   395, -1313,   395, -1313,   395, -1313,
     395, -1313, -1313, -1313, -1313, -1313, -1313, -1313,   395, -1313,
   -1313, -1313, -1313, -1313, -1313,   395, -1313,   395, -1313,   395,
   -1313,   395, -1313,   395, -1313,   395, -1313, -1313, -1313,    63,
      63,    63,    63,    63, -1313,    81,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,   959,
      63,    63,    63,   187, -1313, -1313,    35, -1313,    35, -1313,
      35,    35, -1313,    35, -1313, -1313, -1313,   395, -1313,    35,
      35, -1313,    35,    35,    35,    35,    35,    35,    35,    35,
      35, -1313,    35, -1313,    35, -1313, -1313, -1313, -1313,   157,
   -1313, -1313, -1313, -1313, -1313,   199,   221,   242, -1313, -1313,
   -1313, -1313,   414, -1313,    35, -1313,    35,    35,    35, -1313,
      35,   395,  1210,   243,    82,    82,   755,    63,   305,   349,
     127,   313,   492,   414,   194,   203,   414,   148,   414,    62,
     146,   414,   200,  1129, -1313, -1313, -1313,   520,    73, -1313,
   -1313,    73, -1313, -1313,    73, -1313, -1313,   379,   959, -1313,
   -1313, -1313, -1313, -1313, -1313, -1313,   555, -1313, -1313, -1313,
   -1313,   296,   312,   284,   755, -1313,    35, -1313,    35,   395,
   -1313, -1313, -1313, -1313,   395,    35,   395,    35, -1313,   395,
     395,    35,    35, -1313, -1313, -1313, -1313,    35,    35,    35,
      35, -1313,    35, -1313,    35,    35,    35, -1313, -1313, -1313,
   -1313,   395,   395, -1313, -1313,    35,   395, -1313, -1313,   395,
      35,    35, -1313,    35, -1313, -1313,   395, -1313, -1313,    35,
      35,   395,    35,   395, -1313, -1313,    35,    35,   395,    35,
      35, -1313, -1313,    35,    35,   395, -1313, -1313,    35,    35,
     395, -1313, -1313,    35,    35,   395,    35,   395, -1313, -1313,
      35,   395, -1313,    35, -1313, -1313, -1313, -1313,   395, -1313,
      35,   395,    35,    35,    35,    35,    35, -1313,    35,   395,
     414, -1313,   437,   555, -1313, -1313,   365,   414,   304,   414,
     414,    63,    63,    95,   423,   100,    63,    63,   450,   450,
      95,   100,   450,   450,   755,   437,   414,   497,    63,    63,
     133,   414,    63,    63,    72,   522,   450,    63,   528, -1313,
     247,    63,   528,   555,   522,   450,    63, -1313,   528,   522,
     450,    63,   522,   450,    63, -1313, -1313,   653,   103, -1313,
      63,   450,    63,  1129,   555,   187, -1313,    63,   379,   187,
   -1313,   541,   187, -1313,   379,   502,   959, -1313,   555, -1313,
   -1313, -1313, -1313, -1313, -1313, -1313, -1313,    35,    35,   395,
   -1313,   395, -1313, -1313,    35,    35,   395,    35, -1313, -1313,
   -1313,    35,    35,    35, -1313,    35,   395, -1313, -1313, -1313,
     395,   414,    35, -1313,    35,    35, -1313,    35,   395,    35,
      35,   414,    35,    35, -1313,    35,    35,   755,   414, -1313,
      35,    35,    35, -1313,    35,    35,   395, -1313, -1313,    35,
      35,    35,   395,   414,    35,    35,    35,    35,    35, -1313,
     414,    35,   284,   414,   414, -1313, -1313, -1313, -1313,   395,
      35,    35,    35,   414,    35,    35,   414,    35,    35, -1313,
   -1313,   268, -1313,   414,    35, -1313,   414,    35,    35,    35,
     284,   414, -1313,   414,    35, -1313,    35,   395,    35, -1313,
      35,   395,   414, -1313,   537,   539,    63,    63, -1313, -1313,
     528, -1313,  1258,   527,   528,   414,   243,   100,   543,   414,
     555,  1303, -1313,   522,    90,    90,   522,    63,   522,   100,
   -1313,   522,   522,   183,   414,   396, -1313, -1313, -1313,   522,
      90,    90, -1313, -1313,    63,   414,   243,   522,  1303, -1313,
     522,   261, -1313, -1313,   528, -1313, -1313,   555, -1313, -1313,
    1289,   522,    67, -1313, -1313,   522,   135, -1313,   522,    88,
   -1313, -1313,   555, -1313, -1313,   555, -1313, -1313, -1313,   522,
     349,   873,   414,   555, -1313, -1313,   541,   983,   414,    63,
     562,  1320,   414,    63, -1313,    35, -1313, -1313,   414, -1313,
     414, -1313,    35, -1313,   414,    35, -1313,    35, -1313,    35,
     414, -1313, -1313, -1313,   395, -1313,   284,   414, -1313, -1313,
   -1313, -1313, -1313, -1313, -1313, -1313, -1313,    35, -1313,    35,
      35,    35,    35,   414, -1313,    35,   414,   414,   414,   414,
   -1313, -1313,    35,    35,   395, -1313, -1313, -1313,    35,   395,
     414,    35,    35,    35,    35, -1313,    35, -1313,    35,   414,
      35,   414,    35,    35,    35, -1313,    35, -1313, -1313, -1313,
   -1313,    35,    35,    35,   414,    35,   414,    35,   414,    35,
     327,   478, -1313,   990,   414, -1313, -1313, -1313, -1313,    35,
   -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313,
   -1313,    35,   395,    35,   395, -1313,    35,   395,    35,   395,
      35,   395,    35,   395,    35, -1313,   395,    35, -1313, -1313,
      35, -1313, -1313, -1313,   395,    35,    35,   395,    35,   395,
   -1313, -1313,    35, -1313,   395, -1313, -1313,    35,   539, -1313,
   -1313, -1313, -1313, -1313, -1313,   126, -1313,    63,   555, -1313,
     370,    95,   107,   414,    95,   414, -1313,   541, -1313, -1313,
   -1313, -1313, -1313, -1313,    63, -1313,    63, -1313,    95,   142,
     414,    95,   414,   437,   565, -1313,   370, -1313,    72,   414,
   -1313,   370,   370,   370,   370, -1313,   414, -1313,   414, -1313,
     414, -1313,   555, -1313, -1313,   555,   555, -1313,   482, -1313,
   -1313, -1313, -1313,   497,   346,   648,   760, -1313,    63,   453,
   -1313,    63,   213, -1313,  1258,   105, -1313,  1258,   408, -1313,
     653, -1313,   526, -1313,  1170,   414,    63, -1313, -1313,    63,
   -1313,  1258,   528,   414,   406,   101, -1313, -1313, -1313,    35,
   -1313,    35, -1313, -1313, -1313, -1313,    35,    35,    35,    35,
   -1313,    35, -1313,    35, -1313,    35,    35,    35,    35, -1313,
      35,    35, -1313,    35, -1313, -1313,    35,    35,    35,    35,
   -1313,    35,    35,    35,    35, -1313, -1313, -1313, -1313,   518,
     482, -1313,   990,   555, -1313,    35, -1313,    35, -1313,    35,
   -1313,    35, -1313, -1313,   414,    35,    35,    35, -1313,   414,
      35,    35, -1313,    35,    35, -1313,    35, -1313, -1313,    35,
   -1313,   414, -1313, -1313,    35,    35,    35,   395,    35, -1313,
      35,    35,   414, -1313, -1313, -1313, -1313, -1313, -1313,   414,
      35,    35,   414,   414,   414, -1313, -1313,   414,   283,   414,
     755,   755,   414, -1313,   396, -1313, -1313,   414,   865,   414,
     414,   414,   414,   414,   414,   414, -1313, -1313,   555, -1313,
   -1313, -1313,  1105,   414, -1313,   479, -1313, -1313,   213, -1313,
     105, -1313, -1313, -1313,   105, -1313, -1313,  1258, -1313,  1258,
     653, -1313, -1313, -1313,   976, -1313,   959, -1313,   437,    63,
   -1313,    35,   662,   414,   541, -1313, -1313,    35, -1313, -1313,
      35,    35,    35, -1313, -1313,    35,    35, -1313,    35, -1313,
   -1313, -1313, -1313, -1313,    35, -1313,   395,    35, -1313,    35,
   -1313,    35,    35,    35,    35, -1313, -1313,  1222, -1313, -1313,
     395, -1313,   414,    35,    35,    35, -1313,    35,    35,    35,
      35, -1313,    35, -1313,    35, -1313, -1313,   414,    35,   414,
      35, -1313,    35,   562, -1313,   395,    35,    35, -1313,   683,
   -1313, -1313, -1313,   414,   414, -1313, -1313,    63, -1313,   683,
     683,   683,   683,   683,   459, -1313,  1303, -1313,   262,   580,
     641,   105, -1313, -1313, -1313, -1313,  1258,   398,   414, -1313,
   -1313, -1313,   761,   414,   414,   395,    63, -1313, -1313, -1313,
      35,   395, -1313, -1313,    35,    35,    35,   395,    35,    35,
      35,   395,   289,   459, -1313,    35,    35, -1313,    35, -1313,
   -1313,    35, -1313,    35, -1313, -1313, -1313, -1313, -1313, -1313,
   -1313, -1313,    35,    35, -1313,   395, -1313, -1313,   406,    35,
     599, -1313,    63,   755,   725, -1313,   734, -1313,    63,   811,
   -1313, -1313,   843, -1313,    63, -1313,   580,   641,   641, -1313,
    1258, -1313, -1313,    63,   414,   437, -1313, -1313, -1313, -1313,
   -1313, -1313,   395, -1313, -1313,    35, -1313, -1313, -1313, -1313,
     395, -1313,   395,    35, -1313,    35,    35, -1313,    35,    35,
   -1313, -1313,    35,    35,   395, -1313,    35,    35, -1313,    35,
      35, -1313,    35,    35,    35, -1313, -1313, -1313, -1313,   414,
      35,    35,    35,    63, -1313,    63,    63,   832, -1313,   832,
   -1313,  2474,   414,  1267, -1313,  1267, -1313,    63,  1320,  1481,
   -1313, -1313, -1313,  3262,   641, -1313,   755,   414,  1031,   414,
     414, -1313,    35,    35,    35, -1313, -1313,    35, -1313, -1313,
   -1313, -1313, -1313, -1313, -1313,    35, -1313, -1313, -1313, -1313,
   -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313,
   -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313,
      35,    35, -1313, -1313, -1313, -1313, -1313,   414, -1313,    35,
   -1313,    35, -1313,    35, -1313,    35, -1313,    35,    35,    35,
      35,    35,   414, -1313,    35, -1313,    35, -1313, -1313,    35,
     395, -1313, -1313,   414,   755,   414,   330,   330,   655,   655,
   -1313,   552, -1313, -1313,   414,   476,   414,   538,   330,   496,
     496, -1313,    87, -1313, -1313,   406, -1313,    35, -1313, -1313,
   -1313,    35,    35, -1313,    35,   395,    35,   395, -1313, -1313,
      35,    35, -1313,    35,   395,    35, -1313, -1313, -1313,    35,
      35, -1313,    35, -1313,    35,   395,    35,   395, -1313,    35,
      35, -1313,    35, -1313, -1313,    35, -1313,    35,    35, -1313,
      35, -1313,    35,    35, -1313,    35, -1313,    35, -1313,   414,
     414, -1313, -1313,   639, -1313,  1258,   317, -1313,   555, -1313,
   -1313,   639, -1313,  1258,   317, -1313, -1313,   557, -1313,   524,
   -1313,    93, -1313,  1258, -1313,  1258, -1313, -1313,   192, -1313,
   -1313,   323, -1313, -1313, -1313,   323, -1313, -1313, -1313, -1313,
      35, -1313,    35,    35,    35,    35,   414,    35,    35,   414,
      35,    35,    35,    35,    35,   414,   414,    35,    35,    35,
   -1313, -1313,   317, -1313,   432, -1313, -1313, -1313,   317, -1313,
   -1313, -1313,    93, -1313, -1313, -1313,    79, -1313, -1313, -1313,
   -1313, -1313, -1313,    35,   414,    35,    35, -1313,    35,   395,
   -1313, -1313, -1313,    79, -1313, -1313,   575,    35, -1313, -1313,
     414, -1313, -1313
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,   454,   441,   430,   440,   161,   475,   498,   432,   530,
     533,   658,   702,   737,   740,   559,   552,   391,   614,   540,
     537,   549,   547,   669,   724,   431,   456,   476,   433,   455,
     531,   535,   534,   560,   541,   538,   550,     0,     4,     5,
       2,     0,    13,   381,   382,     0,   641,   420,   418,   419,
     421,   422,     0,     0,     3,     0,    12,   451,     0,   643,
       0,    11,     0,   645,   512,   513,     0,    14,     0,   647,
       0,    15,     0,   649,     0,    16,     0,   651,     0,    17,
       0,   642,   595,   593,   594,   596,   597,   644,     0,   646,
     648,   650,   652,    19,    18,     0,     7,     0,     8,     0,
       9,     0,    10,     0,     6,     0,     1,    73,    74,     0,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    78,   162,     0,   392,     0,   429,
       0,     0,   442,     0,   446,   447,   452,     0,   457,     0,
       0,   499,     0,     0,   458,     0,   542,     0,   542,     0,
     554,   615,     0,   659,     0,   670,   684,   671,   685,   672,
     673,   687,   674,   675,   676,   677,   678,   679,   680,   681,
     682,   683,     0,   667,     0,   703,     0,     0,     0,   708,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    75,    76,   666,     0,     0,   691,
     693,     0,   715,   717,     0,   725,   727,     0,     0,    40,
      20,    37,    38,    39,    41,    42,     0,   163,    21,    22,
      26,     0,    25,    35,     0,   164,   154,   396,     0,     0,
     490,   491,   404,   435,     0,     0,     0,     0,   434,     0,
       0,     0,     0,   599,   602,   600,   603,     0,     0,     0,
       0,   443,     0,   448,     0,   458,     0,   477,   478,   479,
     480,     0,     0,   502,   501,   495,     0,   630,   517,     0,
       0,     0,   516,     0,   626,   627,     0,   467,   470,   190,
     459,     0,   460,     0,   532,   633,     0,     0,     0,   190,
     543,   539,   636,     0,     0,     0,   548,   639,     0,     0,
       0,   566,   562,   190,   190,     0,   190,     0,   553,   620,
       0,     0,   653,     0,   654,   661,   662,   668,     0,   705,
       0,     0,     0,     0,     0,     0,     0,   710,     0,     0,
       0,    34,    27,     0,    33,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    27,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   462,
       0,     0,     0,     0,     0,     0,     0,   544,     0,     0,
       0,     0,     0,     0,     0,   558,   557,     0,     0,   555,
       0,     0,     0,     0,     0,     0,   692,     0,     0,     0,
     716,     0,     0,   726,     0,     0,     0,   707,     0,    29,
      31,    28,    36,   168,   171,   165,   166,   155,   158,     0,
     160,     0,   153,   400,     0,   386,     0,     0,   383,   388,
     397,   394,     0,     0,   406,   410,     0,   238,   428,   208,
       0,     0,     0,   492,     0,     0,   573,     0,     0,     0,
       0,     0,     0,     0,   444,   437,   190,     0,     0,   453,
       0,     0,     0,   508,   190,   495,     0,   494,   503,   190,
       0,     0,     0,     0,     0,     0,   190,   190,   461,   468,
       0,   190,   471,     0,     0,   204,   205,   206,   207,     0,
       0,     0,   190,     0,     0,     0,     0,     0,     0,    50,
     563,    48,   564,     0,   190,   567,     0,     0,     0,   655,
     663,     0,   706,     0,     0,   576,   719,     0,     0,   749,
      80,     0,     0,    32,     0,     0,     0,     0,   385,   390,
       0,   389,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   423,     0,     0,     0,     0,     0,     0,     0,
     425,     0,     0,     0,     0,     0,   450,    24,   445,     0,
       0,     0,   496,   497,     0,     0,     0,     0,     0,   514,
       0,     0,   191,   464,     0,   466,   463,   472,   469,   536,
       0,     0,     0,   545,   546,     0,     0,   551,     0,     0,
      44,    58,     0,    45,    49,     0,   561,   556,   565,     0,
       0,     0,     0,   664,   660,   704,     0,     0,     0,     0,
       0,     0,     0,     0,   709,   156,   159,   169,     0,   172,
       0,   402,   386,   401,     0,   386,   398,   394,   393,     0,
       0,   415,   416,   411,     0,   403,   407,     0,   239,   240,
     241,   242,   243,   244,   245,   246,   247,     0,   427,     0,
     607,     0,   607,     0,   574,     0,     0,     0,     0,     0,
     199,   198,   190,   190,     0,   436,   197,   196,   190,     0,
       0,     0,   482,     0,   482,   509,     0,   500,     0,     0,
       0,     0,     0,   190,   190,   473,     0,   248,   249,   250,
     251,     0,     0,     0,     0,   190,     0,   190,     0,   190,
      48,     0,    59,     0,     0,   621,   622,   623,   624,     0,
     174,   100,   133,   136,   144,   148,    98,   657,    82,    88,
      89,    93,     0,    85,     0,    92,    85,     0,    85,     0,
      85,     0,    85,     0,    85,    84,     0,   655,   640,   665,
     695,   591,   714,   723,     0,   719,   719,     0,    80,     0,
     718,   577,   413,   738,     0,    81,   739,     0,     0,   167,
     170,   387,   399,   384,   395,     0,   424,     0,   408,   405,
       0,     0,     0,     0,     0,     0,   598,     0,   601,   426,
     604,   605,   439,   438,     0,   449,     0,   474,     0,     0,
       0,     0,     0,    27,     0,   515,     0,   625,     0,     0,
     465,     0,     0,     0,     0,   631,     0,   634,     0,   637,
       0,    46,     0,    43,    68,     0,     0,    53,    71,    55,
      66,    67,   613,     0,     0,     0,     0,    91,     0,     0,
     117,     0,     0,   118,     0,     0,   119,     0,     0,   120,
       0,    83,     0,   656,     0,     0,     0,   720,   721,     0,
     722,     0,     0,     0,     0,     0,   741,   743,   157,     0,
     417,   413,   409,   252,   253,   254,   190,   607,   190,   190,
     606,   607,   611,   569,   202,     0,     0,   482,   190,   493,
     190,   190,   481,   482,   488,   510,   505,     0,   190,   190,
     628,   190,   190,   190,   190,   632,   635,   638,    52,    48,
      71,    60,     0,     0,    70,   617,    96,    85,    94,     0,
      90,    85,    87,   101,     0,    85,    85,    85,   134,     0,
      85,    85,   137,     0,    85,   145,     0,   149,   150,     0,
      79,     0,   712,   701,   695,   695,    80,     0,    80,   694,
       0,     0,     0,   414,   575,   731,   732,   729,   730,     0,
     745,     0,     0,     0,     0,   609,   608,     0,     0,     0,
       0,     0,     0,   486,     0,   483,   485,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    47,    69,     0,    54,
      57,    72,     0,     0,    95,     0,    86,    99,     0,   121,
       0,   122,   123,   132,     0,   124,   125,     0,   126,     0,
       0,   173,   696,   697,     0,   698,     0,   700,    27,     0,
     713,     0,     0,     0,     0,   742,   412,     0,   610,   612,
     190,   569,   569,   568,   203,   190,   190,   487,   190,   489,
     188,   186,   185,   187,   190,   511,     0,   190,   504,     0,
     629,     0,     0,     0,     0,    64,    56,     0,   619,   618,
       0,   616,     0,   102,   103,   104,   105,    85,    85,    85,
      85,   138,     0,   146,   142,   151,   152,     0,    80,     0,
     579,   592,   413,     0,   748,     0,   745,   745,   744,     0,
     572,   570,   571,     0,     0,   484,   506,     0,   507,     0,
       0,     0,     0,     0,     0,    63,     0,    97,     0,     0,
       0,     0,   127,   128,   129,   130,     0,     0,     0,   147,
     699,   711,     0,     0,     0,     0,     0,   747,   746,   258,
     224,     0,   192,   189,     0,   519,   229,     0,   211,    80,
     235,     0,    65,     0,    61,   106,   107,   108,   109,   110,
     111,    85,   139,     0,   143,   141,   589,   584,   585,   586,
     587,   588,   190,   190,   582,     0,   578,   590,     0,     0,
       0,   210,     0,     0,     0,   518,     0,   228,     0,     0,
     209,   233,     0,   234,     0,    62,     0,     0,     0,   131,
       0,   581,   580,     0,     0,    27,   183,   180,   179,   182,
     200,   181,     0,   201,   221,    85,   223,   380,   175,   177,
       0,   176,     0,   219,   227,   224,   215,   259,     0,   190,
     528,   523,    80,   524,     0,   232,   230,     0,   214,   211,
      80,   237,   235,     0,   112,   113,   114,   115,   140,     0,
     194,   733,   190,     0,   222,     0,     0,     0,   226,     0,
     225,     0,     0,     0,   520,     0,   522,     0,     0,     0,
     213,   212,   236,     0,     0,   135,     0,     0,     0,     0,
       0,   218,   413,     0,   194,   220,   216,   261,   321,   322,
     323,   324,   325,   326,   327,   263,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
     342,   343,   344,   345,   346,   347,   348,   349,   350,   351,
     265,   267,   352,   353,   354,   355,   356,     0,   260,   285,
     184,   524,   526,   524,   529,   413,   231,   314,   293,   295,
     297,   299,     0,   292,   309,   116,   190,   583,   736,    80,
       0,   728,   750,     0,     0,     0,     0,     0,     0,     0,
     255,     0,   521,   525,     0,     0,     0,     0,     0,     0,
       0,   257,     0,   195,   735,     0,   217,   190,   193,   358,
     362,   190,   190,   262,   190,     0,   190,     0,   264,   360,
     190,   190,   266,   190,     0,   190,   268,   373,   375,   190,
     367,   286,   367,   288,   190,     0,   190,     0,   527,   190,
     367,   315,   367,   317,   256,   190,   294,   190,   190,   296,
     190,   298,   190,   190,   300,   190,   310,   367,   313,     0,
       0,   269,   276,     0,   273,     0,     0,   275,     0,   277,
     284,     0,   281,     0,     0,   283,   287,     0,   291,     0,
     289,     0,   363,     0,   364,     0,   316,   320,     0,   318,
     301,     0,   303,   304,   305,     0,   307,   308,   311,   312,
     733,   178,   190,   190,     0,   190,     0,   190,   190,     0,
     190,   190,   190,   367,   190,     0,     0,   367,   190,   190,
     734,   272,     0,   270,     0,   274,   361,   280,     0,   278,
     359,   282,     0,   368,   369,   290,     0,   365,   372,   374,
     319,   302,   306,   190,     0,   190,   190,   377,   190,     0,
     271,   357,   279,     0,   370,   366,     0,   190,   378,   379,
       0,   371,   376
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1313, -1313, -1313,  -216,  -240,  -124, -1313,   328,  -182,   353,
   -1313, -1313, -1313, -1313, -1313, -1313,  -235,  -347,  -644,   -23,
    -782,  -632, -1313, -1313, -1313, -1313, -1313, -1313, -1313,  -550,
    -211, -1313, -1313, -1313,  -840, -1313, -1313,  -213,   -45,  1714,
     798,  2740, -1313,  -757,  -605,  -629, -1024, -1313,  -123, -1313,
   -1313,  -122, -1313, -1313, -1313,  -118,  -274, -1313, -1313,  -792,
   -1313, -1313, -1313, -1313, -1313,  -786, -1313, -1313, -1313, -1313,
    -760, -1313, -1313, -1313,   794, -1313, -1313, -1313, -1313, -1313,
     190, -1313, -1313,  -494, -1313, -1313,  -785, -1313, -1313,  -856,
   -1313, -1313, -1313, -1313,  -573,  1848,  -392, -1280,  -544, -1313,
   -1313, -1313,  -761,  -928,  -215,   180,  -258, -1313,  -490, -1313,
   -1313, -1313,  -753,  -475, -1313, -1313, -1313, -1313,  -489,  -345,
    -426, -1313, -1313,    13, -1004,  -358,  -434,  -984,  -823, -1313,
   -1223,  -600, -1313, -1313, -1313, -1313,  -610, -1313, -1313, -1313,
   -1313,  -802,  -598, -1313,  -721, -1313,  -818, -1313, -1051, -1312,
   -1030, -1313, -1029, -1313,  -752, -1313, -1313,  -618, -1313,   770,
    -414,  -260,   581,  -425,    75,   -39,  -317,   129, -1313, -1313,
   -1313,   230, -1313,   -87, -1313,  -104, -1313, -1313, -1313, -1313,
   -1313, -1313,  -849, -1313, -1313, -1313, -1313,   657,   664,   667,
     668,  -268,   727, -1313, -1313,  -102,    83, -1313, -1313, -1313,
   -1313, -1313,  -109, -1313, -1313, -1313, -1313,     9, -1313,   422,
     530,   596, -1313, -1313,   412, -1313, -1313,   680, -1313, -1313,
   -1313,  -620, -1313, -1313, -1313,   617,   627,   714,  -151,     4,
     355, -1313, -1313, -1313, -1313, -1313, -1313, -1313,  -362,  -797,
    -948, -1313, -1313,   716,   724, -1313,   267, -1313,  -755,  -674,
   -1313, -1313, -1313,  -177, -1313,   728, -1313,  -157, -1313,   698,
     732, -1313,  -163, -1313,   739, -1313,  -175, -1313, -1313,   461,
   -1313, -1313, -1313, -1313, -1313,   861,  -377, -1313, -1313,  -373,
   -1313, -1313,  -787, -1313, -1313, -1313, -1313,  -783, -1313, -1313,
     742, -1313, -1313,   685, -1313,   689, -1313, -1313,   265,  -588,
     269,   272,   274,   767, -1313, -1313, -1313, -1313, -1313, -1313,
     773, -1313, -1313, -1313, -1313,   777, -1313, -1313,   780, -1313,
   -1313,   787, -1313, -1313,   790,  -167,  -342,   165, -1313, -1313,
   -1313, -1313, -1313, -1313, -1313, -1313, -1313, -1313,   923, -1313,
     584,  -280, -1313,   -94,  -185, -1313, -1313,   -84, -1313,   -35,
   -1313, -1313, -1313,  -822, -1313, -1313, -1313,   582,   -80,   930,
   -1313, -1313,   586, -1123,  -538, -1313, -1017,   933, -1313, -1313,
   -1313,   -36,  -370, -1313, -1313
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   236,   643,   238,   905,   239,   477,
     240,   241,   429,   430,   242,   354,   243,   244,   923,   612,
     520,   613,   521,   721,   919,   614,   837,   999,   615,   838,
     922,  1066,  1067,  1153,   839,   840,   841,   924,   109,   216,
     388,   463,   951,   632,   775,   847,   738,   739,   740,   741,
     742,   743,   744,   934,  1072,   745,   746,   747,   939,   748,
     749,   943,  1082,  1163,  1249,   750,  1128,   751,   946,  1084,
     752,   753,   949,  1087,   496,   357,    41,   136,   246,   437,
     438,   439,   638,   440,   441,   640,   755,   756,  1217,  1218,
    1219,  1220,  1055,  1056,   899,   389,   683,  1221,  1277,   689,
     684,  1222,   895,  1045,   504,   458,   505,   459,  1190,  1260,
    1223,  1282,  1224,  1181,   506,  1187,   507,   508,  1193,   509,
     711,   712,   713,   886,  1140,  1146,  1150,  1141,  1228,  1327,
    1383,  1392,  1328,  1401,  1342,  1416,  1421,  1343,  1426,  1366,
    1411,  1329,  1384,  1385,  1393,  1394,  1386,  1387,  1403,  1448,
    1404,  1405,  1406,  1407,  1518,  1519,  1530,  1226,    42,   253,
     359,   551,    44,   360,   254,   138,   248,   555,   249,   451,
     647,   445,   446,   644,   642,   255,   256,   455,   456,   657,
     559,   653,   873,   654,   881,    46,    47,    48,    49,    50,
      51,   461,   140,    52,    53,   257,   447,   574,    55,   143,
     272,   475,   462,   147,   274,   478,    56,   258,    58,   149,
     203,   300,   301,   500,   302,   303,   503,    59,    60,   276,
     277,   810,   278,   279,   280,   259,   260,   464,   901,   965,
     381,    62,   152,   285,   286,   489,   485,   989,   764,   696,
     906,  1057,    63,    64,    65,   291,   493,  1185,  1266,  1233,
    1234,  1335,    66,    67,    68,    69,    70,    71,    72,   206,
      73,    74,    75,    76,    77,    78,    79,   211,    80,   324,
     325,   523,   326,   327,   526,   966,   979,   468,   675,   970,
     537,   772,   765,  1133,  1173,  1174,  1175,   766,   767,  1092,
      81,    82,    83,   261,    84,   262,    85,    86,   263,   793,
     264,   265,   266,    87,    88,   162,   330,  1003,   331,   729,
      89,   293,   294,   295,   296,    90,   307,   308,    91,   314,
     315,    92,   319,   320,    93,    94,   333,   622,    95,   164,
     337,   338,   531,    96,   182,    97,   183,   184,   967,   219,
     220,   865,    99,   186,   340,   341,   533,   342,   191,   348,
     349,   956,   957,   768,   769,   100,   222,   223,   628,   968,
     102,   225,   226,   969,  1279,   103,   774,   334,   105,   540,
     876,   877,  1033,   541,  1097
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     110,   351,   688,   353,    61,   431,   146,   111,   112,    57,
     113,   870,   350,   116,   271,   117,   737,   118,   486,   549,
     460,   119,   763,   120,   292,   121,   460,   122,   431,   123,
     548,   124,   972,   125,   318,   126,   343,   187,   188,   345,
     492,   444,   958,   127,   313,   896,   332,   306,   536,   190,
     128,   636,   129,  1046,   130,  1204,   131,   935,   132,   928,
     133,   932,   355,   936,   932,   723,   940,   932,   237,   960,
     932,   529,   961,   932,   812,    45,  1135,   954,   947,   836,
    1090,   955,  1000,    54,   795,  1410,     1,   144,   831,   937,
    1450,   267,   941,   268,     1,   944,     5,   290,  1457,     1,
    1459,   250,   197,   471,     5,  1145,   144,  1148,    10,  1380,
       5,   251,    15,   144,   189,  1469,   457,   850,   432,   853,
       5,   856,     5,   859,     5,   861,  1147,   535,  1147,  1151,
     283,     2,   730,    19,  1388,   667,   317,   344,    15,   252,
     346,     4,   483,    17,   106,  1419,   228,   252,   144,   686,
       5,    17,   339,   284,   687,   322,  1214,    33,   502,     5,
     735,  1517,   700,     5,    34,    31,    32,  1397,  1398,    26,
     145,  1505,   519,    29,  1398,  1510,  1225,    26,  1487,   530,
     339,    29,    26,    33,   513,  1239,    29,   516,   247,   145,
     680,  1041,   474,   543,   681,  1042,   145,   321,   322,  1025,
       5,  1027,  1088,    21,   361,   114,  1210,   491,  1151,   362,
     542,   364,   460,  1074,   366,   367,    21,   134,   460,  1075,
       5,   312,  1077,   932,   460,   932,   297,  1115,   189,   932,
       5,   145,   435,  1214,    36,  1214,   378,   379,  1085,   649,
     730,   382,   492,  1134,   383,  1076,   332,    36,  1078,  1096,
    1079,   387,  1429,   760,  1080,  1229,   391,  1252,   393,   953,
      19,  1350,   597,   396,     5,   329,   305,   734,   735,   698,
     401,   298,  1397,  1398,  1154,   404,   289,   982,   998,   757,
     407,   275,   410,   987,   878,   920,   412,   996,  1213,   650,
     623,    34,   483,   414,   733,   577,   417,    10,  1004,   974,
       5,   677,  1006,   977,   426,   247,  1009,  1011,  1012,     2,
       2,  1015,  1016,  1195,  1413,  1018,   734,   735,   252,     4,
       4,  1428,   761,   452,  1206,   656,  1155,  -686,  1207,  1208,
    1209,  1130,  1156,  1158,     5,   344,   932,  1206,   346,  1179,
       5,  1207,  1208,  1209,   490,  1380,  1144,     5,  1346,    10,
    1379,   673,  1380,   676,    31,    32,   678,   679,  1157,  1159,
    1160,  1161,   705,     5,   690,  1237,   247,   443,   250,  -688,
     252,  1243,   453,   730,    17,   701,   835,   720,   251,   470,
     722,   480,   781,   534,   287,   783,   714,  1211,   759,   538,
     716,  -689,  1191,   718,   546,  1216,   547,  1068,   433,   434,
    1211,   552,   736,   686,   724,   680,    31,    32,   687,   681,
    1244,   560,  -690,     5,   703,   561,   788,  1481,  1482,  1481,
    1482,   347,  1484,   567,   893,     5,  1377,  1094,  1481,  1482,
     610,     7,   611,  1353,   719,   730,  1245,  1246,  1247,   686,
     230,   584,  1250,   717,   687,   715,    12,   588,  1122,  1123,
    1124,  1125,   -59,   352,   332,   466,  1349,   -59,   -59,   -59,
     669,   671,   670,   672,   600,   548,   431,  1337,   879,   -30,
       5,  1344,  1216,  1516,  1216,  1264,   691,   693,   692,   694,
     730,   454,   549,  1271,    24,   733,  1364,   702,   722,   -51,
    1210,   611,   629,     5,  1283,  1284,   633,   907,  1380,   466,
    1382,  1382,  1391,  1391,  1285,  1400,  1286,   734,   735,     5,
     732,   733,  1382,     5,  1345,   297,   298,  1389,  1380,   134,
     229,   230,     7,   231,   232,   233,   234,   235,   883,   884,
     885,  1206,  1199,   734,   735,  1207,  1208,  1209,   457,   777,
    1395,  1395,   154,  1402,   156,     5,   158,  1412,   160,  1418,
     539,  1423,  1423,   882,  1427,     5,  1397,  1398,  1379,  1206,
    1380,   107,   108,  1207,  1208,  1209,  1232,   535,  1236,     5,
     433,  1240,  1213,   434,  1380,   835,  1362,  1472,  1363,  1231,
    1475,  1235,   214,   215,  1238,  1477,  1254,  1241,  1480,  1331,
     443,  1333,  1374,  1483,  1211,   335,   336,   918,   234,   235,
     722,   921,   651,   652,  1397,  1398,  1206,  1139,   773,   787,
    1207,  1208,  1209,  1210,   229,  1473,     5,   231,   232,   233,
     234,   235,  1211,  1478,   834,   519,   730,   247,   443,   731,
     732,   733,  1397,  1398,   734,   735,  1513,  1397,  1398,   804,
     610,   909,  1515,  1488,   806,   833,  1206,  1489,  1422,  1422,
    1207,  1208,  1209,   734,   735,   736,     5,  1070,  1528,  1529,
    1332,  1380,  1206,  1336,  1101,  1102,  1207,  1208,  1209,  1211,
    1095,   483,     5,  1348,  1022,  1023,  1389,  1380,   731,     5,
     950,   431,   611,   889,  1212,   867,   868,   722,   921,   730,
     971,  1139,   731,   732,   733,  1213,   735,   845,   832,   848,
     900,   761,   851,   476,   854,   428,   857,   997,   860,  1211,
     229,   862,  1001,   231,   232,   233,   734,   735,   736,   866,
     835,   926,   869,   929,   871,  1211,  1137,  1138,   930,   874,
     942,  1073,  1206,   945,   635,  1164,  1207,  1208,  1209,  1210,
    1355,  1206,     5,   668,  1069,  1207,  1208,  1209,  1210,  1270,
    1258,     5,   730,  1272,  1149,   731,   732,   733,  1242,  1396,
    1424,   730,  1230,  1065,   731,   732,   733,   835,  1166,  1417,
      43,  1527,  1167,  1168,  1169,  1170,   784,     5,     5,   734,
     735,   736,   288,   645,   962,   519,   165,   730,   734,   735,
     736,   732,   733,   166,    40,  1211,   167,   168,  1044,  1044,
     887,   310,   498,   891,  1211,   376,   835,   134,   229,   169,
    1089,   231,   232,   233,   734,   735,   281,   897,  1206,  1139,
     903,  1213,  1207,  1208,  1209,  1210,   282,  1114,     5,   908,
    1213,  1171,   722,  1048,   911,   912,   913,   914,   730,   141,
     583,   731,   732,   733,   150,   170,   153,   925,   155,     5,
     157,  1139,   159,   171,   699,   115,   208,   172,   431,   730,
       5,   173,   731,   732,   733,   734,   735,   736,   174,   524,
     730,   175,  1050,   731,   732,   733,  1051,  1052,  1053,  1152,
     269,  1211,     5,     3,   270,   725,   734,   735,   736,   726,
       5,     6,   727,  1081,   728,  1083,   176,   734,   735,   736,
     730,     8,   177,   731,   732,   733,   178,  1213,     9,   179,
     229,   230,  1026,   231,   232,   233,   180,  1212,   722,   181,
      14,  1514,   863,    98,   416,   420,    16,   734,   735,   736,
     101,   423,  1490,   104,   192,  1035,   193,    18,   194,   195,
      20,   196,    22,     0,     0,     0,     0,   198,   199,     0,
     200,   201,   202,   204,   205,   207,   205,   209,   210,    25,
     212,    27,   213,    28,     0,    30,     0,     0,     0,     3,
       0,    35,     0,     0,     0,     0,     0,     6,     0,     0,
       0,     0,   217,     0,   218,   221,   224,     8,   227,     0,
       0,     0,   483,     5,     9,     0,   299,    11,     0,   309,
       5,  1044,  1162,   730,   323,     0,   731,   732,   733,     0,
     730,  1107,    16,   731,   732,   733,     0,     0,     0,     0,
     762,     0,   761,    18,     0,  1116,    20,   762,    22,     0,
     734,   735,   736,     0,     0,     0,     0,   734,   735,   736,
       0,     0,     0,     0,   356,    25,   358,    27,     5,    28,
    1136,    30,     0,   363,     0,   365,     0,    35,   730,   368,
     369,   731,   732,   733,     0,   370,   371,   372,   373,     0,
     374,     0,   375,   202,   377,   762,  1248,   773,     0,     0,
       0,   465,     0,   380,     0,   734,   735,   736,   384,   385,
    1178,   386,   481,   482,  1044,     0,  1182,     0,   390,     0,
     392,   495,  1188,     0,   394,   395,  1194,     0,   398,     0,
     511,   399,   400,     0,     0,   515,   402,   403,   518,     0,
       0,   457,     5,     0,   408,     0,   528,     0,   411,     0,
    1203,   413,   730,     0,     0,   731,   732,   733,   415,     3,
     418,   419,   421,   422,   424,   229,   425,     6,   231,   232,
     233,   234,   235,   833,     0,   834,     0,     8,     0,   734,
     735,   736,     0,     0,     9,     0,     0,  1253,     0,     0,
       0,     0,  1044,     0,   484,  1255,    14,  1256,     0,   483,
       0,     0,    16,     0,   497,     0,   501,     5,     0,  1267,
       0,     0,   512,    18,     0,     0,    20,   730,    22,     0,
     731,   732,   733,  1476,     0,     0,     0,     0,     0,   761,
       0,     0,     0,   952,   762,    25,     0,    27,     0,    28,
       0,    30,     0,     0,   734,   735,   736,    35,     0,   467,
     469,     0,     0,   472,   473,   544,   545,     0,     0,     0,
       0,     0,   358,   550,     0,   553,     0,   494,     0,   554,
     556,   557,     0,   558,     0,     0,   510,     0,     0,     0,
     563,   514,   564,   565,   517,   566,     0,   568,   569,     0,
     571,   572,   527,   573,   575,     0,     0,     0,   579,   580,
     581,     0,     0,   380,     5,     0,     0,     0,   586,   587,
       0,     0,   590,   591,   730,     0,   594,   731,   732,   733,
       0,     0,     0,     0,  1230,  1375,     0,     0,   601,   602,
       0,  1474,   605,   606,     0,   608,   609,     0,     0,  1479,
       0,   734,   735,   736,     0,   619,   620,   621,     0,  1485,
       0,  1486,   626,     0,   627,     0,   630,     5,   631,     0,
    1435,     0,  1438,     0,   641,     0,     0,   730,   646,  1443,
     731,   732,   733,     0,     0,     0,     0,     0,     0,     0,
    1453,     0,  1455,     0,   134,   229,   230,   682,   231,   232,
     233,   234,   235,     0,   734,   735,   736,   229,   230,     0,
     231,   232,   233,   234,   235,     0,     0,   834,   704,   658,
     659,   660,   661,   662,   663,   664,   665,   666,   706,   707,
     708,   709,   710,   658,   659,   660,   661,   662,   663,   664,
     665,   666,     0,   229,   230,   754,   231,   232,   233,   234,
     235,   754,     0,     0,     0,   754,     0,     0,     0,     0,
       0,     0,     0,   778,     0,     0,     0,     0,     0,     0,
     550,     0,     0,   550,     0,   554,     0,   785,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   790,     0,   791,   792,   794,
     792,     0,     0,   797,  1526,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   808,
     809,   811,   809,     0,   813,     0,   814,     0,   816,     0,
     818,     0,     0,     0,   821,     0,     0,     0,     0,   822,
     823,   824,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   843,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   844,
       0,   846,     0,     0,   849,     0,   852,     0,   855,     0,
     858,     0,   858,     0,     0,   621,     0,     0,   864,     0,
       0,     0,     0,   627,   627,     0,   631,     0,     0,     0,
     872,     0,     0,     0,     0,   875,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   888,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1288,  1289,  1290,  1291,
    1292,  1293,  1294,   898,  1296,  1297,  1298,  1299,  1300,  1301,
    1302,  1303,  1304,  1305,  1306,  1307,  1308,  1309,  1310,  1311,
    1312,  1313,  1314,  1315,  1316,  1317,  1318,  1319,     0,     0,
    1322,  1323,  1324,  1325,  1326,     0,     0,     0,   927,     0,
     931,     0,     0,   931,     0,     0,   931,     0,     0,   931,
       0,     0,   931,     0,     0,     0,     0,     0,   754,     0,
       0,     0,     0,     0,     0,     0,   963,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   785,     0,   872,
       0,     0,     0,     0,     0,   792,     0,     0,     0,   792,
       0,   978,     0,   980,   981,   809,     0,     0,   984,     0,
       0,   809,     0,     0,   988,   814,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1002,     0,   858,     0,  1005,     0,   858,
       0,     0,     0,  1008,  1010,   858,     0,     0,  1014,   858,
       0,  1017,   858,     0,  1019,     0,     0,  1020,     0,     0,
       0,     0,   864,   864,  1024,     0,   631,     0,  1028,  1029,
       0,     0,     0,     0,     0,     0,     0,     0,  1032,  1034,
       0,     0,  1040,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1054,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   754,     0,     0,     0,
       0,     0,   931,     0,   931,     0,     0,     0,   931,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   754,     0,
       0,     0,     0,     0,     0,     0,   754,     0,     0,  1093,
       0,     0,     0,     0,     0,  1099,     0,     0,     0,   978,
     978,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1109,     0,  1110,
    1111,  1112,  1113,     0,     0,     0,     0,     0,     0,     0,
       0,  1118,  1119,  1120,     0,  1121,   858,   858,   858,     0,
    1126,     0,  1127,     0,     0,     0,   631,     0,  1132,     0,
     872,     0,     0,     0,  1032,  1032,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   931,     0,   304,     0,     0,
     311,     0,   316,     0,     0,   328,  1172,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1180,     0,
       0,     0,  1183,  1184,  1186,     0,  1189,   631,  1192,     0,
       0,     0,     0,  1196,  1197,     0,  1198,     0,     0,   858,
       0,  1200,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1215,     0,     0,  1205,   754,     0,
     754,     0,     0,   754,     0,     0,   754,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   858,     0,     0,     0,     0,     0,     0,
       0,  1257,     0,  1180,  1259,     0,  1261,     0,     0,     0,
    1263,  1265,     0,     0,  1268,  1269,     0,  1189,   631,     0,
    1192,  1273,  1274,     0,     0,     0,     0,     0,  1276,  1278,
       0,  1215,     0,  1215,     0,     0,     0,   754,     0,   754,
       0,     0,   754,     0,   427,     0,     0,     0,     0,     0,
       0,   442,   754,   448,   449,     0,     0,     0,     0,     0,
     872,  1354,  1276,     0,     0,  1356,     0,     0,     0,     0,
     479,     0,     0,  1357,     0,   487,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1358,  1359,
       0,     0,     0,     0,     0,     0,     0,  1361,     0,  1265,
       0,  1265,     0,   872,     0,  1365,  1367,  1368,  1369,  1370,
       0,     0,  1372,     0,     0,     0,     0,   631,     0,     0,
    1381,  1381,  1390,  1390,     0,  1399,     0,   397,     0,  1409,
       0,  1415,  1381,  1420,  1420,     0,  1425,     0,     0,     0,
       0,   405,   406,     0,   409,   562,     0,     0,     0,     0,
       0,     0,  1433,     0,  1436,   570,     0,     0,     0,     0,
       0,  1441,   578,  1444,     0,     0,     0,     0,  1447,     0,
    1449,     0,  1451,     0,     0,     0,     0,   589,  1447,     0,
    1458,     0,     0,     0,   595,  1461,     0,   598,   599,     0,
    1465,     0,     0,     0,     0,  1447,     0,   604,     0,     0,
     607,     0,     0,     0,     0,     0,     0,   616,     0,     0,
     618,     0,     0,     0,     0,   624,     0,   625,     0,     0,
       0,     0,     0,     0,     0,     0,   634,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1278,   648,
       0,  1492,  1494,   655,     0,     0,  1498,     0,     0,  1502,
       0,  1447,  1506,     0,     0,  1447,     0,     0,   685,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   697,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1523,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   576,     0,     0,     0,     0,     0,
       0,     0,   582,     0,     0,     0,   758,   585,     0,     0,
       0,     0,   770,     0,   592,   593,   776,     0,     0,   596,
       0,     0,   779,     0,   780,     0,     0,     0,   782,     0,
     603,     0,     0,     0,   786,     0,     0,     0,     0,     0,
       0,   789,   617,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   796,     1,     2,
     798,   799,   800,   801,     3,     0,     0,     0,     0,     4,
       0,     5,     6,     0,   807,     0,     0,     0,     0,     7,
       0,     0,     8,   815,     0,   817,     0,     0,     0,     9,
      10,     0,    11,     0,    12,     0,     0,     0,   825,    13,
     827,    14,   829,     0,    15,     0,     0,    16,   842,     0,
       0,     0,     0,     0,     0,    17,     0,     0,    18,     0,
      19,    20,    21,    22,     0,     0,     0,     0,     0,     0,
       0,    23,    24,     0,     0,     0,     0,     0,     0,     0,
      25,    26,    27,     0,    28,    29,    30,    31,    32,    33,
       0,    34,    35,    36,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   890,     0,   892,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   902,     0,   904,     0,     0,     0,
     802,   803,     0,   910,     0,     0,   805,     0,     0,     0,
     915,     0,   916,     0,   917,     0,     0,     0,     0,     0,
       0,   819,   820,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   826,     0,   828,     0,   830,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   959,
       0,     0,     0,     0,     0,     0,     0,   964,  1287,  1288,
    1289,  1290,  1291,  1292,  1293,  1294,  1295,  1296,  1297,  1298,
    1299,  1300,  1301,  1302,  1303,  1304,  1305,  1306,  1307,  1308,
    1309,  1310,  1311,  1312,  1313,  1314,  1315,  1316,  1317,  1318,
    1319,  1320,  1321,  1322,  1323,  1324,  1325,  1326,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1007,     0,
       0,     0,     0,  1013,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1021,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1030,     0,     0,     0,
       0,     0,     0,  1031,     0,     0,  1036,  1037,  1038,     0,
       0,  1039,     0,  1043,     0,     0,  1047,     0,     0,     0,
       0,  1049,     0,  1058,  1059,  1060,  1061,  1062,  1063,  1064,
       0,     0,     0,     0,     0,     0,     0,  1071,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   973,     0,   975,   976,     0,     0,
       0,     0,     0,     0,     0,     0,   983,  1098,   985,   986,
       0,     0,     0,     0,     0,     0,   990,   991,     0,   992,
     993,   994,   995,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1117,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1129,     0,  1131,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1142,  1143,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1165,     0,     0,     0,     0,  1176,  1177,   135,
     137,   139,   139,   142,     0,     0,   148,   139,   151,   139,
     148,   139,   148,   139,   148,   139,   148,   161,   163,     0,
     185,   185,   185,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1100,     0,
       0,     0,     0,  1103,  1104,     0,  1105,     0,     0,     0,
       0,     0,  1106,     0,     0,  1108,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1251,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   245,     0,     0,     0,     0,   273,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1275,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1330,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1347,     0,  1351,  1352,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1201,  1202,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1360,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1371,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1376,     0,  1378,
       0,     0,     0,     0,     0,     0,     0,  1262,  1408,     0,
    1414,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   436,     0,     0,     0,
    1280,   450,   137,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   488,   137,     0,     0,     0,     0,     0,     0,
       0,   499,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1470,  1471,     0,     0,   522,     0,     0,
     525,     0,     0,     0,     0,     0,     0,   532,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1496,     0,     0,  1500,  1373,     0,     0,     0,     0,  1508,
    1509,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1430,     0,     0,  1521,  1431,
    1432,     0,  1434,     0,  1437,     0,     0,     0,  1439,  1440,
       0,  1442,     0,  1445,  1532,     0,     0,  1446,     0,     0,
       0,     0,  1452,     0,  1454,     0,     0,  1456,     0,     0,
       0,     0,     0,  1460,     0,  1462,  1463,     0,  1464,     0,
    1466,  1467,     0,  1468,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   637,   639,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   674,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1491,  1493,     0,  1495,   695,  1497,  1499,     0,  1501,  1503,
    1504,     0,  1507,     0,     0,     0,  1511,  1512,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1520,     0,  1522,  1524,     0,  1525,     0,     0,   771,
       0,     0,     0,   185,     0,  1531,  1338,  1288,  1289,  1290,
    1291,  1292,  1293,  1294,  1339,  1296,  1297,  1298,  1299,  1300,
    1301,  1302,  1303,  1304,  1305,  1306,  1307,  1308,  1309,  1310,
    1311,  1312,  1313,  1314,  1315,  1316,  1317,  1318,  1319,  1340,
    1341,  1322,  1323,  1324,  1325,  1326,     0,     0,     0,     0,
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
       0,     0,     0,     0,     0,     0,     0,   880,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   894,     0,   894,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   933,     0,
       0,   938,     0,     0,     0,     0,     0,     0,     0,     0,
     948,     0,     0,     0,     0,     0,   695,     0,     0,   695,
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1086,     0,     0,     0,     0,     0,     0,     0,     0,  1091,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   894,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   695,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1227,     0,     0,     0,     0,     0,  1227,     0,
       0,     0,     0,     0,  1227,     0,     0,     0,     0,     0,
       0,     0,     0,   894,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1281,     0,   894,   894,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1334
};

static const yytype_int16 yycheck[] =
{
      45,   236,   575,   243,     0,   352,   115,    52,    53,     0,
      55,   768,   228,    58,   196,    60,   621,    62,   380,   444,
     365,    66,   627,    68,   201,    70,   371,    72,   375,    74,
     444,    76,   881,    78,   209,    80,   221,   131,   132,   224,
     385,   358,   864,    88,   207,   806,   213,   204,   421,   133,
      95,   545,    97,   981,    99,  1178,   101,   849,   103,   844,
     105,   846,   244,   849,   849,   615,   852,   852,   192,   866,
     855,   413,   869,   858,   694,     0,  1093,   864,   860,   723,
    1028,   864,   922,     0,   672,  1365,     4,     6,   720,   849,
    1402,   195,   852,   195,     4,   855,    17,   201,  1410,     4,
    1412,    19,   147,   371,    17,  1109,     6,  1111,    36,    22,
      17,    29,    50,     6,    41,  1427,    16,   746,   353,   748,
      17,   750,    17,   752,    17,   754,  1110,    26,  1112,  1113,
       3,     5,    27,    66,  1357,   561,    74,   221,    50,    57,
     224,    15,     9,    61,     0,  1368,   191,    57,     6,     7,
      17,    61,    79,    26,    12,    52,  1180,    95,   393,    17,
      55,    82,   588,    17,    97,    93,    94,    80,    81,    87,
      89,  1483,   407,    91,    81,  1487,  1180,    87,  1458,   414,
      79,    91,    87,    95,   399,  1189,    91,   402,    62,    89,
       7,   978,   374,   428,    11,   978,    89,    51,    52,   956,
      17,   958,  1024,    68,   249,   170,    14,   384,  1192,   254,
     426,   256,   557,  1005,   259,   260,    68,   154,   563,  1005,
      17,    73,  1008,  1008,   569,  1010,    23,  1067,    41,  1014,
      17,    89,   356,  1257,    99,  1259,   281,   282,  1020,   556,
      27,   286,   587,  1092,   289,  1005,   413,    99,  1008,  1032,
    1010,   296,  1375,   626,  1014,  1183,   301,  1205,   303,   864,
      66,  1278,   502,   308,    17,    65,    72,    54,    55,   586,
     315,    24,    80,    81,  1114,   320,   201,   897,   922,   621,
     325,   198,   327,   903,   778,   835,   331,   919,    96,   557,
     530,    97,     9,   338,    32,   477,   341,    36,   927,   887,
      17,   569,   931,   891,   349,    62,   935,   936,   937,     5,
       5,   940,   941,  1153,  1365,   944,    54,    55,    57,    15,
      15,  1372,    39,   362,     7,   560,  1118,   170,    11,    12,
      13,  1088,  1118,  1119,    17,   419,  1121,     7,   422,  1136,
      17,    11,    12,    13,   383,    22,  1107,    17,  1276,    36,
      20,   566,    22,   568,    93,    94,   571,   572,  1118,  1119,
    1120,  1121,   597,    17,   579,  1188,    62,    63,    19,   170,
      57,  1194,   363,    27,    61,   590,   723,   612,    29,   370,
     615,   377,   642,   418,    71,   645,   601,    70,   623,   424,
     605,   170,  1149,   608,   439,  1180,   441,  1002,    33,    34,
      70,   446,    56,     7,   619,     7,    93,    94,    12,    11,
    1196,   456,   170,    17,   591,   460,   656,  1447,  1447,  1449,
    1449,    42,  1451,   468,   797,    17,  1354,  1032,  1458,  1458,
     162,    25,   164,  1282,   609,    27,  1196,  1197,  1198,     7,
     156,   486,  1203,   606,    12,   602,    40,   492,  1077,  1078,
    1079,  1080,   163,   157,   621,    49,  1278,   168,   169,   170,
     564,   565,   564,   565,   509,   879,   813,  1269,   785,   157,
      17,  1273,  1257,  1502,  1259,  1232,   580,   581,   580,   581,
      27,    58,   907,  1240,    78,    32,  1335,   591,   723,   162,
      14,   164,   537,    17,  1255,  1256,   541,   814,    22,    49,
    1356,  1357,  1358,  1359,  1257,  1361,  1259,    54,    55,    17,
      31,    32,  1368,    17,  1274,    23,    24,    21,    22,   154,
     155,   156,    25,   158,   159,   160,   161,   162,   158,   159,
     160,     7,  1161,    54,    55,    11,    12,    13,    16,   633,
    1358,  1359,   120,  1361,   122,    17,   124,  1365,   126,  1367,
      48,  1369,  1370,   788,  1372,    17,    80,    81,    20,     7,
      22,   166,   167,    11,    12,    13,  1184,    26,  1186,    17,
      33,  1189,    96,    34,    22,   922,  1331,  1433,  1333,  1184,
    1436,  1186,   168,   169,  1189,  1441,  1215,  1192,  1444,  1263,
      63,  1265,  1349,  1449,    70,    75,    76,   832,   161,   162,
     835,   836,    59,    60,    80,    81,     7,     8,    46,   654,
      11,    12,    13,    14,   155,  1433,    17,   158,   159,   160,
     161,   162,    70,  1441,   165,   860,    27,    62,    63,    30,
      31,    32,    80,    81,    54,    55,  1492,    80,    81,   684,
     162,   818,  1498,  1461,   689,   163,     7,  1465,  1369,  1370,
      11,    12,    13,    54,    55,    56,    17,  1002,    83,    84,
    1265,    22,     7,  1268,  1041,  1042,    11,    12,    13,    70,
    1032,     9,    17,  1278,   954,   955,    21,    22,    30,    17,
     154,  1028,   164,   792,    85,   765,   766,   922,   923,    27,
     875,     8,    30,    31,    32,    96,    55,   742,   721,   744,
     809,    39,   747,   375,   749,   352,   751,   920,   753,    70,
     155,   756,   923,   158,   159,   160,    54,    55,    56,   764,
    1067,   844,   767,   845,   769,    70,  1096,  1097,   846,   774,
     854,  1005,     7,   857,   544,  1127,    11,    12,    13,    14,
    1284,     7,    17,   563,  1002,    11,    12,    13,    14,  1239,
    1225,    17,    27,  1242,  1112,    30,    31,    32,  1192,  1359,
    1370,    27,    37,   998,    30,    31,    32,  1114,     7,  1367,
       0,  1523,    11,    12,    13,    14,   647,    17,    17,    54,
      55,    56,   201,   553,   871,  1020,   129,    27,    54,    55,
      56,    31,    32,   129,     0,    70,   129,   129,   980,   981,
     791,   205,   390,   794,    70,   275,  1153,   154,   155,   129,
    1026,   158,   159,   160,    54,    55,   199,   808,     7,     8,
     811,    96,    11,    12,    13,    14,   199,  1067,    17,   816,
      96,    70,  1067,   984,   821,   822,   823,   824,    27,   112,
     485,    30,    31,    32,   117,   129,   119,   843,   121,    17,
     123,     8,   125,   129,   587,    57,   158,   129,  1205,    27,
      17,   129,    30,    31,    32,    54,    55,    56,   129,   408,
      27,   129,     7,    30,    31,    32,    11,    12,    13,  1114,
     195,    70,    17,    10,   195,   620,    54,    55,    56,   620,
      17,    18,   620,  1017,   620,  1019,   129,    54,    55,    56,
      27,    28,   129,    30,    31,    32,   129,    96,    35,   129,
     155,   156,   957,   158,   159,   160,   129,    85,  1153,   129,
      47,  1494,   757,     0,   340,   343,    53,    54,    55,    56,
       0,   345,  1470,     0,   136,   971,   138,    64,   140,   141,
      67,   143,    69,    -1,    -1,    -1,    -1,   149,   150,    -1,
     152,   153,   154,   155,   156,   157,   158,   159,   160,    86,
     162,    88,   164,    90,    -1,    92,    -1,    -1,    -1,    10,
      -1,    98,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,
      -1,    -1,   184,    -1,   186,   187,   188,    28,   190,    -1,
      -1,    -1,     9,    17,    35,    -1,   202,    38,    -1,   205,
      17,  1183,  1126,    27,   210,    -1,    30,    31,    32,    -1,
      27,  1056,    53,    30,    31,    32,    -1,    -1,    -1,    -1,
      44,    -1,    39,    64,    -1,  1070,    67,    44,    69,    -1,
      54,    55,    56,    -1,    -1,    -1,    -1,    54,    55,    56,
      -1,    -1,    -1,    -1,   246,    86,   248,    88,    17,    90,
    1095,    92,    -1,   255,    -1,   257,    -1,    98,    27,   261,
     262,    30,    31,    32,    -1,   267,   268,   269,   270,    -1,
     272,    -1,   274,   275,   276,    44,  1200,    46,    -1,    -1,
      -1,   367,    -1,   285,    -1,    54,    55,    56,   290,   291,
    1135,   293,   378,   379,  1276,    -1,  1141,    -1,   300,    -1,
     302,   387,  1147,    -1,   306,   307,  1151,    -1,   310,    -1,
     396,   313,   314,    -1,    -1,   401,   318,   319,   404,    -1,
      -1,    16,    17,    -1,   326,    -1,   412,    -1,   330,    -1,
    1175,   333,    27,    -1,    -1,    30,    31,    32,   340,    10,
     342,   343,   344,   345,   346,   155,   348,    18,   158,   159,
     160,   161,   162,   163,    -1,   165,    -1,    28,    -1,    54,
      55,    56,    -1,    -1,    35,    -1,    -1,  1212,    -1,    -1,
      -1,    -1,  1354,    -1,   380,  1220,    47,  1222,    -1,     9,
      -1,    -1,    53,    -1,   390,    -1,   392,    17,    -1,  1234,
      -1,    -1,   398,    64,    -1,    -1,    67,    27,    69,    -1,
      30,    31,    32,  1438,    -1,    -1,    -1,    -1,    -1,    39,
      -1,    -1,    -1,    43,    44,    86,    -1,    88,    -1,    90,
      -1,    92,    -1,    -1,    54,    55,    56,    98,    -1,   368,
     369,    -1,    -1,   372,   373,   437,   438,    -1,    -1,    -1,
      -1,    -1,   444,   445,    -1,   447,    -1,   386,    -1,   451,
     452,   453,    -1,   455,    -1,    -1,   395,    -1,    -1,    -1,
     462,   400,   464,   465,   403,   467,    -1,   469,   470,    -1,
     472,   473,   411,   475,   476,    -1,    -1,    -1,   480,   481,
     482,    -1,    -1,   485,    17,    -1,    -1,    -1,   490,   491,
      -1,    -1,   494,   495,    27,    -1,   498,    30,    31,    32,
      -1,    -1,    -1,    -1,    37,  1350,    -1,    -1,   510,   511,
      -1,  1435,   514,   515,    -1,   517,   518,    -1,    -1,  1443,
      -1,    54,    55,    56,    -1,   527,   528,   529,    -1,  1453,
      -1,  1455,   534,    -1,   536,    -1,   538,    17,   540,    -1,
    1385,    -1,  1387,    -1,   550,    -1,    -1,    27,   554,  1394,
      30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1405,    -1,  1407,    -1,   154,   155,   156,   573,   158,   159,
     160,   161,   162,    -1,    54,    55,    56,   155,   156,    -1,
     158,   159,   160,   161,   162,    -1,    -1,   165,   594,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   100,   101,   102,   103,   104,   105,   106,
     107,   108,    -1,   155,   156,   621,   158,   159,   160,   161,
     162,   627,    -1,    -1,    -1,   631,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   635,    -1,    -1,    -1,    -1,    -1,    -1,
     642,    -1,    -1,   645,    -1,   647,    -1,   649,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   667,    -1,   669,   670,   671,
     672,    -1,    -1,   675,  1519,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   691,
     692,   693,   694,    -1,   696,    -1,   698,    -1,   700,    -1,
     702,    -1,    -1,    -1,   706,    -1,    -1,    -1,    -1,   711,
     712,   713,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   729,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   741,
      -1,   743,    -1,    -1,   746,    -1,   748,    -1,   750,    -1,
     752,    -1,   754,    -1,    -1,   757,    -1,    -1,   760,    -1,
      -1,    -1,    -1,   765,   766,    -1,   768,    -1,    -1,    -1,
     772,    -1,    -1,    -1,    -1,   777,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   792,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   115,   116,   117,   118,
     119,   120,   121,   809,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,    -1,    -1,
     149,   150,   151,   152,   153,    -1,    -1,    -1,   844,    -1,
     846,    -1,    -1,   849,    -1,    -1,   852,    -1,    -1,   855,
      -1,    -1,   858,    -1,    -1,    -1,    -1,    -1,   864,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   872,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   879,    -1,   881,
      -1,    -1,    -1,    -1,    -1,   887,    -1,    -1,    -1,   891,
      -1,   893,    -1,   895,   896,   897,    -1,    -1,   900,    -1,
      -1,   903,    -1,    -1,   906,   907,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   925,    -1,   927,    -1,   929,    -1,   931,
      -1,    -1,    -1,   935,   936,   937,    -1,    -1,   940,   941,
      -1,   943,   944,    -1,   946,    -1,    -1,   949,    -1,    -1,
      -1,    -1,   954,   955,   956,    -1,   958,    -1,   960,   961,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   970,   971,
      -1,    -1,   978,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   988,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1002,    -1,    -1,    -1,
      -1,    -1,  1008,    -1,  1010,    -1,    -1,    -1,  1014,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1024,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1032,    -1,    -1,  1031,
      -1,    -1,    -1,    -1,    -1,  1037,    -1,    -1,    -1,  1041,
    1042,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1059,    -1,  1061,
    1062,  1063,  1064,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1073,  1074,  1075,    -1,  1077,  1078,  1079,  1080,    -1,
    1082,    -1,  1084,    -1,    -1,    -1,  1088,    -1,  1090,    -1,
    1092,    -1,    -1,    -1,  1096,  1097,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1121,    -1,   203,    -1,    -1,
     206,    -1,   208,    -1,    -1,   211,  1132,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1140,    -1,
      -1,    -1,  1144,  1145,  1146,    -1,  1148,  1149,  1150,    -1,
      -1,    -1,    -1,  1155,  1156,    -1,  1158,    -1,    -1,  1161,
      -1,  1163,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1180,    -1,    -1,  1179,  1184,    -1,
    1186,    -1,    -1,  1189,    -1,    -1,  1192,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1215,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1223,    -1,  1225,  1226,    -1,  1228,    -1,    -1,    -1,
    1232,  1233,    -1,    -1,  1236,  1237,    -1,  1239,  1240,    -1,
    1242,  1243,  1244,    -1,    -1,    -1,    -1,    -1,  1250,  1251,
      -1,  1257,    -1,  1259,    -1,    -1,    -1,  1263,    -1,  1265,
      -1,    -1,  1268,    -1,   350,    -1,    -1,    -1,    -1,    -1,
      -1,   357,  1278,   359,   360,    -1,    -1,    -1,    -1,    -1,
    1282,  1283,  1284,    -1,    -1,  1287,    -1,    -1,    -1,    -1,
     376,    -1,    -1,  1295,    -1,   381,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1320,  1321,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1329,    -1,  1331,
      -1,  1333,    -1,  1335,    -1,  1337,  1338,  1339,  1340,  1341,
      -1,    -1,  1344,    -1,    -1,    -1,    -1,  1349,    -1,    -1,
    1356,  1357,  1358,  1359,    -1,  1361,    -1,   309,    -1,  1365,
      -1,  1367,  1368,  1369,  1370,    -1,  1372,    -1,    -1,    -1,
      -1,   323,   324,    -1,   326,   461,    -1,    -1,    -1,    -1,
      -1,    -1,  1384,    -1,  1386,   471,    -1,    -1,    -1,    -1,
      -1,  1393,   478,  1395,    -1,    -1,    -1,    -1,  1400,    -1,
    1402,    -1,  1404,    -1,    -1,    -1,    -1,   493,  1410,    -1,
    1412,    -1,    -1,    -1,   500,  1417,    -1,   503,   504,    -1,
    1422,    -1,    -1,    -1,    -1,  1427,    -1,   513,    -1,    -1,
     516,    -1,    -1,    -1,    -1,    -1,    -1,   523,    -1,    -1,
     526,    -1,    -1,    -1,    -1,   531,    -1,   533,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   542,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1470,   555,
      -1,  1473,  1474,   559,    -1,    -1,  1478,    -1,    -1,  1481,
      -1,  1483,  1484,    -1,    -1,  1487,    -1,    -1,   574,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   585,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1516,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   476,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   484,    -1,    -1,    -1,   622,   489,    -1,    -1,
      -1,    -1,   628,    -1,   496,   497,   632,    -1,    -1,   501,
      -1,    -1,   638,    -1,   640,    -1,    -1,    -1,   644,    -1,
     512,    -1,    -1,    -1,   650,    -1,    -1,    -1,    -1,    -1,
      -1,   657,   524,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   673,     4,     5,
     676,   677,   678,   679,    10,    -1,    -1,    -1,    -1,    15,
      -1,    17,    18,    -1,   690,    -1,    -1,    -1,    -1,    25,
      -1,    -1,    28,   699,    -1,   701,    -1,    -1,    -1,    35,
      36,    -1,    38,    -1,    40,    -1,    -1,    -1,   714,    45,
     716,    47,   718,    -1,    50,    -1,    -1,    53,   724,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    64,    -1,
      66,    67,    68,    69,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    77,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    88,    -1,    90,    91,    92,    93,    94,    95,
      -1,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   793,    -1,   795,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   810,    -1,   812,    -1,    -1,    -1,
     682,   683,    -1,   819,    -1,    -1,   688,    -1,    -1,    -1,
     826,    -1,   828,    -1,   830,    -1,    -1,    -1,    -1,    -1,
      -1,   703,   704,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   715,    -1,   717,    -1,   719,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   865,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   873,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   934,    -1,
      -1,    -1,    -1,   939,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   951,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   962,    -1,    -1,    -1,
      -1,    -1,    -1,   969,    -1,    -1,   972,   973,   974,    -1,
      -1,   977,    -1,   979,    -1,    -1,   982,    -1,    -1,    -1,
      -1,   987,    -1,   989,   990,   991,   992,   993,   994,   995,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1003,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   886,    -1,   888,   889,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   898,  1033,   900,   901,
      -1,    -1,    -1,    -1,    -1,    -1,   908,   909,    -1,   911,
     912,   913,   914,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1072,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1087,    -1,  1089,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1103,  1104,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1128,    -1,    -1,    -1,    -1,  1133,  1134,   109,
     110,   111,   112,   113,    -1,    -1,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,    -1,
     130,   131,   132,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1040,    -1,
      -1,    -1,    -1,  1045,  1046,    -1,  1048,    -1,    -1,    -1,
      -1,    -1,  1054,    -1,    -1,  1057,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1204,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   192,    -1,    -1,    -1,    -1,   197,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1249,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1262,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1277,    -1,  1279,  1280,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1172,  1173,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1327,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1342,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1353,    -1,  1355,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1229,  1364,    -1,
    1366,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   356,    -1,    -1,    -1,
    1252,   361,   362,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   382,   383,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   391,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1429,  1430,    -1,    -1,   407,    -1,    -1,
     410,    -1,    -1,    -1,    -1,    -1,    -1,   417,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1476,    -1,    -1,  1479,  1346,    -1,    -1,    -1,    -1,  1485,
    1486,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1377,    -1,    -1,  1514,  1381,
    1382,    -1,  1384,    -1,  1386,    -1,    -1,    -1,  1390,  1391,
      -1,  1393,    -1,  1395,  1530,    -1,    -1,  1399,    -1,    -1,
      -1,    -1,  1404,    -1,  1406,    -1,    -1,  1409,    -1,    -1,
      -1,    -1,    -1,  1415,    -1,  1417,  1418,    -1,  1420,    -1,
    1422,  1423,    -1,  1425,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   546,   547,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   567,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1472,  1473,    -1,  1475,   584,  1477,  1478,    -1,  1480,  1481,
    1482,    -1,  1484,    -1,    -1,    -1,  1488,  1489,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1513,    -1,  1515,  1516,    -1,  1518,    -1,    -1,   629,
      -1,    -1,    -1,   633,    -1,  1527,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,    -1,    -1,    -1,    -1,
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
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   787,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   804,    -1,   806,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   848,    -1,
      -1,   851,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     860,    -1,    -1,    -1,    -1,    -1,   866,    -1,    -1,   869,
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
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1020,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1029,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1107,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1136,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1182,    -1,    -1,    -1,    -1,    -1,  1188,    -1,
      -1,    -1,    -1,    -1,  1194,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1203,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1253,    -1,  1255,  1256,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1267
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     4,     5,    10,    15,    17,    18,    25,    28,    35,
      36,    38,    40,    45,    47,    50,    53,    61,    64,    66,
      67,    68,    69,    77,    78,    86,    87,    88,    90,    91,
      92,    93,    94,    95,    97,    98,    99,   172,   173,   174,
     245,   247,   329,   330,   333,   335,   356,   357,   358,   359,
     360,   361,   364,   365,   367,   369,   377,   378,   379,   388,
     389,   400,   402,   413,   414,   415,   423,   424,   425,   426,
     427,   428,   429,   431,   432,   433,   434,   435,   436,   437,
     439,   461,   462,   463,   465,   467,   468,   474,   475,   481,
     486,   489,   492,   495,   496,   499,   504,   506,   509,   513,
     526,   530,   531,   536,   538,   539,     0,   166,   167,   209,
     209,   209,   209,   209,   170,   211,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   154,   212,   248,   212,   336,   212,
     363,   363,   212,   370,     6,    89,   373,   374,   212,   380,
     363,   212,   403,   363,   380,   363,   380,   363,   380,   363,
     380,   212,   476,   212,   500,   358,   359,   360,   361,   388,
     414,   415,   426,   431,   435,   461,   474,   481,   486,   489,
     492,   495,   505,   507,   508,   212,   514,   514,   514,    41,
     518,   519,   211,   211,   211,   211,   211,   209,   211,   211,
     211,   211,   211,   381,   211,   211,   430,   211,   430,   211,
     211,   438,   211,   211,   168,   169,   210,   211,   211,   510,
     511,   211,   527,   528,   211,   532,   533,   211,   209,   155,
     156,   158,   159,   160,   161,   162,   175,   176,   177,   179,
     181,   182,   185,   187,   188,   212,   249,    62,   337,   339,
      19,    29,    57,   330,   335,   346,   347,   366,   378,   396,
     397,   464,   466,   469,   471,   472,   473,   346,   366,   464,
     466,   179,   371,   212,   375,   367,   390,   391,   393,   394,
     395,   396,   397,     3,    26,   404,   405,    71,   333,   335,
     346,   416,   424,   482,   483,   484,   485,    23,    24,   245,
     382,   383,   385,   386,   210,    72,   428,   487,   488,   245,
     382,   210,    73,   433,   490,   491,   210,    74,   437,   493,
     494,    51,    52,   245,   440,   441,   443,   444,   210,    65,
     477,   479,   496,   497,   538,    75,    76,   501,   502,    79,
     515,   516,   518,   515,   518,   515,   518,    42,   520,   521,
     174,   187,   157,   175,   186,   179,   211,   246,   211,   331,
     334,   209,   209,   211,   209,   211,   209,   209,   211,   211,
     211,   211,   211,   211,   211,   211,   381,   211,   209,   209,
     211,   401,   209,   209,   211,   211,   211,   209,   211,   266,
     211,   209,   211,   209,   211,   211,   209,   266,   211,   211,
     211,   209,   211,   211,   209,   266,   266,   209,   211,   266,
     209,   211,   209,   211,   209,   211,   511,   209,   211,   211,
     528,   211,   211,   533,   211,   211,   209,   210,   180,   183,
     184,   188,   187,    33,    34,   176,   212,   250,   251,   252,
     254,   255,   210,    63,   337,   342,   343,   367,   210,   210,
     212,   340,   336,   378,    58,   348,   349,    16,   276,   278,
     290,   362,   373,   212,   398,   398,    49,   446,   448,   446,
     378,   362,   446,   446,   179,   372,   178,   180,   376,   210,
     400,   398,   398,     9,   245,   407,   409,   210,   212,   406,
     336,   424,   290,   417,   446,   398,   245,   245,   385,   212,
     384,   245,   187,   387,   275,   277,   285,   287,   288,   290,
     446,   398,   245,   275,   446,   398,   275,   446,   398,   187,
     191,   193,   212,   442,   440,   212,   445,   446,   398,   497,
     187,   503,   212,   517,   520,    26,   450,   451,   520,    48,
     540,   544,   174,   187,   211,   211,   209,   209,   331,   334,
     211,   332,   209,   211,   211,   338,   211,   211,   211,   351,
     209,   209,   210,   211,   211,   211,   211,   209,   211,   211,
     210,   211,   211,   211,   368,   211,   266,   179,   210,   211,
     211,   211,   266,   401,   209,   266,   211,   211,   209,   210,
     211,   211,   266,   266,   211,   210,   266,   175,   210,   210,
     209,   211,   211,   266,   210,   211,   211,   210,   211,   211,
     162,   164,   190,   192,   196,   199,   210,   266,   210,   211,
     211,   211,   498,   175,   210,   210,   211,   211,   529,   209,
     211,   211,   214,   209,   210,   251,   254,   212,   253,   212,
     256,   245,   345,   176,   344,   342,   245,   341,   210,   337,
     362,    59,    60,   352,   354,   210,   187,   350,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   291,   276,   346,
     366,   346,   366,   275,   212,   449,   275,   362,   275,   275,
       7,    11,   245,   267,   271,   210,     7,    12,   265,   270,
     275,   346,   366,   346,   366,   212,   410,   210,   337,   417,
     291,   275,   346,   424,   245,   187,   109,   110,   111,   112,
     113,   291,   292,   293,   275,   428,   275,   433,   275,   437,
     187,   194,   187,   200,   275,   469,   471,   472,   473,   480,
      27,    30,    31,    32,    54,    55,    56,   215,   217,   218,
     219,   220,   221,   222,   223,   226,   227,   228,   230,   231,
     236,   238,   241,   242,   245,   257,   258,   497,   210,   187,
     450,    39,    44,   215,   409,   453,   458,   459,   524,   525,
     210,   212,   452,    46,   537,   215,   210,   514,   211,   210,
     210,   332,   210,   332,   338,   211,   210,   209,   175,   210,
     211,   211,   211,   470,   211,   470,   210,   211,   210,   210,
     210,   210,   266,   266,   209,   266,   209,   210,   211,   211,
     392,   211,   392,   211,   211,   210,   211,   210,   211,   266,
     266,   211,   211,   211,   211,   210,   266,   210,   266,   210,
     266,   192,   190,   163,   165,   188,   189,   197,   200,   205,
     206,   207,   210,   211,   211,   209,   211,   216,   209,   211,
     216,   209,   211,   216,   209,   211,   216,   209,   211,   216,
     209,   216,   209,   498,   211,   512,   209,   529,   529,   209,
     214,   209,   211,   353,   209,   211,   541,   542,   254,   337,
     212,   355,   187,   158,   159,   160,   294,   378,   245,   373,
     210,   378,   210,   450,   212,   273,   273,   378,   245,   265,
     373,   399,   210,   378,   210,   178,   411,   337,   294,   424,
     210,   294,   294,   294,   294,   210,   210,   210,   187,   195,
     200,   187,   201,   189,   208,   400,   219,   245,   257,   222,
     226,   245,   257,   212,   224,   230,   236,   241,   212,   229,
     236,   241,   176,   232,   241,   176,   239,   191,   212,   243,
     154,   213,    43,   215,   453,   458,   522,   523,   524,   210,
     410,   410,   344,   245,   210,   400,   446,   509,   530,   534,
     450,   515,   353,   266,   470,   266,   266,   470,   211,   447,
     211,   211,   392,   266,   211,   266,   266,   392,   211,   408,
     266,   266,   266,   266,   266,   266,   192,   208,   189,   198,
     205,   201,   211,   478,   216,   211,   216,   210,   211,   216,
     211,   216,   216,   210,   211,   216,   216,   211,   216,   211,
     211,   210,   512,   512,   211,   214,   209,   214,   211,   211,
     210,   210,   211,   543,   211,   542,   210,   210,   210,   210,
     245,   453,   458,   210,   179,   274,   274,   210,   399,   210,
       7,    11,    12,    13,   245,   263,   264,   412,   210,   210,
     210,   210,   210,   210,   210,   187,   202,   203,   215,   277,
     290,   210,   225,   227,   230,   236,   241,   236,   241,   241,
     241,   176,   233,   176,   240,   191,   212,   244,   524,   174,
     411,   212,   460,   211,   215,   409,   458,   545,   210,   211,
     266,   447,   447,   266,   266,   266,   266,   209,   266,   211,
     211,   211,   211,   211,   175,   205,   209,   210,   211,   211,
     211,   211,   216,   216,   216,   216,   211,   211,   237,   210,
     214,   210,   211,   454,   353,   537,   209,   543,   543,     8,
     295,   298,   210,   210,   273,   295,   296,   298,   295,   296,
     297,   298,   187,   204,   205,   230,   236,   241,   236,   241,
     241,   241,   176,   234,   267,   210,     7,    11,    12,    13,
      14,    70,   245,   455,   456,   457,   210,   210,   209,   410,
     211,   284,   209,   211,   211,   418,   211,   286,   209,   211,
     279,   214,   211,   289,   209,   205,   211,   211,   211,   216,
     211,   266,   266,   209,   534,   211,     7,    11,    12,    13,
      14,    70,    85,    96,   217,   245,   257,   259,   260,   261,
     262,   268,   272,   281,   283,   295,   328,   212,   299,   274,
      37,   215,   328,   420,   421,   215,   328,   299,   215,   295,
     328,   215,   297,   299,   236,   241,   241,   241,   176,   235,
     273,   210,   411,   209,   216,   209,   209,   211,   284,   211,
     280,   211,   266,   211,   214,   211,   419,   209,   211,   211,
     279,   214,   289,   211,   211,   210,   211,   269,   211,   535,
     266,   212,   282,   273,   273,   283,   283,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   300,   303,   312,
     210,   420,   215,   420,   212,   422,   215,   312,   114,   122,
     147,   148,   305,   308,   312,   241,   274,   210,   215,   524,
     537,   210,   210,   353,   211,   269,   211,   211,   211,   211,
     210,   211,   419,   419,   353,   211,   310,   211,   211,   211,
     211,   210,   211,   266,   214,   209,   210,   274,   210,    20,
      22,   245,   260,   301,   313,   314,   317,   318,   301,    21,
     245,   260,   302,   315,   316,   317,   302,    80,    81,   245,
     260,   304,   317,   319,   321,   322,   323,   324,   210,   245,
     268,   311,   317,   319,   210,   245,   306,   313,   317,   301,
     245,   307,   315,   317,   307,   245,   309,   317,   319,   534,
     266,   266,   266,   211,   266,   209,   211,   266,   209,   266,
     266,   211,   266,   209,   211,   266,   266,   211,   320,   211,
     320,   211,   266,   209,   266,   209,   266,   320,   211,   320,
     266,   211,   266,   266,   266,   211,   266,   266,   266,   320,
     210,   210,   260,   317,   176,   260,   187,   260,   317,   176,
     260,   321,   323,   260,   323,   176,   176,   268,   317,   317,
     535,   266,   211,   266,   211,   266,   210,   266,   211,   266,
     210,   266,   211,   266,   266,   320,   211,   266,   210,   210,
     320,   266,   266,   260,   265,   260,   323,    82,   325,   326,
     266,   210,   266,   211,   266,   266,   209,   325,    83,    84,
     327,   266,   210
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int16 yyr1[] =
{
       0,   171,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   173,   173,   173,   173,   173,   173,   174,   174,
     175,   176,   176,   177,   178,   179,   179,   180,   180,   181,
     182,   183,   184,   185,   185,   186,   186,   187,   187,   187,
     187,   188,   188,   189,   190,   191,   191,   191,   192,   192,
     193,   194,   195,   196,   197,   197,   198,   198,   199,   200,
     201,   202,   202,   202,   203,   204,   205,   205,   206,   207,
     207,   208,   208,   209,   209,   210,   210,   211,   212,   213,
     214,   214,   215,   215,   215,   216,   216,   216,   217,   217,
     218,   218,   218,   219,   219,   219,   219,   220,   221,   222,
     223,   224,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   226,   226,   226,
     226,   226,   226,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   237,   238,   239,   240,   241,   242,   243,
     243,   244,   244,   245,   246,   246,   246,   246,   246,   246,
     246,   247,   248,   249,   249,   250,   250,   251,   252,   253,
     254,   255,   256,   257,   258,   259,   259,   260,   261,   262,
     262,   262,   262,   262,   263,   264,   264,   264,   264,   265,
     266,   266,   267,   268,   269,   269,   270,   270,   271,   271,
     272,   272,   273,   274,   275,   275,   275,   275,   276,   277,
     278,   279,   279,   279,   279,   280,   280,   281,   282,   283,
     283,   283,   283,   283,   284,   284,   284,   284,   285,   286,
     286,   286,   286,   287,   288,   289,   289,   289,   290,   291,
     291,   291,   291,   291,   291,   291,   291,   291,   292,   292,
     293,   293,   294,   294,   294,   295,   296,   297,   298,   299,
     300,   300,   300,   300,   300,   300,   300,   300,   300,   301,
     301,   301,   301,   301,   301,   301,   301,   302,   302,   302,
     302,   302,   302,   302,   302,   303,   303,   304,   304,   304,
     304,   304,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   306,   306,   306,   306,   307,   307,   307,   307,   308,
     308,   309,   309,   309,   310,   310,   311,   311,   311,   311,
     311,   312,   312,   312,   312,   312,   312,   312,   312,   312,
     312,   312,   312,   312,   312,   312,   312,   312,   312,   312,
     312,   312,   312,   312,   312,   312,   312,   312,   312,   312,
     312,   312,   312,   312,   312,   312,   312,   313,   314,   315,
     316,   317,   318,   319,   319,   319,   319,   320,   320,   320,
     320,   320,   321,   322,   323,   324,   325,   326,   327,   327,
     328,   329,   329,   330,   331,   331,   332,   332,   333,   334,
     334,   335,   336,   337,   338,   338,   339,   340,   341,   342,
     343,   344,   345,   346,   347,   348,   349,   350,   350,   350,
     351,   351,   352,   353,   353,   354,   354,   355,   356,   356,
     356,   357,   357,   358,   359,   360,   361,   362,   362,   363,
     364,   364,   365,   365,   366,   366,   367,   368,   368,   368,
     369,   369,   370,   371,   372,   373,   374,   374,   375,   376,
     376,   377,   377,   378,   379,   379,   379,   380,   381,   381,
     381,   381,   381,   381,   381,   381,   382,   383,   384,   385,
     386,   387,   387,   387,   388,   389,   389,   390,   390,   390,
     390,   391,   392,   392,   392,   392,   392,   393,   394,   395,
     396,   397,   398,   399,   400,   401,   401,   401,   402,   403,
     404,   405,   405,   406,   407,   408,   408,   408,   409,   410,
     411,   412,   413,   413,   414,   415,   416,   416,   417,   418,
     418,   418,   418,   418,   419,   419,   419,   420,   421,   422,
     423,   423,   424,   425,   425,   425,   426,   427,   427,   428,
     429,   429,   430,   430,   430,   430,   431,   432,   433,   434,
     434,   435,   436,   437,   438,   438,   438,   438,   438,   439,
     439,   440,   441,   442,   442,   443,   444,   445,   446,   447,
     447,   447,   447,   448,   449,   450,   451,   452,   453,   454,
     454,   454,   455,   456,   457,   457,   457,   457,   457,   457,
     458,   459,   460,   461,   461,   461,   462,   462,   463,   464,
     464,   465,   466,   466,   467,   468,   469,   470,   470,   470,
     471,   472,   473,   474,   475,   476,   477,   478,   478,   478,
     479,   480,   480,   480,   480,   481,   482,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   496,   496,   496,   496,   496,   496,   496,   496,
     496,   496,   496,   497,   497,   498,   498,   498,   499,   500,
     501,   502,   502,   503,   503,   503,   504,   505,   505,   506,
     507,   507,   507,   507,   507,   507,   507,   507,   507,   507,
     507,   507,   507,   507,   508,   508,   508,   508,   508,   508,
     508,   509,   510,   510,   511,   512,   512,   512,   512,   512,
     512,   512,   513,   514,   515,   516,   517,   518,   519,   520,
     521,   522,   523,   524,   525,   526,   527,   527,   528,   529,
     529,   529,   529,   529,   530,   531,   532,   532,   533,   534,
     534,   534,   534,   535,   535,   535,   535,   536,   537,   538,
     539,   540,   541,   541,   542,   543,   543,   543,   543,   544,
     545
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
       1,     1,     1,     1,     1,     1,     1,     1,     1,    10,
      10,     0,     3,     3,     2,     0,     2,     5,     1,     1,
       3,     1,     2,     1,     0,     3,     3,     2,    10,     0,
       2,     4,     2,    10,    10,     0,     3,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     6,     7,     6,     1,     1,
       1,     1,     3,     1,     3,     1,     3,     1,     3,     2,
       4,     6,     4,     2,     4,     2,     2,     2,     4,     6,
       4,     2,     4,     2,     2,     1,     3,     2,     1,     2,
       4,     2,     1,     1,     3,     1,     3,     1,     3,     1,
       3,     2,     4,     2,     2,     2,     4,     2,     2,     1,
       3,     2,     2,     1,     0,     2,     2,     1,     2,     4,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     6,     1,     4,
       1,     4,     1,     2,     2,     4,     6,     0,     3,     3,
       5,     7,     4,     1,     4,     1,     4,     1,     1,     1,
       1,     1,     1,     7,     5,     3,     0,     3,     7,     3,
       3,     1,     1,     5,     0,     3,     1,     1,     1,     4,
       1,     1,     1,     5,     1,     4,     1,     1,     2,     3,
       0,     2,     5,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     8,    10,     8,    10,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     9,     0,     3,     3,
       1,     1,     1,     1,     1,     6,     1,     1,     1,     4,
       2,     1,     3,     7,     1,     1,     1,     1,     0,     2,
       2,     4,     3,     5,     5,     7,     4,     1,     1,     4,
       1,     1,     2,     3,    10,     1,     1,     1,     1,     1,
       1,     7,     0,     3,     5,     3,     3,     9,     7,     9,
       1,     1,     1,     1,     7,     0,     3,     3,     1,     1,
       5,     1,     1,     1,     7,     0,     3,     3,     1,     1,
       1,     1,     1,     1,     8,    10,     1,     1,    10,     0,
       3,     5,     3,     2,     0,     3,     2,     5,     1,     1,
       1,     1,     5,     1,     1,     1,     8,     1,     1,     5,
       1,     1,     0,     2,     3,     5,     8,     1,     5,     1,
       1,     8,     1,     5,     0,     3,     5,     3,     3,     1,
       1,     4,     1,     1,     1,     4,     1,     1,     7,     0,
       3,     3,     3,     1,     1,     5,     1,     1,     7,     0,
       3,     3,     1,     5,     1,     1,     1,     1,     1,     1,
       7,     1,     1,     1,     1,     1,     1,     1,    10,     1,
       1,    10,     1,     1,    10,    10,     7,     0,     3,     3,
       9,     7,     9,    10,     1,     1,     9,     0,     2,     2,
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
       1,     4,     3,     1,     4,     0,     3,     3,     2,     1,
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
