/***************************************************************************
                          rosegardenguiview.h  -  description
                             -------------------
    begin                : Mon Jun 19 23:41:03 CEST 2000
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

#ifndef ROSEGARDENGUIVIEW_H
#define ROSEGARDENGUIVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

// include files for Qt
#include <qvbox.h>

#include "rosedebug.h"


class RosegardenGUIDoc;
class NotationView;
class TrackPart;

/**
 * The RosegardenGUIView class provides the view widget for the
 * RosegardenGUIApp instance.  The View instance inherits QWidget as a
 * base class and represents the view object of a KTMainWindow. As
 * RosegardenGUIView is part of the docuement-view model, it needs a
 * reference to the document object connected with it by the
 * RosegardenGUIApp class to manipulate and display the document
 * structure provided by the RosegardenGUIDoc class.
 * 	
 * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team.
 * @version KDevelop version 0.4 code generation
 */
class RosegardenGUIView : public QVBox
{
    Q_OBJECT
public:
    /** Constructor for the main view */
    RosegardenGUIView(QWidget *parent = 0, const char *name=0);
    /** Destructor for the main view */
    ~RosegardenGUIView();

    /**
     * returns a pointer to the document connected to the view
     * instance. Mind that this method requires a RosegardenGUIApp
     * instance as a parent widget to get to the window document
     * pointer by calling the RosegardenGUIApp::getDocument() method.
     *
     * @see RosegardenGUIApp#getDocument
     */
    RosegardenGUIDoc* getDocument() const;

    /** contains the implementation for printing functionality */
    void print(QPrinter *pPrinter);

public slots:
    void editTrackNotation(TrackPart*);

protected:
    NotationView* m_notationView;

};

#endif // ROSEGARDENGUIVIEW_H
