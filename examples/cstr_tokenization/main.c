#include "../../mk_str.h"
#include <stdio.h>

int main() {
    // View of a C string
    mk_str_view_t lipsum = mk_str_view_cstr("Dolorem ipsum quia dolor sit amet consectetur adipisci velit.");
    // Tokenizer struct
    mk_str_tokenizer_t st = mk_str_tokenizer(mk_str_view_cstr(" "));
    // For-each-like macro for easy tokenizatioz
    mk_str_foreach_token(tkn, &st, lipsum) {
        if (tkn.length == 0) {
            continue;
        }
        printf(">");
        mk_str_print(tkn);
        printf("<\n");
    }
    return 0;
}