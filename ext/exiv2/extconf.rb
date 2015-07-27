require 'mkmf'
config = pkg_config('exiv2')
if config
  $CPPFLAGS = [$CPPFLAGS, config[0]].join ' '
  create_makefile("exiv2/exiv2")
else
  abort 'exiv2 devel package is missing'
end
