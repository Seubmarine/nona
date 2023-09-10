#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include <stdio.h>
#include <string.h>

int open_file(char const *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Nona");
        return (0);
    }
    char *file_str = NULL;
    size_t length = 0;

    fseek(file, 0, SEEK_END);
    length = ftell(file) + 1;
    fseek(file, 0, SEEK_SET);
    file_str = malloc(length);
    if (!file_str)
    {
        perror("Nona: couldn't malloc file");
        return (0);
    }
    fread(file_str, 1, length, file);
    file_str[length - 1] = '\0';
    fclose(file);
    struct lexer_info lexer_info = lexer_file(file_str);
    if (lexer_info.tokens == NULL)
        return (0);
    parser parser = parse_file(lexer_info);
    uint32_t *t = expression_get_data(parser.ast, &parser.si);
    printf("result = %i\n", *t);
    free(file_str);
    return (1);
}


int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        printf("no input file\n");
        return (1);
    }
    char *filename_separator = strchr(argv[1], '.');
    if (filename_separator == NULL || strstr(filename_separator + 1, "nona") == NULL)
    {
        printf("input file doesn't have a valid extension\n");
        return (1);
    }
    char const *filename = argv[1];
    return (!open_file(filename));
}
