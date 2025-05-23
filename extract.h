#ifndef EXTRACT_H
#define EXTRACT_H


typedef struct _Extract
{
   char *source_fname;
   FILE *fptr_source_fname;
}extract;


/* Status will be used in fn. return type */
typedef enum
{
    e_failure,
    e_success
} Status;

#define RESET       "\x1b[0m"
#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[34m"
#define MAGENTA     "\x1b[35m"
#define CYAN        "\x1b[36m"


Status do_extract(extract *ext);

Status open_files(extract *ext);

Status extract_main(extract *ext);

#endif