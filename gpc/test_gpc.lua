GPC = require('GPC')
inspect = require('inspect')

function print_poly(p, op)
	local trs = GPC.tristrip(p)
	for i,strip in ipairs(trs) do
		local n = #strip
		local flip = false
		for j = 3,n do
			if not flip then
				print(strip[j-2][1], strip[j-2][2], 'moveto')
				print(strip[j-1][1], strip[j-1][2], 'lineto')
			else
				print(strip[j-1][1], strip[j-1][2], 'moveto')
				print(strip[j-2][1], strip[j-2][2], 'lineto')
			end
			print(strip[j  ][1], strip[j  ][2], 'lineto')
			flip = not flip
			print('closepath', op)
		end
	end
end

print('%!')
print('72 72 scale')
print('4.25 4 translate')
print('0.01 setlinewidth')

local p1 = {
	{ -1, -1 },
	{  1, -1 },
	{  1,  1 },
	{ -1,  1 }
}
local p2 = {
	{ 1.5, 0 },
	{ 0, 1.5 },
	{ -1.5, 0 },
	{ 0, -1.5 }
}

print('1 0 0 setrgbcolor');
print_poly(p1, 'fill');
print('0 1 0 setrgbcolor');
print_poly(p2, 'fill');

local p3 = GPC.difference(p1, p2)
print(inspect(p3))
print('0 0 1 setrgbcolor');
print_poly(p3, 'fill');


print('showpage')


