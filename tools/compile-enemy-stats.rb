# frozen_string_literal: true

# converts enemy stats to assembly

require 'yaml'

input_file, output_file = ARGV

num_enemy_models = File.read("src/enemies.inc").match(/NUM_ENEMY_MODELS\s+(\d+)/)[1].to_i

stats = YAML.safe_load(File.read(input_file))

attr_names = %w[difficulty base_level speed moves]
name_list = []
attr_lists = attr_names.map { |name| [name, []] }.to_h
attr_lists['sprite_index'] = []
stats.each do |key, value|
  name_list << key
  attr_names.each do |name|
    attr_lists[name] << value[name]
  end
  attr_lists['sprite_index'] << ((value['palette'] * num_enemy_models + value['model']) * 4)
end

File.open(output_file, 'wb') do |f|
  f.puts '.segment "RODATA"'
  name_list.each do |name|
    f.puts %(#{name}: .byte "#{name}")
  end

  str_name_list = name_list.join(', ')
  f.puts '.export _enemy_name'
  f.puts "_enemy_name: .word #{str_name_list}"

  attr_lists.each do |attr_name, list|
    str_list = list.map(&:to_s).join(', ')
    f.puts ".export _enemy_#{attr_name}"
    f.puts "_enemy_#{attr_name}: .byte #{str_list}"
  end
end
