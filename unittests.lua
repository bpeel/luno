local serviceManager = XCONTEXT:getServiceManager()
local testHelper = serviceManager:createInstanceWithContext(
    "uk.co.busydoingnothing.luno.qa.TestHelper", XCONTEXT)

-- Helper function to check whether a type has been cached directly as
-- a global variable without having to use the __index mechanism
function isCachedGlobal(fullName)
    local startPos = 1;
    local searchTable = _G;

    while true do
        local endPos = string.find(fullName, ".", startPos, true);
        local length;

        if endPos == nil then
            endPos = -1
        else
            endPos = endPos - 1
        end

        local value = rawget(searchTable, string.sub(fullName, startPos, endPos))

        if value == nil then
            return false;
        end

        if endPos == -1 then
            return true;
        end

        startPos = endPos + 2
        searchTable = value;
    end
end

-- structs
do
    -- Contruct using the table constructor helper
    local structA = uk.co.busydoingnothing.luno.qa.TestStruct:new({
        LongValue = 3,
        StringValue = "three"
    });

    -- Make sure that the struct name is cached
    assert(isCachedGlobal("uk.co.busydoingnothing.luno.qa.TestStruct"));

    assert(structA.LongValue == 3);
    assert(structA.StringValue == "three");

    -- Construct without a table parameter
    local structB = uk.co.busydoingnothing.luno.qa.TestStruct:new()
    structB.LongValue = 4;
    structB.StringValue = "four";
    assert(structB.LongValue == 4);
    assert(structB.StringValue == "four");

    -- Test that inout struct parameters modify the passed in parameter
    local returnValue = testHelper:modifyStruct(33, structA,
                                                "quatre", structB);
    assert(structA.LongValue == 33);
    assert(structA.StringValue == "three");
    assert(structB.LongValue == 4);
    assert(structB.StringValue == "quatre");

    -- There shouldn’t be a return value if the only out parameters are struct inouts
    assert(returnValue == nil);
end
