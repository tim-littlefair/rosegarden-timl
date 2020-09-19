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

#define RG_MODULE_STRING "[MIDIConfigurationPage]"

#include "MIDIConfigurationPage.h"

#include "misc/ConfigGroups.h"  // For GeneralOptionsConfigGroup...
#include "misc/Debug.h"
#include "gui/widgets/FileDialog.h"
#include "gui/widgets/LineEdit.h"
#include "sound/MappedEvent.h"
#include "document/RosegardenDocument.h"
#include "sequencer/RosegardenSequencer.h"
#include "gui/application/RosegardenMainWindow.h"
#include "gui/seqmanager/SequenceManager.h"
#include "base/Studio.h"
#include "gui/studio/StudioControl.h"

#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QStringList>
#include <QWidget>


namespace Rosegarden
{


MIDIConfigurationPage::MIDIConfigurationPage(QWidget *parent):
    TabbedConfigurationPage(parent),
    m_baseOctaveNumber(nullptr)
{

    // ---------------- General tab ------------------

    QWidget *widget = new QWidget;
    addTab(widget, tr("General"));

    QGridLayout *layout = new QGridLayout(widget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(5);

    int row = 0;

    QSettings settings;
    settings.beginGroup(GeneralOptionsConfigGroup);

    // Base octave number
    layout->addWidget(
            new QLabel(tr("Base octave number for MIDI pitch display")),
            row, 0, 1, 2);

    m_baseOctaveNumber = new QSpinBox;
    m_baseOctaveNumber->setMinimum(-10);
    m_baseOctaveNumber->setMaximum(10);
    m_baseOctaveNumber->setValue(
            settings.value("midipitchoctave", -2).toInt());
    connect(m_baseOctaveNumber,
                static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MIDIConfigurationPage::slotModified);
    layout->addWidget(m_baseOctaveNumber, row, 2, 1, 2);

    ++row;

    // Spacer
    layout->setRowMinimumHeight(row, 20);
    ++row;

    // Always use default studio
    layout->addWidget(
            new QLabel(tr("Always use default studio when loading files")),
            row, 0, 1, 2);

    m_useDefaultStudio = new QCheckBox;
    m_useDefaultStudio->setChecked(
            settings.value("alwaysusedefaultstudio", false).toBool());
    connect(m_useDefaultStudio, &QCheckBox::stateChanged,
            this, &MIDIConfigurationPage::slotModified);
    layout->addWidget(m_useDefaultStudio, row, 2);

    ++row;

    // External controller port
    QLabel *label = new QLabel(tr("External controller port"));
    QString toolTip = tr("Enable the external controller port for control surfaces.");
    label->setToolTip(toolTip);
    layout->addWidget(label, row, 0, 1, 2);

    m_externalControllerPort = new QCheckBox;
    m_externalControllerPort->setToolTip(toolTip);
    m_externalControllerPort->setChecked(
            settings.value("external_controller", false).toBool());
    connect(m_externalControllerPort, &QCheckBox::stateChanged,
            this, &MIDIConfigurationPage::slotModified);
    layout->addWidget(m_externalControllerPort, row, 2);

    ++row;

    settings.endGroup();
    settings.beginGroup(SequencerOptionsConfigGroup);

    // Allow Reset All Controllers
    label = new QLabel(tr("Allow Reset All Controllers (CC 121)"));
    toolTip = tr("Rosegarden can send a MIDI Reset All Controllers event when setting up a channel.");
    label->setToolTip(toolTip);
    layout->addWidget(label, row, 0, 1, 2);

    m_allowResetAllControllers = new QCheckBox;
    m_allowResetAllControllers->setToolTip(toolTip);
    const bool sendResetAllControllers =
            settings.value("allowresetallcontrollers", true).toBool();
    m_allowResetAllControllers->setChecked(sendResetAllControllers);
    connect(m_allowResetAllControllers, &QCheckBox::stateChanged,
            this, &MIDIConfigurationPage::slotModified);
    layout->addWidget(m_allowResetAllControllers, row, 2);

    ++row;

    // Sequencer timing source
    label = new QLabel(tr("Sequencer timing source"));
    layout->addWidget(label, row, 0, 1, 2);

    m_sequencerTimingSource = new QComboBox;

    m_originalTimingSource =
            RosegardenSequencer::getInstance()->getCurrentTimer();

    unsigned timerCount = RosegardenSequencer::getInstance()->getTimers();

    for (unsigned i = 0; i < timerCount; ++i) {
        QString timer = RosegardenSequencer::getInstance()->getTimer(i);

        // Skip the HR timer which causes a hard-lock of the computer.
        if (timer == "HR timer")
            continue;

        m_sequencerTimingSource->addItem(timer);
        if (timer == m_originalTimingSource)
            m_sequencerTimingSource->setCurrentIndex(i);
    }

    connect(m_sequencerTimingSource,
                static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            this, &MIDIConfigurationPage::slotModified);
    layout->addWidget(m_sequencerTimingSource, row, 2, 1, 2);

    ++row;

    // Spacer
    layout->setRowMinimumHeight(row, 20);
    ++row;

    // Load SoundFont to SoundBlaster
    label = new QLabel(tr("Load SoundFont to SoundBlaster card at startup"));
    toolTip = tr("Check this box to enable soundfont loading on EMU10K-based cards when Rosegarden is launched");
    label->setToolTip(toolTip);
    layout->addWidget(label, row, 0, 1, 2);

    m_loadSoundFont = new QCheckBox;
    m_loadSoundFont->setToolTip(toolTip);
    m_loadSoundFont->setChecked(
            settings.value("sfxloadenabled", false).toBool());
    connect(m_loadSoundFont, &QCheckBox::stateChanged,
            this, &MIDIConfigurationPage::slotModified);
    connect(m_loadSoundFont, &QAbstractButton::clicked,
            this, &MIDIConfigurationPage::slotLoadSoundFontClicked);
    layout->addWidget(m_loadSoundFont, row, 2);

    ++row;

    // Path to 'asfxload'
    layout->addWidget(
            new QLabel(tr("Path to 'asfxload' or 'sfxload' command")),
            row, 0);

    m_pathToLoadCommand = new LineEdit(
            settings.value("sfxloadpath", "/usr/bin/asfxload").toString());
    layout->addWidget(m_pathToLoadCommand, row, 1, 1, 2);

    m_pathToLoadChoose = new QPushButton(tr("Choose..."));
    connect(m_pathToLoadChoose, &QAbstractButton::clicked,
            this, &MIDIConfigurationPage::slotPathToLoadChoose);
    layout->addWidget(m_pathToLoadChoose, row, 3);

    ++row;

    // SoundFont
    layout->addWidget(new QLabel(tr("SoundFont")), row, 0);

    m_soundFont = new LineEdit(
            settings.value("soundfontpath", "").toString());
    layout->addWidget(m_soundFont, row, 1, 1, 2);

    m_soundFontChoose = new QPushButton(tr("Choose..."));
    connect(m_soundFontChoose, &QAbstractButton::clicked,
            this, &MIDIConfigurationPage::slotSoundFontChoose);
    layout->addWidget(m_soundFontChoose, row, 3);

    ++row;

    // Fill out the rest of the space so that we do not end up centered.
    layout->setRowStretch(row, 10);


    //  -------------- MIDI Sync tab -----------------

    widget = new QWidget;
    addTab(widget, tr("MIDI Sync"));

    layout = new QGridLayout(widget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(5);

    row = 0;

    // MIDI Clock and System messages
    label = new QLabel(tr("MIDI Clock and System messages"));
    layout->addWidget(label, row, 0);

    m_midiClock = new QComboBox;
    m_midiClock->addItem(tr("Off"));
    m_midiClock->addItem(tr("Send MIDI Clock, Start and Stop"));
    m_midiClock->addItem(tr("Accept Start, Stop and Continue"));

    int midiClock = settings.value("midiclock", 0).toInt();
    if (midiClock < 0  ||  midiClock > 2)
        midiClock = 0;
    m_midiClock->setCurrentIndex(midiClock);

    connect(m_midiClock,
                static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            this, &MIDIConfigurationPage::slotModified);

    layout->addWidget(m_midiClock, row, 1);

    ++row;

    // MIDI Machine Control mode
    label = new QLabel(tr("MIDI Machine Control mode"));
    layout->addWidget(label, row, 0);

    m_midiMachineControlMode = new QComboBox;
    m_midiMachineControlMode->addItem(tr("Off"));
    m_midiMachineControlMode->addItem(tr("MMC Source"));
    m_midiMachineControlMode->addItem(tr("MMC Follower"));

    int mmcMode = settings.value("mmcmode", 0).toInt();
    if (mmcMode < 0  ||  mmcMode > 2)
        mmcMode = 0;
    m_midiMachineControlMode->setCurrentIndex(mmcMode);

    connect(m_midiMachineControlMode,
                static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            this, &MIDIConfigurationPage::slotModified);

    layout->addWidget(m_midiMachineControlMode, row, 1);
    
    ++row;

    // MIDI Time Code mode
    label = new QLabel(tr("MIDI Time Code mode"));
    layout->addWidget(label, row, 0);

    m_midiTimeCodeMode = new QComboBox;
    m_midiTimeCodeMode->addItem(tr("Off"));
    m_midiTimeCodeMode->addItem(tr("MTC Source"));
    m_midiTimeCodeMode->addItem(tr("MTC Follower"));

    int mtcMode = settings.value("mtcmode", 0).toInt();
    if (mtcMode < 0  ||  mtcMode > 2)
        mtcMode = 0;
    m_midiTimeCodeMode->setCurrentIndex(mtcMode);

    connect(m_midiTimeCodeMode,
                static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            this, &MIDIConfigurationPage::slotModified);

    layout->addWidget(m_midiTimeCodeMode, row, 1);

    ++row;

    label = new QLabel(
            tr("Automatically connect sync output to all devices in use"));
    label->setWordWrap(true);
    layout->addWidget(label, row, 0);

    m_autoConnectSyncOut = new QCheckBox;
    m_autoConnectSyncOut->setChecked(
            settings.value("midisyncautoconnect", false).toBool());

    connect(m_autoConnectSyncOut, &QCheckBox::stateChanged,
            this, &MIDIConfigurationPage::slotModified);

    layout->addWidget(m_autoConnectSyncOut, row, 1);

    ++row;

    // Fill out the rest of the space so that we do not end up centered.
    layout->setRowStretch(row, 10);

    updateWidgets();

}

void
MIDIConfigurationPage::updateWidgets()
{
    const bool soundFontChecked = m_loadSoundFont->isChecked();

    m_pathToLoadCommand->setEnabled(soundFontChecked);
    m_pathToLoadChoose->setEnabled(soundFontChecked);
    m_soundFont->setEnabled(soundFontChecked);
    m_soundFontChoose->setEnabled(soundFontChecked);
}

void
MIDIConfigurationPage::slotLoadSoundFontClicked(bool /*isChecked*/)
{
    updateWidgets();
}

void
MIDIConfigurationPage::slotPathToLoadChoose()
{
    QString path = FileDialog::getOpenFileName(
            this,  // parent
            tr("sfxload path"),  // caption
            QDir::currentPath());  // dir

    // Canceled?  Bail.
    if (path == "")
        return;

    m_pathToLoadCommand->setText(path);
}

void
MIDIConfigurationPage::slotSoundFontChoose()
{
    QString path = FileDialog::getOpenFileName(
            this,  // parent
            tr("Soundfont path"),  // caption
            QDir::currentPath(),  // dir
            tr("Sound fonts") + " (*.sb *.sf2 *.SF2 *.SB)" + ";;" +
                tr("All files") + " (*)");  // filter

    // Canceled?  Bail.
    if (path == "")
        return;

    m_soundFont->setText(path);
}

void
MIDIConfigurationPage::apply()
{
    //RG_DEBUG << "apply()...";

    // Copy from controls to .conf file.  Send update messages as needed.


    // *** General tab

    QSettings settings;
    settings.beginGroup(GeneralOptionsConfigGroup);

    settings.setValue("midipitchoctave", m_baseOctaveNumber->value());
    settings.setValue("alwaysusedefaultstudio",
                      m_useDefaultStudio->isChecked());
    settings.setValue("external_controller",
                      m_externalControllerPort->isChecked());

    settings.endGroup();

    settings.beginGroup(SequencerOptionsConfigGroup);

    settings.setValue("allowresetallcontrollers",
                      m_allowResetAllControllers->isChecked());

    // If the timer setting has actually changed
    if (m_sequencerTimingSource->currentText() != m_originalTimingSource) {
        RosegardenSequencer::getInstance()->setCurrentTimer(
                m_sequencerTimingSource->currentText());
        // In case this is an Apply without exit, update the cache
        // so that we detect any further changes.
        m_originalTimingSource = m_sequencerTimingSource->currentText();
    }

    settings.setValue("sfxloadenabled", m_loadSoundFont->isChecked());

    settings.setValue("sfxloadpath", m_pathToLoadCommand->text());
    settings.setValue("soundfontpath", m_soundFont->text());


    // *** MIDI Sync tab

    // MIDI Clock and System messages
    const int midiClock = m_midiClock->currentIndex();
    settings.setValue("midiclock", midiClock);

    // Now send it (OLD METHOD - to be removed)
    // !!! No, don't remove -- this controls SPP as well doesn't it?
    MappedEvent midiClockEvent(MidiInstrumentBase,  // InstrumentId
                               MappedEvent::SystemMIDIClock,
                               MidiByte(midiClock));
    StudioControl::sendMappedEvent(midiClockEvent);

    settings.setValue("mmcmode", m_midiMachineControlMode->currentIndex());
    MappedEvent mmcModeEvent(MidiInstrumentBase,  // InstrumentId
                             MappedEvent::SystemMMCTransport,
                             MidiByte(m_midiMachineControlMode->currentIndex()));
    StudioControl::sendMappedEvent(mmcModeEvent);

    settings.setValue("mtcmode", m_midiTimeCodeMode->currentIndex());
    MappedEvent mtcModeEvent(MidiInstrumentBase,  // InstrumentId
                             MappedEvent::SystemMTCTransport,
                             MidiByte(m_midiTimeCodeMode->currentIndex()));
    StudioControl::sendMappedEvent(mtcModeEvent);

    settings.setValue("midisyncautoconnect", m_autoConnectSyncOut->isChecked());
    MappedEvent autoConnectSyncOutEvent(
            MidiInstrumentBase,  // InstrumentId
            MappedEvent::SystemMIDISyncAuto,
            MidiByte(m_autoConnectSyncOut->isChecked() ? 1 : 0));
    StudioControl::sendMappedEvent(autoConnectSyncOutEvent);

    settings.endGroup();


    // Update the metronome mapped segment with new clock ticks if needed.

    RosegardenDocument *doc = RosegardenMainWindow::self()->getDocument();
    Studio &studio = doc->getStudio();
    const MidiMetronome *metronome = studio.getMetronomeFromDevice(
            studio.getMetronomeDevice());

    if (metronome) {
        InstrumentId instrument = metronome->getInstrument();
        doc->getSequenceManager()->metronomeChanged(instrument, true);
    }

}


}
