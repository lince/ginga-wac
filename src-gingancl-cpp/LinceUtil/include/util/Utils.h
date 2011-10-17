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

#ifndef _UTILS_H
#define	_UTILS_H

#include <fcntl.h>
#include <string>
using namespace std;

namespace br {
    namespace ufscar {
        namespace lince {
                namespace util {

                        class Utils {

                        public:

                            /**
                             * Converte um valor em segundos para o formato hh:mm:ss.
                             * @param time valor em segundos
                             * @return hh:mm:ss
                             */
                            static string timeToString(double time);

                            /**
                             * Converte um valor no formato hh:mm:ss.SSS para segundos.
                             * @param str hh:mm:ss.SSS
                             * @return valor convertido em segundos.
                             */
                            static double stringToTime(string str);

                            /**
                             * Substitui todas as ocorrências de uma substring por outra.
                             * @param str string a ser percorrida.
                             * @param from substring a ser substituida.
                             * @param to string substituta.
                             * @return string com alterações.
                             */
                            static const char* stringReplaceChars(string &str, const char *from, const char *to);

                            /**
                             * Substitui caracteres literais por delimitadores XML.
                             * @param str string a ser percorrida.
                             * @return string com alterações.
                             */
                            static const char* parseXMLChars(string &str);

                            /**
                             * Substitui delimitadores XML por caracteres literais.
                             * @param str string a ser percorrida.
                             * @return string com alterações.
                             */
                            static const char* encodeXMLChars(string &str);

                            /**
                             * Substitui todas as ocorrências de letras maiúsculas por minúsculas.
                             * @param str string a ser percorrida.
                             * @return string com alterações.
                             */
                            static const char* stringToLowerCase(string &str);

                            /**
                             * Verifica se duas strings são iguais, ignorando a diferença entre letras maiúsculas e minúsculas.
                             * @param str1 primeira string a ser comparada.
                             * @param str2 segunda string a ser comparada.
                             * @return true, caso sejam iguais, ou false, caso contrário.
                             */
                            static bool equalsIgnoreCase(const char* str1, const char* str2);

                            /**
                             * Verifica se uma string termina com uma determinada sequência de caracteres.
                             * @param str string a ser verificada.
                             * @param end sequência de caracteres final.
                             * @return true, caso termine com a sequência de caracteres, ou false, caso contrário.
                             */
                            static bool stringEndsWith(const char* str, const char* end);

                            /**
                             * Verifica se uma URL está completa ou possui apenas o caminho relativo.
                             * @param url URL a ser verificada.
                             * @return true, caso a URL esteja completa, ou false, caso contenha apenas o caminho relativo.
                             */
                            static bool isAbsoluteURL(string url);

                            /**
                             * Extrai o endereço do host (ip ou domínio) de uma URL.
                             * @param url URL do host.
                             * @return endereço do host.
                             */
                            static char* getHostURL(const char* url);

                            /**
                             * Extrai a porta do host de uma URL.
                             * @param url URL do host.
                             * @return porta do host.
                             */
                            static int getPortURL(const char* url);

                            /**
                             * Extrai o endereço relativo (sem o ip/domínio e porta) de uma URL.
                             * @param url URL do host.
                             * @return endereço relativo.
                             */
                            static char* getRelativeURL(const char *url);

                            /**
                             * Verifica se duas URLs são iguais.
                             * @param absoluteUrl URL completa.
                             * @param url URL relativa ou completa.
                             * @return true, caso a primeira URL, em modo completo ou relativo, seja igual a segunda, ou false, caso contrário.
                             */
                            static bool equalsURL(const char* absoluteUrl, const char* url);

                            /**
                             * Interrompe a execução de uma thread.
                             * @param mtime tempo de interrupção em milisegundos.
                             */
                            static void wait(long mtime);

                            /**
                             * Converte um valor decimal em string.
                             * @param value valor a ser convertido.
                             * @return string do valor.
                             */
                            static char* integerToString(int value);

                            /**
                             * Converte um valor <em>long</em> em string.
                             * @param value valor a ser convertido.
                             * @return string do valor.
                             */
                            static char* longToString(long value);

                            /**
                             * Converte um valor <em>off_t</em> em string.
                             * @param value valor a ser convertido.
                             * @return string do valor.
                             */
                            static char* offtToString(off_t value);

                            /**
                             * Converte uma string em um valor decimal.
                             * @param value string a ser convertida.
                             * @return valor.
                             */
                            static int stringToInteger(const char* value);

                            /**
                             * Cria um valor UUID baseado no tempo corrente.
                             * Formato da UUID: HHHH-HHHH-HHHH-HHHH
                             * Onde H é um valor hexadecimal.
                             * @return string com a UUID criada.
                             */
                            static char* createRandomUUID();

                            /**
                             * Retorna um endereço IP associado a máquina de acordo com um índice.
                             * O índice é necessário pois mais de um IP podem estar associados a máquina.
                             * @param n índice do endereço desejado.
                             * @return string com o endereço IP.
                             */
                            static char* getHostAddress(int n);

                            /**
                             * Retorna a quantidade de interfaces de rede ativa na máquina.
                             * @return quantidade de interfaces de rede.
                             */
                            static int getHostInterfaces();

                            /**
                             * Verifica se um endereço IP é da versão 4.
                             * @param ip endereço IP.
                             * @return true, se o endereço passado for da versão 4, ou false, caso contrário.
                             */
                            static bool isIPv4(const char* ip);

                            /**
                             * Verifica se um endereço IP é da versão 6.
                             * @param ip endereço IP.
                             * @return true, se o endereço passado for da versão 6, ou false, caso contrário.
                             */
                            static bool isIPv6(const char* ip);

                            /**
                             * Cria um servidor de socket.
                             * @param ip endereço IP do servidor, para verificação de versão IPv4 ou IPv6.
                             * @param port porta para atender os sockets clientes.
                             * @return descritor do servidor de socket.
                             */
                            static int socketServer(const char* ip, int* port);

                            /**
                             * Cria um socket cliente.
                             * @param ip endereço IP do servidor.
                             * @param port porta do servidor.
                             * @return descritor do cliente de socket.
                             */
                            static int socketConnect(const char* ip, int port);

                            /**
                             * Envia dados para um servidor de socket.
                             * @param sock descritor do cliente de socket.
                             * @param data dados a serem enviados.
                             * @param length tamanho, em bytes, dos dados.
                             * @return quantidade, em bytes, de dados enviados.
                             */
                            static off_t socketSend(int sock, const char* data, off_t length);

                            /**
                             * Retorna o caminho completo do diretório do usuário.
                             * @return caminho do diretório do usuário.
                             */
                            static const char* getHomeDir();

                        };

            }
        }
    }
}

#endif	/* _UTILS_H */

