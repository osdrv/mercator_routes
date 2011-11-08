require "activeresource"

file = File.new("airports.txt", "r")
result_xml = File.new(File.dirname(__FILE__) + "/airports.xml", "w")

points = []

file.lines.each do |l|
  data = l.split(/\,/)
  id = data[0]
  lat = data[6]
  lon = data[7]
  lat = "#{lat}.0" if (lat =~ /\./).nil?
  lon = "#{lon}.0" if (lon =~ /\./).nil?
  next if !id || !lat || !lon
  points.push({ :id => id, :lat => lat, :lon => lon })
end

result_xml.write(({ :points => points }).to_xml.to_s)
result_xml.close
