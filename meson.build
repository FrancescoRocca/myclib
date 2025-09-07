project(
  'testlib',
  'c',
  version: '0.1',
  default_options: ['c_std=c17'],
)

src = files(
  'hashmap/myhashmap.c',
  'queue/myqueue.c',
  'string/mystring.c',
)

testlib = files(
  'test/test.c',
  'test/hashmap/hm1.c',
  'test/queue/q1.c',
  'test/string/str1.c',
  'test/string/str2.c',
)

sources = src + testlib

inc_dir = include_directories('string', 'queue', 'hashmap')

executable('testlib', sources, include_directories: inc_dir)
