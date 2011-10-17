/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Copyright (C) 2009 UFSCar/Lince, Todos os Direitos Reservados.

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
lince@dc.ufscar.br
http://www.ncl.org.br
http://www.ginga.org.br
http://lince.dc.ufscar.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright (C) 2009 UFSCar/Lince, Todos os Direitos Reservados.

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
lince@dc.ufscar.br
http://www.ncl.org.br
http://www.ginga.org.br
http://lince.dc.ufscar.br
*******************************************************************************/

#ifndef _LOGGERUTIL_H
#define	_LOGGERUTIL_H

typedef void* HLoggerPtr;

#include <sstream>
using namespace std;

namespace br {
    namespace ufscar {
        namespace lince {
                namespace util {

                    class LoggerUtil {
                    public:

                        /**
                         * Inicializa a classe LoggerUtil e configura a saída de informações para o console.
                         */
                        static void configure();

                        /**
                         * Inicializa a classe LoggerUtil e configura a saída de informações de acordo com um arquivo de configurações.
                         * @param xmlFilePath caminho completo do arquivo de configurações XML.
                         */
                        static void configure(const char* xmlFilePath);

                        /**
                         * Retorna uma instância única para saída de informações para cada identificador.
                         * @param id identificador.
                         */
                        static HLoggerPtr getLogger(const char* id);

                        /**
                         * Direciona as informações simples para uma saída de <em>log</em>, seja arquivo ou console.
                         * @param logger instância do direcionador de saída de informações.
                         * @param msg mensagem.
                         */
                        static void info(HLoggerPtr logger, const char* msg);

                        /**
                         * Direciona as informações de <em>debug</em> para uma saída de <em>log</em>, seja arquivo ou console.
                         * @param logger instância do direcionador de saída de informações.
                         * @param msg mensagem.
                         */
                        static void debug(HLoggerPtr logger, const char* msg);

                        /**
                         * Direciona as informações de <em>warning</em> para uma saída de <em>log</em>, seja arquivo ou console.
                         * @param logger instância do direcionador de saída de informações.
                         * @param msg mensagem.
                         */
                        static void warning(HLoggerPtr logger, const char* msg);

                        /**
                         * Direciona as informações de <em>error</em> para uma saída de <em>log</em>, seja arquivo ou console.
                         * @param logger instância do direcionador de saída de informações.
                         * @param msg mensagem.
                         */
                        static void error(HLoggerPtr logger, const char* msg);

                    private:

                        /**
                         * Direciona as informações de <em>log</em> para uma saída padrão (console) caso a biblioteca Log4C++ não esteja presente.
                         * @param logger identificador.
                         * @param msg mensagem.
                         */
                        static void stdLogOut(HLoggerPtr logger, const char* msg);
                    };

                        #define LoggerUtil_info(logger, msg) { \
                            ostringstream out; \
                            out << msg; \
                            LoggerUtil::info(logger, out.str().c_str()); \
                        }

                        #define LoggerUtil_debug(logger, msg) { \
                            ostringstream out; \
                            out << msg; \
                            LoggerUtil::debug(logger, out.str().c_str()); \
                        }

                        #define LoggerUtil_warning(logger, msg) { \
                            ostringstream out; \
                            out << msg; \
                            LoggerUtil::warning(logger, out.str().c_str()); \
                        }

                        #define LoggerUtil_error(logger, msg) { \
                            ostringstream out; \
                            out << msg; \
                            LoggerUtil::error(logger, out.str().c_str()); \
                        }


            }
        }
    }
}

#endif	/* _LOGGERUTIL_H */

