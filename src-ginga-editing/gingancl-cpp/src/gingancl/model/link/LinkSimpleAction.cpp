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

#include "../../../include/LinkSimpleAction.h"
#include "../../../include/LinkAssignmentAction.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkSimpleAction::LinkSimpleAction(FormatterEvent* event, short type) :
		    LinkAction() {

		this->event = event;
		this->actionType = type;
		listeners = new set<LinkActionListener*>;
		typeSet.insert("LinkSimpleAction");
	}

	LinkSimpleAction::~LinkSimpleAction() {
		wclog << "LinkSimpleAction::~LinkSimpleAction" << endl;
		lock();
		if (listeners != NULL) {
			listeners->clear();
			delete listeners;
			listeners = NULL;
		}
		unlock();

		if (event != NULL) {
			event = NULL;
		}
	}

	FormatterEvent* LinkSimpleAction::getEvent() {
		return event;
	}

	short LinkSimpleAction::getType() {
		return actionType;
	}

	void LinkSimpleAction::run() {
		LinkAction::run();
		set<LinkActionListener*>::iterator i;

		lock();
		for (i = listeners->begin(); i != listeners->end(); ++i) {
			((LinkActionListener*)(*i))->runAction((void*)this);
		}
		unlock();

		if (actionType == SimpleAction::ACT_START) {
			notifyProgressionListeners(true);

		} else {
			notifyProgressionListeners(false);
		}
	}

	void LinkSimpleAction::addActionListener(
		    LinkActionListener* listener) {

		lock();
		if (listeners->count(listener) == 0) {
			listeners->insert(listener);
		}
		unlock();
	}

	void LinkSimpleAction::removeActionListener(
		    LinkActionListener* listener) {

		set<LinkActionListener*>::iterator i;

		lock();
		i = listeners->find(listener);
		if (i != listeners->end()) {
			listeners->erase(i);
		}
		unlock();
	}

	set<LinkActionListener*>* LinkSimpleAction::getActionListeners() {
		return listeners;
	}

	vector<FormatterEvent*>* LinkSimpleAction::getEvents() {
		if (event == NULL)
			return NULL;

		vector<FormatterEvent*>* events;
		events = new vector<FormatterEvent*>;

		events->push_back(event);
		return events;
	}

	vector<LinkAction*>* LinkSimpleAction::getImplicitRefRoleActions() {
		vector<LinkAction*>* actions;
		string value;

		actions = new vector<LinkAction*>;

		if (this->instanceOf("LinkAssignmentAction")) {
			value = ((LinkAssignmentAction*)this)->getValue();
			if (value[0] == '$') {
				if (event->instanceOf("AttributionEvent")) {
					actions->push_back((LinkAction*)this);
				}
			}
		}

		if (actions->empty()) {
			delete actions;
			return NULL;
		}

		return actions;
	}
}
}
}
}
}
}
}
