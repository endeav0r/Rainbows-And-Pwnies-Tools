-- nibble to hex
function n2h (n)
    if n == 0 then
        return "0"
    elseif n == 1 then
        return "1"
    elseif n == 2 then
        return "2"
    elseif n == 3 then
        return "3"
    elseif n == 4 then
        return "4"
    elseif n == 5 then
        return "5"
    elseif n == 6 then
        return "6"
    elseif n == 7 then
        return "7"
    elseif n == 8 then
        return "8"
    elseif n == 9 then
        return "9"
    elseif n == 10 then
        return "a"
    elseif n == 11 then
        return "b"
    elseif n == 12 then
        return "c"
    elseif n == 13 then
        return "d"
    elseif n == 14 then
        return "e"
    elseif n == 15 then
        return "f"
    else
        return "?"
    end
end

-- byte to hex
function b2h (b)
    return n2h(math.floor(b / 16)) .. n2h(b % 16)
end

-- word to hex
function w2h (w)
    result =           b2h(math.floor(w / (256*256*256)) % 256)
    result = result .. b2h(math.floor(w / (256*256)) % 256)
    result = result .. b2h(math.floor(w / 256) % 256)
    result = result .. b2h(w % 256)
    return result
end

-- big to little endian word
function b2l (w)
    result = (w % 256) * 256 * 256 * 256
    result = result + ((math.floor(w / 256) % 256) * 256 * 256)
    result = result + ((math.floor(w / (256 * 256)) % 256) * 256)
    result = result + (math.floor(w / (256 * 256 * 256)) % 256)
    return result
end
