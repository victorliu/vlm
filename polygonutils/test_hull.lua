polygonutils = require('polygonutils2')

math.randomseed(os.time())
input = {}
for i = 1,30 do
	table.insert(input, {math.random(), math.random()})
end

out = polygonutils.convexhull(input)




print('%!')
print('72 dup dup scale 1 exch div setlinewidth')
print('3 4 translate')

for i,p in ipairs(input) do
	print('newpath', p[1], p[2], '0.01 0 360 arc fill')
end
n = #out
print(out[1][1], out[1][2], 'moveto')
for i = 2,n do
	print(out[i][1], out[i][2], 'lineto')
end
print('closepath stroke')
print('showpage')

