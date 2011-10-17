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

#include "../../include/ShowButton.h"

#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	ShowButton::ShowButton() : Thread() {
		status = NONE;
		previousStatus = NONE;
		image = NULL;

		win = NULL;
		initializeWindow();
	}

	ShowButton* ShowButton::_instance = 0;

	ShowButton* ShowButton::getInstance() {
		if (ShowButton::_instance == NULL) {
			ShowButton::_instance = new ShowButton();
		}
		return ShowButton::_instance;
	}

	void ShowButton::initializeWindow() {
		int x, y, w, h;
		IComponentManager* cm = IComponentManager::getCMInstance();
		IDeviceManager* dm = NULL;

		if (cm == NULL) {
			return;
		}

		dm = ((DeviceManagerCreator*)(cm->getObject("DeviceManager")))();

/*		win = new Window(
			    (int)(0.92 * GfxManager::getInstance()->getDeviceWidth()),
			    (int)(0.05 * GfxManager::getInstance()->getDeviceHeight()),
			    (int)(0.04 * GfxManager::getInstance()->getDeviceWidth()),
			    (int)(0.05 * GfxManager::getInstance()->getDeviceHeight()));
*/

		x = 0;

		if (dm != NULL) {
			x = (int)(dm->getDeviceWidth() - 70);
		}

		y = 10;
		w = 60;
		h = 60;

		win = ((WindowCreator*)(cm->getObject("Window")))(x, y, w, h);
		if (win != NULL) {
			win->setCaps(win->getCap("ALPHACHANNEL"));
			win->draw();
		}
		/*win->setBackgroundColor(0, 0, 0);
		win->setColorKey(0, 0, 0);
		win->setTransparencyValue((int)(0.5*0xFF));*/
	}

	void ShowButton::play() {
		lock();
		if (status != PLAY && status != NONE) {
			previousStatus = status;
			status = PLAY;
			Thread::start();

		} else {
			status = PLAY;
		}
		unlock();
	}

	void ShowButton::stop() {
		lock();
		if (status != STOP) {
			previousStatus = status;
			status = STOP;
			Thread::start();
		}
		unlock();
	}

	void ShowButton::pause() {
		lock();
		if (status == PAUSE) {
			previousStatus = status;
			status = PAUSE;
			Thread::start();

		} else {
			previousStatus = status;
			status = PLAY;
			Thread::start();
		}
		unlock();
	}

	void ShowButton::resume() {
		pause();
	}

	void ShowButton::release() {
		lock();
		if (image != NULL) {
			delete image;
			image = NULL;
		}

		if (win != NULL) {
			delete win;
			win = NULL;
		}
		unlock();
	}

	void ShowButton::render(string mrl) {
		ISurface* surface;
		surface = ImagePlayer::renderImage(mrl);
		if (win == NULL) {
			return;
		}

		if (surface->setParent((void*)win)) {
			win->renderFrom(surface);
		}
		win->show();
		win->raiseToTop();
		delete surface;
		surface = NULL;
	}

	void ShowButton::run() {
		lock();
		if (win == NULL) {
			initializeWindow();
		}

		if (image != NULL) {
			delete image;
			image = NULL;
		}

		switch (status) {
			case PAUSE:
				render("/usr/local/lib/ginga/files/img/button/pauseButton.png");
				break;

			case STOP:
				if (previousStatus == PAUSE) {
					win->hide();
				}

				render("/usr/local/lib/ginga/files/img/button/stopButton.png");
				::usleep(1000000);
				win->hide();
				break;

			case PLAY:
				if (previousStatus == PAUSE) {
					win->hide();
				}

				render("/usr/local/lib/ginga/files/img/button/playButton.png");
				::usleep(1000000);
				win->hide();
				break;

			default:
				break;
		}
		unlock();
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IShowButton*
		createShowButton() {

	br::pucrio::telemidia::ginga::core::player::ShowButton* sb;
	sb = br::pucrio::telemidia::ginga::core::player::ShowButton::getInstance();
	return sb;
}

extern "C" void destroyShowButton(
		::br::pucrio::telemidia::ginga::core::player::IShowButton* sb) {

	return delete sb;
}
