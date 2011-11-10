require 'rspec/core/rake_task'
RSpec::Core::RakeTask.new(:spec)
task :default => :spec

require 'rake/extensiontask'
Rake::ExtensionTask.new('exiv2') do |ext|
  ext.lib_dir = "lib/exiv2"
  if ENV['EXIV2_DIR']
    ext.config_options << "--with-exiv2-dir=#{ENV['EXIV2_DIR']}"
  end
end
Rake::Task[:spec].prerequisites << :compile

require 'bundler'
Bundler::GemHelper.install_tasks

