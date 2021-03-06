# frozen_string_literal: true

# converts enemy stats to assembly

require 'yaml'

input_file, output_file = ARGV

def dice_to_int(dice_string)
  match = dice_string.match(/(?<amount>\d+)[dD](?<sides>\d+)/)
  raise "Invalid dice string #{dice_string}" unless match

  amount = match['amount'].to_i
  sides = match['sides'].to_i

  raise "Invalid amount #{amount}" if amount > 7
  raise "Invalid sides #{sides}" if sides > 31

  sides * 8 + amount # 5 bits for sides, 3 bits for amount
end

num_enemy_models = File.read("src/enemies.inc").match(/NUM_ENEMY_MODELS\s+(\d+)/)[1].to_i

stats = YAML.safe_load(File.read(input_file))

attr_names = %w[difficulty base_level speed moves]
name_list = []
attr_lists = attr_names.map { |name| [name, []] }.to_h
attr_lists['attack'] = []
attr_lists['sprite_index'] = []
stats.each do |key, value|
  name_list << key
  attr_names.each do |name|
    attr_lists[name] << value[name]
  end
  attr_lists['attack'] << dice_to_int(value['attack'])
  attr_lists['sprite_index'] << ((value['palette'] * num_enemy_models + value['model']) * 4)
end

(1..30).each do |party_level|
  (1..16).each do |dungeon_level|
    min_level = dungeon_level / 6
    avg_level = (party_level + dungeon_level) / 2
    if attr_lists['difficulty'].none? { |difficulty| difficulty >= min_level && difficulty <= avg_level }
      raise "No enemy type compatible w/ dungeon level #{dungeon_level} and party level #{party_level}"
    end
  end
end

File.open(output_file, 'wb') do |f|
  f.puts '.include "../src/charmap.inc"'
  f.puts '.segment "RODATA"'
  name_list.each do |name|
    f.puts %(#{name}: .byte "#{name}")
  end

  str_name_list = name_list.join(', ')
  f.puts '.export _enemy_name'
  f.puts "_enemy_name: .word #{str_name_list}"

  f.puts '.segment "BANK1"'
  attr_lists.each do |attr_name, list|
    str_list = list.map(&:to_s).join(', ')
    f.puts ".export _enemy_#{attr_name}"
    f.puts "_enemy_#{attr_name}: .byte #{str_list}"
  end
end
