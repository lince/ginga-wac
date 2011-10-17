#ifndef EVENTHANDLE_H_
#define EVENTHANDLE_H_

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;


#include "IHandleListener.h"

#include <iostream>
#include <cstdlib>
using namespace std;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace soccer {

class EventHandle : public Thread {
	
public:
	EventHandle();
	~EventHandle();

	void setHandleListener(IHandleListener* listener);
	void setEvent(int eventId);

private:
	IHandleListener* listener;
	int eventId;

protected:
	virtual void run();
};

}
}
}
}
}
}


#endif //EVENTHANDLE_H_

