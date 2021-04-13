project('imp', 'c')

inc = include_directories('include')

libimp = both_libraries('imp', 'imp.c',
  include_directories : inc)

libimp_dep = declare_dependency(
  link_with: libimp,
  include_directories : inc)
