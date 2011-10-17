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

#include "../../../include/PlainTxtPlayerAdapter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace text {
	PlainTxtPlayerAdapter::PlainTxtPlayerAdapter() : FormatterPlayerAdapter() {

	}

	void PlainTxtPlayerAdapter::createPlayer() {
		string paramValue;
		CascadingDescriptor* descriptor;
		vector<string>* params;

		if (fileExists(mrl)) {
			player = new PlainTxtPlayer();
			((PlainTxtPlayer*)player)->setFile(mrl);

		} else {
			player = NULL;
			cout << "PlainTxtPlayerAdapter::createPlayer Warning! ";
			cout << "file not found: '" << mrl.c_str() << "'" << endl;
			return;
		}

		descriptor = object->getDescriptor();
		if (descriptor != NULL && player != NULL) {
			paramValue = descriptor->getParameterValue("x-fontUri");
			if (paramValue != "") {
		    	((PlainTxtPlayer*)player)->setFont(paramValue);

			} else {
				((PlainTxtPlayer*)player)->
					    setFont("/usr/local/share/fonts/truetype/vera.ttf");
			}

			paramValue = descriptor->getParameterValue("x-fontSize");
			if (paramValue != "") {
				((PlainTxtPlayer*)player)->
					    setFontSize((int)stof(paramValue));

			} else {
				((PlainTxtPlayer*)player)->setFontSize(10);
			}

			paramValue = descriptor->getParameterValue("x-bgColor");
			if (paramValue != "") {
				Color* bgColor = new Color(paramValue);
				((TextPlayer*)player)->setBgColor(
		    		    bgColor->getR(),
		    		    bgColor->getG(),
		    		    bgColor->getB(),
		    		    255);

				delete bgColor;
				bgColor = NULL;

			} else {
				paramValue = descriptor->getParameterValue("x-rgbBgColor");
				if (paramValue != "") {
					params = split(paramValue, ",");
					if (params->size() == 3) {
						((TextPlayer*)player)->setBgColor(
				    		    (int)stof((*params)[0]),
				    		    (int)stof((*params)[1]),
				    		    (int)stof((*params)[2]),
				    		    255);

					} else {
						((PlainTxtPlayer*)player)->setBgColor(0, 0, 0, 255);
					}

					delete params;
					params = NULL;

				} else {
					((PlainTxtPlayer*)player)->setBgColor(0, 0, 0, 255);
				}
			}

			paramValue = descriptor->getParameterValue("x-fontColor");
			if (paramValue != "") {
				Color* fontColor = new Color(paramValue);
				((PlainTxtPlayer*)player)->setColor(
		    		    fontColor->getR(),
		    		    fontColor->getG(),
		    		    fontColor->getB(),
		    		    255);

				delete fontColor;
				fontColor = NULL;

			} else {
				paramValue = descriptor->getParameterValue("x-rgbFontColor");
				if (paramValue != "") {
					params = split(paramValue, ",");
					if (params->size() == 3) {
						((TextPlayer*)player)->setColor(
				    		    (int)stof((*params)[0]),
				    		    (int)stof((*params)[1]),
				    		    (int)stof((*params)[2]),
				    		    255);

					} else {
						((PlainTxtPlayer*)player)->setColor(255, 255, 255);
					}

					delete params;
					params = NULL;

				} else {
					((PlainTxtPlayer*)player)->setColor(255, 255, 255);
				}
			}
		}

		FormatterPlayerAdapter::createPlayer();
	}
}
}
}
}
}
}
}
