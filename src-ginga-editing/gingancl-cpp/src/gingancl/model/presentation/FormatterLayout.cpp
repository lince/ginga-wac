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

#include "../../../include/FormatterLayout.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
	FormatterLayout::FormatterLayout() {
		regionMap = new map<string, vector<FormatterRegion*>*>;
		typeSet.insert("FormatterLayout");
		pthread_mutex_init(&mutex, NULL);
		createDeviceRegion();
	}

	void FormatterLayout::createDeviceRegion() {
		deviceRegion = new FormatterDeviceRegion("defaultScreenFormatter");
		deviceRegion->setTop(0, false);
		deviceRegion->setLeft(0, false);
		deviceRegion->setWidth(
			    OutputManager::getInstance()->getDeviceWidth(), false);

		deviceRegion->setHeight(
				OutputManager::getInstance()->getDeviceHeight(), false);
	}

	void FormatterLayout::prepareFormatterRegion(
			ExecutionObject* object, Surface* renderedSurface) {

		CascadingDescriptor* descriptor;
		FormatterRegion* region;

		LayoutRegion *layoutRegion, *parent, *grandParent;
		string regionId;
		vector<FormatterRegion*>* formRegions;

		if (object == NULL || object->getDescriptor() == NULL ||
			    object->getDescriptor()->getFormatterRegion() == NULL) {

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

		lock();
		if (regionMap->count(regionId)==0) {
			formRegions = new vector<FormatterRegion*>;
			(*regionMap)[regionId] = formRegions;

		} else {
			formRegions = (*regionMap)[regionId];
		}
		formRegions->push_back(region);
		region->prepareOutputDisplay(renderedSurface);
		unlock();
	}

	void FormatterLayout::showObject(ExecutionObject* object) {
		FormatterRegion* region;

		if (object == NULL || object->getDescriptor() == NULL ||
			    object->getDescriptor()->getFormatterRegion() == NULL) {

			return;
		}

		region = object->getDescriptor()->getFormatterRegion();
		region->showContent();
	}

	void FormatterLayout::hideObject(ExecutionObject* object) {
		FormatterRegion* region;
		LayoutRegion* layoutRegion, *parent, *grandParent;
		string regionId;

		if (object == NULL || object->getDescriptor() == NULL ||
			    object->getDescriptor()->getFormatterRegion() == NULL) {

			return;
		}

		region = object->getDescriptor()->getFormatterRegion();

		wclog << "FormatterLayout::hideObject hiding content" << endl;
		region->hideContent();
		layoutRegion = region->getLayoutRegion();
		regionId = layoutRegion->getId();
		lock();
		if (regionMap != NULL && regionMap->count(regionId) != 0) {
			vector<FormatterRegion*>* formRegions;
			formRegions = (*regionMap)[regionId];

			vector<FormatterRegion*>::iterator i;
			i = formRegions->begin();
			while (i != formRegions->end()) {
				if (*i == region) {
					formRegions->erase(i);
					if (formRegions->empty()) {
						regionMap->erase(regionMap->find(regionId));
						delete formRegions;
					}
					break;

				}
				++i;
			}
		}
		unlock();
		wclog << "FormatterLayout::hideObject all done!" << endl;

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

	vector<FormatterRegion*>* FormatterLayout::
		    getFormatterRegionsFromNcmRegion(string regionId) {

		lock();
		if (regionMap->count(regionId) == 0) {
			unlock();
			return NULL;
		}

		vector<FormatterRegion*>* regions;
		regions = new vector<FormatterRegion*>(*((*regionMap)[regionId]));
		unlock();
		return regions;
	}

	void FormatterLayout::clear() {
		wclog << "FormatterLayout::clear() " << endl;
		vector<FormatterRegion*>* formRegions;

		lock();
		map<string, vector<FormatterRegion*>*>::iterator i;
		for (i=regionMap->begin(); i!=regionMap->end(); ++i) {
			formRegions = i->second;
			formRegions->clear();
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
