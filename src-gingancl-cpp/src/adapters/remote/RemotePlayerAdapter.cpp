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

#include "../../../include/adapters/remote/RemotePlayerAdapter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace remote {
	RemotePlayerAdapter::RemotePlayerAdapter() :
			FormatterPlayerAdapter() {

		typeSet.insert("RemotePlayerAdapter");
		pthread_mutex_init(&eventMutex, NULL);
		preparedEvents = new map<string, FormatterEvent*>;
		currentEvent = NULL;
		editingCommandListener = NULL;
	}

	RemotePlayerAdapter::~RemotePlayerAdapter() {
		lockEvent();
		if (preparedEvents != NULL) {
			delete preparedEvents;
			preparedEvents = NULL;
		}
		currentEvent = NULL;
		unlockEvent();
		pthread_mutex_destroy(&eventMutex);
	}

	void RemotePlayerAdapter::setNclEditListener(
			INclEditListener* listener) {

		this->editingCommandListener = listener;
	}

	bool RemotePlayerAdapter::hasPrepared() {
		bool presented;
		short st;

		if (object == NULL || player == NULL) {
			return false;
		}

		presented = player->hasPresented();
		if (presented) {
			return false;
		}

		return true;
	}

	bool RemotePlayerAdapter::prepare(
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

	void RemotePlayerAdapter::prepare(FormatterEvent* event) {
		double duration;
		IntervalAnchor* intervalAnchor;

		lockEvent();
		if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
			unlockEvent();
			return;
		}

		if (event->instanceOf("AnchorEvent")) {
			if ((((AnchorEvent*)event)->getAnchor())->instanceOf(
					"LambdaAnchor")) {

				duration = ((PresentationEvent*)event)->getDuration();

				if (duration < IntervalAnchor::OBJECT_DURATION) {
					player->setScope(
							"",
							IPlayer::TYPE_PRESENTATION,
							0.0, duration / 1000);
				}

			} else if (((((AnchorEvent*)event)->getAnchor()))->
					instanceOf("IntervalAnchor")) {

				intervalAnchor = (IntervalAnchor*)(
						((AnchorEvent*)event)->getAnchor());

				player->setScope(
						((AnchorEvent*)event)->getAnchor()->getId(),
						IPlayer::TYPE_PRESENTATION,
						(intervalAnchor->getBegin() / 1000),
						(intervalAnchor->getEnd() / 1000));

			} else if (((((AnchorEvent*)event)->getAnchor()))->
					instanceOf("LabeledAnchor")) {

				player->setScope(((LabeledAnchor*)((AnchorEvent*)event)->
						getAnchor())->getLabel(), IPlayer::TYPE_PRESENTATION);
			}
		}

		(*preparedEvents)[event->getId()] = event;
		unlockEvent();
	}

	bool RemotePlayerAdapter::start() {
		if (object->start()) {
			player->play();
			return true;
		}
		return false;
	}

	bool RemotePlayerAdapter::stop() {
		if (player != NULL) {
			if (!player->isForcedNaturalEnd()) {
				player->stop();
				player->notifyReferPlayers(EventUtil::TR_STOPS);

			} else {
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
		}

		if (object != NULL && object->stop()) {
			unprepare();
			return true;
		}

		cout << "RemotePlayerAdapter::stop() Warning! Should never ";
		cout << "reaches here!" << endl;
		return false;
	}

	bool RemotePlayerAdapter::pause() {
		if (object != NULL && object->pause()) {
			player->pause();
			player->notifyReferPlayers(EventUtil::TR_PAUSES);
			return true;

		} else {
			return false;
		}
	}

	bool RemotePlayerAdapter::resume() {
		if (object != NULL && object->resume()) {
			player->resume();
			player->notifyReferPlayers(EventUtil::TR_RESUMES);
			return true;
		}
		return false;
	}

	bool RemotePlayerAdapter::abort() {
		player->stop();
		player->notifyReferPlayers(EventUtil::TR_ABORTS);

		if (object != NULL && object->abort()) {
			unprepare();
			return true;
		}

		return false;
	}

	bool RemotePlayerAdapter::unprepare() {
		map<string, FormatterEvent*>::iterator i;

		lockEvent();
		if (currentEvent->getCurrentState() == EventUtil::ST_OCCURRING ||
				currentEvent->getCurrentState() == EventUtil::ST_PAUSED) {

			unlockEvent();
			return stop();
		}

		if (preparedEvents->count(currentEvent->getId()) != 0 &&
				preparedEvents->size() == 1) {

			if (object != NULL) {
				object->unprepare();
			}

			preparedEvents->clear();
			object = NULL;

		} else {
			if (object != NULL) {
				object->unprepare();
			}

			i = preparedEvents->find(currentEvent->getId());
			if (i != preparedEvents->end()) {
				preparedEvents->erase(i);
			}
		}
		unlockEvent();
		return true;
	}

	void RemotePlayerAdapter::naturalEnd() {
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

		if (object != NULL && object->stop()) {
			unprepare();
		}
	}

	void RemotePlayerAdapter::updateStatus(
			short code, string param, short type) {

		switch(code) {
			case IPlayer::PL_NOTIFY_START:
				if (object != NULL) {
					startEvent(param, type);
				}
				break;

			case IPlayer::PL_NOTIFY_PAUSE:
				if (object != NULL) {
					pauseEvent(param, type);
				}
				break;

			case IPlayer::PL_NOTIFY_RESUME:
				if (object != NULL) {
					resumeEvent(param, type);
				}
				break;

			case IPlayer::PL_NOTIFY_STOP:
				if (object != NULL) {
					if (param == "") {
						naturalEnd();
					} else {
						stopEvent(param, type);
					}
				}
				break;

			case IPlayer::PL_NOTIFY_ABORT:
				if (object != NULL) {
					abortEvent(param, type);
				}
				break;

			case IPlayer::PL_NOTIFY_NCLEDIT:
				if (editingCommandListener != NULL) {
					editingCommandListener->nclEdit(param);
				}
				break;
		}
	}

	bool RemotePlayerAdapter::startEvent(string anchorId, short type) {
		FormatterEvent* event;
		event = getEvent(anchorId);
		if (event != NULL) {
			if ((event->instanceOf("PresentationEvent") &&
					type == IPlayer::TYPE_PRESENTATION) ||
					(event->instanceOf("AttributionEvent") &&
							type == IPlayer::TYPE_ATTRIBUTION)) {

				return event->start();
			}
		}

		return false;
	}

	bool RemotePlayerAdapter::stopEvent(string anchorId, short type) {
		FormatterEvent* event;
		event = getEvent(anchorId);
		if (event != NULL) {
			if ((event->instanceOf("PresentationEvent") &&
					type == IPlayer::TYPE_PRESENTATION) ||
					(event->instanceOf("AttributionEvent") &&
							type == IPlayer::TYPE_ATTRIBUTION)) {

				return event->stop();
			}

		} else {
			cout << "RemotePlayerAdapter::stopEvent event not found '";
			cout << anchorId << "'" << endl;
		}

		return false;
	}

	bool RemotePlayerAdapter::abortEvent(string anchorId, short type) {
		FormatterEvent* event;
		event = getEvent(anchorId);
		if (event != NULL) {
			if ((event->instanceOf("PresentationEvent") &&
					type == IPlayer::TYPE_PRESENTATION) ||
					(event->instanceOf("AttributionEvent") &&
							type == IPlayer::TYPE_ATTRIBUTION)) {

				return event->abort();
			}
		}

		return false;
	}

	bool RemotePlayerAdapter::pauseEvent(string anchorId, short type) {
		FormatterEvent* event;
		event = getEvent(anchorId);
		if (event != NULL) {
			if ((event->instanceOf("PresentationEvent") &&
					type == IPlayer::TYPE_PRESENTATION) ||
					(event->instanceOf("AttributionEvent") &&
							type == IPlayer::TYPE_ATTRIBUTION)) {

				return event->pause();
			}
		}

		return false;
	}

	bool RemotePlayerAdapter::resumeEvent(string anchorId, short type) {
		FormatterEvent* event;
		event = getEvent(anchorId);
		if (event != NULL) {
			if ((event->instanceOf("PresentationEvent") &&
					type == IPlayer::TYPE_PRESENTATION) ||
					(event->instanceOf("AttributionEvent") &&
							type == IPlayer::TYPE_ATTRIBUTION)) {

				return event->resume();
			}
		}

		return false;
	}

	FormatterEvent* RemotePlayerAdapter::getEvent(string anchorId) {
		vector<FormatterEvent*>* events;

		if (object == NULL) {
			cout << "RemotePlayerAdapter::getEvent Warning! Object is ";
			cout << "NULL" << endl;
			return NULL;
		}

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
							if ((anchor->instanceOf("IntervalAnchor") &&
									anchor->getId() == anchorId) ||
								(anchor->instanceOf("LabeledAnchor") &&
									((LabeledAnchor*)anchor)->getLabel()
											== anchorId) ||
								(anchor->instanceOf("LamdaAnchor") &&
										anchorId == "")) {

								delete events;
								events = NULL;
								return event;
							}
						}

					} else if (event->instanceOf("AttributionEvent")) {
						anchor = ((AttributionEvent*)event)->getAnchor();
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

	void RemotePlayerAdapter::lockEvent() {
		pthread_mutex_lock(&eventMutex);
	}

	void RemotePlayerAdapter::unlockEvent() {
		pthread_mutex_unlock(&eventMutex);
	}
}
}
}
}
}
}
}
