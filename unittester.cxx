// Luno – LibreOffice bindings for Lua
// Copyright (C) 2026  Neil Roberts
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <cppuhelper/bootstrap.hxx>
#include <uk/co/busydoingnothing/luno/LuaException.hpp>
#include <uk/co/busydoingnothing/luno/Runner.hpp>
#include <sal/main.h>
#include <fstream>
#include <string>
#include <iostream>

SAL_IMPLEMENT_MAIN_WITH_ARGS(argc, argv)
{
    css::uno::Reference<css::uno::XComponentContext> xContext(
        cppu::defaultBootstrap_InitialComponentContext());
    if (!xContext.is())
        throw cppu::BootstrapException(rtl::OUString("no local component context!"));

    int ret = 0;

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
            continue;

        css::uno::Reference<uk::co::busydoingnothing::luno::XRunner> xRunner =
            uk::co::busydoingnothing::luno::Runner::create(xContext);

        rtl::OUString sFilename(argv[i], strlen(argv[i]), RTL_TEXTENCODING_UTF8);

        std::ifstream ifs(argv[i]);
        std::string sSourceUtf8((std::istreambuf_iterator<char>(ifs)),
                                (std::istreambuf_iterator<char>()));
        rtl::OUString sSource(sSourceUtf8.c_str(), sSourceUtf8.size(), RTL_TEXTENCODING_UTF8);

        try
        {
            xRunner->executeCode(sFilename, sSource);
        }
        catch (const uk::co::busydoingnothing::luno::LuaException& e)
        {
            std::cerr << sFilename << ": " << e.Message << std::endl;
            ret = 1;
        }
    }

    return ret;
}
