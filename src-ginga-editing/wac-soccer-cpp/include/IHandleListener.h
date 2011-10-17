#ifndef IHANDLELISTENER_H_
#define IHANDLELISTENER_H_

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace wac {
namespace soccer {

class IHandleListener {
	
public:
	virtual ~IHandleListener() { };

	virtual void handleEvent(int eventId)=0;
	
};

}
}
}
}
}
}


#endif //IHANDLELISTENER_H_

