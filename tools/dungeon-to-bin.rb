#!/usr/bin/ruby
# frozen_string_literal: true

# renders a .map sub-nametable into another nametable
# inputs:
# - base nametable (.nam)
# - sub-nametable (.map)
# - x
# - y
# - output file (.nam)

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
