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

#include "../../include/NominalEventMonitor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
	NominalEventMonitor::NominalEventMonitor(
			ExecutionObject* object, Player* player) : Thread() {

		this->running = false;
		this->deleting = false;
		this->player = player;
		this->executionObject = object;
		this->sleepTime = DEFAULT_SLEEP_TIME;
		this->expectedSleepTime = 0;
	}

	NominalEventMonitor::~NominalEventMonitor() {
		deleting = true;
		running = false;
		stopped = true;
		wakeUp();
		unlockConditionSatisfied();

		lock();
		this->player = NULL;
		this->executionObject = NULL;
		unlock();
	}

	void NominalEventMonitor::startMonitor() {
		if (!running && !deleting) {
    		// start monitor only if there is predictable events
			running = true;
    		paused = false;
    		stopped = false;
    		Thread::start();

    	} else {
    		cout << "NominalEventMonitor::startMonitor(";
    		cout << this << ") Warning! Trying to ";
    		cout << "start monitor that is already running" << endl;
		}
	}

	void NominalEventMonitor::pauseMonitor() {
		if (!isInfinity(expectedSleepTime)) {
			wakeUp();
			paused = true;
		}
	}

	void NominalEventMonitor::resumeMonitor() {
		if (!isInfinity(expectedSleepTime) && paused) {
			paused = false;
			unlockConditionSatisfied();
		}
	}

	void NominalEventMonitor::stopMonitor() {
		stopped = true;
		if (running) {
			running = false;
			if (isInfinity(expectedSleepTime) || paused) {
				unlockConditionSatisfied();
			} else {
				wakeUp();
			}
		}
	}

	void NominalEventMonitor::run() {
		lock();
		EventTransition* nextTransition;
		double time;
		double mediaTime = 0;
		double nextEntryTime;
		nextTransition = NULL;

		/*cout << "====== Anchor Monitor Activated for '";
		cout << executionObject->getId().c_str() << "' (";
		cout << this << " ======" << endl;*/

		while (running && !deleting) {
			if (paused) {
				waitForUnlockCondition();

			} else {
				if (executionObject != NULL) {
					nextTransition = executionObject->getNextTransition();

				} else {
					nextTransition = NULL;
					running = false;
				}

				if (nextTransition == NULL) {
					running = false;

				} else {
					nextEntryTime = nextTransition->getTime();
					if (isInfinity(nextEntryTime)) {
						expectedSleepTime = infinity();

					} else {
						mediaTime = (player->getMediaTime() * 1000);
      					expectedSleepTime = nextEntryTime - mediaTime;
					}

					wclog << "ANCHORMONITOR NEXTTRANSITIONTIME = '";
					wclog << nextEntryTime << "' MEDIATIME = '" << mediaTime;
					wclog << "' EXPECTEDSLEEPTIME = '" << expectedSleepTime;
					wclog << "' (" << this << ")" << endl;

					if (running && !deleting) {
						if (isInfinity(expectedSleepTime)) {
							waitForUnlockCondition();

						} else {
							Thread::usleep((int)(expectedSleepTime));
						}

						if (running && !deleting) {
							if (executionObject == NULL || player == NULL) {
								unlock();
								return;
							}

							mediaTime = (player->getMediaTime() * 1000);
							time = nextEntryTime - mediaTime;
							if (time < 0) {
								time = 0;
							}

							if (!paused && time <= DEFAULT_ERROR) {
								// efetua a transicao no estado do evento
								executionObject->updateTransitionTable(
									   mediaTime + DEFAULT_ERROR);
							}
						}
					}
				}
			}
		}

		if (!stopped && player != NULL && !deleting) {
			player->forceNaturalEnd();
		}
		unlock();
	}
}
}
}
}
}
}
