inc = include_directories(
  join_paths('..', 'policy/include'),
  join_paths('..', 'utilities/include'),
  join_paths('..', 'routines/include'),
  join_paths('..', 'server/include')
)

common_sources = files(
  # 'src/common/clients.c',
  'src/common/dispatcher.c',
  'src/common/heartbeat.c'
)

linux_sources = files(
  # 'src/linux/clients.c',
  'src/linux/protocol.c',
  'src/linux/server.c',
)

libserver = shared_library(
  'monarch-server',
    common_sources + linux_sources,
  include_directories: inc,
  dependencies: [libpolicy_dep, libroutines_dep, libutilities_dep],
  install: true,
  install_dir: get_option('libdir')
)

libserver_dep = declare_dependency(
  link_with: libserver,
  include_directories: inc
)

install_headers(
  'include/monarch/server/client.h',
  'include/monarch/server/dispatcher.h',
  'include/monarch/server/heartbeat.h',
  'include/monarch/server/protocol.h',
  'include/monarch/server/server.h',
  subdir: 'monarch/server'
)