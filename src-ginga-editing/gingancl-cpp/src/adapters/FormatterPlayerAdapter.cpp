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

#include "../../include/adapters/FormatterPlayerAdapter.h"

#include "../../include/adapters/procedural/ProceduralPlayerAdapter.h"
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

		im = NULL;
		cm = IComponentManager::getCMInstance();

		if (cm != NULL) {
			im = ((InputManagerCreator*)(cm->getObject("InputManager")))();
		}

		isLocked = false;
		pthread_mutex_init(&objectMutex, NULL);
	}

	FormatterPlayerAdapter::~FormatterPlayerAdapter() {
		if (anchorMonitor != NULL) {
			anchorMonitor->stopMonitor();
			delete anchorMonitor;
			anchorMonitor = NULL;
		}

		if (object != NULL) {
			object = NULL;
		}

		if (player != NULL) {
			player->stop();
			delete player;
			player = NULL;
		}

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

		if (player == NULL && cm != NULL) {
			player = ((PlayerCreator*)(cm->getObject("Player")))(
					NULL, false);
		}

		if (player == NULL) {
			return;
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

	ISurface* FormatterPlayerAdapter::getObjectDisplay() {
		if (player != NULL) {
			return player->getSurface();
		}

		cout << "FormatterPlayerAdapter::getObjectDisplay Warning! player";
		cout << "==NULL for '";
		if (object != NULL) {
			cout << object->getId() << "'" << endl;
		} else {
			cout << "'NULL' object" << endl;
		}
		return NULL;
	}

	bool FormatterPlayerAdapter::hasPrepared() {
		bool presented;
		FormatterEvent* mEv;
		short st;

		if (object == NULL || player == NULL) {
			return false;
		}

		presented = player->hasPresented();
		if (presented) {
			return false;
		}

		presented = player->isForcedNaturalEnd();
		if (presented) {
			return false;
		}

		mEv = object->getMainEvent();
		if (mEv != NULL && !object->instanceOf("ProceduralExecutionObject")) {
			st = mEv->getCurrentState();
			if (st != EventUtil::ST_SLEEPING) {
				return true;
			}
		}

		return isLocked;
	}

	bool FormatterPlayerAdapter::setFocusHandler(bool isHandler) {
		return (player->setFocusHandler(isHandler) && isHandler);
	}

	bool FormatterPlayerAdapter::prepare(
			ExecutionObject* object, FormatterEvent* event) {

		Content* content;

		if (hasPrepared()) {
			cout << "FormatterPlayerAdapter::prepare returns false, ";
			cout << "because player is already prepared" << endl;
			return false;
		}

		if (!lockObject()) {
			if (player != NULL) {
				if (player->hasPresented() || player->isForcedNaturalEnd()) {
					while (!lockObject());

				} else {
					cout << "FormatterPlayerAdapter::prepare returns false, ";
					cout << "because object is locked" << endl;
					return false;
				}
			}
		}

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

		if (object == NULL) {
			return;
		}

		mainEvent = (PresentationEvent*)(object->getMainEvent());
		if (mainEvent->instanceOf("PresentationEvent")) {
			if ((mainEvent->getAnchor())->instanceOf("LambdaAnchor")) {
				duration = mainEvent->getDuration();
				if (duration < IntervalAnchor::OBJECT_DURATION) {
					player->setScope(
							mainEvent->getAnchor()->getId(),
							IPlayer::TYPE_PRESENTATION,
							0.0, duration / 1000);

				} else {
					player->setScope(
							mainEvent->getAnchor()->getId(),
							IPlayer::TYPE_PRESENTATION);
				}

			} else if (((mainEvent->getAnchor()))->instanceOf("IntervalAnchor")) {
				intervalAnchor = (IntervalAnchor*)(mainEvent->getAnchor());
				player->setScope(
						mainEvent->getAnchor()->getId(),
						IPlayer::TYPE_PRESENTATION,
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
			/*cout << "FormatterPlayerAdapter::start(" << object->getId();
			cout << ")" << endl;*/
			descriptor = object->getDescriptor();
			if (descriptor != NULL) {
				paramValue = descriptor->getParameterValue("visible");
				if (paramValue == "false") {
					setVisible(false);

				} else if (paramValue == "true") {
					setVisible(true);
				}
			}
			if (player != NULL) {
				player->play();
			}

			if (object != NULL && !object->start() && player != NULL) {
				player->stop();

			} else if (object != NULL) {
				nextTransition = object->getNextTransition();
				if (nextTransition != NULL &&
						!isInfinity(nextTransition->getTime())) {

					anchorMonitor = new NominalEventMonitor(object, player);
				}

				if (im != NULL) {
					im->addInputEventListener(this, object->getInputEvents());
				}

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
			if (im != NULL) {
				im->removeInputEventListener(this);
			}
			unlockObject();
			return false;

		} else {
			mainEvent = object->getMainEvent();
			events = object->getEvents();
		}

		if (mainEvent != NULL && mainEvent->instanceOf("PresentationEvent")) {
			if (checkRepeat((PresentationEvent*)mainEvent)) {
				return true;
			}
		}

		/*cout << "FormatterPlayerAdapter::stop(" << object->getId();
		cout << ")" << endl;*/

		if (im != NULL) {
			im->removeInputEventListener(this);
		}

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

		if (anchorMonitor != NULL) {
			anchorMonitor->stopMonitor();
		}

		if (player != NULL) {
			player->stop();
			player->notifyReferPlayers(EventUtil::TR_STOPS);
		}

		if (player != NULL && player->isForcedNaturalEnd()) {
			return unprepare();

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
		if (im != NULL) {
			im->removeInputEventListener(this);
		}

		if (anchorMonitor != NULL) {
			anchorMonitor->stopMonitor();
		}

		if (player != NULL) {
			player->stop();
			player->notifyReferPlayers(EventUtil::TR_ABORTS);
		}

		if (object != NULL) {
			if (object->abort()) {
				unprepare();
				return true;
			}
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
			//cout << endl << "FormatterPlayerAdapter::checkRepeat";
			if (anchorMonitor != NULL) {
				anchorMonitor->stopMonitor();
				delete anchorMonitor;
				anchorMonitor = NULL;
			}

			player->stop();
			player->notifyReferPlayers(EventUtil::TR_STOPS);

			if (object != NULL) {
				//cout << "'" << object->getId() << "'";
				object->stop();
			}
			//cout << endl << endl;

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

	IPlayer* FormatterPlayerAdapter::getPlayer() {
		return player;
	}

	void FormatterPlayerAdapter::setTimeBasePlayer(
		    FormatterPlayerAdapter* timeBasePlayerAdapter) {

		IPlayer* timePlayer;
		timePlayer = timeBasePlayerAdapter->getPlayer();
		if (timePlayer != NULL) {
			player->setReferenceTimePlayer(timePlayer);
			player->setTimeBasePlayer(timePlayer);
		}
	}

	void FormatterPlayerAdapter::updateStatus(
			short code, string parameter, short type) {

		switch(code) {
			case IPlayer::PL_NOTIFY_STOP:
				if (object != NULL &&
						type == IPlayer::TYPE_PRESENTATION) {

					if (parameter == "") {
						naturalEnd();
					}
				}
				break;
		}
	}

	bool FormatterPlayerAdapter::userEventReceived(IInputEvent* ev) {
		if (object != NULL && player != NULL) {
			object->select(
				ev->getKeyCode(), player->getMediaTime() * 1000);
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

	bool FormatterPlayerAdapter::lockObject() {
		if (isLocked) {
			return false;
		}
		isLocked = true;
		pthread_mutex_lock(&objectMutex);
		return true;
	}

	bool FormatterPlayerAdapter::unlockObject() {
		if (!isLocked) {
			return false;
		}
		pthread_mutex_unlock(&objectMutex);
		isLocked = false;
		return true;
	}

	//Modificações Caio
	PlayerState* FormatterPlayerAdapter::getPlayerState() {
		if (player!= NULL) {
			return player->getPlayerState();
		} else {
			return NULL;
		}
	}
	//Modificações Caio
}
}
}
}
}
}
