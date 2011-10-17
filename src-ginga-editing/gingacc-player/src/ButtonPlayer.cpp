/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware 
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob 
os termos da Licen�a Publica Geral GNU versao 2 conforme publicada pela Free 
Software Foundation.

Este programa eh distribu�do na expectativa de que seja util, porem, SEM 
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

#include "../include/ButtonPlayer.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	ButtonPlayer::ButtonPlayer() : Thread(), Player("") {
		status = NONE;
		previousStatus = NONE;
		image = NULL;

		win = NULL;
		initializeWindow();
	}

	ButtonPlayer* ButtonPlayer::_instance = 0;

	ButtonPlayer* ButtonPlayer::getInstance() {
		if (ButtonPlayer::_instance == NULL) {
			ButtonPlayer::_instance = new ButtonPlayer();
		}
		return ButtonPlayer::_instance;
	}

	void ButtonPlayer::initializeWindow() {
/*		win = new Window(
			    (int)(0.92 * GfxManager::getInstance()->getDeviceWidth()),
			    (int)(0.05 * GfxManager::getInstance()->getDeviceHeight()),
			    (int)(0.04 * GfxManager::getInstance()->getDeviceWidth()),
			    (int)(0.05 * GfxManager::getInstance()->getDeviceHeight()));
*/
		win = new Window(
			    (int)(OutputManager::getInstance()->getDeviceWidth() - 70),
			    (int)(10),
			    (int)(60),
			    (int)(60));

		win->setCaps(Window::CAPS_ALPHACHANNEL);
		win->draw();
		/*win->setBackgroundColor(0, 0, 0);
		win->setColorKey(0, 0, 0);
		win->setTransparencyValue((int)(0.5*0xFF));*/
	}

	void ButtonPlayer::play() {
		lock();
		if (status != PLAY && status != NONE) {
			Player::play();
			previousStatus = status;
			Thread::start();

		} else {
			Player::play();
		}
		unlock();
	}

	void ButtonPlayer::stop() {
		lock();
		if (status != STOP) {
			previousStatus = status;
			Player::stop();
			Thread::start();
		}
		unlock();
	}

	void ButtonPlayer::pause() {
		lock();
		if (status == PAUSE) {
			previousStatus = status;
			Player::resume();
			Thread::start();

		} else {
			previousStatus = status;
			Player::pause();
			Thread::start();
		}
		unlock();
	}

	void ButtonPlayer::resume() {
		pause();
	}

	void ButtonPlayer::setPropertyValue(
			string name, string value, double duration, double by) {

		//TODO: set button size, type, urls...
		Player::setPropertyValue(name, value, duration, by);
	}

	void ButtonPlayer::release() {
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

	void ButtonPlayer::render(string mrl) {
		Surface* surface;
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

	void ButtonPlayer::run() {
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
				render("/misc/pauseButton.png");
				break;

			case STOP:
				if (previousStatus == PAUSE) {
					win->hide();
				}

				render("/misc/stopButton.png");
				::usleep(1000000);
				win->hide();
				break;

			case PLAY:
				if (previousStatus == PAUSE) {
					win->hide();
				}

				render("/misc/playButton.png");
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
