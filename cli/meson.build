inc = include_directories('../routines/include', '../utilities/include', '../policy/include','../server/include', 'include')
common_sources = files()
linux_sources = files(
  'src/linux/test.c', 
)

executable('monarch', 
linux_sources + common_sources,
  include_directories: inc,
  dependencies: [libpolicy_dep, libroutines_dep, libutilities_dep],
  install: false)

# executable('socket_test_exc', 'test/socket-test.c',
#   include_directories: inc,
#   dependencies: [libmcisid_dep, libmonarch_dep, openssl_dep],
#   install: false)