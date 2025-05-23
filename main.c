/*
    Name: Omkar Irappanavar
    Date: 15/10/2024
    Description: Lexical Analyzer Project
*/

#include<stdio.h>
#include "extract.h"

extract ext;

int main(int argc, char *argv[])
{

    if(argc == 1)
    {
        printf(RED"----------- Command Format -----------\n"RESET);
        printf("Command : \"./a.out\"  \".c file\"\n");
    }
    else
    {
        ext.source_fname = argv[1];

        if(do_extract(&ext) == e_failure)
        {
            return e_failure;
        }
    }
}