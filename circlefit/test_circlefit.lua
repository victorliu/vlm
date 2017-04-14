circlefit = require('circlefit')

print('%!')
print('72 72 scale')
print('4.25 5.5 translate')
print('0.002 setlinewidth')

pt = {}
for i = 1,10 do
	local angle = 2*math.pi * math.random()
	local r = 1 + 0.1 * math.random()
	local p = {
		r * math.cos(angle),
		r * math.sin(angle)
	}
	table.insert(pt, p)
	print('newpath', p[1], p[2], '0.01 0 360 arc closepath fill')
end

c, r = circlefit.fit(pt);
print('newpath', c[1], c[2], r, '0 360 arc closepath stroke')
print('showpage')
