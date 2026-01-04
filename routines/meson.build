inc = include_directories('../policy/include', '../utilities/include', 'include')
common_sources = files(
)
linux_sources = files(
  'src/linux/monitor.c',
  'src/linux/worker.c',
  'src/linux/lock.c'
)

libroutines = shared_library(
  'monarch-routines',
  common_sources + linux_sources,
  include_directories: inc,
  dependencies: [libpolicy_dep, libutilities_dep],
  install: true,
  install_dir: get_option('libdir')
)

libroutines_dep = declare_dependency(
  link_with: libroutines,
  include_directories: inc
)

install_headers(
  'include/monarch/routines/routine.h',
  'include/monarch/routines/monitor.h',
  'include/monarch/routines/consumer.h',
  'include/monarch/routines/pool.h',
  'include/monarch/routines/lock.h',
  subdir: 'monarch/routines'
)