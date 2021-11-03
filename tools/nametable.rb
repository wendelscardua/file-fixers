# frozen_string_literal: true

# High-level manipulation of NES nametables

class Nametable
  attr_reader :width, :height, :tile_bytes, :attribute_bytes

  def initialize(bytes)
    if bytes.size == 1024
      @width = 32
      @height = 30
    else
      # assume "map" subnametable
      @height = bytes.pop * 256 + bytes.pop
      @width = bytes.pop * 256 + bytes.pop
    end

    @tile_bytes, @attribute_bytes = bytes[0...@height * @width], bytes[@height * @width...]
  end

  def self.from_file(filename)
    new(File.open(filename, 'rb', &:read).unpack('C*'))
  end

  def tiles
    @tiles ||= Tiles.new(self)
  end

  def attributes
    @attributes ||= Attributes.new(self)
  end

  def bytes
    @tile_bytes + @attribute_bytes
  end

  class Tiles
    def initialize(nametable)
      @nametable = nametable
    end

    def [](x, y)
      @nametable.tile_bytes[y * @nametable.width + x]
    end

    def []=(x, y, value)
      @nametable.tile_bytes[y * @nametable.width + x] = value
    end
  end

  class Attributes
    def initialize(nametable)
      @nametable = nametable
    end

    def [](meta_x, meta_y)
      attr_row = meta_y / 2
      attr_col = meta_x / 2
      attr_index = attr_row * @nametable.width / 4 + attr_col
      attr_shift = 2 * ((meta_y % 2) * 2 + meta_x % 2)
      (@nametable.attribute_bytes[attr_index] >> attr_shift) & 0b11
    end

    def []=(meta_x, meta_y, value)
      attr_row = meta_y / 2
      attr_col = meta_x / 2
      attr_index = attr_row * @nametable.width / 4 + attr_col
      attr_shift = 2 * ((meta_y % 2) * 2 + meta_x % 2)
      @nametable.attribute_bytes[attr_index] &= (0b11111111 ^ (0b11 << attr_shift))
      @nametable.attribute_bytes[attr_index] |= (value << attr_shift)
    end
  end
end
