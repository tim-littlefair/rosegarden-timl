/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2009 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _RG_MATRIXSCENE_H_
#define _RG_MATRIXSCENE_H_

#include <QGraphicsScene>

#include "base/Composition.h"
#include "gui/general/SelectionManager.h"

class QGraphicsLineItem;

namespace Rosegarden
{

class MatrixWidget;
class Segment;
class RosegardenGUIDoc;
class Event;
class EventSelection;
class MatrixElement;
class MatrixMouseEvent;
class MatrixViewSegment;
class RulerScale;
class SnapGrid;

class MatrixScene : public QGraphicsScene, 
                    public CompositionObserver,
                    public SelectionManager
{
    Q_OBJECT
    
public:
    MatrixScene();    
    ~MatrixScene();

    void setMatrixWidget(MatrixWidget *w);
    void setSegments(RosegardenGUIDoc *doc, std::vector<Segment *> segments);

    void handleEventAdded(Event *);
    void handleEventRemoved(Event *);

    RosegardenGUIDoc *getDocument() { return m_document; }

    virtual EventSelection *getSelection() const { return m_selection; }
    virtual void setSelection(EventSelection* s, bool preview);
    void selectAll();
    
    void setSingleSelectedEvent(MatrixViewSegment *viewSegment,
                                MatrixElement *e,
                                bool preview);

    void setSingleSelectedEvent(Segment *segment,
                                Event *e,
                                bool preview);

    Segment *getCurrentSegment();
    bool segmentsContainNotes() const;

    int getYResolution() const { return m_resolution; }

    const RulerScale *getRulerScale() const { return m_scale; }
    const SnapGrid *getSnapGrid() const { return m_snapGrid; }

    void setSnap(timeT);

    bool constrainToSegmentArea(QPointF &scenePos);

    void playNote(Segment &segment, int pitch, int velocity = -1);
    
    // SegmentObserver method forwarded from MatrixViewSegment
    void segmentEndMarkerTimeChanged(const Segment *s, bool shorten);

signals:
    void mousePressed(const MatrixMouseEvent *e);
    void mouseMoved(const MatrixMouseEvent *e);
    void mouseReleased(const MatrixMouseEvent *e);
    void mouseDoubleClicked(const MatrixMouseEvent *e);
    
    void eventRemoved(Event *e);

    void sceneDeleted(); // all segments have been removed
    
protected slots:    
    void slotCommandExecuted();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);

    void segmentRemoved(const Composition *, Segment *); // CompositionObserver
    void timeSignatureChanged(const Composition *); // CompositionObserver

private:
    MatrixWidget *m_widget; // I do not own this

    RosegardenGUIDoc *m_document; // I do not own this

    std::vector<Segment *> m_segments; // I do not own these
    std::vector<MatrixViewSegment *> m_viewSegments; // I own these

    RulerScale *m_scale; // I own this (it maps between time and scene x)
    SnapGrid *m_snapGrid; // I own this

    int m_resolution;
    EventSelection *m_selection; // I own this

    std::vector<QGraphicsLineItem *> m_horizontals;
    std::vector<QGraphicsLineItem *> m_verticals;
    std::vector<QGraphicsRectItem *> m_highlights;

    void setupMouseEvent(QGraphicsSceneMouseEvent *, MatrixMouseEvent &) const;
    void recreateLines();
    void recreatePitchHighlights();
    void setSelectionElementStatus(EventSelection *, bool, bool = false);
};

}

#endif
