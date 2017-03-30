local polygonutils = {}

function polygonutils.transform(p, t)
	local q = {}
	for i,v in ipairs(p) do
		table.insert(q, {
			t[1][1]*v[1] + t[1][2]*v[2] + t[1][3],
			t[2][1]*v[1] + t[2][2]*v[2] + t[2][3]
		})
	end
	return q
end

function polygonutils.translate(p, v)
	return polygonutils.transform(p, { { 1, 0, v[1] }, { 0, 1, v[2] } })
end

function polygonutils.scale(p, s1, s2)
	if s2 == nil then
		return polygonutils.transform(p, { { s1, 0, 0 }, { 0, s1, 0 } })
	else
		return polygonutils.transform(p, { { s1, 0, 0 }, { 0, s2, 0 } })
	end
end

function polygonutils.rotate(p, angle)
	local cs = math.cos(angle)
	local sn = math.sin(angle)
	return polygonutils.transform(p, { { cs, -sn, 0 }, { sn, cs, 0 } })
end

function polygonutils.copy(p)
	local q = {}
	for i,v in ipairs(p) do
		table.insert(q, { v[1], v[2] })
	end
	return q
end

function polygonutils.convex_offset(u, d)
	local N = #u
	local v = polygonutils.copy(u)
    --Rm = [ 0 1; -1 0 ]; % 90 deg CW rotation
    for i = 1,N do
        local j = (i%N)+1
        local k = ((j)%N)+1
        -- Solve the vector equation
        --   j + d(j-i)^-perp + s(j-i) == j + d(k-j)^-perp - t(k-j)
        --   a + s x == b - t y
        --   s x + t y == b-a
        --   [ x y ] [ s ] == b-a
        --           [ t ]
        --   [ j-i k-j ] [ s ] == d [ (k-j)^-perp - (j-i)^-perp ]
        --               [ t ]
        
        local x = {
			u[j][1] - u[i][1],
			u[j][2] - u[i][2]
		}
		local xnorm = math.sqrt(x[1]*x[1] + x[2]*x[2])
		x[1] = x[1]/xnorm
		x[2] = x[2]/xnorm
        
        local y = {
			u[k][1] - u[j][1],
			u[k][2] - u[j][2]
		}
		local ynorm = math.sqrt(y[1]*y[1] + y[2]*y[2])
		y[1] = y[1]/ynorm
		y[2] = y[2]/ynorm
		local ba = { -- ba = d*Rm*(y-x)
			d * (y[2]-x[2]),
			d * (x[1]-y[1])
		}
		local idet = 1 / (x[1]*y[2] - x[2]*y[1])
		local st = { -- st = [ x y ]\ba;
			idet*(y[2]*ba[1]-y[1]*ba[2]),
			idet*(x[1]*ba[2]-x[2]*ba[1])
		}
        --v(:,j) = u(:,j) + d*Rm*x + st(1)*x;
        v[j][1] = u[j][1] + d*x[2] * st[1]*x[1]
        v[j][2] = u[j][2] - d*x[1] * st[1]*x[2]
    end
	return v
end

function polygonutils.contains(poly, pquery)
	local np = #poly
	local c = false
	for i = 1,np do
		local j = ((i)%np)+1
		if (
			((poly[i][2] > pquery[2]) ~= (poly[j][2] > pquery[2])) and
			(pquery[1] < (poly[j][1]-poly[i][1]) * (pquery[2]-poly[i][2]) / (poly[j][2]-poly[i][2]) + poly[i][1])
		) then
			c = not c
		end
	end
	return c
end

function polygonutils.convex(p)
	local n = #p
	for i = 1,n do
		local j = ((i)%n)+1
		local k = ((j)%n)+1
		local acx = p[i][1] - p[k][1]
		local bcx = p[j][1] - p[k][1]
		local acy = p[i][2] - p[k][2]
		local bcy = p[j][2] - p[k][2]
		if  (acx * bcy - acy * bcx) < 0 then
			return false
		end
	end
	return true
end

function polygonutils.area(p)
	local n = #p
	local area = 0
	for i = 1,n do
		local j = ((i)%n)+1
		area = area + (p[i][1]*p[j][2] - p[j][1]*p[i][2])
	end
	return 0.5*area
end

function polygonutils.perimeter(p)
	local n = #p
	local perim = 0
	for i = 1,np do
		local j = ((i)%n)+1
		local x = p[j][1] - p[i][1]
		local y = p[j][2] - p[i][2]
		perim = perim + math.sqrt(x*x+y*y)
	end
	return perim
end

function polygonutils.centroid(p)
	if nil == p then
		return
	end
	local n = #p
	local cx = 0
	local cy = 0
	local area = 0
	for i = 1,n do
		local j = ((i)%n)+1
		local aterm = p[i][1]*p[j][2] - p[j][1]*p[i][2]
		area = area + aterm
		cx = cx + (p[i][1]+p[j][1])*aterm
		cy = cy + (p[i][2]+p[j][2])*aterm
	end
	area = 3*area
	return { cx / area, cy / area }
end

function polygonutils.bound(p)
	if nil == p then
		return
	end
	local mn = {  math.huge,  math.huge }
	local mx = { -math.huge, -math.huge }
	for i,v in ipairs(p) do
		if v[1] < mn[1] then mn[1] = v[1] end
		if v[2] < mn[2] then mn[2] = v[2] end
		if v[1] > mx[1] then mx[1] = v[1] end
		if v[2] > mx[2] then mx[2] = v[2] end
	end
	return {
		{ mn[1], mn[2] },
		{ mx[1], mn[2] },
		{ mx[1], mx[2] },
		{ mn[1], mx[2] }
	}
end

return polygonutils
