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

#include "../../../../../include/io/interface/input/dfb/DFBGInputEvent.h"

/* macro for a safe call to DirectFB functions */
#ifndef DFBCHECK
#define DFBCHECK(x...)                                            \
{                                                                 \
	DFBResult err = x;                                            \
	if (err != DFB_OK) {                                          \
		fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );    \
	}                                                             \
}
#endif /*DFBCHECK*/

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	DFBGInputEvent::DFBGInputEvent(void* event) {
		this->event = (DFBEvent*)event;
	}

	DFBGInputEvent::DFBGInputEvent(const int keyCode) {
		event = (DFBEvent*)(new DFBInputEvent);
		event->clazz = (DFBEventClass)DFEC_INPUT;
		((DFBInputEvent*)event)->type = DIET_KEYPRESS;
		((DFBInputEvent*)event)->flags = DIEF_KEYSYMBOL;
		((DFBInputEvent*)event)->key_symbol = (DFBInputDeviceKeySymbol)keyCode;
	}

	DFBGInputEvent::DFBGInputEvent(int clazz, int type) {
		if (clazz == DFEC_USER) {
			event = (DFBEvent*)(new DFBUserEvent);
			event->clazz = (DFBEventClass)clazz;
			((DFBUserEvent*)event)->type = type;

		} else if (clazz == DFEC_INPUT) {
			event = (DFBEvent*)(new DFBInputEvent);
			event->clazz = (DFBEventClass)clazz;
			((DFBInputEvent*)event)->type = (DFBInputEventType)type;
		}
	}

	DFBGInputEvent::~DFBGInputEvent() {
		if (event != NULL) {
			delete event;
			event = NULL;
		}
	}

	void* DFBGInputEvent::getContent() {
		return event;
	}

	void DFBGInputEvent::setKeyCode(const int keyCode) {
		if (event != NULL && event->clazz == DFEC_INPUT) {
			((DFBInputEvent*)event)->key_symbol = (DFBInputDeviceKeySymbol)keyCode;
		}
	}

	const int DFBGInputEvent::getKeyCode() {
		if (event != NULL && event->clazz == DFEC_INPUT) {
			return ((DFBInputEvent*)event)->key_symbol;
		}
		return CodeMap::KEY_NULL;
	}

	void DFBGInputEvent::setType(unsigned int type) {
		if (event != NULL) {
			if (event->clazz == DFEC_INPUT) {
				((DFBInputEvent*)event)->type = (DFBInputEventType)type;

			} else if (event->clazz == DFEC_USER) {
				((DFBUserEvent*)event)->type = (DFBInputEventType)type;
			}
		}
	}

	unsigned int DFBGInputEvent::getType() {
		if (event != NULL) {
			if (event->clazz == DFEC_INPUT) {
				return ((DFBInputEvent*)event)->type;

			} else if (event->clazz == DFEC_USER) {
				return ((DFBUserEvent*)event)->type;
			}
		}
		return CodeMap::KEY_NULL;
	}

	bool DFBGInputEvent::isPressedType() {
		if (event != NULL && event->clazz == DFEC_INPUT) {
			return (((DFBInputEvent*)event)->type == DIET_KEYPRESS);
		}
		return false;
	}

	bool DFBGInputEvent::isKeyType() {
		if (event != NULL && event->clazz == DFEC_INPUT) {
			return (((DFBInputEvent*)event)->type == DIET_KEYPRESS ||
					((DFBInputEvent*)event)->type == DIET_KEYRELEASE);
		}
		return false;
	}

	bool DFBGInputEvent::isUserClass() {
		if (event != NULL) {
			return (event->clazz == DFEC_USER);
		}
		return false;
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::IInputEvent*
		createDFBInputEvent(void* event, const int symbol) {

	if (event != NULL) {
		return (new ::br::pucrio::telemidia::ginga::core::system::io::
				DFBGInputEvent(event));
	}

	if (symbol >= 0) {
		return (new ::br::pucrio::telemidia::ginga::core::system::io::
				DFBGInputEvent(symbol));
	}

	return NULL;
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::IInputEvent*
		createDFBUserEvent(int type) {

	return (new ::br::pucrio::telemidia::ginga::core::system::io::
			DFBGInputEvent(DFEC_USER, type));
}


extern "C" void destroyDFBInputEvent(
		::br::pucrio::telemidia::ginga::core::system::io::IInputEvent* eb) {

	delete eb;
}
