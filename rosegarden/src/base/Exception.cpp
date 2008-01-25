// -*- c-basic-offset: 4 -*-

/*
    Rosegarden
    A sequencer and musical notation editor.

    This program is Copyright 2000-2008
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <bownie@bownie.com>

    The moral right of the authors to claim authorship of this work
    has been asserted.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#include "Exception.h"

#include <iostream>

namespace Rosegarden {

Exception::Exception(std::string message) :
    m_message(message)
{
#ifndef NDEBUG
    std::cerr << "WARNING: Rosegarden::Exception: \""
	      << message << "\"" << std::endl;
#endif
}

Exception::Exception(std::string message, std::string file, int line) :
    m_message(message)
{
#ifndef NDEBUG
    std::cerr << "WARNING: Rosegarden::Exception: \""
	      << message << "\" at " << file << ":" << line << std::endl;
#endif
}

}
