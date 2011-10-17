#ifndef _AV_CAPTURE_EXCEPTIONS_H_
#define _AV_CAPTURE_EXCEPTIONS_H_

#include <exception>
#include <string>

using namespace std;

namespace br {
    namespace ufscar {
        namespace lince {
            namespace util {

                /**
                 * Classe de exceção, lançada caso não seja possível escrever no arquivo de destino.
                 */
                class
                FileWriteException : public exception {
                private:
                    string message;

                public:
                    FileWriteException();

                    ~FileWriteException() throw ();

                    virtual const char* what() const throw ();
                };

                /**
                 * Classe de exceção, lançada caso não seja possível escrever no fluxo de saída (OutputStream).
                 */
                class
                StreamWriteException : public exception {
                private:
                    string message;

                public:
                    StreamWriteException();

                    ~StreamWriteException() throw ();

                    virtual const char* what() const throw ();
                };

                /**
                 * Classe de exceção, lançada caso o formato de mídia escolhido não possa ser escrito.
                 */
                class
                EncodingException : public exception {
                private:
                    string message;

                public:
                    EncodingException();

                    ~EncodingException() throw ();

                    virtual const char* what() const throw ();
                };

                /**
                 * Classe de exceção, lançada caso não seja possível enviar o conteúdo via streaming pela rede.
                 */
                class
                NetworkingException : public exception {
                private:
                    string message;

                public:
                    NetworkingException();

                    ~NetworkingException() throw ();

                    virtual const char* what() const throw ();
                };

                /**
                 * Classe de exceção, lançada caso não seja possível reconhecer a URI.
                 */
                class
                URIException : public exception {
                private:
                    string message;

                public:
                    URIException();

                    ~URIException() throw ();

                    virtual const char* what() const throw ();
                };

            }
        }
    }
}

#endif //_AV_CAPTURE_EXCEPTIONS_H_

