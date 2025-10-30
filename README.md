# mk_str

Simple string implementation in C

## We have

- Arena allocator
- Length-based string
- String view
- Tokenizer
- Interop with C strings

## To do

- Efficient string builder

## Example

```c
#include "../../mk_str.h"
#include <stdio.h>

int main() {
    // Arena to allocate strings
    // Underlying memory is provided by user
    char buffer[1000];
    mk_str_arena_t sa = mk_str_arena(buffer, sizeof(buffer));

    mk_str_view_t v1 = mk_str_view_cstr("Hello");
    mk_str_view_t v2 = mk_str_view_cstr("world!");

    mk_str_t hello_world = mk_str_concat(&sa, v1, mk_str_view(mk_str_concat(&sa, mk_str_view_cstr(" "), v2)));
    // No need to check each individual function
    if (!mk_str_is_valid(hello_world)) {
        printf("Arena ran out of space!\n");
        return 0;
    }

    mk_str_println(mk_str_view(hello_world));

    // No need to deallocate anything manually
    return 0;
}
```