/****************************************************************************************

    FFIO - Flexible format I/O

    FFIO provides functionality for reading proj style instruction strings written
    in a less strict format than usual:

    *  Whitespace is generally allowed everywhere
    *  Comments can be written inline, '#' style
    *  ... or as free format blocks

    The overall mission of FFIO is to facilitate communications of geodetic
    parameters and test material in a format that is highly human readable,
    and provides ample room for comment, documentation, and test material.

    See the PROJ ".gie" test suites for examples of supported formatting.

****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <ctype.h>

#include <math.h>
#include <errno.h>

#include "proj_internal.h"
#include "proj_api.h"


static const char *gie_tags[] = {
    "<gie>", "operation", "accept", "expect", "roundtrip", "banner", "verbose",
    "direction", "tolerance", "builtins", "echo", "</gie>"
};

static const size_t n_gie_tags = sizeof gie_tags / sizeof gie_tags[0];

typedef struct {
    FILE *f;
    const char **tags;
    const char *tag;
    char *args;
    char *next_args;
    size_t n_tags;
    size_t args_size;
    size_t next_args_size;
    size_t argc;
    size_t lineno, next_lineno;
    size_t level;
}  ffio;


/***************************************************************************************/
static ffio *ffio_create (const char **tags, size_t n_tags, size_t max_record_size) {
/****************************************************************************************

****************************************************************************************/
    ffio *F = calloc (1, sizeof (ffio));
    if (0==F)
        return 0;

    if (0==max_record_size)
        max_record_size = 1000;

    F->args = calloc (1, 5*max_record_size);
    if (0==F->args) {
        free (F);
        return 0;
    }

    F->next_args = calloc (1, max_record_size);
    if (0==F->args) {
        free (F->args);
        free (F);
        return 0;
    }

    F->args_size = 5*max_record_size;
    F->next_args_size = max_record_size;

    F->tags = tags;
    F->n_tags = n_tags;
    return F;
}


/***************************************************************************************/
static ffio *ffio_destroy (ffio *F) {
/****************************************************************************************

****************************************************************************************/
    free (F->args);
    free (F->next_args);
    free (F);
    return 0;
}

#if 0
/***************************************************************************************/
static char *chomp (char *c) {
/****************************************************************************************
    Strip pre- and postfix whitespace. Note: Inline comments (indicated by '#')
    are considered whitespace.
****************************************************************************************/
    int i, n;
    char *comment;
    if (0==c)
        return 0;
    comment = strchr (c, '#');
    if (comment)
        *comment = 0;
    n = strlen (c);
    for (i = n - 1; isspace (c[i]) || ';'==c[i]; i--)
        c[i] = 0;
    for (n = 0; isspace (c[n]); n++);
    for (i = 0;  0 != c[i + n];  i++)
        c[i] = c[i + n];
    c[i] = 0;
    return c;
}


/***************************************************************************************/
static char *shrink (char *c) {
/****************************************************************************************
    Clash repeated whitespace, remove '+' and ';', make ',' and '=' greedy, eating
    their surrounding whitespace.
****************************************************************************************/
    int i, j, n, ws;

    chomp (c);
    n = strlen (c);

    /* First clash repeated whitespace (including +/;) */

    for (i = j = ws = 0; j < n; j++) {

        /* Blank out prefix '+', only if *preceeded* by whitespace, i.e. keep it in 1.23e+08    */
        /* (strictly, in that case, we could just throw it away, but we still need to avoid     */
        /* having it work as delimiter, turning 1.23e+08 into the two tokens  '1.23e' and '08') */
        if ((j==0) && ('+'==c[j]))
            continue;
        if (('+'==c[j]) && isspace (c[j - 1])) {
            c[i++] = ' ';
            continue;
        }
        if (isspace (c[j]) || ';'==c[j] || '\\'==c[j]) {
            if (0==ws && (i > 0))
                c[i++] = ' ';
            ws = 1;
            continue;
        }
        else
            ws = 0;
        c[i++] = c[j];
    }
    c[i] = 0;
    n = strlen(c);


    /* Then make ',' and '=' greedy (eating surrounding whitespace) */

    for (i = j = 0; j < n; j++) {
        if (i==0) {
            c[i++] = c[j];
            continue;
        }

        /* 1. skip space before '=' and ',' */
        if ('='==c[j] || ','==c[j]) {
            if (c[i - 1]==' ')
               c[i - 1] = c[j];
            else
                c[i++] = c[j];
            continue;
        }

        /* 2. skip space after '=' and ','  */
        /* Note: we have already replaced repeated whitespace with a single ' ', so */
        /* we do not need to call isspace here, neither to loop over multiple ' '   */
        if (' '==c[j] && ('='==c[i - 1] || ','==c[i - 1]) )
            continue;

        c[i++] = c[j];
    }
    c[i] = 0;
    return c;
}
#endif



/***************************************************************************************/
static int at_decorative_element (ffio *F) {
/****************************************************************************************
    A decorative element consists of a line of at least 5 consecutive identical chars,
    starting at buffer position 0:
    "-----", "=====", "*****", etc.

    A decorative element serves as a end delimiter for the current element, and
    continues until a gie command verb is found at the start of a line
****************************************************************************************/
    int i;
    char *c;
    if (0==F)
        return 0;
    c = F->next_args;
    if (0==c)
        return 0;
    if (0==c[0])
        return 0;
    for (i = 1; i < 5; i++)
        if (c[i]!=c[0])
            return 0;
    return 1;
}


/***************************************************************************************/
static const char *at_tag (ffio *F) {
/****************************************************************************************
    A start of a new command serves as an end delimiter for the current command
****************************************************************************************/
    size_t j;
    for (j = 0;  j < F->n_tags;  j++)
        if (strncmp (F->next_args, F->tags[j], strlen(F->tags[j]))==0)
            return F->tags[j];
    return 0;
}


/***************************************************************************************/
static int at_end_delimiter (ffio *F) {
/****************************************************************************************
    An instruction consists of everything from its introductory tag to its end
    delimiter.  An end delimiter can be either the introductory tag of the next
    instruction, or a "decorative element", i.e. one of the "ascii art" style
    block delimiters typically used to mark up block comments in a free format
    file.
****************************************************************************************/
    if (F==0)
        return 0;
    if (at_decorative_element (F))
        return 1;
    if (at_tag (F))
        return 1;
    return 0;
}


/***************************************************************************************/
static int nextline (ffio *F) {
/****************************************************************************************
    Read next line of input file
****************************************************************************************/
    F->next_args[0] = 0;
    if (0==fgets (F->next_args, F->next_args_size - 1, F->f))
        return 0;
    if (feof (F->f))
        return 0;
    pj_chomp (F->next_args);
    F->next_lineno++;
    return 1;
}

/***************************************************************************************/
static int locate_tag (ffio *F, const char *tag) {
/****************************************************************************************
    find start-of-line tag
****************************************************************************************/
    size_t n = strlen (tag);
    while (0!=strncmp (tag, F->next_args, n))
        if (0==nextline (F))
            return 0;
    return 1;
}


/***************************************************************************************/
static int step_into_gie_block (ffio *F) {
/****************************************************************************************
    Make sure we're inside a <gie>-block
****************************************************************************************/
    /* Already inside */
    if (F->level % 2)
        return 1;

    if (0==locate_tag (F, "<gie>"))
        return 0;

    while (0!=strncmp ("<gie>", F->next_args, 5)) {
        printf ("skipping [%s]\n", F->next_args);
        F->next_args[0] = 0;
        if (feof (F->f))
            return 0;
        if (0==fgets (F->next_args, F->next_args_size - 1, F->f))
            return 0;
        pj_chomp (F->next_args);
        F->next_lineno++;
    }
    F->level++;

    /* We're ready at the start - now step into the block */
    return nextline (F);
}


/***************************************************************************************/
static int skip_to_next_tag (ffio *F) {
/****************************************************************************************
    Skip forward to the next command tag
****************************************************************************************/
    const char *c;
    if (0==step_into_gie_block (F))
        return 0;

    c = at_tag (F);

    /* If not already there - get there */
    while (!c) {
        if (0==nextline (F))
            return 0;
        c = at_tag (F);
    }

    /* If we reached the end of a <gie> block, locate the next and retry */
    if (0==strcmp (c, "</gie>")) {
        F->level++;
        if (feof (F->f))
            return 0;
        if (0==step_into_gie_block (F))
            return 0;
        F->args[0] = 0;
        return skip_to_next_tag (F);
    }
    F->lineno = F->next_lineno;

    return 1;
}


static int append_args (ffio *F) {
    size_t skip_chars = 0;
    size_t next_len = strlen (F->next_args);
    size_t args_len = strlen (F->args);
    const char *tag = at_tag (F);

    if (tag)
        skip_chars = strlen (tag);

    if (F->args_size < args_len + next_len - skip_chars + 1) {
        void *p = realloc (F->args, 2 * F->args_size);
        if (0==p)
            return 0;
        F->args = p;
        F->args_size = 2 * F->args_size;
    }

    F->args[args_len] = ' ';
    strcpy (F->args + args_len + 1,  F->next_args + skip_chars);

    F->next_args[0] = 0;
    return 1;
}





/***************************************************************************************/
static int get_inp (ffio *F) {
/****************************************************************************************
    The primary command reader for gie. May be useful in the init-file reader as well.
****************************************************************************************/
    F->args[0] = 0;

    if (0==skip_to_next_tag (F))
        return 0;
    F->tag = at_tag (F);

    if (0==F->tag)
        return 0;

    do {
        append_args (F);
        if (0==nextline (F))
            return 0;
    } while (!at_end_delimiter (F));

    pj_shrink (F->args);
    return 1;
}




char *pj_expand_init_id (PJ_CONTEXT *ctx, const char *id) {
    char *dup = pj_calloc (1, strlen (id) + 1), fname;
    char *section;
    const char *tags[2];
    size_t n_tags = 2;
    const char *endtag = {"<"};
    char *starttag;

    PAFile *fid;
    char linebuf[1001];
    char *buf;

    if (0==dup)
        return 0;
    strcpy (dup, id);
    fname = strstr (dup, "init=");
    if (0==fname)
        fname = dup;
    else
        fname += 5;

    section = strrchr(fname, ':');
    if (0==section) {
        proj_errno (ctx, PJD_ERR_NO_COLON_IN_INIT_STRING);
        return pj_dealloc (dup);
    }

    *section = '\0';
    section++;

    starttag = pj_calloc (1, strlen (section) + 3);
    if (0==starttag)
        return pj_dealloc (dup);
    starttag[0] = '<';
    strcpy (starttag + 1, section);
    starttag[strlen(starttag)] = '>';

    tags[0] = starttag;

    fid = pj_open_lib(ctx, id, "rt");
    if (0==fid)
        return 0;

    pj_ctx_fclose(ctx, fid);

    return 1;
}




#if 0
int main (void) {
    ffio *F = ffio_create (gie_tags, n_gie_tags, 1000);
    F->f = stdin;

    while (get_inp (F))
        printf ("\ntag:  %s\ndef:  %s\n", F->tag, shrink(F->args));
    return 0;
}



    if (o) {
        char *p = get_inp_from_argc_argv (o->pargc, (const char **) o->pargv);
        printf ("free format: %d\n", o->free_format);
        printf ("-: %2.2d  %s\n", o->margc, o->margc? o->margv[0]: "(empty)");
        printf ("+: %2.2d  %s\n", o->pargc, o->pargc? o->pargv[0]: "(empty)");
        printf ("f: %2.2d  %s\n", o->fargc, o->fargc? o->fargv[0]: "(empty)");
        printf ("pstring: [%s]\n", p);
        return 0;
    }

#endif
