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

#include "../include/ImagePlayer.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	ImagePlayer::ImagePlayer(string mrl) : Player(mrl) {
		string path, name, clientPath, newMrl;

		if (fileExists(mrl)) {
			provider = new ImageProvider((char*)(mrl.c_str()));

		} else {
			if (mrl.find("/") != std::string::npos) {
				path = mrl.substr(0, mrl.find_last_of("/"));
				name = mrl.substr(
						mrl.find_last_of("/") + 1,
						mrl.length() - mrl.find_last_of("/"));

			} else if (mrl.find("\\") != std::string::npos) {
				path = mrl.substr(0, mrl.find_last_of("\\"));
				name = mrl.substr(
						mrl.find_last_of("\\") + 1,
						mrl.length() - mrl.find_last_of("\\"));
			}

			cout << "ImagePlayer trying to find '" << mrl << "'" << endl;
			clientPath = GingaLocatorFactory::getInstance()->getLocation(path);
			newMrl = clientPath + name;
			cout << "ImagePlayer found newMrl = '" << newMrl << "'" << endl;

			if (fileExists(newMrl)) {
				provider = new ImageProvider((char*)(newMrl.c_str()));

			} else {
				provider = NULL;
				cout << "ImagePlayer::ImagePlayer Warning! File not Found: '";
				cout << newMrl.c_str() << "'" << endl;
			}
		}

		if (provider != NULL) {
			surface = ImagePlayer::prepareSurface(provider, mrl);
		}
	}

	ImagePlayer::~ImagePlayer() {
		if (provider != NULL) {
			delete provider;
			provider = NULL;
		}
	}

	void ImagePlayer::play() {
		if (provider == NULL ||
				surface == NULL || surface->getContent() == NULL) {

			return;
		}

		provider->playOver(surface);
		Player::play();
	}

	void ImagePlayer::stop() {
		if (provider == NULL) {
			return;
		}

		delete provider;
		provider = NULL;
		Player::stop();
	}

	void ImagePlayer::resume() {
		ImagePlayer::play();
	}

	void ImagePlayer::setPropertyValue(
			string name, string value, double duration, double by) {

		//TODO: set brightness, rotate...
		//refresh changes
		Window* win;
		win = (Window*)(surface->getParent());
		if (win != NULL) {
			win->renderFrom(surface);
		}

		Player::setPropertyValue(name, value, duration, by);
	}

	Surface* ImagePlayer::prepareSurface(ImageProvider* provider, string mrl) {
		Surface* renderedSurface = NULL;

		if (mrl.length() > 4 && upperCase(
				mrl.substr(mrl.length() - 4, mrl.length())) == ".GIF") {

			renderedSurface = provider->prepare(true);

		} else {
			renderedSurface = provider->prepare(false);
		}

		return renderedSurface;
	}

	Surface* ImagePlayer::renderImage(string mrl) {
		ImageProvider* imgProvider = NULL;
		Surface* renderedSurface = NULL;

		if (fileExists(mrl)) {
			imgProvider = new ImageProvider((char*)(mrl.c_str()));
		}

		if (imgProvider != NULL) {
			renderedSurface = ImagePlayer::prepareSurface(imgProvider, mrl);
			delete imgProvider;
		}

		return renderedSurface;
	}
}
}
}
}
}
}
