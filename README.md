# Luno

Luno is a work-in-progress LibreOffice extension to allow scripting in the [Lua](https://lua.org) language.

## Building

First install the Lua development package for your distro. On Fedora this is in the `lua-devel` package. The make files for Luno assume there is a working pkg-config file for Lua, which isn’t the case if you install it from the official source code.

Next, make sure you have the LibreOffice SDK installed. On Fedora this is in the `libreoffice-sdk` package. If you’re building LibreOffice from source make sure you have the `--enable-odk` in your `autogen.input`.

There should be a script called `setsdkenv_unix`. On Fedora this is installed in `/usr/lib64/libreoffice/sdk`. If you haven’t already, run the script and answer all of the configuration questions. This will create another script in your home directory which should be something like `~/libreoffice26.2_sdk/yourhostname/setsdkenv_unix.sh`. Source the script into your current shell environment like this:

```bash
source "$HOME/$(cd ~ && ls -d libreoffice??.?_sdk -t | head -n 1)/$HOSTNAME/setsdkenv_unix.sh"
```

Now you should be able to build the extension by just running `make`. This should also magically install the extension onto your LibreOffice installation.

## Using

Luno is currently only an experiment. The only way to use it for now is it copy some Lua source code into a Writer document, select it all and then click on “Run Lua Code” from the “Tools→AddOn” menu. The source code can use the builtin `XSCRIPTCONTEXT` global variable to start accessing UNO objects. Note that this is currently just the `XComponentContext` and not the `XScriptContext` like it is for Python and Basic.

## Example

Here is some example code to insert some text onto the end of the current Writer document:

```
local desktop = XSCRIPTCONTEXT:getValueByName(
  "/singletons/com.sun.star.frame.theDesktop");
local frame = desktop:getCurrentFrame();
local controller = frame:getController();
local model = controller:getModel();
local text = model:getText();
text:getEnd():setString("This text came from Lua!");
```

## Name

Lua is the Portuguese word for moon and Luno is the Esperanto word for moon. It also looks like a combination of Lua+UNO, where UNO is the name of the object system used by the LibreOffice SDK.
