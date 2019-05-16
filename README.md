# Exiv2

[![Build Status](https://travis-ci.org/envato/exiv2.svg)](https://travis-ci.org/envato/exiv2)

A simple wrapper around the C++ Exiv2 libary for reading and writing image metadata.

See http://www.exiv2.org/

Requires that the exiv2 C++ library is installed.

## Usage

```
gem install exiv2
```

if you get errors with header could not be found below:

```
exiv2.cpp:1:10: fatal error: 'exiv2/image.hpp' file not found
#include "exiv2/image.hpp"
```

please explicitly declare the header path

```
gem install exiv2 -- --with-exiv2-include="${EXIV2_PREFIX}/include" --with-exiv2-lib="${EXIV2_PREFIX}/lib"
```

on OSX with Homebrew's exiv2, the `EXIV2_PREFIX` can be set:

```
export EXIV2_PREFIX=$(brew --prefix exiv2)
```

If you get this error while trying to install as part of a bundle install, you can set these paths using:
```
bundle config build.exiv2 --with-exiv2-include="${EXIV2_PREFIX}/include" --with-exiv2-lib="${EXIV2_PREFIX}/lib"
```


If you are on new version of Command Line Tool (that is newer than 6.2, and bump into following error:

```
/Library/Developer/CommandLineTools/usr/bin/../include/c++/v1/iterator:341:10: fatal error: '__debug' file not found
#include <__debug>
```

You can follow the quick hack by touching a new file `/Library/Developer/CommandLineTools/usr/include/c++/v1/__debug` with content:

```
#ifndef _LIBCPP_ASSERT
#define _LIBCPP_ASSERT(...) ((void)0)
#endif
```

Once everything is successfully installed, you can give it a go:

```ruby
require 'exiv2'
image = Exiv2::ImageFactory.open("image.jpg")
image.read_metadata
image.iptc_data.each do |key, value|
  puts "#{key} = #{value}\n"
end
image.exif_data.each { ... }
image.xmp_data.each { ... }

iptc_data_hash  = image.iptc_data.to_hash
xmp_data_hash   = image.xmp_data.to_hash

image.exif_data.delete("Exif.Image.Software")
image.iptc_data.delete_all("Iptc.Application2.Keywords")

image.iptc_data["Iptc.Application2.Caption"] = "A New Caption"
image.iptc_data.add("Iptc.Application2.Keywords", "fishy")

image.write_metadata
```

## Why?

None of the existing Ruby libraries for reading and writing image metadata did quite what
we wanted, so we figured it wouldn't be too hard to wrap enough of Exiv2 to
meet our needs.

The intention is to just mirror the Exiv2 API in Ruby, so the path to extending
this to support more of Exiv2's functionality should be straightforward. Patches
are welcome.

## Compatibility

Tested on 1.9.3, 2.0.x and 2.1.x with Exiv2 0.27.1

## Developing

* Fork the project.
* Make your feature addition or bug fix.
* Add tests for it. This is important so I don't break it in a
  future version unintentionally.
* Commit, do not mess with rakefile, version, or history.
  (if you want to have your own version, that is fine but bump version in a commit by itself I can ignore when I pull)
* Send me a pull request. Bonus points for topic branches.

## Status

In early development. Very little of Exiv2's API is supported, and it hasn't
been heavily tested.

## Contributors

Pete Yandell, Mark Turnley, Lucas Parry, Norbert Wojtwoicz, Jan Graichen, John Barton

## Copyright

Copyright (c) 2014 Envato & Pete Yandell. See LICENSE for details.
