#include "../../include/ImagemDisplay.h"

#include <sstream>
#include <fstream>

#define TIME_CONSTANT 200000

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace core {
namespace player {
	ImagemDisplay::ImagemDisplay(string url) : ShowButton() {
		image = NULL;
		win = NULL;
		imageURL = url;
		
	}

	void ImagemDisplay::initializeWindow(int x, int y, int tamX, int tamY) {
/*		win = new Window(
			    (int)(x),
			    (int)(y),
			    (int)(tamX),
			    (int)(tamY));*/

		IComponentManager* cm = IComponentManager::getCMInstance();
		IDeviceManager* dm = NULL;

		if (cm == NULL) {
			return;
		}

		dm = ((DeviceManagerCreator*)(cm->getObject("DeviceManager")))();

		win = ((WindowCreator*)(cm->getObject("Window")))(x, y, tamX, tamY);
		if (win != NULL) {
			win->setCaps(win->getCap("ALPHACHANNEL"));
			win->draw();
		}
		/*win->setBackgroundColor(0, 0, 0);
		win->setColorKey(0, 0, 0);
		win->setTransparencyValue((int)(0.5*0xFF));*/

	}

	void ImagemDisplay::display(int time) {
		this->time = time;
		Thread::start();
	}

	void ImagemDisplay::run() {
		lock();
		ShowButton::render(imageURL);
		if (time != 0) {
			int sleeptTime = 0;
			showing = true;
			while(showing && time >= sleeptTime) {
			   ::usleep(TIME_CONSTANT);
			   sleeptTime += TIME_CONSTANT;
			}
		}
		else {
			showing = true;
			while(showing) {
			   ::usleep(TIME_CONSTANT);
			}
		}
		win->hide();
		unlock();
		
	}

	void ImagemDisplay::hide() {
		showing = false;
	}

}
}
}
}
}
}
