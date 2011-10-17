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

#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include "../thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;

#include "IInputManager.h"

#include <iostream>
#include <set>
#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	class InputManager : public IInputManager, public Thread {
		private:
			static InputManager* _instance;

		protected:
			map<IInputEventListener*, set<int>*>* eventListeners;
			map<IInputEventListener*, set<int>*>* toAddEventListeners;
			set<IInputEventListener*>* toRemoveEventListeners;
			set<IInputEventListener*>* proceduralListeners;
			set<IInputEventListener*>* toAddProcListeners;
			set<IInputEventListener*>* toRemoveProcListeners;
			IComponentManager* cm;
			bool running;
			bool notifying;
			bool notifyingProc;
			IEventBuffer* eventBuffer;
			double lastEventTime;
			double timeStamp;
			double imperativeIntervalTime;
			double declarativeIntervalTime;
			pthread_mutex_t addMutex;
			pthread_mutex_t removeMutex;
			pthread_mutex_t procMutex;
			pthread_mutex_t addProcMutex;
			pthread_mutex_t removeProcMutex;

		protected:
			InputManager();
			virtual ~InputManager();
			void initializeInputIntervalTime();
				
		public:
			void release();
			static InputManager* getInstance();
			void addInputEventListener(
					IInputEventListener* listener, set<int>* events=NULL);

			void removeInputEventListener(IInputEventListener* listener);

		protected:
			bool dispatchEvent(IInputEvent* keyEvent);
			bool dispatchProceduralEvent(IInputEvent* keyEvent);

		public:
			void addProceduralInputEventListener(IInputEventListener* listener);
			void removeProceduralInputEventListener(
					IInputEventListener* listener);

			void postEvent(IInputEvent* event);
			void postEvent(int keyCode);

		protected:
			virtual void run();
	};
}
}
}
}
}
}
}

#endif /*INPUTMANAGER_H_*/
