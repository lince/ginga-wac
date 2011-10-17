#include "BadArgumentException.h"

namespace br {
    namespace ufscar {
        namespace lince {
            namespace util {

                BadArgumentException::BadArgumentException(string mensagem) : exception() {
                    men = mensagem;

                }

                const char* BadArgumentException::what() const throw () {
                    return men.c_str();
                }

                BadArgumentException::~BadArgumentException() throw () {

                }


            }
        }
    }
}
