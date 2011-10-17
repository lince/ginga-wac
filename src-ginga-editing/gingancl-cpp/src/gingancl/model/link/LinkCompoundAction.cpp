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

#include "../../../include/LinkCompoundAction.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkCompoundAction::LinkCompoundAction(short op) : LinkAction() {
		actions = new vector<LinkAction*>;
		this->op= op;
		typeSet.insert("LinkCompoundAction");
	}

	LinkCompoundAction::~LinkCompoundAction() {
		wclog << "LinkCompoundAction::~LinkCompoundAction" << endl;
		vector<LinkAction*>::iterator i;
		LinkAction* action;

		lock();
		for (i = actions->begin(); i != actions->end(); ++i) {
			action = (LinkAction*)(*i);
			action->removeActionProgressionListener(this);
			delete action;
			action = NULL;
		}

		actions->clear();
	}

	short LinkCompoundAction::getOperator() {
		return op;
	}

	void LinkCompoundAction::addAction(LinkAction* action) {
		lock();
		actions->push_back(action);
		unlock();
		action->addActionProgressionListener(this);
	}

	vector<LinkAction*>* LinkCompoundAction::getActions() {
		vector<LinkAction*>* acts;

		lock();
		if (actions->empty()) {
			unlock();
			return NULL;
		}
		acts = new vector<LinkAction*>(*actions);
		unlock();

		return acts;
	}

	vector<FormatterEvent*>* LinkCompoundAction::getEvents() {
		lock();
		if (actions->empty()) {
			unlock();
			return NULL;
		}

		vector<LinkAction*>* acts;
		vector<LinkAction*>::iterator i;
		LinkAction* action;
		vector<FormatterEvent*>* events;
		vector<FormatterEvent*>* actionEvents;
		vector<FormatterEvent*>::iterator j;

		acts = new vector<LinkAction*>(*actions);
		unlock();
		events = new vector<FormatterEvent*>;

		for (i = acts->begin(); i != acts->end(); ++i) {
			action = (LinkAction*)(*i);
			actionEvents = action->getEvents();
			if (actionEvents != NULL) {
				for (j = actionEvents->begin(); j != actionEvents->end(); ++j) {
					events->push_back(*j);
				}
				delete actionEvents;
				actionEvents = NULL;
			}
		}

		delete acts;
		if (events->empty()) {
			delete events;
			return NULL;
		}

		return events;
	}

	vector<LinkAction*>* LinkCompoundAction::getImplicitRefRoleActions() {
		lock();
		if (actions->empty()) {
			unlock();
			return NULL;
		}

		vector<LinkAction*>* acts;
		vector<LinkAction*>::iterator i;
		vector<LinkAction*>* assignmentActs;
		vector<LinkAction*>* refActs;
		vector<LinkAction*>::iterator j;

		acts = new vector<LinkAction*>(*actions);
		unlock();

		refActs = new vector<LinkAction*>;

		for (i = acts->begin(); i != acts->end(); ++i) {
			assignmentActs = (*i)->getImplicitRefRoleActions();
			if (assignmentActs != NULL) {
				for (j = assignmentActs->begin();
						j != assignmentActs->end(); ++j) {

					refActs->push_back(*j);
				}
				delete assignmentActs;
				assignmentActs = NULL;
			}
		}

		delete acts;
		if (refActs->empty()) {
			delete refActs;
			return NULL;
		}

		return refActs;
	}

	void LinkCompoundAction::run() {
		int i, size;
		LinkAction *action;
		vector<LinkAction*>* acts;

		lock();
		size = actions->size();
		acts = new vector<LinkAction*>(*actions);
		unlock();

		pendingActions = size;
		hasStart = false;

		LinkAction::run();

		if (op == CompoundAction::OP_PAR) {
			for (i = 0; i < size; i++) {
				action = (LinkAction*)(acts->at(i));
				//actionThread = new Thread(action);
				action->start();
			}

		} else {
			for (i = 0; i < size; i++) {
				action = (LinkAction*)(acts->at(i));
				action->run();
			}
		}
		delete acts;
	}

	void LinkCompoundAction::actionProcessed(bool start) {
		pendingActions--;
		hasStart = (hasStart || start);
		if (pendingActions == 0) {
			notifyProgressionListeners(hasStart);
		}
	}
}
}
}
}
}
}
}
