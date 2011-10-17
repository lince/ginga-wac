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

#include "../include/LinksPlayer.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	LinksPlayer::LinksPlayer(
			string mrl, int x, int y, int w, int h) : Player(mrl) {

		void* s;

		mBrowser = NULL;
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;

		setBrowserDFB(OutputManager::getInstance()->getGraphicsRoot());
		setDisplayMenu(0);
		mBrowser = openBrowser(x, y, w, h);
		while (mBrowser == NULL) {
			mBrowser = openBrowser(x, y, w, h);
		}

		loadUrlOn(mBrowser, mrl.c_str());
		s = browserGetSurface(mBrowser);
		if (s != NULL) {
			this->surface = new Surface(s);
		}
		hasBrowser = true;
	}

	LinksPlayer::~LinksPlayer() {
		Window* parent;
		cout << "LinksPlayer::~LinksPlayer()" << endl;
		if (hasBrowser) {
			closeBrowser(mBrowser);
			mBrowser = NULL;
		}
	}

	void LinksPlayer::setBounds(int x, int y, int w, int h) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	void LinksPlayer::updateBounds(int x, int y, int w, int h) {
		Window* parent;

//		closeBrowser(mBrowser);
//		mBrowser = NULL;
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;

//		mBrowser = openBrowser(x, y, w, h);
		browserResizeCoord(mBrowser, x, y, w, h);
		//browserRequestFocus(mBrowser);
		/*parent = (Window*)(surface->getParent());
		if (parent != NULL) {
			cout << " UPDATE RENDER" << endl;
			parent->renderFrom(surface);
			parent->validate();

		} else {
			cout << " UPDATE PARENT IS NULL" << endl;
		}*/
	}

	void LinksPlayer::play() {
		browserShow(mBrowser);
		//browserResizeCoord(mBrowser, x, y, w, h);
		//loadUrlOn(mBrowser, mrl.c_str());
		//browserRequestFocus(mBrowser);

		Thread::start();
		//::usleep(3000000);

		Player::play();
	}

	void LinksPlayer::stop() {
		Window* parent;

		if (hasBrowser) {
			closeBrowser(mBrowser);
			mBrowser = NULL;
			hasBrowser = false;
		}
		Player::stop();
	}

	void LinksPlayer::setPropertyValue(
			string name, string value, double duration, double by) {

		//TODO: set scrollbar, support...
		if (name == "transparency") {
			double val;

			val = stof(value);
			if (val >= 0.0 && val <= 1.0) {
				browserSetAlpha((int)(val * 0xFF), mBrowser);
			}
		}

		Player::setPropertyValue(name, value, duration, by);
	}

	void LinksPlayer::setFocusHandler(bool isHandler) {
		browserSetFocusHandler((int)isHandler, mBrowser);
	}

	void LinksPlayer::run() {
		Window* parent;

		::usleep(500000);
		if (surface != NULL) {
			parent = (Window*)(surface->getParent());
			if (parent != NULL) {
				if (surface->setParent((void*)parent)) {
					parent->renderFrom(surface);
				}
			}
		}
		::usleep(1500000);
		if (surface != NULL) {
			parent = (Window*)(surface->getParent());
			if (parent != NULL) {
				if (surface->setParent((void*)parent)) {
					parent->renderFrom(surface);
				}
			}
		}
	}
}
}
}
}
}
}
