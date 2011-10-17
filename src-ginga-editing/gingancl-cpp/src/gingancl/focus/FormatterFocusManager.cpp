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

#include "../../include/FormatterFocusManager.h"

#include "../../include/FormatterConverter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::converter;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace focus {
	FormatterFocusManager::FormatterFocusManager(
		    PlayerAdapterManager* playerManager,
		    void* converter) {

		focusTable = new map<string, ExecutionObject*>;
		focusSequence = new vector<string>;
		currentFocus = "";

		defaultFocusBorderColor = new Color("blue");
		defaultFocusBorderWidth = -3;
		defaultSelBorderColor = new Color("green");
		selectedObject = NULL;

		this->playerManager = playerManager;
		this->converter = converter;
	}

	FormatterFocusManager::~FormatterFocusManager() {
		selectedObject = NULL;

		if (defaultFocusBorderColor != NULL) {
			delete defaultFocusBorderColor;
			defaultFocusBorderColor = NULL;
		}

		if (defaultSelBorderColor != NULL) {
			delete defaultSelBorderColor;
			defaultSelBorderColor = NULL;
		}

		delete focusSequence;
		focusSequence = NULL;

		delete focusTable;
		focusTable = NULL;

		playerManager = NULL;
	}

	void FormatterFocusManager::setKeyMaster(string focusIndex) {
		ExecutionObject* nextObject;
		ExecutionObject* currentObject;
		CascadingDescriptor* currentDescriptor = NULL;
		FormatterPlayerAdapter* player;

		if (focusTable->count(focusIndex) == 0) {
			return;
		}

		nextObject = (*focusTable)[focusIndex];
		if (!(nextObject->getDescriptor()->getFormatterRegion()->isVisible())) {
			return;
		}

		if (focusTable->count(currentFocus) != 0) {
			currentObject = (*focusTable)[currentFocus];
			currentDescriptor = currentObject->getDescriptor();

			if (currentObject != selectedObject) {
				currentDescriptor->getFormatterRegion()->setFocus(false);
				recoveryDefaultState(currentObject);
			}
		}

		currentFocus = focusIndex;

		if (currentDescriptor != NULL &&
				currentDescriptor->getFormatterRegion()->setSelection(true)) {

			if (selectedObject != NULL) {
				selectedObject->getDescriptor()->
					    getFormatterRegion()->setSelection(false);

				recoveryDefaultState(selectedObject);
			}

			selectedObject = currentObject;
			player = playerManager->getPlayer(selectedObject);

			enterSelection(player);
			currentObject->select(
					CodeMap::KEY_NULL, player->getMediaTime());

		} else {
			cout << "FormatterFocusManager::setCurrentKeyMaster cant ";
			cout << " select '" << currentFocus << "'" << endl;
		}
	}

	void FormatterFocusManager::setFocus(string focusIndex) {
		ExecutionObject* nextObject;
		ExecutionObject* currentObject;
		CascadingDescriptor* currentDescriptor;

		if (focusTable->count(focusIndex) == 0) {
			cout << "FormatterFocusManager::setFocus Warning! index '";
			cout << focusIndex << "' not found in focusTable." << endl;
			return;
		}

		nextObject = (*focusTable)[focusIndex];
		if (!(nextObject->getDescriptor()->getFormatterRegion()->isVisible())) {
			cout << "FormatterFocusManager::setFocus Warning! index '";
			cout << focusIndex << "' is not visible." << endl;
			return;
		}

		if (focusTable->count(currentFocus) != 0) {
			currentObject = (*focusTable)[currentFocus];
			currentDescriptor = currentObject->getDescriptor();

			if (currentObject != selectedObject) {
				currentDescriptor->getFormatterRegion()->setFocus(false);
				recoveryDefaultState(currentObject);
			}
		}

		currentFocus = focusIndex;
		PresentationContext::getInstance()->setPropertyValue(
				"service.currentFocus", currentFocus);

		if (nextObject != selectedObject) {
			setFocus(nextObject->getDescriptor());
		}
	}

	void FormatterFocusManager::setFocus(CascadingDescriptor* descriptor) {
		double borderAlpha;
		Color* focusColor = NULL;
		Color* selColor = NULL;
		int borderWidth = -3;
		int width;
		FormatterRegion* region = NULL;

		if (descriptor != NULL) {
			focusColor = descriptor->getFocusBorderColor();
			borderWidth = descriptor->getFocusBorderWidth();
			selColor = descriptor->getSelBorderColor();
			region = descriptor->getFormatterRegion();
		}

		if (focusColor == NULL) {
			focusColor = defaultFocusBorderColor;
		}

		borderAlpha = descriptor->getFocusBorderTransparency();
		if (!isNaN(borderAlpha)) {
			focusColor = new Color(focusColor->getR(), focusColor->getG(),
					focusColor->getB(), (int)(borderAlpha * 255));
		}

		width = borderWidth;

		if (selColor == NULL) {
			selColor = defaultSelBorderColor;
		}

		if (!isNaN(borderAlpha)) {
			selColor = new Color(selColor->getR(), selColor->getG(),
					selColor->getB(), (int)(borderAlpha * 255));
		}

		if (region != NULL) {
			region->setFocusInfo(focusColor, width,
					descriptor->getFocusSrc(), selColor, width,
					descriptor->getSelectionSrc());

			region->setFocus(true);
		}

		if (focusColor != NULL && focusColor != defaultFocusBorderColor) {
			if (descriptor == NULL || (descriptor != NULL &&
					descriptor->getFocusBorderColor() != focusColor)) {

				delete focusColor;
				focusColor = NULL;
			}
		}

		if (selColor != NULL && selColor != defaultSelBorderColor) {
			if (descriptor == NULL || (descriptor != NULL &&
					descriptor->getSelBorderColor() != selColor)) {

				delete selColor;
				selColor = NULL;
			}
		}
	}

	void FormatterFocusManager::recoveryDefaultState(ExecutionObject* object) {
		if (object == NULL || object->getDescriptor() == NULL ||
				object->getDescriptor()->getFormatterRegion() == NULL) {

			return;
		}

		FormatterPlayerAdapter* player;
		player = playerManager->getPlayer(object);
		if (player != NULL) {
			Surface* renderedSurface;
			renderedSurface = player->getObjectDisplay();
			object->getDescriptor()->getFormatterRegion()->
				    renderSurface(renderedSurface);
		}
	}

	void FormatterFocusManager::showObject(ExecutionObject* object) {
		CascadingDescriptor* descriptor;
		string focusIndex, auxIndex;
		string paramValue;
		FormatterPlayerAdapter* player;
		ExecutionObject* currentObject = NULL;
		set<int>* evs;
		vector<string>::iterator i;

		descriptor = object->getDescriptor();
		if (object == NULL || descriptor == NULL) {
			cout << "FormatterFocusManager::showObject Warning! object or";
			cout << " descriptor is null." << endl;
			return;
		}

		focusIndex = descriptor->getFocusIndex();
		if (focusIndex == "") {
			cout << "FormatterFocusManager::showObject Warning! focusIndex";
			cout << " is null." << endl;
			return;
		}

		(*focusTable)[focusIndex] = object;

		// put new index in alphabetical order
		for (i = focusSequence->begin(); i != focusSequence->end(); ++i) {
			auxIndex = *i;
			if (focusIndex.compare(auxIndex) <= 0) {
				break;
			}
		}

		focusSequence->insert(i, focusIndex);

		if (currentFocus == "") {
			evs = new set<int>;
			evs->insert(CodeMap::KEY_CURSOR_DOWN);
			evs->insert(CodeMap::KEY_CURSOR_LEFT);
			evs->insert(CodeMap::KEY_CURSOR_RIGHT);
			evs->insert(CodeMap::KEY_CURSOR_UP);

			evs->insert(CodeMap::KEY_ENTER);
			evs->insert(CodeMap::KEY_OK);

			InputManager::getInstance()->addInputEventListener(this, evs);
			currentFocus = focusIndex;
			paramValue = PresentationContext::getInstance()->getPropertyValue(
					"service.currentKeyMaster");

			if (paramValue == currentFocus) {
				setKeyMaster(paramValue);

			} else {
				setFocus(descriptor);
			}

		} else {
			paramValue = PresentationContext::getInstance()->
					getPropertyValue("service.currentFocus");

			if (paramValue != "" &&
				    paramValue == focusIndex &&
				    descriptor->getFormatterRegion()->isVisible()) {

				if (focusTable->count(currentFocus) != 0) {
					currentObject = (*focusTable)[currentFocus];
					currentObject->getDescriptor()->
						    getFormatterRegion()->setFocus(false);

					recoveryDefaultState(currentObject);
				}

			 	currentFocus = focusIndex;
			 	setFocus(descriptor);
			}

			//service.currentKeyMaster
			paramValue = PresentationContext::getInstance()->getPropertyValue(
					"service.currentKeyMaster");

			if (paramValue == focusIndex &&
					descriptor->getFormatterRegion()->isVisible()) {

				// first set as current focus
				if (focusTable->count(currentFocus) != 0) {
					currentObject = (*focusTable)[currentFocus];
					currentObject->getDescriptor()->
						    getFormatterRegion()->setFocus(false);

					recoveryDefaultState(currentObject);
				}

				currentFocus = focusIndex;
				PresentationContext::getInstance()->setPropertyValue(
						"service.currentFocus", currentFocus);

				setFocus(descriptor);

				// then set as selected
				if (descriptor->getFormatterRegion()->setSelection(true)) {
					// unselect the previous selected object, if exists
					if (selectedObject != NULL) {
						selectedObject->getDescriptor()->
							    getFormatterRegion()->setSelection(false);

						recoveryDefaultState(selectedObject);
					}

					selectedObject = object;
					player = playerManager->getPlayer(selectedObject);
					enterSelection(player);
				}
			}
    	}
	}

	void FormatterFocusManager::hideObject(ExecutionObject* object) {
		string focusIndex;
		FormatterRegion* region;
		ExecutionObject* newFocusObject;
		FormatterPlayerAdapter* player;

		if (object == NULL || object->getDescriptor() == NULL ||
				object->getDescriptor()->getFormatterRegion() == NULL) {

			return;
		}

		region = object->getDescriptor()->getFormatterRegion();
		focusIndex = object->getDescriptor()->getFocusIndex();
		if (focusIndex != "") {
			map<string, ExecutionObject*>::iterator i;
			i = focusTable->find(focusIndex);
			if (i != focusTable->end()) {
				focusTable->erase(i);
			}

			vector<string>::iterator j;
			j = focusSequence->begin();
			while (j != focusSequence->end()) {
				if (*j == focusIndex) {
					focusSequence->erase(j);
					break;
				}
				++j;
			}

			if (region->getFocusState() == FormatterRegion::SELECTED &&
					selectedObject == object) {

				player = playerManager->getPlayer(selectedObject);
				exitSelection(player);
				region->setSelection(false);
				recoveryDefaultState(selectedObject);
				selectedObject = NULL;
			}

			if (currentFocus == focusIndex) {
				region->setFocus(false);
				recoveryDefaultState(object);

				if (focusSequence->empty()) {
					currentFocus = "";
					InputManager::getInstance()->
						    removeInputEventListener(this);

				} else {
					currentFocus = (*focusSequence)[0];
					newFocusObject = (*focusTable)[currentFocus];
					setFocus(newFocusObject->getDescriptor());
				}
			}
		}
	}

	void FormatterFocusManager::enterSelection(
			FormatterPlayerAdapter* player) {

		set<int>* iEvs;

		iEvs = new set<int>;
		iEvs->insert(CodeMap::KEY_BACKSPACE);
		iEvs->insert(CodeMap::KEY_BACK);
		InputManager::getInstance()->addInputEventListener(this, iEvs);

		PresentationContext::getInstance()->setPropertyValue(
				"service.currentKeyMaster", currentFocus);

		player->setFocusHandler(true);
	}

	void FormatterFocusManager::exitSelection(FormatterPlayerAdapter* player) {
		set<int>* evs;

		InputManager::getInstance()->removeInputEventListener(this);
		player->setFocusHandler(false);

		PresentationContext::getInstance()->setPropertyValue(
				"service.currentKeyMaster", "");

		evs = new set<int>;
		evs->insert(CodeMap::KEY_CURSOR_DOWN);
		evs->insert(CodeMap::KEY_CURSOR_LEFT);
		evs->insert(CodeMap::KEY_CURSOR_RIGHT);
		evs->insert(CodeMap::KEY_CURSOR_UP);

		evs->insert(CodeMap::KEY_ENTER);
		evs->insert(CodeMap::KEY_OK);
		InputManager::getInstance()->addInputEventListener(this, evs);
	}

	void FormatterFocusManager::setDefaultFocusBorderColor(Color* color) {
		if (defaultFocusBorderColor != NULL) {
			delete defaultFocusBorderColor;
		}
		defaultFocusBorderColor = color;
	}

	void FormatterFocusManager::setDefaultFocusBorderWidth(int width) {
		defaultFocusBorderWidth = width;
	}

	void FormatterFocusManager::setDefaultSelBorderColor(Color* color) {
		if (defaultSelBorderColor != NULL) {
			delete defaultSelBorderColor;
		}
		defaultSelBorderColor = color;
	}

	void FormatterFocusManager::changeSettingState(string name, string act) {
		set<ExecutionObject*>* settingObjects;
		set<ExecutionObject*>::iterator i;
		vector<FormatterEvent*>* events;
		vector<FormatterEvent*>::iterator j;
		PropertyAnchor* property;
		string pName;

		settingObjects = ((FormatterConverter*)
				converter)->getSettingNodeObjects();

		i = settingObjects->begin();
		while (i != settingObjects->end()) {
			events = (*i)->getEvents();
			if (events != NULL) {
				j = events->begin();
				while (j != events->end()) {
					if ((*j)->instanceOf("AttributionEvent")) {
						property = ((AttributionEvent*)(*j))->getAnchor();
						pName = property->getPropertyName();
						if (pName == name) {
							if (act == "start") {
								(*j)->start();

							} else if (act == "stop") {
								if (pName == "service.currentKeyMaster" ||
										pName == "service.currentFocus") {

									((AttributionEvent*)(*j))->setValue(
											currentFocus);
								}
								(*j)->stop();
							}
						}
					}
					++j;
				}
				delete events;
			}
			++i;
		}

		delete settingObjects;
	}

	bool FormatterFocusManager::userEventReceived(InputEvent* userEvent) {
		ExecutionObject* currentObject;
		CascadingDescriptor* currentDescriptor, *selectedDescriptor;
		string nextIndex;
		FormatterPlayerAdapter* player;

		const int code = userEvent->getKeyCode();

		if (focusTable->count(currentFocus) == 0) {
			cout << "FormatterFocusManager::userEventReceived ";
			cout << "focus not Found current is '" << currentFocus;
			cout << "'" << endl;
			return true;
		}
		currentObject = (*focusTable)[currentFocus];
		currentDescriptor = currentObject->getDescriptor();
		nextIndex = "";
		if (selectedObject != NULL) {
			if (code == CodeMap::KEY_BACKSPACE || code == CodeMap::KEY_BACK) {
				selectedDescriptor = selectedObject->getDescriptor();
				selectedDescriptor->getFormatterRegion()->setSelection(false);
				recoveryDefaultState(selectedObject);
				nextIndex = selectedDescriptor->getFocusIndex();
				selectedDescriptor->getFormatterRegion()->setFocus(
						currentFocus == nextIndex);

				player = playerManager->getPlayer(selectedObject);
				changeSettingState("service.currentKeyMaster", "start");
				exitSelection(player);
				changeSettingState("service.currentKeyMaster", "stop");
				selectedObject = NULL;
				return false;
			}

		} else if (code == CodeMap::KEY_CURSOR_UP) {
			nextIndex = currentDescriptor->getMoveUp();

		} else if (code == CodeMap::KEY_CURSOR_DOWN) {
			nextIndex = currentDescriptor->getMoveDown();

		} else if (code == CodeMap::KEY_CURSOR_LEFT) {
			nextIndex = currentDescriptor->getMoveLeft();

		} else if (code == CodeMap::KEY_CURSOR_RIGHT) {
			nextIndex = currentDescriptor->getMoveRight();

		} else if (code == CodeMap::KEY_ENTER || code == CodeMap::KEY_OK) {
			userEvent->setKeyCode(CodeMap::KEY_NULL);
			if (currentDescriptor->
				    getFormatterRegion()->setSelection(true)) {

				if (selectedObject != NULL) {
					selectedObject->getDescriptor()->
						    getFormatterRegion()->setSelection(false);

					recoveryDefaultState(selectedObject);
				}

				selectedObject = currentObject;
				player = playerManager->getPlayer(selectedObject);

				changeSettingState("service.currentKeyMaster", "start");
				enterSelection(player);
				changeSettingState("service.currentKeyMaster", "stop");

				selectedObject->select(
						CodeMap::KEY_NULL, player->getMediaTime());

			} else {
				cout << "FormatterFocusManager::userEventReceived cant ";
				cout << " select '" << currentFocus << "'" << endl;
			}

			return false;
		}

		changeSettingState("service.currentFocus", "start");
		setFocus(nextIndex);
		changeSettingState("service.currentFocus", "stop");
		return true;
	}
}
}
}
}
}
}
