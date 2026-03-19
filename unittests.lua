-- This file is part of Luno.
--
-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.

local serviceManager = XCONTEXT:getServiceManager()
local testHelper = uk.co.busydoingnothing.luno.qa.TestHelper:create(XCONTEXT)

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

--- issubclassof
assert(com.sun.star.lang.IllegalArgumentException:issubclassof(
       com.sun.star.uno.Exception))
assert(not com.sun.star.lang.IllegalArgumentException:issubclassof(
       com.sun.star.lang.NoSuchFieldException))

-- lunotype
assert(lunotype(uk.co.busydoingnothing.luno.qa.TestStruct:new()) ==
       uk.co.busydoingnothing.luno.qa.TestStruct)
-- Non struct/exception objects shouldn’t have a type
assert(lunotype(serviceManager) == nil)

-- Test catching an exception
do
    local ret, exception = pcall(function() testHelper:throwException() end)

    -- ret should be false indicating that an error occurred
    assert(ret == false)
    -- the exception should be wrapped into a userdata with the right type
    assert(lunotype(exception) == com.sun.star.lang.IllegalArgumentException)
    assert(exception.Message == "Your argument is illegal")
end

-- Helper functions to compare tables (but only if they are like arrays)
function isTableEqual(a, b)
    if #a ~= #b then return false end

    for i = 1, #a do
        if a[i] ~= b[i] then return false end
    end

    return true
end

function prettyTable(t)
    local strings = {}
    for k, v in ipairs(t) do
        strings[k] = tostring(v)
    end

    return "{" .. table.concat(strings, ",") .. "}"
end

function assertTableEqual(expected, actual)
    if not isTableEqual(expected, actual) then
        error("Table comparison failed\n  Expected: " ..
            prettyTable(expected) .. "\n  Actual:   " .. prettyTable(actual))
    end
end

-- Service constructors
do
    local TestHelper = uk.co.busydoingnothing.luno.qa.TestHelper
    local TestConstructors = uk.co.busydoingnothing.luno.qa.TestConstructors

    -- Default constructor should be called “create”
    assertTableEqual({}, TestHelper:create(XCONTEXT):getArguments())

    -- Multiple arguments
    do
        local obj = TestConstructors:multipleArguments(XCONTEXT, 42, " is half of ", 84.0)
        assertTableEqual({42, " is half of ", 84.0}, obj:getArguments())
    end

    -- Interface argument
    do
        local obj = TestConstructors:interfaceArgument(XCONTEXT, XCONTEXT)
        assertTableEqual({XCONTEXT}, obj:getArguments())
    end

    -- null interface argument
    do
        -- nil should be coerced into a null reference even though normally it’s supposed to
        -- represent the void type
        local obj = TestConstructors:interfaceArgument(XCONTEXT, nil)
        assertTableEqual({nil}, obj:getArguments())
    end

    -- Rest arguments, empty
    do
        local obj = TestConstructors:restArgument(XCONTEXT)
        assertTableEqual({}, obj:getArguments())
    end

    -- Rest arguments, some
    do
        local obj = TestConstructors:restArgument(XCONTEXT, 1, 2, "buckle my shoe")
        assertTableEqual({1, 2, "buckle my shoe"}, obj:getArguments())
    end

    -- Missing context argument
    do
        local ret, e = pcall(function() TestHelper:create() end)
        assert(not ret)
        assert(e == "uk.co.busydoingnothing.luno.qa.TestHelper::create requires 1 argument")
    end

    -- Missing both arguments
    do
        local ret, e = pcall(function() TestConstructors:interfaceArgument() end)
        assert(not ret)
        assert(e == "uk.co.busydoingnothing.luno.qa.TestConstructors::interfaceArgument requires "
               .. "2 arguments")
    end

    -- Too many arguments
    do
        local ret, e = pcall(function() TestConstructors:interfaceArgument(XCONTEXT, 1, 2) end)
        assert(not ret)
        assert(e == "uk.co.busydoingnothing.luno.qa.TestConstructors::interfaceArgument requires "
               .. "2 arguments")
    end

    -- Missing context when there are rest arguments
    do
        local ret, e = pcall(function() TestConstructors:restArgument() end)
        assert(not ret)
        assert(e == "uk.co.busydoingnothing.luno.qa.TestConstructors::restArgument requires "
               .. "at least 1 argument")
    end

    -- Wrong type for the context
    do
        local ret, e = pcall(function() TestHelper:create(true) end)
        assert(not ret)
        assert(e == "First argument to a service constructor must be an XComponentContext")
    end

    -- Passing the wrong type
    do
        local ret, e = pcall(function() TestConstructors:interfaceArgument(XCONTEXT, 12) end)
        assert(not ret)
        assert(lunotype(e):issubclassof(com.sun.star.script.CannotConvertException))
    end

    -- The service constructors should be cached so that the same object is returned every time they
    -- are retrieved
    assert(TestHelper.create == TestHelper.create)
    assert(TestConstructors.interfaceArgument ~= TestHelper.restArgument)
end
