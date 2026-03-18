# Luno

Luno is a work-in-progress LibreOffice extension to allow scripting in the [Lua](https://lua.org) language.

## Building

Make sure you have the LibreOffice SDK installed. On Fedora this is in the `libreoffice-sdk` package. If you’re building LibreOffice from source make sure you have the `--enable-odk` in your `autogen.input`.

There should be a script called `setsdkenv_unix`. On Fedora this is installed in `/usr/lib64/libreoffice/sdk`. If you haven’t already, run the script and answer all of the configuration questions. This will create another script in your home directory which should be something like `~/libreoffice26.2_sdk/yourhostname/setsdkenv_unix.sh`. Source the script into your current shell environment like this:

```bash
source "$HOME/$(cd ~ && ls -d libreoffice??.?_sdk -t | head -n 1)/$HOSTNAME/setsdkenv_unix.sh"
```

Now you should be able to build the extension by just running `make`. This should also magically install the extension onto your LibreOffice installation.

If you want to additionally run some unit tests you can type `make check`.

## Using

Once the extension is installed you can create Lua macros by adding files in the right directory. On Linux this will probably be `~/.config/libreoffice/4/user/Scripts/lua`. Add your script file to that directory with the extension `.lua`. Note that unlike pyuno, each script file can only have a single macro and the code for it is the entire file. Don’t put the macro code into a function, although you can of course define functions that the macro will use. Once the script file is created you can find the macro in Tools→Macros→Organize Macros→Lua. Once you find the macro in the tree you can select it and press “Run” to run it. You should also be able to find the macro in the keyboard customization dialog where you can assign a shortcut for it.

The macro source code can use the builtin `XCONTEXT` global variable to access `XComponentContext` and find UNO services.

### Types

The types from UNO are exposed as global variables in Lua so you can just access them like this:

```lua
interface = com.sun.star.uno.XInterface
```

The module parts leading up to the type are real objects so you can store them if you want. You could use this to make a convenient alias like this:

```lua
css = com.sun.star
assert(css.uno.XInterface == com.sun.star.uno.XInterface)
```

### Methods

Methods can be called on an object using the “:” operator like this:

```lua
text:insertTextContent(range, content, false)
```

If the method has out parameters they will all be returned in order as multiple return values. For example, if a method is defined like this in the IDL:

```
long getThreeNumbers([out] long secondNumber, [out] long thirdNumber);
```

then you can call it like this:

```lua
mainNumber, secondNumber, thirdNumber = foo:getThreeNumbers()
```

If the method has inout parameters then the outputs will be returned as multiple return values as above, _unless_ the type is a struct or a sequence. In that case the passed in object will be updated with the new values instead. For example to use an `XURLTransformer` you can call it like this:

```lua
local url = com.sun.star.util.URL:new({Complete="https://libreoffice.org"})
urlTransformer:parseStrict(url)
assert(url.Protocol == "https://")
```

Note that this is different from pyuno which would return the new URL as one of the return values.

### Sequences

Sequences are converted to and from Lua tables. Note that the indices start from 1 instead of 0 as is the custom in Lua. Eg:

```lua
-- Prints the first available service
print(XCONTEXT:getServiceManager():getAvailableServiceNames()[1])
```

### Structs

Structs are handled as userdata objects instead of tables. You can still access all the members with the same syntax as for tables though, but it will stop you from adding members that aren’t in the struct. You can construct a struct with the `new` method on the type, eg:

```lua
pv = com.sun.star.beans.PropertyValue:new()
pv.Name = "Colour"
pv.Value = "Green"
```

The constructor can also take a single table parameter to set the values upfront, eg:

```lua
pv = com.sun.star.beans.PropertyValue:new({Name = "Colour", Value = "Green"})
assert(pv.Name == "Colour")
```

### Enums

Enums are userdata values that can be accessed like this:

```lua
anchorType = com.sun.star.text.TextContentAnchorType.AT_PAGE
```

You can make an alias for the enum type like this to save typing:

```lua
TextContentAnchorType = com.sun.star.text.TextContentAnchorType
assert(TextContentAnchorType.AT_PAGE ==
       com.sun.star.text.TextContentAnchorType.AT_PAGE)
```

You can get the integer value of an enum with the `value` property:

```lua
assert(com.sun.star.text.TextContentAnchorType.AT_PAGE.value == 2)
```

### Constants

Constants are accessible directly and have whatever Lua type most closely resembles the type they are defined as (in practice this is probably always a Lua number). Eg:

```lua
assert(com.sun.star.awt.FontWeight.BOLD == 150)
```

### Exceptions

Exceptions thrown when calling a method on an UNO object are wrapped into a userdata value and then set as a Lua error. You can catch the errors with the builtin `pcall` function. This calls the given function and adds a boolean return value. If the boolean is false then an error occurred and you can get access to the error object as the second return value. You can then use the `lunotype` function to get access to the type of the error and use the `issubclassof` method to check if the error object is a subclass of the type you’re looking for. For example:

```lua
local ret, exception = pcall(
    function()
        model:getPropertyValue("notARealProperty")
    end
)

if not ret and lunotype(exception):issubclassof(
    com.sun.star.beans.UnknownPropertyException) then
    print("Caught UnknownPropertyException: " .. exception.Message)
end
```

## Example

Here is some example code to insert text and shapes onto the end of the current Writer document:

```lua
local desktop = XCONTEXT:getValueByName(
  "/singletons/com.sun.star.frame.theDesktop");
local frame = desktop:getCurrentFrame();
local controller = frame:getController();
local model = controller:getModel();
local text = model:getText()
local drawPage = model:getDrawPage();

text:getEnd():setString("\n\n")

-- Make an alias for the Size type so we can construct it more easily
local Size = com.sun.star.awt.Size;

function addSquares(reverse)
    for i = 1, 10 do
        local size = i;
        if reverse then
            size = 11 - i
        end
        local rect = model:createInstance("com.sun.star.drawing.RectangleShape");
        rect:setSize(Size:new({Width = size * 110, Height = size * 110}));
        rect:setPropertyValue("TextRange", text:getEnd())
        drawPage:add(rect)
    end
end

addSquares(false)

local cursor = text:createTextCursorByRange(text:getEnd())
cursor:setString("Luno")
cursor:setPropertyValue("CharWeight", com.sun.star.awt.FontWeight.BOLD)
cursor:setPropertyValue("CharHeight", 50)

addSquares(true)
```

## Name

Lua is the Portuguese word for moon and Luno is the Esperanto word for moon. It also looks like a combination of Lua+UNO, where UNO is the name of the object system used by the LibreOffice SDK.
