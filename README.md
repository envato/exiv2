# Exiv2

[![License MIT](https://img.shields.io/badge/license-MIT-brightgreen.svg)](https://github.com/envato/exiv2/blob/master/LICENSE)
[![Gem Version](https://img.shields.io/gem/v/exiv2.svg?maxAge=2592000)](https://rubygems.org/gems/exiv2)
[![Gem Downloads](https://img.shields.io/gem/dt/exiv2.svg?maxAge=2592000)](https://rubygems.org/gems/exiv2)
[![Build Status](https://github.com/envato/exiv2/workflows/tests/badge.svg?branch=master)](https://github.com/envato/exiv2/actions?query=branch%3Amaster+workflow%3Atests)

A simple wrapper around the C++ Exiv2 libary for reading and writing image metadata.

See http://www.exiv2.org/

Requires that the exiv2 C++ library is installed.

## Usage

```
gem install exiv2
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

Tested on 2.7.x, 3.0.x, 3.1.x and 3.2.x with Exiv2 0.27.1 and 0.28.0.

## Developing

- Fork the project.
- Make your feature addition or bug fix.
- Add tests for it. This is important so I don't break it in a
  future version unintentionally.
- Commit, do not mess with rakefile, version, or history.
  (if you want to have your own version, that is fine but bump version in a commit by itself I can ignore when I pull)
- Send me a pull request. Bonus points for topic branches.

## Status

In early development. Very little of Exiv2's API is supported, and it hasn't
been heavily tested.

## Contributors

Pete Yandell, Mark Turnley, Lucas Parry, Norbert Wojtwoicz, Jan Graichen, John Barton

## Copyright

Copyright (c) 2014 Envato & Pete Yandell. See LICENSE for details.
