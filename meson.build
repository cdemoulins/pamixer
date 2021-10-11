project('pamixer', 'cpp', version : '1.6-dev')

src = ['callbacks.cc', 'device.cc', 'pulseaudio.cc', 'pamixer.cc']

conf_data = configuration_data()
conf_data.set_quoted('VERSION', meson.project_version())
configure_file(output : 'config.hh', configuration : conf_data)
configuration_inc = include_directories('.')

pulse = dependency('libpulse')
boost = dependency('boost', modules : 'program_options')

executable('pamixer', 
  sources : src,
  install : true,
  include_directories : configuration_inc,
  dependencies : [pulse, boost])

install_man('pamixer.1')
