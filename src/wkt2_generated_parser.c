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
#define YYLAST   3343

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  170
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  367
/* YYNRULES -- Number of rules.  */
#define YYNRULES  732
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1496

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
    1391,  1393,  1399,  1401,  1401,  1402,  1402,  1406,  1412,  1412,
    1414,  1419,  1425,  1430,  1436,  1441,  1446,  1452,  1457,  1462,
    1468,  1473,  1478,  1484,  1484,  1485,  1485,  1486,  1486,  1487,
    1487,  1488,  1488,  1489,  1489,  1492,  1492,  1494,  1495,  1496,
    1498,  1500,  1504,  1507,  1507,  1510,  1511,  1512,  1514,  1518,
    1519,  1521,  1523,  1523,  1524,  1524,  1525,  1525,  1525,  1526,
    1527,  1527,  1528,  1528,  1529,  1529,  1531,  1531,  1532,  1532,
    1533,  1534,  1534,  1538,  1542,  1543,  1546,  1551,  1552,  1553,
    1554,  1555,  1556,  1557,  1559,  1561,  1563,  1566,  1568,  1570,
    1572,  1574,  1576,  1578,  1580,  1582,  1584,  1589,  1593,  1594,
    1597,  1602,  1603,  1604,  1605,  1606,  1608,  1613,  1618,  1619,
    1622,  1628,  1628,  1628,  1628,  1630,  1631,  1632,  1633,  1635,
    1637,  1642,  1648,  1650,  1655,  1656,  1659,  1667,  1668,  1669,
    1670,  1672,  1674
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
     405,   406,   407,   408,   409,    46,    69,    49,    50,    51,
      43,    45,    58,    84,    90,    91,    40,    93,    41,    44
};
# endif

#define YYPACT_NINF (-1255)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-673)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    2583, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255,
   -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255,
   -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255,
   -1255, -1255, -1255, -1255, -1255, -1255, -1255,   139, -1255, -1255,
   -1255,   315, -1255, -1255, -1255,   315, -1255, -1255, -1255, -1255,
   -1255, -1255,   315,   315, -1255,   315, -1255,   -59,   315, -1255,
     315, -1255,   315, -1255, -1255, -1255,   315, -1255,   315, -1255,
     315, -1255,   315, -1255,   315, -1255,   315, -1255,   315, -1255,
     315, -1255, -1255, -1255, -1255, -1255, -1255, -1255,   315, -1255,
   -1255, -1255, -1255, -1255, -1255,   315, -1255,   315, -1255,   315,
   -1255,   315, -1255,   315, -1255,   315, -1255, -1255, -1255,     6,
       6,     6,     6,     6, -1255,    86,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   994,
       6,     6,     6,   184, -1255, -1255,   -59, -1255,   -59, -1255,
     -59,   -59, -1255,   -59, -1255, -1255, -1255,   315, -1255,   -59,
     -59, -1255,   -59,   -59,   -59,   -59,   -59,   -59,   -59,   -59,
     -59, -1255,   -59, -1255,   -59, -1255, -1255, -1255, -1255,    17,
   -1255, -1255, -1255, -1255, -1255,   116,   132,   215, -1255, -1255,
   -1255, -1255,   403, -1255,   -59, -1255,   -59,   -59,   -59, -1255,
     -59,   315,  1465,   180,    87,    87,   624,     6,   195,   362,
     110,   317,   461,   403,    37,   280,   403,   322,   403,    64,
     165,   403,   294,  1211, -1255, -1255, -1255,   477,   190, -1255,
   -1255,   190, -1255, -1255,   190, -1255, -1255,   365,   994, -1255,
   -1255, -1255, -1255, -1255, -1255, -1255,   454, -1255, -1255, -1255,
   -1255,   269,   278,   290,   624, -1255,   -59, -1255,   -59,   315,
   -1255, -1255, -1255, -1255,   315,   -59,   315,   -59, -1255,   315,
     315,   -59,   -59, -1255, -1255, -1255, -1255,   -59,   -59,   -59,
     -59, -1255,   -59, -1255,   -59,   -59,   -59, -1255, -1255, -1255,
   -1255,   315,   315, -1255, -1255,   -59,   315, -1255, -1255,   315,
     -59,   -59, -1255,   -59, -1255, -1255,   315, -1255, -1255,   -59,
     -59,   315,   -59,   315, -1255, -1255,   -59,   -59,   315,   -59,
     -59, -1255, -1255,   -59,   -59,   315, -1255, -1255,   -59,   -59,
     315, -1255, -1255,   -59,   -59,   315,   -59,   315, -1255, -1255,
     -59,   315, -1255,   -59, -1255, -1255, -1255, -1255,   315, -1255,
     -59,   315,   -59,   -59,   -59,   -59,   -59, -1255,   -59,   315,
     403, -1255,   455,   454, -1255, -1255,   338,   403,   145,   403,
     403,     6,     6,    71,   399,    79,     6,     6,   413,   413,
      71,    79,   413,   413,   624,   455,   403,   440,     6,     6,
     248,   403,     6,     6,    96,   473,   413,     6,   453, -1255,
     175,     6,   453,   454,   473,   413,     6, -1255,   453,   473,
     413,     6,   473,   413,     6, -1255, -1255,   700,   100, -1255,
       6,   413,     6,  1211,   454,   184, -1255,     6,   365,   184,
   -1255,   451,   184, -1255,   365,   452,   994, -1255,   454, -1255,
   -1255, -1255, -1255, -1255, -1255, -1255, -1255,   -59,   -59,   315,
   -1255,   315, -1255, -1255,   -59,   -59,   315,   -59, -1255, -1255,
   -1255,   -59,   -59,   -59, -1255,   -59,   315, -1255, -1255, -1255,
   -1255, -1255, -1255,   315,   403,   -59, -1255,   -59,   -59, -1255,
     -59,   315,   -59,   -59,   403,   -59,   -59, -1255,   -59,   -59,
     624,   403, -1255,   -59,   -59,   -59, -1255,   -59,   -59,   315,
   -1255, -1255,   -59,   -59,   -59,   315,   403,   -59,   -59,   -59,
     -59,   -59, -1255,   403,   -59,   290,   403,   403,   -59,   -59,
     -59,   403,   -59,   -59,   403,   -59,   -59, -1255, -1255,   131,
   -1255,   403,   -59, -1255,   403,   -59,   -59,   -59,   290,   403,
   -1255,   403,   -59, -1255,   -59,   315,   -59, -1255,   -59,   315,
     403, -1255,   472,   475,     6,     6, -1255, -1255,   453, -1255,
    1303,   450,   453,   403,   180,    79,   622,   403,   454,  1308,
   -1255,   473,    80,    80,   473,     6,   473,    79, -1255,   473,
     473,   431,   403,   504, -1255, -1255, -1255,   473,    80,    80,
   -1255, -1255,     6,   403,   180,   473,  1228, -1255,   473,   196,
   -1255, -1255,   453, -1255, -1255,   454, -1255, -1255,   473,   289,
   -1255, -1255,   473,   288, -1255,   473,   135, -1255, -1255,   454,
   -1255, -1255,   454, -1255, -1255, -1255,   473,   362,  1459,   403,
     454, -1255, -1255,   451,  1561,   403,     6,   474,  1309,   403,
       6, -1255,   -59, -1255, -1255,   403, -1255,   403, -1255,   -59,
   -1255,   403,   -59, -1255,   -59, -1255,   -59,   403, -1255, -1255,
   -1255,   315, -1255,   290,   403, -1255, -1255, -1255, -1255, -1255,
   -1255, -1255, -1255, -1255,   -59, -1255, -1255, -1255, -1255,   -59,
     -59,   -59, -1255,   -59,   -59,   -59,   -59,   403, -1255,   -59,
     403,   403,   403,   403, -1255, -1255,   -59,   -59,   315, -1255,
   -1255, -1255,   -59,   315,   403,   -59,   -59,   -59,   -59, -1255,
     -59, -1255,   -59,   403,   -59,   403,   -59,   -59,   -59, -1255,
     403,   -59,   403,   -59,   403,   -59,   274,   379, -1255,   908,
     403, -1255, -1255, -1255, -1255,   -59, -1255, -1255, -1255, -1255,
   -1255, -1255, -1255, -1255, -1255, -1255, -1255,   -59,   315,   -59,
     315, -1255,   -59,   315,   -59,   315,   -59,   315,   -59,   315,
     -59, -1255,   315,   -59, -1255, -1255,   -59, -1255, -1255, -1255,
     315,   -59,   -59,   315,   -59,   315, -1255, -1255,   -59, -1255,
     315, -1255, -1255,   -59,   475, -1255, -1255, -1255, -1255, -1255,
   -1255,   157, -1255,     6,   454, -1255,   488,   488,   488,   488,
      71,    83,   403,    71,   403, -1255,   451, -1255, -1255, -1255,
   -1255, -1255, -1255,     6, -1255,     6, -1255,    71,   299,   403,
      71,   403,   455,   634, -1255,   488, -1255,    96,   403, -1255,
   -1255,   403, -1255,   403, -1255,   403, -1255,   454, -1255, -1255,
     454,   454, -1255,   395, -1255, -1255, -1255, -1255,   440,   107,
     532,   283, -1255,     6,   385, -1255,     6,   487, -1255,  1303,
     113, -1255,  1303,   381, -1255,   700, -1255,   432, -1255,  1328,
     403,     6, -1255, -1255,     6, -1255,  1303,   453,   403,   321,
     101, -1255, -1255, -1255,   -59, -1255,   -59, -1255, -1255, -1255,
   -1255,   -59,   -59,   -59,   -59,   -59,   -59,   -59, -1255,   -59,
   -1255,   -59, -1255,   -59,   -59,   -59,   -59, -1255,   -59,   -59,
   -1255,   -59, -1255, -1255,   -59,   -59,   -59,   -59, -1255, -1255,
   -1255, -1255, -1255,   418,   395, -1255,   908,   454, -1255,   -59,
   -1255,   -59, -1255,   -59, -1255,   -59, -1255, -1255,   403,   -59,
     -59,   -59, -1255,   403,   -59,   -59, -1255,   -59,   -59, -1255,
     -59, -1255, -1255,   -59, -1255,   403, -1255, -1255,   -59,   -59,
     -59,   315,   -59, -1255,   -59,   -59,   403, -1255, -1255, -1255,
   -1255, -1255, -1255,   403,   -59,   -59,   403,   403,   403,   403,
     403,   403, -1255, -1255,   403,   189,   403,   624,   624,   403,
   -1255,   504, -1255, -1255,   403,   733,   403,   403,   403, -1255,
   -1255,   454, -1255, -1255, -1255,   403, -1255,   397, -1255, -1255,
     487, -1255,   113, -1255, -1255, -1255,   113, -1255, -1255,  1303,
   -1255,  1303,   700, -1255, -1255, -1255,  1259, -1255,   994, -1255,
     455,     6, -1255,   -59,  1129,   403,   451, -1255, -1255,   -59,
     -59,   -59,   -59, -1255, -1255,   -59,   -59,   -59, -1255, -1255,
     -59,   -59, -1255,   -59, -1255, -1255, -1255, -1255, -1255,   -59,
   -1255,   315,   -59, -1255,   -59, -1255, -1255, -1255,   944, -1255,
     403,   -59,   -59,   -59, -1255,   -59,   -59,   -59,   -59, -1255,
     -59, -1255,   -59, -1255, -1255,   403,   -59,   403,   -59, -1255,
     -59,   474, -1255,   315,   -59,   -59, -1255,   587,   587,   587,
     587, -1255, -1255, -1255,   403,   403, -1255, -1255,     6, -1255,
     587,   959, -1255, -1255,   428,   687,   555,   113, -1255, -1255,
   -1255, -1255,  1303,   366,   403, -1255, -1255, -1255,   351,   403,
     403,   315,     6, -1255, -1255, -1255,   -59,   315,   -59,   315,
     -59,   -59,   315, -1255, -1255,   -59,   -59,   435,   959, -1255,
     -59,   -59, -1255,   -59, -1255, -1255,   -59, -1255,   -59, -1255,
   -1255, -1255, -1255, -1255, -1255, -1255, -1255,   -59,   -59, -1255,
     315, -1255, -1255,   321,   -59,   860, -1255,     6,   537, -1255,
       6, -1255,  1080, -1255,     6,   624,   849, -1255, -1255,   687,
     555,   555, -1255,  1303, -1255, -1255,     6,   403,   455, -1255,
   -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255,
     315, -1255,   315,   -59, -1255,   -59, -1255,   -59,   -59,   -59,
   -1255,   -59,   -59,   -59, -1255, -1255,   -59,   -59,   315,   -59,
   -1255, -1255, -1255, -1255,   403,   -59,   -59,   -59,     6,     6,
    1309,  1778, -1255, -1255,  2693, -1255,  2922,   403,   904, -1255,
     904, -1255,     6,   555, -1255,   624,   403,  1162,   403,   403,
     -59,   -59, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255,
   -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255,
   -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255,
   -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255,   -59,
     -59,   -59,   -59,   -59,   403, -1255,   -59,   -59,   -59,   -59,
     -59,   403, -1255,   -59, -1255,   -59, -1255,   -59, -1255,   -59,
   -1255,   -59, -1255, -1255,   -59,   315, -1255, -1255,   624,   403,
      98,   403,   566,   566,   717,   717, -1255,   495,   650,   566,
     710,   710, -1255,   262, -1255, -1255,   403, -1255, -1255,   321,
     -59, -1255, -1255, -1255, -1255,   -59,   -59, -1255,   -59,   315,
   -1255,   -59,   315,   -59,   315, -1255, -1255,   -59,   -59, -1255,
     -59,   315,   -59, -1255, -1255,   -59,   -59, -1255,   -59,   315,
     -59, -1255,   -59,   -59, -1255,   -59, -1255,   -59, -1255,   -59,
     -59, -1255,   -59, -1255,   -59,   -59, -1255,   -59, -1255,   -59,
   -1255, -1255,   403,   403, -1255,   671, -1255,    74, -1255,   454,
      84, -1255,  1303, -1255,  1303, -1255, -1255,   900, -1255,  1303,
     526, -1255, -1255, -1255,   900, -1255,  1303,   526, -1255, -1255,
   -1255,   393, -1255, -1255,   201, -1255, -1255, -1255,   201, -1255,
   -1255, -1255, -1255,   -59, -1255,   -59,   -59,   -59,   403,   -59,
     403,   403,   -59,   -59,   -59,   -59,   -59,   -59,   403,   -59,
     -59,   -59,   -59, -1255,    84, -1255, -1255, -1255, -1255,    94,
   -1255, -1255, -1255, -1255,   526, -1255,   442, -1255, -1255,   526,
   -1255, -1255, -1255, -1255, -1255, -1255,   -59, -1255,   -59,   315,
     -59,   403,   -59,    94, -1255, -1255,   670, -1255, -1255, -1255,
     -59, -1255, -1255,   403, -1255, -1255
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,   439,   426,   415,   425,   161,   460,   483,   417,   515,
     518,   640,   684,   719,   722,   544,   537,   376,   599,   525,
     522,   534,   532,   651,   706,   416,   441,   461,   418,   440,
     516,   520,   519,   545,   526,   523,   535,     0,     4,     5,
       2,     0,    13,   366,   367,     0,   623,   405,   403,   404,
     406,   407,     0,     0,     3,     0,    12,   436,     0,   625,
       0,    11,     0,   627,   497,   498,     0,    14,     0,   629,
       0,    15,     0,   631,     0,    16,     0,   633,     0,    17,
       0,   624,   580,   578,   579,   581,   582,   626,     0,   628,
     630,   632,   634,    19,    18,     0,     7,     0,     8,     0,
       9,     0,    10,     0,     6,     0,     1,    73,    74,     0,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    78,   162,     0,   377,     0,   414,
       0,     0,   427,     0,   431,   432,   437,     0,   442,     0,
       0,   484,     0,     0,   443,     0,   527,     0,   527,     0,
     539,   600,     0,   641,     0,   652,   666,   653,   667,   654,
     655,   669,   656,   657,   658,   659,   660,   661,   662,   663,
     664,   665,     0,   649,     0,   685,     0,     0,     0,   690,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    75,    76,   648,     0,     0,   673,
     675,     0,   697,   699,     0,   707,   709,     0,     0,    40,
      20,    37,    38,    39,    41,    42,     0,   163,    21,    22,
      26,     0,    25,    35,     0,   164,   154,   381,     0,     0,
     475,   476,   389,   420,     0,     0,     0,     0,   419,     0,
       0,     0,     0,   584,   587,   585,   588,     0,     0,     0,
       0,   428,     0,   433,     0,   443,     0,   462,   463,   464,
     465,     0,     0,   487,   486,   480,     0,   612,   502,     0,
       0,     0,   501,     0,   608,   609,     0,   452,   455,   190,
     444,     0,   445,     0,   517,   615,     0,     0,     0,   190,
     528,   524,   618,     0,     0,     0,   533,   621,     0,     0,
       0,   551,   547,   190,   190,     0,   190,     0,   538,   602,
       0,     0,   635,     0,   636,   643,   644,   650,     0,   687,
       0,     0,     0,     0,     0,     0,     0,   692,     0,     0,
       0,    34,    27,     0,    33,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    27,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   447,
       0,     0,     0,     0,     0,     0,     0,   529,     0,     0,
       0,     0,     0,     0,     0,   543,   542,     0,     0,   540,
       0,     0,     0,     0,     0,     0,   674,     0,     0,     0,
     698,     0,     0,   708,     0,     0,     0,   689,     0,    29,
      31,    28,    36,   168,   171,   165,   166,   155,   158,     0,
     160,     0,   153,   385,     0,   371,     0,     0,   368,   373,
     382,   379,     0,     0,   391,   395,     0,   223,   413,   204,
     205,   206,   207,     0,     0,     0,   477,     0,     0,   558,
       0,     0,     0,     0,     0,     0,     0,   429,   422,   190,
       0,     0,   438,     0,     0,     0,   493,   190,   480,     0,
     479,   488,   190,     0,     0,     0,     0,     0,     0,   190,
     190,   446,   453,     0,   190,   456,     0,     0,     0,     0,
     190,     0,     0,     0,     0,     0,     0,    50,   548,    48,
     549,     0,   190,   552,     0,     0,     0,   637,   645,     0,
     688,     0,     0,   561,   701,     0,     0,   731,    80,     0,
       0,    32,     0,     0,     0,     0,   370,   375,     0,   374,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     408,     0,     0,     0,     0,     0,     0,     0,   410,     0,
       0,     0,     0,     0,   435,    24,   430,     0,     0,     0,
     481,   482,     0,     0,     0,     0,     0,   499,     0,     0,
     191,   449,     0,   451,   448,   457,   454,   521,     0,     0,
     530,   531,     0,     0,   536,     0,     0,    44,    58,     0,
      45,    49,     0,   546,   541,   550,     0,     0,     0,     0,
     646,   642,   686,     0,     0,     0,     0,     0,     0,     0,
       0,   691,   156,   159,   169,     0,   172,     0,   387,   371,
     386,     0,   371,   383,   379,   378,     0,     0,   400,   401,
     396,     0,   388,   392,     0,   224,   225,   226,   227,   228,
     229,   230,   231,   232,     0,   233,   234,   235,   236,     0,
       0,     0,   412,     0,   592,     0,   592,     0,   559,     0,
       0,     0,     0,     0,   199,   198,   190,   190,     0,   421,
     197,   196,   190,     0,     0,     0,   467,     0,   467,   494,
       0,   485,     0,     0,     0,     0,     0,   190,   190,   458,
       0,   190,     0,   190,     0,   190,    48,     0,    59,     0,
       0,   603,   604,   605,   606,     0,   174,   100,   133,   136,
     144,   148,    98,   639,    82,    88,    89,    93,     0,    85,
       0,    92,    85,     0,    85,     0,    85,     0,    85,     0,
      85,    84,     0,   637,   622,   647,   677,   576,   696,   705,
       0,   701,   701,     0,    80,     0,   700,   562,   398,   720,
       0,    81,   721,     0,     0,   167,   170,   372,   384,   369,
     380,     0,   409,     0,   393,   390,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   583,     0,   586,   411,   589,
     590,   424,   423,     0,   434,     0,   459,     0,     0,     0,
       0,     0,    27,     0,   500,     0,   607,     0,     0,   450,
     613,     0,   616,     0,   619,     0,    46,     0,    43,    68,
       0,     0,    53,    71,    55,    66,    67,   598,     0,     0,
       0,     0,    91,     0,     0,   117,     0,     0,   118,     0,
       0,   119,     0,     0,   120,     0,    83,     0,   638,     0,
       0,     0,   702,   703,     0,   704,     0,     0,     0,     0,
       0,   723,   725,   157,     0,   402,   398,   394,   237,   238,
     239,   190,   190,   190,   190,   592,   190,   190,   591,   592,
     596,   554,   202,     0,     0,   467,   190,   478,   190,   190,
     466,   467,   473,   495,   490,     0,   190,   190,   610,   614,
     617,   620,    52,    48,    71,    60,     0,     0,    70,   190,
      96,    85,    94,     0,    90,    85,    87,   101,     0,    85,
      85,    85,   134,     0,    85,    85,   137,     0,    85,   145,
       0,   149,   150,     0,    79,     0,   694,   683,   677,   677,
      80,     0,    80,   676,     0,     0,     0,   399,   560,   713,
     714,   711,   712,     0,   727,     0,     0,     0,     0,     0,
       0,     0,   594,   593,     0,     0,     0,     0,     0,     0,
     471,     0,   468,   470,     0,     0,     0,     0,     0,    47,
      69,     0,    54,    57,    72,     0,    95,     0,    86,    99,
       0,   121,     0,   122,   123,   132,     0,   124,   125,     0,
     126,     0,     0,   173,   678,   679,     0,   680,     0,   682,
      27,     0,   695,     0,     0,     0,     0,   724,   397,     0,
       0,     0,     0,   595,   597,   190,   554,   554,   553,   203,
     190,   190,   472,   190,   474,   188,   186,   185,   187,   190,
     496,     0,   190,   489,     0,   611,    64,    56,     0,   601,
       0,   102,   103,   104,   105,    85,    85,    85,    85,   138,
       0,   146,   142,   151,   152,     0,    80,     0,   564,   577,
     398,     0,   730,     0,   727,   727,   726,     0,     0,     0,
       0,   557,   555,   556,     0,     0,   469,   491,     0,   492,
       0,     0,    63,    97,     0,     0,     0,     0,   127,   128,
     129,   130,     0,     0,     0,   147,   681,   693,     0,     0,
       0,     0,     0,   729,   728,   243,   214,     0,   209,     0,
      80,   220,     0,   192,   189,     0,   504,    65,     0,    61,
     106,   107,   108,   109,   110,   111,    85,   139,     0,   143,
     141,   574,   569,   570,   571,   572,   573,   190,   190,   567,
       0,   563,   575,     0,     0,     0,   213,     0,     0,   208,
       0,   218,     0,   219,     0,     0,     0,   503,    62,     0,
       0,     0,   131,     0,   566,   565,     0,     0,    27,   183,
     180,   179,   182,   200,   181,   201,   217,   365,   175,   177,
       0,   176,     0,   215,   244,     0,   212,   209,    80,     0,
     222,   220,     0,   190,   513,   508,    80,   509,     0,   112,
     113,   114,   115,   140,     0,   194,   715,   190,     0,     0,
       0,     0,   211,   210,     0,   221,     0,     0,     0,   505,
       0,   507,     0,     0,   135,     0,     0,     0,     0,     0,
       0,   194,   216,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,   339,   340,   341,   299,
     246,   248,   250,   252,     0,   245,   270,   278,   280,   282,
     284,     0,   277,   294,   184,   509,   511,   509,   514,   398,
     116,   190,   568,   718,    80,     0,   710,   732,     0,     0,
       0,     0,     0,     0,     0,     0,   240,     0,     0,     0,
       0,     0,   242,     0,   506,   510,     0,   195,   717,     0,
     190,   193,   347,   358,   360,   190,   352,   300,   352,     0,
     302,   190,     0,   190,     0,   241,   343,   190,   190,   247,
     190,     0,   190,   249,   345,   190,   190,   251,   190,     0,
     190,   253,   190,   352,   271,   352,   273,   190,   279,   190,
     190,   281,   190,   283,   190,   190,   285,   190,   295,   352,
     298,   512,     0,     0,   301,     0,   305,     0,   303,     0,
       0,   348,     0,   349,     0,   254,   261,     0,   258,     0,
       0,   260,   262,   269,     0,   266,     0,     0,   268,   272,
     276,     0,   274,   286,     0,   288,   289,   290,     0,   292,
     293,   296,   297,   715,   178,   190,   190,   352,     0,   190,
       0,     0,   190,   190,     0,   190,   190,   190,     0,   190,
     352,   190,   190,   716,     0,   353,   354,   304,   346,     0,
     350,   357,   359,   257,     0,   255,     0,   259,   265,     0,
     263,   344,   267,   275,   287,   291,   190,   362,   190,     0,
     190,     0,   190,     0,   355,   351,     0,   256,   342,   264,
     190,   363,   364,     0,   356,   361
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1255, -1255, -1255,  -223,  -237,  -172, -1255,   244,  -193,   297,
   -1255, -1255, -1255, -1255, -1255, -1255,   -94,  -336,  -652,   -87,
    -785,  -635, -1255, -1255, -1255, -1255, -1255, -1255, -1255,  -553,
    -253, -1255, -1255, -1255,  -867, -1255, -1255,  -246,   -45,  1461,
     501,  2101, -1255,  -755,  -578,  -599, -1255, -1255,  -166, -1255,
   -1255,  -154, -1255, -1255, -1255,  -142,  -294, -1255, -1255,  -796,
   -1255, -1255, -1255, -1255, -1255,  -776, -1255, -1255, -1255, -1255,
    -757, -1255, -1255, -1255,   503, -1255, -1255, -1255, -1255, -1255,
     177, -1255, -1255,  -501, -1255, -1255,  -767, -1255, -1255,  -793,
   -1255, -1255, -1255, -1255,  -572,  1647,  -392, -1222,  -528, -1255,
   -1255, -1255,  -773,  -921,   -42, -1255,  -481, -1255, -1255, -1255,
   -1255,  -476,  -330,   162, -1255, -1255,  -470, -1004,  -346,  -417,
    -982,  -794, -1255,  -768,  -568, -1255, -1255, -1255, -1255,  -567,
   -1255, -1255, -1255, -1255,  -711,  -556, -1255,  -571, -1255, -1202,
   -1255,  -761, -1254, -1028, -1255, -1088, -1255,  -709, -1255, -1255,
    -878, -1255,   780,  -416,  -298,   589,  -430,    73,   -60,  -334,
     149, -1255, -1255, -1255,   249, -1255,   -62, -1255,  -148, -1255,
   -1255, -1255, -1255, -1255, -1255,  -850, -1255, -1255, -1255, -1255,
     676,   677,   681,   684,  -272,   510, -1255, -1255,   -91,    62,
   -1255, -1255, -1255, -1255, -1255,   -97, -1255, -1255, -1255, -1255,
      19, -1255,   590,   541,   612, -1255, -1255,   434, -1255, -1255,
     689, -1255, -1255, -1255,  -634, -1255, -1255, -1255,   626,   629,
     222,  -151,     2,   344, -1255, -1255, -1255, -1255, -1255, -1255,
   -1255,  -358,  -795,  -941, -1255, -1255,   704,   706, -1255,   251,
   -1255,  -723,  -600, -1255, -1255, -1255,  -197, -1255,   709, -1255,
    -163, -1255,   690,   713, -1255,  -171, -1255,   721, -1255,  -179,
   -1255, -1255,   443, -1255, -1255, -1255, -1255, -1255,   306,  -270,
   -1255, -1255,  -377, -1255, -1255,  -781, -1255, -1255, -1255, -1255,
    -762, -1255, -1255,   726, -1255, -1255,   669, -1255,   673, -1255,
   -1255,   252,  -613,   253,   258,   261,   736, -1255, -1255, -1255,
   -1255, -1255,   753, -1255, -1255, -1255, -1255,   755, -1255, -1255,
     756, -1255, -1255,   759, -1255, -1255,   760,  -175,  -352,   143,
   -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255, -1255,
     898, -1255,   559,  -160, -1255,  -120,  -187, -1255, -1255,   -68,
   -1255,   181, -1255, -1255, -1255,  -814, -1255, -1255, -1255,   557,
      35,   902, -1255, -1255,   563, -1087,  -524, -1255,  -999,   910,
   -1255, -1255, -1255,   -41,  -286, -1255, -1255
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
    1129,  1205,  1294,  1359,  1367,  1295,  1374,  1301,  1378,  1383,
    1302,  1388,  1321,  1347,  1289,  1360,  1361,  1368,  1369,  1362,
    1349,  1350,  1396,  1351,  1352,  1353,  1354,  1478,  1479,  1493,
    1203,    42,   253,   359,   549,    44,   360,   254,   138,   248,
     553,   249,   451,   644,   445,   446,   641,   639,   255,   256,
     455,   456,   654,   557,   650,   868,   651,   876,    46,    47,
      48,    49,    50,    51,   464,   140,    52,    53,   257,   447,
     572,    55,   143,   272,   478,   465,   147,   274,   481,    56,
     258,    58,   149,   203,   300,   301,   503,   302,   303,   506,
      59,    60,   276,   277,   809,   278,   279,   280,   259,   260,
     467,   899,   959,   381,    62,   152,   285,   286,   492,   488,
     986,   760,   700,   904,  1052,    63,    64,    65,   291,   496,
    1177,  1241,  1217,  1218,  1309,    66,    67,    68,    69,    70,
      71,    72,   206,    73,    74,    75,    76,    77,    78,    79,
     211,    80,   324,   325,   521,   326,   327,   524,   960,   976,
     471,   679,   964,   535,   768,   761,  1119,  1158,  1159,  1160,
     762,   763,  1080,    81,    82,    83,   261,    84,   262,    85,
      86,   263,   792,   264,   265,   266,    87,    88,   162,   330,
     331,   725,    89,   293,   294,   295,   296,    90,   307,   308,
      91,   314,   315,    92,   319,   320,    93,    94,   333,   619,
      95,   164,   337,   338,   529,    96,   182,    97,   183,   184,
     961,   219,   220,   860,    99,   186,   340,   341,   531,   342,
     191,   348,   349,   950,   951,   764,   765,   100,   222,   223,
     625,   962,   102,   225,   226,   963,  1248,   103,   770,   334,
     105,   538,   871,   872,  1025,   539,  1085
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     110,   692,    61,   271,   292,   350,   353,   111,   112,   865,
     113,   187,   188,   116,   547,   117,   431,   118,   146,    57,
     237,   119,   489,   120,   444,   121,   966,   122,   546,   123,
     318,   124,   894,   125,   343,   126,   313,   345,   332,   431,
     733,   306,   633,   127,   534,   952,   759,   267,   929,   993,
     128,   355,   129,   290,   130,   495,   131,  1041,   132,   719,
     133,   527,    54,   794,   811,   190,   954,   831,   930,   955,
     941,   934,   922,    45,   926,     1,  1187,   926,   948,  1078,
     926,   826,  1121,   926,     1,   144,   926,   931,  1193,   144,
     935,     1,   144,   938,  1398,   457,  1136,   949,  1346,   474,
       5,     5,   197,    19,   268,  1127,   250,  1127,  1132,   305,
     114,     5,  1193,   283,    15,     5,   251,     5,  1348,  1420,
    1342,  1422,  1370,  1370,     5,  1375,  1380,   533,  1385,  1385,
       5,  1389,    10,    34,   726,  1432,   284,   252,   317,   106,
     726,    17,   351,   845,   252,   848,   228,   851,    17,   854,
       2,   856,   322,   344,  1343,  1344,   346,    26,    33,   134,
       4,    29,     2,   732,  1207,  1344,    26,   145,   731,  1195,
      29,   145,     4,    26,   145,  1437,  1477,    29,  1343,  1344,
     339,   477,     5,  1457,   435,    15,  -668,   494,    31,    32,
    1132,  1102,     5,  1195,  1036,  1017,  1473,  1019,   486,   298,
       2,  1062,  1076,   540,   361,  1443,     5,   247,   443,   362,
       4,   364,  1447,  1037,   366,   367,   321,   322,     5,   247,
     646,  1063,  1451,  1342,  1065,   189,  1452,  1073,   757,    33,
    1120,   189,    10,   926,  1139,   926,   378,   379,   332,   926,
    1064,   382,   247,  1066,   383,  1067,   756,  1227,  1315,  1068,
     702,   387,  1392,   252,  1213,   495,   391,   486,   393,   432,
     275,   979,  1084,   396,   991,     5,   753,   984,   595,   339,
     401,  1178,   971,   873,   289,   404,   974,   914,   989,     5,
     407,   947,   410,   647,  1342,  -670,   412,   575,    31,    32,
    1208,   620,   607,   414,   608,   681,   417,     5,  1216,   505,
       5,  -671,   452,   297,   426,   144,   690,  1436,  1140,  1436,
     726,   691,  1439,   517,   728,   729,     5,   882,   883,   884,
     528,  1116,   996,   493,  1311,  1135,   998,  1164,  1141,  1143,
    1001,  1003,  1004,  1436,   541,  1007,  1008,   730,   731,  1010,
     926,   777,  1343,  1344,   779,   906,     7,  1142,  1144,  1145,
    1146,   344,   507,    10,   346,    19,    21,   511,  1151,   329,
     514,    12,  1152,  1153,  1154,  1155,  1476,  1435,     5,  1435,
     469,   433,   434,   684,   252,  1171,  1209,   685,    17,   483,
    1212,   250,   453,   830,  -672,    34,    36,   145,   287,   473,
      21,   251,   707,  1435,   544,   312,   545,  1340,     5,    24,
    1189,   550,     5,  1219,  1190,  1191,  1192,   347,   726,    31,
      32,   558,   726,  1225,   673,   675,   784,   729,   559,   891,
      36,  1156,  1220,  1221,  1222,   352,   565,   715,   728,   729,
     695,   697,   713,  1314,   -30,   -51,   711,   608,   684,   730,
     731,   706,   685,   332,   582,   230,  1082,   874,     5,   690,
     586,   730,   731,  1233,   691,  1250,  1251,   454,   546,  1336,
     729,  1239,   469,  1194,   653,     7,  1108,  1109,  1110,  1111,
       5,   674,   676,  1343,  1344,   547,   431,   533,     5,   905,
     107,   108,   730,   731,   297,   298,  1310,   696,   698,   457,
     626,   134,   229,   230,   630,   231,   232,   233,   234,   235,
     537,   709,  1189,    40,     5,   433,  1190,  1191,  1192,   434,
     773,   690,     5,   443,   726,   716,   691,  1342,   718,   672,
     769,     5,   677,  1296,   680,  1303,   755,   682,   683,  1358,
    1358,  1366,  1366,  1189,  1373,   694,  1358,  1190,  1191,  1192,
     607,   730,   731,     5,  1189,  1125,   705,  1182,  1190,  1191,
    1192,  1193,   335,   336,     5,  1363,   710,   828,   115,  1338,
     712,  1381,   727,   714,   726,  1194,  1376,   727,   728,   729,
     214,   215,  1390,  1189,   720,  1343,  1344,  1190,  1191,  1192,
     830,   608,  1334,     5,  1335,   944,  1356,  1196,  1342,   468,
    1206,   730,   731,   732,  1210,  1125,  1194,   -59,  1215,   532,
     484,   485,   -59,   -59,   -59,   536,   783,  1194,   229,   498,
     731,   231,   232,   233,  1442,   234,   235,  1445,   509,   479,
     907,  1446,   141,   513,  1449,   718,   516,   150,  1450,   153,
     827,   155,  1195,   157,   526,   159,  1194,   192,  1305,   193,
    1307,   194,   195,   803,   196,   878,   879,   880,   805,   428,
     198,   199,  1252,   200,   201,   202,   204,   205,   207,   205,
     209,   210,  1306,   212,   994,   213,  1083,     5,   990,  1313,
    1356,  1480,  1342,   920,   470,   472,  1482,   936,   475,   476,
     939,   648,   649,   965,   431,   217,   923,   218,   221,   224,
     877,   227,   497,   840,   887,   843,   247,   443,   846,   924,
     849,   508,   852,  1061,   855,   299,   512,   857,   309,   515,
     154,   898,   156,   323,   158,   861,   160,   525,   864,   632,
     866,  1149,   830,  1319,  1189,   869,  1232,     5,  1190,  1191,
    1192,  1364,  1342,   912,     5,  1235,   718,   915,  1364,  1342,
    1045,   730,   731,  1130,  1046,  1047,  1048,   356,   704,   358,
       5,  1343,  1344,  1491,  1492,  1211,   363,  1371,   365,  1384,
    1384,   517,   368,   369,  1386,   830,  1092,  1093,   370,   371,
     372,   373,  1379,   374,  1490,   375,   202,   377,   229,   230,
      43,   231,   232,   233,  1039,  1039,   380,  1194,  1014,  1015,
     288,   384,   385,   780,   386,  1077,   862,   863,  1123,  1124,
     642,   390,   830,   392,   956,   165,   166,   394,   395,   885,
     167,   398,   889,   168,   399,   400,   376,   310,   169,   402,
     403,  1101,   718,   915,   501,   281,   895,   408,   282,   901,
    1043,   411,   581,   170,   413,   171,   703,  1069,   172,  1071,
     919,   415,   173,   418,   419,   421,   422,   424,   208,   425,
     174,   522,   431,   134,   229,   175,  1189,   231,   232,   233,
    1190,  1191,  1192,  1193,   269,   176,     5,  1189,   270,   721,
     722,  1190,  1191,  1192,  1193,   723,   726,     5,   724,   727,
     728,   729,   177,   487,   178,   179,  1214,   726,   180,   181,
     727,   728,   729,   500,  1481,   504,   858,  1056,    98,   416,
     420,   510,   101,   730,   731,   732,  1018,  1189,   423,  1453,
     104,  1190,  1191,  1192,   730,   731,   732,     5,   517,  1194,
       0,     5,  1342,     0,  1027,     0,     0,     0,     0,     0,
    1194,   726,     0,     0,   727,   728,   729,     0,   542,   543,
    1147,  1214,     0,     0,  1195,   358,   548,     0,   551,     0,
       0,     0,   552,   554,   555,  1195,   556,     0,   730,   731,
     732,     0,     0,     0,   718,     0,   561,     0,   562,   563,
    1194,   564,     0,   566,   567,     0,   569,   570,     0,   571,
     573,     0,  1039,     0,   577,   578,   579,     0,     0,   380,
       0,     0,     0,     0,   584,   585,     0,     0,   588,   589,
       0,     0,   592,     0,     3,     0,  1098,  1137,     0,   598,
     599,  1223,     6,   602,   603,     0,   605,   606,     0,     0,
       0,     0,     8,     0,     0,     0,   616,   617,   618,     9,
       0,     0,    11,   623,     0,   624,     0,   627,  1122,   628,
       0,     0,     0,     0,   718,     0,     0,    16,     0,     0,
       0,   638,  1039,     0,     0,   643,     0,     0,    18,     0,
       0,    20,   229,    22,     0,   231,   232,   233,   234,   235,
     828,     0,   829,     0,   686,     0,  1163,     0,     0,    25,
       0,    27,  1167,    28,  1170,    30,     0,  1174,  1125,     0,
       0,    35,     0,     0,     0,   708,     0,     5,   229,   230,
       0,   231,   232,   233,   234,   235,     0,   726,   829,     0,
     727,   728,   729,   229,     0,  1186,   231,   232,   233,   234,
     235,   750,     0,   829,     0,  1039,     0,   750,     0,     0,
       0,   750,     0,   774,   730,   731,   732,     0,   486,     0,
     548,     0,     0,   548,     0,   552,     5,   781,     0,     0,
       0,     0,     0,     0,     0,  1228,   726,  1229,     0,   727,
     728,   729,     0,     0,     0,   786,     0,     0,   757,     0,
     787,   788,   789,  1242,   790,   791,   793,   791,     0,     5,
     796,     0,     0,   730,   731,   732,     0,     0,     0,   726,
       0,     0,   727,   728,   729,     0,   807,   808,   810,   808,
       0,   812,     0,   813,     0,   815,   758,   817,   769,     0,
       0,     0,     0,     0,     0,     0,   730,   731,   732,     0,
       0,     3,     0,     0,     0,     0,   838,     0,     0,     6,
    1440,     0,  1441,     0,     0,     0,     0,  1444,   839,     8,
     841,     0,     0,   844,  1448,   847,     9,   850,     0,   853,
       0,   853,     0,     0,   618,     0,     0,   859,    14,     0,
       0,     0,   624,   624,    16,   628,     0,     0,     0,   867,
    1339,     0,     0,     0,   870,    18,     5,     0,    20,     0,
      22,     0,     0,     0,     0,     0,   726,     0,     0,   727,
     728,   729,     0,     0,   886,     0,    25,     0,    27,     0,
      28,     0,    30,   758,  1399,  1438,     0,  1402,    35,  1404,
       0,   896,     0,   730,   731,   732,  1409,     0,     0,     0,
       0,     0,     0,     0,  1416,     0,     5,   655,   656,   657,
     658,   659,   660,   661,   662,   663,   726,   486,     0,   727,
     728,   729,   921,     0,   925,     5,     0,   925,     0,     0,
     925,     0,     0,   925,     0,   726,   925,     0,   727,   728,
     729,     0,   750,   730,   731,   732,     0,   757,     0,     0,
     957,   946,   758,     0,     0,   781,     0,   867,     0,     0,
       0,     0,   730,   731,   732,     0,   791,     0,     0,     0,
     791,     0,   975,     0,   977,   978,   808,     0,     0,   981,
       0,     0,   808,     0,     0,   985,   813,   655,   656,   657,
     658,   659,   660,   661,   662,   663,   664,   665,   666,   667,
     668,     0,   853,     0,   997,     0,   853,     0,     0,     0,
    1000,  1002,   853,     0,  1486,  1006,   853,     0,  1009,   853,
       0,  1011,     0,     0,  1012,     0,     0,     0,     0,   859,
     859,  1016,     0,   628,     0,  1020,  1021,   229,   230,     0,
     231,   232,   233,   234,   235,  1024,  1026,     0,     0,     3,
       0,     0,     0,     0,     0,     0,     5,     6,  1035,     0,
       0,     0,     0,     0,     0,     0,   726,     8,  1049,   727,
     728,   729,     0,     0,     9,     0,     0,     0,     0,     0,
       0,     0,     0,   925,     0,   925,    14,     0,     0,   925,
       0,     0,    16,   730,   731,   732,     0,     0,     0,   750,
       0,     0,     0,    18,  1081,     0,    20,   750,    22,     0,
    1087,  1088,  1089,  1090,     0,     0,     0,   975,   975,     0,
       0,     0,     0,     0,    25,     0,    27,     0,    28,     0,
      30,     0,     0,     0,     0,  1100,    35,     0,     0,     0,
       0,     0,  1104,  1105,  1106,     0,  1107,   853,   853,   853,
     486,  1112,     0,  1113,     0,     0,     0,   628,     5,  1118,
       0,   867,     0,     0,     0,  1024,  1024,     0,   726,     0,
       0,   727,   728,   729,     0,     0,     0,     0,     0,     0,
     757,     0,     0,     0,     0,   758,     0,     0,     0,     0,
     925,     0,     0,     0,     0,   730,   731,   732,   134,   229,
     230,  1157,   231,   232,   233,   234,   235,  1165,     0,  1168,
       0,   628,  1172,     0,     0,     0,  1175,  1176,     0,     0,
       0,  1179,  1180,     0,  1181,     0,     0,   853,     0,  1183,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   304,  1188,     0,   311,   750,   316,
       0,   750,   328,     0,     0,   750,     0,     0,     0,   750,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1230,     0,  1231,     0,  1168,   628,
    1234,     0,  1172,  1236,     0,     0,     0,  1238,  1240,     0,
    1243,     0,     0,     0,     0,     0,  1245,  1247,     0,     0,
       0,     0,     0,   750,     0,     0,     0,     0,     0,     0,
       0,   750,     0,   750,     0,     0,     0,     0,     0,     0,
     750,  1318,  1245,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1320,  1322,  1323,  1324,  1325,     0,     0,  1327,  1328,  1329,
    1330,  1331,     0,     0,  1333,     0,  1240,     0,  1240,     0,
     867,   427,     0,     0,     0,   628,     0,     0,   442,     0,
     448,   449,     0,  1345,     0,  1357,  1357,  1365,  1365,     0,
    1372,  1377,  1357,  1382,  1382,     0,  1387,   482,     0,     0,
       0,     0,   490,     0,     0,     0,     0,  1395,     0,  1397,
       0,     0,  1400,     0,     0,     0,     0,     0,     0,     0,
       0,  1407,     0,  1410,     0,     0,     0,     0,     0,  1414,
       0,  1417,     0,     0,  1395,     0,  1421,     0,     0,     0,
    1424,     0,     0,     0,     0,  1428,     0,     0,     0,     0,
    1395,     0,  1253,  1254,  1255,  1256,  1257,  1258,  1259,     0,
    1260,  1261,  1262,  1263,  1264,  1265,  1266,  1267,  1268,  1269,
    1270,  1271,  1272,  1273,  1274,  1275,  1276,  1277,  1278,  1279,
    1280,  1281,  1282,  1283,     0,   560,  1284,  1285,  1286,  1287,
    1288,     0,     0,     0,  1247,   568,  1454,     0,  1395,     0,
    1459,     0,   576,     0,  1464,  1466,     0,     0,  1469,     0,
       0,  1395,     0,     0,     0,     0,   397,   587,     0,     0,
       0,     0,     0,     0,   593,     0,     0,   596,   597,     0,
     405,   406,   601,   409,     0,   604,     0,  1483,     0,     0,
       0,     0,   613,     0,     0,   615,     0,     0,     0,     0,
     621,     0,   622,     0,     0,     0,     0,     0,     0,     0,
       0,   631,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   645,     0,     0,     0,   652,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   689,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   701,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     754,     0,     0,     0,     0,     0,   766,     0,     0,     0,
     772,     0,     0,     0,     0,     0,   775,     0,   776,     0,
       0,     0,   778,     0,     0,     0,     0,     0,   782,     0,
       0,     0,     0,     0,     0,   785,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   574,     0,     0,     0,
       0,     0,     0,     0,   580,     0,     0,     0,   795,   583,
       0,   797,   798,   799,   800,     0,   590,   591,     0,     0,
       0,   594,     0,     0,     0,   806,     0,   600,     0,     0,
       0,     0,     0,     0,   814,     0,   816,     0,     0,   614,
       0,   820,     0,   822,     0,   824,     0,     0,     0,     0,
       0,   837,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     135,   137,   139,   139,   142,     0,     0,   148,   139,   151,
     139,   148,   139,   148,   139,   148,   139,   148,   161,   163,
       0,   185,   185,   185,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   888,     0,   890,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     900,     0,   902,     0,     0,     0,     0,     0,     0,   908,
       0,     0,   909,     0,   910,     0,   911,     0,     0,     0,
       0,     0,     0,   245,     0,     0,     0,     0,   273,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   953,     0,     0,     0,     0,     0,     0,     0,   958,
       0,     0,     0,   801,   802,     0,     0,     0,     0,   804,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   818,   819,     0,     0,   821,     0,
     823,     0,   825,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   999,
       0,     0,     0,     0,  1005,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1013,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1022,     0,     0,
       0,     0,     0,     0,  1023,     0,     0,  1028,  1029,  1030,
    1031,  1032,  1033,     0,     0,  1034,     0,  1038,     0,     0,
    1042,     0,     0,     0,     0,  1044,     0,  1053,  1054,  1055,
       0,     0,     0,     0,     0,     0,  1059,   436,     0,     0,
       0,     0,   450,   137,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   491,   137,     0,  1086,     0,     0,     0,
       0,     0,   502,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   520,     0,
       0,   523,     0,     0,     0,     0,     0,     0,   530,     0,
       0,  1103,     0,     0,     0,     0,     0,     0,   967,   968,
     969,   970,     0,   972,   973,     0,  1115,     0,  1117,     0,
       0,     0,     0,   980,     0,   982,   983,     0,     0,     0,
       0,     0,     0,   987,   988,  1133,  1134,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   995,     0,     0,     0,
       0,     0,     0,     0,     0,  1150,     0,     0,     0,     0,
    1161,  1162,     0,     0,     0,     0,     0,     1,     2,     0,
       0,     0,     0,     3,     0,     0,     0,     0,     4,     0,
       5,     6,     0,     0,     0,     0,     0,     0,     7,     0,
       0,     8,     0,     0,     0,     0,     0,     0,     9,    10,
       0,    11,     0,    12,     0,     0,     0,     0,    13,     0,
      14,     0,     0,    15,     0,     0,    16,     0,     0,     0,
       0,     0,     0,     0,    17,   634,   636,    18,  1226,    19,
      20,    21,    22,     0,     0,     0,     0,     0,     0,     0,
      23,    24,     0,     0,     0,     0,   678,     0,    25,    26,
      27,     0,    28,    29,    30,    31,    32,    33,     0,    34,
      35,    36,  1091,   699,     0,  1244,     0,  1094,  1095,     0,
    1096,     0,     0,     0,     0,     0,  1097,     0,  1304,  1099,
       0,     0,     0,     0,     0,     0,     0,  1312,     0,  1316,
    1317,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   767,     0,     0,
       0,   185,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1326,     0,     0,     0,     0,
       0,     0,  1332,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1341,     0,  1355,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1391,     0,     0,
       0,     0,     0,     0,  1184,  1185,  1290,  1253,  1254,  1255,
    1256,  1257,  1258,  1259,  1291,  1260,  1261,  1262,  1263,  1264,
    1265,  1266,  1267,  1268,  1269,  1270,  1271,  1272,  1273,  1274,
    1275,  1276,  1277,  1278,  1279,  1280,  1281,  1282,  1283,  1292,
    1293,  1284,  1285,  1286,  1287,  1288,     0,     0,     0,     0,
       0,     0,     0,  1433,  1434,     0,     0,     0,     0,     0,
    1237,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1249,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   875,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1458,
       0,  1461,  1462,     0,   892,     0,   892,     0,     0,  1471,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1488,     0,   927,     0,     0,   932,     0,     0,
       0,     0,     0,     0,  1495,     0,   942,     0,  1337,     0,
       0,     0,   699,     0,     0,   699,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1393,     0,     0,
       0,     0,  1394,     0,     0,     0,     0,     0,  1401,     0,
    1403,     0,     0,     0,  1405,  1406,     0,  1408,     0,  1411,
       0,     0,  1412,  1413,     0,  1415,     0,  1418,     0,  1419,
       0,     0,     0,     0,  1423,     0,  1425,  1426,     0,  1427,
       0,  1429,  1430,     0,  1431,  1297,  1253,  1254,  1255,  1256,
    1257,  1258,  1259,  1298,  1260,  1261,  1262,  1263,  1264,  1265,
    1266,  1267,  1268,  1269,  1270,  1271,  1272,  1273,  1274,  1275,
    1276,  1277,  1278,  1279,  1280,  1281,  1282,  1283,  1299,  1300,
    1284,  1285,  1286,  1287,  1288,     0,     0,     0,     0,     0,
       0,     0,  1455,  1456,     0,     0,  1460,     0,     0,  1463,
    1465,     0,  1467,  1468,  1470,     0,  1472,     0,  1474,  1475,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1074,     0,     0,     0,     0,     0,     0,
       0,     0,  1079,  1484,     0,  1485,     0,  1487,     0,  1489,
       0,     0,     0,     0,     0,     0,     0,  1494,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   892,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   699,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1204,     0,
       0,  1204,     0,     0,     0,  1204,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   892,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   892,
     892,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1308
};

static const yytype_int16 yycheck[] =
{
      45,   573,     0,   196,   201,   228,   243,    52,    53,   764,
      55,   131,   132,    58,   444,    60,   352,    62,   115,     0,
     192,    66,   380,    68,   358,    70,   876,    72,   444,    74,
     209,    76,   805,    78,   221,    80,   207,   224,   213,   375,
     618,   204,   543,    88,   421,   859,   624,   195,   844,   916,
      95,   244,    97,   201,    99,   385,   101,   978,   103,   612,
     105,   413,     0,   676,   698,   133,   861,   719,   844,   864,
     855,   847,   839,     0,   841,     4,  1163,   844,   859,  1020,
     847,   716,  1081,   850,     4,     6,   853,   844,    14,     6,
     847,     4,     6,   850,  1348,    16,  1100,   859,  1320,   371,
      17,    17,   147,    66,   195,  1087,    19,  1089,  1090,    72,
     169,    17,    14,     3,    50,    17,    29,    17,  1320,  1373,
      22,  1375,  1324,  1325,    17,  1327,  1328,    26,  1330,  1331,
      17,  1333,    36,    96,    27,  1389,    26,    57,    74,     0,
      27,    61,   236,   742,    57,   744,   191,   746,    61,   748,
       5,   750,    52,   221,    80,    81,   224,    86,    94,   153,
      15,    90,     5,    56,  1168,    81,    86,    88,    55,    95,
      90,    88,    15,    86,    88,  1397,    82,    90,    80,    81,
      79,   374,    17,  1437,   356,    50,   169,   384,    92,    93,
    1172,  1058,    17,    95,   975,   950,  1450,   952,     9,    24,
       5,   997,  1016,   426,   249,  1407,    17,    62,    63,   254,
      15,   256,  1414,   975,   259,   260,    51,    52,    17,    62,
     554,   997,  1424,    22,  1000,    41,  1428,  1012,    39,    94,
    1080,    41,    36,  1000,  1101,  1002,   281,   282,   413,  1006,
     997,   286,    62,  1000,   289,  1002,   623,  1188,  1247,  1006,
     584,   296,  1339,    57,  1175,   585,   301,     9,   303,   353,
     198,   895,  1024,   308,   916,    17,   618,   901,   505,    79,
     315,  1138,   885,   774,   201,   320,   889,   830,   913,    17,
     325,   859,   327,   555,    22,   169,   331,   480,    92,    93,
    1168,   528,   161,   338,   163,   567,   341,    17,  1176,   393,
      17,   169,   362,    23,   349,     6,     7,  1395,  1104,  1397,
      27,    12,  1400,   407,    31,    32,    17,   787,   788,   789,
     414,  1076,   921,   383,  1245,  1098,   925,  1122,  1104,  1105,
     929,   930,   931,  1421,   428,   934,   935,    54,    55,   938,
    1107,   639,    80,    81,   642,   815,    25,  1104,  1105,  1106,
    1107,   419,   394,    36,   422,    66,    68,   399,     7,    65,
     402,    40,    11,    12,    13,    14,  1454,  1395,    17,  1397,
      49,    33,    34,     7,    57,  1130,  1170,    11,    61,   377,
    1174,    19,   363,   719,   169,    96,    98,    88,    71,   370,
      68,    29,   589,  1421,   439,    73,   441,  1318,    17,    78,
       7,   446,    17,  1179,    11,    12,    13,    42,    27,    92,
      93,   456,    27,  1186,   562,   563,   653,    32,   463,   796,
      98,    70,  1179,  1180,  1181,   156,   471,   606,    31,    32,
     578,   579,   603,  1247,   156,   161,   599,   163,     7,    54,
      55,   589,    11,   618,   489,   155,  1024,   781,    17,     7,
     495,    54,    55,  1208,    12,  1228,  1229,    58,   874,  1309,
      32,  1216,    49,    70,   558,    25,  1065,  1066,  1067,  1068,
      17,   562,   563,    80,    81,   905,   812,    26,    17,   813,
     165,   166,    54,    55,    23,    24,  1243,   578,   579,    16,
     535,   153,   154,   155,   539,   157,   158,   159,   160,   161,
      48,   595,     7,     0,    17,    33,    11,    12,    13,    34,
     630,     7,    17,    63,    27,   609,    12,    22,   612,   561,
      46,    17,   564,  1234,   566,  1236,   620,   569,   570,  1322,
    1323,  1324,  1325,     7,  1327,   577,  1329,    11,    12,    13,
     161,    54,    55,    17,     7,     8,   588,  1146,    11,    12,
      13,    14,    75,    76,    17,  1323,   598,   162,    57,  1314,
     602,  1329,    30,   605,    27,    70,  1327,    30,    31,    32,
     167,   168,  1333,     7,   616,    80,    81,    11,    12,    13,
     916,   163,  1305,    17,  1307,   153,    20,  1165,    22,   367,
    1168,    54,    55,    56,  1172,     8,    70,   162,  1176,   418,
     378,   379,   167,   168,   169,   424,   651,    70,   154,   387,
      55,   157,   158,   159,  1407,   160,   161,  1410,   396,   375,
     817,  1414,   112,   401,  1417,   719,   404,   117,  1421,   119,
     717,   121,    95,   123,   412,   125,    70,   136,  1238,   138,
    1240,   140,   141,   688,   143,   157,   158,   159,   693,   352,
     149,   150,  1230,   152,   153,   154,   155,   156,   157,   158,
     159,   160,  1240,   162,   917,   164,  1024,    17,   914,  1247,
      20,  1464,    22,   839,   368,   369,  1469,   849,   372,   373,
     852,    59,    60,   870,  1020,   184,   840,   186,   187,   188,
     784,   190,   386,   738,   791,   740,    62,    63,   743,   841,
     745,   395,   747,   997,   749,   202,   400,   752,   205,   403,
     120,   808,   122,   210,   124,   760,   126,   411,   763,   542,
     765,  1113,  1058,  1251,     7,   770,  1207,    17,    11,    12,
      13,    21,    22,   827,    17,  1211,   830,   831,    21,    22,
       7,    54,    55,  1089,    11,    12,    13,   246,   586,   248,
      17,    80,    81,    83,    84,  1172,   255,  1325,   257,  1330,
    1331,   855,   261,   262,  1331,  1101,  1036,  1037,   267,   268,
     269,   270,  1328,   272,  1483,   274,   275,   276,   154,   155,
       0,   157,   158,   159,   977,   978,   285,    70,   948,   949,
     201,   290,   291,   644,   293,  1018,   761,   762,  1084,  1085,
     551,   300,  1138,   302,   866,   129,   129,   306,   307,   790,
     129,   310,   793,   129,   313,   314,   275,   205,   129,   318,
     319,  1058,   916,   917,   390,   199,   807,   326,   199,   810,
     981,   330,   488,   129,   333,   129,   585,  1009,   129,  1011,
     838,   340,   129,   342,   343,   344,   345,   346,   158,   348,
     129,   408,  1188,   153,   154,   129,     7,   157,   158,   159,
      11,    12,    13,    14,   195,   129,    17,     7,   195,   617,
     617,    11,    12,    13,    14,   617,    27,    17,   617,    30,
      31,    32,   129,   380,   129,   129,    37,    27,   129,   129,
      30,    31,    32,   390,  1466,   392,   753,   991,     0,   340,
     343,   398,     0,    54,    55,    56,   951,     7,   345,  1433,
       0,    11,    12,    13,    54,    55,    56,    17,  1012,    70,
      -1,    17,    22,    -1,   965,    -1,    -1,    -1,    -1,    -1,
      70,    27,    -1,    -1,    30,    31,    32,    -1,   437,   438,
    1112,    37,    -1,    -1,    95,   444,   445,    -1,   447,    -1,
      -1,    -1,   451,   452,   453,    95,   455,    -1,    54,    55,
      56,    -1,    -1,    -1,  1058,    -1,   465,    -1,   467,   468,
      70,   470,    -1,   472,   473,    -1,   475,   476,    -1,   478,
     479,    -1,  1175,    -1,   483,   484,   485,    -1,    -1,   488,
      -1,    -1,    -1,    -1,   493,   494,    -1,    -1,   497,   498,
      -1,    -1,   501,    -1,    10,    -1,  1051,  1101,    -1,   508,
     509,  1183,    18,   512,   513,    -1,   515,   516,    -1,    -1,
      -1,    -1,    28,    -1,    -1,    -1,   525,   526,   527,    35,
      -1,    -1,    38,   532,    -1,   534,    -1,   536,  1083,   538,
      -1,    -1,    -1,    -1,  1138,    -1,    -1,    53,    -1,    -1,
      -1,   548,  1245,    -1,    -1,   552,    -1,    -1,    64,    -1,
      -1,    67,   154,    69,    -1,   157,   158,   159,   160,   161,
     162,    -1,   164,    -1,   571,    -1,  1121,    -1,    -1,    85,
      -1,    87,  1127,    89,  1129,    91,    -1,  1132,     8,    -1,
      -1,    97,    -1,    -1,    -1,   592,    -1,    17,   154,   155,
      -1,   157,   158,   159,   160,   161,    -1,    27,   164,    -1,
      30,    31,    32,   154,    -1,  1160,   157,   158,   159,   160,
     161,   618,    -1,   164,    -1,  1318,    -1,   624,    -1,    -1,
      -1,   628,    -1,   632,    54,    55,    56,    -1,     9,    -1,
     639,    -1,    -1,   642,    -1,   644,    17,   646,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1200,    27,  1202,    -1,    30,
      31,    32,    -1,    -1,    -1,   664,    -1,    -1,    39,    -1,
     669,   670,   671,  1218,   673,   674,   675,   676,    -1,    17,
     679,    -1,    -1,    54,    55,    56,    -1,    -1,    -1,    27,
      -1,    -1,    30,    31,    32,    -1,   695,   696,   697,   698,
      -1,   700,    -1,   702,    -1,   704,    44,   706,    46,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    55,    56,    -1,
      -1,    10,    -1,    -1,    -1,    -1,   725,    -1,    -1,    18,
    1402,    -1,  1404,    -1,    -1,    -1,    -1,  1409,   737,    28,
     739,    -1,    -1,   742,  1416,   744,    35,   746,    -1,   748,
      -1,   750,    -1,    -1,   753,    -1,    -1,   756,    47,    -1,
      -1,    -1,   761,   762,    53,   764,    -1,    -1,    -1,   768,
    1315,    -1,    -1,    -1,   773,    64,    17,    -1,    67,    -1,
      69,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    30,
      31,    32,    -1,    -1,   791,    -1,    85,    -1,    87,    -1,
      89,    -1,    91,    44,  1349,  1399,    -1,  1352,    97,  1354,
      -1,   808,    -1,    54,    55,    56,  1361,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1369,    -1,    17,    99,   100,   101,
     102,   103,   104,   105,   106,   107,    27,     9,    -1,    30,
      31,    32,   839,    -1,   841,    17,    -1,   844,    -1,    -1,
     847,    -1,    -1,   850,    -1,    27,   853,    -1,    30,    31,
      32,    -1,   859,    54,    55,    56,    -1,    39,    -1,    -1,
     867,    43,    44,    -1,    -1,   874,    -1,   876,    -1,    -1,
      -1,    -1,    54,    55,    56,    -1,   885,    -1,    -1,    -1,
     889,    -1,   891,    -1,   893,   894,   895,    -1,    -1,   898,
      -1,    -1,   901,    -1,    -1,   904,   905,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,    -1,   921,    -1,   923,    -1,   925,    -1,    -1,    -1,
     929,   930,   931,    -1,  1479,   934,   935,    -1,   937,   938,
      -1,   940,    -1,    -1,   943,    -1,    -1,    -1,    -1,   948,
     949,   950,    -1,   952,    -1,   954,   955,   154,   155,    -1,
     157,   158,   159,   160,   161,   964,   965,    -1,    -1,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    17,    18,   975,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    28,   985,    30,
      31,    32,    -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1000,    -1,  1002,    47,    -1,    -1,  1006,
      -1,    -1,    53,    54,    55,    56,    -1,    -1,    -1,  1016,
      -1,    -1,    -1,    64,  1023,    -1,    67,  1024,    69,    -1,
    1029,  1030,  1031,  1032,    -1,    -1,    -1,  1036,  1037,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    87,    -1,    89,    -1,
      91,    -1,    -1,    -1,    -1,  1054,    97,    -1,    -1,    -1,
      -1,    -1,  1061,  1062,  1063,    -1,  1065,  1066,  1067,  1068,
       9,  1070,    -1,  1072,    -1,    -1,    -1,  1076,    17,  1078,
      -1,  1080,    -1,    -1,    -1,  1084,  1085,    -1,    27,    -1,
      -1,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    44,    -1,    -1,    -1,    -1,
    1107,    -1,    -1,    -1,    -1,    54,    55,    56,   153,   154,
     155,  1118,   157,   158,   159,   160,   161,  1126,    -1,  1128,
      -1,  1130,  1131,    -1,    -1,    -1,  1135,  1136,    -1,    -1,
      -1,  1140,  1141,    -1,  1143,    -1,    -1,  1146,    -1,  1148,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   203,  1164,    -1,   206,  1165,   208,
      -1,  1168,   211,    -1,    -1,  1172,    -1,    -1,    -1,  1176,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1203,    -1,  1205,    -1,  1207,  1208,
    1209,    -1,  1211,  1212,    -1,    -1,    -1,  1216,  1217,    -1,
    1219,    -1,    -1,    -1,    -1,    -1,  1225,  1226,    -1,    -1,
      -1,    -1,    -1,  1230,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1238,    -1,  1240,    -1,    -1,    -1,    -1,    -1,    -1,
    1247,  1250,  1251,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1289,  1290,  1291,  1292,  1293,    -1,    -1,  1296,  1297,  1298,
    1299,  1300,    -1,    -1,  1303,    -1,  1305,    -1,  1307,    -1,
    1309,   350,    -1,    -1,    -1,  1314,    -1,    -1,   357,    -1,
     359,   360,    -1,  1320,    -1,  1322,  1323,  1324,  1325,    -1,
    1327,  1328,  1329,  1330,  1331,    -1,  1333,   376,    -1,    -1,
      -1,    -1,   381,    -1,    -1,    -1,    -1,  1346,    -1,  1348,
      -1,    -1,  1351,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1360,    -1,  1362,    -1,    -1,    -1,    -1,    -1,  1368,
      -1,  1370,    -1,    -1,  1373,    -1,  1375,    -1,    -1,    -1,
    1379,    -1,    -1,    -1,    -1,  1384,    -1,    -1,    -1,    -1,
    1389,    -1,   114,   115,   116,   117,   118,   119,   120,    -1,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,    -1,   464,   148,   149,   150,   151,
     152,    -1,    -1,    -1,  1433,   474,  1435,    -1,  1437,    -1,
    1439,    -1,   481,    -1,  1443,  1444,    -1,    -1,  1447,    -1,
      -1,  1450,    -1,    -1,    -1,    -1,   309,   496,    -1,    -1,
      -1,    -1,    -1,    -1,   503,    -1,    -1,   506,   507,    -1,
     323,   324,   511,   326,    -1,   514,    -1,  1476,    -1,    -1,
      -1,    -1,   521,    -1,    -1,   524,    -1,    -1,    -1,    -1,
     529,    -1,   531,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   540,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   553,    -1,    -1,    -1,   557,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   572,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   583,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     619,    -1,    -1,    -1,    -1,    -1,   625,    -1,    -1,    -1,
     629,    -1,    -1,    -1,    -1,    -1,   635,    -1,   637,    -1,
      -1,    -1,   641,    -1,    -1,    -1,    -1,    -1,   647,    -1,
      -1,    -1,    -1,    -1,    -1,   654,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   479,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   487,    -1,    -1,    -1,   677,   492,
      -1,   680,   681,   682,   683,    -1,   499,   500,    -1,    -1,
      -1,   504,    -1,    -1,    -1,   694,    -1,   510,    -1,    -1,
      -1,    -1,    -1,    -1,   703,    -1,   705,    -1,    -1,   522,
      -1,   710,    -1,   712,    -1,   714,    -1,    -1,    -1,    -1,
      -1,   720,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     109,   110,   111,   112,   113,    -1,    -1,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
      -1,   130,   131,   132,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   792,    -1,   794,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     809,    -1,   811,    -1,    -1,    -1,    -1,    -1,    -1,   818,
      -1,    -1,   821,    -1,   823,    -1,   825,    -1,    -1,    -1,
      -1,    -1,    -1,   192,    -1,    -1,    -1,    -1,   197,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   860,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   868,
      -1,    -1,    -1,   686,   687,    -1,    -1,    -1,    -1,   692,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   707,   708,    -1,    -1,   711,    -1,
     713,    -1,   715,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   928,
      -1,    -1,    -1,    -1,   933,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   945,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   956,    -1,    -1,
      -1,    -1,    -1,    -1,   963,    -1,    -1,   966,   967,   968,
     969,   970,   971,    -1,    -1,   974,    -1,   976,    -1,    -1,
     979,    -1,    -1,    -1,    -1,   984,    -1,   986,   987,   988,
      -1,    -1,    -1,    -1,    -1,    -1,   995,   356,    -1,    -1,
      -1,    -1,   361,   362,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   382,   383,    -1,  1025,    -1,    -1,    -1,
      -1,    -1,   391,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   407,    -1,
      -1,   410,    -1,    -1,    -1,    -1,    -1,    -1,   417,    -1,
      -1,  1060,    -1,    -1,    -1,    -1,    -1,    -1,   881,   882,
     883,   884,    -1,   886,   887,    -1,  1075,    -1,  1077,    -1,
      -1,    -1,    -1,   896,    -1,   898,   899,    -1,    -1,    -1,
      -1,    -1,    -1,   906,   907,  1094,  1095,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   919,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1114,    -1,    -1,    -1,    -1,
    1119,  1120,    -1,    -1,    -1,    -1,    -1,     4,     5,    -1,
      -1,    -1,    -1,    10,    -1,    -1,    -1,    -1,    15,    -1,
      17,    18,    -1,    -1,    -1,    -1,    -1,    -1,    25,    -1,
      -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,
      -1,    38,    -1,    40,    -1,    -1,    -1,    -1,    45,    -1,
      47,    -1,    -1,    50,    -1,    -1,    53,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    61,   544,   545,    64,  1187,    66,
      67,    68,    69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      77,    78,    -1,    -1,    -1,    -1,   565,    -1,    85,    86,
      87,    -1,    89,    90,    91,    92,    93,    94,    -1,    96,
      97,    98,  1035,   582,    -1,  1224,    -1,  1040,  1041,    -1,
    1043,    -1,    -1,    -1,    -1,    -1,  1049,    -1,  1237,  1052,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1246,    -1,  1248,
    1249,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   626,    -1,    -1,
      -1,   630,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1294,    -1,    -1,    -1,    -1,
      -1,    -1,  1301,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1319,    -1,  1321,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1336,    -1,    -1,
      -1,    -1,    -1,    -1,  1157,  1158,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1392,  1393,    -1,    -1,    -1,    -1,    -1,
    1213,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1227,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   783,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1438,
      -1,  1440,  1441,    -1,   803,    -1,   805,    -1,    -1,  1448,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1481,    -1,   843,    -1,    -1,   846,    -1,    -1,
      -1,    -1,    -1,    -1,  1493,    -1,   855,    -1,  1311,    -1,
      -1,    -1,   861,    -1,    -1,   864,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1340,    -1,    -1,
      -1,    -1,  1345,    -1,    -1,    -1,    -1,    -1,  1351,    -1,
    1353,    -1,    -1,    -1,  1357,  1358,    -1,  1360,    -1,  1362,
      -1,    -1,  1365,  1366,    -1,  1368,    -1,  1370,    -1,  1372,
      -1,    -1,    -1,    -1,  1377,    -1,  1379,  1380,    -1,  1382,
      -1,  1384,  1385,    -1,  1387,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1435,  1436,    -1,    -1,  1439,    -1,    -1,  1442,
    1443,    -1,  1445,  1446,  1447,    -1,  1449,    -1,  1451,  1452,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1012,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1021,  1476,    -1,  1478,    -1,  1480,    -1,  1482,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1490,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1098,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1122,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1167,    -1,
      -1,  1170,    -1,    -1,    -1,  1174,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1186,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1228,
    1229,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1242
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
     431,   453,   454,   455,   457,   459,   460,   466,   467,   472,
     477,   480,   483,   486,   487,   490,   495,   497,   500,   504,
     517,   521,   522,   527,   529,   530,     0,   165,   166,   208,
     208,   208,   208,   208,   169,   210,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   153,   211,   247,   211,   328,   211,
     355,   355,   211,   362,     6,    88,   365,   366,   211,   372,
     355,   211,   395,   355,   372,   355,   372,   355,   372,   355,
     372,   211,   468,   211,   491,   350,   351,   352,   353,   380,
     406,   407,   418,   423,   427,   453,   466,   472,   477,   480,
     483,   486,   496,   498,   499,   211,   505,   505,   505,    41,
     509,   510,   210,   210,   210,   210,   210,   208,   210,   210,
     210,   210,   210,   373,   210,   210,   422,   210,   422,   210,
     210,   430,   210,   210,   167,   168,   209,   210,   210,   501,
     502,   210,   518,   519,   210,   523,   524,   210,   208,   154,
     155,   157,   158,   159,   160,   161,   174,   175,   176,   178,
     180,   181,   184,   186,   187,   211,   248,    62,   329,   331,
      19,    29,    57,   322,   327,   338,   339,   358,   370,   388,
     389,   456,   458,   461,   463,   464,   465,   338,   358,   456,
     458,   178,   363,   211,   367,   359,   382,   383,   385,   386,
     387,   388,   389,     3,    26,   396,   397,    71,   325,   327,
     338,   408,   416,   473,   474,   475,   476,    23,    24,   244,
     374,   375,   377,   378,   209,    72,   420,   478,   479,   244,
     374,   209,    73,   425,   481,   482,   209,    74,   429,   484,
     485,    51,    52,   244,   432,   433,   435,   436,   209,    65,
     469,   470,   487,   488,   529,    75,    76,   492,   493,    79,
     506,   507,   509,   506,   509,   506,   509,    42,   511,   512,
     173,   186,   156,   174,   185,   178,   210,   245,   210,   323,
     326,   208,   208,   210,   208,   210,   208,   208,   210,   210,
     210,   210,   210,   210,   210,   210,   373,   210,   208,   208,
     210,   393,   208,   208,   210,   210,   210,   208,   210,   265,
     210,   208,   210,   208,   210,   210,   208,   265,   210,   210,
     210,   208,   210,   210,   208,   265,   265,   208,   210,   265,
     208,   210,   208,   210,   208,   210,   502,   208,   210,   210,
     519,   210,   210,   524,   210,   210,   208,   209,   179,   182,
     183,   187,   186,    33,    34,   175,   211,   249,   250,   251,
     253,   254,   209,    63,   329,   334,   335,   359,   209,   209,
     211,   332,   328,   370,    58,   340,   341,    16,   274,   275,
     277,   279,   280,   282,   354,   365,   211,   390,   390,    49,
     438,   440,   438,   370,   354,   438,   438,   178,   364,   177,
     179,   368,   209,   392,   390,   390,     9,   244,   399,   401,
     209,   211,   398,   328,   416,   282,   409,   438,   390,   244,
     244,   377,   211,   376,   244,   186,   379,   274,   438,   390,
     244,   274,   438,   390,   274,   438,   390,   186,   190,   192,
     211,   434,   432,   211,   437,   438,   390,   488,   186,   494,
     211,   508,   511,    26,   442,   443,   511,    48,   531,   535,
     173,   186,   210,   210,   208,   208,   323,   326,   210,   324,
     208,   210,   210,   330,   210,   210,   210,   343,   208,   208,
     209,   210,   210,   210,   210,   208,   210,   210,   209,   210,
     210,   210,   360,   210,   265,   178,   209,   210,   210,   210,
     265,   393,   208,   265,   210,   210,   208,   209,   210,   210,
     265,   265,   210,   209,   265,   174,   209,   209,   210,   210,
     265,   209,   210,   210,   209,   210,   210,   161,   163,   189,
     191,   195,   198,   209,   265,   209,   210,   210,   210,   489,
     174,   209,   209,   210,   210,   520,   208,   210,   210,   213,
     208,   209,   250,   253,   211,   252,   211,   255,   244,   337,
     175,   336,   334,   244,   333,   209,   329,   354,    59,    60,
     344,   346,   209,   186,   342,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   283,
     284,   285,   274,   338,   358,   338,   358,   274,   211,   441,
     274,   354,   274,   274,     7,    11,   244,   266,   270,   209,
       7,    12,   264,   269,   274,   338,   358,   338,   358,   211,
     402,   209,   329,   409,   283,   274,   338,   416,   244,   186,
     274,   420,   274,   425,   274,   429,   186,   193,   186,   199,
     274,   461,   463,   464,   465,   471,    27,    30,    31,    32,
      54,    55,    56,   214,   216,   217,   218,   219,   220,   221,
     222,   225,   226,   227,   229,   230,   235,   237,   240,   241,
     244,   256,   257,   488,   209,   186,   442,    39,    44,   214,
     401,   445,   450,   451,   515,   516,   209,   211,   444,    46,
     528,   214,   209,   505,   210,   209,   209,   324,   209,   324,
     330,   210,   209,   208,   174,   209,   210,   210,   210,   210,
     210,   210,   462,   210,   462,   209,   210,   209,   209,   209,
     209,   265,   265,   208,   265,   208,   209,   210,   210,   384,
     210,   384,   210,   210,   209,   210,   209,   210,   265,   265,
     209,   265,   209,   265,   209,   265,   191,   189,   162,   164,
     187,   188,   196,   199,   204,   205,   206,   209,   210,   210,
     208,   210,   215,   208,   210,   215,   208,   210,   215,   208,
     210,   215,   208,   210,   215,   208,   215,   208,   489,   210,
     503,   208,   520,   520,   208,   213,   208,   210,   345,   208,
     210,   532,   533,   253,   329,   211,   347,   186,   157,   158,
     159,   286,   286,   286,   286,   370,   244,   365,   209,   370,
     209,   442,   211,   272,   272,   370,   244,   264,   365,   391,
     209,   370,   209,   177,   403,   329,   286,   416,   209,   209,
     209,   209,   186,   194,   199,   186,   200,   188,   207,   392,
     218,   244,   256,   221,   225,   244,   256,   211,   223,   229,
     235,   240,   211,   228,   235,   240,   175,   231,   240,   175,
     238,   190,   211,   242,   153,   212,    43,   214,   445,   450,
     513,   514,   515,   209,   402,   402,   336,   244,   209,   392,
     438,   500,   521,   525,   442,   506,   345,   265,   265,   265,
     265,   462,   265,   265,   462,   210,   439,   210,   210,   384,
     265,   210,   265,   265,   384,   210,   400,   265,   265,   191,
     207,   188,   197,   204,   200,   265,   215,   210,   215,   209,
     210,   215,   210,   215,   215,   209,   210,   215,   215,   210,
     215,   210,   210,   209,   503,   503,   210,   213,   208,   213,
     210,   210,   209,   209,   210,   534,   210,   533,   209,   209,
     209,   209,   209,   209,   209,   244,   445,   450,   209,   178,
     273,   273,   209,   391,   209,     7,    11,    12,    13,   244,
     262,   263,   404,   209,   209,   209,   186,   201,   202,   209,
     224,   226,   229,   235,   240,   235,   240,   240,   240,   175,
     232,   175,   239,   190,   211,   243,   515,   173,   403,   211,
     452,   210,   214,   401,   450,   536,   209,   210,   210,   210,
     210,   265,   439,   439,   265,   265,   265,   265,   208,   265,
     210,   174,   204,   209,   210,   210,   210,   210,   215,   215,
     215,   215,   210,   210,   236,   209,   213,   209,   210,   446,
     345,   528,   208,   534,   534,     8,   288,   290,   287,   290,
     288,   289,   290,   209,   209,   272,   287,   186,   203,   204,
     229,   235,   240,   235,   240,   240,   240,   175,   233,   266,
     209,     7,    11,    12,    13,    14,    70,   244,   447,   448,
     449,   209,   209,   208,   402,   210,   278,   208,   210,   276,
     208,   213,   210,   281,   208,   210,   210,   410,   204,   210,
     210,   210,   215,   210,   265,   265,   208,   525,   210,     7,
      11,    12,    13,    14,    70,    95,   214,   258,   259,   260,
     261,   267,   271,   320,   211,   291,   214,   287,   320,   291,
     214,   289,   291,   273,    37,   214,   320,   412,   413,   235,
     240,   240,   240,   175,   234,   272,   209,   403,   208,   208,
     210,   210,   276,   213,   210,   281,   210,   265,   210,   213,
     210,   411,   208,   210,   209,   210,   268,   210,   526,   265,
     272,   272,   214,   114,   115,   116,   117,   118,   119,   120,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   148,   149,   150,   151,   152,   304,
     113,   121,   146,   147,   292,   295,   304,   113,   121,   146,
     147,   297,   300,   304,   209,   412,   214,   412,   211,   414,
     240,   273,   209,   214,   515,   528,   209,   209,   210,   268,
     210,   302,   210,   210,   210,   210,   209,   210,   210,   210,
     210,   210,   209,   210,   411,   411,   345,   265,   213,   208,
     273,   209,    22,    80,    81,   244,   267,   303,   309,   310,
     311,   313,   314,   315,   316,   209,    20,   244,   259,   293,
     305,   306,   309,   293,    21,   244,   259,   294,   307,   308,
     309,   294,   244,   259,   296,   309,   311,   244,   298,   305,
     309,   293,   244,   299,   307,   309,   299,   244,   301,   309,
     311,   209,   525,   265,   265,   210,   312,   210,   312,   208,
     210,   265,   208,   265,   208,   265,   265,   210,   265,   208,
     210,   265,   265,   265,   210,   265,   208,   210,   265,   265,
     312,   210,   312,   265,   210,   265,   265,   265,   210,   265,
     265,   265,   312,   209,   209,   313,   315,   267,   186,   315,
     175,   175,   259,   309,   175,   259,   259,   309,   175,   259,
     259,   309,   309,   526,   210,   265,   265,   312,   209,   210,
     265,   209,   209,   265,   210,   265,   210,   265,   265,   210,
     265,   209,   265,   312,   265,   265,   315,    82,   317,   318,
     259,   264,   259,   210,   265,   265,   208,   265,   209,   265,
     317,    83,    84,   319,   265,   209
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
     468,   469,   470,   471,   471,   471,   471,   472,   473,   473,
     474,   475,   476,   477,   478,   479,   480,   481,   482,   483,
     484,   485,   486,   487,   487,   487,   487,   487,   487,   487,
     487,   487,   487,   487,   487,   488,   488,   489,   489,   489,
     490,   491,   492,   493,   493,   494,   494,   494,   495,   496,
     496,   497,   498,   498,   498,   498,   498,   498,   498,   498,
     498,   498,   498,   498,   498,   498,   499,   499,   499,   499,
     499,   499,   499,   500,   501,   501,   502,   503,   503,   503,
     503,   503,   503,   503,   504,   505,   506,   507,   508,   509,
     510,   511,   512,   513,   514,   515,   516,   517,   518,   518,
     519,   520,   520,   520,   520,   520,   521,   522,   523,   523,
     524,   525,   525,   525,   525,   526,   526,   526,   526,   527,
     528,   529,   530,   531,   532,   532,   533,   534,   534,   534,
     534,   535,   536
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
