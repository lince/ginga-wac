#include "InitializationException.h"

namespace br {
    namespace ufscar {
        namespace lince {
            namespace util {

                InitializationException::InitializationException(string men) : exception() {
                    this->message = men;
                }

                const char* InitializationException::what() const throw () {
                    string ret = "Lançada InitializationException()\n";
                    ret += message;
                    return ret.c_str();
                }

                InitializationException::~InitializationException() throw () {

                }

            }
        }
    }
}
