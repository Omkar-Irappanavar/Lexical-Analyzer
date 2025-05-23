/*
    Name: Omkar Irappanavar
    Date: 15/10/2024
    Description: Lexical Analyzer Project
*/

#include <stdio.h>
#include "extract.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

FILE *output_fptr;

Status do_extract(extract *ext)
{
    if (open_files(ext) == e_failure)
    {
        printf("Open file failure\n");
        return e_failure;
    }
    // printf("Open file Success\n");

    extract_main(ext);
    
    return e_success;
}

Status open_files(extract *ext)
{
    // output_fptr = fopen("output.s", "w");
    output_fptr = fopen("output.c", "w");

    if(output_fptr==NULL)
    {
            printf("Error: File not created\n");
            return e_failure;
    }
    ext->fptr_source_fname = fopen(ext->source_fname, "r");
    if (ext->fptr_source_fname == NULL)
    {
        printf("Source file not opened\n");
        return e_failure;
    }

    return e_success;
}

/* function to check keywords */
int is_keyword(const char *word)
{
    const char *keywords[] = {
        "auto", "break", "case", "char", "const", "continue", "default",
        "do", "double", "else", "enum", "extern", "float", "for",
        "goto", "if", "int", "long", "register", "return", "short",
        "signed", "sizeof", "static", "struct", "switch", "typedef",
        "union", "unsigned", "void", "volatile", "while"};

    for (int i = 0; i < 32; i++)
    {
        if (strcmp(word, keywords[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}

// /* Function to print the type of token */
void print_token(const char *token, const char *type)
{
    printf("%s : %s\n", type, token);
}

/* Function to extract the sample code */
Status extract_main(extract *ext)
{
    char ch, backupch;
    char buffer[100];
    int i = 0, j = 0;
    static int floatflag = 0;
    static int ppd_flag = 0, comment_flag = 0, curlyb_flag = 0, paranthesis_flag = 0, sqbracket_flag = 0, squote_flag = 0, dquote_flag = 0; // ppd_flag => #, curlyb_flag => {}, sflagp => (), sqbracket_flag => [], squote_flag => '', dquote_flag => "",

    fscanf(ext->fptr_source_fname, "%[^\n]\n", buffer);
    int space=0, newline=0;

    while ((ch = fgetc(ext->fptr_source_fname)) != EOF)
    {
        // if ((ch = fgetc(ext->fptr_source_fname)) == '#')
        // {
        //     char buffer[20];
        //     while (ch != '\n')
        //     {
        //         buffer[j++] = ch;
        //         ch = fgetc(ext->fptr_source_fname);
        //     }
        //     buffer[j] = '\0';
        //     printf(BLUE "Preprocessing Directive : %s\n" RESET, buffer);
        // }
        // else
        // {
        //     fseek(ext->fptr_source_fname, -1, SEEK_CUR);
        // }

        if (isspace(ch) && ch != '\n')
        {
            if(space==0)
                fprintf(output_fptr, "%c", ch);
            space++;
            // if(backupch!=ch)
            newline = 0;
            continue;
        }
        if (ch == '\n')
        {
            if(newline==0)
                fprintf(output_fptr, "%c", ch);
            newline++;
            if (paranthesis_flag > 0)
            {
                printf(RED "ERROR: ) missing\n");
                exit(0);
            }
            else if (paranthesis_flag < 0)
            {
                printf(RED "ERROR: ( missing\n");
                exit(0);
            }
            paranthesis_flag = 0;

            if (sqbracket_flag > 0)
            {
                printf(RED "ERROR: ] missing\n");
                exit(0);
            }
            else if (sqbracket_flag < 0)
            {
                printf(RED "ERROR: [ missing\n");
                exit(0);
            }
            sqbracket_flag = 0;

            if (squote_flag != 0 || dquote_flag != 0)
            {
                buffer[i] = '\0';
                printf(RED "ERROR: %s is string error\n" RESET, buffer);
                exit(0);
                squote_flag = 0, dquote_flag = 0;
            }
        }
        space=0;
        if(ch!='\n')
            newline=0;
        /* Handle identifers and keywords */
        if (isalpha(ch) || ch == '_')
        {
            buffer[i++] = ch;
            while (isalnum(ch = fgetc(ext->fptr_source_fname)) || ch == '_')
            {
                buffer[i++] = ch;
            }
            buffer[i] = '\0';
            if (ch != EOF)
            {
                fseek(ext->fptr_source_fname, -1, SEEK_CUR); // Put the last character back to the stream
            }

            // if( strcmp(buffer, "float") == 0)
            // {
            //     floatflag=1;
            // }

            if (is_keyword(buffer))
            {
                printf(YELLOW "KEYWORD : " RESET);
                printf("%s\n", buffer);
                fprintf(output_fptr, "%s", buffer);
            }
            else
            {
                printf(RED "IDENTIFIER : " RESET);
                printf("%s\n", buffer);
                fprintf(output_fptr, "%s", buffer);
            }
            i = 0;
            continue;
        }

        /* Handle numbers */ /* Edit st*/
        if (isdigit(ch))
        {
            if (ch == '0')
            {
                buffer[i++] = ch;
                ch = fgetc(ext->fptr_source_fname);
                if (ch == 'b') // binary number
                {
                    buffer[i++] = ch;
                    int flag = 1;
                    ch = fgetc(ext->fptr_source_fname);
                    while (flag)
                    {
                        if (ch == '0' || ch == '1')
                        {
                            buffer[i++] = ch;
                            if (!isalnum(ch = fgetc(ext->fptr_source_fname)))
                            {
                                buffer[i] = '\0';
                                fseek(ext->fptr_source_fname, -1, SEEK_CUR);
                                flag = 0;
                            }
                        }
                        else
                        {
                            buffer[i++] = ch;
                            buffer[i] = '\0';
                            printf(RED "ERROR: %s is Invalid binary number\n" RESET, buffer);
                            // flag=0;
                            exit(0);
                        }
                    }
                }
                else if (ch == 'x' || ch == 'X') // Hexa number
                {
                    buffer[i++] = ch;
                    ch = fgetc(ext->fptr_source_fname);
                    int flag = 1;
                    while (flag)
                    {
                        if ((ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))
                        {
                            buffer[i++] = ch;
                            if (!isalnum(ch = fgetc(ext->fptr_source_fname)))
                            {
                                buffer[i] = '\0';
                                fseek(ext->fptr_source_fname, -1, SEEK_CUR);
                                flag = 0;
                            }
                        }
                        else
                        {
                            buffer[i++] = ch;
                            buffer[i] = '\0';
                            printf(RED "ERROR: %s is Invalid Hexa number\n" RESET, buffer);
                            // flag=0;
                            exit(0);
                        }
                    }
                }
                else if (!isalnum(ch))
                {
                    buffer[i] = '\0';
                    fseek(ext->fptr_source_fname, -1, SEEK_CUR);
                }
                else // octal number
                {
                    int flag = 1;
                    while (flag)
                    {
                        if (ch >= '0' && ch <= '7')
                        {
                            buffer[i++] = ch;
                            if (!isalnum(ch = fgetc(ext->fptr_source_fname)))
                            {
                                buffer[i] = '\0';
                                fseek(ext->fptr_source_fname, -1, SEEK_CUR);
                                flag = 0;
                            }
                        }
                        else
                        {
                            buffer[i++] = ch;
                            buffer[i] = '\0';
                            printf(RED "ERROR: %s is Invalid Octal number\n" RESET, buffer);
                            // flag=0;
                            exit(0);
                        }
                    }
                }
            }
            else
            {
                buffer[i++] = ch;
                int flag = 1;
                ch = fgetc(ext->fptr_source_fname);
                while (flag)
                {
                    if ((ch >= '0' && ch <= '9') || (ch == '.'))
                    {
                        buffer[i++] = ch;
                        if ((!isalnum(ch = fgetc(ext->fptr_source_fname))) && ch != '.')
                        {
                            buffer[i] = '\0';
                            fseek(ext->fptr_source_fname, -1, SEEK_CUR);
                            flag = 0;
                        }
                    }
                    else if (!isalnum(ch))
                    {
                        buffer[i] = '\0';
                        fseek(ext->fptr_source_fname, -1, SEEK_CUR);
                        flag = 0;
                    }
                    else if (ch == 'f')
                    {
                        char temp = fgetc(ext->fptr_source_fname);
                        if (!isalnum(temp))
                        {
                            buffer[i++] = ch;
                            buffer[i] = '\0';
                            fseek(ext->fptr_source_fname, -1, SEEK_CUR);
                            flag = 0;
                        }
                        else
                        {
                            buffer[i++] = ch;
                            int flag = 1;
                            while (flag)
                            {
                                if (isalnum(temp))
                                {
                                    buffer[i++] = temp;
                                    temp = fgetc(ext->fptr_source_fname);
                                }
                                else
                                {
                                    buffer[i] = '\0';
                                    fseek(ext->fptr_source_fname, -1, SEEK_CUR);
                                    printf(RED "ERROR: %s Invalid float number\n" RESET, buffer);
                                    exit(0);
                                }
                            }
                        }
                    }
                    else
                    {
                        buffer[i++] = ch;
                        buffer[i] = '\0';
                        printf(RED "ERROR: %s is Invalid Number\n" RESET, buffer);
                        // flag=0;
                        exit(0);
                    }
                }
            }

            printf("NUMERIC CONST : ");
            printf("%s\n", buffer);
            fprintf(output_fptr, "%s", buffer);
            i = 0;
            continue;
        }

        /* Handle special characters and operators */

        // if(ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == ';' || ch == ',' || ch == '+' || ch == '-' || ch == '*' || ch == '/')
        if (ch == '&' || ch == '|' || ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ';' || ch == ',' || ch == '+' || ch == '-' || ch == ':')
        {
            if (ch == '{')
            {
                curlyb_flag++;
            }

            if (ch == '}')
            {
                curlyb_flag--;
            }

            if (ch == '[')
            {
                sqbracket_flag++;
            }

            if (ch == ']')
            {
                sqbracket_flag--;
            }

            char temp[3] = {ch, '\0', '\0'};
            if ((ch == '+') && (ch = fgetc(ext->fptr_source_fname) == '+'))
            {
                strcat(temp, "+");
                printf(MAGENTA "OPERATOR : " RESET);
                printf("%s\n", temp);
                fprintf(output_fptr, "%s", temp);
                continue;
            }
            else if ((ch == '-') && (ch = fgetc(ext->fptr_source_fname) == '-'))
            {
                strcat(temp, "-");

                printf(MAGENTA "OPERATOR : " RESET);
                printf("%s\n", temp);
                fprintf(output_fptr, "%s", temp);
                continue;
            }
            else if ((ch == '&') && (ch = fgetc(ext->fptr_source_fname) == '&'))
            {
                strcat(temp, "&");
                // print_token(temp, "OPERATOR");
                // printf(MAGENTA"OPERATOR : %s\n"RESET, temp);
                printf(MAGENTA "OPERATOR : " RESET);
                printf("%s\n", temp);
                fprintf(output_fptr, "%s", temp);
                continue;
            }
            else if ((ch == '|') && (ch = fgetc(ext->fptr_source_fname) == '|'))
            {
                strcat(temp, "|");

                printf(MAGENTA "OPERATOR : " RESET);
                printf("%s\n", temp);
                fprintf(output_fptr, "%s", temp);
                continue;
            }
            else if (temp[0] != ch)
            {
                fseek(ext->fptr_source_fname, -1, SEEK_CUR);
            }
            // else{
            // fseek(ext->fptr_source_fname, -1, SEEK_CUR);
            // }

            printf(BLUE "SPECIAL CHAR : " RESET);
            printf("%s\n", temp);
            fprintf(output_fptr, "%s", temp);

            continue;
        }

        /* Handle Operators */
        if (ch == '=' || ch == '<' || ch == '>' || ch == '!' || ch == '(' || ch == ')' || ch == '.' || ch == '~' || ch == '*' || ch == '/' || ch == '%' || ch == '&' || ch == '^' || ch == '|')
        {
            // char temp[3] = {ch, '=', '\0'};
            // char temp[2] = {ch, '\0'};

            // if (ch == '/')
            // {
            //     if ((ch = fgetc(ext->fptr_source_fname) == '/'))
            //     {
            //         while (ch != '\n')
            //         {
            //             ch = fgetc(ext->fptr_source_fname);
            //         }
            //         continue;
            //     }
            //     else if( (ch = fgetc(ext->fptr_source_fname) == '*') )
            //     {
            //         // char ch1;
            //         while( ch != EOF )
            //         {
            //             if( (ch = fgetc(ext->fptr_source_fname) == '*') && (ch = fgetc(ext->fptr_source_fname) == '/') )
            //             {
            //                 break;
            //             }
            //         }
            //         continue;
            //     }
            //     else
            //     {
            //         ch = '/';
            //         fseek(ext->fptr_source_fname, -1, SEEK_CUR);
            //     }
            // }

            if (ch == '/')
            {
                ch = fgetc(ext->fptr_source_fname);
                if (ch == '/')
                {
                    // Handle single-line comment
                    while (ch != '\n' && ch != EOF)
                    {
                        ch = fgetc(ext->fptr_source_fname);
                    }
                    continue;
                }
                else if (ch == '*')
                {
                    // Handle multi-line comment
                    char prev = '\0';
                    while (ch != EOF)
                    {
                        prev = ch;
                        ch = fgetc(ext->fptr_source_fname);
                        if (prev == '*' && ch == '/')
                        {
                            // End of multi-line comment
                            ch = fgetc(ext->fptr_source_fname); // Move past '/'
                            break;
                        }
                    }
                    continue;
                }
                else
                {
                    
                    fseek(ext->fptr_source_fname, -1, SEEK_CUR);
                    ch = '/';
                }
            }

            if (ch == '(')
            {
                paranthesis_flag++;
            }
            if (ch == ')')
            {
                paranthesis_flag--;
            }

            char temp[2] = {ch, '\0'};
            ch = fgetc(ext->fptr_source_fname);
            if (ch == '=')
            {
                // strcat(temp, "=");
                temp[1] = '=';
            }
            else
            {
                fseek(ext->fptr_source_fname, -1, SEEK_CUR);
            }
            // print_token(temp, "OPERATOR");
            // printf(CYAN"OPERATOR : %s\n"RESET, temp);
            printf(CYAN "OPERATOR : " RESET);
            printf("%s\n", temp);
            fprintf(output_fptr, "%s", temp);
            continue;
        }

        /* Handle strings */
        if (ch == '"' || ch == '\'')
        {
            buffer[i++] = ch;
            if (ch == '"')
                dquote_flag++;
            else if (ch == '\'')
                squote_flag++;

            while (((ch = fgetc(ext->fptr_source_fname)) != '"') && ch != EOF && ch != '\'')
            {
                if (ch == '\n') // own written \n will read as \ n and pressed enter(next line read as \n)
                {
                    buffer[i] = '\0';
                    printf(RED "ERROR: %s is string error\n" RESET, buffer);
                    exit(0);
                }
                buffer[i++] = ch;
            }
            if (ch == '"' || ch == '\'')
            {
                if (ch == '"')
                    dquote_flag--;
                else if (ch == '\'')
                    squote_flag--;

                buffer[i++] = ch;
                buffer[i] = '\0';

                if (squote_flag != 0 || dquote_flag != 0)
                    continue;

                printf(GREEN "LITERAL : " RESET);
                printf("%s\n", buffer);
                fprintf(output_fptr, "%s", buffer);
                i = 0;
            }
            continue;
        }
    }

    if (curlyb_flag > 0)
    {
        printf(RED "ERROR: } missing\n");
    }
    else if (curlyb_flag < 0)
    {
        printf(RED "ERROR: { missing\n");
    }

    fclose(ext->fptr_source_fname);
    fclose(output_fptr);
}