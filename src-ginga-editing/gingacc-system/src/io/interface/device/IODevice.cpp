/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "../../../../include/io/interface/device/IODevice.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	IODevice::IODevice() {
		audioDevices = new map<unsigned int, IDeviceAudio*>;
		screenDevices = new map<unsigned int, IDeviceScreen*>;
		connectorDevices = new map<unsigned int, IDeviceCommunication*>;
	}

	IODevice::~IODevice() {
		map<unsigned int, IDeviceScreen*>::iterator i;

		if (audioDevices != NULL) {
			delete audioDevices;
		}

		if (screenDevices != NULL) {
			i = screenDevices->begin();
			while (i != screenDevices->end()) {
				delete i->second;
				++i;
			}
			delete screenDevices;
		}

		if (connectorDevices != NULL) {
			delete connectorDevices;
		}
	}

	unsigned int IODevice::addAudio(IDeviceAudio* audioResource) {
		unsigned int i = audioDevices->size();
		(*audioDevices)[i] = audioResource;
		return i;
	}

	unsigned int IODevice::addScreen(IDeviceScreen* screenResource) {
		unsigned int i = screenDevices->size();
		(*screenDevices)[i] = screenResource;
		return i;
	}

	unsigned int IODevice::addChannel(IDeviceCommunication* channelResource) {
		unsigned int i = connectorDevices->size();
		(*connectorDevices)[i] = channelResource;
		return i;
	}

	unsigned int IODevice::getScreenWidthRes(unsigned int screenNumber) {
		IDeviceScreen* scr;

		if (screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			return scr->getWidthResolution();
		}
		return 0;
	}

	unsigned int IODevice::getScreenHeightRes(unsigned int screenNumber) {
		IDeviceScreen* scr;

		if (screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			return scr->getHeightResolution();
		}
		return 0;
	}

	void* IODevice::getGfxRoot() {
		IDeviceScreen* scr;

		if (screenDevices->size() > 0) {
			scr = screenDevices->begin()->second;
			return scr->getGfxRoot();
		}

		return NULL;
	}

	void* IODevice::createWindow(void* winDesc, unsigned int screenNumber) {
		IDeviceScreen* scr;

		if (screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			return scr->createWindow(winDesc);
		}
		return NULL;
	}

	void IODevice::releaseWindow(void* win, unsigned int screenNumber) {
		IDeviceScreen* scr;

		if (screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			scr->releaseWindow(win);
		}
	}

	void* IODevice::createSurface(void* surDesc, unsigned int screenNumber) {
		IDeviceScreen* scr;

		if (screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			return scr->createSurface(surDesc);
		}
		return NULL;
	}

	void IODevice::releaseSurface(void* sur, unsigned int screenNumber) {
		IDeviceScreen* scr;

		if (screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			scr->releaseSurface(sur);
		}
	}
}
}
}
}
}
}
}
