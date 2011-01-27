  class Exiv2::IptcData
    def to_hash
      result = {}

      self.each do |key, value|
        if result[key]
          if result[key].is_a? Array
            result[key] << value
          else
            result[key] = [result[key], value]
          end
        else
          result[key] = value
        end
      end
      result
    end
  end
