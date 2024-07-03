local function serialize(table)
    local serializedValues = {}
    for key, value in pairs(table) do
        local serializedValue
        if type(value) == "table" then
            serializedValue = serialize(value)
        elseif type(value) == "string" then
            serializedValue = string.format("%q", value)
        else
            serializedValue = tostring(value)
        end
        table.insert(serializedValues, string.format("[%q] = %s", key, serializedValue))
    end
    return "{ " .. table.concat(serializedValues, ", ") .. " }"
end

local function updateField(config, path, value)
    local fields = {}
    for field in string.gmatch(path, "[^%.]+") do
        table.insert(fields, field)
    end

    local t = config
    for i = 1, #fields - 1 do
        t = t[fields[i]]
    end
    t[fields[#fields]] = value
end

local function updateConfig(updates)
    local config = dofile("conf.lua")

    for path, value in pairs(updates) do
        updateField(config, path, value)
    end

    local file = io.open("conf.lua", "w")
    if file then
        file:write("local customization = ")
        file:write(serialize(config))
        file:write("\nreturn customization")
        file:close()
    else
        print("Error: Unable to open conf.lua for writing")
    end
end

return {
    updateConfig = updateConfig
}