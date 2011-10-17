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

#include "../../include/focus/FormatterFocusManager.h"

#include "../../include/emconverter/FormatterConverter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::emconverter;

#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace focus {
	FormatterFocusManager::FormatterFocusManager(
		    PlayerAdapterManager* playerManager,
		    void* converter) {

		string strValue;

		IComponentManager* cm = IComponentManager::getCMInstance();
		im = NULL;

		if (cm != NULL) {
			im = ((InputManagerCreator*)(cm->getObject("InputManager")))();
		}

		focusTable = new map<string, set<ExecutionObject*>*>;
		focusSequence = new vector<string>;
		currentFocus = "";

		strValue = PresentationContext::getInstance()->getPropertyValue(
				ContextBase::DEFAULT_FOCUS_BORDER_COLOR);

		if (strValue == "") {
			defaultFocusBorderColor = new Color("blue");

		} else {
			defaultFocusBorderColor = new Color(strValue);
		}

		strValue = PresentationContext::getInstance()->getPropertyValue(
				ContextBase::DEFAULT_FOCUS_BORDER_WIDTH);

		if (strValue == "") {
			defaultFocusBorderWidth = -3;

		} else {
			defaultFocusBorderWidth = stof(strValue);
		}

		strValue = PresentationContext::getInstance()->getPropertyValue(
				ContextBase::DEFAULT_SEL_BORDER_COLOR);

		if (strValue == "") {
			defaultSelBorderColor = new Color("green");

		} else {
			defaultSelBorderColor = new Color(strValue);
		}

		this->selectedObject = NULL;
		this->playerManager = playerManager;
		this->converter = converter;

		pthread_mutex_init(&mutexFocus, NULL);
		pthread_mutex_init(&mutexTable, NULL);
	}

	FormatterFocusManager::~FormatterFocusManager() {
		pthread_mutex_lock(&mutexFocus);
		pthread_mutex_lock(&mutexTable);
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
		pthread_mutex_unlock(&mutexFocus);
		pthread_mutex_destroy(&mutexFocus);
		pthread_mutex_unlock(&mutexTable);
		pthread_mutex_destroy(&mutexTable);
	}

	ExecutionObject* FormatterFocusManager::getObject(string focusIndex) {
		map<string, set<ExecutionObject*>*>::iterator i;
		set<ExecutionObject*>::iterator j;
		CascadingDescriptor* desc;
		bool visible;

		i = focusTable->find(focusIndex);
		if (i == focusTable->end()) {
			return NULL;
		}

		j = i->second->begin();
		while (j != i->second->end()) {
			desc = (*j)->getDescriptor();
			if (desc != NULL && desc->getFormatterRegion() != NULL) {
				visible = desc->getFormatterRegion()->isVisible();
				if (visible) {
					return *j;
				}
			}
			++j;
		}

		delete i->second;
		focusTable->erase(i);

		return NULL;
	}

	void FormatterFocusManager::insertObject(
			ExecutionObject* obj, string focusIndex) {

		string auxIndex;
		map<string, set<ExecutionObject*>*>::iterator i;
		vector<string>::iterator j;
		set<ExecutionObject*>* objs;

		i = focusTable->find(focusIndex);
		if (i != focusTable->end()) {
			objs = i->second;

		} else {
			objs = new set<ExecutionObject*>;
			(*focusTable)[focusIndex] = objs;
		}

		objs->insert(obj);

		// put new index in alphabetical order
		for (j = focusSequence->begin(); j != focusSequence->end(); ++j) {
			auxIndex = *j;
			if (focusIndex.compare(auxIndex) < 0) {
				break;

			} else if (focusIndex.compare(auxIndex) == 0) {
				return;
			}
		}

		focusSequence->insert(j, focusIndex);
	}

	void FormatterFocusManager::removeObject(
			ExecutionObject* obj, string focusIndex) {

		map<string, set<ExecutionObject*>*>::iterator i;
		set<ExecutionObject*>::iterator j;
		vector<string>::iterator k;
		set<ExecutionObject*>* objs;

		i = focusTable->find(focusIndex);
		if (i != focusTable->end()) {
			objs = i->second;
			j = objs->find(obj);
			if (j != objs->end()) {
				objs->erase(j);
			}
			if (objs->empty()) {
				delete objs;
				focusTable->erase(i);
			}
		}

		k = focusSequence->begin();
		while (k != focusSequence->end()) {
			if (*k == focusIndex) {
				focusSequence->erase(k);
				k = focusSequence->begin();

			} else {
				++k;
			}
		}
	}

	void FormatterFocusManager::setKeyMaster(string focusIndex) {
		ExecutionObject* nextObject = NULL;
		ExecutionObject* currentObject = NULL;
		CascadingDescriptor* currentDescriptor = NULL;
		CascadingDescriptor* nextDescriptor = NULL;
		FormatterPlayerAdapter* player = NULL;

		pthread_mutex_lock(&mutexTable);
		pthread_mutex_lock(&mutexFocus);

		nextObject = getObject(focusIndex);
		if (nextObject == NULL) {
			cout << "FormatterFocusManager::setKeyMaster can't set '";
			cout << focusIndex << "' as master object is not available.";
			cout << endl;
			pthread_mutex_unlock(&mutexTable);
			pthread_mutex_unlock(&mutexFocus);
			return;
		}

		nextDescriptor = nextObject->getDescriptor();
		if (nextDescriptor == NULL ||
				nextDescriptor->getFormatterRegion() == NULL) {

			cout << "FormatterFocusManager::setKeyMaster can't set '";
			cout << focusIndex << "' as master. Descriptor is NULL." << endl;
			pthread_mutex_unlock(&mutexTable);
			pthread_mutex_unlock(&mutexFocus);
			return;
		}

		currentObject = getObject(currentFocus);
		if (currentObject != NULL && currentObject != nextObject) {
			pthread_mutex_unlock(&mutexTable);
			//recovering focused object
			currentDescriptor = currentObject->getDescriptor();
			currentDescriptor->getFormatterRegion()->setFocus(false);
			recoveryDefaultState(currentObject);

		} else {
			pthread_mutex_unlock(&mutexTable);
		}

		currentFocus = focusIndex;

		if (nextDescriptor->getFormatterRegion()->setSelection(true)) {
			//recovering old selected object
			if (selectedObject != NULL && selectedObject != nextObject) {
				selectedObject->getDescriptor()->
						getFormatterRegion()->setSelection(false);

				recoveryDefaultState(selectedObject);
			}

			//selecting new object
			selectedObject = nextObject;
			player = playerManager->getPlayer(selectedObject);

			enterSelection(player);
			nextObject->select(
					CodeMap::KEY_NULL, player->getMediaTime());

		} else {
			cout << "FormatterFocusManager::setCurrentKeyMaster cant ";
			cout << " select '" << currentFocus << "'" << endl;
		}
		pthread_mutex_unlock(&mutexFocus);
	}

	void FormatterFocusManager::setFocus(string focusIndex) {
		ExecutionObject* nextObject = NULL;
		ExecutionObject* currentObject = NULL;
		CascadingDescriptor* currentDescriptor = NULL;

		if (focusIndex == currentFocus && currentFocus != "") {
			cout << "FormatterFocusManager::setFocus unnecessary currentFocus";
			cout << " = '" << currentFocus << "' focusIndex = '" << focusIndex;
			cout << "'" << endl;
			return;
		}

		pthread_mutex_lock(&mutexTable);
		pthread_mutex_lock(&mutexFocus);

		nextObject = getObject(focusIndex);
		if (nextObject == NULL) {

			cout << "FormatterFocusManager::setFocus Warning! index '";
			cout << focusIndex << "' is not visible." << endl;

			pthread_mutex_unlock(&mutexFocus);
			pthread_mutex_unlock(&mutexTable);
			return;
		}

		currentObject = getObject(currentFocus);
		if (currentObject != NULL) {
			pthread_mutex_unlock(&mutexTable);
			currentDescriptor = currentObject->getDescriptor();

		} else {
			currentDescriptor = NULL;
			cout << "FormatterFocusManager::setFocus index '";
			cout << focusIndex << "' is not in focus Table." << endl;
			pthread_mutex_unlock(&mutexTable);
		}

		currentFocus = focusIndex;
		PresentationContext::getInstance()->setPropertyValue(
				"service.currentFocus", currentFocus);

		if (currentDescriptor != NULL) {
			recoveryDefaultState(currentObject);
			currentDescriptor->getFormatterRegion()->setFocus(false);

		} else if (currentObject != NULL) {
			cout << "FormatterFocusManager::setFocus Warning can't recover '";
			cout << currentObject->getId() << "'" << endl;
		}

		setFocus(nextObject->getDescriptor());

		pthread_mutex_unlock(&mutexFocus);
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
			ISurface* renderedSurface;
			renderedSurface = player->getObjectDisplay();
			if (renderedSurface != NULL) {
				object->getDescriptor()->getFormatterRegion()->
						renderSurface(renderedSurface);

			} else {
				cout << "FormatterFocusManager::recoveryDefaultState ";
				cout << "Warning! null renderedsurface for '";
				cout << object->getId() << "'" << endl;
			}
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

		if (object == NULL) {
			cout << "FormatterFocusManager::showObject Warning! object ";
			cout << "is null." << endl;
			return;
		}

		descriptor = object->getDescriptor();
		if (descriptor == NULL) {
			cout << "FormatterFocusManager::showObject Warning! ";
			cout << " descriptor is null." << endl;
			return;
		}

		focusIndex = descriptor->getFocusIndex();
		if (focusIndex == "") {
			return;
		}

		pthread_mutex_lock(&mutexTable);
		insertObject(object, focusIndex);
		pthread_mutex_unlock(&mutexTable);

		if (currentFocus == "") {
			evs = new set<int>;
			evs->insert(CodeMap::KEY_CURSOR_DOWN);
			evs->insert(CodeMap::KEY_CURSOR_LEFT);
			evs->insert(CodeMap::KEY_CURSOR_RIGHT);
			evs->insert(CodeMap::KEY_CURSOR_UP);

			evs->insert(CodeMap::KEY_ENTER);
			evs->insert(CodeMap::KEY_OK);

			if (im != NULL) {
				im->addInputEventListener(this, evs);
			}

			paramValue = PresentationContext::getInstance()->getPropertyValue(
					"service.currentKeyMaster");

			if (paramValue == focusIndex) {
				setKeyMaster(paramValue);

			} else {
				setFocus(focusIndex);
			}

		} else {
			paramValue = PresentationContext::getInstance()->
					getPropertyValue("service.currentFocus");

			if (paramValue != "" &&
				    paramValue == focusIndex &&
				    descriptor->getFormatterRegion()->isVisible()) {

				/*if (focusTable->count(currentFocus) != 0) {
					currentObject = (*focusTable)[currentFocus];
					currentObject->getDescriptor()->
						    getFormatterRegion()->setFocus(false);

					recoveryDefaultState(currentObject);
				}

			 	currentFocus = focusIndex;*/
			 	setFocus(focusIndex);
			}

			//service.currentKeyMaster
			paramValue = PresentationContext::getInstance()->getPropertyValue(
					"service.currentKeyMaster");

			if (paramValue == focusIndex &&
					descriptor->getFormatterRegion()->isVisible()) {

				// first set as current focus
				/*if (focusTable->count(currentFocus) != 0) {
					currentObject = (*focusTable)[currentFocus];
					currentObject->getDescriptor()->
						    getFormatterRegion()->setFocus(false);

					recoveryDefaultState(currentObject);
				}

				currentFocus = focusIndex;
				PresentationContext::getInstance()->setPropertyValue(
						"service.currentFocus", currentFocus);*/

				setFocus(focusIndex);

				// then set as selected
				if (descriptor->getFormatterRegion()->setSelection(true)) {
					// unselect the previous selected object, if exists
					/*if (selectedObject != NULL) {
						selectedObject->getDescriptor()->
							    getFormatterRegion()->setSelection(false);

						recoveryDefaultState(selectedObject);
					}*/

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
		map<string, set<ExecutionObject*>*>::iterator i;

		if (object == NULL || object->getDescriptor() == NULL ||
				object->getDescriptor()->getFormatterRegion() == NULL) {

			return;
		}

		region = object->getDescriptor()->getFormatterRegion();
		focusIndex = object->getDescriptor()->getFocusIndex();
		if (focusIndex != "") {
			pthread_mutex_lock(&mutexTable);
			removeObject(object, focusIndex);
			pthread_mutex_unlock(&mutexTable);

			if (region->getFocusState() == FormatterRegion::SELECTED &&
					selectedObject == object) {

				player = playerManager->getPlayer(selectedObject);
				exitSelection(player);
				//region->setSelection(false);
				//recoveryDefaultState(selectedObject);
				selectedObject = NULL;
			}

			if (currentFocus == focusIndex) {
				//region->setFocus(false);
				//recoveryDefaultState(object);

				pthread_mutex_lock(&mutexTable);
				if (focusSequence->empty()) {
					pthread_mutex_unlock(&mutexTable);
					currentFocus = "";
					if (im != NULL) {
						im->removeInputEventListener(this);
					}

				} else {
					string ix = *(focusSequence->begin());
					i = focusTable->find(ix);
					while (i == focusTable->end()) {
						focusSequence->erase(focusSequence->begin());
						if (focusSequence->empty()) {
							pthread_mutex_unlock(&mutexTable);
							currentFocus = "";
							if (im != NULL) {
								im->removeInputEventListener(this);
							}
							return;
						}
						ix = *(focusSequence->begin());
						i = focusTable->find(ix);
					}
					pthread_mutex_unlock(&mutexTable);
					setFocus(ix);
				}
			}
		}
	}

	bool FormatterFocusManager::keyCodeOk(ExecutionObject* currentObject) {
		FormatterPlayerAdapter* player;
		bool isHandling = false;

		if (currentObject != selectedObject) {
			if (selectedObject != NULL) {
				cout << "FormatterFocusManager::keyCodeOk Warning! ";
				cout << "selecting an object with another selected." << endl;
			}
		}

		selectedObject = currentObject;
		player = playerManager->getPlayer(selectedObject);

		changeSettingState("service.currentKeyMaster", "start");
		isHandling = enterSelection(player);

		selectedObject->select(
				CodeMap::KEY_NULL, player->getMediaTime());

		changeSettingState("service.currentKeyMaster", "stop");

		return isHandling;
	}

	bool FormatterFocusManager::keyCodeBack() {
		CascadingDescriptor* selectedDescriptor;
		FormatterRegion* fr;
		FormatterPlayerAdapter* player;
		string ix;

		if (selectedObject == NULL) {
			return false;
		}

		selectedDescriptor = selectedObject->getDescriptor();
		if (selectedDescriptor == NULL) {
			return false;
		}

		ix = selectedDescriptor->getFocusIndex();
		fr = selectedDescriptor->getFormatterRegion();
		if (fr == NULL) {
			return false;
		}

		fr->setSelection(false);

		pthread_mutex_lock(&mutexFocus);
		if (ix == currentFocus) {
			fr->setFocus(true);
		}

		player = playerManager->getPlayer(selectedObject);
		changeSettingState("service.currentKeyMaster", "start");
		exitSelection(player);
		changeSettingState("service.currentKeyMaster", "stop");
		selectedObject = NULL;
		pthread_mutex_unlock(&mutexFocus);

		return false;
	}

	bool FormatterFocusManager::enterSelection(
			FormatterPlayerAdapter* player) {

		set<int>* iEvs;

		iEvs = new set<int>;
		iEvs->insert(CodeMap::KEY_BACKSPACE);
		iEvs->insert(CodeMap::KEY_BACK);
		if (im != NULL) {
			im->addInputEventListener(this, iEvs);
		}

		PresentationContext::getInstance()->setPropertyValue(
				"service.currentKeyMaster", currentFocus);

		return player->setFocusHandler(true);
	}

	void FormatterFocusManager::exitSelection(FormatterPlayerAdapter* player) {
		set<int>* evs;

		if (im != NULL) {
			im->removeInputEventListener(this);
		}
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

		if (im != NULL) {
			im->addInputEventListener(this, evs);
		}
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

		/*cout << "FormatterFocusManager::changeSettingState number of ";
		cout << "settings objects: '" << settingObjects->size() << "'";
		cout << endl;*/

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

	bool FormatterFocusManager::userEventReceived(IInputEvent* userEvent) {
		ExecutionObject* currentObject;
		CascadingDescriptor* currentDescriptor;
		FormatterRegion* fr;
		string nextIndex;
		map<string, set<ExecutionObject*>*>::iterator i;

		const int code = userEvent->getKeyCode();

		pthread_mutex_lock(&mutexTable);
		i = focusTable->find(currentFocus);
		if (i == focusTable->end()) {
			if (currentFocus != "") {
				cout << "FormatterFocusManager::userEventReceived ";
				cout << "currentFocus not found which is '" << currentFocus;
				cout << "'" << endl;
			}

			if (!focusSequence->empty()) {
				nextIndex = (*focusSequence)[0];
				pthread_mutex_unlock(&mutexTable);
				setFocus(nextIndex);

			} else {
				pthread_mutex_unlock(&mutexTable);
			}
			return true;
		}

		currentObject = getObject(currentFocus);
		if (currentObject == NULL) {
			cout << "FormatterFocusManager::userEventReceived ";
			cout << "Warning! object == NULL" << endl;
			cout << "'" << endl;

			pthread_mutex_unlock(&mutexTable);
			return true;
		}
		pthread_mutex_unlock(&mutexTable);

		currentDescriptor = currentObject->getDescriptor();
		if (currentDescriptor == NULL) {
			cout << "FormatterFocusManager::userEventReceived ";
			cout << "Warning! descriptor == NULL" << endl;
			cout << "'" << endl;
			return true;
		}

		nextIndex = "";
		if (selectedObject != NULL) {
			if (code == CodeMap::KEY_BACKSPACE || code == CodeMap::KEY_BACK) {
				return keyCodeBack();
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
			fr = currentDescriptor->getFormatterRegion();
			if (fr != NULL && fr->setSelection(true)) {
				if (!keyCodeOk(currentObject)) {
					if (im != NULL) {
						im->postEvent(CodeMap::KEY_BACK);
					}
				}

			} else {
				cout << "FormatterFocusManager::userEventReceived cant ";
				cout << " select '" << currentFocus << "'" << endl;
			}

			return false;
		}

		if (nextIndex != "") {
			changeSettingState("service.currentFocus", "start");
			setFocus(nextIndex);
			changeSettingState("service.currentFocus", "stop");
		}
		return true;
	}
}
}
}
}
}
}
