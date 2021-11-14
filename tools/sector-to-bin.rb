#!/usr/bin/ruby
# frozen_string_literal: true

# generates sector layout from tmx file
# inputs:
# - tiled file (.tmx)
# - output layout file (.bin)

require 'bundler/inline'

gemfile do
  source 'https://rubygems.org'

  gem 'nokogiri'
end

tmx_file, bin_file = ARGV

document = Nokogiri::XML(File.read(tmx_file))

metatiles = document.xpath('//layer/data')
                    .text
                    .scan(/\d+/)
                    .map { |t| t.to_i - 1 }

File.open(bin_file, 'wb') do |f|
  f.write metatiles.pack('C*')
end
