#!/usr/bin/ruby
# frozen_string_literal: true

# converts png into a chr 8k bank

require 'bundler/inline'

gemfile do
  source 'https://rubygems.org'

  gem 'chunky_png'
  gem 'pry'
end

input, output = ARGV

png = ChunkyPNG::Image.from_file(input)

chr = Array.new(128) { Array.new(128) { 0 } }

color_to_twobits = {}

(0...png.height).each do |y|
  (0...png.width).each do |x|
    color = png[x, y]
    chr[y][x] = (color_to_twobits[color] ||= color_to_twobits.size)
  rescue => e
    binding.pry
  end
end

raise 'Too many colors' if color_to_twobits.size > 4

File.open(output, 'wb') do |f|
  (0...16).each do |row|
    (0...16).each do |col|
      first_plane = []
      second_plane = []
      (0...8).each do |subrow|
        fp_byte = 0
        sp_byte = 0
        (0...8).each do |subcol|
          fp_byte <<= 1
          sp_byte <<= 1
          if (chr[row * 8 + subrow][col * 8 + subcol] & 0b01) != 0
            fp_byte += 1
          end
          if (chr[row * 8 + subrow][col * 8 + subcol] & 0b10) != 0
            sp_byte += 1
          end
        end
        first_plane << fp_byte
        second_plane << sp_byte
      end
      f.write(first_plane.pack('C*'))
      f.write(second_plane.pack('C*'))
    end
  end
end
