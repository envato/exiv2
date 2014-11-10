require 'mkmf'
config = pkg_config('exiv2')
$CPPFLAGS = [$CPPFLAGS, config[0]].join ' '
create_makefile("exiv2/exiv2")
