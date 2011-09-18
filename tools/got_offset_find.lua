--[[
Used for finding ROP gadgets in libc when you can only add so many bits to
memory. Give it your target binary and libc, how many bytes +- the offsets in
libc that will be loaded in your binary's GOT, the depth you want to search at
in ROP instructions, and optionally the mnemonics you're searching for. This
script will do the rest.

18 September, 2011
endeavor@rainbowsandpwnies.com
]]

dofile("tools/rop_tools.lua")

ROP_OFFSET_SIZE = 256
DEPTH = 2
TARGET_MNEMONICS = {"add", "sub"} -- set to nil to show all possibilities
LIBC_FILENAME = "/lib/i386-linux-gnu/libc-2.13.so"
TARGET_FILENAME = "/path/to/target"

libc = elf_open(LIBC_FILENAME)
sublog = elf_open(TARGET_FILENAME)

-- find all symbols that are GLIBC in TARGET
glibc_symbols = {}
for k, sec in pairs(sublog["sections"]) do
    if sec["type"] == "symtab" then
        for i, symbol in pairs(sec["symbols"]) do
            if string.find(symbol["name"], "@@GLIBC") then
                glibc_symbols[symbol["name"]] = symbol
            end
        end
    end
end

-- cut out the @@GLIBC part
for name, symbol in pairs(glibc_symbols) do
    offset = string.find(name, "@@GLIBC")
    symbol["name"] = string.sub(symbol["name"], 0, offset - 1)
end

-- print out our symbols we're looking for
for name, symbol in pairs(glibc_symbols) do
    print("looking for " .. symbol["name"])
end

-- go ahead and search through libc symbols and pull out symbols that match
libc_symbols = {}
for k, sec in pairs(libc["sections"]) do
    if sec["type"] == "dynsym" then
        for i, symbol in pairs(sec["symbols"]) do
            for j, glibc_symbol in pairs(glibc_symbols) do
                if symbol["name"] == glibc_symbol["name"] then
                    libc_symbols[symbol["name"]] = symbol
                end
            end
        end
    end
end

-- print out our addresses we found in libc
for name, symbol in pairs(libc_symbols) do
    print("found " .. name .. " with offset " .. w2h(symbol["address"]))
end

-- get our rops from libc
print("getting rops from libc, may take a few seconds")
rop_table = make_rop_table(LIBC_FILENAME, DEPTH)

print("finding rops within offset")
valid_rops = {}
-- find all rops that are within ROP_OFFSET_SIZE from the addresses in libc_symbols
for k, rop in pairs(rop_table) do
    for k, libc_symbol in pairs(libc_symbols) do
        if not rop_added then
            if rop[1]["address"] < libc_symbol["address"] + ROP_OFFSET_SIZE and 
               rop[1]["address"] > libc_symbol["address"] - ROP_OFFSET_SIZE then
                offset = rop[1]["address"] - libc_symbol["address"]
                if offset < 0 then
                    sign = "-"
                    offset = offset * -1
                else
                    sign = "+"
                end
                valid_rops[libc_symbol["name"] .. " " .. sign .. w2h(offset)] = rop
                break
            end
        end
    end
end

-- if we have a target mnemonic, get rid of all rops that don't have it
if TARGET_MNEMONICS then
    valid = {}
    for name, valid_rop in pairs(valid_rops) do
        accept = false
        for k, rop in pairs(valid_rop) do
            for k, mnemonic in pairs(TARGET_MNEMONICS) do
                if rop["mnemonic"] == mnemonic then
                    accept = true
                    break
                end
            end
        end
        if accept then
            valid[name] = valid_rop
        end
    end
    valid_rops = valid
end

-- print findings
for name, valid_rop in pairs(valid_rops) do
    print(name)
    for i, rop in pairs(valid_rop) do
        print("  " .. w2h(rop["address"]) .. " " .. rop["description"])
    end
end
