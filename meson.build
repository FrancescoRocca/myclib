project(
  'testlib',
  'c',
  version: '0.2',
  default_options: ['c_std=c18', 'warning_level=3'],
)

cc = meson.get_compiler('c')

# Add global posix feature macro
add_global_arguments('-D_POSIX_C_SOURCE=200112L', language: 'c')

# Sources without test files
lib_src = files(
  'hashmap/myhashmap.c',
  'queue/myqueue.c',
  'socket/mysocket.c',
  'stack/mystack.c',
  'string/mystring.c',
  'vector/myvector.c',
)

# Test files
test_src = files(
  'test/hashmap/hm1.c',
  'test/queue/queue1.c',
  'test/socket/socket1.c',
  'test/stack/stack1.c',
  'test/string/str1.c',
  'test/string/str2.c',
  'test/string/str3.c',
  'test/test.c',
  'test/vector/vec1.c',
)

# Windows Socket lib
winsock_dep = cc.find_library('ws2_32', required: false)

# Include directories
inc_dir = include_directories('string', 'queue', 'hashmap', 'vector', 'stack', 'socket')
if host_machine.system() == 'windows'
  win_inc_dir = include_directories('c:/include/')
else
  win_inc_dir = []
endif

# Static library
myclib_lib = static_library(
  'myclib',
  lib_src,
  include_directories: inc_dir,
  dependencies: winsock_dep,
  install: true,
)

# Install headers
install_headers(
  [
    'hashmap/myhashmap.h',
    'queue/myqueue.h',
    'string/mystring.h',
    'vector/myvector.h',
    'stack/mystack.h',
    'socket/mysocket.h',
  ],
  subdir: 'myclib',
)

# Test executable
executable(
  'testlib',
  lib_src + test_src,
  include_directories: [inc_dir, win_inc_dir],
  dependencies: winsock_dep,
  link_with: myclib_lib,
)
