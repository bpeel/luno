/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
        std::string sSourceUtf8;

        try
        {
            std::ifstream ifs(argv[i]);
            ifs.exceptions(std::ifstream::failbit);
            sSourceUtf8.append((std::istreambuf_iterator<char>(ifs)),
                               (std::istreambuf_iterator<char>()));
        }
        catch (const std::ios_base::failure& rFail)
        {
            std::cerr << sFilename << ": " << rFail.what() << std::endl;
            ret = 1;
            continue;
        }

        css::uno::Sequence<sal_Int8> sSourceSequence(
            reinterpret_cast<const sal_Int8*>(sSourceUtf8.data()), sSourceUtf8.size());

        try
        {
            xRunner->setCode(sFilename, sSourceSequence);
            xRunner->execute();
        }
        catch (const uk::co::busydoingnothing::luno::LuaException& e)
        {
            std::cerr << sFilename << ": " << e.Message << std::endl;
            ret = 1;
        }
    }

    return ret;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
