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

#include "../../include/io/DeviceManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
int DeviceManager::numArgs = 0;
char** DeviceManager::args = NULL;

	DeviceManager::DeviceManager() {
		devices  = new map<unsigned int, IODevice*>;
		profiles = new map<unsigned int, string>;

		cm = IComponentManager::getCMInstance();
	}

	DeviceManager::~DeviceManager() {
		if (devices != NULL) {
			delete devices;
			devices = NULL;
		}

		if (profiles != NULL) {
			delete profiles;
			profiles = NULL;
		}
	}

	DeviceManager* DeviceManager::_instance = NULL;

	void DeviceManager::release() {
		if (_instance != NULL) {
			delete _instance;
			_instance = NULL;
		}
		//TODO: delete devices
	}

	void DeviceManager::setParameters(int numArgs, char* args[]) {
		DeviceManager::numArgs = numArgs;
		DeviceManager::args = args;
	}

	DeviceManager* DeviceManager::getInstance() {
		if (DeviceManager::_instance == NULL) {
			DeviceManager::_instance = new DeviceManager();
		}
		return DeviceManager::_instance;
	}

	int DeviceManager::getDeviceWidth(
			unsigned int deviceNumber, unsigned int screenNumber) {

		IODevice* dev;

		if (devices->count(deviceNumber) == 0) {
			return 0;
		}

		dev = (*devices)[deviceNumber];
		return dev->getScreenWidthRes(screenNumber);
	}

	int DeviceManager::getDeviceHeight(
			unsigned int deviceNumber, unsigned int screenNumber) {

		IODevice* dev;

		if (devices->count(deviceNumber) == 0) {
			return 0;
		}

		dev = (*devices)[deviceNumber];
		return dev->getScreenHeightRes(screenNumber);
	}

	void* DeviceManager::getGfxRoot(unsigned int deviceNumber) {
		IODevice* dev;

		if (devices->count(deviceNumber) == 0) {
			return 0;
		}

		dev = (*devices)[deviceNumber];
		return dev->getGfxRoot();
	}

	void DeviceManager::clearWidgetPools() {
		//TODO: delete pools in screen devices
		map<unsigned int, IODevice*>::iterator i;

		if (devices != NULL) {
			i = devices->begin();
			while (i != devices->end()) {
				delete i->second;
				++i;
			}

			devices->clear();
		}
	}

	int DeviceManager::createDevice(string description) {
		unsigned int deviceNumber;
		IODevice* dev;
		//DeviceAudio* aud;
		//DeviceCommunication* com;
		IDeviceScreen* scr = NULL;

		if (description == "default") {
			deviceNumber = devices->size();
			dev = new IODevice();
			(*devices)[deviceNumber] = dev;
			(*profiles)[deviceNumber] = description;

			if (cm != NULL) {
				scr = ((ScreenCreator*)(cm->getObject(
						"DeviceScreen")))(0, NULL);
			}

			if (scr != NULL) {
				dev->addScreen(scr);
			}

			return (int)deviceNumber;
		}
		return -1;
	}

	void* DeviceManager::createWindow(
			void* winDesc,
			unsigned int deviceNumber, unsigned int screenNumber) {

		IODevice* dev;

		if (devices->count(deviceNumber) == 0) {
			return NULL;
		}

		dev = (*devices)[deviceNumber];
		return dev->createWindow(winDesc, screenNumber);
	}

	void DeviceManager::releaseWindow(
			void* win, unsigned int deviceNumber, unsigned int screenNumber) {

		IODevice* dev;

		if (devices->count(deviceNumber) == 0) {
			return;
		}

		dev = (*devices)[deviceNumber];
		return dev->releaseWindow(win, screenNumber);
	}

	void* DeviceManager::createSurface(
			void* surfaceDesc,
			unsigned int deviceNumber, unsigned int screenNumber) {

		IODevice* dev;

		if (devices->count(deviceNumber) == 0) {
			return NULL;
		}

		dev = (*devices)[deviceNumber];
		return dev->createSurface(surfaceDesc, screenNumber);
	}

	void DeviceManager::releaseSurface(
			void* sur,
			unsigned int deviceNumber, unsigned int screenNumber) {

		IODevice* dev;

		if (devices->count(deviceNumber) == 0) {
			return;
		}

		dev = (*devices)[deviceNumber];
		return dev->releaseSurface(sur, screenNumber);
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::
		IDeviceManager* createDeviceManager() {

	return ::br::pucrio::telemidia::ginga::core::system::io::
			DeviceManager::getInstance();
}

extern "C" void destroyDeviceManager(::br::pucrio::telemidia::ginga::core::
		system::io::IDeviceManager* dm) {

	dm->release();
}
