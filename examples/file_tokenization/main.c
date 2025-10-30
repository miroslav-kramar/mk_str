#include "../../mk_str.h"
#include <stdlib.h>
#include <stdio.h>

char * load_file(FILE * fp, size_t * length) {
    size_t capacity = 0;
    *length = 0;
    char * data = NULL;

    while (1) {
        if (*length == capacity) {
            size_t new_capacity = (capacity == 0) ? 1024 : capacity * 2;
            char * new_data = realloc(data, new_capacity);
            if (new_data == NULL) {
                free(data);
                return NULL;
            }
            data = new_data;
            capacity = new_capacity;
        }

        int c = fgetc(fp);
        if (c == EOF) {
            data[*length] = '\0';
            break;
        }
        data[(*length)++] = (char) c;
    }
    return data;
}

int main() {
    FILE * fp = fopen("file.txt", "r");
    if (fp == NULL) {
        printf("Failed to open file!\n");
        return 0;
    }

    size_t length;
    char * file_contents = load_file(fp, &length);
    if (file_contents == NULL) {
        printf("Failed to load file!\n");
        return 0;
    }

    mk_str_view_t file_str = {
        .data = file_contents,
        .length = length
    };

    mk_str_tokenizer_t st = mk_str_tokenizer(mk_str_view_cstr(" ,.;"));
    mk_str_foreach_token(tkn, &st, file_str) {
        if (tkn.length == 0) {
            continue;
        }
        printf(">");
        mk_str_print(tkn);
        printf("<\n");
    }

    free(file_contents);
    fclose(fp);
    return 0;
}