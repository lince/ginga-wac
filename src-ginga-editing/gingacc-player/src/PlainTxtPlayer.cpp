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

#include "../include/PlainTxtPlayer.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	PlainTxtPlayer::PlainTxtPlayer() : TextPlayer() {
		pthread_mutex_init(&mutex, NULL);
		content = "";
	}

	PlainTxtPlayer::~PlainTxtPlayer() {
		pthread_mutex_lock(&mutex);
		if (surface != NULL) {
			/*
			 * the surface could never be a child of window
			 * (it gets the widget surface)
			 */
			surface->setParent(NULL);
		}
		pthread_mutex_unlock(&mutex);
		pthread_mutex_destroy(&mutex);
   	}

	void PlainTxtPlayer::setFile(string mrl) {
		if (mrl == "" || !fileExists(mrl)) {
			cout << "PlainTxtPlayer::setFile Warning! File not found: '";
			cout << mrl << "'" << endl; 
			return;
		}

		if (mrl.length() > 4) {
			string fileType;

			this->mrl = mrl;
			fileType = upperCase(this->mrl.substr(this->mrl.length() - 4, 4));
			if (fileType != ".TXT") {
				cout << "PlainTxtPlayer::loadFile Warning! Unknown file ";
				cout << "type for: '" << this->mrl << "'" << endl;
			}

		} else {
			cout << "PlainTxtPlayer::loadFile Warning! Unknown extension ";
			cout << "type for: '" << mrl << "'" << endl;
		}
	}

	void PlainTxtPlayer::loadTxt() {
		ifstream fis;
		string line, aux;
		int surfaceW, surfaceH;

		if (isXmlStr(mrl)) {
			mrl = ("/usr/local/lib/ginga/epgfactory/src/" + 
					mrl.substr(mrl.find_last_of("/"), mrl.length()));
		}

		pthread_mutex_lock(&mutex);
		fis.open((this->mrl).c_str(), ifstream::in);
		if (!fis.is_open() && (mrl != "" || content == "")) {
			cout << "PlainTxtPlayer::loadFile Warning! can't open input ";
			cout << "file: '" << this->mrl << "'" << endl;
			pthread_mutex_unlock(&mutex);
			return;
		}

		if (fontColor == NULL) {
			fontColor = new Color(255, 255, 255, 255);
		}

		//TODO: to decide if the default bgcolor is transparent or black
		if (bgColor == NULL) {
			bgColor = new Color(0, 0, 0, 255);
		}

		if (surface != NULL && surface->getParent() != NULL) {
			surface->clearSurface();
			surface->clear();
			if (bgColor != NULL) {
				surface->setBgColor(bgColor);
			}
		}

		this->currentLine = 0;
		this->currentColumn = 0;

		if (mrl != "" && content == "") {
			surface->getSize(&surfaceW, &surfaceH);
			while (!fis.eof() && fis.good() &&
					(currentLine + fontHeight) < surfaceH) {

				getline(fis, line);
				drawTextLn(line);
			}

		} else if (content != "") {
			drawTextLn(content);
		}

		if (surface != NULL && surface->getParent() != NULL) {
			((Window*)(surface->getParent()))->validate();
		}

		fis.close();
		pthread_mutex_unlock(&mutex);
	}

	void PlainTxtPlayer::play() {
		Player::play();
		if (surface != NULL && surface->getContent() != NULL) {
			wclog << "PlainTxtPlayer::play ok" << endl;
			loadTxt();

		} else {
			wclog << "PlainTxtPlayer::play warning" << endl;
		}
	}

	Surface* PlainTxtPlayer::getSurface() {
		/*if (surface != NULL && surface->getSurface() != NULL) {
			loadTxt();
		}*/
		return surface;
	}

	void PlainTxtPlayer::setContent(string content) {
		pthread_mutex_lock(&mutex);

		if (surface != NULL && surface->getParent() != NULL) {
			surface->clearSurface();
			surface->clear();

			if (bgColor != NULL) {
				surface->setBgColor(bgColor);
			}
		}

		this->currentLine = 0;
		this->currentColumn = 0;
		this->content = content;

		if (content != "") {
			drawTextLn(this->content);
			mrl = "";
		}

		if (surface != NULL && surface->getParent() != NULL) {
			((Window*)(surface->getParent()))->validate();
		}

		pthread_mutex_unlock(&mutex);
	}

	void PlainTxtPlayer::setPropertyValue(
			string name, string value, double duration, double by) {

		//wclog << "PlainTxtPlayer::setPropertyValue name = '" << name.c_str();
		//wclog << "' value = '" << value.c_str() << "'" << endl;
		vector<string>* params;

		if (name == "x-fontColor" && value != "") {
			if (fontColor != NULL) {
				delete fontColor;
				fontColor = NULL;
			}

			fontColor = new Color(value);

			// refreshing changes
			loadTxt();

		} else if (name == "x-fontSize" && value != "" &&
				isNumeric((void*)(value.c_str()))) {

			setFontSize((int)(stof(value)));

			// refreshing changes
			loadTxt();

		} else if (name == "x-fontUri" && value != "") {
			setFont(value);

			// refreshing changes
			loadTxt();

		} else if (name == "x-bgColor" && value != "") {
			if (surface != NULL) {
				if (bgColor != NULL) {
					delete bgColor;
					bgColor = NULL;
				}

				bgColor = new Color(value);
				surface->setBgColor(bgColor);

				// refreshing changes
				loadTxt();
			}

		} else if (name == "x-rgbBgColor" && value != "") {
			params = split(value, ",");
			if (params->size() == 3) {
				if (surface != NULL) {
					if (bgColor != NULL) {
						delete bgColor;
						bgColor = NULL;
					}

					bgColor = new Color(
							(int)stof((*params)[0]),
							(int)stof((*params)[1]),
							(int)stof((*params)[2]));

					surface->setBgColor(bgColor);

					// refreshing changes
					loadTxt();
				}
			}

			delete params;
			params = NULL;

		} else if (name == "x-rgbFontColor" && value != "") {
			params = split(value, ",");
			if (params->size() == 3) {
				if (fontColor != NULL) {
					delete fontColor;
					fontColor = NULL;
				}

				fontColor = new Color(
						(int)stof((*params)[0]),
						(int)stof((*params)[1]),
						(int)stof((*params)[2]));

				// refreshing changes
				loadTxt();
			}

			delete params;
			params = NULL;

		} else if (name == "x-content") {
			setContent(value);
		}

		Player::setPropertyValue(name, value, duration, by);
	}
}
}
}
}
}
}
