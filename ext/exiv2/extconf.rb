require 'mkmf'

$CXXFLAGS += " -std=c++11"
RbConfig::CONFIG['PKG_CONFIG'] = 'pkg-config'

if dir_config("exiv2") == [nil, nil]
  pkg_config("exiv2")
end
have_library("exiv2")
create_makefile("exiv2/exiv2")
