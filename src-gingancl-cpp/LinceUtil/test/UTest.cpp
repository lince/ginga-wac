#include <stdio.h>
#include "util/LoggerUtil.h"
#include "util/Utils.h"
using namespace std;
using namespace br::ufscar::lince::util;

HLoggerPtr logger = LoggerUtil::getLogger("br.ufscar.lince.ginga.util");

int main(int argc, char* argv[]){
    int result = 0;

    try{

        LoggerUtil::configure();

        LoggerUtil_info(logger, "Entrando na aplicacao.");

        int ch;

    	do
	{
		ch = getchar();
		ch = toupper( ch );
		switch (ch) {
                case 'V':
                {
                    int count = Utils::getHostInterfaces();
                    for(int i=0; i < count; i++){
                        LoggerUtil_debug(logger, "Interface " << i << ": " << Utils::getHostAddress(i));
                    }
                    break;
                }
		case 'Q':
                    break;
                default:
                    LoggerUtil_info(logger, "'q' : Quit\n");
		}
	} while( ch != 'Q');

	LoggerUtil_info(logger, "Saindo da aplicacao.");

    } catch(...) {
        result = -1;
    }

    return result;
}

