
/*
    Rosegarden-4 v0.1
    A sequencer and musical notation editor.

    This program is Copyright 2000-2001
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

#ifndef NOTATIONVIEW_H
#define NOTATIONVIEW_H

#include <kmainwindow.h>

#include "notationelement.h"
#include "viewelementsmanager.h"
#include "notationhlayout.h"
#include "notationvlayout.h"
#include "notationcanvasview.h"

class QCanvasItem;
class RosegardenGUIDoc;

/**
  *@author Guillaume Laurent, Chris Cannam, Rich Bown
  */

class NotationView : public KMainWindow
{
    Q_OBJECT
public:

    NotationView(RosegardenGUIDoc* doc, unsigned int trackNb, QWidget *parent,
                 int resolution);
    ~NotationView();

    const RosegardenGUIDoc *getDocument() const { return m_document; }
    RosegardenGUIDoc *getDocument() { return m_document; }

    /// draw all elements
    virtual bool showElements(NotationElementList::iterator from,
                              NotationElementList::iterator to);

    /// same, with dx,dy offset
    virtual bool showElements(NotationElementList::iterator from,
                              NotationElementList::iterator to,
                              double dxoffset, double dyoffset);

    /// same, relative to the specified item
    virtual bool showElements(NotationElementList::iterator from,
                              NotationElementList::iterator to,
                              QCanvasItem*);

    /// Normally calls applyHorizontalLayout() then applyVerticalLayout()
    virtual bool applyLayout();

    /// Calculate cached values for use in layout
    virtual bool applyHorizontalPreparse();

    /// Set the 'x'-coord on all doc elements - should be called after applyHorizontalPreparse()
    virtual bool applyHorizontalLayout();

    /// Set the 'y'-coord on all doc elements - should be called after applyHorizontalLayout()
    virtual bool applyVerticalLayout();
    
    void setHorizontalLayoutEngine(NotationHLayout* e) { m_hlayout = e; }
    void setVerticalLayoutEngine(NotationVLayout* e)   { m_vlayout = e; }

    LayoutEngine* getHorizontalLayoutEngine() { return m_hlayout; }
    LayoutEngine* getVerticalLayoutEngine()   { return m_vlayout; }

    QCanvas* canvas() { return m_canvasView->canvas(); }

    void setCurrentSelectedNote(Note::Type);

    Note::Type currentSelectedNote() const { return m_currentSelectedNote; }

public slots:

    /** undo
     */
    void slotEditUndo();
    /** redo
     */
    void slotEditRedo();
    
    /** put the marked text/object into the clipboard and remove
     *	it from the document
     */
    void slotEditCut();

    /** put the marked text/object into the clipboard
     */
    void slotEditCopy();

    /** paste the clipboard into the document
     */
    void slotEditPaste();

    /** toggles the toolbar
     */
    void slotToggleToolBar();

    /** toggles the statusbar
     */
    void slotToggleStatusBar();

    /** changes the statusbar contents for the standard label permanently, used to indicate current actions.
     * @param text the text that is displayed in the statusbar
     */
    void slotStatusMsg(const QString &text);

    /** changes the status message of the whole statusbar for two seconds, then restores the last status. This is used to display
     * statusbar messages that give information about actions for toolbar icons and menuentries.
     * @param text the text that is displayed in the statusbar
     */
    void slotStatusHelpMsg(const QString &text);


    // note switch slots
    void slotWhole();
    void slotHalf();
    void slotQuarter();
    void slot8th();
    void slot16th();
    void slot32nd();
    void slot64th();

    // Canvas actions slots
    void insertNote(int pitch, const QPoint&);
    void hoveredOverNoteChanged(const QString&);


signals:
    void changeCurrentNote(Note::Type);

protected:

    /**
     * save general Options like all bar positions and status as well
     * as the geometry and the recent file list to the configuration
     * file
     */
    void saveOptions();

    /**
     * read general Options again and initialize all variables like the recent file list
     */
    void readOptions();

    /**
     * create menus and toolbars
     */
    void setupActions();

    /**
     * setup status bar
     */
    void initStatusBar();

    /**
     * show bar lines
     */
    bool showBars(NotationElementList::iterator from,
                  NotationElementList::iterator to);

    /**
     * find the NotationElement which X is closest to eventX
     */
    NotationElementList::iterator findClosestNote(double eventX);

    /**
     * replace the rest element pointed to by the iterator
     * by the NotationElement
     */
    bool replaceRestWithNote(NotationElementList::iterator, NotationElement*);


    void perfTest();
#ifdef NOT_DEFINED
    void test();
#endif

    KConfig* m_config;

    RosegardenGUIDoc* m_document;

    /// Displayed in the status bar, holds the pixmap of the current note
    QLabel* m_currentNotePixmap;

    /// Displayed in the status bar, shows the pitch the cursor is at
    QLabel* m_hoveredOverNoteName;

    NotationCanvasView* m_canvasView;

    Staff* m_mainStaff;
    Staff* m_currentStaff;
    NotePixmapFactory &m_notePixmapFactory;

    ViewElementsManager* m_viewElementsManager;
    NotationElementList* m_notationElements;
    
    NotationHLayout* m_hlayout;
    NotationVLayout* m_vlayout;

    Note::Type m_currentSelectedNote;
};

#endif
