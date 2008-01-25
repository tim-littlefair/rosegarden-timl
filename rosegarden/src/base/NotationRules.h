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

#ifndef _NOTATION_RULES_H_
#define _NOTATION_RULES_H_


/**
 * Common major and minor scales.
 *
 * For example, sixth note in 12-basis on Cmajor scale:
 *   scale_Cmajor[5] = 9
 */
static int scale_Cmajor[] = { 0, 2, 4, 5, 7, 9, 11 };
static int scale_Cminor[] = { 0, 2, 3, 5, 7, 8, 10 };
static int scale_Cminor_harmonic[] = { 0, 2, 3, 5, 7, 8, 11 };
/**
 * Steps of common major and minor scales.
 *
 * For example, get accidental in 12-basis on Cmajor scale:
 *   10 - scale_Cmajor[steps_Cmajor[10]] = 10 - 9 = +1
 */
static int steps_Cmajor[] = { 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6 };
static int steps_Cminor[] = { 0, 0, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6 };
static int steps_Cminor_harmonic[] = { 0, 0, 1, 2, 2, 3, 3, 4, 5, 5, 5, 6 };
/**
 * Same as previosly, but the use of accidentals is explicitly written.
 *
 * For example, get accidental in 12-basis on Cmajor scale:
 *   10 - scale_Cmajor[steps_Cmajor_with_sharps[10]] = 10 - 9 = +1
 *   10 - scale_Cmajor[steps_Cmajor_with_flats[10]] = 10 - 11 = -1
 */
static int steps_Cmajor_with_sharps[] = { 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6 };
static int steps_Cmajor_with_flats[] = { 0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6 };

namespace Rosegarden
{

/*
 * NotationRules.h
 *
 * This file contains the model for rules which are used in notation decisions.
 *
 */

class NotationRules
{
public:
    NotationRules() { };
    ~NotationRules() { };

    /**
     * If a single note is above the middle line, the preferred direction is up.
     *
     * If a single note is on the middle line, the preferred direction is down.
     *
     * If a single note is below the middle line, the preferred direction is down.
     */
    bool isStemUp(int heightOnStaff) { return heightOnStaff < 4; }

    /**
     * If the highest note in a chord is more distant from the middle
     * line than the lowest note in a chord, the preferred direction is down.
     *
     * If the extreme notes in a chord are an equal distance from the 
     * middle line, the preferred direction is down.
     *
     * If the lowest note in a chord is more distant from the middle
     * line than the highest note in a chord, the preferred direction is up.
     */
    bool isStemUp(int highestHeightOnStaff, int lowestHeightOnStaff) {
        return (highestHeightOnStaff + lowestHeightOnStaff) < 2*4;
    }

    /**
     * If majority of notes are below the middle line, 
     * the preferred direction is up.
     *
     * If notes are equally distributed around the middle line,
     * the preferred direction is down.
     *
     * If majority of notes are above the middle line, 
     * the preferred direction is down.
     */
    bool isBeamAboveWeighted(int weightAbove, int weightBelow) {
        return weightBelow > weightAbove;
    }

    /**
     * If the highest note in a group is more distant from the middle
     * line than the lowest note in a group, the preferred direction is down.
     *
     * If the extreme notes in a group are an equal distance from the 
     * middle line, the preferred direction is down.
     *
     * If the lowest note in a group is more distant from the middle
     * line than the highest note in a group, the preferred direction is up.
     */
    bool isBeamAbove(int highestHeightOnStaff, int lowestHeightOnStaff) {
        return (highestHeightOnStaff + lowestHeightOnStaff) < 2*4;
    }
    bool isBeamAbove(int highestHeightOnStaff, int lowestHeightOnStaff,
                     int weightAbove, int weightBelow) {
        if (highestHeightOnStaff + lowestHeightOnStaff == 2*4) {
	    return isBeamAboveWeighted(weightAbove,weightBelow);
	} else {
	    return isBeamAbove(highestHeightOnStaff,lowestHeightOnStaff);
	}
    }
};

}

#endif
