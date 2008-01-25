/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
 
    This program is Copyright 2000-2008
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <richard.bown@ferventsoftware.com>
 
    This file is Copyright 2005
        Toni Arnold         <toni__arnold@bluewin.ch>

    The moral rights of Guillaume Laurent, Chris Cannam, and Richard
    Bown to claim authorship of this work have been asserted.
 
    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/


#include "LircClient.h"

#ifdef HAVE_LIRC

#include "misc/Debug.h"
#include "base/Exception.h"
#include <qobject.h>
#include <qsocketnotifier.h>
#include <fcntl.h>
#include <cstdlib>

namespace Rosegarden
{

LircClient::LircClient(void)
        : QObject()
{
    int socketFlags;

    // socket setup with nonblock
    m_socket = lirc_init("rosegarden", 1);
    if (m_socket == -1) {
        throw Exception("Failed to connect to LIRC");
    }

    if (lirc_readconfig(NULL, &m_config, NULL) == -1) {
        throw Exception("Failed reading LIRC config file");
    }

    fcntl(m_socket, F_GETOWN, getpid());
    socketFlags = fcntl(m_socket, F_GETFL, 0);
    if (socketFlags != -1) {
        fcntl(socketFlags, F_SETFL, socketFlags | O_NONBLOCK);
    }

    m_socketNotifier = new QSocketNotifier(m_socket, QSocketNotifier::Read, 0);
    connect(m_socketNotifier, SIGNAL(activated(int)), this, SLOT(readButton()) );

    RG_DEBUG << "LircClient::LircClient: connected to socket: " << m_socket << endl;
}

LircClient::~LircClient()
{
    lirc_freeconfig(m_config);
    delete m_socketNotifier;
    lirc_deinit();

    RG_DEBUG << "LircClient::~LircClient: cleaned up" << endl;
}

void LircClient::readButton()
{
    char *code;
    int ret;

    RG_DEBUG << "LircClient::readButton" << endl;

    if (lirc_nextcode(&code) == 0 && code != NULL) {   // no error && a string is available
        // handle any command attached to that button
        while ( (ret = lirc_code2char(m_config, code, &m_command)) == 0 && m_command != NULL ) 
        {
            emit buttonPressed(m_command);
        }
        free(code);
    }
}

}

#include "LircClient.moc"

#endif
