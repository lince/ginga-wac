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

#include "../../../include/ProceduralPlayerAdapter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace procedural {
	ProceduralPlayerAdapter::ProceduralPlayerAdapter() {
		typeSet.insert("ProceduralPlayerAdapter");
		pthread_mutex_init(&eventMutex, NULL);
		preparedEvents = new map<string, FormatterEvent*>;
		currentEvent = NULL;
		editingCommandListener = NULL;
	}

	ProceduralPlayerAdapter::~ProceduralPlayerAdapter() {
		lockEvent();
		if (preparedEvents != NULL) {
			delete preparedEvents;
			preparedEvents = NULL;
		}
		currentEvent = NULL;
		unlockEvent();
		pthread_mutex_destroy(&eventMutex);
	}

	void ProceduralPlayerAdapter::setNclEditListener(
			NclEditListener* listener) {

		this->editingCommandListener = listener;
	}

	bool ProceduralPlayerAdapter::prepare(
			ExecutionObject* object, FormatterEvent* event) {

		Content* content;

		lockEvent();
		if (object == NULL) {
			unlockEvent();
			return false;
		}

		if (this->object != object) {
			preparedEvents->clear();

			lockObject();
			this->object = object;
			unlockObject();

			if (this->object->getDataObject() != NULL &&
					this->object->getDataObject()->getDataEntity() != NULL) {

				content = ((NodeEntity*)(object->getDataObject()->
						getDataEntity()))->getContent();

				if (content != NULL && content->instanceOf(
						"ReferenceContent")) {

					this->mrl = ((ReferenceContent*)content)->
						    getCompleteReferenceUrl();
				} else {
					this->mrl = "";
				}
			}
			createPlayer();
		}

		if (event->getCurrentState() == EventUtil::ST_SLEEPING) {
			this->object->prepare(event, 0);
			unlockEvent();
			prepare(event);
			return true;
		}

		unlockEvent();
		return false;
	}

	void ProceduralPlayerAdapter::prepare(FormatterEvent* event) {
		double duration;
		IntervalAnchor* intervalAnchor;

		if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
			return;
		}

		lockEvent();
		if (event->instanceOf("AnchorEvent")) {
			if ((((AnchorEvent*)event)->getAnchor())->instanceOf(
					"LambdaAnchor")) {

				duration = ((PresentationEvent*)event)->getDuration();

				if (duration < IntervalAnchor::OBJECT_DURATION) {
					player->setScope(
							"",
							PlayerListener::TYPE_PRESENTATION,
							0.0, duration / 1000);
				}

			} else if (((((AnchorEvent*)event)->getAnchor()))->
					instanceOf("IntervalAnchor")) {

				intervalAnchor = (IntervalAnchor*)(
						((AnchorEvent*)event)->getAnchor());

				player->setScope(
						((AnchorEvent*)event)->getAnchor()->getId(),
						PlayerListener::TYPE_PRESENTATION,
						(intervalAnchor->getBegin() / 1000),
						(intervalAnchor->getEnd() / 1000));

			} else if (((((AnchorEvent*)event)->getAnchor()))->
					instanceOf("LabeledAnchor")) {

				player->setScope(((AnchorEvent*)event)->
						getAnchor()->getId());
			}
		}

		(*preparedEvents)[event->getId()] = event;
		unlockEvent();
	}

	bool ProceduralPlayerAdapter::start() {
		if (object->start()) {
			player->play();
			return true;
		}
		return false;
	}

	bool ProceduralPlayerAdapter::stop() {
		player->stop();
		player->notifyReferPlayers(EventUtil::TR_STOPS);
		if (player->isForcedNaturalEnd()) {
			map<string, FormatterEvent*>::iterator i;
			FormatterEvent* event;
			i = preparedEvents->begin();
			while (i != preparedEvents->end()) {
				event = i->second;
				if (event != NULL && event->instanceOf("AnchorEvent") &&
						((AnchorEvent*)event)->getAnchor() != NULL &&
						((AnchorEvent*)event)->getAnchor()->instanceOf(
								"LambdaAnchor")) {

					currentEvent = event;
					currentEvent->stop();
					unprepare();
					return true;
				}
				++i;
			}
		}

		if (object->stop()) {
			unprepare();
			return true;
		}

		cout << "ProceduralPlayerAdapter::stop() Warning! Should never ";
		cout << "reaches here!" << endl;
		return false;
	}

	bool ProceduralPlayerAdapter::pause() {
		if (object->pause()) {
			player->pause();
			player->notifyReferPlayers(EventUtil::TR_PAUSES);
			return true;

		} else {
			return false;
		}
	}

	bool ProceduralPlayerAdapter::resume() {
		if (object->resume()) {
			player->resume();
			player->notifyReferPlayers(EventUtil::TR_RESUMES);
			return true;
		}
		return false;
	}

	bool ProceduralPlayerAdapter::abort() {
		player->stop();
		player->notifyReferPlayers(EventUtil::TR_ABORTS);

		if (object != NULL) {
			if (object->abort()) {
				unprepare();
				return true;
			}
		}

		return false;
	}

	bool ProceduralPlayerAdapter::unprepare() {
		if (currentEvent->getCurrentState() == EventUtil::ST_OCCURRING ||
				currentEvent->getCurrentState() == EventUtil::ST_PAUSED) {

			return stop();
		}

		lockEvent();
		if (preparedEvents->count(currentEvent->getId()) != 0 &&
				preparedEvents->size() == 1) {

			if (anchorMonitor != NULL) {
				anchorMonitor->stopMonitor();
				delete anchorMonitor;
				anchorMonitor = NULL;
			}

			if (player != NULL) {
				delete player;
				player = NULL;
			}

			object->unprepare();
			preparedEvents->clear();
			object = NULL;

		} else {
			object->unprepare();
			preparedEvents->erase(preparedEvents->find(currentEvent->getId()));
		}
		unlockEvent();
		return true;
	}

	void ProceduralPlayerAdapter::naturalEnd() {
		if (player == NULL || object == NULL) {
			return;
		}

		player->notifyReferPlayers(EventUtil::TR_STOPS);
		map<string, FormatterEvent*>::iterator i;
		FormatterEvent* event;
		i = preparedEvents->begin();
		while (i != preparedEvents->end()) {
			event = i->second;
			if (event != NULL && event->instanceOf("AnchorEvent") &&
					((AnchorEvent*)event)->getAnchor() != NULL &&
					((AnchorEvent*)event)->getAnchor()->instanceOf(
							"LambdaAnchor")) {

				currentEvent = event;
				currentEvent->stop();
				unprepare();
				return;
			}
			++i;
		}

		if (object->stop()) {
			unprepare();
		}
	}

	void ProceduralPlayerAdapter::updateStatus(
			short code, string param, short type) {

		switch(code) {
			case PL_NOTIFY_START:
				if (object != NULL) {
					startEvent(param, type);
				}
				break;

			case PL_NOTIFY_PAUSE:
				if (object != NULL) {
					pauseEvent(param, type);
				}
				break;

			case PL_NOTIFY_RESUME:
				if (object != NULL) {
					resumeEvent(param, type);
				}
				break;

			case PL_NOTIFY_STOP:
				if (object != NULL) {
					if (param == "") {
						naturalEnd();
					} else {
						stopEvent(param, type);
					}
				}
				break;

			case PL_NOTIFY_ABORT:
				if (object != NULL) {
					abortEvent(param, type);
				}
				break;

			case PL_NOTIFY_NCLEDIT:
				if (editingCommandListener != NULL) {
					editingCommandListener->nclEdit(param);
				}
				break;
		}
	}

	bool ProceduralPlayerAdapter::startEvent(string anchorId, short type) {
		FormatterEvent* event;
		event = getEvent(anchorId);
		if (event != NULL) {
			if ((event->instanceOf("PresentationEvent") &&
					type == PlayerListener::TYPE_PRESENTATION) ||
					(event->instanceOf("AttributionEvent") &&
							type == PlayerListener::TYPE_ATTRIBUTION)) {

				return event->start();
			}
		}

		return false;
	}

	bool ProceduralPlayerAdapter::stopEvent(string anchorId, short type) {
		FormatterEvent* event;
		event = getEvent(anchorId);
		if (event != NULL) {
			if ((event->instanceOf("PresentationEvent") &&
					type == PlayerListener::TYPE_PRESENTATION) ||
					(event->instanceOf("AttributionEvent") &&
							type == PlayerListener::TYPE_ATTRIBUTION)) {

				return event->stop();
			}

		} else {
			cout << "ProceduralPlayerAdapter::stopEvent event not found";
			cout << endl;
		}

		return false;
	}

	bool ProceduralPlayerAdapter::abortEvent(string anchorId, short type) {
		FormatterEvent* event;
		event = getEvent(anchorId);
		if (event != NULL) {
			if ((event->instanceOf("PresentationEvent") &&
					type == PlayerListener::TYPE_PRESENTATION) ||
					(event->instanceOf("AttributionEvent") &&
							type == PlayerListener::TYPE_ATTRIBUTION)) {

				return event->abort();
			}
		}

		return false;
	}

	bool ProceduralPlayerAdapter::pauseEvent(string anchorId, short type) {
		FormatterEvent* event;
		event = getEvent(anchorId);
		if (event != NULL) {
			if ((event->instanceOf("PresentationEvent") &&
					type == PlayerListener::TYPE_PRESENTATION) ||
					(event->instanceOf("AttributionEvent") &&
							type == PlayerListener::TYPE_ATTRIBUTION)) {

				return event->pause();
			}
		}

		return false;
	}

	bool ProceduralPlayerAdapter::resumeEvent(string anchorId, short type) {
		FormatterEvent* event;
		event = getEvent(anchorId);
		if (event != NULL) {
			if ((event->instanceOf("PresentationEvent") &&
					type == PlayerListener::TYPE_PRESENTATION) ||
					(event->instanceOf("AttributionEvent") &&
							type == PlayerListener::TYPE_ATTRIBUTION)) {

				return event->resume();
			}
		}

		return false;
	}

	FormatterEvent* ProceduralPlayerAdapter::getEvent(string anchorId) {
		vector<FormatterEvent*>* events;
		events = object->getEvents();
		if (events != NULL) {
			vector<FormatterEvent*>::iterator i;
			FormatterEvent* event;
			Anchor* anchor;

			i = events->begin();
			while (i != events->end()) {
				event = *i;
				if (event != NULL) {
					if (event->instanceOf("AnchorEvent")) {
						anchor = ((AnchorEvent*)event)->getAnchor();
						if (anchor != NULL) {
							if ((anchor->instanceOf("LabeledAnchor") && 
									anchor->getId() == anchorId) ||
									(anchor->instanceOf("LamdaAnchor") && 
											anchorId == "")) {

								delete events;
								events = NULL;
								return event;
							}
						}

					} else if (event->instanceOf("AttributionEvent")) {
						anchor = ((AnchorEvent*)event)->getAnchor();
						if (anchor != NULL) {
							if (((PropertyAnchor*)anchor)->getPropertyName()
									== anchorId) {

								delete events;
								events = NULL;
								return event;
							}
						}
					}
				}
				++i;
			}
			delete events;
			events = NULL;
		}
		return NULL;
	}

	void ProceduralPlayerAdapter::lockEvent() {
		pthread_mutex_lock(&eventMutex);
	}

	void ProceduralPlayerAdapter::unlockEvent() {
		pthread_mutex_unlock(&eventMutex);
	}
}
}
}
}
}
}
}
