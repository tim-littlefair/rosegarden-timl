// -*- c-basic-offset: 4 -*-

/*
    Rosegarden
    A sequencer and musical notation editor.
    Copyright 2000-2008 the Rosegarden development team.
    See the AUTHORS file for more details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#include "LayoutEngine.h"

namespace Rosegarden 
{

LayoutEngine::LayoutEngine() :
    m_status(0)
{
    // empty
}

LayoutEngine::~LayoutEngine()
{
    // empty
}


HorizontalLayoutEngine::HorizontalLayoutEngine(Composition *c) :
    LayoutEngine(),
    RulerScale(c)
{
    // empty
}

HorizontalLayoutEngine::~HorizontalLayoutEngine()
{
    // empty
}


VerticalLayoutEngine::VerticalLayoutEngine() :
    LayoutEngine()
{
    // empty
}

VerticalLayoutEngine::~VerticalLayoutEngine()
{
    // empty
}

}
