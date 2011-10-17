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

#include "../../../include/model/FormatterLayout.h"

#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
	FormatterLayout::FormatterLayout() {
		regionMap = new map<string, set<FormatterRegion*>*>;
		typeSet.insert("FormatterLayout");
		pthread_mutex_init(&mutex, NULL);
		createDeviceRegion();
	}

	FormatterLayout::~FormatterLayout() {
		clear();
		lock();
		if (regionMap != NULL) {
			delete regionMap;
			regionMap = NULL;
		}
		unlock();
	}

	void FormatterLayout::createDeviceRegion() {
		IComponentManager* cm = IComponentManager::getCMInstance();
		IDeviceManager* dm = NULL;
		int w = 0;
		int h = 0;

		if (cm != NULL) {
			dm = ((DeviceManagerCreator*)(cm->getObject("DeviceManager")))();
		}

		if (dm != NULL) {
			w = dm->getDeviceWidth();
			h = dm->getDeviceHeight();
		}

		deviceRegion = new FormatterDeviceRegion("defaultScreenFormatter");
		deviceRegion->setTop(0, false);
		deviceRegion->setLeft(0, false);
		deviceRegion->setWidth(w, false);
		deviceRegion->setHeight(h, false);
	}

	void FormatterLayout::prepareFormatterRegion(
			ExecutionObject* object, ISurface* renderedSurface) {

		CascadingDescriptor* descriptor;
		FormatterRegion* region;

		LayoutRegion *layoutRegion, *parent, *grandParent;
		string regionId;
		set<FormatterRegion*>* formRegions;

		if (object == NULL || object->getDescriptor() == NULL ||
			    object->getDescriptor()->getFormatterRegion() == NULL) {

			/*cout << "FormatterLayout::prepareFormatterRegion Warning! Can't";
			cout << " prepare FormatterRegion for '";
			if (object != NULL) {
				cout << object->getId();

			} else {
				cout << "NULL";
			}

			cout << "' object" << endl;*/
			return;
		}

		descriptor = object->getDescriptor();
		region = descriptor->getFormatterRegion();
		layoutRegion = region->getOriginalRegion();

		//every presented object has a region root
		//the formatter device region
		parent = layoutRegion;
		grandParent = layoutRegion->getParent();
		if (grandParent != NULL) {
			while (grandParent->getParent() != NULL) {
				parent = grandParent;
				grandParent = grandParent->getParent();
			}
		}

		if (grandParent != deviceRegion) {
			parent->setParent(deviceRegion);
			deviceRegion->addRegion(parent);
		}

		regionId = layoutRegion->getId();

		/*cout << "FormatterLayout::prepareFormatterRegion call prepareOD";
		cout << " for '" << object->getId() << "'" << endl;*/
		lock();
		if (regionMap->count(regionId)==0) {
			formRegions = new set<FormatterRegion*>;
			(*regionMap)[regionId] = formRegions;

		} else {
			formRegions = (*regionMap)[regionId];
		}
		formRegions->insert(region);
		region->prepareOutputDisplay(renderedSurface);
		unlock();
	}

	void FormatterLayout::showObject(ExecutionObject* object) {
		FormatterRegion* region;

		if (object == NULL || object->getDescriptor() == NULL ||
			    object->getDescriptor()->getFormatterRegion() == NULL) {

			/*cout << "FormatterLayout::showObject can't find region for '";
			if (object != NULL) {
				cout << object->getId();

			} else {
				cout << "NULL";
			}
			cout << "'" << endl;*/
			return;
		}

		/*cout << "FormatterLayout::showObject call showContent for '";
		cout << object->getId() << "'" << endl;*/
		region = object->getDescriptor()->getFormatterRegion();
		region->showContent();
	}

	void FormatterLayout::hideObject(ExecutionObject* object) {
		FormatterRegion* region = NULL;
		LayoutRegion* layoutRegion, *parent, *grandParent;
		string regionId;

		set<FormatterRegion*>* formRegions;
		set<FormatterRegion*>::iterator i;
		map<string, set<FormatterRegion*>*>::iterator j;

		if (object != NULL && object->getDescriptor() != NULL) {
			region = object->getDescriptor()->getFormatterRegion();
		}

		if (region == NULL) {
			return;
		}

		region = object->getDescriptor()->getFormatterRegion();
		region->hideContent();
		layoutRegion = region->getLayoutRegion();
		regionId = layoutRegion->getId();
		lock();
		if (regionMap != NULL && regionMap->count(regionId) != 0) {
			formRegions = (*regionMap)[regionId];
			i = formRegions->find(region);
			if (i != formRegions->end()) {
				formRegions->erase(i);
				if (formRegions->empty()) {
					j = regionMap->find(regionId);
					if (j != regionMap->end()) {
						regionMap->erase(j);
						delete formRegions;
					}
				}
			}
		}
		unlock();

		/*map<string, vector<FormatterRegion*>*>::iterator i;
		i=regionMap->begin();
		while(i!=regionMap->end()) {
			if (i->first == regionId) {
				formRegions = i->second;

				if (formRegions->size() == 1) {
					formRegions->clear();

				} else {
					vector<FormatterRegion*>::iterator j;
					j=formRegions->begin();
					while (j!=formRegions->end()) {
						if (*j == region) {
							formRegions->erase(j);
							j = formRegions->begin();

						} else {
							++j;
						}
					}
				}

				regionMap->erase(i);
				i = regionMap->begin();

			} else {
				++i;
			}
		}*/
	}

	set<FormatterRegion*>* FormatterLayout::
		    getFormatterRegionsFromNcmRegion(string regionId) {

		lock();
		if (regionMap->count(regionId) == 0) {
			unlock();
			return NULL;
		}

		set<FormatterRegion*>* regions;
		regions = new set<FormatterRegion*>(*((*regionMap)[regionId]));
		unlock();
		return regions;
	}

	void FormatterLayout::clear() {
		map<string, set<FormatterRegion*>*>::iterator i;
		set<FormatterRegion*>* formRegions;

		lock();
		for (i=regionMap->begin(); i!=regionMap->end(); ++i) {
			formRegions = i->second;
			formRegions->clear();
			delete formRegions;
		}

		regionMap->clear();
		unlock();
		deviceRegion->removeRegions();
	}

	void FormatterLayout::lock() {
		pthread_mutex_lock(&mutex);
	}

	void FormatterLayout::unlock() {
		pthread_mutex_unlock(&mutex);
	}
}
}
}
}
}
}
}
