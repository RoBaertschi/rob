local function dump(o)
	if type(o) == "table" then
		local s = "{ "

		for k, v in pairs(o) do
			if type(k) ~= "number" then
				k = '"' .. k .. '"'
			end
			s = s .. "[" .. k .. "] = " .. dump(v) .. ","
		end
		return s .. "} "
	else
		return tostring(o)
	end
end

local s = "src/"
local srcs = { s .. "main.c", s .. "luadsl.c", s .. "utils.c", s .. "rdir.c" }

local cc_options = rob.c.ccOptions()
cc_options:includeDir("libs/include")
local objs = rob.c.buildObjs(srcs, cc_options)
print(dump(objs))
