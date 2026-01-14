project('mcmp', 'c', version: '1.0.0')

inc = include_directories('include')
common_sources = files(
  'src/common/statos.c'
)

linux_sources = files(
  
)

libmcmp = shared_library(
  'mcmp',
  common_sources + linux_sources,
  include_directories: inc,
  install: true,
  install_dir: get_option('libdir')
)

libmcmp_dep = declare_dependency(
  link_with: libmcmp,
  include_directories: inc
)

install_headers(
  'include/mcmp/statos.h',
  'include/mcmp/stat.h',
  'include/mcmp/protocol.h',
  'include/mcmp/utilities.h',
  subdir: 'mcmp'
)

subdir('utilities')
# subdir('routines')
# subdir('server')
# subdir('store')
# subdir('cli')