
/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2017 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef RG_ERASESEGMENTSSTARTINGINRANGECOMMAND_H
#define RG_ERASESEGMENTSSTARTINGINRANGECOMMAND_H

#include "document/Command.h"
#include <vector>
#include "base/Event.h"
#include <QCoreApplication>

namespace Rosegarden
{

class Segment;
class Composition;


class EraseSegmentsStartingInRangeCommand : public NamedCommand
{
    Q_DECLARE_TR_FUNCTIONS(Rosegarden::EraseSegmentsStartingInRangeCommand)

public:
    EraseSegmentsStartingInRangeCommand(Composition *composition,
                                        timeT begin,
                                        timeT end);
    virtual ~EraseSegmentsStartingInRangeCommand();
    
    virtual void execute();
    virtual void unexecute();
    
private:
    Composition *m_composition;
    timeT m_beginTime;
    timeT m_endTime;
    
    bool m_detached;
    std::vector<Segment *> m_detaching;
};


}

#endif
