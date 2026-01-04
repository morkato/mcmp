# project('monarch', 'c', version: '1.0.0')

# inc = include_directories('../mcisid/include', 'policy/include', 'include')
# openssl_dep = dependency('openssl')
# common_libsources = files(
#   'src/common/monarch.c',
#   'src/common/uid.c',
#   'src/common/log.c'
# )
# linux_libsources = files(
#   'src/linux/monarch.c',
#   'src/linux/store.c',
#   'src/linux/uid.c',
#   'src/linux/log.c',
#   'src/linux/util.c'
# )
# libmonarch = shared_library(
#   'monarch',
#   common_libsources + linux_libsources,
#   include_directories: inc,
#   dependencies: [libmcisid_dep, libpolicy_dep, openssl_dep],
#   install: true,
#   install_dir: get_option('libdir')
# )

# libmonarch_dep = declare_dependency(
#   link_with: libmonarch,
#   include_directories: inc
# )

# install_headers(
#   'include/monarch/monarch.h',
#   'include/monarch/store.h',
#   'include/monarch/uid.h',
#   'include/monarch/log.h',
#   subdir: 'monarch'
# )

# monarch_test_bitmap = executable('monarch-botmap', 'test/monarch-bitmap.c',
#   include_directories: inc,
#   dependencies: [libmcisid_dep, libmonarch_dep],
#   install: false)

# executable('simple_test_exc', 'test/simple-test.c',
#   include_directories: inc,
#   dependencies: [libmcisid_dep, libmonarch_dep, openssl_dep],
#   install: false)

# test('monarch-bitmap', monarch_test_bitmap)

subdir('policy')
subdir('utilities')
subdir('routines')
subdir('server')
subdir('store')
subdir(('cli'))