local polygonutils = {}
--local predicates = require('geompredicates')

local orient2d = function(a, b, c)
	return ((b[1]-a[1])*(c[2]-a[2]) - (b[2]-a[2])*(c[1]-a[1]))
end
--local orient2d = predicates.orient

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

function polygonutils.orient(p, neg)
	local a = polygonutils.area(p)
	local ng = neg or false
	if ((ng and a > 0) or ((not ng) and a < 0)) then
		local n = #p
		local r = {}
		for i = 1,n do
			r[i] = { p[n-i+1][1], p[n-i+1][2] }
		end
		return r
	end
	return p
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

function polygonutils.sanitize(p, tol)
	local droptol = tol or 0
	local n = #p
	-- remove duplicates
	local q = {}
	for i = 1, n do
		local j = i+1
		if j > n then j = 1 end
		if p[i][1] ~= p[j][1] or p[i][2] ~= p[j][2] then
			table.insert(q, {p[i][1], p[i][2]})
		end
	end
	-- check for collinear segments
	local coll = {}
	n = #q
	local any_collinear = false
	for j = 1,n do
		local i = j-1
		if i < 1 then i = n end
		local k = j+1
		if k > n then k = 1 end
		if math.abs(orient2d(q[i], q[j], q[k])) < droptol then
			coll[j] = true
			any_collinear = true
		else
			coll[j] = false
		end
	end
	if not any_collinear then return q end
	local r = {}
	for j = 1,n do
		if not coll[j] then
			table.insert(r, q[j])
		end
	end
	return polygonutils.orient(r)
end

-- Given a set of points, compute the convex hull of the set
-- The input point set may be re-arranged.
function polygonutils.convexhull(pt)
	local n = #pt
	if n <= 3 then
		return pt
	end
	
	-- Find lowest and remove it
	local first
	local p = {}
	do
		local m = 1
		for i = 2,n do
			if (pt[i][2] < pt[m][2]) or ((pt[i][2] == pt[m][2]) and (pt[i][1] > pt[m][1])) then
				m = i
			end
		end
		first = { pt[m][1], pt[m][2] }
		for i = 1,n do
			if i ~= m then
				table.insert(p, {pt[i][1], pt[i][2]})
			end
		end
	end
	
	-- Sort by angle
	table.sort(p, function(a,b)
		local area = orient2d(first, a, b)
		if area > 0 then
			return true
		elseif area < 0 then
			return false
		else
			local ax = a[1]-first[1]
			local ay = a[2]-first[2]
			local bx = b[1]-first[1]
			local by = b[2]-first[2]
			return ax*ax+ay*ay < bx*bx+by*by
		end
	end)
	p[0] = first
	
	-- Graham scan
	local S = { n-1, 0 }
	local i = 1
	while i < n do
		local area = orient2d(p[S[#S-1]], p[S[#S]], p[i])
		if area > 0 then
			table.insert(S, i)
			i = i+1
		else
			table.remove(S)
		end
	end
	table.remove(S)
	local ret = {}
	for i,ind in ipairs(S) do
		ret[i] = p[ind]
	end
	return ret
end

function polygonutils.offset(u, dist)
	local n = #u
	local v = {}
	for i = 1,n do
		local j = i+1
		if j > n then j = 1 end
		local k = j+1
		if k > n then k = 1 end
		v[j] = {}
		-- Solve the vector equation
		--   j + d(j-i)^-perp + s(j-i) == j + d(k-j)^-perp - t(k-j)
		--   a + s x == b - t y
		--   s x + t y == b-a
		--   [ x y ] [ s ] == b-a
		--           [ t ]
		--   [ j-i k-j ] [ s ] == d [ (k-j)^-perp - (j-i)^-perp ]
		--               [ t ]
		local x = { u[j][1] - u[i][1], u[j][2] - u[i][2] }
		local nrm = 1 / math.sqrt(x[1]*x[1] + x[2]*x[2])
		x[1] = x[1] * nrm
		x[2] = x[2] * nrm
		local y = { u[k][1] - u[j][1], u[k][2] - u[j][2] }
		nrm = 1 / math.sqrt(y[1]*y[1] + y[2]*y[2])
		y[1] = y[1] * nrm
		y[2] = y[2] * nrm
		local ba = { dist*(y[2]-x[2]), dist*(x[1]-y[1]) }
		local xy = (x[1]*y[2] - x[2]*y[1])
		--local st = { (y[2]*ba[1] - y[1]*ba[2])/xy, (x[1]*ba[2]-x[2]*ba[1])/xy }
		local s = 0
		if 0 ~= xy then
			s = (y[2]*ba[1] - y[1]*ba[2]) / xy
		end
		--print(x[1], x[2], y[1], y[2], s)
		v[j][1] = u[j][1] + dist*x[2] + s * x[1]
		v[j][2] = u[j][2] - dist*x[1] + s * x[2]
	end
	return v
end

function polygonutils.extremepoint(p, dir)
	local n = #p
	if n < 1 then return end
	local ibest = 1
	local dbest = p[1][1]*dir[1] + p[1][2]*dir[2]
	for i = 2,n do
		local d = p[i][1]*dir[1] + p[i][2]*dir[2]
		if d > dbest then
			dbest = d
			ibest = i
		end
	end
	return ibest, p[ibest]
end

-- n = number of vertices
-- r = radius
-- offset: 0 means first vertex is (r, 0)
--         1 means first vertex is (r*cos(2pi/n), r*sin(2pi/n))
--         else first vertex is (r*cos(2pi/n * offset), r*sin(2pi/n * offset))
function polygonutils.regular(n, r, offset)
	local off = offset or 0
	local q = 2*math.pi/n
	local c = math.cos(q)
	local s = math.sin(q)
	local p = {
		{ r*math.cos(off*q), r*math.sin(off*q) }
	}
	for i = 2,n do
		p[i] = {
			c * p[i-1][1] - s * p[i-1][2],
			c * p[i-1][2] + s * p[i-1][1],
		}
	end
	return p
end

function polygonutils.normal(p, j)
	local n = #p
	local i = j-1
	if i < 1 then i = n end
	local k = j+1
	if k > n then k = 1 end
	local a = { p[j][1] - p[i][1], p[j][2] - p[i][2] }
	local n = 1 / math.sqrt(a[1]*a[1] + a[2]*a[2])
	a[1] = n*a[1]
	a[2] = n*a[2]
	local b = { p[k][1] - p[j][1], p[k][2] - p[j][2] }
	local n = 1 / math.sqrt(b[1]*b[1] + b[2]*b[2])
	b[1] = n*b[1]
	b[2] = n*b[2]
	local c = { a[1]+b[1], a[2]+b[2] }
	local n = 1 / math.sqrt(c[1]*c[1] + c[2]*c[2])
	return { c[2]*n, -c[1]*n }
end

return polygonutils
