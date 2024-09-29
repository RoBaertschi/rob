-- local srcs = rob.glob("src/*.c")
local s = "src/"
local srcs = { s .. "main.c", s .. "luadsl.c", s .. "utils.c", s .. "rdir.c" }

local cc_options = rob.c.ccOptions()
cc_options:includeDir("libs/include")
local objs = rob.c.buildObjs(srcs, cc_options)

local make_target = rob.utils.make("./lua-5.4.7/", "install")

local link_options = rob.c.linkOptions()

link_options:libraryDir("libs/lib")
link_options:linkLibrary("lua")

local rob_target = rob.ld.linkBin("rob", objs, link_options)
rob_target:dependsOn(make_target)

return rob.project({
	name = "rob",
	languages = { "c" },
	targets = { rob_target },
})
