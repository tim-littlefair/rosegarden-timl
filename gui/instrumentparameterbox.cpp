// -*- c-basic-offset: 4 -*-

/*
    Rosegarden-4
    A sequencer and musical notation editor.

    This program is Copyright 2000-2003
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

#include <iostream>
#include <cstdio>

#include <qdial.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qregexp.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <qsignalmapper.h>
#include <qwidgetstack.h>
#include <qsignalmapper.h>

#include <kcombobox.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kapp.h>

#include "Midi.h"
#include "Instrument.h"
#include "MidiDevice.h"
#include "MappedStudio.h"
#include "ControlParameter.h"
#include "AudioLevel.h"

#include "audioplugindialog.h"
#include "instrumentparameterbox.h"
#include "audiopluginmanager.h"
#include "widgets.h"
#include "studiocontrol.h"
#include "rosegardenguidoc.h"
#include "trackvumeter.h"
#include "rosegardengui.h"

#include "rosestrings.h"
#include "rosedebug.h"

#include "studiocontrol.h"
#include "studiowidgets.h"

using Rosegarden::Instrument;
using Rosegarden::MidiDevice;

InstrumentParameterBox::InstrumentParameterBox(RosegardenGUIDoc *doc,
                                               QWidget *parent)
    : RosegardenParameterBox(1, Qt::Horizontal, i18n("Instrument Parameters"), parent),
//      m_widgetStack(new QWidgetStack(this)),
      m_widgetStack(0), //!!!
      m_noInstrumentParameters(new QVBox(this)),
      m_midiInstrumentParameters(new MIDIInstrumentParameterPanel(doc, this)),
      m_audioInstrumentParameters(new AudioInstrumentParameterPanel(doc, this)),
      m_selectedInstrument(0),
      m_doc(doc)
{
//     m_widgetStack->setFont(m_font);
    m_noInstrumentParameters->setFont(m_font);
    m_midiInstrumentParameters->setFont(m_font);
    m_audioInstrumentParameters->setFont(m_font);

    bool contains = false;

    std::vector<InstrumentParameterBox*>::iterator it =
        instrumentParamBoxes.begin();

    for (; it != instrumentParamBoxes.end(); it++)
        if ((*it) == this)
            contains = true;

    if (!contains)
        instrumentParamBoxes.push_back(this);

    QLabel *label = new QLabel(i18n("<no instrument>"), m_noInstrumentParameters);
    label->setAlignment(label->alignment() | Qt::AlignHCenter);

//     m_widgetStack->addWidget(m_midiInstrumentParameters);
//     m_widgetStack->addWidget(m_audioInstrumentParameters);
//     m_widgetStack->addWidget(m_noInstrumentParameters);

    m_midiInstrumentParameters->adjustSize();
    m_audioInstrumentParameters->adjustSize();
    m_noInstrumentParameters->adjustSize();

    connect(m_audioInstrumentParameters, SIGNAL(updateAllBoxes()),
            this, SLOT(slotUpdateAllBoxes()));

    connect(m_audioInstrumentParameters,
            SIGNAL(muteButton(Rosegarden::InstrumentId, bool)),
            this, 
            SIGNAL(setMute(Rosegarden::InstrumentId, bool)));
    
    connect(m_audioInstrumentParameters,
            SIGNAL(soloButton(Rosegarden::InstrumentId, bool)),
            this,
            SIGNAL(setSolo(Rosegarden::InstrumentId, bool)));
    
    connect(m_audioInstrumentParameters,
            SIGNAL(recordButton(Rosegarden::InstrumentId, bool)),
            this,
            SIGNAL(setRecord(Rosegarden::InstrumentId, bool)));
    
    connect(m_midiInstrumentParameters, SIGNAL(updateAllBoxes()),
            this, SLOT(slotUpdateAllBoxes()));

    connect(m_midiInstrumentParameters, SIGNAL(changeInstrumentLabel(Rosegarden::InstrumentId, QString)),
            this, SIGNAL(changeInstrumentLabel(Rosegarden::InstrumentId, QString)));
}


InstrumentParameterBox::~InstrumentParameterBox()
{
    // deregister this parameter box
    std::vector<InstrumentParameterBox*>::iterator it =
        instrumentParamBoxes.begin();

    for (; it != instrumentParamBoxes.end(); it++)
    {
        if ((*it) == this)
        {
            instrumentParamBoxes.erase(it);
            break;
        }
    }
}

void
InstrumentParameterBox::setAudioMeter(float ch1, float ch2)
{
    m_audioInstrumentParameters->setAudioMeter(ch1, ch2);
}

void
InstrumentParameterBox::setDocument(RosegardenGUIDoc* doc)
{
    m_doc = doc;
    m_midiInstrumentParameters->setDocument(m_doc);
    m_audioInstrumentParameters->setDocument(m_doc);
}


void
InstrumentParameterBox::useInstrument(Instrument *instrument)
{
    RG_DEBUG << "useInstrument() - populate Instrument\n";

    if (instrument == 0)
    {
//         m_widgetStack->raiseWidget(m_noInstrumentParameters);
	m_audioInstrumentParameters->hide();
	m_midiInstrumentParameters->hide();
	if (m_noInstrumentParameters->isHidden()) 
	    m_noInstrumentParameters->show();
        return;
    } 

    // ok
    m_selectedInstrument = instrument;

    // Hide or Show according to Instrumen type
    //
    if (instrument->getType() == Instrument::Audio)
    {
	m_noInstrumentParameters->hide();
	m_midiInstrumentParameters->hide();
        m_audioInstrumentParameters->setupForInstrument(m_selectedInstrument);
	if (m_audioInstrumentParameters->isHidden())
	    m_audioInstrumentParameters->show();

//         m_widgetStack->raiseWidget(m_audioInstrumentParameters);

    } else { // Midi

	m_noInstrumentParameters->hide();
	m_audioInstrumentParameters->hide();
        m_midiInstrumentParameters->setupForInstrument(m_selectedInstrument);
	if (m_midiInstrumentParameters->isHidden())
	    m_midiInstrumentParameters->show();

//         m_widgetStack->raiseWidget(m_midiInstrumentParameters);
    }
    
}

void
InstrumentParameterBox::setMute(bool value)
{
    if (m_selectedInstrument && 
            m_selectedInstrument->getType() == Instrument::Audio)
    {
        m_audioInstrumentParameters->slotSetMute(value);
    }
}

/*
 * Set the record state of the audio instrument parameter panel
 */
void
InstrumentParameterBox::setRecord(bool value)
{
    if (m_selectedInstrument &&
            m_selectedInstrument->getType() == Instrument::Audio)
    {
        m_audioInstrumentParameters->slotSetRecord(value);
    }
}

void
InstrumentParameterBox::setSolo(bool value)
{
    if (m_selectedInstrument &&
            m_selectedInstrument->getType() == Instrument::Audio)
    {
        m_audioInstrumentParameters->slotSetSolo(value);
    }
}



void
AudioInstrumentParameterPanel::slotSelectAudioLevel(float dB)
{
    if (m_selectedInstrument == 0)
        return;

    std::cerr << "AudioInstrumentParameterPanel::slotSelectAudioLevel("
	      << dB << ")" << std::endl;

    if (m_selectedInstrument->getType() == Instrument::Audio)
    {
        // If this is the record track then we store and send the record level
        //
        if (m_audioFader->m_recordButton->isOn())
        {
            //cout << "SETTING STORED RECORD LEVEL = " << value << endl;

	    m_selectedInstrument->setRecordLevel(dB);

            Rosegarden::StudioControl::setStudioObjectProperty
              (Rosegarden::MappedObjectId(m_selectedInstrument->getMappedId()),
               Rosegarden::MappedAudioFader::FaderRecordLevel,
               Rosegarden::MappedObjectValue(dB));
        }
        else
        {
            //cout << "SETTING STORED LEVEL = " << value << endl;

	    m_selectedInstrument->setLevel(dB);

            Rosegarden::StudioControl::setStudioObjectProperty
              (Rosegarden::MappedObjectId(m_selectedInstrument->getMappedId()),
               Rosegarden::MappedAudioFader::FaderLevel,
               Rosegarden::MappedObjectValue(dB));
        }
    }

    emit updateAllBoxes();
}

void 
AudioInstrumentParameterPanel::slotSetMute(bool value)
{
    RG_DEBUG << "AudioInstrumentParameterPanel::slotSetMute - "
             << "value = " << value << endl;
    m_audioFader->m_muteButton->setOn(value);
}

void
AudioInstrumentParameterPanel::slotSetSolo(bool value)
{
    RG_DEBUG << "AudioInstrumentParameterPanel::slotSetSolo - "
             << "value = " << value << endl;
    m_audioFader->m_soloButton->setOn(value);
}

void 
AudioInstrumentParameterPanel::slotSetRecord(bool value)
{
    RG_DEBUG << "AudioInstrumentParameterPanel::slotSetRecord - "
             << "value = " << value << endl;

    //if (m_selectedInstrument)
        //cout << "INSTRUMENT NAME = " 
               //<< m_selectedInstrument->getName() << endl;

    // Set the background colour for the button
    //
    if (value)
    {
        m_audioFader->m_recordButton->
            setPalette(QPalette(RosegardenGUIColours::ActiveRecordTrack));

        if (m_selectedInstrument &&
            (m_selectedInstrument->getType() == Instrument::Audio))
        {
            // set the fader value to the record value

            disconnect(m_audioFader->m_fader, SIGNAL(faderChanged(float)),
                       this, SLOT(slotSelectAudioLevel(float)));

            m_audioFader->m_fader->
//                setFader(m_selectedInstrument->getRecordLevel());
//		setFader(Rosegarden::AudioLevel::dB_to_fader
//			 (m_selectedInstrument->getRecordLevel(),
//			  127,
//			  Rosegarden::AudioLevel::ShortFader));
		setFader(m_selectedInstrument->getRecordLevel());

            //cout << "SETTING VISIBLE FADER RECORD LEVEL = " << 
                    //int(m_selectedInstrument->getRecordLevel()) << endl;

            connect(m_audioFader->m_fader, SIGNAL(faderChanged(float)),
                    this, SLOT(slotSelectAudioLevel(float)));

            // Set the prepend text on the audio fader
//!!!            m_audioFader->m_fader->setPrependText(i18n("Record level = "));
        }
    }
    else
    {
        m_audioFader->m_recordButton->unsetPalette();

        if (m_selectedInstrument &&
            (m_selectedInstrument->getType() == Instrument::Audio))
        {
            disconnect(m_audioFader->m_fader, SIGNAL(faderChanged(float)),
                       this, SLOT(slotSelectAudioLevel(float)));

            // set the fader value to the playback value
            m_audioFader->m_fader->
//                setFader(m_selectedInstrument->getVolume());
//		setFader(Rosegarden::AudioLevel::dB_to_fader
//			 (m_selectedInstrument->getLevel(),
//			  127,
//			  Rosegarden::AudioLevel::ShortFader));
		setFader(m_selectedInstrument->getLevel());

            //cout << "SETTING VISIBLE FADER LEVEL = " << 
                    //int(m_selectedInstrument->getVolume()) << endl;

            connect(m_audioFader->m_fader, SIGNAL(faderChanged(float)),
                    this, SLOT(slotSelectAudioLevel(float)));

            // Set the prepend text on the audio fader
//!!!            m_audioFader->m_fader->setPrependText(i18n("Playback level = "));
        }
    }

    m_audioFader->m_recordButton->setOn(value);
}


    

void
InstrumentParameterBox::slotUpdateAllBoxes()
{
    std::vector<InstrumentParameterBox*>::iterator it =
        instrumentParamBoxes.begin();

    for (; it != instrumentParamBoxes.end(); it++)
    {
        if ((*it) != this && m_selectedInstrument &&
            (*it)->getSelectedInstrument() == m_selectedInstrument)
            (*it)->useInstrument(m_selectedInstrument);
    }

}


void
AudioInstrumentParameterPanel::slotSelectPlugin(int index)
{
    int key = (index << 24) + m_selectedInstrument->getId();

    if (m_pluginDialogs[key] == 0)
    {
        // only create a dialog if we've got a plugin instance
        Rosegarden::AudioPluginInstance *inst = 
            m_selectedInstrument->getPlugin(index);

        if (inst)
        {
            // Create the plugin dialog
            //
            m_pluginDialogs[key] = 
                new Rosegarden::AudioPluginDialog(this,
                                              m_doc->getPluginManager(),
                                              m_selectedInstrument,
                                              index);

            // Get the App pointer and plug the new dialog into the 
            // standard keyboard accelerators so that we can use them
            // still while the plugin has focus.
            //
            QWidget *par = parentWidget();
            while (!par->isTopLevel()) par = par->parentWidget();

            RosegardenGUIApp *app = dynamic_cast<RosegardenGUIApp*>(par);

            app->plugAccelerators(m_pluginDialogs[key],
                                  m_pluginDialogs[key]->getAccelerators());

            connect(m_pluginDialogs[key], SIGNAL(pluginSelected(int, int)),
                    this, SLOT(slotPluginSelected(int, int)));

            connect(m_pluginDialogs[key],
		    SIGNAL(pluginPortChanged(int, int, float)),
                    this, SLOT(slotPluginPortChanged(int, int, float)));

            connect(m_pluginDialogs[key], SIGNAL(bypassed(int, bool)),
                    this, SLOT(slotBypassed(int, bool)));

	    connect(m_pluginDialogs[key], SIGNAL(destroyed(int)),
		    this, SLOT(slotPluginDialogDestroyed(int)));

            m_pluginDialogs[key]->show();

            // Set modified
            m_doc->slotDocumentModified();
        }
        else
        {
            RG_DEBUG << "AudioInstrumentParameterPanel::slotSelectPlugin - "
		     << "no AudioPluginInstance found for index "
		     << index << endl;
        }
    }
    else
    {
	m_pluginDialogs[key]->raise();
    }
}

void
AudioInstrumentParameterPanel::slotPluginSelected(int index, int plugin)
{

    Rosegarden::AudioPluginInstance *inst = 
        m_selectedInstrument->getPlugin(index);

    if (inst)
    {
        if (plugin == -1)
        {
            RG_DEBUG << "InstrumentParameterBox::slotPluginSelected - "
                       << "no plugin selected" << endl;

            // Destroy plugin instance
            if (Rosegarden::StudioControl::
                    destroyStudioObject(inst->getMappedId()))
            {
                RG_DEBUG << "InstrumentParameterBox::slotPluginSelected - "
                         << "cannot destroy Studio object "
                         << inst->getMappedId() << endl;
            }

            inst->setAssigned(false);
            m_audioFader->m_plugins[index]->setText(i18n("<no plugin>"));

        }
        else
        {
            Rosegarden::AudioPlugin *plgn = 
                m_doc->getPluginManager()->getPlugin(plugin);

            // If unassigned then create a sequencer instance of this
            // AudioPluginInstance.
            //
            if (inst->isAssigned())
            {
                // unassign, destory and recreate
                std::cout << "MAPPED ID = " << inst->getMappedId() 
			  << " for Instrument " << inst->getId() << std::endl;

                RG_DEBUG << "InstrumentParameterBox::slotPluginSelected - "
                         << "MappedObjectId = "
                         << inst->getMappedId()
                         << " - UniqueId = " << plgn->getUniqueId()
                         << endl;


#ifdef HAVE_LADSPA
                Rosegarden::StudioControl::setStudioObjectProperty
                    (inst->getMappedId(),
                     Rosegarden::MappedLADSPAPlugin::UniqueId,
                     plgn->getUniqueId());
#endif

            }
            else
            {
                // create a studio object at the sequencer
                Rosegarden::MappedObjectId newId =
                    Rosegarden::StudioControl::createStudioObject
                        (Rosegarden::MappedObject::LADSPAPlugin);

                RG_DEBUG << "InstrumentParameterBox::slotPluginSelected - "
                         << " new MappedObjectId = " << newId << endl;

                // set the new Mapped ID and that this instance
                // is assigned
                inst->setMappedId(newId);
                inst->setAssigned(true);

#ifdef HAVE_LADSPA
                // set the instrument id
                Rosegarden::StudioControl::setStudioObjectProperty
                    (newId,
                     Rosegarden::MappedObject::Instrument,
                     Rosegarden::MappedObjectValue(
                         m_selectedInstrument->getId()));

                // set the position
                Rosegarden::StudioControl::setStudioObjectProperty
                    (newId,
                     Rosegarden::MappedObject::Position,
                     Rosegarden::MappedObjectValue(index));

                // set the plugin id
                Rosegarden::StudioControl::setStudioObjectProperty
                    (newId,
                     Rosegarden::MappedLADSPAPlugin::UniqueId,
                     Rosegarden::MappedObjectValue(
                         plgn->getUniqueId()));
#endif
            }

            Rosegarden::AudioPlugin *pluginClass 
                = m_doc->getPluginManager()->getPlugin(plugin);

            if (pluginClass)
                m_audioFader->m_plugins[index]->
                    setText(pluginClass->getLabel());
        }

        // Set modified
        m_doc->slotDocumentModified();
    }
    else
        RG_DEBUG << "InstrumentParameterBox::slotPluginSelected - "
                 << "got index of unknown plugin!" << endl;
}

void
AudioInstrumentParameterPanel::slotPluginPortChanged(int pluginIndex,
                                                     int portIndex,
                                                     float value)
{
    Rosegarden::AudioPluginInstance *inst = 
        m_selectedInstrument->getPlugin(pluginIndex);

    if (inst)
    {

#ifdef HAVE_LADSPA

        Rosegarden::StudioControl::
            setStudioPluginPort(inst->getMappedId(),
                                portIndex,
                                value);
                                
        RG_DEBUG << "InstrumentParameterBox::slotPluginPortChanged - "
                 << "setting plugin port (" << portIndex << ") to "
                 << value << endl;

        // Set modified
        m_doc->slotDocumentModified();

#endif // HAVE_LADSPA

	(void)portIndex; // avoid compiler warnings
	(void)value;
    }

}


void
AudioInstrumentParameterPanel::slotPluginDialogDestroyed(int index)
{
    int key = (index << 24) + m_selectedInstrument->getId();
    m_pluginDialogs[key] = 0;
}

void
AudioInstrumentParameterPanel::slotBypassed(int pluginIndex, bool bp)
{
    Rosegarden::AudioPluginInstance *inst = 
        m_selectedInstrument->getPlugin(pluginIndex);

    if (inst)
    {
#ifdef HAVE_LADSPA
        Rosegarden::StudioControl::setStudioObjectProperty
            (inst->getMappedId(),
             Rosegarden::MappedLADSPAPlugin::Bypassed,
             Rosegarden::MappedObjectValue(bp));
#endif // HAVE_LADSPA


        /// Set the colour on the button
        //
        setBypassButtonColour(pluginIndex, bp);

        // Set the bypass on the instance
        //
        inst->setBypass(bp);

        // Set modified
        m_doc->slotDocumentModified();
    }
}

// Set the button colour
//
void
AudioInstrumentParameterPanel::setBypassButtonColour(int pluginIndex,
                                                     bool bypassState)
{
    // Set the bypass colour on the plugin button
    if (bypassState)
    {
        m_audioFader->m_plugins[pluginIndex]->
            setPaletteForegroundColor(kapp->palette().
                    color(QPalette::Active, QColorGroup::Button));

        m_audioFader->m_plugins[pluginIndex]->
            setPaletteBackgroundColor(kapp->palette().
                    color(QPalette::Active, QColorGroup::ButtonText));
    }
    else
    {
        m_audioFader->m_plugins[pluginIndex]->
            setPaletteForegroundColor(kapp->palette().
                    color(QPalette::Active, QColorGroup::ButtonText));

        m_audioFader->m_plugins[pluginIndex]->
            setPaletteBackgroundColor(kapp->palette().
                    color(QPalette::Active, QColorGroup::Button));
    }
}



InstrumentParameterPanel::InstrumentParameterPanel(RosegardenGUIDoc *doc, 
                                                   QWidget* parent)
    : QFrame(parent),
      m_instrumentLabel(new QLabel(this)),
      m_selectedInstrument(0),
      m_doc(doc)
{
}

void
InstrumentParameterPanel::setDocument(RosegardenGUIDoc* doc)
{
    m_doc = doc;
}

AudioInstrumentParameterPanel::AudioInstrumentParameterPanel(RosegardenGUIDoc* doc, QWidget* parent)
    : InstrumentParameterPanel(doc, parent),
      m_audioFader(new AudioFaderWidget(this, AudioFaderWidget::FaderBox))
{
    QGridLayout *gridLayout = new QGridLayout(this, 2, 1, 5, 5);

    // Instrument label : first row, all cols
    gridLayout->addMultiCellWidget(m_instrumentLabel, 0, 0, 0, 0, AlignCenter);

    // fader and connect it
    gridLayout->addMultiCellWidget(m_audioFader, 1, 1, 0, 0);

    connect(m_audioFader, SIGNAL(audioChannelsChanged(int)),
            this, SLOT(slotAudioChannels(int)));

    connect(m_audioFader->m_signalMapper, SIGNAL(mapped(int)),
            this, SLOT(slotSelectPlugin(int)));

    connect(m_audioFader->m_fader, SIGNAL(faderChanged(float)),
            this, SLOT(slotSelectAudioLevel(float)));

    connect(m_audioFader->m_muteButton, SIGNAL(clicked()),
            this, SLOT(slotMute()));

    connect(m_audioFader->m_soloButton, SIGNAL(clicked()),
            this, SLOT(slotSolo()));

    connect(m_audioFader->m_recordButton, SIGNAL(clicked()),
            this, SLOT(slotRecord()));

    connect(m_audioFader->m_pan, SIGNAL(valueChanged(float)),
            this, SLOT(slotSetPan(float)));

    connect(m_audioFader->m_audioInput, SIGNAL(activated(int)),
            this, SLOT(slotSelectAudioInput(int)));

}

void
AudioInstrumentParameterPanel::slotMute()
{
    RG_DEBUG << "AudioInstrumentParameterPanel::slotMute" << endl;
    emit muteButton(m_selectedInstrument->getId(),
                    m_audioFader->m_muteButton->isOn());
}

void
AudioInstrumentParameterPanel::slotSolo()
{
    RG_DEBUG << "AudioInstrumentParameterPanel::slotSolo" << endl;
    emit soloButton(m_selectedInstrument->getId(),
                    m_audioFader->m_soloButton->isOn());
}

void
AudioInstrumentParameterPanel::slotRecord()
{
    RG_DEBUG << "AudioInstrumentParameterPanel::slotRecord - " 
             << " isOn = " <<  m_audioFader->m_recordButton->isOn() << endl;

    // At the moment we can't turn a recording button off
    //
    if (m_audioFader->m_recordButton->isOn())
    {
        m_audioFader->m_recordButton->setOn(true);

        if (m_selectedInstrument)
        {
            //std::cout << "SETTING FADER RECORD LEVEL = " 
                 //<< int(m_selectedInstrument->getRecordLevel()) << endl;

            // set the fader value to the record value
            m_audioFader->m_fader->
                setFader
//		(Rosegarden::AudioLevel::dB_to_fader
//		 (m_selectedInstrument->getRecordLevel(),
//		  127, Rosegarden::AudioLevel::ShortFader));
		(m_selectedInstrument->getRecordLevel());
        }

        emit recordButton(m_selectedInstrument->getId(),
                          m_audioFader->m_recordButton->isOn());
    }
    else
    {
        m_audioFader->m_recordButton->setOn(true);
    }

    /*
    else
    {
        if (m_selectedInstrument)
        {
            // set the fader value to the record value
            m_audioFader->m_fader->
                setFader(m_selectedInstrument->getVolume());

            cout << "SETTING FADER LEVEL = " 
                 << int(m_selectedInstrument->getVolume()) << endl;
        }
    }
    */
}

void
AudioInstrumentParameterPanel::slotSetPan(float pan)
{
    RG_DEBUG << "AudioInstrumentParameterPanel::slotSetPan - "
             << "pan = " << pan << endl;

    Rosegarden::StudioControl::setStudioObjectProperty
        (Rosegarden::MappedObjectId(m_selectedInstrument->getMappedId()),
         Rosegarden::MappedAudioFader::Pan,
         Rosegarden::MappedObjectValue(pan));

    m_selectedInstrument->setPan(Rosegarden::MidiByte(pan + 100.0));
}

void
AudioInstrumentParameterPanel::setAudioMeter(float dBleft, float dBright)
{
    std::cerr << "AudioInstrumentParameterPanel::setAudioMeter: (" << dBleft
	      << "," << dBright << ")" << std::endl;
    
//    double ch1 = (double)Rosegarden::AudioLevel::dB_to_fader
//	(dBleft, 127, Rosegarden::AudioLevel::ShortFader);

    if (m_selectedInstrument)
    {
        if (m_selectedInstrument->getAudioChannels() == 1) {
//            m_audioFader->m_vuMeter->setLevel(ch1 / 127.0);
	    m_audioFader->m_vuMeter->setLevel(dBleft);
	} else {
//	    double ch2 = (double)Rosegarden::AudioLevel::dB_to_fader
//		(dBright, 127, Rosegarden::AudioLevel::ShortFader);
//            m_audioFader->m_vuMeter->setLevel(ch1 / 127.0, ch2 / 127.0);
	    m_audioFader->m_vuMeter->setLevel(dBleft, dBright);
	}
    }
}


void
AudioInstrumentParameterPanel::setupForInstrument(Instrument* instrument)
{
    m_selectedInstrument = instrument;

    m_instrumentLabel->setText(strtoqstr(instrument->getName()));

    /*
    if (m_audioFader->m_recordButton->isOn())
        m_audioFader->m_fader->setFader(instrument->getRecordLevel());
    else
        m_audioFader->m_fader->setFader(instrument->getVolume());
        */


    for (unsigned int i = 0; i < m_audioFader->m_plugins.size(); i++)
    {
        m_audioFader->m_plugins[i]->show();

        Rosegarden::AudioPluginInstance *inst = instrument->getPlugin(i);

        if (inst && inst->isAssigned())
        {
            Rosegarden::AudioPlugin *pluginClass 
                = m_doc->getPluginManager()->getPlugin(
                        m_doc->getPluginManager()->
                            getPositionByUniqueId(inst->getId()));

            if (pluginClass)
                m_audioFader->m_plugins[i]->
                    setText(pluginClass->getLabel());

            setBypassButtonColour(i, inst->isBypassed());

        }
        else
        {
            m_audioFader->m_plugins[i]->setText(i18n("<no plugin>"));

            if (inst)
                setBypassButtonColour(i, inst->isBypassed());
            else
                setBypassButtonColour(i, false);
        }
    }

    // Set the number of channels on the fader widget
    //
    m_audioFader->setAudioChannels(instrument->getAudioChannels());

    // Pan - adjusted backwards
    //
    m_audioFader->m_pan->setPosition(instrument->getPan() - 100);
}

void
AudioInstrumentParameterPanel::slotAudioChannels(int channels)
{
    RG_DEBUG << "AudioInstrumentParameterPanel::slotAudioChannels - "
             << "channels = " << channels << endl;

    m_selectedInstrument->setAudioChannels(channels);
    /*
    Rosegarden::MappedEvent *mE =
        new Rosegarden::MappedEvent(
                m_selectedInstrument->getId(),
                Rosegarden::MappedEvent::AudioChannels,
                Rosegarden::MidiByte(m_selectedInstrument->getAudioChannels()));

    Rosegarden::StudioControl::sendMappedEvent(mE);
    */

    Rosegarden::StudioControl::setStudioObjectProperty
        (Rosegarden::MappedObjectId(m_selectedInstrument->getMappedId()),
         Rosegarden::MappedAudioObject::Channels,
         Rosegarden::MappedObjectValue(channels));

}

void
AudioInstrumentParameterPanel::slotSelectAudioInput(int value)
{
    RG_DEBUG << "AudioInstrumentParameterPanel::slotSelectAudioInput - "
             << value << endl;

    /*
   Rosegarden::MappedObjectValueList connectionList;
   connectionList.push_back(value);
   */

   Rosegarden::StudioControl::setStudioObjectProperty
        (Rosegarden::MappedObjectId(m_selectedInstrument->getMappedId()),
         Rosegarden::MappedAudioObject::ConnectionsIn,
         Rosegarden::MappedObjectValue(value));
}




MIDIInstrumentParameterPanel::MIDIInstrumentParameterPanel(RosegardenGUIDoc *doc, QWidget* parent):
    InstrumentParameterPanel(doc, parent),
    m_rotaryFrame(0),
    m_rotaryMapper(new QSignalMapper(this))
{
    m_mainGrid = new QGridLayout(this, 8, 3, 8, 1);

    m_connectionLabel = new QLabel(this);
    m_bankValue = new KComboBox(this);
    m_channelValue = new KComboBox(this);
    m_programValue = new KComboBox(this);
    m_variationValue = new KComboBox(this);
    m_bankCheckBox = new QCheckBox(this);
    m_programCheckBox = new QCheckBox(this);
    m_variationCheckBox = new QCheckBox(this);
    m_percussionCheckBox = new QCheckBox(this);

    m_bankLabel = new QLabel(i18n("Bank"), this);
    m_variationLabel = new QLabel(i18n("Variation"), this);
    QLabel* programLabel = new QLabel(i18n("Program"), this);
    QLabel* channelLabel = new QLabel(i18n("Channel"), this);
    QLabel *percussionLabel = new QLabel(i18n("Percussion"), this);

    // Ensure a reasonable amount of space in the program dropdowns even
    // if no instrument initially selected
    QFontMetrics metrics(m_programValue->font());
    int width = metrics.width("Acoustic Grand Piano 123");
    m_bankValue->setMinimumWidth(width);
    m_programValue->setMinimumWidth(width);

    m_mainGrid->addMultiCellWidget(m_instrumentLabel, 0, 0, 0, 2, AlignCenter);
    m_mainGrid->addMultiCellWidget(m_connectionLabel, 1, 1, 0, 2, AlignCenter);

    m_mainGrid->addWidget(channelLabel, 2, 0, AlignLeft);
    m_mainGrid->addWidget(m_channelValue, 2, 2, AlignRight);

    m_mainGrid->addWidget(percussionLabel, 3, 0, AlignLeft);
    m_mainGrid->addWidget(m_percussionCheckBox, 3, 2, AlignRight);

    m_mainGrid->addWidget(m_bankLabel, 4, 0, AlignLeft);
    m_mainGrid->addWidget(m_bankCheckBox, 4, 1);
    m_mainGrid->addWidget(m_bankValue, 4, 2, AlignRight);

    m_mainGrid->addWidget(programLabel, 5, 0);
    m_mainGrid->addWidget(m_programCheckBox, 5, 1);
    m_mainGrid->addWidget(m_programValue, 5, 2, AlignRight);

    m_mainGrid->addWidget(m_variationLabel, 6, 0);
    m_mainGrid->addWidget(m_variationCheckBox, 6, 1);
    m_mainGrid->addWidget(m_variationValue, 6, 2, AlignRight);

    // Populate channel list
    for (int i = 0; i < 16; i++)
        m_channelValue->insertItem(QString("%1").arg(i+1));

    // Disable these by default - they are activate by their
    // checkboxes
    //
    m_programValue->setDisabled(true);
    m_bankValue->setDisabled(true);
    m_variationValue->setDisabled(true);

    // Only active if we have an Instrument selected
    //
    m_percussionCheckBox->setDisabled(true);
    m_programCheckBox->setDisabled(true);
    m_bankCheckBox->setDisabled(true);
    m_variationCheckBox->setDisabled(true);

    // Connect up the toggle boxes
    //
    connect(m_percussionCheckBox, SIGNAL(toggled(bool)),
	    this, SLOT(slotTogglePercussion(bool)));

    connect(m_programCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(slotToggleProgramChange(bool)));

    connect(m_bankCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(slotToggleBank(bool)));

    connect(m_variationCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(slotToggleVariation(bool)));


    // Connect activations
    //
    connect(m_bankValue, SIGNAL(activated(int)),
            this, SLOT(slotSelectBank(int)));

    connect(m_variationValue, SIGNAL(activated(int)),
            this, SLOT(slotSelectVariation(int)));

    connect(m_programValue, SIGNAL(activated(int)),
            this, SLOT(slotSelectProgram(int)));

    connect(m_channelValue, SIGNAL(activated(int)),
            this, SLOT(slotSelectChannel(int)));



    // don't select any of the options in any dropdown
    m_programValue->setCurrentItem(-1);
    m_bankValue->setCurrentItem(-1);
    m_channelValue->setCurrentItem(-1);
    m_variationValue->setCurrentItem(-1);

    connect(m_rotaryMapper, SIGNAL(mapped(int)),
            this, SLOT(slotControllerChanged(int)));
}

void
MIDIInstrumentParameterPanel::setupForInstrument(Instrument *instrument)
{
    MidiDevice *md = dynamic_cast<MidiDevice*>
	(instrument->getDevice());
    if (!md) {
	RG_DEBUG << "WARNING: MIDIInstrumentParameterPanel::setupForInstrument: No MidiDevice for Instrument "
	  << instrument->getId() << endl;
	return;
    }

    m_selectedInstrument = instrument;

    // Set instrument name
    //
    m_instrumentLabel->setText(strtoqstr(instrument->getPresentationName()));

    // Set Studio Device name
    //
    QString connection(strtoqstr(md->getConnection()));
    if (connection == "") {
	m_connectionLabel->setText(i18n("[ %1 ]").arg(i18n("No connection")));
    } else {
	
	// remove trailing "(duplex)", "(read only)", "(write only)" etc
	connection.replace(QRegExp("\\s*\\([^)0-9]+\\)\\s*$"), "");
	
	QString text = i18n("[ %1 ]").arg(connection);
	QString origText(text);
	
	QFontMetrics metrics(m_connectionLabel->fontMetrics());
	int maxwidth = metrics.width
	    ("Program: [X]   Acoustic Grand Piano 123");// kind of arbitrary!
	
	int hlen = text.length() / 2;
	while (metrics.width(text) > maxwidth && text.length() > 10) {
	    --hlen;
	    text = origText.left(hlen) + "..." + origText.right(hlen);
	}
	
	if (text.length() > origText.length() - 7) text = origText;
	m_connectionLabel->setText(text);
    }

    // Enable all check boxes
    //
    m_percussionCheckBox->setDisabled(false);
    m_programCheckBox->setDisabled(false);
    m_bankCheckBox->setDisabled(false);
    m_variationCheckBox->setDisabled(false);

    // Activate all checkboxes
    //
    m_percussionCheckBox->setChecked(instrument->isPercussion());
    m_programCheckBox->setChecked(instrument->sendsProgramChange());
    m_bankCheckBox->setChecked(instrument->sendsBankSelect());
    m_variationCheckBox->setChecked(instrument->sendsBankSelect());

    // Basic parameters
    //
    m_channelValue->setCurrentItem((int)instrument->getMidiChannel());

    // Check for program change
    //
    populateBankList();
    populateProgramList();
    populateVariationList();

    // Setup the ControlParameters
    //
    setupControllers(md);

    // Set all the positions by controller number
    //
    for (RotaryMap::iterator it = m_rotaries.begin() ; it != m_rotaries.end(); ++it)
    {
        Rosegarden::MidiByte value;
        try
        {
            value = instrument->getControllerValue(Rosegarden::MidiByte(it->first));
        }
        catch(...)
        {
            continue;
        }

        setRotaryToValue(it->first, int(value));
    }

    // Special cases
    //
    setRotaryToValue(int(Rosegarden::MIDI_CONTROLLER_PAN), instrument->getPan());
    setRotaryToValue(int(Rosegarden::MIDI_CONTROLLER_VOLUME), instrument->getVolume());

    /*

    m_panRotary->setPosition((float)instrument->getPan());
    m_volumeRotary->setPosition((float)instrument->getVolume());
    // Advanced MIDI controllers
    //
    m_chorusRotary->setPosition(float(instrument->getChorus()));
    m_reverbRotary->setPosition(float(instrument->getReverb()));
    m_highPassRotary->setPosition(float(instrument->getFilter()));
    m_resonanceRotary->setPosition(float(instrument->getResonance()));
    m_attackRotary->setPosition(float(instrument->getAttack()));
    m_releaseRotary->setPosition(float(instrument->getRelease()));
    */

}

void
MIDIInstrumentParameterPanel::setupControllers(MidiDevice *md)
{
    if (!m_rotaryFrame) {
	m_rotaryFrame = new QFrame(this);
	m_mainGrid->addMultiCellWidget(m_rotaryFrame, 7, 7, 0, 2, Qt::AlignHCenter);
	m_rotaryGrid = new QGridLayout(m_rotaryFrame, 10, 3, 8, 1);
	m_rotaryGrid->addItem(new QSpacerItem(10, 4), 0, 1);
    }

    // To cut down on flicker, we avoid destroying and recreating
    // widgets as far as possible here.  If a label already exists,
    // we just set its text; if a rotary exists, we only replace it
    // if we actually need a different one.

    Rosegarden::Composition &comp = m_doc->getComposition();
    Rosegarden::ControlList list = md->getControlParameters();

    // sort by IPB position
    //
    std::sort(list.begin(), list.end(),
	      Rosegarden::ControlParameter::ControlPositionCmp());

    int count = 0;
    RotaryMap::iterator rmi = m_rotaries.begin();

    for (Rosegarden::ControlList::iterator it = list.begin();
	 it != list.end(); ++it)
    {
        if (it->getIPBPosition() == -1) continue;

	// Get the knob colour - only if the colour is non-default (>0)
	//
	QColor knobColour = Qt::black; // special case for RosegardenRotary
	if (it->getColourIndex() > 0) {
	    Rosegarden::Colour c =
		comp.getGeneralColourMap().getColourByIndex
		(it->getColourIndex());
	    knobColour = QColor(c.getRed(), c.getGreen(), c.getBlue());
	}

	RosegardenRotary *rotary = 0;

	if (rmi != m_rotaries.end()) {

	    rotary = rmi->second.first;
	    int redraw = 0; // 1 -> position, 2 -> all

	    if (rotary->getMinValue() != it->getMin()) {
		rotary->setMinValue(it->getMin());
		redraw = 1;
	    }
	    if (rotary->getMaxValue() != it->getMax()) {
		rotary->setMaxValue(it->getMax());
		redraw = 1;
	    }
	    if (rotary->getKnobColour() != knobColour) {
		rotary->setKnobColour(knobColour);
		redraw = 2;
	    }
	    if (redraw == 1 || rotary->getPosition() != it->getDefault()) {
		rotary->setPosition(it->getDefault());
		if (redraw == 1) redraw = 0;
	    }
	    if (redraw == 2) {
		rotary->repaint();
	    }

	    QLabel *label = rmi->second.second;
	    label->setText(strtoqstr(it->getName()));

	    ++rmi;

	} else {

	    QHBox *hbox = new QHBox(m_rotaryFrame);
	    hbox->setSpacing(8);

	    float smallStep = 1.0;

	    float bigStep = 5.0;
	    if (it->getMax() - it->getMin() < 10) bigStep = 1.0;
	    else if (it->getMax() - it->getMin() < 20) bigStep = 2.0;

	    rotary = new RosegardenRotary
		(hbox,
		 it->getMin(),
		 it->getMax(),
		 smallStep,
		 bigStep,
		 it->getDefault(),
		 20);

	    rotary->setKnobColour(knobColour);

	    // Add a label
	    QLabel *label = new QLabel(strtoqstr(it->getName()), hbox);

	    RG_DEBUG << "Adding new widget at " << (count/2) << "," << (count%2) << endl;

	    // Add the compound widget
	    //
	    m_rotaryGrid->addWidget(hbox, count/2, (count%2) * 2, AlignLeft);
	    hbox->show();

	    // Add to list
	    //
	    m_rotaries.push_back(std::pair<int, RotaryPair>
				 (it->getControllerValue(),
				  RotaryPair(rotary, label)));

	    // Connect
	    //
	    connect(rotary, SIGNAL(valueChanged(float)),
		    m_rotaryMapper, SLOT(map()));

	    rmi = m_rotaries.end();
	}

	// Add signal mapping
	//
	m_rotaryMapper->setMapping(rotary,
				   int(it->getControllerValue()));

        count++;
    }

    if (rmi != m_rotaries.end()) {
	for (RotaryMap::iterator rmj = rmi; rmj != m_rotaries.end(); ++rmj) {
	    delete rmj->second.first;
	    delete rmj->second.second;
	}
	m_rotaries = std::vector<std::pair<int, RotaryPair> >
	    (m_rotaries.begin(), rmi);
    }

    m_rotaryFrame->show();
}


void
MIDIInstrumentParameterPanel::setRotaryToValue(int controller, int value)
{
    /*
    RG_DEBUG << "MIDIInstrumentParameterPanel::setRotaryToValue - "
             << "controller = " << controller
             << ", value = " << value << std::endl;
             */

    for (RotaryMap::iterator it = m_rotaries.begin() ; it != m_rotaries.end(); ++it)
    {
        if (it->first == controller)
        {
            it->second.first->setPosition(float(value));
            return;
        }
    }
}



void
MIDIInstrumentParameterPanel::slotSelectChannel(int index)
{
    if (m_selectedInstrument == 0)
        return;

    m_selectedInstrument->setMidiChannel(index);

    // don't use the emit - use this method instead
    Rosegarden::StudioControl::sendMappedInstrument(
            Rosegarden::MappedInstrument(m_selectedInstrument));
    emit updateAllBoxes();
}


void
MIDIInstrumentParameterPanel::populateBankList()
{
    if (m_selectedInstrument == 0)
	return;

    m_bankValue->clear();
    m_banks.clear();

    MidiDevice *md = dynamic_cast<MidiDevice*>
	(m_selectedInstrument->getDevice());
    if (!md) {
	RG_DEBUG << "WARNING: MIDIInstrumentParameterPanel::populateBankList: No MidiDevice for Instrument "
                 << m_selectedInstrument->getId() << endl;
	return;
    }

    int currentBank = -1;
    Rosegarden::BankList banks;

    RG_DEBUG << "MIDIInstrumentParameterPanel::populateBankList: variation type is " << md->getVariationType() << endl;

    if (md->getVariationType() == MidiDevice::NoVariations) {

	if (m_bankLabel->isHidden()) {
	    m_bankLabel->show();
	    m_bankCheckBox->show();
	    m_bankValue->show();
	}
	banks = md->getBanks(m_selectedInstrument->isPercussion());

	for (unsigned int i = 0; i < banks.size(); ++i) {
	    if (m_selectedInstrument->getProgram().getBank() == banks[i]) {
		currentBank = i;
	    }
	}

    } else {

	Rosegarden::MidiByteList bytes;
	bool useMSB = (md->getVariationType() == MidiDevice::VariationFromLSB);

	if (useMSB) {
	    bytes = md->getDistinctMSBs(m_selectedInstrument->isPercussion());
	} else {
	    bytes = md->getDistinctLSBs(m_selectedInstrument->isPercussion());
	}
	
	if (bytes.size() < 2) {
	    if (!m_bankLabel->isHidden()) {
		m_bankLabel->hide();
		m_bankCheckBox->hide();
		m_bankValue->hide();
	    }
	} else {
	    if (m_bankLabel->isHidden()) {
		m_bankLabel->show();
		m_bankCheckBox->show();
		m_bankValue->show();
	    }
	}

	if (useMSB) {
	    for (unsigned int i = 0; i < bytes.size(); ++i) {
		Rosegarden::BankList bl = md->getBanksByMSB
		    (m_selectedInstrument->isPercussion(), bytes[i]);
		RG_DEBUG << "MIDIInstrumentParameterPanel::populateBankList: have " << bl.size() << " variations for msb " << bytes[i] << endl;

		if (bl.size() == 0) continue;
		if (m_selectedInstrument->getMSB() == bytes[i]) {
		    currentBank = banks.size();
		}
		banks.push_back(bl[0]);
	    }
	} else {
	    for (unsigned int i = 0; i < bytes.size(); ++i) {
		Rosegarden::BankList bl = md->getBanksByLSB
		    (m_selectedInstrument->isPercussion(), bytes[i]);
		RG_DEBUG << "MIDIInstrumentParameterPanel::populateBankList: have " << bl.size() << " variations for lsb " << bytes[i] << endl;
		if (bl.size() == 0) continue;
		if (m_selectedInstrument->getLSB() == bytes[i]) {
		    currentBank = banks.size();
		}
		banks.push_back(bl[0]);
	    }
	}
    }

    for (Rosegarden::BankList::const_iterator i = banks.begin();
	 i != banks.end(); ++i) {
	m_banks.push_back(*i);
	m_bankValue->insertItem(strtoqstr(i->getName()));
    }
	    
    m_bankValue->setCurrentItem(currentBank);
    m_bankValue->setEnabled(m_selectedInstrument->sendsBankSelect());
}    
	
// Populate program list by bank context
//
void
MIDIInstrumentParameterPanel::populateProgramList()
{
    if (m_selectedInstrument == 0)
        return;

    m_programValue->clear();
    m_programs.clear();

    MidiDevice *md = dynamic_cast<MidiDevice*>
	(m_selectedInstrument->getDevice());
    if (!md) {
	RG_DEBUG << "WARNING: MIDIInstrumentParameterPanel::populateProgramList: No MidiDevice for Instrument "
                 << m_selectedInstrument->getId() << endl;
	return;
    }

    RG_DEBUG << "MIDIInstrumentParameterPanel::populateProgramList: variation type is " << md->getVariationType() << endl;

    Rosegarden::MidiBank bank(m_selectedInstrument->isPercussion(), 0, 0);

    if (m_selectedInstrument->sendsBankSelect()) {
	bank = m_selectedInstrument->getProgram().getBank();
    }

    Rosegarden::ProgramList programs = md->getPrograms(bank);
    for (unsigned int i = 0; i < programs.size(); ++i) {
	std::string programName = programs[i].getName();
	if (programName != "") {
	    m_programValue->insertItem(QString("%1. %2")
				       .arg(programs[i].getProgram() + 1)
				       .arg(strtoqstr(programName)));
	    if (m_selectedInstrument->getProgram() == programs[i]) {
		m_programValue->setCurrentItem(i);
	    }
	    m_programs.push_back(programs[i]);
	}
    }

    m_programValue->setEnabled(m_selectedInstrument->sendsProgramChange());
}

void
MIDIInstrumentParameterPanel::populateVariationList()
{
    if (m_selectedInstrument == 0)
	return;

    m_variationValue->clear();
    m_variations.clear();

    MidiDevice *md = dynamic_cast<MidiDevice*>
	(m_selectedInstrument->getDevice());
    if (!md) {
	RG_DEBUG << "WARNING: MIDIInstrumentParameterPanel::populateVariationList: No MidiDevice for Instrument "
		  << m_selectedInstrument->getId() << endl;
	return;
    }

    RG_DEBUG << "MIDIInstrumentParameterPanel::populateVariationList: variation type is " << md->getVariationType() << endl;

    if (md->getVariationType() == MidiDevice::NoVariations) {
	if (!m_variationLabel->isHidden()) {
	    m_variationLabel->hide();
	    m_variationCheckBox->hide();
	    m_variationValue->hide();
	}
	return;
    } 

    bool useMSB = (md->getVariationType() == MidiDevice::VariationFromMSB);
    Rosegarden::MidiByteList variations;

    if (useMSB) {
	Rosegarden::MidiByte lsb = m_selectedInstrument->getLSB();
	variations = md->getDistinctMSBs(m_selectedInstrument->isPercussion(),
					 lsb);
	RG_DEBUG << "MIDIInstrumentParameterPanel::populateVariationList: have " << variations.size() << " variations for lsb " << lsb << endl;

    } else {
	Rosegarden::MidiByte msb = m_selectedInstrument->getMSB();
	variations = md->getDistinctLSBs(m_selectedInstrument->isPercussion(),
					 msb);
	RG_DEBUG << "MIDIInstrumentParameterPanel::populateVariationList: have " << variations.size() << " variations for msb " << msb << endl;
    }
    
    m_variationValue->setCurrentItem(-1);

    Rosegarden::MidiProgram defaultProgram;

    if (useMSB) {
	defaultProgram = Rosegarden::MidiProgram
	    (Rosegarden::MidiBank(m_selectedInstrument->isPercussion(),
				  0,
				  m_selectedInstrument->getLSB()),
	     m_selectedInstrument->getProgramChange());
    } else {
	defaultProgram = Rosegarden::MidiProgram
	    (Rosegarden::MidiBank(m_selectedInstrument->isPercussion(),
				  m_selectedInstrument->getMSB(),
				  0),
	     m_selectedInstrument->getProgramChange());
    }
    std::string defaultProgramName = md->getProgramName(defaultProgram);

    for (unsigned int i = 0; i < variations.size(); ++i) {

	Rosegarden::MidiProgram program;

	if (useMSB) {
	    program = Rosegarden::MidiProgram
		(Rosegarden::MidiBank(m_selectedInstrument->isPercussion(),
				      variations[i],
				      m_selectedInstrument->getLSB()),
		 m_selectedInstrument->getProgramChange());
	} else {
	    program = Rosegarden::MidiProgram
		(Rosegarden::MidiBank(m_selectedInstrument->isPercussion(),
				      m_selectedInstrument->getMSB(),
				      variations[i]),
		 m_selectedInstrument->getProgramChange());
	}

	std::string programName = md->getProgramName(program);

	if (programName != "") { // yes, that is how you know whether it exists
/*
	    m_variationValue->insertItem(programName == defaultProgramName ?
					 i18n("(default)") :
					 strtoqstr(programName));
*/
	    m_variationValue->insertItem(QString("%1. %2")
					 .arg(variations[i] + 1)
					 .arg(strtoqstr(programName)));
	    if (m_selectedInstrument->getProgram() == program) {
		m_variationValue->setCurrentItem(i);
	    }
	    m_variations.push_back(variations[i]);
	}
    }

    if (m_variations.size() < 2) {
	if (!m_variationLabel->isHidden()) {
	    m_variationLabel->hide();
	    m_variationCheckBox->hide();
	    m_variationValue->hide();
	}
	
    } else {
	//!!! seem to have problems here -- the grid layout doesn't
	//like us adding stuff in the middle so if we go from 1
	//visible row (say program) to 2 (program + variation) the
	//second one overlaps the control knobs

	if (m_variationLabel->isHidden()) {
	    m_variationLabel->show();
	    m_variationCheckBox->show();
	    m_variationValue->show();
	}

	if (m_programValue->width() > m_variationValue->width()) {
	    m_variationValue->setMinimumWidth(m_programValue->width());
	} else {
	    m_programValue->setMinimumWidth(m_variationValue->width());
	}
    }

    m_variationValue->setEnabled(m_selectedInstrument->sendsBankSelect());
}


void
MIDIInstrumentParameterPanel::slotTogglePercussion(bool value)
{
    if (m_selectedInstrument == 0)
    {
	m_percussionCheckBox->setChecked(false);
        emit updateAllBoxes();
        return;
    }

    m_selectedInstrument->setPercussion(value);

    populateBankList();
    populateProgramList();
    populateVariationList();

    sendBankAndProgram();

    emit changeInstrumentLabel(m_selectedInstrument->getId(),
			       strtoqstr(m_selectedInstrument->
					 getProgramName()));
    emit updateAllBoxes();
}


void
MIDIInstrumentParameterPanel::slotToggleBank(bool value)
{
    if (m_selectedInstrument == 0)
    {
        m_bankCheckBox->setChecked(false);
        emit updateAllBoxes();
        return;
    }

    m_variationCheckBox->setChecked(value);
    m_selectedInstrument->setSendBankSelect(value);

    m_bankValue->setDisabled(!value);
    populateBankList();
    populateProgramList();
    populateVariationList();

    sendBankAndProgram();

    emit changeInstrumentLabel(m_selectedInstrument->getId(),
			       strtoqstr(m_selectedInstrument->
					 getProgramName()));
    emit updateAllBoxes();
}

void
MIDIInstrumentParameterPanel::slotToggleProgramChange(bool value)
{
    if (m_selectedInstrument == 0)
    {
        m_programCheckBox->setChecked(false);
        emit updateAllBoxes();
        return;
    }

    m_selectedInstrument->setSendProgramChange(value);

    m_programValue->setDisabled(!value);
    populateProgramList();
    populateVariationList();

    if (value) sendBankAndProgram();

    emit changeInstrumentLabel(m_selectedInstrument->getId(),
			       strtoqstr(m_selectedInstrument->
					 getProgramName()));
    emit updateAllBoxes();
}

void
MIDIInstrumentParameterPanel::slotToggleVariation(bool value)
{
    if (m_selectedInstrument == 0)
    {
        m_variationCheckBox->setChecked(false);
        emit updateAllBoxes();
        return;
    }

    m_bankCheckBox->setChecked(value);
    m_selectedInstrument->setSendBankSelect(value);

    m_variationValue->setDisabled(!value);
    populateVariationList();

    sendBankAndProgram();

    emit changeInstrumentLabel(m_selectedInstrument->getId(),
			       strtoqstr(m_selectedInstrument->
					 getProgramName()));
    emit updateAllBoxes();
}


void
MIDIInstrumentParameterPanel::slotSelectBank(int index)
{
    if (m_selectedInstrument == 0)
        return;

    MidiDevice *md = dynamic_cast<MidiDevice*>
	(m_selectedInstrument->getDevice());
    if (!md) {
	RG_DEBUG << "WARNING: MIDIInstrumentParameterPanel::slotSelectBank: No MidiDevice for Instrument "
		  << m_selectedInstrument->getId() << endl;
	return;
    }

    const Rosegarden::MidiBank *bank = &m_banks[index];

    if (md->getVariationType() != MidiDevice::VariationFromLSB) {
	m_selectedInstrument->setLSB(bank->getLSB());
    }
    if (md->getVariationType() != MidiDevice::VariationFromMSB) {
	m_selectedInstrument->setMSB(bank->getMSB());
    }

    populateProgramList();

    sendBankAndProgram();

    emit updateAllBoxes();
}

void
MIDIInstrumentParameterPanel::slotSelectProgram(int index)
{
    const Rosegarden::MidiProgram *prg = &m_programs[index];
    if (prg == 0) {
        RG_DEBUG << "program change not found in bank" << endl;
        return;
    }
    m_selectedInstrument->setProgramChange(prg->getProgram());

    sendBankAndProgram();

    populateVariationList();

    emit changeInstrumentLabel(m_selectedInstrument->getId(),
			       strtoqstr(m_selectedInstrument->
					 getProgramName()));
    emit updateAllBoxes();
}

void
MIDIInstrumentParameterPanel::slotSelectVariation(int index)
{
    MidiDevice *md = dynamic_cast<MidiDevice*>
	(m_selectedInstrument->getDevice());
    if (!md) {
	RG_DEBUG << "WARNING: MIDIInstrumentParameterPanel::slotSelectVariation: No MidiDevice for Instrument "
		  << m_selectedInstrument->getId() << endl;
	return;
    }

    if (index < 0 || index > int(m_variations.size())) {
	RG_DEBUG << "WARNING: MIDIInstrumentParameterPanel::slotSelectVariation: index " << index << " out of range" << endl;
	return;
    }

    Rosegarden::MidiByte v = m_variations[index];
    
    if (md->getVariationType() == MidiDevice::VariationFromLSB) {
	m_selectedInstrument->setLSB(v);
    } else if (md->getVariationType() == MidiDevice::VariationFromMSB) {
	m_selectedInstrument->setMSB(v);
    }

    sendBankAndProgram();
}

void
MIDIInstrumentParameterPanel::sendBankAndProgram()
{
    if (m_selectedInstrument == 0)
        return;

    MidiDevice *md = dynamic_cast<MidiDevice*>
	(m_selectedInstrument->getDevice());
    if (!md) {
	RG_DEBUG << "WARNING: MIDIInstrumentParameterPanel::slotSelectBank: No MidiDevice for Instrument "
		  << m_selectedInstrument->getId() << endl;
	return;
    }

    if (m_selectedInstrument->sendsBankSelect()) {

        // Send the bank select message before any PC message
        //
        Rosegarden::MappedEvent mEMSB(m_selectedInstrument->getId(), 
                                      Rosegarden::MappedEvent::MidiController,
                                      Rosegarden::MIDI_CONTROLLER_BANK_MSB,
                                      m_selectedInstrument->getMSB());
        Rosegarden::StudioControl::sendMappedEvent(mEMSB);

        Rosegarden::MappedEvent mELSB(m_selectedInstrument->getId(), 
                                      Rosegarden::MappedEvent::MidiController,
                                      Rosegarden::MIDI_CONTROLLER_BANK_LSB,
                                      m_selectedInstrument->getLSB());
        Rosegarden::StudioControl::sendMappedEvent(mELSB);
    }

    Rosegarden::MappedEvent mE(m_selectedInstrument->getId(), 
                               Rosegarden::MappedEvent::MidiProgramChange,
                               m_selectedInstrument->getProgramChange(),
                               (Rosegarden::MidiByte)0);

    // Send the controller change
    //
    Rosegarden::StudioControl::sendMappedEvent(mE);
}


void
MIDIInstrumentParameterPanel::slotControllerChanged(int controllerNumber)
{
    /*
     RG_DEBUG<< "MIDIInstrumentParameterPanel::slotControllerChanged - controller = "
             << controllerNumber << "\n";
             */

    if (m_selectedInstrument == 0)
        return;

    MidiDevice *md = dynamic_cast<MidiDevice*>
	(m_selectedInstrument->getDevice());
    if (!md) return;

    Rosegarden::ControlParameter *controller = 
	md->getControlParameter(Rosegarden::MidiByte(controllerNumber));

    int value = getValueFromRotary(controllerNumber);

    if (value == -1)
    {
        RG_DEBUG << "MIDIInstrumentParameterPanel::slotControllerChanged - "
                 << "couldn't get value of rotary for controller " << controllerNumber << "\n";
        return;
    }


    // two special cases
    if (controllerNumber == int(Rosegarden::MIDI_CONTROLLER_PAN))
    {
        float adjValue = value;
        if (m_selectedInstrument->getType() == Instrument::Audio)
            value += 100;

        m_selectedInstrument->setPan(Rosegarden::MidiByte(adjValue));
    }
    else if (controllerNumber == int(Rosegarden::MIDI_CONTROLLER_VOLUME))
    {
        m_selectedInstrument->setVolume(Rosegarden::MidiByte(value));
    }
    else if (controller)
    {
        m_selectedInstrument->setControllerValue(Rosegarden::MidiByte(controllerNumber),
                                                 Rosegarden::MidiByte(value));

        //RG_DEBUG << "SET CONTROLLER VALUE (" << controllerNumber << ") = " << value << std::endl;
    }
    else
    {
        RG_DEBUG << "MIDIInstrumentParameterPanel::slotControllerChanged - "
                 << "no controller retrieved\n";
        return;
    }

    Rosegarden::MappedEvent mE(m_selectedInstrument->getId(), 
                               Rosegarden::MappedEvent::MidiController,
                               (Rosegarden::MidiByte)controllerNumber,
                               (Rosegarden::MidiByte)value);
    Rosegarden::StudioControl::sendMappedEvent(mE);

    emit updateAllBoxes();
    
}

int
MIDIInstrumentParameterPanel::getValueFromRotary(int rotary)
{
    for (RotaryMap::iterator it = m_rotaries.begin(); it != m_rotaries.end(); ++it)
    {
        if (it->first == rotary)
            return int(it->second.first->getPosition());
    }

    return -1;
}

