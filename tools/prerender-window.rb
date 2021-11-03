#!/usr/bin/ruby
# frozen_string_literal: true

# renders a .map sub-nametable into another nametable
# inputs:
# - base nametable (.nam)
# - sub-nametable (.map)
# - x
# - y
# - output file (.nam)

require_relative './nametable'

base_nametable_file, window_file, window_x, window_y, output_file = ARGV

base_nametable = Nametable.from_file(base_nametable_file)
window = Nametable.from_file(window_file)

(0...window.height).each do |y|
  (0...window.width).each do |x|
    base_nametable.tiles[x + window_x.to_i, y + window_y.to_i] = window.tiles[x, y]
  end
end

(0...window.height / 2).each do |y|
  (0...window.width / 2).each do |x|
    base_nametable.attributes[x + window_x.to_i / 2, y + window_y.to_i / 2] = window.attributes[x, y]
  end
end

File.open(output_file, 'wb') do |f|
  f.write base_nametable.bytes.pack('C*')
end
