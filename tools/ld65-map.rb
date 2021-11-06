#!/usr/bin/ruby
# frozen_string_literal: true

require 'json'

def hex(hex_value)
  hex_value.to_i(16)
end

def scale(x, total)
  x.to_f / total
end

def parse_map(map_file)
  map_text = File.readlines(map_file, chomp: true)

  module_data = []

  mode = nil

  map_text.each do |line|
    case line
    when 'Modules list:'
      mode = :modules_list
    when ''
      mode = nil
    end

    case mode
    when :modules_list
      if (m = line.match(/\A\s*(?<name>[^\s]*):/))
        module_data << { module: m['name'], segments: [] }
      elsif (m = line.match(/\A\s+(?<segment>\S+)\s+Offs=(?<offset>\S+)\s+Size=(?<size>\S+)\s+Align=(?<align>\S+)\s+Fill=(?<fill>\S+)/))
        module_data.last[:segments] << {
          segment: m['segment'],
          offset: hex(m['offset']),
          size: hex(m['size']),
          align: hex(m['align']),
          fill: hex(m['fill'])
        }
      end
    end
  end

  module_data
end

def parse_size(size_string)
  if size_string.start_with?('$')
    size_string[1..].to_i(16)
  else
    size_string.to_i
  end
end

def parse_cfg(config_file)
  config_text = File.readlines(config_file, chomp: true)

  categories = {}

  mode = nil

  config_text.each do |line|
    case line.gsub(/#.*/, '')
    when /MEMORY.*\{/
      mode = :memory
    when /SEGMENTS.*\{/
      mode = :segments
    when /}/
      mode = nil
    end

    case mode
    when :memory
      if (m = line.match(/(?<name>\w+):.*size\s*=\s*(?<size>\$?\d+)/))
        categories[m['name']] = { size: parse_size(m['size']), segments: [] }
      end
    when :segments
      if (m = line.match(/(?<name>\w+):.*load\s*=\s*(?<cat>\w+)/))
        categories[m['cat']][:segments] <<= m['name']
      end
    end
  end
  categories
end

if ARGV.empty?
  puts 'Arguments: map cfg json-output'
  exit 1
end

map_file = ARGV[0]
config_file = ARGV[1]
output = ARGV[2]

module_data = parse_map(map_file)
categories = parse_cfg(config_file)

json_map = { name: 'ld65-map', children: [] }

puts 'Usage per category:'
categories.each do |cat_name, cat_data|
  puts "#{cat_name}:"
  level_1_child = { name: cat_name, children: [] }
  json_map[:children] << level_1_child

  free = cat_data[:size]
  cat_data[:segments].each do |seg_name|
    usage = module_data.flat_map { |mod| mod[:segments] }
                       .select { |segment| segment[:segment] == seg_name }
                       .map { |segment| segment[:size] }
                       .sum
    puts "  #{seg_name}: #{usage}"
    level_2_child = { name: seg_name, children: [] }
    level_1_child[:children] << level_2_child

    module_data.map do |mod|
      [
        mod[:module],
        mod[:segments].find { |seg| seg[:segment] == seg_name }
                      .then { |seg| seg ? seg[:size] : nil }
      ]
    end.reject { |_m, s| s.nil? }
               .sort_by { |_m, s| s }
               .reverse
               .each do |m, s|
      puts "    #{m}: #{s} bytes"
      level_3_child = { name: m, size: s, value: scale(s, cat_data[:size].to_f), group: cat_name, subgroup: seg_name }
      level_2_child[:children] << level_3_child
    end

    free -= usage
  end
  level_1_child = { name: 'Free', size: free, value: scale(free, cat_data[:size].to_f), group: cat_name }
  json_map[:children] << level_1_child
  puts "  Free: #{free}"
end

File.open(output, 'w') do |file|
  file.write json_map.to_json
end
