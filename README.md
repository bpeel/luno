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

Luno is currently only an experiment. The only way to use it for now is it copy some Lua source code into a Writer document, select it all and then click on “Run Lua Code” from the “Tools→AddOn” menu. The source code can use the builtin `XCONTEXT` global variable to access `XComponentContext` and find UNO services.

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
