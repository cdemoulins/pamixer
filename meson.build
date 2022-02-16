project('pamixer', 'cpp', version : '1.6-dev',
  default_options : ['cpp_std=c++11'])

src = files([
  'src/callbacks.cc',
  'src/device.cc',
  'src/pamixer.cc',
  'src/pulseaudio.cc'
])

conf_data = configuration_data()
conf_data.set_quoted('VERSION', meson.project_version())
configure_file(output : 'config.hh', configuration : conf_data)
configuration_inc = include_directories('src')

pulse = dependency('libpulse')
cxxopts = dependency('cxxopts')

executable('pamixer', 
  sources : src,
  install : true,
  include_directories : configuration_inc,
  dependencies : [pulse, cxxopts])

install_man('pamixer.1')
