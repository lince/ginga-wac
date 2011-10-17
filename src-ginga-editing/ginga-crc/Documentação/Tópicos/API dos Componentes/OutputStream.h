namespace br {
    namespace ufscar {
        namespace lince {
                namespace util {

                        class OutputStream {
                        public:
                            /**
                             * Escreve bytes no fluxo destino.
                             * @param b um byte da origem (arquivo, vídeo, áudio, imagem, rede, etc).
                             */
                            virtual void write(int b) = 0;

                            /**
                             * Escreve bytes no fluxo destino.
                             * @param b origem dos dados.
                             * @param len quantidade de bytes da origem.
                             * @return quantidade de bytes escritos ou -1 caso ocorra erro.
                             */
                            virtual int write(const char* b, int len) = 0;
                            
                        };

            }
        }
    }
}
