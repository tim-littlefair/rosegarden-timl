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

#ifndef RG_COMPOSITIONMODELIMPL_H
#define RG_COMPOSITIONMODELIMPL_H

#include "base/SnapGrid.h"
#include "CompositionRect.h"
#include "CompositionItem.h"
#include "SegmentOrderer.h"

#include <QColor>
#include <QPoint>
#include <QRect>

#include <vector>
#include <map>
#include <set>


namespace Rosegarden
{


class Studio;
class Segment;
class RulerScale;
class Event;
class Composition;
class AudioPreviewUpdater;
class AudioPreviewThread;

/// For Audio Previews
typedef std::vector<QImage> PixmapArray;

/// Segment previews and selection functionality.
/**
 * I assume that by Model we are referring to Smalltalk's
 * Model/View/Controller pattern.  Qt has a modified Model/View version
 * of this.  Maybe this is in some way related?  MVC never made any
 * sense to me.  Smalltalk doesn't make any sense to me.
 *
 * A skim of this indicates that it is mostly random functionality (e.g.
 * selection and previews) that is closely tied to CompositionView.  This
 * could probably be combined with CompositionView into one gigantic
 * monster-class, then big coherent pieces (like selection and previews)
 * broken off into smaller, more sensible classes.
 *
 * This class works together with CompositionView to provide the composition
 * user interface (the segment canvas).  TrackEditor creates and owns the
 * only instance of this class.
 */
class CompositionModelImpl : public QObject, public CompositionObserver, public SegmentObserver
{
    Q_OBJECT
public:
    CompositionModelImpl(Composition& compo,
                         Studio& studio,
                         RulerScale *rulerScale,
                         int vStep);

    virtual ~CompositionModelImpl();

    typedef std::vector<QRect> RectList;

    struct RectRange {
        std::pair<RectList::iterator, RectList::iterator> range;
        QPoint basePoint;
        QColor color;
    };

    typedef std::vector<RectRange> RectRanges;

    struct AudioPreviewDrawDataItem {
        AudioPreviewDrawDataItem(PixmapArray p, QPoint bp, QRect r) :
            pixmap(p), basePoint(bp), rect(r), resizeOffset(0) {};
        PixmapArray pixmap;
        QPoint basePoint;
        QRect rect;

        // when showing a segment that is being resized from the
        // beginning, this contains the offset between the current
        // rect of the segment and the resized one
        int resizeOffset;
    };

    typedef std::vector<AudioPreviewDrawDataItem> AudioPreviewDrawData;

    typedef std::vector<CompositionRect> RectContainer;

    /// Get the segment rectangles and segment previews
    const RectContainer& getSegmentRects(const QRect &clipRect,
                                         RectRanges *notationPreview,
                                         AudioPreviewDrawData *audioPreview);

    typedef std::vector<int> YCoordList;

    /// Get the Y coords of each track within clipRect.
    /**
     * CompositionView::drawSegments() uses this to draw the track dividers.
     */
    YCoordList getTrackDividersIn(const QRect &clipRect);

    /// Compares Segment pointers in a CompositionItem.
    /**
     * Is this really better than just comparing the CompositionItemPtr
     * addresses?
     *
     *    // Compare the QPointer addresses
     *    return c1.data() < c2.data();
     *
     * All this indexing with pointers gives me the willies.  IDs are safer.
     */
    struct CompositionItemCompare {
        bool operator()(CompositionItemPtr c1, CompositionItemPtr c2) const
        {
            // This strikes me as odd.  I think the one below is better.
            //return CompositionItemHelper::getSegment(c1) < CompositionItemHelper::getSegment(c2);

            // operator< on Segment *'s?  I guess order isn't too important.
            return c1->getSegment() < c2->getSegment();
        }
    };

    typedef std::set<CompositionItemPtr, CompositionItemCompare> ItemContainer;

    /// Used by CompositionView on mouse double-click.
    ItemContainer getItemsAt(const QPoint &);
    /// Get the start time of the repeat nearest the point.
    /**
     * Used by CompositionView to determine the time at which to edit a repeat.
     *
     * Looking closely at the implementation of this, we find that this is a
     * function that brings together CompositionItem and SnapGrid.  It mainly
     * uses CompositionItem, so it likely belongs there.  Perhaps more as a
     *
     *   CompositionItem::getRepeatTimeAt(const SnapGrid &, const QPoint &)
     */
    timeT getRepeatTimeAt(const QPoint &, CompositionItemPtr);

    SnapGrid& grid() { return m_grid; }

    void setPointerPos(int xPos);
    void setSelected(CompositionItemPtr, bool selected = true);
    bool isSelected(CompositionItemPtr) const;
    void setSelected(const ItemContainer&);
    void setSelected(const Segment*, bool selected = true);
    bool isSelected(const Segment*) const;
    void clearSelected();
    bool haveSelection() const { return !m_selectedSegments.empty(); }
    bool haveMultipleSelection() const { return m_selectedSegments.size() > 1; }
    void signalSelection();
    void setSelectionRect(const QRect&);
    void finalizeSelectionRect();
    QRect getSelectionContentsRect();
    void signalContentChange();

    void addRecordingItem(CompositionItemPtr);
    void removeRecordingItem(CompositionItemPtr);
    void clearRecordingItems();
    bool haveRecordingItems() { return !m_recordingSegments.empty(); }

    enum ChangeType { ChangeMove, ChangeResizeFromStart, ChangeResizeFromEnd };

    void startChange(CompositionItemPtr, ChangeType change);
    void startChangeSelection(ChangeType change);
    ItemContainer& getChangingItems() { return m_changingItems; }
    void endChange();
    ChangeType getChangeType() { return m_changeType; }

    void setLength(int width);
    int  getLength();

    unsigned int getHeight();
    
    void setAudioPreviewThread(AudioPreviewThread *thread);
    AudioPreviewThread* getAudioPreviewThread() { return m_audioPreviewThread; }

    void clearPreviewCache();
    void clearSegmentRectsCache(bool clearPreviews = false) { clearInCache(0, clearPreviews); }

    CompositionRect computeSegmentRect(const Segment&, bool computeZ = false);
    QPoint          computeSegmentOrigin(const Segment&);
    void            computeRepeatMarks(CompositionItemPtr);

    SegmentSelection getSelectedSegments() { return m_selectedSegments; }
    Composition&     getComposition()      { return m_composition; }
    Studio&          getStudio()           { return m_studio; }


    // CompositionObserver Interface
    virtual void segmentAdded(const Composition *, Segment *);
    virtual void segmentRemoved(const Composition *, Segment *);
    virtual void segmentRepeatChanged(const Composition *, Segment *, bool);
    virtual void segmentStartChanged(const Composition *, Segment *, timeT);
    virtual void segmentEndMarkerChanged(const Composition *, Segment *, bool);
    virtual void segmentTrackChanged(const Composition *, Segment *, TrackId);
    virtual void endMarkerTimeChanged(const Composition *, bool /*shorten*/);

    // SegmentObserver Interface
    virtual void eventAdded(const Segment *, Event *);
    virtual void eventRemoved(const Segment *, Event *);
    virtual void AllEventsChanged(const Segment *s);
    virtual void appearanceChanged(const Segment *);
    virtual void endMarkerTimeChanged(const Segment *, bool /*shorten*/);
    virtual void segmentDeleted(const Segment*) { /* nothing to do - handled by CompositionObserver::segmentRemoved() */ };

    /// rename: AudioPreview
    /// And pull out into a separate header.
    class AudioPreviewData {
    public:
        AudioPreviewData(bool showMinima, unsigned int channels) : m_showMinima(showMinima), m_channels(channels) {};
        // ~AudioPreviewData();

        bool showsMinima()              { return m_showMinima; }
        void setShowMinima(bool s)      { m_showMinima = s;    }

        unsigned int getChannels()       { return m_channels;   }
        void setChannels(unsigned int c) { m_channels = c;      }

        const std::vector<float> &getValues() const { return m_values;  }
        void setValues(const std::vector<float>&v) { m_values = v; }

        QRect getSegmentRect()              { return m_segmentRect; }
        void setSegmentRect(const QRect& r) { m_segmentRect = r; }

    protected:
        std::vector<float> m_values;
        bool               m_showMinima;
        unsigned int       m_channels;
        QRect              m_segmentRect;

    private:
        // no copy ctor
        AudioPreviewData(const AudioPreviewData&);
    };

signals:
    void needContentUpdate();
    void needContentUpdate(const QRect&);
    void needArtifactsUpdate();
    void selectedSegments(const SegmentSelection &);
    void needSizeUpdate();

public slots:
    void slotAudioFileFinalized(Segment*);
    void slotInstrumentParametersChanged(InstrumentId);

private slots:
    void slotAudioPreviewComplete(AudioPreviewUpdater*);

private:
    bool setTrackHeights(Segment *changed = 0); // true if something changed

    bool isTmpSelected(const Segment*) const;
    bool wasTmpSelected(const Segment*) const;
    bool isMoving(const Segment*) const;
    bool isRecording(const Segment*) const;

    void computeRepeatMarks(CompositionRect& sr, const Segment* s);
    unsigned int computeZForSegment(const Segment* s);
        
    // Segment Previews
    /// Make and cache notation or audio preview for segment.
    void makePreviewCache(const Segment* s);
    /// Remove cached notation or audio preview for segment.
    void removePreviewCache(const Segment* s);

    // Notation Previews
    /// rename: getNotationPreviewStatic()?
    void makeNotationPreviewRects(QPoint basePoint, const Segment* segment,
                                  const QRect& clipRect, RectRanges* npData);
    /// rename: getNotationPreviewMoving()?
    void makeNotationPreviewRectsMovingSegment(
            QPoint basePoint, const Segment* segment,
            const QRect& currentSR, RectRanges* npData);
    /// rename: getNotationPreview()
    RectList* getNotationPreviewData(const Segment* s);
    /// rename: cacheNotationPreview()
    RectList* makeNotationPreviewDataCache(const Segment *s);
    /// For notation preview
    void createEventRects(const Segment* segment, RectList*);

    // Audio Previews
    void makeAudioPreviewRects(AudioPreviewDrawData* apRects, const Segment*,
                               const CompositionRect& segRect, const QRect& clipRect);
    PixmapArray getAudioPreviewPixmap(const Segment* s);
    AudioPreviewData* getAudioPreviewData(const Segment* s);
    QRect postProcessAudioPreview(AudioPreviewData* apData, const Segment*);
    /// rename: cacheAudioPreview()
    AudioPreviewData* makeAudioPreviewDataCache(const Segment *s);
    /// rename: makeAudioPreview()
    void updatePreviewCacheForAudioSegment(const Segment* s, AudioPreviewData* apData);

    // Segment Rects (m_segmentRectMap)
    void putInCache(const Segment*, const CompositionRect&);
    const CompositionRect& getFromCache(const Segment*, timeT& endTime);
    bool isCachedRectCurrent(const Segment& s, const CompositionRect& r,
                             QPoint segmentOrigin, timeT segmentEndTime);
    void clearInCache(const Segment*, bool clearPreviewCache = false);

    //--------------- Data members ---------------------------------
    Composition&     m_composition;
    Studio&          m_studio;
    SnapGrid         m_grid;
    SegmentSelection m_selectedSegments;
    SegmentSelection m_tmpSelectedSegments;
    SegmentSelection m_previousTmpSelectedSegments;

    timeT            m_pointerTimePos;

    typedef std::set<Segment *>  RecordingSegmentSet;
    RecordingSegmentSet          m_recordingSegments;

    AudioPreviewThread*          m_audioPreviewThread;

    // Notation Preview
    typedef std::map<const Segment *, RectList *> NotationPreviewDataCache;
    NotationPreviewDataCache     m_notationPreviewDataCache;

    // Audio Preview
    typedef std::map<const Segment *, AudioPreviewData *> AudioPreviewDataCache;
    AudioPreviewDataCache        m_audioPreviewDataCache;

    /// Used by getSegmentRects() to return a reference for speed.
    RectContainer m_segmentRects;
    ItemContainer m_changingItems;
    ChangeType    m_changeType;

    typedef std::vector<CompositionItemPtr> ItemGC;
    ItemGC m_itemGC;

    QRect m_selectionRect;
    QRect m_previousSelectionUpdateRect;

    std::map<const Segment*, CompositionRect> m_segmentRectMap;
    std::map<const Segment*, timeT> m_segmentEndTimeMap;
    std::map<const Segment*, PixmapArray> m_audioSegmentPreviewMap;
    std::map<TrackId, int> m_trackHeights;

    typedef std::map<const Segment*, AudioPreviewUpdater *>
        AudioPreviewUpdaterMap;
    AudioPreviewUpdaterMap m_audioPreviewUpdaterMap;

    SegmentOrderer m_segmentOrderer;

};



}

#endif
