module SharedMethods
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

  def inspect
    items = []
    self.to_hash.sort.each do |k,v|
      items << %Q["#{k}"=>#{v.inspect}]
    end
    "#<#{self.class.name}: {#{items.join(', ')}}>"
  end
  
  def [](key) 
    self.to_hash[key]
  end
  
  def []=(key, value)
    delete_all(key)
    if value.is_a?(Array)
      value.each do |v|
        self.add(key, v)
      end
    else
      self.add(key, value)
    end
  end
  
  def delete_all(key)
    del = true
    while(del) do
      del = self.delete(key)
    end 
  end
end
