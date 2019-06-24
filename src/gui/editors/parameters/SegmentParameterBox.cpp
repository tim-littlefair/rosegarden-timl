/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2018 the Rosegarden development team.
 
    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#define RG_MODULE_STRING "[SegmentParameterBox]"

#include "SegmentParameterBox.h"

#include "misc/Debug.h"
#include "misc/Strings.h"  // qstrtostr() etc...
#include "base/Colour.h"
#include "base/ColourMap.h"
//#include "base/NotationTypes.h"
#include "base/BasicQuantizer.h"
#include "base/RealTime.h"
#include "base/Segment.h"
#include "commands/segment/SegmentChangeQuantizationCommand.h"
#include "commands/segment/SegmentColourCommand.h"
#include "commands/segment/SegmentColourMapCommand.h"
#include "commands/segment/SegmentCommandRepeat.h"
#include "commands/segment/SegmentLabelCommand.h"
#include "commands/segment/SegmentLinkTransposeCommand.h"
#include "document/CommandHistory.h"
#include "document/RosegardenDocument.h"
#include "gui/dialogs/IntervalDialog.h"
#include "gui/editors/notation/NotationStrings.h"
#include "gui/editors/notation/NotePixmapFactory.h"
#include "gui/general/GUIPalette.h"
#include "gui/widgets/ColorCombo.h"
#include "gui/widgets/ColourTable.h"
#include "gui/widgets/TristateCheckBox.h"
#include "gui/widgets/CollapsingFrame.h"
#include "gui/widgets/LineEdit.h"
#include "gui/widgets/InputDialog.h"
#include "gui/widgets/Label.h"
#include "gui/application/RosegardenMainWindow.h"
#include "gui/application/RosegardenMainViewWidget.h"
#include "gui/editors/segment/compositionview/CompositionView.h"

#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QFontMetrics>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QString>
#include <QWidget>


namespace Rosegarden
{


enum Tristate
{
    None,
    Some,
    All,
    NotApplicable  // no applicable segments selected
};

SegmentParameterBox::SegmentParameterBox(RosegardenDocument* doc,
                                         QWidget *parent) :
    RosegardenParameterBox(tr("Segment Parameters"), parent),
    m_doc(doc),
    m_standardQuantizations(BasicQuantizer::getStandardQuantizations())
{
    setObjectName("Segment Parameter Box");

    // * Label

    QLabel *label = new QLabel(tr("Label"), this);
    label->setFont(m_font);

    m_label = new Label("", this);
    // SPECIAL_LABEL => Gray background, black text.  See ThornStyle.cpp.
    // ??? Can't we just inline that here?
    m_label->setObjectName("SPECIAL_LABEL");
    m_label->setFont(m_font);
    QFontMetrics fontMetrics(m_font);
    const int width20 = fontMetrics.width("12345678901234567890");
    m_label->setFixedWidth(width20);
    m_label->setToolTip(tr("<qt>Click to edit the segment label for any selected segments</qt>"));
    connect(m_label, &Label::clicked,
            this, &SegmentParameterBox::slotEditSegmentLabel);

    // * Edit button

    // ??? This Edit button is now no longer needed.  The user can just
    //     click on the label to edit it.
    m_edit = new QPushButton(tr("Edit"), this);
    m_edit->setFont(m_font);
    m_edit->setToolTip(tr("<qt>Edit the segment label for any selected segments</qt>"));
    connect(m_edit, &QAbstractButton::released,
            this, &SegmentParameterBox::slotEditSegmentLabel);

    // * Repeat

    QLabel *repeatLabel = new QLabel(tr("Repeat"), this);
    repeatLabel->setFont(m_font);

    m_repeat = new TristateCheckBox(this);
    m_repeat->setFont(m_font);
    m_repeat->setToolTip(tr("<qt><p>When checked,     any selected segments will repeat until they run into another segment,  "
                                 "or the end of the composition.</p><p>When viewed in the notation editor or printed via LilyPond, "
                                 "the segments will be bracketed by repeat signs.</p><p><center><img src=\":pixmaps/tooltip/repeats"
                                 ".png\"></img></center></p><br>These can be used in conjunction with special LilyPond export direct"
                                 "ives to create repeats with first and second alternate endings. See rosegardenmusic.com for a tut"
                                 "orial. [Ctrl+Shift+R] </qt>"));
    connect(m_repeat, &QAbstractButton::pressed,
            this, &SegmentParameterBox::slotRepeatPressed);

    // * Transpose

    QLabel *transposeLabel = new QLabel(tr("Transpose"), this);
    transposeLabel->setFont(m_font);

    m_transpose = new QComboBox(this);
    m_transpose->setFont(m_font);
    m_transpose->setToolTip(tr("<qt><p>Raise or lower playback of any selected segments by this number of semitones</p><p>"
                                    "<i>NOTE: This control changes segments that already exist.</i></p><p><i>Use the transpose "
                                    "control in <b>Track Parameters</b> under <b>Create segments with</b> to pre-select this   "
                                    "setting before drawing or recording new segments.</i></p></qt>"));
    // ??? QComboBox::activated() is overloaded, so we have to use SIGNAL().
    connect(m_transpose, SIGNAL(activated(int)),
            SLOT(slotTransposeSelected(int)));
    connect(m_transpose, &QComboBox::editTextChanged,
            this, &SegmentParameterBox::slotTransposeTextChanged);

    QPixmap noMap = NotePixmapFactory::makeToolbarPixmap("menu-no-note");

    constexpr int transposeRange = 48;

    for (int i = -transposeRange; i < transposeRange + 1; ++i) {
        m_transpose->addItem(noMap, QString("%1").arg(i));
    }

    // * Quantize

    QLabel *quantizeLabel = new QLabel(tr("Quantize"), this);
    quantizeLabel->setFont(m_font);

    m_quantize = new QComboBox(this);
    m_quantize->setFont(m_font);
    m_quantize->setToolTip(tr(
            "<qt><p>Quantize the selected segments using the Grid quantizer.  "
            "This quantization can be removed at any time in "
            "the future by setting it to off.</p></qt>"));
    // ??? QComboBox::activated() is overloaded, so we have to use SIGNAL().
    connect(m_quantize, SIGNAL(activated(int)),
            SLOT(slotQuantizeSelected(int)));

    // For each standard quantization value
    for (unsigned int i = 0; i < m_standardQuantizations.size(); ++i) {
        timeT time = m_standardQuantizations[i];
        timeT error = 0;
        QString label = NotationStrings::makeNoteMenuLabel(time, true, error);
        QPixmap pmap = NotePixmapFactory::makeNoteMenuPixmap(time, error);
        // Add the icon and label to the ComboBox.
        m_quantize->addItem(error ? noMap : pmap, label);
    }
    m_quantize->addItem(noMap, tr("Off"));

    // * Delay

    QLabel *delayLabel = new QLabel(tr("Delay"), this);
    delayLabel->setFont(m_font);

    m_delay = new QComboBox(this);
    m_delay->setFont(m_font);
    m_delay->setToolTip(tr("<qt><p>Delay playback of any selected segments by this number of miliseconds</p><p><i>NOTE: "
                                "Rosegarden does not support negative delay.  If you need a negative delay effect, set the   "
                                "composition to start before bar 1, and move segments to the left.  You can hold <b>shift</b>"
                                " while doing this for fine-grained control, though doing so will have harsh effects on music"
                                " notation rendering as viewed in the notation editor.</i></p></qt>"));
    // ??? QComboBox::activated() is overloaded, so we have to use SIGNAL().
    connect(m_delay, SIGNAL(activated(int)),
            SLOT(slotDelaySelected(int)));
    connect(m_delay, &QComboBox::editTextChanged,
            this, &SegmentParameterBox::slotDelayTextChanged);

    m_delays.clear();

    // For each note duration delay
    for (int i = 0; i < 6; i++) {

        // extra range checks below are benign - they account for the
        // option of increasing the range of the loop beyond 0-5

        timeT time = 0;
        if (i > 0 && i < 6) {
            time = Note(Note::Hemidemisemiquaver).getDuration() << (i - 1);
        } else if (i > 5) {
            time = Note(Note::Crotchet).getDuration() * (i - 4);
        }

        m_delays.push_back(time);

        timeT error = 0;
        QString label = NotationStrings::makeNoteMenuLabel(time, true, error);
        QPixmap pmap = NotePixmapFactory::makeNoteMenuPixmap(time, error);

        // check if it's a valid note duration (it will be for the
        // time defn above, but if we were basing it on the sequencer
        // resolution it might not be) & include a note pixmap if so
        m_delay->addItem((error ? noMap : pmap), label);
    }

    // For each real-time delay (msecs)
    for (int i = 0; i < 10; i++) {
        int rtd = (i < 5 ? ((i + 1) * 10) : ((i - 3) * 50));
        m_realTimeDelays.push_back(rtd);
        m_delay->addItem(tr("%1 ms").arg(rtd));
    }

    // * Color

    QLabel *colourLabel = new QLabel(tr("Color"), this);
    colourLabel->setFont(m_font);

    m_color = new ColorCombo(this);
    m_color->setFont(m_font);
    m_color->setToolTip(tr("<qt><p>Change the color of any selected segments</p></qt>"));
    connect(m_color, SIGNAL(activated(int)),
            SLOT(slotColourChanged(int)));

    connect(m_doc, &RosegardenDocument::docColoursChanged,
            this, &SegmentParameterBox::slotDocColoursChanged);
    // Populate the colours.
    m_color->updateColors();

    // * Linked segment parameters (hidden)

    // Outer collapsing frame
    CollapsingFrame *linkedSegmentParametersFrame = new CollapsingFrame(
            tr("Linked segment parameters"), this, "segmentparameterslinked", false);

    // Unhide this if you want to play with the linked segment
    // transpose parameters.  I've hidden it for the time being until
    // we've decided how we're going to interact with these transpose params.
    linkedSegmentParametersFrame->hide();

    // Inner fixed widget
    QWidget *linkedSegmentParameters = new QWidget(linkedSegmentParametersFrame);
    linkedSegmentParametersFrame->setWidget(linkedSegmentParameters);
    linkedSegmentParameters->setContentsMargins(3, 3, 3, 3);

    // Transpose
    QLabel *linkTransposeLabel = new QLabel(tr("Transpose"), linkedSegmentParameters);
    linkTransposeLabel->setFont(m_font);

    // Change
    QPushButton *changeButton = new QPushButton(tr("Change"), linkedSegmentParameters);
    changeButton->setFont(m_font);
    changeButton->setToolTip(tr("<qt>Edit the relative transposition on the linked segment</qt>"));
    connect(changeButton, &QAbstractButton::released,
            this, &SegmentParameterBox::slotChangeLinkTranspose);

    // Reset
    QPushButton *resetButton = new QPushButton(tr("Reset"), linkedSegmentParameters);
    resetButton->setFont(m_font);
    resetButton->setToolTip(tr("<qt>Reset the relative transposition on the linked segment to zero</qt>"));
    connect(resetButton, &QAbstractButton::released,
            this, &SegmentParameterBox::slotResetLinkTranspose);

    // Linked segment parameters layout

    QGridLayout *groupLayout = new QGridLayout(linkedSegmentParameters);
    groupLayout->setContentsMargins(5,0,0,5);
    groupLayout->setSpacing(2);
    groupLayout->addWidget(linkTransposeLabel, 0, 0, Qt::AlignLeft);
    groupLayout->addWidget(changeButton, 0, 1);
    groupLayout->addWidget(resetButton, 0, 2);
    groupLayout->setColumnStretch(3, 1);

    // SegmentParameterBox Layout

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setMargin(0);
    gridLayout->setSpacing(2);
    // Row 0: Label
    gridLayout->addWidget(label, 0, 0);
    gridLayout->addWidget(m_label, 0, 1, 1, 4);
    gridLayout->addWidget(m_edit, 0, 5);
    // Row 1: Repeat/Transpose
    gridLayout->addWidget(repeatLabel, 1, 0);
    gridLayout->addWidget(m_repeat, 1, 1);
    gridLayout->addWidget(transposeLabel, 1, 2, 1, 2, Qt::AlignRight);
    gridLayout->addWidget(m_transpose, 1, 4, 1, 2);
    // Row 2: Quantize/Delay
    gridLayout->addWidget(quantizeLabel, 2, 0);
    gridLayout->addWidget(m_quantize, 2, 1, 1, 2);
    gridLayout->addWidget(delayLabel, 2, 3, Qt::AlignRight);
    gridLayout->addWidget(m_delay, 2, 4, 1, 2);
    // Row 3: Color
    gridLayout->addWidget(colourLabel, 3, 0);
    gridLayout->addWidget(m_color, 3, 1, 1, 5);
    // Row 4: Linked segment parameters
    gridLayout->addWidget(linkedSegmentParametersFrame, 4, 0, 1, 5);

    // SegmentParameterBox

    setContentsMargins(4, 7, 4, 4);

    //RG_DEBUG << "ctor: " << this << ": font() size is " << (this->font()).pixelSize() << "px (" << (this->font()).pointSize() << "pt)";

    m_doc->getComposition().addObserver(this);

    connect(RosegardenMainWindow::self(),
                &RosegardenMainWindow::documentChanged,
            this, &SegmentParameterBox::slotNewDocument);

    // ??? commandExecuted() is overloaded so we must use SLOT().
    //     Rename to commandExecutedOrUn().
    // ??? We should subscribe for documentModified instead of this.
    connect(CommandHistory::getInstance(), SIGNAL(commandExecuted()),
            this, SLOT(slotUpdate()));
}

SegmentParameterBox::~SegmentParameterBox()
{
    if (!isCompositionDeleted()) {
        RosegardenMainWindow::self()->getDocument()->
            getComposition().removeObserver(this);
    }
}

void
SegmentParameterBox::setDocument(RosegardenDocument *doc)
{
    if (m_doc) {
        disconnect(m_doc, &RosegardenDocument::docColoursChanged,
                   this, &SegmentParameterBox::slotDocColoursChanged);
    }

    m_doc = doc;

    // Detect when the document colours are updated
    connect (m_doc, &RosegardenDocument::docColoursChanged,
             this, &SegmentParameterBox::slotDocColoursChanged);

    // repopulate combo
    slotDocColoursChanged();
}

namespace
{
    SegmentSelection
    getSelectedSegments()
    {
        // Delegates to CompositionModelImpl::getSelectedSegments().

        // ??? COPY
        return RosegardenMainWindow::self()->getView()->
                   getTrackEditor()->getCompositionView()->getModel()->
                   getSelectedSegments();
    }
}

void
SegmentParameterBox::useSegments(const SegmentSelection &segments)
{
    // ??? Switch this push approach to a pull approach.
    //     Use getSelectedSegments() and get rid of m_segments.
    // ??? Since selection isn't part of the document, we'll need
    //     to keep this to get notifications of selection changes
    //     when they happen.

    // Copy from segments which is a std::set to m_segments which
    // is a std::vector.
    m_segments.clear();
    m_segments.resize(segments.size());
    std::copy(segments.begin(), segments.end(), m_segments.begin());

    updateWidgets();
}

void
SegmentParameterBox::slotDocColoursChanged()
{
    // The color combobox is handled differently from the others.  Since
    // there are 420 strings of up to 25 chars in here, it would be
    // expensive to detect changes by comparing vectors of strings.

    // For now, we'll handle the document colors changed notification
    // and reload the combobox then.

    // See the comments on RosegardenDocument::docColoursChanged()
    // in RosegardenDocument.h.

    // Note that as of this writing (June 2019) there is no way
    // to modify the document colors.  See ColourConfigurationPage
    // which was probably meant to be used by DocumentConfigureDialog.
    // See TrackParameterBox::slotDocColoursChanged().

    m_color->updateColors();
    m_color->setCurrentIndex(0);
}

void SegmentParameterBox::slotUpdate()
{
    // ??? I'm guessing this should evolve into some sort of
    //     slotDocumentModified()?

    RG_DEBUG << "slotUpdate()";

    updateWidgets();
}

void
SegmentParameterBox::segmentRemoved(const Composition *composition,
                                    Segment *segment)
{
    // ??? If we switch to getSelectedSegments() this will no longer be
    //     needed.  Instead, we should be able to get notification of
    //     segments removed as part of the doc modified notification.
    //     If that's not the case, at the very least, this can just be
    //     reduced to an updateWidgets() call.

    RG_DEBUG << "segmentRemoved()...";

    // Not our composition?  Bail.
    if (composition != &m_doc->getComposition()) {
        RG_DEBUG << "segmentRemoved(): received a delete for the wrong Composition";
        return;
    }

    // For each Segment that we are displaying...
    for (SegmentVector::const_iterator it =
             m_segments.begin();
         it != m_segments.end();
         ++it) {

        // If we found the segment in question, delete it from our list.
        if (*it == segment) {
            RG_DEBUG << "segmentRemoved(): found the segment to remove";
            m_segments.erase(it);
            return;
        }

    }

    // ??? We don't slotUpdate() here, so we still show old data.
}

void
SegmentParameterBox::updateLabel()
{
    SegmentSelection segmentSelection = getSelectedSegments();

    // No Segments selected?  Blank.
    if (segmentSelection.empty()) {
        m_label->setEnabled(false);
        m_label->setText("");
        return;
    }

    // One or more Segments selected

    m_label->setEnabled(true);

    SegmentSelection::const_iterator i = segmentSelection.begin();

    // ??? Use the same approach as for transpose.  Get the label
    //     value and use that for the comparisons later.

    m_label->setText(QObject::trUtf8((*i)->getLabel().c_str()));

    // Just one?  We're done.
    if (segmentSelection.size() == 1)
        return;

    // More than one Segment selected.

    // Skip to the second Segment.
    ++i;

    // For each Segment
    for (/* ...starting with the second one */;
         i != segmentSelection.end();
         ++i) {
        // If the labels do not match, set label to "*"
        if (QObject::trUtf8((*i)->getLabel().c_str()) != m_label->text()) {
            m_label->setText("*");
            break;
        }
    }
}

void
SegmentParameterBox::updateRepeat()
{
    SegmentSelection segmentSelection = getSelectedSegments();

    // No Segments selected?  Disable/uncheck.
    if (segmentSelection.empty()) {
        m_repeat->setEnabled(false);
        m_repeat->setCheckState(Qt::Unchecked);
        return;
    }

    // One or more Segments selected

    m_repeat->setEnabled(true);

    SegmentSelection::const_iterator i = segmentSelection.begin();

    // Just one?  Set and bail.
    if (segmentSelection.size() == 1) {
        m_repeat->setCheckState(
                (*i)->isRepeating() ? Qt::Checked : Qt::Unchecked);
        return;
    }

    // More than one Segment selected.

    std::size_t repeating = 0;

    // For each Segment, count the repeating ones
    for (/* Starting with the first... */;
         i != segmentSelection.end();
         ++i) {
        if ((*i)->isRepeating())
            ++repeating;
    }

    if (repeating == 0)  // none
        m_repeat->setCheckState(Qt::Unchecked);
    else if (repeating == segmentSelection.size())  // all
        m_repeat->setCheckState(Qt::Checked);
    else  // some
        m_repeat->setCheckState(Qt::PartiallyChecked);
}

void
SegmentParameterBox::updateTranspose()
{
    SegmentSelection segmentSelection = getSelectedSegments();

    // No Segments selected?  Disable and set to 0.
    if (segmentSelection.empty()) {
        m_transpose->setEnabled(false);
        m_transpose->setCurrentIndex(m_transpose->findText("0"));
        return;
    }

    // One or more Segments selected

    m_transpose->setEnabled(true);

    SegmentSelection::const_iterator i = segmentSelection.begin();
    int transposeValue = (*i)->getTranspose();

    // Just one?  Set and bail.
    if (segmentSelection.size() == 1) {
        m_transpose->setCurrentIndex(m_transpose->findText(
                QString("%1").arg(transposeValue)));
        return;
    }

    // More than one Segment selected.

    // Skip to the second Segment.
    ++i;

    bool allSame = true;

    // For each Segment
    for (/* ...starting with the second one */;
         i != segmentSelection.end();
         ++i) {
        if ((*i)->getTranspose() != transposeValue) {
            allSame = false;
            break;
        }
    }

    if (allSame) {
        m_transpose->setCurrentIndex(m_transpose->findText(
                QString("%1").arg(transposeValue)));
    } else {
        m_transpose->setCurrentIndex(-1);
    }
}

int
SegmentParameterBox::quantizeIndex(timeT t)
{
    for (unsigned i = 0;
         i < m_standardQuantizations.size();
         ++i) {
        if (m_standardQuantizations[i] == t)
            return i;
    }

    // Nothing?  Return one beyond the last, which is "Off" in the UI.
    return m_standardQuantizations.size();
}

void
SegmentParameterBox::updateQuantize()
{
    SegmentSelection segmentSelection = getSelectedSegments();

    // No Segments selected?  Disable and set to off.
    if (segmentSelection.empty()) {
        m_quantize->setEnabled(false);
        m_quantize->setCurrentIndex(m_quantize->count() - 1);  // Off
        return;
    }

    // One or more Segments selected

    m_quantize->setEnabled(true);

    SegmentSelection::const_iterator i = segmentSelection.begin();
    timeT quantizeValue =
            (*i)->hasQuantization() ?
                    (*i)->getQuantizer()->getUnit() :
                    0;

    // Just one?  Set and bail.
    if (segmentSelection.size() == 1) {
        m_quantize->setCurrentIndex(quantizeIndex(quantizeValue));
        return;
    }

    // More than one Segment selected.

    // Skip to the second Segment.
    ++i;

    bool allSame = true;

    // For each Segment
    for (/* ...starting with the second one */;
         i != segmentSelection.end();
         ++i) {
        timeT quantizeValue2 =
                (*i)->hasQuantization() ?
                        (*i)->getQuantizer()->getUnit() :
                        0;
        // If any of them are different from the first...
        if (quantizeValue2 != quantizeValue) {
            allSame = false;
            break;
        }
    }

    if (allSame)
        m_quantize->setCurrentIndex(quantizeIndex(quantizeValue));
    else
        m_quantize->setCurrentIndex(-1);
}

namespace
{
    // Flatten Segment::getDelay() and getRealTimeDelay() to
    // return a single delay value.  Real-time delay is returned
    // as a negative.
    // ??? This means we can't do negative delays, which could be very
    //     useful.  Might want to redo this using a vector to translate
    //     the value to an index.  Like quantizeIndex().  See m_delays
    //     and m_realTimeDelays which might be combined.
    timeT
    delay(Segment *s)
    {
        // Note duration delay (1/4, 1/8, etc...)
        timeT delayValue = s->getDelay();
        if (delayValue != 0)
            return delayValue;

        // Millisecond delay (10ms, 20ms, etc...)
        return -(s->getRealTimeDelay().sec * 1000 +
                 s->getRealTimeDelay().msec());
    }
}

void
SegmentParameterBox::setDelay(timeT t)
{
    // Note duration delay (1/4, 1/8, etc...)
    if (t >= 0) {
        timeT error = 0;

        QString label =
                NotationStrings::makeNoteMenuLabel(
                        t,  // duration
                        true,  // brief
                        error);  // errorReturn
        m_delay->setCurrentIndex(m_delay->findText(label));

        return;
    }

    // Millisecond delay (10ms, 20ms, etc...)
    m_delay->setCurrentIndex(m_delay->findText(tr("%1 ms").arg(-t)));
}

void
SegmentParameterBox::updateDelay()
{
    SegmentSelection segmentSelection = getSelectedSegments();

    // No Segments selected?  Disable and set to 0.
    if (segmentSelection.empty()) {
        m_delay->setEnabled(false);
        m_delay->setCurrentIndex(m_delay->findText("0"));
        return;
    }

    // One or more Segments selected

    m_delay->setEnabled(true);

    SegmentSelection::const_iterator i = segmentSelection.begin();
    timeT delayValue = delay(*i);

    // Just one?  Set and bail.
    if (segmentSelection.size() == 1) {
        setDelay(delayValue);
        return;
    }

    // More than one Segment selected.

    // Skip to the second Segment.
    ++i;

    bool allSame = true;

    // For each Segment
    for (/* ...starting with the second one */;
         i != segmentSelection.end();
         ++i) {
        if (delay(*i) != delayValue) {
            allSame = false;
            break;
        }
    }

    if (allSame)
        setDelay(delayValue);
    else
        m_delay->setCurrentIndex(-1);
}

void
SegmentParameterBox::updateColor()
{
    SegmentSelection segmentSelection = getSelectedSegments();

    // No Segments selected?  Disable and set to 0.
    if (segmentSelection.empty()) {
        m_color->setEnabled(false);
        m_color->setCurrentIndex(0);
        return;
    }

    // One or more Segments selected

    m_color->setEnabled(true);

    SegmentSelection::const_iterator i = segmentSelection.begin();

    unsigned colorIndex = (*i)->getColourIndex();

    // Just one?  Set and bail.
    if (segmentSelection.size() == 1) {
        m_color->setCurrentIndex(colorIndex);
        return;
    }

    // More than one Segment selected.

    // Skip to the second Segment.
    ++i;

    bool allSame = true;

    // For each Segment
    for (/* ...starting with the second one */;
         i != segmentSelection.end();
         ++i) {
        if ((*i)->getColourIndex() != colorIndex) {
            allSame = false;
            break;
        }
    }

    if (allSame)
        m_color->setCurrentIndex(colorIndex);
    else
        m_color->setCurrentIndex(-1);
}

void
SegmentParameterBox::updateWidgets()
{
    // ??? Recommend reorganizing this to focus on one widget at
    //     a time even though that will result in duplicated for loops.
    // ??? Then switch it over to getSelectedSegments() and ignore
    //     m_segments.

    updateLabel();
    updateRepeat();
    updateTranspose();
    updateQuantize();
    updateDelay();
    updateColor();


    // * The Rest

    SegmentVector::iterator it;
    Tristate highlow = NotApplicable;
    unsigned int myHigh = 127;
    unsigned int myLow = 0;

    // I never noticed this after all this time, but it seems to go all the way
    // back to the "..." button that this was never disabled if there was no
    // segment, and therefore no label to edit.  So we disable the edit button
    // and repeat checkbox first:
    m_edit->setEnabled(false);


    for (it = m_segments.begin(); it != m_segments.end(); ++it) {
        // ok, first thing is we know we have at least one segment
        //
        // and since there is at least one segment, we can re-enable the edit button
        // and repeat checkbox:
        m_edit->setEnabled(true);

        if (highlow == NotApplicable)
            highlow = None;

        // Highest/Lowest playable
        //
        if (it == m_segments.begin()) {
            myHigh = (unsigned int)(*it)->getHighestPlayable();
            myLow = (unsigned int)(*it)->getLowestPlayable();
        } else {
            if (myHigh != (unsigned int)(*it)->getHighestPlayable() ||
                myLow != (unsigned int)(*it)->getLowestPlayable()) {
                highlow = All;
            }
        }

    }
}

void SegmentParameterBox::slotRepeatPressed()
{
    // ??? We need to redo this so that it uses QCheckBox the
    //     way it was intended to be used.  E.g. we should be handling
    //     the clicked() signal, not pressed().  TristateCheckbox
    //     should provide a nextCheckState() that skips over
    //     PartiallyChecked.  Etc...

    if (m_segments.size() == 0)
        return ;

    bool state = false;

    switch (m_repeat->checkState()) {
    case Qt::Unchecked:
        state = true;
        break;

    case Qt::PartiallyChecked:
    case Qt::Checked:
    default:
        state = false;
        break;
    }

    // update the check box and all current Segments
    m_repeat->setChecked(state);

    CommandHistory::getInstance()->addCommand(
            new SegmentCommandRepeat(m_segments, state));

    //     SegmentVector::iterator it;

    //     for (it = m_segments.begin(); it != m_segments.end(); it++)
    //         (*it)->setRepeating(state);
}

void
SegmentParameterBox::slotQuantizeSelected(int qLevel)
{
    bool off = (qLevel == m_quantize->count() - 1);

    SegmentChangeQuantizationCommand *command =
        new SegmentChangeQuantizationCommand
        (off ? 0 : m_standardQuantizations[qLevel]);

    SegmentVector::iterator it;
    for (it = m_segments.begin(); it != m_segments.end(); ++it) {
        command->addSegment(*it);
    }

    CommandHistory::getInstance()->addCommand(command);
}

void
SegmentParameterBox::slotTransposeTextChanged(const QString &text)
{
    if (text.isEmpty() || m_segments.size() == 0)
        return ;

    int transposeValue = text.toInt();

    //CommandHistory::getInstance()->addCommand(
    //        new SegmentCommandChangeTransposeValue(m_segments, transposeValue));

    SegmentVector::iterator it;
    for (it = m_segments.begin(); it != m_segments.end(); ++it) {
        (*it)->setTranspose(transposeValue);
    }

    emit documentModified();
}

void
SegmentParameterBox::slotTransposeSelected(int value)
{
    slotTransposeTextChanged(m_transpose->itemText(value));
}

void
SegmentParameterBox::slotChangeLinkTranspose()
{
    if (m_segments.size() == 0)
        return ;

    bool foundTransposedLinks = false;
    SegmentVector linkedSegs;
    SegmentVector::iterator it;
    for (it = m_segments.begin(); it != m_segments.end(); ++it) {
        Segment *linkedSeg = *it;
        if (linkedSeg->isLinked()) {
            if (linkedSeg->getLinkTransposeParams().m_semitones==0) {
                linkedSegs.push_back(linkedSeg);
            } else {
                foundTransposedLinks = true;
                break;
            }
        }
    }
    
    if (foundTransposedLinks) {
        QMessageBox::critical(this, tr("Rosegarden"), 
                tr("Existing transpositions on selected linked segments must be removed\nbefore new transposition can be applied."),
                QMessageBox::Ok);
        return;
    }
        
    if (linkedSegs.size()==0) {
        return;
    }
    
    IntervalDialog intervalDialog(this, true, true);
    int ok = intervalDialog.exec();
    
    if (!ok) {
        return;
    }

    bool changeKey = intervalDialog.getChangeKey();
    int steps = intervalDialog.getDiatonicDistance();
    int semitones = intervalDialog.getChromaticDistance();
    bool transposeSegmentBack = intervalDialog.getTransposeSegmentBack();
     
    CommandHistory::getInstance()->addCommand
        (new SegmentLinkTransposeCommand(linkedSegs, changeKey, steps, 
                                         semitones, transposeSegmentBack));
}

void
SegmentParameterBox::slotResetLinkTranspose()
{
    if (m_segments.size() == 0)
        return ;

    SegmentVector linkedSegs;
    SegmentVector::iterator it;
    for (it = m_segments.begin(); it != m_segments.end(); ++it) {
        Segment *linkedSeg = *it;
        if (linkedSeg->isLinked()) {
            linkedSegs.push_back(linkedSeg);
        }
    }

    if (linkedSegs.size() == 0) {
        return;
    }

    int reset = QMessageBox::question(this, tr("Rosegarden"), 
                   tr("Remove transposition on selected linked segments?"));

    if (reset == QMessageBox::No) {
        return ;
    }

    CommandHistory::getInstance()->addCommand
        (new SegmentLinkResetTransposeCommand(linkedSegs));
}

void
SegmentParameterBox::slotDelayTimeChanged(timeT delayValue)
{
    // by convention and as a nasty hack, we use negative timeT here
    // to represent positive RealTime in ms

    if (delayValue > 0) {

        SegmentVector::iterator it;
        for (it = m_segments.begin(); it != m_segments.end(); ++it) {
            (*it)->setDelay(delayValue);
            (*it)->setRealTimeDelay(RealTime::zeroTime);
        }

    } else if (delayValue < 0) {

        SegmentVector::iterator it;
        for (it = m_segments.begin(); it != m_segments.end(); ++it) {
            (*it)->setDelay(0);
            int sec = ( -delayValue) / 1000;
            int nsec = (( -delayValue) - 1000 * sec) * 1000000;
            (*it)->setRealTimeDelay(RealTime(sec, nsec));
        }
    } else {

        SegmentVector::iterator it;
        for (it = m_segments.begin(); it != m_segments.end(); ++it) {
            (*it)->setDelay(0);
            (*it)->setRealTimeDelay(RealTime::zeroTime);
        }
    }

    emit documentModified();
}

void
SegmentParameterBox::slotDelayTextChanged(const QString &text)
{
    if (text.isEmpty() || m_segments.size() == 0)
        return ;

    slotDelayTimeChanged( -(text.toInt()));
}

void
SegmentParameterBox::slotDelaySelected(int value)
{
    if (value < int(m_delays.size())) {
        slotDelayTimeChanged(m_delays[value]);
    } else {
        slotDelayTimeChanged( -(m_realTimeDelays[value - m_delays.size()]));
    }
}

void
SegmentParameterBox::slotColourChanged(int index)
{
    SegmentSelection segments = getSelectedSegments();
    SegmentColourCommand *command =
            new SegmentColourCommand(segments, index);

    CommandHistory::getInstance()->addCommand(command);

#if 0
// This will never happen since the "Add Color" option is never added.
    if (index == m_addColourPos) {
        ColourMap newMap = m_doc->getComposition().getSegmentColourMap();
        QColor newColour;
        bool ok = false;

        QString newName = InputDialog::getText(this,
                                               tr("New Color Name"),
                                               tr("Enter new name:"),
                                               LineEdit::Normal,
                                               tr("New"), &ok);

        if ((ok == true) && (!newName.isEmpty())) {
//             QColorDialog box(this, "", true);
//             int result = box.getColor(newColour);

            //QRgb QColorDialog::getRgba(0xffffffff, &ok, this);
            QColor newColor = QColorDialog::getColor(Qt::white, this);

            if (newColor.isValid()) {
                Colour newRColour = GUIPalette::convertColour(newColour);
                newMap.addItem(newRColour, qstrtostr(newName));
                SegmentColourMapCommand *command =
                        new SegmentColourMapCommand(m_doc, newMap);
                CommandHistory::getInstance()->addCommand(command);
                slotDocColoursChanged();
            }
        }
        // Else we don't do anything as they either didn't give a name·
        // or didn't give a colour
    }
#endif
}

void
SegmentParameterBox::slotEditSegmentLabel()
{
    QString editLabel;

    //!!!  This is too simplistic to be translated properly, but I'm leaving it
    // alone.  The right way is to use %n and all that, but we don't want the
    // number to appear in any version of the string, and I don't see a way to
    // handle plurals without a %n placemarker.
    if (m_segments.size() == 0) return;
    else if (m_segments.size() == 1) editLabel = tr("Modify Segment label");
    else editLabel = tr("Modify Segments label");

    bool ok = false;

    // Remove the asterisk if we're using it
    //
    QString label = m_label->text();
    if (label == "*")
        label = "";

    QString newLabel = InputDialog::getText(this, 
                                            editLabel,
                                            tr("Enter new label:"),
                                            LineEdit::Normal,
                                            m_label->text(),
                                            &ok);

    if (ok) {
        SegmentSelection segments;
        SegmentVector::iterator it;
        for (it = m_segments.begin(); it != m_segments.end(); ++it)
            segments.insert(*it);

        SegmentLabelCommand *command = new
                                       SegmentLabelCommand(segments, newLabel);

        CommandHistory::getInstance()->addCommand(command);

        // fix #1776915, maybe?
        slotUpdate();
    }
}

void
SegmentParameterBox::slotNewDocument(RosegardenDocument *doc)
{
    // Connect to the new document.
    m_doc = doc;
    m_doc->getComposition().addObserver(this);

    // Make sure everything is correct.
    slotUpdate();
}


}
