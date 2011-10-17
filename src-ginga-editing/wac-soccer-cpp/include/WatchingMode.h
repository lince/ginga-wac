#ifndef WATCHINGMODE_H
#define WATCHINGMODE_H

#include "system/io/interface/input/CodeMap.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#include "inputmode/InputModeModifier.h"
#include "inputmode/EnhancedInputManager.h"
using namespace ::br::ufscar::lince::ginga::core::inputmode;

#include <ginga/linceutil/LoggerUtil.h>
using namespace br::ufscar::lince::util;

#include "player/ImagemDisplay.h"
using namespace ::br::ufscar::lince::ginga::core::player;

#include <iostream>
#include <cstdlib>
using namespace std;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace soccer {

class WatchingMode : public InputModeModifier {

private:
	HLoggerPtr logger;
	int green;
	int red;
	int yellow;
	int blue;

	ImagemDisplay* imageStarting;
	ImagemDisplay* imageExiting;

public:
	WatchingMode();
	~WatchingMode();

	virtual void startingInputMode();
	virtual void exitingInputMode();
	virtual void initializeInputMode();
	virtual IInputEvent* modifierEvent(IInputEvent* event);

};

}
}
}
}
}
}


#endif //WATCHINGMODE_H

