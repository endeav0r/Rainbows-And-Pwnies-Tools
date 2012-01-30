#!/usr/bin/lua

dofile("build/build.lua")

function make_rt ()
    for i, source in pairs(sources) do
        execute("gcc -O2 -c " .. source .. ".c -o " .. source .. ".o")
    end
    
    local objects = table.concat(object_files, " ")

    execute("gcc -O2 -o rt " .. objects .. " " .. CC_LIBS)
end

function make_librt ()

    for i, source in pairs(sources) do
        execute("gcc -O2 -c -fpic " .. source .. ".c -o " .. source .. ".o")
    end

    local objects = table.concat(object_files, " ")

    execute("gcc -O2 -shared -fpic -o rt.so " .. objects .. " " .. CC_LIBS)
end

make_rt() 
print("make_rt done")
make_librt()
print("make librt done")
