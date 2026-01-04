inc = include_directories('../policy/include', 'include')
common_sources = files(
  'src/common/chunkedqueue.c',
  'src/common/setqueue.c',
  'src/common/queue.c',
  'src/common/atomicbitmap.c',
  'src/common/bitmap.c'
)
linux_sources = files(
)

libutilities = shared_library(
  'monarch-utilities',
  common_sources + linux_sources,
  include_directories: inc,
  dependencies: [libpolicy_dep],
  install: true,
  install_dir: get_option('libdir')
)

libutilities_dep = declare_dependency(
  link_with: libutilities,
  include_directories: inc
)

install_headers(
  'include/monarch/util/setqueue.h',
  'include/monarch/util/queue.h',
  subdir: 'monarch/util'
)