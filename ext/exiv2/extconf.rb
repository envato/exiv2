require 'mkmf'

RbConfig::MAKEFILE_CONFIG['CC'] = ENV['CC'] if ENV['CC']
RbConfig::MAKEFILE_CONFIG['CCFLAGS'] = ENV['CCFLAGS'] if ENV['CCFLAGS']
RbConfig::MAKEFILE_CONFIG['CXX'] = ENV['CXX'] if ENV['CXX']
RbConfig::MAKEFILE_CONFIG['CXXFLAGS'] = ENV['CXXFLAGS'] if ENV['CXXFLAGS']

pkg_config("exiv2")
dir_config("exiv2")
have_library("exiv2")
create_makefile("exiv2/exiv2")
