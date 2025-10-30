#include "mk_str.h"

int main() {
    mk_str_view_t v = mk_str_view_cstr("Hello World!");
    char cstr[5];
    // Only 'Hello' gets copied
    mk_str_to_cstr(v, cstr, sizeof(cstr));
    printf("%s\n", cstr);
}