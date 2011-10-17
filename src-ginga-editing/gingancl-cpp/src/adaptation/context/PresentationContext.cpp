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

#include "../../../include/adaptation/context/PresentationContext.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adaptation {
namespace context {
	PresentationContext::PresentationContext():ContextBase() {
		createObserversVector();
		contextTable = NULL;

		cm = IComponentManager::getCMInstance();
		if (cm != NULL) {
			contextManager = ((ContextManagerCreator*)(cm->getObject(
					"ContextManager")))();

			initializeUserContext();
			initializeSystemValues();
		}
	}

	PresentationContext* PresentationContext::_instance = 0;

	PresentationContext* PresentationContext::getInstance() {
		if (PresentationContext::_instance == NULL) {
			PresentationContext::_instance = new PresentationContext();
		}
		return PresentationContext::_instance;
	}

	void PresentationContext::setPropertyValue(
		    string property, string value) {

		string oldValue = "";
		map<string, string>::iterator i;

		if ((property.length() >= 7 && property.substr(0, 7) == "system.") ||
				(property.length() >= 5 && property.substr(0, 5) == "user.")) {

			return;
		}

		i = contextTable->find(property);

		if (i != contextTable->end()) {
			oldValue = i->second;
		}

		(*contextTable)[property] = value;
		if ((value != "") && (value != oldValue)) {
			notifyObservers(&property);
		}
	}

	vector<string>* PresentationContext::getPropertyNames() {
		vector<string>* propertyNames = new vector<string>;
		map<string, string>::iterator i;
		for (i = contextTable->begin(); i != contextTable->end(); ++i) {
			propertyNames->push_back(i->first);
		}

		return propertyNames;
	}

	string PresentationContext::getPropertyValue(string attributeId) {
		if (contextTable->count(attributeId) == 0) {
			return "";
		}

		return (*contextTable)[attributeId];
	}

	void PresentationContext::initializeUserContext() {
		int currentUserId;

		currentUserId = contextManager->getCurrentUserId();
		contextTable = contextManager->getUserProfile(currentUserId);

		if (contextTable == NULL) {
			contextManager->addContextVar(
					currentUserId, "system.background-color", "000000");

			contextTable = contextManager->getUserProfile(currentUserId);
		}

		initializeUserInfo(currentUserId);
	}

	void PresentationContext::initializeUserInfo(int currentUserId) {
		IGingaUser* user;

		user = contextManager->getUser(currentUserId);
		if (user != NULL) {
			(*contextTable)[USER_AGE] = itos(user->getUserAge());
			(*contextTable)[USER_LOCATION] = user->getUserLocation();
			(*contextTable)[USER_GENRE] = user->getUserGenre();
		}
	}

	void PresentationContext::initializeSystemValues() {
		ISystemInfo* si;
		int w, h;

		si = contextManager->getSystemInfo();

		(*contextTable)[SYSTEM_LANGUAGE] = si->getSystemLanguage();
		(*contextTable)[SYSTEM_CAPTION] = si->getCaptionLanguage();
		(*contextTable)[SYSTEM_SUBTITLE] = si->getSubtitleLanguage();
		(*contextTable)[SYSTEM_RETURN_BIT_RATE] = itos(si->getReturnBitRate());

		si->getScreenSize(&w, &h);
		(*contextTable)[SYSTEM_SCREEN_SIZE] = itos(w) + "," + itos(h);

		si->getScreenGraphicSize(&w, &h);
		(*contextTable)[SYSTEM_SCREEN_GRAPHIC_SIZE] = itos(w) + "," + itos(h);
		(*contextTable)[SYSTEM_AUDIO_TYPE] = si->getAudioType();
		(*contextTable)[SYSTEM_CPU] = itos(si->getCPUClock());
		(*contextTable)[SYSTEM_MEMORY] = itos(si->getMemorySize());
		(*contextTable)[SYSTEM_OPERATING_SYSTEM] = si->getOperatingSystem();

		if (contextTable->count(DEFAULT_FOCUS_BORDER_COLOR) == 0) {
			(*contextTable)[DEFAULT_FOCUS_BORDER_COLOR] = "blue";
		}

		if (contextTable->count(DEFAULT_SEL_BORDER_COLOR) == 0) {
			(*contextTable)[DEFAULT_SEL_BORDER_COLOR] = "green";
		}

		if (contextTable->count(DEFAULT_FOCUS_BORDER_WIDTH) == 0) {
			(*contextTable)[DEFAULT_FOCUS_BORDER_WIDTH] = "3";
		}

		if (contextTable->count(DEFAULT_FOCUS_BORDER_TRANSPARENCY) == 0) {
			(*contextTable)[DEFAULT_FOCUS_BORDER_TRANSPARENCY] = "255";
		}
	}

	void PresentationContext::save() {
		string property;
		map<string, string>::iterator i;

		i = contextTable->begin();
		while (i != contextTable->end()) {
			property = i->first;
			if (property.find("system.") == std::string::npos &&
					property.find("default.") == std::string::npos) {

				contextTable->erase(i);
				i = contextTable->begin();

			} else {
				++i;
			}
		}
		//contextManager->saveUsersAccounts();
		contextManager->saveUsersProfiles();
	}
}
}
}
}
}
}
}
