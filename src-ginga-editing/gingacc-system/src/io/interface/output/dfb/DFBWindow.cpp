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

#include "util/Color.h"

#include "../../../../../include/io/DeviceManager.h"
#include "../../../../../include/io/interface/output/dfb/DFBWindow.h"
#include "../../../../../include/io/interface/output/dfb/DFBSurface.h"

/* macro for a safe call to DirectFB functions */
#ifdef DFBCHECK
#undef DFBCHECK
#endif

#ifndef DFBCHECK
/*
#define DFBCHECK(x...)                                            \
{                                                                 \
	DFBResult err = x;                                            \
	if (err != DFB_OK) {                                          \
		fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );    \
		DirectFBErrorFatal( #x, err );                            \
	}                                                             \
}
*/
#define DFBCHECK(x...)                                            \
{                                                                 \
	DFBResult err = x;                                            \
	if (err != DFB_OK) {                                          \
		fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );    \
	}                                                             \
}
#endif /*DFBCHECK*/

struct DynamicRender {
	IDirectFBSurface* destination;
	IDirectFBSurface* frame;
};

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	DFBWindow::DFBWindow(int x, int y, int width, int height) {
		this->win = NULL;
		this->winSur = NULL;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->visible = false;
		this->transparencyValue = 255;
		this->r = -1;
		this->g = -1;
		this->b = -1;
		this->alpha = 0xFF;
		this->childSurfaces = new vector<ISurface*>;
		this->releaseListener = NULL;
		this->fit = true;
		this->stretch = true;
		this->caps = DWCAPS_NODECORATION;

		pthread_mutex_init(&mutex, NULL);
		pthread_mutex_init(&mutexC, NULL);
	}

	DFBWindow::~DFBWindow() {
		ISurface* surface;
		vector<ISurface*>::iterator i;

		lock();
		lockChilds();
		if (releaseListener != NULL) {
			releaseListener->setParent(NULL);
		}

		if (childSurfaces != NULL) {
			i = childSurfaces->begin();
			while (i != childSurfaces->end()) {
				surface = *i;
				if (surface != NULL) {
					surface->setParent(NULL);
				}
				++i;
			}
			delete childSurfaces;
			childSurfaces = NULL;
		}
		unlockChilds();

		if (winSur != NULL) {
			DeviceManager::getInstance()->releaseSurface(winSur);
			winSur = NULL;
		}

		if (win != NULL) {
			DeviceManager::getInstance()->releaseWindow(win);
			win = NULL;
		}
		unlock();

		pthread_mutex_unlock(&mutex);
		pthread_mutex_unlock(&mutexC);

		pthread_mutex_destroy(&mutex);
		pthread_mutex_destroy(&mutexC);
	}

	void DFBWindow::setReleaseListener(ISurface* listener) {
		this->releaseListener = listener;
	}

	int DFBWindow::getCap(string cap) {
		if (cap == "ALL") {
			return DWCAPS_ALL;
		} else if (cap == "NOSTRUCTURE") {
			return DWCAPS_NODECORATION;
		} else if (cap == "ALPHACHANNEL") {
			return DWCAPS_ALPHACHANNEL;
		} else {
			return DWCAPS_NONE;
		}
	}

	void DFBWindow::setCaps(int caps) {
		this->caps = caps;
	}

	void DFBWindow::addCaps(int capability) {
		this->caps = (this->caps | capability);
	}

	int DFBWindow::getCaps() {
		return caps;
	}

	void DFBWindow::draw() {
		if (win != NULL) {
			cout << "DFBWindow::draw Warning! Requesting redraw" << endl;

		} else {
			DFBWindowDescription dsc;

			dsc.flags  = (DFBWindowDescriptionFlags)(
				    DWDESC_POSX |
				    DWDESC_POSY |
				    DWDESC_WIDTH |
				    DWDESC_HEIGHT | DWDESC_CAPS);

			dsc.posx   = x;
			dsc.posy   = y;
			dsc.width  = width;
			dsc.height = height;
			dsc.caps  = (DFBWindowCapabilities)(caps);

			win = (IDirectFBWindow*)(
					DeviceManager::getInstance()->createWindow(&dsc));

			if (win != NULL) {
				DFBCHECK(win->SetOpacity(win, 0x00));
				DFBCHECK(win->GetSurface(win, &winSur));
			}
		}

		if (win != NULL && (caps & DWCAPS_ALPHACHANNEL)) {
			DFBCHECK(win->SetOptions(win, (DFBWindowOptions)DWOP_ALPHACHANNEL));
		}

		setBackgroundColor(r, g, b, alpha);
	}

	void DFBWindow::setBounds(int posX, int posY, int w, int h) {
		if (win == NULL) {
			this->x = posX;
			this->y = posY;
			this->width = w;
			this->height = h;
			return;
		}

		this->x = posX;
		this->y = posY;
		this->width = w;
		this->height = h;

		lock();
		if (win != NULL) {
			DFBCHECK(win->SetBounds(win, x, y, width, height));
		}
		unlock();
	}

	void DFBWindow::setBackgroundColor(int r, int g, int b, int alpha) {
		//wclog << this << ">> DFBWindow::setBackgroundColor" << endl;
		if (win == NULL) {
			return;
		}

		if (winSur != NULL) {
			this->r = r;
			this->g = g;
			this->b = b;
			this->alpha = alpha;

			if (r < 0 || g < 0 || b < 0) {
				if (caps & DWCAPS_ALPHACHANNEL) {
					DFBCHECK(win->SetOptions(win, (DFBWindowOptions)
						    (DWOP_ALPHACHANNEL)));
				}
				DFBCHECK(winSur->SetColor(winSur, 0x00, 0x00, 0x00, 0x00));
				DFBCHECK(winSur->Clear(winSur, 0x00, 0x00, 0x00, 0x00));

			} else {
				//DFBCHECK(win->SetOptions(win, (DFBWindowOptions)(DWOP_OPAQUE_REGION)));
				DFBCHECK(winSur->Clear(winSur, r, g, b, alpha));
				DFBCHECK(winSur->SetColor(winSur, r, g, b, alpha));
				DFBCHECK(winSur->FillRectangle(winSur, 0, 0, width, height));
			}

			DFBCHECK(winSur->FillRectangle(winSur, 0, 0, width, height));
			DFBCHECK(winSur->Flip(winSur, NULL, (DFBSurfaceFlipFlags)0));
		}
	}

	IColor* DFBWindow::getBgColor() {
		return new Color(r, g, b, alpha);
	}

	void DFBWindow::setColorKey(int r, int g, int b) {
		//cout << this << ">> DFBWindow::setColorKey" << endl;
		//lock();
		if (win == NULL) {
			return;
		}

		DFBCHECK(win->SetColorKey(win, r, g, b));
		if (caps & DWCAPS_ALPHACHANNEL) {
			DFBCHECK(win->SetOptions(win, (DFBWindowOptions)
				    (DWOP_COLORKEYING | DWOP_ALPHACHANNEL)));

		} else {
			DFBCHECK(win->SetOptions(
					win, (DFBWindowOptions)(DWOP_COLORKEYING)));
		}

		//unlock();
		//setBackgroundColor(r, g, b, alpha);
	}

	void DFBWindow::moveTo(int posX, int posY) {
		//wclog << this << ">> DFBWindow::moveTo" << endl;

		if (win == NULL) {
			return;
		}

		this->x = posX;
		this->y = posY;

		lock();
		if (win != NULL) {
			DFBCHECK(win->MoveTo(win, x, y));
		}
		unlock();
	}

	void DFBWindow::resize(int width, int height) {
		//wclog << this << ">> DFBWindow::resize" << endl;
		if (win == NULL) {
			return;
		}

		this->width = width;
		this->height = height;

		lock();
		if (win != NULL) {
			DFBCHECK(win->Resize(win, width, height));
		}
		unlock();
	}

	void DFBWindow::raise() {
		if (win != NULL) {
			DFBCHECK(win->Raise(win));
		}
	}

	void DFBWindow::lower() {
		if (win != NULL) {
			DFBCHECK(win->Lower(win));
		}
	}

	void DFBWindow::raiseToTop() {
		if (win != NULL) {
			DFBCHECK(win->RaiseToTop(win));
		}
	}

	void DFBWindow::lowerToBottom() {
		if (win != NULL) {
			DFBCHECK(win->LowerToBottom(win));
		}
	}

	void DFBWindow::setCurrentTransparency(int alpha) {
		if (alpha != 0) {
			this->visible = true;

		} else {
			this->visible = false;
		}

		lock();
		if (win != NULL) {
			win->SetOpacity(win, alpha);
		}
		unlock();
	}

	void DFBWindow::setOpaqueRegion(int x1, int y1, int x2, int y2) {
		lock();
		if (win != NULL) {
			win->SetOpaqueRegion(win, x1, y1, x2, y2);
		}
		unlock();
	}

	void DFBWindow::setTransparencyValue(int alpha) {
		this->transparencyValue = alpha;
	}

	int DFBWindow::getTransparencyValue() {
		return this->transparencyValue;
	}

	void DFBWindow::show() {
		this->visible = true;

		if (win != NULL) {
			win->SetOpacity(win, transparencyValue);
		}
	}

	void DFBWindow::hide() {
		this->visible = false;

		lock();
		if (win != NULL) {
			DFBCHECK(win->SetOpacity(win, 0x00));
		}
		unlock();
	}

	int DFBWindow::getX() {
		return this->x;
	}

	int DFBWindow::getY() {
		return this->y;
	}

	int DFBWindow::getW() {
		return this->width;
	}

	int DFBWindow::getH() {
		return this->height;
	}

	void* DFBWindow::getContent() {
		return win;
	}
/*
	IDirectFBSurface* DFBWindow::getContentSurface() {
		if (win != NULL) {
			IDirectFBSurface* s;
			DFBCHECK(win->GetSurface(win, &s));
			return s;

		} else {
			return NULL;
		}
	}
*/
	void DFBWindow::setColor(int r, int g, int b, int alpha) {
		if (win != NULL && winSur != NULL) {
			DFBCHECK(winSur->SetColor(winSur, r, g, b, alpha));
		}
	}

	void DFBWindow::setBorder(int r, int g, int b, int alpha, int bWidth) {
		int i;

		//lock();
		if (win != NULL && winSur != NULL) {
			DFBCHECK(winSur->SetColor(winSur, r, g, b, alpha));
			if (bWidth < 0) {
				bWidth = bWidth * -1;
			}

			for (i=0; i < bWidth; i++) {
				DFBCHECK(winSur->DrawRectangle(
						winSur, i, i, width - (2*i), height - (2*i)));
			}

			DFBCHECK(winSur->Flip(winSur, NULL, (DFBSurfaceFlipFlags)0));
		}
		//unlock();
	}

	void DFBWindow::setBorder(IColor* color, int bWidth) {
		setBorder(
			    color->getR(),
			    color->getG(),
			    color->getB(),
			    color->getAlpha(),
			    bWidth);
	}

	bool DFBWindow::isVisible() {
		return this->visible;
	}

	void DFBWindow::validate() {
		if (win != NULL && winSur != NULL) {
			if (winSur != NULL) {
				lockChilds();
				if (childSurfaces != NULL && !childSurfaces->empty()) {
					ISurface* surface;
					surface = childSurfaces->at(0);
					if (surface != NULL) {
						renderFrom(surface);
					}

				} else {
					DFBCHECK(winSur->Flip(
							winSur, NULL, (DFBSurfaceFlipFlags)0));
				}
				unlockChilds();
			}
		}
		//wclog << "DFBWindow::validate " << endl;
	}

	void DFBWindow::addChildSurface(ISurface* s) {
		int i;
		ISurface* surface;

		lockChilds();
		for (i = 0; i < childSurfaces->size(); i++) {
			surface = childSurfaces->at(i);
			if (surface == s) {
				unlockChilds();
				return;
			}
		}
		childSurfaces->push_back(s);
		unlockChilds();
	}

	bool DFBWindow::removeChildSurface(ISurface* s) {
		int i;
		vector<ISurface*>::iterator j;
		ISurface* surface;

		lockChilds();
		if (releaseListener == s) {
			releaseListener = NULL;
		}

		if (childSurfaces == NULL) {
			unlockChilds();
			return false;
		}

		for (i=0; i < childSurfaces->size(); i++) {
			surface = childSurfaces->at(i);
			if (surface == s) {
				j = childSurfaces->begin() + i;
				childSurfaces->erase(j);
				unlockChilds();
				return true;
			}
		}
		unlockChilds();
		return false;
	}

	void DFBWindow::setStretch(bool stretchTo) {
		this->stretch = stretchTo;
	}

	bool DFBWindow::getStretch() {
		return this->stretch;
	}

	void DFBWindow::setFit(bool fitTo) {
		this->fit = fitTo;
	}

	bool DFBWindow::getFit() {
		return this->fit;
	}

	void DFBWindow::clearContent() {
		DFBCHECK(winSur->Clear(winSur, 0, 0, 0, 0));
	}

	bool DFBWindow::isMine(ISurface* surface) {
		IDirectFBSurface* contentSurface;

		if (win == NULL || winSur == NULL || surface == NULL) {
			return false;
		}

		contentSurface = (IDirectFBSurface*)(surface->getContent());
		if (contentSurface == winSur) {
			return true;
		}
		return false;
	}

	void DFBWindow::renderFrom(ISurface* surface) {
		IDirectFBSurface* contentSurface, *s;
		int w, h;

		lock();
		if (win != NULL && !isMine(surface)) {
			contentSurface = (IDirectFBSurface*)(surface->getContent());
			if (contentSurface == NULL) {
				unlock();
				return;
			}

			DFBCHECK(contentSurface->GetSize(contentSurface, &w, &h));
			if (winSur != NULL && winSur != contentSurface) {
				DFBCHECK(winSur->Clear(winSur, 0, 0, 0, 0));
				if ((w != width || h != height) && fit) {
					if (stretch) {
						DFBCHECK(winSur->StretchBlit(
								winSur, contentSurface, NULL, NULL));

					} else {
						ISurface* sur;
						IDirectFBSurface* s2;
						sur = new DFBSurface(width, height);
						s2 = (IDirectFBSurface*)(sur->getContent());

						DFBCHECK(s2->StretchBlit(
							    s2,
							    contentSurface,
							    NULL,
							    NULL));

						DFBCHECK(s2->Flip(
								s2, NULL, (DFBSurfaceFlipFlags)DSFLIP_BLIT));

						DFBCHECK(winSur->Blit(winSur, s2, NULL, 0, 0));
						DFBCHECK(winSur->Flip(
								winSur,
								NULL, (DFBSurfaceFlipFlags) DSFLIP_BLIT));

						delete sur;
					}

				} else {
					DFBCHECK(winSur->Blit(winSur, contentSurface, NULL, 0, 0));
				}
				DFBCHECK(winSur->Flip(winSur, NULL, (DFBSurfaceFlipFlags)0));
			}
		}
		unlock();
	}

	void DFBWindow::lock() {
		pthread_mutex_lock(&mutex);
	}

	void DFBWindow::unlock() {
		pthread_mutex_unlock(&mutex);
	}

	void DFBWindow::lockChilds() {
		pthread_mutex_lock(&mutexC);
	}

	void DFBWindow::unlockChilds() {
		pthread_mutex_unlock(&mutexC);
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::IWindow*
		createDFBWindow(int x, int y, int width, int height) {

	return new ::br::pucrio::telemidia::ginga::core::system::io::DFBWindow(
			x, y, width, height);
}

extern "C" void destroyDFBWindow(
		::br::pucrio::telemidia::ginga::core::system::io::IWindow* w) {

	delete w;
}
