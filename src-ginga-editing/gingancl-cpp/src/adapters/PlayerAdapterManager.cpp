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

#include "../../include/adapters/PlayerAdapterManager.h"

#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
	PlayerAdapterManager::PlayerAdapterManager() {
		objectPlayers = new map<string, IPlayerAdapter*>;
		editingCommandListener = NULL;
		epgFactoryAdapter = NULL;
		pthread_mutex_init(&mutexPlayer, NULL);
		readConfigFiles();
	}

	PlayerAdapterManager::~PlayerAdapterManager() {
		clear();
		pthread_mutex_lock(&mutexPlayer);
		if (objectPlayers != NULL) {
			delete objectPlayers;
			objectPlayers = NULL;
		}

		mimeDefaultTable->clear();
		delete mimeDefaultTable;
		mimeDefaultTable = NULL;

		playerTable->clear();
		delete playerTable;
		playerTable = NULL;

		pthread_mutex_unlock(&mutexPlayer);
		pthread_mutex_destroy(&mutexPlayer);
	}

	void PlayerAdapterManager::clear() {
		pthread_mutex_lock(&mutexPlayer);
		if (objectPlayers != NULL) {
			map<string, IPlayerAdapter*>::iterator i;
			IPlayerAdapter* player;

			i = objectPlayers->begin();
			while (i != objectPlayers->end()) {
				player = i->second;
				if (player != NULL) {
					delete player;
					player = NULL;
				}
				++i;
			}

			objectPlayers->clear();
		}

		pthread_mutex_unlock(&mutexPlayer);
	}

	void PlayerAdapterManager::setNclEditListener(INclEditListener* listener) {
		this->editingCommandListener = listener;
	}

	string PlayerAdapterManager::getPlayerClass(
		    CascadingDescriptor* descriptor, NodeEntity* dataObject) {

		string toolName = "";
		string mime = "";
		string nodeType = "";

		if (descriptor != NULL) {
			toolName = descriptor->getPlayerName();
		}

		if (dataObject->instanceOf("ContentNode")) {
			mime = ((ContentNode*)dataObject)->getNodeType();
			if (mime == "" && toolName == "") {
				return "";
			}

			if (((ContentNode*)dataObject)->getNodeType() ==
					ContentNode::SETTING_NODE) {

				return "SETTING_NODE";
			}
		}

		if (toolName == "") {
			// there is no player defined, so it should be chose a player
			// based on the node content type
			if (mimeDefaultTable->count(mime) != 0) {
				toolName = (*mimeDefaultTable)[mime];
			}

			if (toolName != "") {
				return toolName;
			}

		} else {
			if (playerTable->count(toolName)) {
				return (*playerTable)[toolName];
			}
		}

		toolName = "";
		return toolName;
	}

	void PlayerAdapterManager::readConfigFiles() {
		ifstream fisMime;
		ifstream fisCtrl;
		string line, key, value;

		fisMime.open(
				"/usr/local/lib/ginga/files/cfg/formatter/mimedefs.ini",
				ifstream::in);

		if (!fisMime.is_open()) {
			cout << "PlayerAdapterManager: can't open mimedefs.ini" << endl;
			return;
		}

		mimeDefaultTable = new map<string, string>;
		while (fisMime.good()) {
			fisMime >> line;
			key = line.substr(0, line.find_last_of("="));
			value = line.substr(
					(line.find_first_of("=") + 1),
					line.length() - (line.find_first_of("=") + 1));

			(*mimeDefaultTable)[key] = value;
		}

		fisMime.close();

		fisCtrl.open("/usr/local/lib/ginga/files/cfg/formatter/ctrldefs.ini");
		if (!fisCtrl.is_open()) {
			cout << "PlayerAdapterManager: can't open ctrldefs.ini" << endl;
			return;
		}

		playerTable = new map<string, string>;
		while (fisCtrl.good()) {
			fisCtrl >> line;
			key = line.substr(0, line.find_last_of("="));
			value = line.substr(
					(line.find_first_of("=") + 1),
					line.length() - (line.find_first_of("=") + 1));

			(*playerTable)[key] = value;
		}

		fisCtrl.close();
	}

	FormatterPlayerAdapter* PlayerAdapterManager::initializePlayer(
		    ExecutionObject* object) {

		IComponentManager* cm;
		CascadingDescriptor* descriptor;
		NodeEntity* dataObject;
		string playerClassName, objId;
		IPlayerAdapter* player = NULL;
		vector<string>* args;

		if (object == NULL) {
			return NULL;
		}

		objId = object->getId();
		descriptor = object->getDescriptor();
		dataObject = (NodeEntity*)(object->getDataObject()->getDataEntity());

		// checking if is a main AV reference
		Content* content;
		string url = "";

		playerClassName = "";
		content = dataObject->getContent();
		if (content != NULL) {
			if (content->instanceOf("ReferenceContent")) {
				url = ((ReferenceContent*)(content))->
					    getCompleteReferenceUrl();

				if (url.length() > 10 && url.substr(0,10) == "x-sbtvdts:") {
					playerClassName =
						"libginganclpavpadapter.so,createPAVAdapter";
				}
			}
		}

		if (playerClassName == "") {
			playerClassName = getPlayerClass(descriptor, dataObject);
		}

		if (playerClassName == "SETTING_NODE") {
			return NULL;
		}

		if (playerClassName == "") {
			player = new FormatterPlayerAdapter();
			(*objectPlayers)[objId] = player;
			return (FormatterPlayerAdapter*)player;
		}

		args = split(playerClassName, ",");
		if (args->size() < 1) {
			delete args;
			return NULL;

		} else if (args->size() == 1) {
			args->push_back("");
		}

		cm = IComponentManager::getCMInstance();
		player = ((AdapterCreator*)(cm->getObject(
				(*args)[0])))((void*)(((*args)[1]).c_str()));

		if (player == NULL) {
			delete args;
			return NULL;
		}

		if ((*args)[1] == "epg") {
			epgFactoryAdapter = player;
		}

		if ((*args)[1] == "epg" || (*args)[1] == "nclEdit") {
			((FormatterPlayerAdapter*)player)->setNclEditListener(
					editingCommandListener);
		}

//#IF HAVA_WAC
		StateManager::getInstance()->addPlayerAdapter(objId, player);
//#ENDIF
		delete args;
		(*objectPlayers)[objId] = player;
		return (FormatterPlayerAdapter*)player;
	}

	FormatterPlayerAdapter* PlayerAdapterManager::getPlayer(
		    ExecutionObject* execObj) {

		map<string, IPlayerAdapter*>::iterator i;
		FormatterPlayerAdapter* player;
		string objId;

		pthread_mutex_lock(&mutexPlayer);
		objId = execObj->getId();
		i = objectPlayers->find(objId);
		if (i == objectPlayers->end()) {
			player = initializePlayer(execObj);

		} else {
			player = (FormatterPlayerAdapter*)(i->second);
		}
		pthread_mutex_unlock(&mutexPlayer);

		return player;
	}

	bool PlayerAdapterManager::isProcedural(NodeEntity* dataObject) {
		string mediaType = "";
		string url = "";
		string::size_type pos;
		Descriptor* descriptor;
		Content* content;

		//first, descriptor
		descriptor = (Descriptor*)(dataObject->getDescriptor());
		if (descriptor != NULL) {
			mediaType = descriptor->getPlayerName();
			if (mediaType != "") {
				if (mediaType == "NCLetPlayerAdapter" ||
						mediaType == "LuaPlayerAdapter") {

					return true;
				}
				return false;
			}
		}

		//second, media type
		if (dataObject->instanceOf("ContentNode")) {
			mediaType = ((ContentNode*)dataObject)->getNodeType();
			if (mediaType != "") {
				if (mediaType == "application/x-ginga-NCLua" ||
						mediaType == "application/x-ginga-NCLet") {

					return true;
				}
				return false;
			}
		}

		//finally, content file extension
		content = dataObject->getContent();
		if (content != NULL) {
			if (content->instanceOf("ReferenceContent")) {
				url = ((ReferenceContent*)(content))->
					    getCompleteReferenceUrl();

				if (url != "") {
					pos = url.find_last_of(".");
					if (pos != std::string::npos) {
						pos++;
						mediaType = ContentTypeManager::getInstance()->
								getMimeType(
										url.substr(pos, url.length() - pos));

						if (mediaType == "application/x-ginga-NCLua" ||
								mediaType == "application/x-ginga-NCLet") {

							return true;
						}
					}
				}
			}
		}

		return false;
	}

	void PlayerAdapterManager::pushEPGEventToEPGFactory(map<string, string> t) {
		if (epgFactoryAdapter != NULL) {
			//epgFactoryAdapter->pushEPGEvent(t);
		}
	}
}
}
}
}
}
}
