local table_stats = {}

function table_stats.summarize(tab)
	local ret = {}
	local ret.sum = 0
	local ret.n = #tab
	ret.min = math.huge
	ret.max = -math.huge
	for i,v in ipairs(tab)
		ret.sum = ret.sum + v
		if v < ret.min then
			ret.min = v
		end
		if v > ret.max then
			ret.max = v
		end
	end
	ret.mean = ret.sum / ret.n
	local ssq = 0
	for i,v in ipairs(tab)
		local diff = v - ret.mean
		ssq = ssq + diff*diff
	end
	ret.stdev = math.sqrt(ssq/ret.n)
	return ret
end

return table_stats
