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
#define YYLAST   2990

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  164
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  354
/* YYNRULES -- Number of rules.  */
#define YYNRULES  699
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1432

#define YYUNDEFTOK  2
#define YYMAXUTOK   403


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
static const yytype_int16 yyrline[] =
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
    1070,  1075,  1076,  1077,  1079,  1081,  1083,  1085,  1089,  1089,
    1091,  1097,  1104,  1104,  1107,  1114,  1115,  1116,  1117,  1118,
    1120,  1121,  1122,  1124,  1128,  1130,  1132,  1132,  1136,  1141,
    1141,  1141,  1145,  1150,  1150,  1152,  1156,  1156,  1160,  1165,
    1167,  1171,  1171,  1175,  1180,  1182,  1186,  1187,  1188,  1189,
    1190,  1192,  1192,  1194,  1197,  1199,  1199,  1201,  1203,  1205,
    1209,  1215,  1216,  1217,  1218,  1220,  1222,  1226,  1231,  1233,
    1236,  1242,  1243,  1244,  1246,  1250,  1256,  1256,  1256,  1256,
    1256,  1256,  1260,  1265,  1267,  1272,  1272,  1273,  1275,  1275,
    1277,  1284,  1284,  1286,  1293,  1293,  1295,  1302,  1309,  1314,
    1315,  1316,  1318,  1324,  1329,  1337,  1343,  1345,  1347,  1353,
    1355,  1355,  1356,  1356,  1360,  1366,  1366,  1368,  1373,  1379,
    1384,  1390,  1395,  1400,  1406,  1411,  1416,  1422,  1427,  1432,
    1438,  1438,  1439,  1439,  1440,  1440,  1441,  1441,  1442,  1442,
    1443,  1443,  1446,  1446,  1448,  1449,  1450,  1452,  1454,  1458,
    1461,  1461,  1464,  1465,  1466,  1468,  1472,  1473,  1475,  1477,
    1477,  1478,  1478,  1479,  1479,  1479,  1480,  1481,  1481,  1482,
    1482,  1483,  1483,  1485,  1485,  1486,  1486,  1487,  1488,  1488,
    1492,  1496,  1497,  1500,  1505,  1506,  1507,  1508,  1509,  1510,
    1511,  1513,  1515,  1517,  1520,  1522,  1524,  1526,  1528,  1530,
    1532,  1534,  1536,  1538,  1543,  1547,  1548,  1551,  1556,  1557,
    1558,  1559,  1560,  1562,  1567,  1572,  1573,  1576,  1582,  1582,
    1582,  1582,  1584,  1585,  1586,  1587,  1589,  1591,  1596,  1602,
    1604,  1609,  1610,  1613,  1621,  1622,  1623,  1624,  1626,  1628
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
     395,   396,   397,   398,   399,   400,   401,   402,   403,    46,
      69,    49,    50,    51,    43,    45,    58,    84,    90,    91,
      40,    93,    41,    44
};
# endif

#define YYPACT_NINF (-1231)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-640)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    2659, -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231,
   -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231,
   -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231,
   -1231, -1231, -1231, -1231, -1231, -1231, -1231,   113, -1231, -1231,
   -1231,   263, -1231, -1231, -1231,   263, -1231, -1231, -1231, -1231,
   -1231, -1231,   263,   263, -1231,   263, -1231,    17,   263, -1231,
     263, -1231,   263, -1231, -1231, -1231,   263, -1231,   263, -1231,
     263, -1231,   263, -1231,   263, -1231,   263, -1231,   263, -1231,
     263, -1231, -1231, -1231, -1231, -1231, -1231, -1231,   263, -1231,
   -1231, -1231, -1231, -1231, -1231,   263, -1231,   263, -1231,   263,
   -1231,   263, -1231,   263, -1231,   263, -1231, -1231, -1231,   156,
     156,   156,   156,   156, -1231,    55,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,  1227,
     156,   156,   156,   175, -1231, -1231,    17, -1231,    17, -1231,
      17,    17, -1231,    17, -1231, -1231, -1231,   263, -1231,    17,
      17, -1231,    17,    17,    17,    17,    17,    17,    17,    17,
      17, -1231,    17, -1231,    17, -1231, -1231, -1231, -1231,    91,
   -1231, -1231, -1231, -1231, -1231,   158,   218,   228, -1231, -1231,
   -1231, -1231,   317, -1231,    17, -1231,    17,    17,    17, -1231,
      17,   263,   824,   352,   122,   122,   873,   156,   364,   173,
     283,   698,   305,   317,   108,   317,   201,   317,    69,   245,
     317,   369,  1589, -1231, -1231, -1231,   444,    66, -1231, -1231,
      66, -1231, -1231,    66, -1231, -1231,   411,  1227, -1231, -1231,
   -1231, -1231, -1231, -1231, -1231,   297, -1231, -1231, -1231, -1231,
     323,   343,   361,   873, -1231,    17, -1231,    17,   263, -1231,
   -1231, -1231, -1231,   263,    17,   263,    17, -1231,   263,   263,
      17,    17, -1231, -1231, -1231, -1231,    17,    17,    17,    17,
   -1231,    17, -1231,    17,    17,    17, -1231, -1231, -1231, -1231,
     263,   263, -1231, -1231,    17,   263, -1231, -1231,   263,    17,
      17, -1231,    17, -1231, -1231,   263, -1231,    17,    17,   263,
   -1231, -1231,    17,    17,   263, -1231, -1231,    17,    17,   263,
   -1231, -1231,    17,    17,   263, -1231, -1231,    17,    17,   263,
      17,   263, -1231, -1231,    17,   263, -1231,    17, -1231, -1231,
   -1231, -1231,   263, -1231,    17,   263,    17,    17,    17,    17,
      17, -1231,    17,   263,   317, -1231,   416,   297, -1231, -1231,
     558,   317,   109,   317,   317,   156,   156,    54,   457,   105,
     156,   156,   474,   474,    54,   105,   474,   474,   873,   416,
     317,   516,   156,   156,   191,   317,   156,   156,   272,   533,
     474,   156,   537, -1231,   537,   156,   533,   474,   156,   533,
     474,   156,   533,   474,   156, -1231, -1231,   757,   126, -1231,
     156,   474,   156,  1589,   297,   175, -1231,   156,   411,   175,
   -1231,   531,   175, -1231,   411,   519,  1227, -1231,   297, -1231,
   -1231, -1231, -1231, -1231, -1231, -1231, -1231,    17,    17,   263,
   -1231,   263, -1231, -1231,    17,    17,   263,    17, -1231, -1231,
   -1231,    17,    17,    17, -1231,    17,   263, -1231, -1231, -1231,
   -1231, -1231, -1231,   263,   317,    17, -1231,    17,    17, -1231,
      17,   263,    17,    17,   317,    17,    17, -1231,    17,    17,
     873,   317, -1231,    17,    17,    17, -1231,    17,    17,   263,
   -1231, -1231,    17,    17,    17,   263,   317,    17,    17,    17,
      17, -1231,   317,   317,    17,    17,   317,    17,    17,   317,
      17,    17, -1231, -1231,   -93, -1231,   317,    17, -1231,   317,
      17,    17,    17,   361,   317, -1231,   317,    17, -1231,    17,
     263,    17, -1231,    17,   263,   317, -1231,   532,   535,   156,
     156, -1231, -1231,   537, -1231,   461,   539,   537,   317,   352,
     105,   515,   317,   297,  1328, -1231,   533,    48,    48,   533,
     156,   533,   105, -1231,   533,   533,   345,   317,   177, -1231,
   -1231, -1231,   533,    48,    48, -1231, -1231,   156,   317,   352,
     533,  1603, -1231,   533,   231, -1231, -1231, -1231, -1231,   533,
      71, -1231,   533,   169, -1231,   533,    21, -1231, -1231,   297,
   -1231, -1231,   297, -1231, -1231, -1231,   533,   173,  1462,   317,
     297, -1231, -1231,   531,  1549,   317,   156,   550,  1509,   317,
     156, -1231,    17, -1231, -1231,   317, -1231,   317, -1231,    17,
   -1231,   317,    17, -1231,    17, -1231,    17,   317, -1231, -1231,
   -1231,   263, -1231,   361,   317, -1231, -1231, -1231, -1231, -1231,
   -1231, -1231, -1231, -1231,    17, -1231, -1231, -1231, -1231,    17,
      17,    17, -1231,    17,    17,    17,    17,   317, -1231,    17,
     317,   317,   317,   317, -1231, -1231,    17,    17,   263, -1231,
   -1231, -1231,    17,   263,   317,    17,    17,    17,    17, -1231,
      17, -1231,    17,   317,    17,   317,    17,    17,   317,    17,
     317,    17,   317,    17,   306,   456, -1231,   576,   317, -1231,
   -1231, -1231, -1231,    17, -1231, -1231, -1231, -1231, -1231, -1231,
   -1231, -1231, -1231, -1231, -1231,    17,   263,    17,   263, -1231,
      17,   263,    17,   263,    17,   263,    17,   263,    17, -1231,
     263,    17, -1231, -1231,    17, -1231, -1231, -1231,   263,    17,
      17,   263,    17,   263, -1231, -1231,    17, -1231,   263, -1231,
   -1231,    17,   535, -1231, -1231, -1231, -1231, -1231, -1231,   107,
   -1231,   156,   297, -1231,   488,   488,   488,   488,    54,    70,
     317,    54,   317, -1231,   531, -1231, -1231, -1231, -1231, -1231,
   -1231,   156, -1231,   156, -1231,    54,   252,   317,    54,   317,
     416,   517, -1231,   488, -1231,   272,   317, -1231,   317, -1231,
     317, -1231,   317, -1231,   297, -1231, -1231,   297,   297, -1231,
     468, -1231, -1231, -1231, -1231,   516,   150,   593,   600, -1231,
     156,   366, -1231,   156,   312, -1231,   461,   248, -1231,   461,
     346, -1231,   757, -1231,   481, -1231,  1143,   317,   156, -1231,
   -1231,   156, -1231,   461,   537,   317,   265,    57, -1231, -1231,
   -1231,    17, -1231,    17, -1231, -1231, -1231, -1231,    17,    17,
      17,    17,    17,    17,    17, -1231,    17, -1231,    17, -1231,
      17,    17,    17,    17, -1231,    17,    17, -1231,    17, -1231,
   -1231,    17,    17,    17,    17, -1231, -1231, -1231, -1231, -1231,
     480,   468, -1231,   576,   297, -1231,    17, -1231,    17, -1231,
      17, -1231,    17, -1231, -1231,   317,    17,    17,    17, -1231,
     317,    17,    17, -1231,    17,    17, -1231,    17, -1231, -1231,
      17, -1231,   317, -1231, -1231,    17,    17,    17,   263,    17,
   -1231,    17,    17,   317, -1231, -1231, -1231, -1231, -1231, -1231,
     317,    17,    17,   317,   317,   317,   317,   317,   317, -1231,
   -1231,   317,   466,   317,   873,   873,   317, -1231,   177, -1231,
   -1231,   317,   638,   317,   317,   317, -1231, -1231,   297, -1231,
   -1231, -1231,   317, -1231,   261, -1231, -1231,   312, -1231,   248,
   -1231, -1231, -1231,   248, -1231, -1231,   461, -1231,   461,   757,
   -1231, -1231, -1231,   989, -1231,  1227, -1231,   416,   156, -1231,
      17,  1088,   317,   531, -1231, -1231,    17,    17,    17,    17,
   -1231, -1231,    17,    17,    17, -1231, -1231,    17,    17, -1231,
      17, -1231, -1231, -1231, -1231, -1231,    17, -1231,   263,    17,
   -1231,    17, -1231, -1231, -1231,   859, -1231,   317,    17,    17,
      17, -1231,    17,    17,    17,    17, -1231,    17, -1231,    17,
   -1231, -1231,   317,    17,   317,    17, -1231,    17,   550, -1231,
     263,    17,    17, -1231,   636,   636,   636,   636, -1231, -1231,
   -1231,   317,   317, -1231, -1231,   156, -1231,   636,   635, -1231,
   -1231,   244,   528,   594,   248, -1231, -1231, -1231, -1231,   461,
     428,   317, -1231, -1231, -1231,   520,   317,   317,   263,   156,
   -1231, -1231, -1231,    17,   263,    17,   263,    17,    17,   263,
   -1231, -1231,    17,    17,   390,   635, -1231,    17,    17, -1231,
      17, -1231, -1231,    17, -1231,    17, -1231, -1231, -1231, -1231,
   -1231, -1231, -1231, -1231,    17,    17, -1231,   263, -1231, -1231,
     265,    17,  1137, -1231,   156,   709, -1231,   156, -1231,   863,
   -1231,   156,   873,  1023, -1231, -1231,   528,   594,   594, -1231,
     461, -1231, -1231,   156,   317,   416, -1231, -1231, -1231, -1231,
   -1231, -1231, -1231, -1231, -1231, -1231, -1231,   263, -1231,   263,
      17, -1231,    17, -1231,    17,    17,    17, -1231,    17,    17,
      17, -1231, -1231,    17,    17,   263,    17, -1231, -1231, -1231,
   -1231,   317,    17,    17,    17,   156,   156,  1509,  2844, -1231,
   -1231,  2765, -1231,  2805,   317,  1596, -1231,  1596, -1231,   156,
     594, -1231,   873,   317,  1268,   317,   317,    17,    17, -1231,
   -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231,
   -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231,
   -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231, -1231,
   -1231, -1231, -1231, -1231, -1231, -1231,    17,    17,    17,    17,
      17,   317, -1231,    17,    17,    17,    17,    17,   317, -1231,
      17, -1231,    17, -1231,    17, -1231,    17, -1231,    17, -1231,
   -1231,    17,   263, -1231, -1231,   873,   317,   141,   317,   469,
     469,   758,   758, -1231,   313,   475,   469,   407,   407, -1231,
     288, -1231, -1231,   317, -1231, -1231,   265,    17, -1231, -1231,
      17,    17, -1231,    17,   263, -1231, -1231,    17,    17, -1231,
      17,   263,    17, -1231, -1231,    17,    17, -1231,    17,   263,
      17, -1231,    17,    17, -1231,    17,    17, -1231,    17,    17,
   -1231,    17, -1231,    17,    17, -1231,    17, -1231,    17, -1231,
     317,   317, -1231, -1231,    77, -1231,   297, -1231, -1231,   313,
   -1231,   461,   686, -1231, -1231, -1231,   313, -1231,   461,   686,
   -1231, -1231, -1231,   686, -1231, -1231,   288, -1231, -1231, -1231,
     288, -1231, -1231, -1231, -1231,    17, -1231,    17,   317,    17,
      17,    17,    17,    17,    17,   317,    17,    17,    17,    17,
   -1231, -1231, -1231, -1231,   686, -1231,   395, -1231, -1231,   686,
   -1231, -1231, -1231, -1231, -1231, -1231,    17,   317,    17, -1231,
   -1231, -1231
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
     643,     0,     0,     0,   379,   527,   680,   681,   678,   679,
       0,   694,     0,     0,     0,     0,     0,     0,     0,   561,
     560,     0,     0,     0,     0,     0,     0,   442,     0,   439,
     441,     0,     0,     0,     0,     0,    47,    69,     0,    54,
      57,    72,     0,    95,     0,    86,    99,     0,   121,     0,
     122,   123,   132,     0,   124,   125,     0,   126,     0,     0,
     173,   645,   646,     0,   647,     0,   649,    27,     0,   662,
       0,     0,     0,     0,   691,   377,     0,     0,     0,     0,
     562,   564,   190,   521,   521,   520,   203,   190,   190,   443,
     190,   445,   188,   186,   185,   187,   190,   467,     0,   190,
     460,     0,   578,    64,    56,     0,   568,     0,   102,   103,
     104,   105,    85,    85,    85,    85,   138,     0,   146,   142,
     151,   152,     0,    80,     0,   531,   544,   378,     0,   697,
       0,   694,   694,   693,     0,     0,     0,     0,   524,   522,
     523,     0,     0,   440,   462,     0,   463,     0,     0,    63,
      97,     0,     0,     0,     0,   127,   128,   129,   130,     0,
       0,     0,   147,   648,   660,     0,     0,     0,     0,     0,
     696,   695,   243,   214,     0,   209,     0,    80,   220,     0,
     192,   189,     0,   475,    65,     0,    61,   106,   107,   108,
     109,   110,   111,    85,   139,     0,   143,   141,   541,   536,
     537,   538,   539,   540,   190,   190,   534,     0,   530,   542,
       0,     0,     0,   213,     0,     0,   208,     0,   218,     0,
     219,     0,     0,     0,   474,    62,     0,     0,     0,   131,
       0,   533,   532,     0,     0,    27,   183,   180,   179,   182,
     200,   181,   201,   217,   345,   175,   177,     0,   176,     0,
     215,   244,     0,   212,   209,    80,     0,   222,   220,     0,
     190,   484,   479,    80,   480,     0,   112,   113,   114,   115,
     140,     0,   194,   682,   190,     0,     0,     0,     0,   211,
     210,     0,   221,     0,     0,     0,   476,     0,   478,     0,
       0,   135,     0,     0,     0,     0,     0,     0,   194,   216,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,   297,   246,   248,   250,
     252,     0,   245,   270,   277,   279,   281,   283,     0,   276,
     293,   184,   480,   482,   480,   485,   378,   116,   190,   535,
     685,    80,     0,   677,   699,     0,     0,     0,     0,     0,
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
   -1231, -1231, -1231,  -221,  -223,  -191, -1231,   299,  -194,   335,
   -1231, -1231, -1231, -1231, -1231, -1231,  -232,  -333,  -653,   -11,
    -776,  -637, -1231, -1231, -1231, -1231, -1231, -1231, -1231,  -550,
    -208, -1231, -1231, -1231,  -871, -1231, -1231,  -203,  1296,  1294,
     -57,  1339, -1231,  -708,  -565,  -575, -1231, -1231,  -126, -1231,
   -1231,  -125, -1231, -1231, -1231,  -127,  -279, -1231, -1231,  -793,
   -1231, -1231, -1231, -1231, -1231,  -781, -1231, -1231, -1231, -1231,
    -478, -1231, -1231, -1231,   110, -1231, -1231, -1231, -1231, -1231,
     174, -1231, -1231,  -505, -1231, -1231,  -477, -1231, -1231,  -553,
   -1231, -1231, -1231, -1231,  -554,  1443,  -394, -1224,  -528, -1231,
   -1231, -1231,  -759,  -928,   -38, -1231,  -480, -1231, -1231, -1231,
   -1231,  -473,  -331,   154, -1231, -1231,  -677, -1022,  -347,  -412,
   -1001,  -644, -1231,  -945,  -561, -1231, -1231, -1231, -1231,  -564,
   -1231, -1231, -1231, -1231,  -688,  -562, -1231,  -722, -1231, -1230,
   -1231,  -694, -1231,   750,  -417,  -107,   551,  -426,    47,  -202,
    -317,   132, -1231, -1231, -1231,   223, -1231,   -77, -1231,   -76,
   -1231, -1231, -1231, -1231, -1231, -1231,  -828, -1231, -1231, -1231,
   -1231,   643,   644,   647,   648,  -280,   843, -1231, -1231,  -174,
      45, -1231, -1231, -1231, -1231, -1231,   -99, -1231, -1231, -1231,
   -1231,    68, -1231,   437,  -105, -1231, -1231, -1231,   652, -1231,
   -1231, -1231,  -641, -1231, -1231, -1231,   568,   592,   148,  -166,
       5,   321, -1231, -1231, -1231, -1231, -1231, -1231, -1231,  -365,
    -800,  -937, -1231, -1231,   671,   672, -1231,   232, -1231,  -756,
    -672, -1231, -1231, -1231,  -187, -1231,   675, -1231,  -184, -1231,
     677, -1231,  -188, -1231,   678, -1231,  -193, -1231, -1231,   410,
   -1231, -1231, -1231, -1231, -1231,   540,  -381, -1231, -1231,  -380,
   -1231, -1231,  -790, -1231, -1231, -1231, -1231,  -767, -1231, -1231,
     681, -1231, -1231,   622, -1231,   627, -1231, -1231,   234,  -624,
     236,   237,   238,   699, -1231, -1231, -1231, -1231, -1231,   700,
   -1231, -1231, -1231, -1231,   708, -1231, -1231,   711, -1231, -1231,
     713, -1231, -1231,   715,  -183,  -349,   116, -1231, -1231, -1231,
   -1231, -1231, -1231, -1231, -1231, -1231, -1231,   838, -1231,   514,
    -269, -1231,  -120,  -213, -1231, -1231,  -103, -1231,  -130, -1231,
   -1231, -1231,  -810, -1231, -1231, -1231,   522,   -67,   852, -1231,
   -1231,   525, -1090,  -530, -1231,  -999,   867, -1231, -1231, -1231,
     -69,  -386, -1231, -1231
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,   235,   620,   237,   880,   238,   470,
     239,   240,   419,   420,   241,   348,   242,   243,   894,   589,
     503,   590,   504,   695,   890,   591,   809,   969,   592,   810,
     893,  1034,  1035,  1115,   811,   812,   813,   895,   109,   215,
     382,   456,   922,   609,   749,   819,   712,   713,   714,   715,
     716,   717,   718,   905,  1037,   719,   720,   721,   910,   722,
     723,   914,  1047,  1125,  1201,   724,  1091,   725,   917,  1049,
     726,   727,   920,  1052,   489,   351,    41,   136,   245,   427,
     428,   429,   615,   430,   431,   617,   729,   730,  1174,  1175,
    1176,  1177,  1027,  1028,   874,   383,   667,  1178,  1223,   673,
     668,  1179,   870,  1017,   448,   449,  1146,   450,  1143,   451,
     452,  1150,   453,   649,   650,   651,   858,  1105,  1103,  1108,
    1106,  1182,  1271,  1329,  1337,  1272,  1344,  1278,  1347,  1352,
    1279,  1357,  1298,  1322,  1266,  1330,  1331,  1338,  1339,  1332,
    1324,  1180,    42,   252,   353,   534,    44,   354,   253,   138,
     247,   538,   248,   441,   624,   435,   436,   621,   619,   254,
     255,   445,   446,   634,   542,   630,   845,   631,   853,    46,
      47,    48,    49,    50,    51,   454,   140,    52,    53,   256,
     437,   557,    55,   143,   271,   468,   455,   147,   273,   471,
      56,   257,    58,   149,   203,   298,   299,   492,    59,    60,
     275,   276,   787,   277,   278,   279,   258,   259,   457,   876,
     936,   375,    62,   152,   284,   285,   482,   478,   963,   738,
     680,   881,  1029,    63,    64,    65,   290,   486,  1154,  1218,
    1194,  1195,  1286,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,   210,    80,   318,
     319,   506,   320,   321,   509,   937,   953,   461,   659,   941,
     520,   746,   739,  1096,  1135,  1136,  1137,   740,   741,  1057,
      81,    82,    83,   260,    84,   261,    85,    86,   262,   770,
     263,   264,   265,    87,    88,   162,   324,   325,   703,    89,
     292,   293,   294,   295,    90,   303,   304,    91,   308,   309,
      92,   313,   314,    93,    94,   327,   599,    95,   164,   331,
     332,   514,    96,   182,    97,   183,   184,   938,   218,   219,
     837,    99,   186,   334,   335,   516,   336,   191,   342,   343,
     927,   928,   742,   743,   100,   221,   222,   605,   939,   102,
     224,   225,   940,  1225,   103,   748,   328,   105,   523,   848,
     849,  1002,   524,  1062
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     115,   236,   270,   345,   672,    61,   344,   337,   532,   479,
     339,   187,   188,   421,   291,   312,   146,   531,   307,   347,
     302,   267,   970,   613,   871,   943,   929,  1018,   906,   326,
     190,   519,   772,   711,   842,   434,   421,   789,   931,   737,
     907,   932,   697,   911,   808,    54,   925,    45,   485,   349,
    1164,   205,     1,   207,   512,  1113,   918,   803,     1,  1098,
    1055,   144,   587,  1104,   588,  1104,  1109,  1323,    57,   926,
      15,  1340,  1340,  1321,  1345,  1349,   144,  1354,  1354,   192,
    1358,   193,   518,   194,   195,   464,   196,     5,   859,   860,
     861,  1170,   198,   199,     5,   200,   201,   202,   204,   202,
     206,   202,   208,   209,   251,   211,   189,   212,    17,    33,
      40,   144,     2,   106,     2,   422,   883,   338,    15,   266,
     340,   447,     4,  1184,     4,   289,     1,   216,    26,   217,
     220,   223,    29,   226,    26,   333,    19,   145,    29,  1400,
    1397,   249,   311,     5,   333,   822,  1404,   825,  1109,   828,
     250,   831,   145,   833,   442,  1170,  1408,    33,     5,   425,
    1409,    34,  1013,  1319,  1079,   502,  1172,     5,   246,   370,
     246,   433,   513,    19,   467,   483,   704,   316,   251,   301,
     114,  1039,    17,  1053,   670,  1014,   526,   145,   350,   671,
     352,   484,   249,  1040,     5,   525,  1042,   357,    34,   359,
     476,   250,    26,   362,   363,   710,    29,  1116,     5,   364,
     365,   366,   367,  1050,   368,   189,   369,   202,   371,   994,
     326,   996,   626,   734,  1190,  1292,  1360,   374,  1204,  1097,
    1172,   956,   378,   379,  1061,   380,    21,   961,   948,   485,
     968,   384,   951,   274,  1155,   386,   387,   850,   288,   731,
     389,   390,   682,   966,  -635,   392,   393,   891,   144,   670,
     627,    36,     5,   398,   671,     5,    10,   401,    21,     5,
     403,   924,   661,   306,   704,   707,   560,   405,   517,   408,
     409,   411,   412,   414,   521,   415,   282,   251,  1117,     7,
     600,   706,   707,    36,  1288,   315,   316,   708,   709,  1141,
    1118,  1120,   709,   134,    12,     5,   338,    10,   283,   340,
    1319,   633,   297,   459,   708,   709,  1112,    31,    32,   317,
    1166,  -637,     5,   973,  1167,  1168,  1169,   975,   296,     5,
       5,   978,   980,   981,   145,  1319,   984,   985,   704,   899,
     987,   903,    24,   908,   903,  1093,   912,   903,   493,   915,
     903,   496,   664,   903,   499,  1333,   665,   694,    31,    32,
     696,  1350,     5,     5,   807,   708,   709,  1317,   733,     2,
     527,   528,   704,   654,   656,  1196,   473,   352,   533,     4,
     536,  -638,  1171,     5,   537,   539,   540,   687,   541,   676,
     678,  -639,   704,   693,   868,   691,   689,   707,   546,  1148,
     547,   548,   670,   549,  1202,   551,   552,   671,   554,   555,
     762,   556,   558,   246,  1291,   326,   562,   563,   564,   708,
     709,   374,   107,   108,     5,   443,   569,   570,  1334,  1319,
     573,   574,   463,   323,   531,   664,  1059,   579,   580,   665,
     582,   583,   851,   585,   586,   228,  1227,  1228,   230,   231,
     232,  1185,   341,   596,   597,   598,   532,   421,  1313,  1193,
     603,   -51,   604,   588,   607,   696,   608,  1085,  1086,  1087,
    1088,   653,   655,   346,   882,   476,  1166,  1210,   213,   214,
    1167,  1168,  1169,     5,   477,  1216,     5,   675,   677,  1326,
     751,  1319,     5,   -30,   490,  1326,  1041,  1319,   686,  1043,
     903,  1044,   903,  1186,   735,  1045,   903,  1189,   652,   458,
     229,   657,   755,   660,   444,   757,   662,   663,   329,   330,
     474,   475,   459,  1273,   674,  1280,  1311,  1128,  1312,   488,
     854,  1129,  1130,  1131,  1132,   685,   495,     5,  1171,   498,
       7,   688,   501,  1282,   690,  1284,   -59,   692,  1159,   447,
     511,   -59,   -59,   -59,     5,   752,   518,   154,   698,   156,
     807,   158,   533,   160,   423,   533,   522,   537,   424,   759,
     233,   234,   889,   628,   629,   696,   892,  1173,   246,   433,
    1183,   708,   709,  1315,  1187,  1353,  1353,   764,  1192,  1133,
     423,   424,   765,   766,   767,   747,   768,   769,   771,   769,
     502,   433,   774,  1119,  1121,  1122,  1123,   903,   884,   228,
     229,   587,   230,   231,   232,   233,   234,     5,   785,   786,
     788,   786,   705,   790,   805,   791,   704,   793,   921,   795,
     706,   707,  1069,  1070,   942,   913,  1060,   588,   916,   855,
     856,   857,  1229,   618,  1102,  1022,   815,   623,   709,  1023,
    1024,  1025,  1283,   708,   709,     5,   991,   992,   816,  1290,
     818,   696,   892,   821,   421,   824,   666,   827,   469,   830,
     864,   830,   839,   840,   598,  1100,  1101,   836,  1197,  1198,
    1199,   418,   604,   604,   804,   608,   971,   875,   967,   844,
     897,   901,   900,  1166,   847,  1038,  1126,  1167,  1168,  1169,
    1296,   612,   807,     5,  1209,   134,   228,   229,   728,   230,
     231,   232,   233,   234,   728,  1212,  1166,  1102,   728,  1107,
    1167,  1168,  1169,  1170,   228,   684,     5,   230,   231,   232,
     233,   234,   805,    10,   806,   704,  1033,  1188,   705,   706,
     707,  1341,  1287,  1348,  1355,   807,  1328,  1328,  1336,  1336,
      43,  1343,   287,  1328,   251,  1171,   758,   502,    17,   622,
    1016,  1016,   708,   709,   710,  1166,   933,   280,   286,  1167,
    1168,  1169,   165,   166,  1054,     5,   167,   168,  1171,  1334,
    1319,   169,   807,   228,    31,    32,   230,   231,   232,   233,
     234,   281,  1020,   806,   759,  1046,   844,  1048,  1172,   566,
     170,   171,   683,   696,   172,   769,   173,   174,   507,   769,
     175,   952,  1078,   954,   955,   786,  1399,   268,   958,  1402,
     896,   786,   269,  1403,   962,   791,  1406,  1171,   176,   177,
    1407,   699,   421,   700,   701,   702,   862,   178,    98,   866,
     179,   830,   180,   974,   181,   830,  1114,   835,   406,   977,
     979,   830,   101,   872,   983,   830,   878,   986,   830,   410,
     988,  1426,  1427,   989,   413,  1410,  1428,   104,   836,   836,
     993,  1102,   608,  1004,   997,   998,     0,     0,     0,   863,
       5,     0,     0,   696,  1001,  1003,     0,     0,     0,   704,
       0,     0,   705,   706,   707,     0,   873,     0,  1124,     0,
       0,     0,   460,   462,   134,   228,   465,   466,   230,   231,
     232,     0,     0,     0,     0,     0,   708,   709,   710,     0,
     487,     0,     0,     0,     0,     0,   898,   494,   902,     0,
     497,   902,     0,   500,   902,     0,     0,   902,     0,     0,
     902,   510,     0,  1058,     0,     0,   728,     0,     0,  1064,
    1065,  1066,  1067,     0,   934,   141,   952,   952,  1016,     0,
     150,     0,   153,     0,   155,     0,   157,     0,   159,  1200,
       0,   134,   228,   229,  1077,   230,   231,   232,   233,   234,
       0,  1081,  1082,  1083,     0,  1084,   830,   830,   830,     0,
    1089,     0,  1090,     0,     0,     0,   608,     0,  1095,     0,
     844,     0,     0,     0,  1001,  1001,     5,   228,   229,     0,
     230,   231,   232,   233,   234,   704,     0,   806,   705,   706,
     707,   228,   229,     0,   230,   231,   232,     0,  1016,     0,
    1166,     0,   736,     0,  1167,  1168,  1169,  1170,     0,     0,
       5,     0,   708,   709,   710,     0,  1142,     0,  1145,   704,
     608,  1149,   705,   706,   707,  1152,  1153,     0,     0,  1191,
    1156,  1157,  1012,  1158,     0,     0,   830,     0,  1160,     0,
       0,     0,  1026,     0,     0,     0,   708,   709,   710,     0,
       0,     0,     0,     0,  1165,     0,     0,   902,     0,   902,
       0,     0,  1171,   902,     0,     0,     0,   476,     0,     0,
       0,  1016,     0,   728,     0,     5,     0,     0,     0,     0,
       0,   728,  1172,     0,   704,     0,     0,   705,   706,   707,
       0,     0,     0,  1207,     0,  1208,   735,  1145,   608,  1211,
       0,  1149,  1213,     0,  1398,     0,  1215,  1217,     0,  1220,
       0,   708,   709,   710,  1166,  1222,  1224,     0,  1167,  1168,
    1169,  1170,   476,     0,     5,     0,     0,     0,     0,     0,
       5,     0,     0,   704,     0,     0,   705,   706,   707,   704,
    1295,  1222,   705,   706,   707,     0,     0,     0,     0,     0,
    1401,   735,     0,     0,     0,   923,   736,  1405,     0,     0,
     708,   709,   710,     0,   902,     0,   708,   709,   710,     0,
       0,     0,     0,     0,     0,  1134,  1171,     0,     0,  1297,
    1299,  1300,  1301,  1302,     0,     0,  1304,  1305,  1306,  1307,
    1308,     0,     0,  1310,     0,  1217,  1172,  1217,     0,   844,
       0,     0,     0,     0,   608,     0,     0,     3,     0,     0,
       0,     0,     0,     0,     0,     6,     0,     0,     0,     0,
       0,     0,   728,     0,     8,   728,     0,     0,     0,   728,
       0,     9,     0,   728,    11,     0,  1364,     0,     0,     0,
       0,     0,     0,  1369,     0,  1372,     0,     0,     0,    16,
       0,  1376,     0,  1379,     0,     5,     0,     0,  1383,     0,
      18,  1386,     0,    20,   704,    22,  1390,   705,   706,   707,
       0,     0,     0,     0,     0,     0,    25,     0,    27,     0,
      28,   736,    30,   747,     0,     0,     0,   728,    35,     0,
       0,   708,   709,   710,     0,   728,     0,   728,     0,     0,
       0,     0,     0,     0,   728,     0,     0,     0,  1224,     0,
       0,   110,     0,  1414,  1416,     0,     0,  1419,   111,   112,
       0,   113,     0,     0,   116,     0,   117,     0,   118,     0,
       0,     0,   119,     0,   120,     0,   121,     0,   122,     0,
     123,     0,   124,     0,   125,     0,   126,     0,     0,     0,
       0,     0,     0,     0,   127,     0,     0,     0,     0,     0,
       0,   128,     0,   129,     0,   130,     0,   131,     0,   132,
       0,   133,     0,     0,     0,     0,     0,  1320,     0,  1327,
    1327,  1335,  1335,     0,  1342,  1346,  1327,  1351,  1351,     0,
    1356,   635,   636,   637,   638,   639,   640,   641,   642,   643,
     644,   645,   646,   647,   648,     0,     0,     0,     0,     0,
       0,     0,     0,   197,     0,     0,     0,     0,   135,   137,
     139,   139,   142,     0,     0,   148,   139,   151,   139,   148,
     139,   148,   139,   148,   139,   148,   161,   163,     0,   185,
     185,   185,     3,     0,     0,     0,     0,     0,     0,     5,
       6,     0,     0,     0,     0,     0,     0,   227,   704,     8,
       0,   705,   706,   707,     0,     0,     9,   300,     0,   305,
       0,   310,     0,     0,   322,     0,     0,     0,    14,     0,
       0,     0,     0,     0,    16,   708,   709,   710,     0,     0,
       0,     0,     0,     0,     0,    18,     5,     0,    20,     0,
      22,   244,     0,     0,     0,   704,   272,     0,   705,   706,
     707,    25,     0,    27,   355,    28,     0,    30,     0,   356,
       0,   358,     0,    35,   360,   361,     0,     0,   476,     0,
       0,     0,   708,   709,   710,     0,     5,     0,     0,     0,
       0,     0,     0,     0,     0,   704,   372,   373,   705,   706,
     707,   376,     0,     0,   377,     0,     0,   735,     0,     0,
       0,   381,   736,     0,     0,   385,     0,     0,     0,     3,
     388,     0,   708,   709,   710,   391,     0,     6,     0,     0,
     394,     0,     0,     5,     0,   397,     8,   400,     0,     0,
       0,   402,   704,     9,     0,   705,   706,   707,   404,     0,
       0,   407,  1191,     0,     0,    14,     0,     0,   417,   416,
       0,    16,     0,     0,     0,   432,     0,   438,   439,   708,
     709,   710,    18,     0,     0,    20,     0,    22,     0,     0,
       0,     0,     0,     0,   472,     0,     0,     0,    25,   480,
      27,     0,    28,     0,    30,     0,     0,     0,     0,     0,
      35,     0,     0,     0,     0,     0,     0,     0,     0,   426,
       0,     0,     0,     0,   440,   137,   635,   636,   637,   638,
     639,   640,   641,   642,   643,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   481,   137,     0,     0,     0,
       0,     0,     0,     0,   491,   529,     0,   530,     0,     0,
       0,     0,   535,     0,     0,     0,   505,     0,     0,   508,
       0,     0,   543,     0,     0,     0,   515,     0,   545,   544,
       0,     0,     0,     0,     0,     0,     0,   550,   553,     0,
     395,   396,     0,   399,     0,   561,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   567,     0,     0,     0,     0,
     572,   571,     0,     0,     0,     0,   577,   578,     0,     0,
     581,     0,     0,   584,     0,     0,     0,     0,     0,     0,
     593,     0,     0,   595,     0,     0,     0,     0,   601,     0,
     602,     0,     0,     0,     0,     0,   606,     0,     0,   611,
     610,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   625,     0,     0,     0,   632,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   669,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   681,     0,     0,     0,     0,     0,   614,   616,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   658,
       0,     0,     0,   732,     0,     0,     0,     0,     0,   744,
       0,     0,     0,   750,     0,     0,   679,     0,     0,   753,
       0,   754,   559,     0,     0,   756,     0,     0,     0,     0,
     565,   760,     0,     0,     0,   568,     0,   761,   763,     0,
       0,     0,   575,   576,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   745,     0,     0,     0,   185,
     594,   773,     0,     0,   775,   776,   777,   778,     0,     0,
       0,     0,     0,     0,   781,     0,     0,     0,   784,   783,
       0,     0,     0,     0,     0,     0,     0,   792,     0,   794,
       0,     0,   797,     0,   799,     0,   801,     0,     0,     0,
       0,     0,   814,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   817,     0,   820,     0,     0,   823,     0,   826,
       0,   829,     0,   832,     0,     0,   834,     0,     0,     0,
       0,     0,     0,     0,   838,     0,     0,   841,     0,   843,
       0,     0,     0,     0,   846,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   865,     0,   867,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   877,     0,   879,     0,     0,     0,     0,     0,     0,
     885,     0,   886,     0,   887,     0,   888,     0,     0,     0,
     852,     0,     0,     0,     0,     0,     0,     0,     0,   779,
     780,     0,     0,     0,     0,   782,     0,     0,     0,     0,
     869,     0,   869,     0,     0,     0,     0,     0,     0,     0,
     796,   930,   798,     0,   800,     0,   802,     0,     0,   935,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   904,
       0,     0,   909,     0,     0,     0,     0,     0,     0,     0,
       0,   919,     0,     0,     0,     0,     0,   679,     0,     0,
     679,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   976,
       0,     0,     0,     0,   982,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   990,     0,     0,     0,
       0,     0,     0,     0,   995,     0,     0,   999,     0,     0,
       0,     0,     0,     0,  1000,     0,     0,  1005,  1006,  1007,
    1008,  1009,  1010,     0,     0,  1011,     0,  1015,     0,     0,
    1019,     0,     0,     0,     0,  1021,     0,  1030,  1031,  1032,
       0,     0,     0,     0,     0,     0,  1036,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1063,     0,     0,     0,
       0,   944,   945,   946,   947,     0,   949,   950,     0,     0,
       0,     0,     0,     0,     0,     0,   957,     0,   959,   960,
       0,     0,     0,     0,  1075,     0,   964,   965,  1051,     0,
       0,  1080,     0,     0,     0,     0,     0,  1056,     0,   972,
       0,     0,     0,     0,     0,     0,  1092,     0,  1094,     0,
       0,     0,     0,     0,     0,     0,  1099,     0,     0,     0,
       0,     0,     0,     0,     0,  1110,  1111,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1127,     0,     0,     0,     0,
    1138,  1139,     0,     0,  1140,     0,     0,     0,     0,     0,
    1144,     0,  1147,     0,     0,  1151,     0,     0,     0,     0,
       0,     0,     0,     0,   869,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1163,     0,     0,     0,     0,   679,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1068,     0,     0,  1203,     0,
    1071,  1072,     0,  1073,     0,     0,     0,     0,     0,  1074,
       0,     0,  1076,  1205,     0,  1206,     0,     0,     0,     0,
       0,     0,     0,  1181,     0,     0,  1181,     0,     0,     0,
    1181,  1219,     0,     0,     0,  1221,     0,     0,     0,     0,
       0,     0,   869,     0,     0,     0,     0,     0,  1281,     0,
       0,     0,     0,     0,     0,     0,     0,  1289,     0,  1293,
    1294,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   869,   869,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1285,     0,
       0,     0,     0,     0,     0,  1303,     0,     0,     0,     0,
       0,     0,  1309,     0,     0,     0,     0,  1161,  1162,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1316,     0,
    1318,     0,  1325,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1359,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1366,     0,     0,     0,     0,     0,     0,  1371,     0,     0,
       0,     0,     0,  1214,     0,  1378,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1226,     0,     0,
       0,     0,     0,     0,  1395,  1396,     0,     0,     0,     0,
       0,     0,     0,     1,     2,     0,     0,     0,     0,     3,
       0,     0,     0,     0,     4,     0,     5,     6,     0,     0,
       0,     0,     0,     7,     0,     0,     8,     0,     0,     0,
       0,     0,  1412,     9,    10,     0,    11,     0,    12,  1421,
       0,     0,     0,    13,     0,    14,     0,     0,    15,     0,
       0,    16,     0,     0,     0,     0,     0,     0,     0,    17,
       0,  1430,    18,     0,    19,    20,    21,    22,     0,     0,
       0,  1314,     0,     0,     0,    23,    24,     0,    25,    26,
      27,     0,    28,    29,    30,    31,    32,    33,     0,    34,
      35,    36,     0,     0,     0,     0,     0,     0,     0,     0,
    1361,     0,     0,  1362,  1363,     0,  1365,     0,     0,     0,
    1367,  1368,     0,  1370,     0,  1373,     0,     0,  1374,  1375,
       0,  1377,     0,  1380,     0,  1381,  1382,     0,  1384,  1385,
       0,  1387,  1388,     0,  1389,     0,  1391,  1392,     0,  1393,
       0,  1394,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1411,     0,  1413,  1415,     0,  1417,  1418,  1420,     0,  1422,
    1423,  1424,  1425,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1429,
       0,  1431,  1267,  1230,  1231,  1232,  1233,  1234,  1235,  1236,
    1268,  1237,  1238,  1239,  1240,  1241,  1242,  1243,  1244,  1245,
    1246,  1247,  1248,  1249,  1250,  1251,  1252,  1253,  1254,  1255,
    1256,  1257,  1258,  1259,  1260,  1269,  1270,  1261,  1262,  1263,
    1264,  1265,  1274,  1230,  1231,  1232,  1233,  1234,  1235,  1236,
    1275,  1237,  1238,  1239,  1240,  1241,  1242,  1243,  1244,  1245,
    1246,  1247,  1248,  1249,  1250,  1251,  1252,  1253,  1254,  1255,
    1256,  1257,  1258,  1259,  1260,  1276,  1277,  1261,  1262,  1263,
    1264,  1265,  1230,  1231,  1232,  1233,  1234,  1235,  1236,     0,
    1237,  1238,  1239,  1240,  1241,  1242,  1243,  1244,  1245,  1246,
    1247,  1248,  1249,  1250,  1251,  1252,  1253,  1254,  1255,  1256,
    1257,  1258,  1259,  1260,     0,     0,  1261,  1262,  1263,  1264,
    1265
};

static const yytype_int16 yycheck[] =
{
      57,   192,   196,   235,   558,     0,   227,   220,   434,   374,
     223,   131,   132,   346,   201,   208,   115,   434,   206,   242,
     204,   195,   893,   528,   783,   853,   836,   955,   821,   212,
     133,   411,   656,   598,   742,   352,   369,   678,   838,   604,
     821,   841,   592,   824,   697,     0,   836,     0,   379,   243,
    1140,   156,     4,   158,   403,  1077,   832,   694,     4,  1058,
     997,     6,   155,  1064,   157,  1066,  1067,  1297,     0,   836,
      49,  1301,  1302,  1297,  1304,  1305,     6,  1307,  1308,   136,
    1310,   138,    25,   140,   141,   365,   143,    17,   765,   766,
     767,    14,   149,   150,    17,   152,   153,   154,   155,   156,
     157,   158,   159,   160,    56,   162,    40,   164,    60,    88,
       0,     6,     5,     0,     5,   347,   793,   220,    49,   195,
     223,    16,    15,  1145,    15,   201,     4,   184,    80,   186,
     187,   188,    84,   190,    80,    78,    65,    82,    84,  1369,
    1364,    19,    73,    17,    78,   720,  1376,   722,  1149,   724,
      28,   726,    82,   728,   356,    14,  1386,    88,    17,   350,
    1390,    90,   952,    22,  1035,   397,    89,    17,    61,   274,
      61,    62,   404,    65,   368,   377,    26,    51,    56,    71,
     163,   974,    60,   993,     7,   952,   418,    82,   245,    12,
     247,   378,    19,   974,    17,   416,   977,   254,    90,   256,
       9,    28,    80,   260,   261,    55,    84,  1078,    17,   266,
     267,   268,   269,   989,   271,    40,   273,   274,   275,   927,
     403,   929,   539,   603,  1152,  1224,  1316,   284,  1165,  1057,
      89,   872,   289,   290,  1001,   292,    67,   878,   862,   570,
     893,   298,   866,   198,  1115,   302,   303,   752,   201,   598,
     307,   308,   569,   890,   163,   312,   313,   807,     6,     7,
     540,    92,    17,   320,    12,    17,    35,   324,    67,    17,
     327,   836,   552,    72,    26,    31,   470,   334,   408,   336,
     337,   338,   339,   340,   414,   342,     3,    56,  1081,    24,
     513,    30,    31,    92,  1222,    50,    51,    53,    54,  1099,
    1081,  1082,    54,   147,    39,    17,   409,    35,    25,   412,
      22,   543,   202,    48,    53,    54,  1075,    86,    87,   209,
       7,   163,    17,   898,    11,    12,    13,   902,    23,    17,
      17,   906,   907,   908,    82,    22,   911,   912,    26,   816,
     915,   818,    77,   821,   821,  1053,   824,   824,   386,   827,
     827,   389,     7,   830,   392,  1300,    11,   589,    86,    87,
     592,  1306,    17,    17,   697,    53,    54,  1295,   600,     5,
     427,   428,    26,   547,   548,  1156,   371,   434,   435,    15,
     437,   163,    69,    17,   441,   442,   443,   574,   445,   563,
     564,   163,    26,   586,   774,   583,   580,    31,   455,  1107,
     457,   458,     7,   460,  1163,   462,   463,    12,   465,   466,
     633,   468,   469,    61,  1224,   598,   473,   474,   475,    53,
      54,   478,   159,   160,    17,   357,   483,   484,    21,    22,
     487,   488,   364,    64,   851,     7,  1001,   494,   495,    11,
     497,   498,   759,   500,   501,   148,  1205,  1206,   151,   152,
     153,  1145,    41,   510,   511,   512,   882,   790,  1286,  1153,
     517,   155,   519,   157,   521,   697,   523,  1042,  1043,  1044,
    1045,   547,   548,   150,   791,     9,     7,  1185,   161,   162,
      11,    12,    13,    17,   374,  1193,    17,   563,   564,    20,
     610,    22,    17,   150,   384,    20,   974,    22,   574,   977,
     977,   979,   979,  1147,    38,   983,   983,  1151,   546,   361,
     149,   549,   619,   551,    57,   622,   554,   555,    74,    75,
     372,   373,    48,  1211,   562,  1213,  1282,     7,  1284,   381,
     762,    11,    12,    13,    14,   573,   388,    17,    69,   391,
      24,   579,   394,  1215,   582,  1217,   156,   585,  1123,    16,
     402,   161,   162,   163,    17,   612,    25,   120,   596,   122,
     893,   124,   619,   126,    32,   622,    47,   624,    33,   626,
     154,   155,   804,    58,    59,   807,   808,  1142,    61,    62,
    1145,    53,    54,  1291,  1149,  1307,  1308,   644,  1153,    69,
      32,    33,   649,   650,   651,    45,   653,   654,   655,   656,
     832,    62,   659,  1081,  1082,  1083,  1084,  1084,   795,   148,
     149,   155,   151,   152,   153,   154,   155,    17,   675,   676,
     677,   678,    29,   680,   156,   682,    26,   684,   147,   686,
      30,    31,  1013,  1014,   847,   826,  1001,   157,   829,   151,
     152,   153,  1207,   533,     8,     7,   703,   537,    54,    11,
      12,    13,  1217,    53,    54,    17,   925,   926,   715,  1224,
     717,   893,   894,   720,   997,   722,   556,   724,   369,   726,
     769,   728,   739,   740,   731,  1061,  1062,   734,  1156,  1157,
    1158,   346,   739,   740,   695,   742,   894,   786,   891,   746,
     816,   818,   817,     7,   751,   974,  1090,    11,    12,    13,
    1228,   527,  1035,    17,  1184,   147,   148,   149,   598,   151,
     152,   153,   154,   155,   604,  1188,     7,     8,   608,  1066,
      11,    12,    13,    14,   148,   571,    17,   151,   152,   153,
     154,   155,   156,    35,   158,    26,   968,  1149,    29,    30,
      31,  1302,  1220,  1305,  1308,  1078,  1299,  1300,  1301,  1302,
       0,  1304,   201,  1306,    56,    69,   624,   989,    60,   536,
     954,   955,    53,    54,    55,     7,   843,   199,    70,    11,
      12,    13,   129,   129,   995,    17,   129,   129,    69,    21,
      22,   129,  1115,   148,    86,    87,   151,   152,   153,   154,
     155,   199,   958,   158,   851,   986,   853,   988,    89,   478,
     129,   129,   570,  1035,   129,   862,   129,   129,   398,   866,
     129,   868,  1035,   870,   871,   872,  1369,   195,   875,  1372,
     815,   878,   195,  1376,   881,   882,  1379,    69,   129,   129,
    1383,   597,  1165,   597,   597,   597,   768,   129,     0,   771,
     129,   898,   129,   900,   129,   902,  1078,   731,   334,   906,
     907,   908,     0,   785,   911,   912,   788,   914,   915,   337,
     917,  1414,  1416,   920,   339,  1395,  1419,     0,   925,   926,
     927,     8,   929,   942,   931,   932,    -1,    -1,    -1,   769,
      17,    -1,    -1,  1115,   941,   942,    -1,    -1,    -1,    26,
      -1,    -1,    29,    30,    31,    -1,   786,    -1,  1089,    -1,
      -1,    -1,   362,   363,   147,   148,   366,   367,   151,   152,
     153,    -1,    -1,    -1,    -1,    -1,    53,    54,    55,    -1,
     380,    -1,    -1,    -1,    -1,    -1,   816,   387,   818,    -1,
     390,   821,    -1,   393,   824,    -1,    -1,   827,    -1,    -1,
     830,   401,    -1,  1000,    -1,    -1,   836,    -1,    -1,  1006,
    1007,  1008,  1009,    -1,   844,   112,  1013,  1014,  1152,    -1,
     117,    -1,   119,    -1,   121,    -1,   123,    -1,   125,  1160,
      -1,   147,   148,   149,  1031,   151,   152,   153,   154,   155,
      -1,  1038,  1039,  1040,    -1,  1042,  1043,  1044,  1045,    -1,
    1047,    -1,  1049,    -1,    -1,    -1,  1053,    -1,  1055,    -1,
    1057,    -1,    -1,    -1,  1061,  1062,    17,   148,   149,    -1,
     151,   152,   153,   154,   155,    26,    -1,   158,    29,    30,
      31,   148,   149,    -1,   151,   152,   153,    -1,  1222,    -1,
       7,    -1,    43,    -1,    11,    12,    13,    14,    -1,    -1,
      17,    -1,    53,    54,    55,    -1,  1103,    -1,  1105,    26,
    1107,  1108,    29,    30,    31,  1112,  1113,    -1,    -1,    36,
    1117,  1118,   952,  1120,    -1,    -1,  1123,    -1,  1125,    -1,
      -1,    -1,   962,    -1,    -1,    -1,    53,    54,    55,    -1,
      -1,    -1,    -1,    -1,  1141,    -1,    -1,   977,    -1,   979,
      -1,    -1,    69,   983,    -1,    -1,    -1,     9,    -1,    -1,
      -1,  1295,    -1,   993,    -1,    17,    -1,    -1,    -1,    -1,
      -1,  1001,    89,    -1,    26,    -1,    -1,    29,    30,    31,
      -1,    -1,    -1,  1180,    -1,  1182,    38,  1184,  1185,  1186,
      -1,  1188,  1189,    -1,  1366,    -1,  1193,  1194,    -1,  1196,
      -1,    53,    54,    55,     7,  1202,  1203,    -1,    11,    12,
      13,    14,     9,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      17,    -1,    -1,    26,    -1,    -1,    29,    30,    31,    26,
    1227,  1228,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,
    1371,    38,    -1,    -1,    -1,    42,    43,  1378,    -1,    -1,
      53,    54,    55,    -1,  1084,    -1,    53,    54,    55,    -1,
      -1,    -1,    -1,    -1,    -1,  1095,    69,    -1,    -1,  1266,
    1267,  1268,  1269,  1270,    -1,    -1,  1273,  1274,  1275,  1276,
    1277,    -1,    -1,  1280,    -1,  1282,    89,  1284,    -1,  1286,
      -1,    -1,    -1,    -1,  1291,    -1,    -1,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,
      -1,    -1,  1142,    -1,    27,  1145,    -1,    -1,    -1,  1149,
      -1,    34,    -1,  1153,    37,    -1,  1323,    -1,    -1,    -1,
      -1,    -1,    -1,  1330,    -1,  1332,    -1,    -1,    -1,    52,
      -1,  1338,    -1,  1340,    -1,    17,    -1,    -1,  1345,    -1,
      63,  1348,    -1,    66,    26,    68,  1353,    29,    30,    31,
      -1,    -1,    -1,    -1,    -1,    -1,    79,    -1,    81,    -1,
      83,    43,    85,    45,    -1,    -1,    -1,  1207,    91,    -1,
      -1,    53,    54,    55,    -1,  1215,    -1,  1217,    -1,    -1,
      -1,    -1,    -1,    -1,  1224,    -1,    -1,    -1,  1395,    -1,
      -1,    45,    -1,  1400,  1401,    -1,    -1,  1404,    52,    53,
      -1,    55,    -1,    -1,    58,    -1,    60,    -1,    62,    -1,
      -1,    -1,    66,    -1,    68,    -1,    70,    -1,    72,    -1,
      74,    -1,    76,    -1,    78,    -1,    80,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    88,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    -1,    97,    -1,    99,    -1,   101,    -1,   103,
      -1,   105,    -1,    -1,    -1,    -1,    -1,  1297,    -1,  1299,
    1300,  1301,  1302,    -1,  1304,  1305,  1306,  1307,  1308,    -1,
    1310,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   147,    -1,    -1,    -1,    -1,   109,   110,
     111,   112,   113,    -1,    -1,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,    -1,   130,
     131,   132,    10,    -1,    -1,    -1,    -1,    -1,    -1,    17,
      18,    -1,    -1,    -1,    -1,    -1,    -1,   191,    26,    27,
      -1,    29,    30,    31,    -1,    -1,    34,   203,    -1,   205,
      -1,   207,    -1,    -1,   210,    -1,    -1,    -1,    46,    -1,
      -1,    -1,    -1,    -1,    52,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    63,    17,    -1,    66,    -1,
      68,   192,    -1,    -1,    -1,    26,   197,    -1,    29,    30,
      31,    79,    -1,    81,   248,    83,    -1,    85,    -1,   253,
      -1,   255,    -1,    91,   258,   259,    -1,    -1,     9,    -1,
      -1,    -1,    53,    54,    55,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    26,   280,   281,    29,    30,
      31,   285,    -1,    -1,   288,    -1,    -1,    38,    -1,    -1,
      -1,   295,    43,    -1,    -1,   299,    -1,    -1,    -1,    10,
     304,    -1,    53,    54,    55,   309,    -1,    18,    -1,    -1,
     314,    -1,    -1,    17,    -1,   319,    27,   321,    -1,    -1,
      -1,   325,    26,    34,    -1,    29,    30,    31,   332,    -1,
      -1,   335,    36,    -1,    -1,    46,    -1,    -1,   344,   343,
      -1,    52,    -1,    -1,    -1,   351,    -1,   353,   354,    53,
      54,    55,    63,    -1,    -1,    66,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,   370,    -1,    -1,    -1,    79,   375,
      81,    -1,    83,    -1,    85,    -1,    -1,    -1,    -1,    -1,
      91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   350,
      -1,    -1,    -1,    -1,   355,   356,    93,    94,    95,    96,
      97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   376,   377,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   385,   429,    -1,   431,    -1,    -1,
      -1,    -1,   436,    -1,    -1,    -1,   397,    -1,    -1,   400,
      -1,    -1,   446,    -1,    -1,    -1,   407,    -1,   454,   453,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   461,   464,    -1,
     317,   318,    -1,   320,    -1,   471,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   479,    -1,    -1,    -1,    -1,
     486,   485,    -1,    -1,    -1,    -1,   492,   493,    -1,    -1,
     496,    -1,    -1,   499,    -1,    -1,    -1,    -1,    -1,    -1,
     506,    -1,    -1,   509,    -1,    -1,    -1,    -1,   514,    -1,
     516,    -1,    -1,    -1,    -1,    -1,   520,    -1,    -1,   525,
     524,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   538,    -1,    -1,    -1,   542,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   557,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   568,    -1,    -1,    -1,    -1,    -1,   529,   530,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   550,
      -1,    -1,    -1,   599,    -1,    -1,    -1,    -1,    -1,   605,
      -1,    -1,    -1,   609,    -1,    -1,   567,    -1,    -1,   615,
      -1,   617,   469,    -1,    -1,   621,    -1,    -1,    -1,    -1,
     477,   627,    -1,    -1,    -1,   482,    -1,   631,   634,    -1,
      -1,    -1,   489,   490,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   606,    -1,    -1,    -1,   610,
     507,   657,    -1,    -1,   660,   661,   662,   663,    -1,    -1,
      -1,    -1,    -1,    -1,   668,    -1,    -1,    -1,   674,   673,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   683,    -1,   685,
      -1,    -1,   688,    -1,   690,    -1,   692,    -1,    -1,    -1,
      -1,    -1,   698,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   716,    -1,   718,    -1,    -1,   721,    -1,   723,
      -1,   725,    -1,   727,    -1,    -1,   730,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   738,    -1,    -1,   741,    -1,   743,
      -1,    -1,    -1,    -1,   748,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   770,    -1,   772,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   787,    -1,   789,    -1,    -1,    -1,    -1,    -1,    -1,
     796,    -1,   798,    -1,   800,    -1,   802,    -1,    -1,    -1,
     761,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   666,
     667,    -1,    -1,    -1,    -1,   672,    -1,    -1,    -1,    -1,
     781,    -1,   783,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     687,   837,   689,    -1,   691,    -1,   693,    -1,    -1,   845,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   820,
      -1,    -1,   823,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   832,    -1,    -1,    -1,    -1,    -1,   838,    -1,    -1,
     841,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   905,
      -1,    -1,    -1,    -1,   910,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   922,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   928,    -1,    -1,   933,    -1,    -1,
      -1,    -1,    -1,    -1,   940,    -1,    -1,   943,   944,   945,
     946,   947,   948,    -1,    -1,   951,    -1,   953,    -1,    -1,
     956,    -1,    -1,    -1,    -1,   961,    -1,   963,   964,   965,
      -1,    -1,    -1,    -1,    -1,    -1,   972,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1002,    -1,    -1,    -1,
      -1,   858,   859,   860,   861,    -1,   863,   864,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   873,    -1,   875,   876,
      -1,    -1,    -1,    -1,  1028,    -1,   883,   884,   989,    -1,
      -1,  1037,    -1,    -1,    -1,    -1,    -1,   998,    -1,   896,
      -1,    -1,    -1,    -1,    -1,    -1,  1052,    -1,  1054,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1060,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1071,  1072,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1091,    -1,    -1,    -1,    -1,
    1096,  1097,    -1,    -1,  1098,    -1,    -1,    -1,    -1,    -1,
    1104,    -1,  1106,    -1,    -1,  1109,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1075,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1137,    -1,    -1,    -1,    -1,  1099,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1012,    -1,    -1,  1164,    -1,
    1017,  1018,    -1,  1020,    -1,    -1,    -1,    -1,    -1,  1026,
      -1,    -1,  1029,  1177,    -1,  1179,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1144,    -1,    -1,  1147,    -1,    -1,    -1,
    1151,  1195,    -1,    -1,    -1,  1201,    -1,    -1,    -1,    -1,
      -1,    -1,  1163,    -1,    -1,    -1,    -1,    -1,  1214,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1223,    -1,  1225,
    1226,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1205,  1206,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1219,    -1,
      -1,    -1,    -1,    -1,    -1,  1271,    -1,    -1,    -1,    -1,
      -1,    -1,  1278,    -1,    -1,    -1,    -1,  1134,  1135,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1292,    -1,
    1296,    -1,  1298,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1313,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1324,    -1,    -1,    -1,    -1,    -1,    -1,  1331,    -1,    -1,
      -1,    -1,    -1,  1190,    -1,  1339,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1204,    -1,    -1,
      -1,    -1,    -1,    -1,  1360,  1361,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     4,     5,    -1,    -1,    -1,    -1,    10,
      -1,    -1,    -1,    -1,    15,    -1,    17,    18,    -1,    -1,
      -1,    -1,    -1,    24,    -1,    -1,    27,    -1,    -1,    -1,
      -1,    -1,  1398,    34,    35,    -1,    37,    -1,    39,  1405,
      -1,    -1,    -1,    44,    -1,    46,    -1,    -1,    49,    -1,
      -1,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,
      -1,  1427,    63,    -1,    65,    66,    67,    68,    -1,    -1,
      -1,  1288,    -1,    -1,    -1,    76,    77,    -1,    79,    80,
      81,    -1,    83,    84,    85,    86,    87,    88,    -1,    90,
      91,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1317,    -1,    -1,  1320,  1321,    -1,  1323,    -1,    -1,    -1,
    1327,  1328,    -1,  1330,    -1,  1332,    -1,    -1,  1335,  1336,
      -1,  1338,    -1,  1340,    -1,  1342,  1343,    -1,  1345,  1346,
      -1,  1348,  1349,    -1,  1351,    -1,  1353,  1354,    -1,  1356,
      -1,  1358,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1397,    -1,  1399,  1400,    -1,  1402,  1403,  1404,    -1,  1406,
    1407,  1408,  1409,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1426,
      -1,  1428,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   108,   109,   110,   111,   112,   113,   114,    -1,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,    -1,    -1,   142,   143,   144,   145,
     146
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     4,     5,    10,    15,    17,    18,    24,    27,    34,
      35,    37,    39,    44,    46,    49,    52,    60,    63,    65,
      66,    67,    68,    76,    77,    79,    80,    81,    83,    84,
      85,    86,    87,    88,    90,    91,    92,   165,   166,   167,
     238,   240,   306,   307,   310,   312,   333,   334,   335,   336,
     337,   338,   341,   342,   344,   346,   354,   355,   356,   362,
     363,   374,   376,   387,   388,   389,   397,   398,   399,   400,
     401,   402,   403,   404,   405,   406,   407,   408,   409,   410,
     412,   434,   435,   436,   438,   440,   441,   447,   448,   453,
     458,   461,   464,   467,   468,   471,   476,   478,   481,   485,
     498,   502,   503,   508,   510,   511,     0,   159,   160,   202,
     202,   202,   202,   202,   163,   204,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   147,   205,   241,   205,   313,   205,
     340,   340,   205,   347,     6,    82,   350,   351,   205,   357,
     340,   205,   377,   340,   357,   340,   357,   340,   357,   340,
     357,   205,   449,   205,   472,   335,   336,   337,   338,   362,
     388,   389,   400,   404,   408,   434,   447,   453,   458,   461,
     464,   467,   477,   479,   480,   205,   486,   486,   486,    40,
     490,   491,   204,   204,   204,   204,   204,   202,   204,   204,
     204,   204,   204,   358,   204,   358,   204,   358,   204,   204,
     411,   204,   204,   161,   162,   203,   204,   204,   482,   483,
     204,   499,   500,   204,   504,   505,   204,   202,   148,   149,
     151,   152,   153,   154,   155,   168,   169,   170,   172,   174,
     175,   178,   180,   181,   205,   242,    61,   314,   316,    19,
      28,    56,   307,   312,   323,   324,   343,   355,   370,   371,
     437,   439,   442,   444,   445,   446,   323,   343,   437,   439,
     172,   348,   205,   352,   344,   364,   365,   367,   368,   369,
     370,   371,     3,    25,   378,   379,    70,   310,   312,   323,
     390,   398,   454,   455,   456,   457,    23,   238,   359,   360,
     203,    71,   402,   459,   460,   203,    72,   406,   462,   463,
     203,    73,   410,   465,   466,    50,    51,   238,   413,   414,
     416,   417,   203,    64,   450,   451,   468,   469,   510,    74,
      75,   473,   474,    78,   487,   488,   490,   487,   490,   487,
     490,    41,   492,   493,   167,   180,   150,   168,   179,   172,
     204,   239,   204,   308,   311,   202,   202,   204,   202,   204,
     202,   202,   204,   204,   204,   204,   204,   204,   204,   204,
     358,   204,   202,   202,   204,   375,   202,   202,   204,   204,
     204,   202,   204,   259,   204,   202,   204,   204,   202,   204,
     204,   202,   204,   204,   202,   259,   259,   202,   204,   259,
     202,   204,   202,   204,   202,   204,   483,   202,   204,   204,
     500,   204,   204,   505,   204,   204,   202,   203,   173,   176,
     177,   181,   180,    32,    33,   169,   205,   243,   244,   245,
     247,   248,   203,    62,   314,   319,   320,   344,   203,   203,
     205,   317,   313,   355,    57,   325,   326,    16,   268,   269,
     271,   273,   274,   276,   339,   350,   205,   372,   372,    48,
     419,   421,   419,   355,   339,   419,   419,   172,   349,   171,
     173,   353,   203,   374,   372,   372,     9,   238,   381,   383,
     203,   205,   380,   313,   398,   276,   391,   419,   372,   238,
     238,   205,   361,   268,   419,   372,   268,   419,   372,   268,
     419,   372,   180,   184,   186,   205,   415,   413,   205,   418,
     419,   372,   469,   180,   475,   205,   489,   492,    25,   423,
     424,   492,    47,   512,   516,   167,   180,   204,   204,   202,
     202,   308,   311,   204,   309,   202,   204,   204,   315,   204,
     204,   204,   328,   202,   202,   203,   204,   204,   204,   204,
     202,   204,   204,   203,   204,   204,   204,   345,   204,   259,
     172,   203,   204,   204,   204,   259,   375,   202,   259,   204,
     204,   202,   203,   204,   204,   259,   259,   203,   203,   204,
     204,   203,   204,   204,   203,   204,   204,   155,   157,   183,
     185,   189,   192,   203,   259,   203,   204,   204,   204,   470,
     168,   203,   203,   204,   204,   501,   202,   204,   204,   207,
     202,   203,   244,   247,   205,   246,   205,   249,   238,   322,
     169,   321,   319,   238,   318,   203,   314,   339,    58,    59,
     329,   331,   203,   180,   327,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   277,
     278,   279,   268,   323,   343,   323,   343,   268,   205,   422,
     268,   339,   268,   268,     7,    11,   238,   260,   264,   203,
       7,    12,   258,   263,   268,   323,   343,   323,   343,   205,
     384,   203,   314,   391,   277,   268,   323,   398,   268,   402,
     268,   406,   268,   410,   180,   187,   180,   193,   268,   442,
     444,   445,   446,   452,    26,    29,    30,    31,    53,    54,
      55,   208,   210,   211,   212,   213,   214,   215,   216,   219,
     220,   221,   223,   224,   229,   231,   234,   235,   238,   250,
     251,   469,   203,   180,   423,    38,    43,   208,   383,   426,
     431,   432,   496,   497,   203,   205,   425,    45,   509,   208,
     203,   486,   204,   203,   203,   309,   203,   309,   315,   204,
     203,   202,   168,   203,   204,   204,   204,   204,   204,   204,
     443,   204,   443,   203,   204,   203,   203,   203,   203,   259,
     259,   202,   259,   202,   203,   204,   204,   366,   204,   366,
     204,   204,   203,   204,   203,   204,   259,   203,   259,   203,
     259,   203,   259,   185,   183,   156,   158,   181,   182,   190,
     193,   198,   199,   200,   203,   204,   204,   202,   204,   209,
     202,   204,   209,   202,   204,   209,   202,   204,   209,   202,
     204,   209,   202,   209,   202,   470,   204,   484,   202,   501,
     501,   202,   207,   202,   204,   330,   202,   204,   513,   514,
     247,   314,   205,   332,   180,   151,   152,   153,   280,   280,
     280,   280,   355,   238,   350,   203,   355,   203,   423,   205,
     266,   266,   355,   238,   258,   350,   373,   203,   355,   203,
     171,   385,   314,   280,   398,   203,   203,   203,   203,   180,
     188,   193,   180,   194,   182,   201,   374,   212,   238,   250,
     215,   219,   238,   250,   205,   217,   223,   229,   234,   205,
     222,   229,   234,   169,   225,   234,   169,   232,   184,   205,
     236,   147,   206,    42,   208,   426,   431,   494,   495,   496,
     203,   384,   384,   321,   238,   203,   374,   419,   481,   502,
     506,   423,   487,   330,   259,   259,   259,   259,   443,   259,
     259,   443,   204,   420,   204,   204,   366,   259,   204,   259,
     259,   366,   204,   382,   259,   259,   185,   201,   182,   191,
     198,   194,   259,   209,   204,   209,   203,   204,   209,   204,
     209,   209,   203,   204,   209,   209,   204,   209,   204,   204,
     203,   484,   484,   204,   207,   202,   207,   204,   204,   203,
     203,   204,   515,   204,   514,   203,   203,   203,   203,   203,
     203,   203,   238,   426,   431,   203,   172,   267,   267,   203,
     373,   203,     7,    11,    12,    13,   238,   256,   257,   386,
     203,   203,   203,   180,   195,   196,   203,   218,   220,   223,
     229,   234,   229,   234,   234,   234,   169,   226,   169,   233,
     184,   205,   237,   496,   167,   385,   205,   433,   204,   208,
     383,   431,   517,   203,   204,   204,   204,   204,   259,   420,
     420,   259,   259,   259,   259,   202,   259,   204,   168,   198,
     203,   204,   204,   204,   204,   209,   209,   209,   209,   204,
     204,   230,   203,   207,   203,   204,   427,   330,   509,   202,
     515,   515,     8,   282,   284,   281,   284,   282,   283,   284,
     203,   203,   266,   281,   180,   197,   198,   223,   229,   234,
     229,   234,   234,   234,   169,   227,   260,   203,     7,    11,
      12,    13,    14,    69,   238,   428,   429,   430,   203,   203,
     202,   384,   204,   272,   202,   204,   270,   202,   207,   204,
     275,   202,   204,   204,   392,   198,   204,   204,   204,   209,
     204,   259,   259,   202,   506,   204,     7,    11,    12,    13,
      14,    69,    89,   208,   252,   253,   254,   255,   261,   265,
     305,   205,   285,   208,   281,   305,   285,   208,   283,   285,
     267,    36,   208,   305,   394,   395,   229,   234,   234,   234,
     169,   228,   266,   203,   385,   202,   202,   204,   204,   270,
     207,   204,   275,   204,   259,   204,   207,   204,   393,   202,
     204,   203,   204,   262,   204,   507,   259,   266,   266,   208,
     108,   109,   110,   111,   112,   113,   114,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   142,   143,   144,   145,   146,   298,   107,   115,   140,
     141,   286,   289,   298,   107,   115,   140,   141,   291,   294,
     298,   203,   394,   208,   394,   205,   396,   234,   267,   203,
     208,   496,   509,   203,   203,   204,   262,   204,   296,   204,
     204,   204,   204,   203,   204,   204,   204,   204,   204,   203,
     204,   393,   393,   330,   259,   207,   202,   267,   203,    22,
     238,   261,   297,   303,   304,   203,    20,   238,   253,   287,
     299,   300,   303,   287,    21,   238,   253,   288,   301,   302,
     303,   288,   238,   253,   290,   303,   238,   292,   299,   303,
     287,   238,   293,   301,   303,   293,   238,   295,   303,   203,
     506,   259,   259,   259,   204,   259,   202,   259,   259,   204,
     259,   202,   204,   259,   259,   259,   204,   259,   202,   204,
     259,   259,   259,   204,   259,   259,   204,   259,   259,   259,
     204,   259,   259,   259,   259,   203,   203,   261,   180,   253,
     303,   169,   253,   253,   303,   169,   253,   253,   303,   303,
     507,   259,   203,   259,   204,   259,   204,   259,   259,   204,
     259,   203,   259,   259,   259,   259,   253,   258,   253,   259,
     203,   259
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int16 yyr1[] =
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
     381,   382,   382,   382,   383,   384,   385,   386,   387,   387,
     388,   389,   390,   390,   391,   392,   392,   392,   392,   392,
     393,   393,   393,   394,   395,   396,   397,   397,   398,   399,
     399,   399,   400,   401,   401,   402,   403,   403,   404,   405,
     406,   407,   407,   408,   409,   410,   411,   411,   411,   411,
     411,   412,   412,   413,   414,   415,   415,   416,   417,   418,
     419,   420,   420,   420,   420,   421,   422,   423,   424,   425,
     426,   427,   427,   427,   428,   429,   430,   430,   430,   430,
     430,   430,   431,   432,   433,   434,   434,   434,   435,   435,
     436,   437,   437,   438,   439,   439,   440,   441,   442,   443,
     443,   443,   444,   445,   446,   447,   448,   449,   450,   451,
     452,   452,   452,   452,   453,   454,   454,   455,   456,   457,
     458,   459,   460,   461,   462,   463,   464,   465,   466,   467,
     468,   468,   468,   468,   468,   468,   468,   468,   468,   468,
     468,   468,   469,   469,   470,   470,   470,   471,   472,   473,
     474,   474,   475,   475,   475,   476,   477,   477,   478,   479,
     479,   479,   479,   479,   479,   479,   479,   479,   479,   479,
     479,   479,   479,   480,   480,   480,   480,   480,   480,   480,
     481,   482,   482,   483,   484,   484,   484,   484,   484,   484,
     484,   485,   486,   487,   488,   489,   490,   491,   492,   493,
     494,   495,   496,   497,   498,   499,   499,   500,   501,   501,
     501,   501,   501,   502,   503,   504,   504,   505,   506,   506,
     506,   506,   507,   507,   507,   507,   508,   509,   510,   511,
     512,   513,   513,   514,   515,   515,   515,   515,   516,   517
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
       7,     0,     3,     3,     1,     5,     1,     1,     1,     1,
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
       4,     3,     1,     4,     0,     3,     3,     2,     1,     7
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
