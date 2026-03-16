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

-- constants
do
    -- Make an alias for the constants module
    local TestConstants = uk.co.busydoingnothing.luno.qa.TestConstants;
    assert(TestConstants.LONG_CONSTANT == 12345678);

    -- The constants should be added to the global cache even if they are accessed through an alias
    assert(isCachedGlobal("uk.co.busydoingnothing.luno.qa.TestConstants.LONG_CONSTANT"));

    assert(math.abs(TestConstants.FLOAT_CONSTANT - 3.141592654) < 0.001);
end

-- Test that an inout sequence parameter modifies the argument
do
    local fourValues = { 1, 2, 3, 4 };
    local returnValue = testHelper:modifySequence(7, 8, 9, fourValues);
    assert(#fourValues == 4);
    assert(fourValues[1] == 7);
    assert(fourValues[2] == 8);
    assert(fourValues[3] == 9);
    assert(fourValues[4] == 4);

    -- There shouldn’t be a return value if the only out parameters are sequence inouts
    assert(returnValue == nil);
end

-- multiple return values
do
    local r1, r2, r3, r4 = testHelper:multipleReturn(
        1, -- mainReturnValue
        2, -- secondReturnValue
        4, -- fourthInputThirdOutput
        3 -- thirdReturnValue
    );

    assert(r1 == 1);
    assert(r2 == 2);
    assert(r3 == 3);
    assert(r4 == 4);
end

-- enums
do
    local zero = uk.co.busydoingnothing.luno.qa.TestEnum.ZERO
    local one = uk.co.busydoingnothing.luno.qa.TestEnum.ONE
    local two = uk.co.busydoingnothing.luno.qa.TestEnum.TWO
    local four = uk.co.busydoingnothing.luno.qa.TestEnum.FOUR

    assert(zero.value == 0)
    assert(one.value == 1)
    assert(two.value == 2)
    assert(four.value == 4)

    -- the same object should be returned the second time the enum is accessed
    assert(zero == uk.co.busydoingnothing.luno.qa.TestEnum["ZERO"])

    assert(zero ~= one)

    -- a new object returned from the API should still be equal
    assert(testHelper:getFourEnum() == four)

    -- test passing values into UNO
    assert(testHelper:getEnumValue(two) == 2);
end

-- Objects returned multiple times from the SDK should be equal to each other
assert(XCONTEXT:getServiceManager() == serviceManager);
-- different objects shouldn’t be equal
assert(serviceManager ~= testHelper);
assert(serviceManager ~= 3);
