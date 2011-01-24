# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)
require "exiv2/version"

Gem::Specification.new do |s|
  s.name        = "exiv2"
  s.version     = Exiv2::VERSION
  s.platform    = Gem::Platform::RUBY
  s.authors     = ["Pete Yandell"]
  s.email       = ["pete@envato.com"]
  s.homepage    = ""
  s.summary     = %q{A simple wrapper around exiv2}
  s.description = %q{A simple wrapper around the C++ exiv2 libary for reading image metadata}

  s.rubyforge_project = "exiv2"

  s.add_development_dependency "rspec"
  s.add_development_dependency "rake-compiler"

  s.files         = `git ls-files`.split("\n")
  s.test_files    = `git ls-files -- {test,spec,features}/*`.split("\n")
  s.executables   = `git ls-files -- bin/*`.split("\n").map{ |f| File.basename(f) }
  s.require_paths = ["lib", "ext"]
  s.extensions    = ["ext/exiv2/extconf.rb"]
end
