/***************************************************************************
                          staff.h  -  description
                             -------------------
    begin                : Mon Jun 19 2000
    copyright            : (C) 2000 by Guillaume Laurent, Chris Cannam, Rich Bown
    email                : glaurent@telegraph-road.org, cannam@all-day-breakfast.com, bownie@bownie.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STAFF_H
#define STAFF_H

#include <vector>

#include "qcanvasitemgroup.h"

class QCanvasLineGroupable;


class Staff : public QCanvasItemGroup
{
public:
    typedef vector<QCanvasLineGroupable*> barlines;
    
    Staff(QCanvas*);
    ~Staff();

    int yCoordOfHeight(int height) const;

    /// Returns the height of a bar line
    unsigned int barLineHeight() const { return m_barLineHeight; }

    void insertBar(unsigned int barPos);
    void deleteBars(unsigned int fromPos);
    void deleteBars();

    static const int noteHeight;
    static const int noteWidth;
    static const int lineWidth;
    static const int accidentWidth;
    static const int stalkLen;
    static const int nbLines;
    static const int linesOffset;

protected:
    int m_barLineHeight;
    int m_horizLineLength;

    barlines m_barLines;
};

#endif
