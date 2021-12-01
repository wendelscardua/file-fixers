# frozen_string_literal: true

# converts dialogs to assembly

require 'yaml'

input_file, output_file = ARGV

dialogs = YAML.safe_load(File.read(input_file))

dialog_table = {}

dialogs.each do |dialog_name, dialog_content|
  dialog_table[dialog_name] = []
  dialog_content.each do |dialog_entry|
    key = dialog_entry.keys.first
    value = dialog_entry.values.first
    if key == 'Amda'
      dialog_table[dialog_name] << '1'
    else
      dialog_table[dialog_name] << '2'
    end
    dialog_table[dialog_name] << '"' + value + '"'
  end
  dialog_table[dialog_name] << 0
end

File.open(output_file, 'wb') do |f|
  f.puts '.include "../src/charmap.inc"'
  f.puts '.segment "BANK1"'

  dialog_table.each do |dialog_name, dialog_content|
    f.puts ".export _dialog_#{dialog_name}"
    f.puts "_dialog_#{dialog_name}: .byte #{dialog_content.join(', ')}"
  end
end
