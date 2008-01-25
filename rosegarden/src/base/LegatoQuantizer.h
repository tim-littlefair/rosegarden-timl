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

#ifndef LEGATO_QUANTIZER_H
#define LEGATO_QUANTIZER_H

#include "Quantizer.h"

namespace Rosegarden {

class BasicQuantizer;

class LegatoQuantizer : public Quantizer
{
public:
    // The default unit is the shortest note type.  A unit of
    // zero means do no quantization -- unlike for BasicQuantizer
    // this does have a purpose, as it still does the legato step
    LegatoQuantizer(timeT unit = -1);
    LegatoQuantizer(std::string source, std::string target, timeT unit = -1);
    LegatoQuantizer(const LegatoQuantizer &);
    virtual ~LegatoQuantizer();

    void setUnit(timeT unit) { m_unit = unit; }
    timeT getUnit() const { return m_unit; }

    virtual void quantizeRange(Segment *,
                               Segment::iterator,
                               Segment::iterator) const;

protected:
    virtual void quantizeSingle(Segment *, Segment::iterator,
                                Segment::iterator &) const;

    timeT quantizeTime(timeT) const;

private:
    LegatoQuantizer &operator=(const BasicQuantizer &); // not provided

    timeT m_unit;
};

}

#endif

