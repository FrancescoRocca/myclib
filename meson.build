project(
  'examples',
  'c',
  version: '0.1',
  default_options: ['c_std=c17'],
)

string = files(
  'examples/string/str2.c',
  'string/mystring.c',
)

queue = files(
  'examples/queue/q1.c',
  'queue/myqueue.c',
)

hashmap = files(
  'examples/hashmap/hm1.c',
  'hashmap/myhashmap.c',
)

inc_dir = include_directories('string', 'queue', 'hashmap')

executable('string', string, include_directories: inc_dir)
executable('queue', queue, include_directories: inc_dir)
executable('hashmap', hashmap, include_directories: inc_dir)
