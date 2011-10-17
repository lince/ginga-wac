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

#include "../../../../../include/io/interface/device/dfb/DFBDeviceScreen.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {

const unsigned int DFBDeviceScreen::DSA_UNKNOWN = 0;
const unsigned int DFBDeviceScreen::DSA_4x3     = 1;
const unsigned int DFBDeviceScreen::DSA_16x9    = 2;

unsigned int DFBDeviceScreen::numOfDFBScreens = 0;
IDirectFB* DFBDeviceScreen::dfb = NULL;
IDirectFBDisplayLayer* DFBDeviceScreen::gfxLayer = NULL;

	DFBDeviceScreen::DFBDeviceScreen(int numArgs, char* args[]) {
		aspect = DSA_UNKNOWN;
		hSize = 0;
		vSize = 0;
		hRes = 0;
		wRes = 0;
		numOfDFBScreens++;

		windowPool = new set<IDirectFBWindow*>;
		surfacePool = new set<IDirectFBSurface*>;

		pthread_mutex_init(&winMutex, NULL);
		pthread_mutex_init(&surMutex, NULL);

		if (DFBDeviceScreen::dfb == NULL) {
			DFBCHECK(DirectFBInit(&numArgs, &args));
			DFBCHECK(DirectFBCreate( &dfb ));

			DFBResult ret;

			if (gfxLayer == NULL) {
				DFBCHECK(dfb->GetDisplayLayer(
						dfb, DLID_PRIMARY, &gfxLayer));
			}

			DFBDisplayLayerConfig layer_config;
			DFBCHECK(gfxLayer->GetConfiguration(
					gfxLayer, &layer_config));

			wRes = layer_config.width;
			hRes = layer_config.height;

			/*if ((ret = gfxLayer->SetCooperativeLevel(
			  gfxLayer, DLSCL_EXCLUSIVE )) != DFB_OK)
			  DirectFBError("IDirectFBScreen::SetCooperativeLevel",
			  ret);*/

			layer_config.flags = (DFBDisplayLayerConfigFlags)
					( DLCONF_BUFFERMODE | DLCONF_OPTIONS );

			layer_config.buffermode = DLBM_BACKVIDEO;
			layer_config.options = DLOP_ALPHACHANNEL;

			DFBDisplayLayerConfigFlags failed_flags;
			if ((ret = gfxLayer->TestConfiguration(
					gfxLayer, &layer_config, &failed_flags )) != DFB_OK) {

				DirectFBError(
						"gingacc-systemio DFBDeviceScreen TestConfig error: ",
						ret );

			} else {
				DFBCHECK(gfxLayer->
						SetConfiguration(gfxLayer, &layer_config));
			}
		}
	}

	DFBDeviceScreen::~DFBDeviceScreen() {
		cout << "windowPool size = " << windowPool->size() << endl;
		cout << "surfacePool size = " << surfacePool->size() << endl;

		//Releasing still Window objects in Window Pool
		set<IDirectFBWindow*>::iterator w;
		for (w = windowPool->begin(); w != windowPool->end(); ++w) {
			if ((*w) != NULL) {
				(*w)->Release(*w);
			}
		}
		windowPool->clear();
		delete windowPool;
		windowPool = NULL;
		pthread_mutex_unlock(&winMutex);
		pthread_mutex_destroy(&winMutex);

		//Releasing still Surface objects in Surface Pool
		set<IDirectFBSurface*>::iterator s;
		for (s = surfacePool->begin(); s != surfacePool->end(); ++s) {
			if ((*s) != NULL) {
				(*s)->Release(*s);
			}
		}
		surfacePool->clear();
		delete surfacePool;
		surfacePool = NULL;
		pthread_mutex_unlock(&surMutex);
		pthread_mutex_destroy(&surMutex);

		numOfDFBScreens--;
		if (numOfDFBScreens == 0) {
			gfxLayer->Release(gfxLayer);
			dfb->Release(dfb);
			gfxLayer = NULL;
			dfb = NULL;
		}
	}

	unsigned int DFBDeviceScreen::getWidthResolution() {
		return wRes;
	}

	void DFBDeviceScreen::setWidthResolution(unsigned int wRes) {
		this->wRes = wRes;
	}

	unsigned int DFBDeviceScreen::getHeightResolution() {
		return hRes;
	}

	void DFBDeviceScreen::setHeightResolution(unsigned int hRes) {
		this->hRes = hRes;
	}

	void DFBDeviceScreen::setColorKey(int r, int g, int b) {
		if (gfxLayer != NULL) {
			DFBCHECK(gfxLayer->SetSrcColorKey(gfxLayer, r, g, b));
		}
	}

	void* DFBDeviceScreen::createWindow(void* desc) {
		IDirectFBWindow* window = NULL;

		if (gfxLayer != NULL) {
			DFBCHECK(gfxLayer->CreateWindow(
					gfxLayer, (const DFBWindowDescription*)desc, &window));

			pthread_mutex_lock(&winMutex);
			windowPool->insert(window);
			pthread_mutex_unlock(&winMutex);
		}

		return (void*)window;
	}

	void DFBDeviceScreen::releaseWindow(void* win) {
		set<IDirectFBWindow*>::iterator i;
		IDirectFBWindow* w;
		w = (IDirectFBWindow*)win;

		pthread_mutex_lock(&winMutex);
		i = windowPool->find(w);
		if (i != windowPool->end()) {
			windowPool->erase(i);
			pthread_mutex_unlock(&winMutex);

		} else {
			pthread_mutex_unlock(&winMutex);
			return;
		}

		w->Release(w);
		win = NULL;
		w = NULL;
	}

	void* DFBDeviceScreen::createSurface(void* desc) {
		IDirectFBSurface* surface;

		if (dfb != NULL) {
			DFBCHECK(dfb->CreateSurface(
					dfb, (const DFBSurfaceDescription*)desc, &surface));
		}

		pthread_mutex_lock(&surMutex);
		surfacePool->insert(surface);
		pthread_mutex_unlock(&surMutex);
		return (void*)surface;
	}

	void DFBDeviceScreen::releaseSurface(void* sur) {
		set<IDirectFBSurface*>::iterator i;
		IDirectFBSurface* s;
		s = (IDirectFBSurface*)sur;

		pthread_mutex_lock(&surMutex);
		i = surfacePool->find(s);
		if (i != surfacePool->end()) {
			surfacePool->erase(i);
			pthread_mutex_unlock(&surMutex);

		} else {
			pthread_mutex_unlock(&surMutex);
			return;
		}

		s->Clear(s, 0, 0, 0, 0);
		s->Release(s);
		s = NULL;
		sur = NULL;
	}

	void* DFBDeviceScreen::getGfxRoot() {
		return (void*)dfb;
	}

	/*
	static DFBEnumerationResult
	Graphics::display_layer_callback( DFBDisplayLayerID   id,
	                        DFBDisplayLayerDescription  desc,
	                        void                       *arg ) {
		int i;
		bool acceptVideo = false;
		bool acceptGraphics = false;
		for (i=0; layer_types[i].type; i++) {
			if (desc.caps & layer_types[i].type) {
				if (layer_types[i].name == "VIDEO") {
					acceptVideo = true;
				}
				if (layer_types[i].name == "GRAPHICS") {
					acceptGraphics = true;
				}
			}
		}

		if (acceptVideo && acceptGraphics) {
			DFBResult ret;
			IDirectFB* dfb = NULL;
			dfb = lite_get_dfb_interface();
			ret = dfb->GetDisplayLayer(dfb, id, (IDirectFBDisplayLayer**)
				    (arg));

			if (ret) {
				DirectFBError(
					    "Graphics DirectFB::GetDisplayLayer() failed",
					    ret);

				return (DFBEnumerationResult)ret;
			}
		}

		return (DFBEnumerationResult)DFB_OK;
	}
	*/
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::IDeviceScreen*
		createDFBScreen(int numArgs, char* args[]) {

	return (new ::br::pucrio::telemidia::ginga::core::system::
			io::DFBDeviceScreen(numArgs, args));
}

extern "C" void destroyDFBScreen(
		::br::pucrio::telemidia::ginga::core::system::io::IDeviceScreen* ds) {

	delete ds;
}
