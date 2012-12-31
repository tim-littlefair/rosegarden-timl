/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2013 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _MARKERMAPPER_H_
#define _MARKERMAPPER_H_

#include "SpecialSegmentMapper.h"


namespace Rosegarden
{
class RosegardenDocument;

// @class mapper for markers.  We don't store a copy of the marker
// list here, we get it from composition when we need it.
// @author Tom Breton (Tehom)
class MarkerMapper : public SpecialSegmentMapper
{
    friend class SegmentMapperFactory;

protected:
    MarkerMapper(RosegardenDocument *doc);

    // override from MappedEventBuffer
    virtual int calculateSize();

    // override from MappedEventBuffer
    virtual void fillBuffer();

    virtual bool shouldPlay(MappedEvent *evt, RealTime /*startTime*/);
};

}

#endif /* ifndef _MARKERMAPPER_H_ */
