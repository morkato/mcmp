inc = include_directories('include')
common_sources = files(
  'src/common/statos.c',
  'src/common/monarch.c',
  'src/common/log.c'
)

linux_sources = files(
  'src/linux/utilities.c',
  'src/linux/monarch.c',
  'src/linux/time.c',
  'src/linux/log.c'
)

libpolicy = shared_library(
  'monarch-policy',
  common_sources + linux_sources,
  include_directories: inc,
  install: true,
  install_dir: get_option('libdir')
)

libpolicy_dep = declare_dependency(
  link_with: libpolicy,
  include_directories: inc
)

install_headers(
  'include/monarch/statos.h',
  'include/monarch/stat.h',
  subdir: 'monarch'
)