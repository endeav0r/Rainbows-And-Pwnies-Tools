#!/usr/bin/lua

dofile("build/build.lua")

for i, source in pairs(sources) do
    os.execute("rm " .. source .. ".o")
end
