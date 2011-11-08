require "activeresource"

file = File.new("routes.txt", "r")
result_xml = File.new(File.dirname(__FILE__) + "/routes.xml", "w")

routes = {}

file.lines.each do |l|
  data = l.split(/\,/)
  key_a = [data[3].to_s, data[5].to_s]
  next if key_a.include?('\N')
  key = key_a.sort.join(".")
  routes[key] = { :from => data[3], :to => data[5] }
end

result_xml.write(({ :routes => routes.values }).to_xml.to_s)
result_xml.close
