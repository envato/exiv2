require 'mkmf'

$CXXFLAGS += " -std=c++11"
RbConfig::CONFIG['PKG_CONFIG'] = 'pkg-config'

if dir_config("exiv2") == [nil, nil]
  pkg_config("exiv2")
end
have_library("exiv2")

# Some extensions are optional in versions <= 0.27 and also don't exist in
# versions >= 0.28.
# Check if they're enabled in the existing exiv2 headers
# configuration and include the relevant libraries.
if have_macro("EXV_USE_SSH", "exiv2/exv_conf.h")
  if dir_config("libssh") == [nil, nil]
    pkg_config("libssh")
  end
  have_library("libssh")
end

if have_macro("EXV_USE_CURL", "exiv2/exv_conf.h")
  if dir_config("libcurl") == [nil, nil]
    pkg_config("libcurl")
  end
  have_library("libcurl")
end

create_makefile("exiv2/exiv2")
