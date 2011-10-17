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

#include "../../include/PlayerAdapterManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
	PlayerAdapterManager::PlayerAdapterManager() {
		objectPlayers = new map<string, FormatterPlayerAdapter*>;
		editingCommandListener = NULL;
		readConfigFiles();
	}

	PlayerAdapterManager::~PlayerAdapterManager() {
		wclog << "PlayerAdapterManager::~PlayerAdapterManager" << endl;
		if (objectPlayers != NULL) {
			map<string, FormatterPlayerAdapter*>::iterator i;
			FormatterPlayerAdapter* player;

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
			delete objectPlayers;
			objectPlayers = NULL;
		}

		mimeDefaultTable->clear();
		delete mimeDefaultTable;
		mimeDefaultTable = NULL;

		playerTable->clear();
		delete playerTable;
		playerTable = NULL;
	}

	void PlayerAdapterManager::setNclEditListener(NclEditListener* listener) {
		this->editingCommandListener = listener;
	}

	string PlayerAdapterManager::getPlayerClass(
		    CascadingDescriptor* descriptor, NodeEntity* dataObject) {

		string toolName = "";
		string mime = "";

		if (dataObject->instanceOf("ContentNode")) {
			if (((ContentNode*)dataObject)->getNodeType() == "") {
				return "";
			}

			if (upperCase(((ContentNode*)dataObject)->getNodeType()) ==
					upperCase(ContentNode::SETTING_NODE)) {

				return "SETTING_NODE";
			}
		}

		if (descriptor == NULL) {
			toolName = "";

		} else {
			toolName = descriptor->getPlayerName();
		}

		wclog << "PlayerAdapterManager::getPlayerClass toolName = '";
		wclog << toolName.c_str() << "'" << endl;
		if (toolName == "") {
			// there is no player defined, so it should be chose a player
			// based on the node content type
			if (dataObject->instanceOf("ContentNode")) {
				mime = ((ContentNode*)dataObject)->getNodeType();
				wclog << "PlayerAdapterManager::getPlayerClass mime = '";
				wclog << mime.c_str() << "'" << endl;

			} else {
				wclog << "PlayerAdapterManager::getPlayerClass !contentNode";
				wclog << ", return ''" << endl;
				return "";
			}

			if (mime != "") {
				if (mimeDefaultTable->count(mime) != 0) {
					toolName = (*mimeDefaultTable)[mime];
				}

				if (toolName != "") {
					return toolName;
				}

			} else {
				return "";
			}

		} else {
			if (playerTable->count(toolName)) {
				return (*playerTable)[toolName];
			}
		}
		return "";
	}

	void PlayerAdapterManager::readConfigFiles() {
		ifstream fisMime;
		ifstream fisCtrl;
		string line, key, value;

		fisMime.open("/misc/mimedefs.ini", ifstream::in);
		if (!fisMime.is_open()) {
			wclog << "PlayerAdapterManager: can't open mimedefs.ini" << endl;
			return;
		}

		mimeDefaultTable = new map<string, string>;
		while (fisMime.good()) {
			fisMime >> line;
			key = line.substr(0, line.find_last_of("="));
			value = line.substr((line.find_first_of("=") + 1), line.length());
			(*mimeDefaultTable)[key] = value;
		}

		fisMime.close();

		fisCtrl.open("/misc/ctrldefs.ini");
		if (!fisCtrl.is_open()) {
			wclog << "PlayerAdapterManager: can't open ctrldefs.ini" << endl;
			return;
		}

		playerTable = new map<string, string>;
		while (fisCtrl.good()) {
			fisCtrl >> line;
			key = line.substr(0, line.find_last_of("="));
			value = line.substr((line.find_first_of("=") + 1), line.length());
			(*playerTable)[key] = value;
		}

		fisCtrl.close();
	}

	FormatterPlayerAdapter* PlayerAdapterManager::initializePlayer(
		    ExecutionObject* object) {

		CascadingDescriptor* descriptor;
		NodeEntity* dataObject;
		string playerClassName, objId;
		FormatterPlayerAdapter* player = NULL;

		if (object == NULL) {
			return NULL;
		}

		objId = object->getId();
		descriptor = object->getDescriptor();
		dataObject = (NodeEntity*)(object->getDataObject()->getDataEntity());

		// checking if is a main AV reference
		Content* content;
		string url = "";

		content = dataObject->getContent();

		if (content != NULL) {
			if (content->instanceOf("ReferenceContent")) {
				url = ((ReferenceContent*)(content))->
					    getCompleteReferenceUrl();

				if (url.substr(0,10) == "x-sbtvdts:") {
					player = new ProgramAVPlayerAdapter();
					(*objectPlayers)[objId] = player;
					return player;
				}
			}
		}

		playerClassName = getPlayerClass(descriptor, dataObject);

		if (playerClassName == "formatter.player.text.SubtitlePlayerAdapter") {
			player = new SubtitlePlayerAdapter();

		} else if (playerClassName ==
			    "formatter.player.text.PlainTxtPlayerAdapter") {

			player = new PlainTxtPlayerAdapter();

		} else if (playerClassName ==
			    "formatter.player.text.LinksPlayerAdapter") {

			player = new LinksPlayerAdapter();

		} else if (playerClassName ==
			    "formatter.player.image.ImagePlayerAdapter") {

			player = new ImagePlayerAdapter();

		} else if (playerClassName ==
			    "formatter.player.av.AudioPlayerAdapter") {

			player = new AVPlayerAdapter(false);

		} else if (playerClassName ==
			    "formatter.player.av.VideoPlayerAdapter") {

			player = new AVPlayerAdapter(true);

		} else if (playerClassName == 
			    "formatter.player.procedural.lua.LuaPlayerAdapter") {

			player = new LuaPlayerAdapter();
			((ProceduralPlayerAdapter*)player)->setNclEditListener(
					editingCommandListener);

		} else if (playerClassName == 
			    "formatter.player.av.VideoChannelPlayerAdapter") {

			player = new ChannelPlayerAdapter(true);

		} else if (playerClassName == 
			    "formatter.player.av.AudioChannelPlayerAdapter") {

			player = new ChannelPlayerAdapter(false);

		} else if (playerClassName != "SETTING_NODE") {
			cout << "PlayerAdapterManager::initializePlayer is creating a ";
			cout << "new time player for '" << objId << "'" << endl;
			player = new FormatterPlayerAdapter();

		} else {
			cout << "PlayerAdapterManager::initializePlayer is returning a ";
			cout << "NULL player for '" << objId << "'" << endl;
			return NULL;
		}

		(*objectPlayers)[objId] = player;
		return player;
	}

	FormatterPlayerAdapter* PlayerAdapterManager::getPlayer(
		    ExecutionObject* execObj) {

		string objId;
		objId = execObj->getId();
		if (objectPlayers->count(objId) == 0) {
			return initializePlayer(execObj);

		} else {
			return (*objectPlayers)[objId];
		}
	}

	bool PlayerAdapterManager::isProcedural(NodeEntity* dataObject) {
		string mediaType = "";
		string url = "";
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

				if (url.find(".") != std::string::npos) {
					mediaType = ContentTypeManager::getInstance()->getMimeType(
							url.substr(url.find_last_of("."), url.length()));

					if (mediaType == "application/x-ginga-NCLua" ||
							mediaType == "application/x-ginga-NCLet") {

						return true;
					}
				}
			}
		}

		return false;
	}

}
}
}
}
}
}
