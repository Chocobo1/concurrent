project('concurrent', 'cpp',
        default_options: ['buildtype=release',
                          #'buildtype=debugoptimized',
                          'cpp_std=c++11',
                          'warning_level=1',
                          'werror=true',
                          'strip=true',
                          'b_sanitize=undefined',
                          'b_asneeded=false',
                          'b_lundef=false'
                         ])

CXXFLAGS = ''
CXXFLAGS = CXXFLAGS.split(' ')

LDFLAGS = ''
LDFLAGS = LDFLAGS.split(' ')

sources = files('main.cpp',
                'helpers.cpp', 'master.cpp', 'semaphore.cpp')

exe = executable('concurrent', sources,
                 cpp_args: ['/MT'],
                 #link_args: LDFLAGS
                )
