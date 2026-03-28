project(
    'testlib',
    'c',
    version: '0.3',
    default_options: ['c_std=c18', 'warning_level=3'],
)

# Add global posix feature macro

add_global_arguments('-D_POSIX_C_SOURCE=200112L', language: 'c')

# Sources without test files

lib_src = files(
    'hashmap/myhashmap.c',
    'queue/myqueue.c',
    'set/myset.c',
    'stack/mystack.c',
    'string/mystring.c',
    'vector/myvector.c',
)

# Include directories

inc_dir = include_directories('string', 'queue', 'hashmap', 'vector', 'stack', 'set')
if host_machine.system() == 'windows'
    win_inc_dir = include_directories('c:/include/')
else
    win_inc_dir = []
endif

# Static library

pkg = import('pkgconfig')

myclib_lib = static_library(
    'myclib',
    lib_src,
    include_directories: inc_dir,
    install: true,
)

pkg.generate(
    myclib_lib,
    name: 'myclib',
    description: 'My personal C std library.',
)

# Install headers

install_headers(
    [
        'hashmap/myhashmap.h',
        'queue/myqueue.h',
        'string/mystring.h',
        'vector/myvector.h',
        'set/myset.h',
        'stack/mystack.h',
    ],
    subdir: 'myclib',
)

# Per-file test registrations

test_cases = [
    ['hashmap_hm1', 'test/hashmap/hm1.c'],
    ['queue_queue1', 'test/queue/queue1.c'],
    ['set_set1', 'test/set/set1.c'],
    ['stack_stack1', 'test/stack/stack1.c'],
    ['string_str1', 'test/string/str1.c'],
    ['string_str2', 'test/string/str2.c'],
    ['string_str3', 'test/string/str3.c'],
    ['vector_vec1', 'test/vector/vec1.c'],
]

foreach tc : test_cases
    test_name = tc[0]
    test_source = tc[1]

    test_exe = executable(
        'test_' + test_name,
        test_source,
        include_directories: [inc_dir, win_inc_dir],
        link_with: myclib_lib,
    )

    test(test_name, test_exe)
endforeach
