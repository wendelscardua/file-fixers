#!/usr/bin/ruby
# frozen_string_literal: true

# generates sector room data from tmx file
# inputs:
# - tiled file (.tmx)
# - output room file (.bin)

require 'bundler/inline'

gemfile do
  source 'https://rubygems.org'

  gem 'nokogiri'
  gem 'pry'
end

tmx_file, room_file = ARGV

document = Nokogiri::XML(File.read(tmx_file))

room_data = []
document.xpath('//object').each do |object|
  room_data << (object['x'].to_f / 16).to_i
  room_data << (object['y'].to_f / 16).to_i
  room_data << (object['width'].to_f / 16).to_i
  room_data << (object['height'].to_f / 16).to_i
end
room_data << 0xff

File.open(room_file, 'wb') do |f|
  f.write room_data.pack('C*')
end
