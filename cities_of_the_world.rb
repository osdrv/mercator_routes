require "nokogiri"
require "activeresource"

doc = Nokogiri::XML(File.read(File.dirname(__FILE__) + "/cities_of_the_world.xml"))

points = []

doc.css("coordinates").each do |node|
  lat, lon, _a = node.content.split(/,/)
  points.push({ :lat => lat, :lon => lon })
end

result_xml = File.new(File.dirname(__FILE__) + "/result.xml", "w")
result_xml.write(({ :points => points }).to_xml.to_s)
result_xml.close