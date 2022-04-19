project(duze1)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_FLAGS "-std=c17 -Wall -Wextra -Wno-implicit-fallthrough -fanalyzer")
set(CMAKE_C_FLAGS_DEBUG "-g")

add_library(duze1 SHARED
    src/compressed_trie.c
    src/string_lib.c
    src/string_vector.c
    src/phone_forward.c
)

add_executable(duze_binary
    src/main.c)

target_link_libraries(duze_binary
    PRIVATE
        duze1
)