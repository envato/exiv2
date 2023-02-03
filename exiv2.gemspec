# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)
require "exiv2/version"

Gem::Specification.new do |s|
  s.name        = "exiv2"
  s.version     = Exiv2::VERSION
  s.platform    = Gem::Platform::RUBY
  s.authors     = ["Pete Yandell"]
  s.email       = ["pete@envato.com"]
  s.homepage    = "https://github.com/envato/exiv2"
  s.summary     = %q{A simple wrapper around Exiv2}
  s.description = %q{A simple wrapper around the C++ Exiv2 libary for reading image metadata}

  s.rubyforge_project = "exiv2"

  s.add_development_dependency "rspec"
  s.add_development_dependency "rake-compiler"

  s.files = Dir.chdir(__dir__) do
    `git ls-files -z`.split("\x0").select do |file|
      file.start_with?('ext', 'lib', 'LICENSE', 'README')
    end
  end
  s.require_paths = ["lib", "ext"]
  s.extensions    = ["ext/exiv2/extconf.rb"]
end
