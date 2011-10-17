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

#include "../../../include/ChannelPlayerAdapter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace av {
  	ChannelPlayerAdapter::ChannelPlayerAdapter(bool visual) :
  		    FormatterPlayerAdapter() {

		//cout << "ChannelPlayerAdapter::ChannelPlayerAdapter" << endl;
	    typeSet.insert("ChannelPlayerAdapter");
	    hasVisual = visual;
	}

	void ChannelPlayerAdapter::createPlayer() {
		Player* childPlayer = NULL;
		CompositeExecutionObject* cpExObj;
		ExecutionObject* childObj;
		vector<ExecutionObject*>* objects;
		map<string, Player*>* objectMap;
		Content* content;
		string mrlPlayer;
		string selectedObject = "";

		cpExObj = (CompositeExecutionObject*)object;
		objectMap = new map<string, Player*>; 
		objects = cpExObj->getExecutionObjects();

		if (objects != NULL && cpExObj->getDescriptor() != NULL) {
			vector<ExecutionObject*>::iterator i;
			i = objects->begin();
			while (i != objects->end()) {
				childObj = *i;
				if (childObj != NULL && childObj->getDataObject() != NULL &&
						childObj->getDataObject()->getDataEntity() != NULL &&
						((NodeEntity*)(childObj->getDataObject()->
							getDataEntity()))->getContent() != NULL) {

					content = ((NodeEntity*)(childObj->getDataObject()->
						    getDataEntity()))->getContent();

					if (content->instanceOf("ReferenceContent")) {
						mrlPlayer = ((ReferenceContent*)content)->
							    getCompleteReferenceUrl();

						childPlayer = new AVPlayer(mrlPlayer, hasVisual);
						(*objectMap)[childObj->
						             getDataObject()->getId()] = childPlayer;
					}
				}
				++i;
			}

			//cout << "ChannelPlayerAdapter::createPlayer objMap->size = ";
			//cout << objectMap->size() << endl;
			selectedObject = cpExObj->getDescriptor()->
				    getParameterValue("selectedObject");
		}

		if (objects != NULL) {
			delete objects;
			objects = NULL;
		}

		player = new ChannelPlayer(objectMap);
		if (selectedObject != "") {
			childPlayer = ((ChannelPlayer*)player)->getPlayer(selectedObject);
			((ChannelPlayer*)player)->select(childPlayer);
		}

		FormatterPlayerAdapter::createPlayer();
	}

	bool ChannelPlayerAdapter::setPropertyValue(
		    AttributionEvent* event, string value, Animation* animation) {

		if (value == "") {
			event->stop();
	  		return false;
	  	}

		CascadingDescriptor* descriptor;
		string propName = "";
		string paramValue = "FALSE";

		descriptor = object->getDescriptor();
		propName = event->getAnchor()->getPropertyName();
		if (propName == "selectedObject") {
			Player* oldPlayer = NULL;
			Player* newPlayer = NULL;
			double oldPlayerMediaTime = 0;

			oldPlayer = ((ChannelPlayer*)player)->getSelectedPlayer();
			if (oldPlayer == NULL) {
				cout << "ChannelPlayerAdapter::setPropertyValue Warning!";
				cout << " cant find oldPlayer to new '" << value << "'";
				cout << endl;
				event->stop();
				return false;
			}

			newPlayer = ((ChannelPlayer*)player)->getPlayer(value);
			if (newPlayer == NULL) {
				cout << "ChannelPlayerAdapter::setPropertyValue Warning!";
				cout << " cant find newPlayer for '" << value << "'";
				cout << ". Did you selected the same option twice?" << endl;
				event->stop();
				return false;
			}

			if (descriptor->getParameterValue("x-entryInstant") == "begin") {
				((AVPlayer*)newPlayer)->setMediaTime(0);

			} else {
				oldPlayerMediaTime = oldPlayer->getMediaTime();
				((AVPlayer*)newPlayer)->setMediaTime(oldPlayerMediaTime);
			}

			oldPlayer->stop();
			if (oldPlayer->getSurface() != NULL &&
					oldPlayer->getSurface()->getParent() != NULL) {

				Color* color = new Color("black");
				oldPlayer->getSurface()->setBgColor(color);
				delete color;
				color = NULL;
			}
			newPlayer->play();

			((ChannelPlayer*)player)->select(newPlayer);
			oldPlayer->stop();
			event->stop();
			return true;

		} else {
			return FormatterPlayerAdapter::setPropertyValue(
					event, value, animation);
		}
	}

	bool ChannelPlayerAdapter::getHasVisual() {
		return hasVisual;
	}
}
}
}
}
}
}
}
