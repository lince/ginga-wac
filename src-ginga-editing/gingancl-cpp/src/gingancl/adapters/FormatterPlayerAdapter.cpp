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

#include "../../include/FormatterPlayerAdapter.h"

#include "../../include/ProceduralPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::procedural;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
	FormatterPlayerAdapter::FormatterPlayerAdapter() {
		typeSet.insert("FormatterPlayerAdapter");
		object = NULL;
		player = NULL;
		anchorMonitor = NULL;
		mrl = "";
		pthread_mutex_init(&objectMutex, NULL);
	}

	FormatterPlayerAdapter::~FormatterPlayerAdapter() {
		lockObject();
		if (anchorMonitor != NULL) {
			anchorMonitor->stopMonitor();
			delete anchorMonitor;
			anchorMonitor = NULL;
		}

		if (object != NULL) {
			object = NULL;
		}

		if (player != NULL) {
			delete player;
			player = NULL;
		}

		unlockObject();
		pthread_mutex_destroy(&objectMutex);
	}

	bool FormatterPlayerAdapter::instanceOf(string s) {
		if (!typeSet.empty()) {
			return (typeSet.find(s) != typeSet.end());
		} else {
			return false;
		}
	}

	void FormatterPlayerAdapter::createPlayer() {
		vector<FormatterEvent*>* events;
		PropertyAnchor* property;

		if (player == NULL) {
			player = new Player("");
		}

		player->addListener(this);

		if (object == NULL) {
			return;
		}

		events = object->getEvents();
		if (events != NULL) {
			vector<FormatterEvent*>::iterator i;
			i = events->begin();
			while (i != events->end()) {
				if (*i != NULL && (*i)->instanceOf("AttributionEvent")) {
					property = ((AttributionEvent*)*i)->getAnchor();
					player->setPropertyValue(
							property->getPropertyName(),
							property->getPropertyValue());

					((AttributionEvent*)(*i))->setValueMaintainer(this);
				}
				++i;
			}
			delete events;
			events = NULL;
		}
	}

	Surface* FormatterPlayerAdapter::getObjectDisplay() {
		if (player != NULL) {
			return player->getSurface();
		}
		return NULL;
	}

	bool FormatterPlayerAdapter::hasPrepared() {
		bool presented;

		if (object == NULL || player == NULL) {
			return false;
		}

		presented = player->hasPresented();
		return !presented;
	}

	void FormatterPlayerAdapter::setFocusHandler(bool isHandler) {
		player->setFocusHandler(isHandler);
	}

	bool FormatterPlayerAdapter::prepare(
			ExecutionObject* object, FormatterEvent* event) {

		Content* content;

		lockObject();
		if (object == NULL) {
			unlockObject();
			return false;
		}

		this->object = object;
		if (object->getDataObject() != NULL &&
				object->getDataObject()->getDataEntity() != NULL) {

			content = ((NodeEntity*)(object->getDataObject()->
					getDataEntity()))->getContent();

			if (content != NULL && content->instanceOf("ReferenceContent")) {
				this->mrl = ((ReferenceContent*)content)->
					    getCompleteReferenceUrl();
			} else {
				this->mrl = "";
			}
		}

		if (anchorMonitor != NULL) {
			anchorMonitor->stopMonitor();
			delete anchorMonitor;
			anchorMonitor = NULL;
		}

		if (player != NULL) {
			delete player;
			player = NULL;
		}

		createPlayer();
		if (event->getCurrentState() == EventUtil::ST_SLEEPING) {
			object->prepare((PresentationEvent*)event, 0);
			prepare();
			return true;

		} else {
			return false;
		}
	}

	void FormatterPlayerAdapter::prepare() {
		double duration;
		IntervalAnchor* intervalAnchor;
		PresentationEvent* mainEvent;

		mainEvent = (PresentationEvent*)(object->getMainEvent());
		if (mainEvent->instanceOf("PresentationEvent")) {
			if ((mainEvent->getAnchor())->instanceOf("LambdaAnchor")) {
				duration = mainEvent->getDuration();
				if (duration < IntervalAnchor::OBJECT_DURATION) {
					player->setScope(
							mainEvent->getAnchor()->getId(),
							PlayerListener::TYPE_PRESENTATION,
							0.0, duration / 1000);

				} else {
					player->setScope(mainEvent->getAnchor()->getId());
				}

			} else if (((mainEvent->getAnchor()))->instanceOf("IntervalAnchor")) {
				intervalAnchor = (IntervalAnchor*)(mainEvent->getAnchor());
				player->setScope(
						mainEvent->getAnchor()->getId(),
						PlayerListener::TYPE_PRESENTATION,
						(intervalAnchor->getBegin() / 1000),
						(intervalAnchor->getEnd() / 1000));
			}
		}

		if (player->immediatelyStart()) {
			player->setImmediatelyStart(false);
			start();
		}
	}

	bool FormatterPlayerAdapter::start() {
		EventTransition* nextTransition;
		CascadingDescriptor* descriptor;
		string paramValue;

		if (object != NULL) {
			descriptor = object->getDescriptor();
			if (descriptor != NULL) {
				paramValue = descriptor->getParameterValue("visible");
				if (paramValue == "false") {
					setVisible(false);

				} else if (paramValue == "true") {
					setVisible(true);
				}
			}
			player->play();
			if (object != NULL && !object->start()) {
				player->stop();

			} else if (object != NULL) {
				nextTransition = object->getNextTransition();
				if (nextTransition != NULL &&
						!isInfinity(nextTransition->getTime())) {

					anchorMonitor = new NominalEventMonitor(object, player);
				}

				InputManager::getInstance()->addInputEventListener(
						this, object->getInputEvents());

				if (anchorMonitor != NULL) {
					anchorMonitor->startMonitor();
				}
				return true;
			}
		}
		return false;
	}

	bool FormatterPlayerAdapter::stop() {
		FormatterEvent* mainEvent;
		vector<FormatterEvent*>* events = NULL;

		if (player == NULL || object == NULL) {
			InputManager::getInstance()->removeInputEventListener(this);
			unlockObject();
			return false;

		} else {
			mainEvent = object->getMainEvent();
			events = object->getEvents();
		}

		if (mainEvent != NULL && mainEvent->instanceOf("PresentationEvent")) {
			if (checkRepeat((PresentationEvent*)mainEvent)) {
				return false;
			}
		}

		InputManager::getInstance()->removeInputEventListener(this);
		if (events != NULL) {
			vector<FormatterEvent*>::iterator i;
			i = events->begin();
			while (i != events->end()) {
				if (*i != NULL && (*i)->instanceOf("AttributionEvent")) {
					((AttributionEvent*)(*i))->setValueMaintainer(NULL);
				}

				if (i != events->end()) {
					++i;
				}
			}
			delete events;
			events = NULL;
		}

		player->stop();

		if (anchorMonitor != NULL) {
			anchorMonitor->stopMonitor();
		}

		player->notifyReferPlayers(EventUtil::TR_STOPS);
		if (player->isForcedNaturalEnd()) {
			unprepare();
			return true;

		} else if (object != NULL) {
			object->stop();
			unprepare();
			return true;
		}
		return false;
	}

	bool FormatterPlayerAdapter::pause() {
		if (object != NULL && player != NULL && object->pause()) {
			player->pause();
			if (anchorMonitor != NULL) {
				anchorMonitor->pauseMonitor();
			}

			player->notifyReferPlayers(EventUtil::TR_PAUSES);
			return true;

		} else {
			return false;
		}
	}

	bool FormatterPlayerAdapter::resume() {
		if (object != NULL && player != NULL && object->resume()) {
			player->resume();
			if (anchorMonitor != NULL) {
				anchorMonitor->resumeMonitor();
			}

			player->notifyReferPlayers(EventUtil::TR_RESUMES);
			return true;
		}
		return false;
	}

	bool FormatterPlayerAdapter::abort() {
		InputManager::getInstance()->removeInputEventListener(this);
		if (player != NULL) {
			player->stop();
		}

		if (anchorMonitor != NULL) {
			anchorMonitor->stopMonitor();
			delete anchorMonitor;
			anchorMonitor = NULL;
		}

		if (player != NULL) {
			player->notifyReferPlayers(EventUtil::TR_ABORTS);
		}

		if (object != NULL) {
			if (object->abort()) {
				unprepare();
				return true;
			}
		}

		if (player != NULL) {
			player->stop();
		}

		unlockObject();
		return false;
	}

	void FormatterPlayerAdapter::naturalEnd() {
		bool freeze;
		PresentationEvent* mainEvent;

		if (object == NULL || player == NULL) {
			unlockObject();
			return;
		}

		// if freeze is true the natural end is not performed 
		if (object->getDescriptor() != NULL) {
			freeze = object->getDescriptor()->getFreeze();
			if (freeze) {
				return;
			}
		}

		stop();
	}

	bool FormatterPlayerAdapter::checkRepeat(PresentationEvent* event) {
		if (event->getRepetitions() > 1) {
			if (anchorMonitor != NULL) {
				anchorMonitor->stopMonitor();
				delete anchorMonitor;
				anchorMonitor = NULL;
			}

			player->stop();
			player->notifyReferPlayers(EventUtil::TR_STOPS);

			if (object != NULL) {
				object->stop();
			}

			player->setImmediatelyStart(true);
			prepare();
			return true;
		}

		return false;
	}

	bool FormatterPlayerAdapter::unprepare() {
		if (object != NULL && object->getMainEvent() != NULL && (
				object->getMainEvent()->getCurrentState() ==
					EventUtil::ST_OCCURRING ||
			    object->getMainEvent()->getCurrentState() ==
			    	EventUtil::ST_PAUSED)) {

			return stop();
		}

		if (anchorMonitor != NULL) {
			delete anchorMonitor;
			anchorMonitor = NULL;
		}

		if (object != NULL) {
			object->unprepare();
			object = NULL;
		}

		unlockObject();
		return true;
	}

	bool FormatterPlayerAdapter::setPropertyValue(
		    AttributionEvent* event, string value, Animation* anim) {

		if (player == NULL || object == NULL) {
			cout << "FormatterPlayerAdapter::setPropertyValue Warning!";
			cout << " cant set property '" << event->getId();
			cout << "' value = '" << value << "' object = '";
			if (object != NULL) {
				cout << object->getId();
			} else {
				cout << object;
			}
			cout << "' player = '";
			cout << player << "'" << endl;

			event->stop();
	  		return false;
	  	}

		//cout << "FormatterPlayerAdapter::setPropertyValue" << endl;
		string attName;
		string strValue;

		int targetValue;
		int currentValue;
		double time, initTime;
		int factor;

		attName = (event->getAnchor())->getPropertyName();
		if (attName == "visible") {
			if (value == "false") {
				setVisible(false);

			} else if (value == "true") {
				setVisible(true);
			}
			event->stop();
			return true;

		} else if (!object->setPropertyValue(event, value, anim)) {
			if (this->instanceOf("ProceduralPlayerAdapter")) {
				((ProceduralPlayerAdapter*)this)->setCurrentEvent(event);
			}

			if (anim != NULL) {
				player->setPropertyValue(
						attName, value,
						stof(anim->getDuration()), stof(anim->getBy()));

			} else {
				player->setPropertyValue(attName, value);
			}

			if (!this->instanceOf("ProceduralPlayerAdapter")) {
				event->stop();
			}
			return true;

		} else {
			return true;
		}
	}

	string FormatterPlayerAdapter::getPropertyValue(AttributionEvent* event) {
		string value= "";
		string attName;

		if (player == NULL || event == NULL) {
			return "";
		}

		attName = event->getAnchor()->getPropertyName();
		return player->getPropertyValue(attName);
	}

	double FormatterPlayerAdapter::getObjectExpectedDuration() {
		return IntervalAnchor::OBJECT_DURATION;
	}

	void FormatterPlayerAdapter::updateObjectExpectedDuration() {
		PresentationEvent* wholeContentEvent;
		double duration;
		double implicitDur;

		wholeContentEvent = object->getWholeContentPresentationEvent();
		duration = wholeContentEvent->getDuration();
		if ((object->getDescriptor() == NULL) ||
				(isNaN((object->getDescriptor())->getExplicitDuration()))
				|| (duration < 0)
				|| (isNaN(duration))) {

			implicitDur = getObjectExpectedDuration();
			((IntervalAnchor*)wholeContentEvent->getAnchor())->setEnd(
				    implicitDur);

			wholeContentEvent->setDuration(implicitDur);
		}
	}

	double FormatterPlayerAdapter::getMediaTime() {
		if (player != NULL) {
			return player->getMediaTime();
		}
		return 0;
	}

	Player* FormatterPlayerAdapter::getPlayer() {
		return player;
	}

	void FormatterPlayerAdapter::setTimeBasePlayer(
		    FormatterPlayerAdapter* timeBasePlayerAdapter) {

		Player* timePlayer;
		timePlayer = timeBasePlayerAdapter->getPlayer();
		if (timePlayer != NULL) {
			player->setReferenceTimePlayer(timePlayer);
			player->setTimeBasePlayer(timePlayer);
		}
	}

	void FormatterPlayerAdapter::updateStatus(
			short code, string parameter, short type) {

		switch(code) {
			case PL_NOTIFY_STOP:
				if (object != NULL &&
						type == PlayerListener::TYPE_PRESENTATION) {

					if (parameter == "") {
						naturalEnd();
					}
				}
				break;
		}
	}

	bool FormatterPlayerAdapter::userEventReceived(InputEvent* ev) {
		if (object != NULL && player != NULL) {
			object->select(
				ev->getKeyCode(), ((Player*)player)->getMediaTime() * 1000);
		}
		return true;
	}

	void FormatterPlayerAdapter::setVisible(bool visible) {
		CascadingDescriptor* descriptor;
		FormatterRegion* region;

  		descriptor = object->getDescriptor();
  		if (descriptor != NULL) {
			region = descriptor->getFormatterRegion();
			if (region != NULL && region->isVisible() != visible) {
				region->setRegionVisibility(visible);
				player->setVisible(visible);
			}
  		}
	}

	void FormatterPlayerAdapter::lockObject() {
		pthread_mutex_lock(&objectMutex);
	}

	void FormatterPlayerAdapter::unlockObject() {
		pthread_mutex_unlock(&objectMutex);
	}
}
}
}
}
}
}
