#ifndef MK_STR_H

#include <stdio.h>
#include <stdbool.h>

#define MKSTRDEF static inline

typedef struct {
    char * data;
    size_t capacity;
    size_t length;
} mk_str_arena_t;

typedef struct {
    char * data;
    size_t length;
} mk_str_t;

typedef struct {
    const char * data;
    size_t length;
} mk_str_view_t;

typedef struct {
    mk_str_view_t delimiters;
    size_t next_offset;
} mk_str_tokenizer_t;

MKSTRDEF mk_str_arena_t mk_str_arena(void * memory, size_t size) {
    return (mk_str_arena_t){
        .data = memory,
        .capacity = size,
        .length = 0
    };
}

MKSTRDEF void mk_str_arena_parse(mk_str_arena_t * arena) {
    arena->length = 0;
}

MKSTRDEF void mk_str_arena_print_info(mk_str_arena_t arena) {
    printf("String arena info:\n");
    printf("  data ptr: %llu\n", (unsigned long long)arena.data);
    printf("  capacity: %zu\n", arena.capacity);
    printf("  length:   %zu\n", arena.length);
}

MKSTRDEF void mk_str_arena_dump_chars(mk_str_arena_t arena) {
    printf("String arena dump:\n");
    for (size_t i = 0; i < arena.capacity; i++) {
        fputc(arena.data[i], stdout);
    }
}

MKSTRDEF void mk_str_arena_dump_bytes(mk_str_arena_t arena) {
    printf("String arena byte dump:\n");
    for (size_t i = 0; i < arena.capacity; i++) {
        printf("%3u ", (unsigned char) arena.data[i]);
    }
}

MKSTRDEF mk_str_view_t mk_str_view(mk_str_t string) {
    return (mk_str_view_t) {
        .data = string.data,
        .length = string.length
    };
}

MKSTRDEF mk_str_view_t mk_str_view_cstr(const char * cstr) {
    size_t i;
    for (i = 0; cstr[i] != '\0'; i++);
    return (mk_str_view_t) {
        .data = cstr,
        .length = i
    };
}

MKSTRDEF bool mk_str_view_is_valid(mk_str_view_t view) {
    return view.data != NULL;
}

MKSTRDEF bool mk_str_is_valid(mk_str_t string) {
    return mk_str_view_is_valid(mk_str_view(string));
}

MKSTRDEF mk_str_t mk_str_invalid() {
    return (mk_str_t) {
        .data = NULL,
        .length = 0
    };
}

MKSTRDEF mk_str_t mk_str_empty(mk_str_arena_t * arena, size_t length) {
    if (arena->length + length > arena->capacity || length == 0) {
        return mk_str_invalid();
    }

    mk_str_t out = {
        .data = arena->data + arena->length,
        .length = length
    };
    arena->length += length;

    return out;
}

MKSTRDEF mk_str_t mk_str(mk_str_arena_t * arena, const char * cstr) {
    size_t i;
    for (i = 0; cstr[i] != '\0'; i++) {
        if (arena->length + i == arena->capacity) {
            return mk_str_invalid();
        }
        arena->data[arena->length + i] = cstr[i];
    }

    mk_str_t out = {
        .data = arena->data + arena->length,
        .length = i
    };
    arena->length += i;

    return out;
}

MKSTRDEF mk_str_t mk_str_concat(mk_str_arena_t * arena, mk_str_view_t a, mk_str_view_t b) {
    if (!mk_str_view_is_valid(a) || !mk_str_view_is_valid(b)) {
        return mk_str_invalid();
    }

    mk_str_t out = mk_str_empty(arena, a.length + b.length);
    if (!mk_str_is_valid(out)) {
        return mk_str_invalid();
    }

    for (size_t i = 0; i < a.length; i++) {
        out.data[i] = a.data[i];
    }
    for (size_t i = 0; i < b.length; i++) {
        out.data[a.length + i] = b.data[i];
    }

    return out;
}

MKSTRDEF bool mk_str_contains(mk_str_view_t haystack, mk_str_view_t needle) {
    if (haystack.length < needle.length) {
        return false;
    }

    for (size_t i = 0; i < haystack.length; i++) {
        for (size_t j = 0; j < needle.length; j++) {
            if (haystack.data[i + j] != needle.data[j]) {
                goto NESTED_END;
            }
        }
        return true;
        NESTED_END:
    }

    return false;
}

MKSTRDEF bool mk_str_contains_char(mk_str_view_t haystack, char needle) {
    for (size_t i = 0; i < haystack.length; i++) {
        if (haystack.data[i] == needle) {
            return true;
        }
    }
    return false;
}

MKSTRDEF bool mk_str_equal(mk_str_view_t a, mk_str_view_t b) {
    if (a.length != b.length) {
        return false;
    }
    for (int i = 0; i < a.length; i++) {
        if (a.data[i] != b.data[i]) {
            return false;
        }
    }
    return true;
}

MKSTRDEF mk_str_t mk_str_copy(mk_str_arena_t * arena, mk_str_view_t view) {
    if (!mk_str_view_is_valid(view)) {
        return mk_str_invalid();
    }
    mk_str_t out = mk_str_empty(arena, view.length);
    for (size_t i = 0; i < view.length; i++) {
        out.data[i] = view.data[i];
    }
    return out;
}

MKSTRDEF mk_str_tokenizer_t mk_str_tokenizer(mk_str_view_t delimiters) {
    return (mk_str_tokenizer_t) {
        .delimiters = delimiters,
        .next_offset = 0
    };
}

MKSTRDEF mk_str_view_t mk_str_tokenizer_get_next(mk_str_tokenizer_t * tokenizer, mk_str_view_t view) {
    if (tokenizer->next_offset >= view.length) {
        return mk_str_view(mk_str_invalid());
    }

    const char * ptr;
    for (ptr = view.data + tokenizer->next_offset; ptr < view.data + view.length; ptr++) {
        if (mk_str_contains_char(tokenizer->delimiters, *ptr)) {
            break;
        }
    }

    mk_str_view_t out = {
        .data = view.data + tokenizer->next_offset,
        .length = (size_t)(ptr - (view.data + tokenizer->next_offset))
    };
    tokenizer->next_offset = (size_t)(ptr - view.data) + 1;

    return out;
}

MKSTRDEF void mk_str_tokenizer_reset(mk_str_tokenizer_t * tokenizer) {
    tokenizer->next_offset = 0;
}

#define mk_str_foreach_token(iterator, tokenizer_ptr, view_to_parse)\
for (\
    mk_str_view_t iterator = mk_str_tokenizer_get_next(tokenizer_ptr, view_to_parse);\
    mk_str_view_is_valid(iterator);\
    iterator = mk_str_tokenizer_get_next(tokenizer_ptr, view_to_parse)\
)

MKSTRDEF void mk_str_print_stream(mk_str_view_t view, FILE * stream) {
    if (!mk_str_view_is_valid(view)) {
        return;
    }
    
    for (size_t i = 0; i < view.length; i++) {
        fputc(view.data[i], stream);
    }
}

MKSTRDEF void mk_str_println_stream(mk_str_view_t view, FILE * stream) {
    mk_str_print_stream(view, stream);
    fputc('\n', stream);
}

MKSTRDEF void mk_str_print(mk_str_view_t view) {
    mk_str_print_stream(view, stdout);
}

MKSTRDEF void mk_str_println(mk_str_view_t view) {
    mk_str_println_stream(view, stdout);
}

#endif // MK_STR_H