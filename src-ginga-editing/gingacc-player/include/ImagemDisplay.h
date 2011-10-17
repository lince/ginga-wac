#ifndef IMAGEMDISPLAY_H_
#define IMAGEMDISPLAY_H_

#include "ShowButton.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include <iostream>
#include <string>
using namespace std;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace core {
namespace player {
	class ImagemDisplay: public ShowButton {
		protected:
			string imageURL;
			int time;
			bool showing;

		public:
			ImagemDisplay(string url);
			void initializeWindow(int x, int y, int tamX, int tamY);
			void display(int time=0);
			void hide();
		private:
			void run();
	};

}
}
}
}
}
}

#endif //IMAGEMDISPLAY_H
