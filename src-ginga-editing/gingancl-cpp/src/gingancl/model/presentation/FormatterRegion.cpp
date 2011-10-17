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

#include "../../../include/CascadingDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "../../../include/FormatterRegion.h"
#include "../../../include/FormatterLayout.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
	FormatterRegion::FormatterRegion(
		    string objectId, void* descriptor, void* layoutManager) {

		this->layoutManager = layoutManager;
		this->objectId = objectId;
		this->descriptor = descriptor;

		originalRegion = NULL;
		if (descriptor != NULL) {
			originalRegion = ((CascadingDescriptor*)descriptor)->getRegion();
		}

		if (originalRegion != NULL) {
			ncmRegion = originalRegion->cloneRegion();

		} else {
			ncmRegion = NULL;
		}

		this->outputDisplay = NULL;
		this->focusState = FormatterRegion::UNSELECTED;
		this->focusBorderColor = new Color("white");
		this->focusBorderWidth = 0;
		this->focusComponentSrc = "";
		this->selBorderColor = new Color("red");
		this->selBorderWidth = 0;
		this->selComponentSrc = "";
		this->chromaKey = NULL;
		this->transitionIn = "";
		this->transitionOut = "";
		this->abortTransitionIn = false;
		this->abortTransitionOut = false;

		pthread_mutex_init(&mutex, NULL);
		pthread_mutex_init(&mutexT, NULL);

		// TODO: look for descriptor parameters overriding region attributes
		string value;
		value = ((CascadingDescriptor*)descriptor)->getParameterValue(
			    "transparency");

		if (value != "") {
			transparency = stof(value);
			if (transparency < 0.0 || transparency > 1.0) {
				transparency = 1.0;
			}

		} else {
			transparency = 1.0;
		}

		value = ((CascadingDescriptor*)descriptor)->getParameterValue(
			    "background");

		if (value != "") {
			if (upperCase(value) == "TRANSPARENT") {
				bgColor = NULL;

			} else {
				bgColor = new Color(value, (int)(transparency * 255));
			}

		} else {
			bgColor = NULL;
		}

		fit = Descriptor::FIT_FILL;
		value = ((CascadingDescriptor*)descriptor)->getParameterValue("fit");
		if (value != "") {
			fit = DescriptorUtil::getFitCode(value);
			if (fit < 0) {
				fit = Descriptor::FIT_FILL;
			}
		}

		scroll = Descriptor::SCROLL_NONE;
		value = ((CascadingDescriptor*)descriptor)->
			    getParameterValue("scroll");

		if (value != "") {
			scroll = DescriptorUtil::getScrollCode(value);
			if (scroll < 0) {
				scroll = Descriptor::SCROLL_NONE;
			}
		}

		value = ((CascadingDescriptor*)descriptor)->getParameterValue(
			    "chromakey");

		if (value != "") {
			chromaKey = new Color(value);
		}

		value = ((CascadingDescriptor*)descriptor)->getParameterValue(
			    "x-rgbChromakey");

		if (value != "") {
			vector<string>* params;

			params = split(value, ",");
			if (params->size() == 3) {
				chromaKey = new Color(
						(int)stof((*params)[0]),
						(int)stof((*params)[1]),
						(int)stof((*params)[2]));
			}
			delete params;
		}

		value = ((CascadingDescriptor*)descriptor)->getParameterValue(
			    "transitionIn");

		if (value != "") {
			transitionIn = value;
		}

		value = ((CascadingDescriptor*)descriptor)->getParameterValue(
			    "transitionOut");

		if (value != "") {
			transitionOut = value;
		}

		//TODO: fit and scroll
	}

	FormatterRegion::~FormatterRegion() {
		lock();
		layoutManager = NULL;
		descriptor = NULL;

		originalRegion = NULL;
		if (ncmRegion != NULL) {
			delete ncmRegion;
			ncmRegion = NULL;
		}

		if (outputDisplay != NULL) {
			delete outputDisplay;
			outputDisplay = NULL;
		}

		if (focusBorderColor != NULL) {
			delete focusBorderColor;
			focusBorderColor = NULL;
		}

		if (selBorderColor != NULL) {
			delete selBorderColor;
			selBorderColor = NULL;
		}

		if (bgColor != NULL) {
			delete bgColor;
			bgColor = NULL;
		}

		if (chromaKey != NULL) {
			delete chromaKey;
			chromaKey = NULL;
		}
		unlock();
		pthread_mutex_destroy(&mutex);
		lockTransition();
		unlockTransition();
		pthread_mutex_destroy(&mutexT);
	}

	void FormatterRegion::meetComponent(
		    int width,
		    int height,
		    int prefWidth,
		    int prefHeight,
		    Surface* component) {

		int finalH, finalW;

		if (prefWidth == 0 || prefHeight == 0) {
			return;
		}

		finalH = (prefHeight * width) / prefWidth;
		if (finalH <= height) {
			finalW = width;

		} else {
			finalH = height;
			finalW = (prefWidth * height) / prefHeight;
		}
		//component->setSize(finalW, finalH);
	}

	void FormatterRegion::sliceComponent(
		    int width,
		    int height,
		    int prefWidth,
		    int prefHeight,
		    Surface* component) {

		int finalH, finalW;

		if (prefWidth == 0 || prefHeight == 0) {
			return;
		}

		finalH = (prefHeight * width) / prefWidth;
		if (finalH > height) {
			finalW = width;

		} else {
			finalH = height;
			finalW = (prefWidth * height) / prefHeight;
		}

		//component->setSize(finalW, finalH);
	}

	void FormatterRegion::updateCurrentComponentSize() {
		//int prefWidth, prefHeight, width, height;

		wclog << "FormatterRegion::updateCurrentComponentSize()" << endl;
		//sizeRegion();

		switch (fit) {	
			case Descriptor::FIT_HIDDEN:
				/*currentComponent.setSize(
					(int)currentComponent.getPreferredSize().getWidth(),
					(int)currentComponent.getPreferredSize().getHeight());*/
				break;

			case Descriptor::FIT_MEET:
/*				prefWidth = (int)currentComponent.
					    getPreferredSize().getWidth();

				prefHeight = (int)currentComponent.
					    getPreferredSize().getHeight();

				width = outputDisplay.getWidth();
				height = outputDisplay.getHeight();
				meetComponent(
					    width,
					    height,
					    prefWidth,
					    prefHeight,
					    currentComponent);*/

				break;
				
			case Descriptor::FIT_MEETBEST:
				/*prefWidth = (int)currentComponent.
					    getPreferredSize().getWidth();

				prefHeight = (int)currentComponent.
					    getPreferredSize().getHeight();

				width = outputDisplay.getWidth();
				height = outputDisplay.getHeight();

				// the scale factor must not overtake 100% (2 times)
				if ((2 * prefWidth) >= width &&	(2 * prefHeight) >= height) {
					meetComponent(
						    width,
						    height,
						    prefWidth,
						    prefHeight,
						    currentComponent);

				}*/

				break;

			case Descriptor::FIT_SLICE:
				/*prefWidth = (int)currentComponent->
					    getPreferredSize().getWidth();

				prefHeight = (int)currentComponent->
					    getPreferredSize().getHeight();

				width = outputDisplay.getWidth();
				height = outputDisplay.getHeight();
				sliceComponent(
					    width,
					    height,
					    prefWidth,
					    prefHeight,
					    currentComponent);*/

				break;
				
			case Descriptor::FIT_FILL:
			default:
				/*currentComponent->setSize(
						outputDisplay.getWidth(), 
						outputDisplay.getHeight());*/
				break;
		}
	}

	void FormatterRegion::updateCurrentComponentLocation() {
		//currentComponent->setLocation(0, 0);
	}

	void FormatterRegion::updateRegionBounds() {
		sizeRegion();
		if (outputDisplay != NULL) {
			//updateCurrentComponentSize();
    		//updateCurrentComponentLocation();
			if (focusState == FormatterRegion::UNSELECTED) {
				unselect();

			} else { //is focused (at least)
				setFocus(true);
				// if is focused and selected
				if (focusState == FormatterRegion::SELECTED) {
					setSelection(true);
				}
			}

			//outputDisplay->raiseToTop();
			/*if (ncmRegion != NULL) {
				bringChildrenToFront(ncmRegion);
			}

			bringSiblingToFront(this);*/
		}
	}

	void FormatterRegion::sizeRegion() {
		int left = 0;
		int top = 0;
		int width = 0;
		int height = 0;

		if (ncmRegion != NULL) {
			left = ncmRegion->getLeftInPixels();
			top = ncmRegion->getTopInPixels();
			width = ncmRegion->getWidthInPixels();
			height = ncmRegion->getHeightInPixels();
		}

		/*wclog << "FormatterRegion::sizeRegion windowAdd = '" << outputDisplay;
		wclog << "' x = '" << left;
		wclog << "' y = '" << top;
		wclog << "' w = '" << width;
		wclog << "' h = '" << height << "'" << endl;*/

		if (left < 0)
			left = 0;

		if (top < 0)
			top = 0;

		if (width <= 0)
			width = 1;

		if (height <= 0)
			height = 1;

		lock();
		if (outputDisplay != NULL) {
			outputDisplay->setBounds(left, top, width, height);
		}
		unlock();
	}

	LayoutRegion* FormatterRegion::getLayoutRegion() {
		return ncmRegion;
	}

	LayoutRegion* FormatterRegion::getOriginalRegion() {
		return originalRegion;
	}

	Window* FormatterRegion::getODContentPane() {
		if (outputDisplay == NULL) {
			cout << "FormatterRegion::getODContentPane(" << this << ")";
			cout << " Warning! outputdisplay = NULL" << endl;
		}
		return outputDisplay;
	}

	void FormatterRegion::prepareOutputDisplay(Surface* renderedSurface) {
		lock();
		if (outputDisplay == NULL) {
			string title;
			int left = 0;
			int top = 0;
			int width = 0;
			int height = 0;

			if (ncmRegion == NULL) {
				title = objectId;

			} else {
				if (ncmRegion->getTitle() == "") {
					title = objectId;

				} else {
					title = ncmRegion->getTitle();
				}

				left = ncmRegion->getAbsoluteLeft();
				top = ncmRegion->getAbsoluteTop();
				width = ncmRegion->getWidthInPixels();
				height = ncmRegion->getHeightInPixels();
			}

			/*wclog << "FormatterRegion::prepareOutputDisplay";
			wclog << " regionId = " << ncmRegion->getId();
			wclog << " ax = " << left;
			wclog << " ay = " << top;
			wclog << " wip = " << width;
			wclog << " w = " << ncmRegion->getWidth();
			wclog << " hip = " << height;
			wclog << " h = " << ncmRegion->getHeight();
			wclog << endl;

			LayoutRegion *parentRegion, *grandParent;
			parentRegion = ncmRegion->getParent();
			wclog << "FormatterRegion::prepareOutputDisplay";
			wclog << " parentId = " << parentRegion->getId();
			wclog << " ax = " << parentRegion->getAbsoluteLeft();
			wclog << " ay = " << parentRegion->getAbsoluteTop();
			wclog << " wip = " << parentRegion->getWidthInPixels();
			wclog << " w = " << parentRegion->getWidth();
			wclog << " hip = " << parentRegion->getHeightInPixels();
			wclog << " h = " << parentRegion->getHeight();
			wclog << endl;
			grandParent = parentRegion->getParent();
			while (grandParent != NULL) {
				parentRegion = grandParent;
				grandParent = grandParent->getParent();
			}
			wclog << "FormatterRegion::prepareOutputDisplay";
			wclog << " grandParentId = " << parentRegion->getId();
			wclog << " ax = " << parentRegion->getAbsoluteLeft();
			wclog << " ay = " << parentRegion->getAbsoluteTop();
			wclog << " wip = " << parentRegion->getWidthInPixels();
			wclog << " w = " << parentRegion->getWidth();
			wclog << " hip = " << parentRegion->getHeightInPixels();
			wclog << " h = " << parentRegion->getHeight();
			wclog << endl;*/

			if (left < 0)
				left = 0;

			if (top < 0)
				top = 0;

			if (width <= 0)
				width = 1;

			if (height <= 0)
				height = 1;

			outputDisplay = new Window(left, top, width, height);
			if (bgColor != NULL) {
				outputDisplay->setBackgroundColor(
					    bgColor->getR(),
					    bgColor->getG(),
					    bgColor->getB());
			}

			outputDisplay->setTransparencyValue((int)(transparency * 255));

			if (renderedSurface->getCaps() & Window::CAPS_ALPHACHANNEL) {
				outputDisplay->addCaps(Window::CAPS_ALPHACHANNEL);
			}

			if (chromaKey != NULL) {
				outputDisplay->setCaps(Window::CAPS_NOSTRUCTURE);
				outputDisplay->draw();
				outputDisplay->setColorKey(
					    chromaKey->getR(),
					    chromaKey->getG(),
					    chromaKey->getB());

			} else {
				outputDisplay->draw();
			}

			if (scroll != Descriptor::SCROLL_NONE) {
				//int vertPolicy, horzPolicy;
				switch (scroll) {
					case Descriptor::SCROLL_HORIZONTAL:
						/*vertPolicy = JScrollPane.
							    VERTICAL_SCROLLBAR_NEVER;

						horzPolicy = JScrollPane.
							    HORIZONTAL_SCROLLBAR_ALWAYS;*/
						break;
						
					case Descriptor::SCROLL_VERTICAL:
						/*vertPolicy = JScrollPane.
							    VERTICAL_SCROLLBAR_ALWAYS;

						horzPolicy = JScrollPane.
							    HORIZONTAL_SCROLLBAR_NEVER;*/

						break;

					case Descriptor::SCROLL_BOTH:
						/*vertPolicy = JScrollPane.
							    VERTICAL_SCROLLBAR_ALWAYS;

						horzPolicy = JScrollPane.
							    HORIZONTAL_SCROLLBAR_ALWAYS;*/

						break;

					case Descriptor::SCROLL_AUTOMATIC:
						/*vertPolicy = JScrollPane.
							    VERTICAL_SCROLLBAR_AS_NEEDED;

						horzPolicy = JScrollPane.
							    HORIZONTAL_SCROLLBAR_AS_NEEDED;*/
						break;

					default:
						/*vertPolicy = JScrollPane.
							    VERTICAL_SCROLLBAR_NEVER;

						horzPolicy = JScrollPane.
							    HORIZONTAL_SCROLLBAR_NEVER;*/
						break;
				}

				/*scrollPane = new JScrollPane(vertPolicy, horzPolicy);
				scrollPane.getViewport().setBackground(bgColor);
				contentPane.add(scrollPane);*/
			}

		}

		/*if (component != null) {
			updateCurrentComponentSize();
			updateCurrentComponentLocation();
		}
		if (scrollPane != null) {
			scrollPane.setViewportView(currentComponent);
		}
		else {
			contentPane.add(currentComponent);
		}*/
		unlock();
		renderSurface(renderedSurface);
	}

	void FormatterRegion::renderSurface(Surface* renderedSurface) {
		lock();
		if (renderedSurface != NULL && outputDisplay != NULL) {
			if (renderedSurface->setParent((void*)outputDisplay)) {
				outputDisplay->renderFrom(renderedSurface);
			}
		}
		unlock();
	}

	void FormatterRegion::showContent() {
		string value;
		/*struct transT* t;
		pthread_t threadId_;*/

		lockTransition();
		value = ((CascadingDescriptor*)descriptor)->getParameterValue(
			    "visible");

		abortTransitionIn = false;
		abortTransitionOut = true;
		if (value == "" || upperCase(value) != "FALSE") {
			vector<Transition*>* transitions;
			transitions = ((CascadingDescriptor*)descriptor)->
				    getInputTransitions();

			if (!transitions->empty()) {
				wclog << endl;
				Transition* transition;
				int transitionType;
				unsigned int i;
				for (i = 0; i < transitions->size(); i++) {
					transition = (*transitions)[i];
					transitionType = transition->getType();
					if (transitionType == Transition::TYPE_FADE) {
						toFront();

						fade(transition, true);
						unlockTransition();
						/*t = new struct transT;
						t->fr = this;
						t->t = transition;

						//show with fade transition type
						pthread_create(
								&threadId_,
								0, FormatterRegion::fadeT, (void*)t);

						pthread_detach(threadId_);*/
						return;

					} else if (transitionType == Transition::TYPE_BARWIPE) {
						toFront();

						barWipe(transition, true);
						unlockTransition();
						/*t = new struct transT;
						t->fr = this;
						t->t = transition;

						//show with barwipe transition type
						pthread_create(
								&threadId_,
								0, FormatterRegion::barWipeT, (void*)t);

						pthread_detach(threadId_);*/
						return;
					}
				}
			}

			toFront();
			unlockTransition();
			setRegionVisibility(true);

		} else {
			unlockTransition();
		}
	}

	void FormatterRegion::hideContent() {
		abortTransitionIn = true;
		abortTransitionOut = false;
		lockTransition();
		if (outputDisplay != NULL && outputDisplay->isVisible()) {
			vector<Transition*>* transitions;
			transitions = ((CascadingDescriptor*)descriptor)->
				    getOutputTransitions();

			if (!transitions->empty()) {
				unsigned int i;
				Transition* transition;
				int transitionType;

				for (i = 0; i < transitions->size(); i++) {
					transition = (*transitions)[i];
					transitionType = transition->getType();
					if (transitionType == Transition::TYPE_FADE) {
						//hide with transition type
						fade(transition, false);
						unlockTransition();
						return;

					} else if (transitionType == Transition::TYPE_BARWIPE) {
						//hide with transition type
						barWipe(transition, false);
						unlockTransition();
						return;
					}
				}
			}
			setRegionVisibility(false);
			abortTransitionIn = true;
			abortTransitionOut = true;
			disposeOutputDisplay();
			unlockTransition();

		} else {
			if (outputDisplay == NULL) {
				cout << "FormatterRegion::hideContent(" << this << ")";
				cout << " display is NULL" << endl;

			} else {
				cout << "FormatterRegion::hideContent(" << this << ")";
				cout << " display is not visible" << endl;
				abortTransitionIn = true;
				abortTransitionOut = true;
				disposeOutputDisplay();
			}
			unlockTransition();
		}
	}

	void FormatterRegion::setRegionVisibility(bool visible) {
		lock();
		if (outputDisplay != NULL) {
			if (!visible) {
				outputDisplay->hide();

			} else {
				outputDisplay->show();
			}

		} else {
			cout << "FormatterRegion::setRegionVisibility warning! ";
			cout << "outputdisplay = NULL. links???"<< endl;
		}
		unlock();
	}

	void FormatterRegion::disposeOutputDisplay() {
		lock();
		if (outputDisplay != NULL) {
			outputDisplay->hide();
			delete outputDisplay;
			outputDisplay = NULL;
		}
		unlock();
	}

	void FormatterRegion::toFront() {
		if (outputDisplay != NULL) {
			outputDisplay->raiseToTop();
			if (ncmRegion != NULL) {
				bringChildrenToFront(ncmRegion);
			}
			bringSiblingToFront(this);
		}
	}

	void FormatterRegion::bringChildrenToFront(LayoutRegion* parentRegion) {
		vector<LayoutRegion*>* regions = NULL;
		vector<LayoutRegion*>::iterator i;
		vector<FormatterRegion*>* formRegions = NULL;
		vector<FormatterRegion*>::iterator j;
		LayoutRegion* layoutRegion;
		FormatterRegion* region;

		if (parentRegion != NULL) {
			regions = parentRegion->getRegionsSortedByZIndex();
		}

		/*wclog << endl << endl << endl;
		wclog << "DEBUG REGIONS SORTED BY ZINDEX parentID: ";
		wclog << parentRegion->getId() << endl;
		i = regions->begin();
		while (i != regions->end()) {
			wclog << "region: " << (*i)->getId();
			wclog << "zindex: " << (*i)->getZIndex();
			++i;
		}
		wclog << endl << endl << endl;*/

		i = regions->begin();
		while (i != regions->end()) {
			layoutRegion = *i;

			if (layoutRegion != NULL) {
				bringChildrenToFront(layoutRegion);
				formRegions = ((FormatterLayout*)layoutManager)->
					    getFormatterRegionsFromNcmRegion(
					    	    layoutRegion->getId());

				if (formRegions != NULL) {
					j = formRegions->begin();
					while (j != formRegions->end()) {
						region = *j;
						if (region != NULL) {
							region->toFront();

						} else {
							wclog << "FormatterRegion::";
							wclog << "bringChildrenToFront";
							wclog << " Warning! region == NULL";
						}

						++j;
					}

					delete formRegions;
					formRegions = NULL;
				}
			}
			++i;
		}

		delete regions;
		regions = NULL;
	}

	void FormatterRegion::traverseFormatterRegions(
		    LayoutRegion *region, LayoutRegion *baseRegion) {

		LayoutRegion* auxRegion;
		vector<FormatterRegion*>* formRegions;
		FormatterRegion *formRegion;
		//Window *regionRect;
		vector<FormatterRegion*>::iterator it;

		formRegions = ((FormatterLayout*)layoutManager)->
			    getFormatterRegionsFromNcmRegion(region->getId());

		if (formRegions != NULL) {
			it = formRegions->begin();
			while (it != formRegions->end()) {
				formRegion = *it;
				if (formRegion != NULL) {
					auxRegion = formRegion->getLayoutRegion();
					if (ncmRegion->intersects(auxRegion) &&
							ncmRegion != auxRegion) {

						formRegion->toFront();
					}

				} else {
					cout << "FormatterRegion::traverseFormatterRegion";
					cout << " Warning! formRegion == NULL" << endl;
				}
				/*
				wclog << "FormatterRegion::traverseFormatterRegion toFront = ";
				wclog << "'" << formRegion->getLayoutRegion()->getId();
				wclog << "'" << endl;
				*/
				++it;
			}
			delete formRegions;
			formRegions = NULL;

		} else {
			bringHideWindowToFront(baseRegion, region);
		}
	}

	void FormatterRegion::bringHideWindowToFront(
		    LayoutRegion *baseRegion, LayoutRegion *hideRegion) {

		vector<LayoutRegion*> *regions;
		LayoutRegion *region;
		vector<LayoutRegion*>::iterator it;

		// dummyRegion = new FormatterRegion(NULL, hideRegion);
		// regionRect = dummyRegion->getRegionRectangle();
		// if(baseRect.intersects(regionRect))
		if (ncmRegion->intersects(hideRegion) && ncmRegion != hideRegion) {
			regions = hideRegion->getRegions();
			if (regions != NULL) {
				for (it = regions->begin(); it != regions->end(); ++it) {
					region = *it;
					traverseFormatterRegions(region, baseRegion);
				}
			}
			delete regions;
			regions = NULL;
		}
	}

	void FormatterRegion::bringSiblingToFront(FormatterRegion *region) {
		Window *regionRect;
		LayoutRegion *layoutRegion, *parentRegion, *baseRegion, *siblingRegion;
		vector<LayoutRegion*> *regions;
		vector<LayoutRegion*>::iterator it;

		regionRect = region->getRegionRectangle();
		layoutRegion = region->getOriginalRegion();
		if (layoutRegion == NULL) {
			return;
		}
		parentRegion = layoutRegion->getParent();
		baseRegion = layoutRegion;

		while (parentRegion != NULL) {
			regions = parentRegion->getRegionsOverRegion(baseRegion);
			for (it=regions->begin(); it!=regions->end(); ++it) {
				siblingRegion = *it;
				traverseFormatterRegions(siblingRegion, layoutRegion);
			}
			baseRegion = parentRegion;
			delete regions;
			regions = NULL;
			parentRegion = parentRegion->getParent();
		}
	}

	void FormatterRegion::windowGainedFocus() {
		if (ncmRegion != NULL) {
			bringChildrenToFront(ncmRegion);
		}
		bringSiblingToFront(this);
		lock();
		if (outputDisplay != NULL) {
			outputDisplay->validate();
		}
		unlock();
	}

	Window* FormatterRegion::getRegionRectangle() {
		//TODO: check if is this method usefull
		return NULL;
	}

	bool FormatterRegion::isVisible() {
		if (outputDisplay != NULL) {
			return outputDisplay->isVisible();
		}
		return false;
	}

	short FormatterRegion::getFocusState() {
		return focusState;
	}

	bool FormatterRegion::setSelection(bool selOn) {
		lock();
		if (selOn && focusState == FormatterRegion::SELECTED ||
			    outputDisplay == NULL) {

			unlock();
			return false;
		}

		if (selOn) {
			focusState = FormatterRegion::SELECTED;
			if (selComponentSrc != "") {
				Surface* selSurface = NULL;
				selSurface = FocusSourceManager::getComponent(selComponentSrc);

				if (selSurface != NULL && outputDisplay != NULL) {
					outputDisplay->renderFrom(selSurface);
					delete selSurface;
					selSurface = NULL;
				}
			}

			if (outputDisplay != NULL) {
				outputDisplay->setBorder(selBorderColor, selBorderWidth);
			}
			unlock();

		} else {
			unlock();
			unselect();
		}

		return selOn;
	}

	void FormatterRegion::setFocus(bool focusOn) {
		if (focusOn) {
			focusState = FormatterRegion::FOCUSED;
/*if (outputDisplay != NULL) {
	outputDisplay->clear();
}*/

/*			if (borderWidth > 0) {
				outputDisplay->setBounds(
						outputDisplay->getX() - borderWidth, 
						outputDisplay->getY() - borderWidth, 
						outputDisplay->getW() + (2 * borderWidth), 
						outputDisplay->getH() + (2 * borderWidth));

				currentComponent.setLocation(
  					currentComponent.getX() + borderWidth, 
  					currentComponent.getY() + borderWidth);

			} else if (borderWidth < 0) {
				currentComponent.setSize(
  					currentComponent.getWidth() + (2 * borderWidth),
    				currentComponent.getHeight() + (2* borderWidth));
  			
	  			currentComponent.setLocation(
	  				currentComponent.getX() - borderWidth, 
	  				currentComponent.getY() - borderWidth);
			}*/

			if (focusComponentSrc != "") {
				Surface* focusSurface = NULL;
				focusSurface = FocusSourceManager::getComponent(
					    focusComponentSrc);

				lock();
				if (focusSurface != NULL && outputDisplay != NULL) {
					outputDisplay->renderFrom(focusSurface);
					delete focusSurface;
					focusSurface = NULL;
				}
				unlock();
			}

			lock();
			if (outputDisplay != NULL) {
				outputDisplay->setBorder(focusBorderColor, focusBorderWidth);
			}
			unlock();

		} else { 
			unselect();
  		}
	}

	void FormatterRegion::unselect() {
		focusState = FormatterRegion::UNSELECTED;

		lock();
		if (outputDisplay != NULL) {
			outputDisplay->validate();
		}
		unlock();

/*if (outputDisplay != NULL) {
	outputDisplay->clear();
}*/

/*		wclog << "FormatterRegion::unselect(" << this << ")" << endl;
		lock();
		if (outputDisplay == NULL) {
			unlock();
			return;
		}

		if (bgColor != NULL) {
			outputDisplay->setBackgroundColor(
				    bgColor->getR(),
				    bgColor->getG(),
				    bgColor->getB());

		} else {
			outputDisplay->setBackgroundColor(0, 0, 0);
		}
*/
		/*currentComponent.setLocation(
					currentComponent.getX() - borderWidth, 
					currentComponent.getY() - borderWidth);
		}
		else if (borderWidth < 0){
			currentComponent.setLocation(
					currentComponent.getX() + borderWidth, 
					currentComponent.getY() + borderWidth);
			
			currentComponent.setSize(
					currentComponent.getWidth() - (2 * borderWidth),
  				currentComponent.getHeight() - (2* borderWidth));
		}*/

/*
		outputDisplay->showContentSurface();
		outputDisplay->validate();
		unlock();
*/
	}

	void FormatterRegion::setFocusInfo(
		    Color* focusBorderColor,
		    int focusBorderWidth,
		    string focusComponentSrc,
		    Color* selBorderColor,
		    int selBorderWidth,
		    string selComponentSrc) {

		if (this->focusBorderColor != NULL) {
			delete this->focusBorderColor;
			this->focusBorderColor = NULL;
		}

		this->focusBorderColor = new Color(
				focusBorderColor->getR(),
				focusBorderColor->getG(),
				focusBorderColor->getB());

		this->focusBorderWidth = focusBorderWidth;
		this->focusComponentSrc = focusComponentSrc;

		if (this->selBorderColor != NULL) {
			delete this->selBorderColor;
			this->selBorderColor = NULL;
		}

		this->selBorderColor = new Color(
				selBorderColor->getR(),
				selBorderColor->getG(),
				selBorderColor->getB());

		this->selBorderWidth = selBorderWidth;
		this->selComponentSrc = selComponentSrc;
	}

	string FormatterRegion::getFocusIndex() {
		return ((CascadingDescriptor*)descriptor)->getFocusIndex();
	}

	Color* FormatterRegion::getBackgroundColor() {
		return bgColor;
	}

	void FormatterRegion::barWipe(Transition* transition, bool isShowEffect) {
		int i, factor, x, y, width, height;
		double time, initTime;
		int transitionSubType, transparencyValue;
		double transitionDur;
		short transitionDir;

		transitionDur = transition->getDur();
		transitionSubType = transition->getSubtype();
		transitionDir = transition->getDirection();

		if (outputDisplay == NULL) {
			cout << "FormatterRegion::barWipe(" << this << ")";
			cout << "Warning! return cause ";
			cout << "abortIn = '" << abortTransitionIn << "' and ";
			cout << "abortOut = '" << abortTransitionOut << "' and ";
			cout << "display = '" << outputDisplay << "' and ";
			cout << "isShow = '" << isShowEffect << "'" << endl;
			return;
		}

		transparencyValue = outputDisplay->getTransparencyValue();

		x = outputDisplay->getX();
		y = outputDisplay->getY();
		width = outputDisplay->getW();
		height = outputDisplay->getH();

		//outputDisplay->setStretch(false);
		initTime = getCurrentTimeMillis();
		if (transitionSubType == Transition::SUBTYPE_BARWIPE_LEFTTORIGHT) {
			if (isShowEffect) {
				i = 1;
				factor = 1;
				lock();
				if (outputDisplay != NULL) {
					outputDisplay->setCurrentTransparency(transparencyValue);
					outputDisplay->resize(1, height);
				}
				unlock();

			} else {
				i = width - 1;
				factor = -1;
			}

			while (i < width && i > 0) {
				time = getCurrentTimeMillis();
				lock();
				if (outputDisplay != NULL) {
					if (transitionDir == Transition::DIRECTION_REVERSE) {
						outputDisplay->setBounds(
								x + (width - i), y, i, height);

					} else {
						outputDisplay->resize(i, height);
					}

				} else {
					unlock();
					return;
				}
				unlock();

				i = getNextStepValue(
						i,
						width,
						factor, time, initTime, transitionDur, 1);

				outputDisplay->validate();
				if ((abortTransitionIn && isShowEffect) ||
						(abortTransitionOut && !isShowEffect)) {

					if (outputDisplay != NULL) {
						outputDisplay->setBounds(x, y, width, height);
						setRegionVisibility(isShowEffect);
						//outputDisplay->setStretch(true);
						if (!isShowEffect) {
							disposeOutputDisplay();
						}
					}
					return;
				}
			}

		} else if (transitionSubType == 
				Transition::SUBTYPE_BARWIPE_TOPTOBOTTOM) {

			if (isShowEffect) {
				i = 1;
				factor = 1;
				lock();
				if (outputDisplay != NULL) {
					outputDisplay->setCurrentTransparency(transparencyValue);
					outputDisplay->resize(width, 1);
				}
				unlock();

			} else {
				i = height - 1;
				factor = -1;
			}

			while (i < height && i > 0) {
				time = getCurrentTimeMillis();
				lock();
				if (outputDisplay != NULL) {
					if (transitionDir == Transition::DIRECTION_REVERSE) {
						outputDisplay->setBounds(
								x, y + (height - i), width, i);

					} else {
						outputDisplay->resize(width, i);
					}

				} else {
					unlock();
					return;
				}
				unlock();

				i = getNextStepValue(
						i,
						height,
						factor, time, initTime, transitionDur, 1);

				outputDisplay->validate();
				if ((abortTransitionIn && isShowEffect) ||
						(abortTransitionOut && !isShowEffect)) {

					if (outputDisplay != NULL) {
						outputDisplay->setBounds(x, y, width, height);
						//outputDisplay->setStretch(true);
						if (!isShowEffect) {
							disposeOutputDisplay();
						}
					}
					setRegionVisibility(isShowEffect);
					return;
				}
			}
		}

		if (!isShowEffect) {
			disposeOutputDisplay();

		} else {
			//outputDisplay->setStretch(true);
			outputDisplay->setBounds(x, y, width, height);
			outputDisplay->validate();
		}
	}

	void* FormatterRegion::barWipeT(void* ptr) {
		struct transT* trans;
		Transition* t;
		bool isShow;
		FormatterRegion* fr;

		trans = (struct transT*)ptr;
		fr = trans->fr;
		t = trans->t;

		delete trans;

		fr->barWipe(t, true);
	}

	void FormatterRegion::fade(Transition* transition, bool isShowEffect) {
		int i, factor;
		double time, initTime;
		int transparencyValue;
		double transitionDur;

		if (outputDisplay == NULL) {
			cout << "FormatterRegion::fade(" << this << ")";
			cout << "Warning! return cause ";
			cout << "abortIn = '" << abortTransitionIn << "' and ";
			cout << "abortOut = '" << abortTransitionOut << "' and ";
			cout << "display = '" << outputDisplay << "' and ";
			cout << "isShow = '" << isShowEffect << "'" << endl;
			return;
		}

		transparencyValue = outputDisplay->getTransparencyValue();
		transitionDur = transition->getDur();

		if (isShowEffect) {
			factor = 1;
			i = 1;

		} else {
			factor = -1;
			i = transparencyValue - 1;
		}

		initTime = getCurrentTimeMillis();
		while (i < transparencyValue && i > 0) {
			time = getCurrentTimeMillis();
			lock();
			if (outputDisplay != NULL) {
				outputDisplay->setCurrentTransparency(i);

			} else {
				unlock();
				return;
			}
			unlock();

			i = getNextStepValue(
					i,
					transparencyValue,
					factor,
					time, initTime, transitionDur, 1);

			if ((abortTransitionIn && isShowEffect) ||
					(abortTransitionOut && !isShowEffect)) {

				break;
			}
		}

		if (!isShowEffect) {
			disposeOutputDisplay();
		} else {
			setRegionVisibility(isShowEffect);			
		}
	}

	void* FormatterRegion::fadeT(void* ptr) {
		struct transT* trans;
		Transition* t;
		bool isShow;
		FormatterRegion* fr;

		trans = (struct transT*)ptr;
		fr = trans->fr;
		t = trans->t;

		delete trans;

		fr->fade(t, true);
	}

	/*void changeCurrentComponent(Component newComponent) {
		if (newComponent != null && currentComponent != null &&
				outputDisplay != null) {
			newComponent.setBounds(currentComponent.getBounds());
			outputDisplay.remove(currentComponent);
			outputDisplay.add(newComponent);
			currentComponent = newComponent;
			outputDisplay.validate();
		}
	}*/

	void FormatterRegion::lock() {
		pthread_mutex_lock(&mutex);
	}

	void FormatterRegion::unlock() {
		pthread_mutex_unlock(&mutex);
	}

	void FormatterRegion::lockTransition() {
		pthread_mutex_lock(&mutexT);
	}

	void FormatterRegion::unlockTransition() {
		pthread_mutex_unlock(&mutexT);
	}
}
}
}
}
}
}
}
