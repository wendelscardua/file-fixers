#!/usr/bin/ruby
# frozen_string_literal: true

# compress a .nam nametable with NESST-compatible RLE
# inputs:
# - .nam filename
# - .rle filename

require_relative './rle'

map_file, rle_file = ARGV

compressed_bytes = RLE.rle(File.open(map_file, 'rb', &:read).unpack('C*'))

File.open(rle_file, 'wb') do |f|
  f.write compressed_bytes.pack('C*')
end
