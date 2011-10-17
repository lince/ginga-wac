#ifndef __IMAGE_CAPTURE_H__
#define __IMAGE_CAPTURE_H__

#include <string>
#include <vector>
#include <sstream>

#include <ffmpeg_ginga.h>

#define OUTPUT_STREAM 0
#if OUTPUT_STREAM
  #include <ginga/capture/OutputStream.h>
#endif

using namespace std;

namespace br {
    namespace ufscar {
        namespace lince {
                namespace util {

                        class ImageCapture {
                        private:
                            int width;
                            int height;
                            string imageFormat;
                            string destination;
#if OUTPUT_STREAM
                            vector<OutputStream *> outputStreams;
#endif
                            static ImageCapture* instance;
                            static GingaFFMpeg ffmpegCapture;

                        public:

                            /**
                             * Factory
                             * Devolve uma instância de ImageCapture.
                             * @return instância de ImageCapture.
                             */
                            static ImageCapture* ImageCaptureFactory();

                            /**
                             * Destrutor.
                             */
                            ~ImageCapture();

                            /**
                             * Faz a captura de imagem do framebuffer conforme os parâmetros préviamente configurados.
                             * Caso não possa escrever no arquivo de destino, lança exception (FileWriteException)
                             * Caso não possa escrever no fluxo de saída (OutputStream) lança exception (StreamWriteException)
                             * Caso não possa escrever no formato informado (jpg, png, etc) lança exception (EncondingException)
                             * @return true, caso a ação ocorra com sucesso, ou false, caso contrário.
                             */
                            bool capture() throw();

                            /**
                             * Configura a largura (em pixels) da imagem a ser capturada.
                             * @param width largura da imagem destino.
                             */
                            void setWidth(int Width);

                            /**
                             * Retorna a largura (em pixels) da imagem a ser capturada.
                             * @return largura da imagem destino.
                             */
                            int getWidth();

                            /**
                             * Configura a altura (em pixels) da imagem a ser capturada.
                             * @param height altura da imagem destino.
                             */
                            void setHeight(int Height);

                            /**
                             * Retorna a altura (em pixels) da imagem a ser capturada.
                             * @return altura da imagem destino.
                             */
                            int getHeight();

                            /**
                             * Configura o formato da imagem a ser capturada.
                             * @param imageFormat formato da imagem destino (possíveis valores: png, jpeg).
                             */
                            void setImageFormat(const char* ImageFormat);

                            /**
                             * Retorna o formato da imagem a ser capturada.
                             * @return formato da imagem destino.
                             */
                            const char* getImageFormat();

                            /**
                             * Configura o destino (arquivo local) da imagem a ser capturada.
                             * Caso não seja especificado um destino, o método write() deve ser usado.
                             * Caso não consiga reconhecer a URI lança exception (URIException)
                             * @param destination destino da imagem (possíveis valores: "/home/user/file.jpg", "").
                             */
                            void setDestination(const char* Destination) throw();

                            /**
                             * Retorna o destino da imagem a ser capturada.
                             * @return destino da imagem.
                             */
                            const char* getDestination();

#if OUTPUT_STREAM
                            /**
                             * Adiciona um fluxo de saída (OutputStream) à captura.
                             * Quando adicionado algum fluxo de saída (OutputStream), o método write() deve ser usado.
                             * @param stream fluxo de saída.
                             * @return true, se a ação for executada com sucesso, ou false, caso contrário.
                             */
                            bool addOutputStream(OutputStream* Stream);

                            /**
                             * Remove um fluxo de saída (OutputStream) da captura.
                             * @param stream fluxo de saída.
                             * @return true, se a ação for executada com sucesso, ou false, caso contrário.
                             */
                            bool removeOutputStream(OutputStream* Stream);
#endif

                        private:

                            /**
                             * Construtor.
                             */
                            ImageCapture();

                            /**
                             * Percorre o vetor de fluxos de saída e envia os bytes da imagem que está sendo capturada para cada fluxo (OutputStream).
                             * Este método somente é usado caso não seja especificado um destino para a imagem a ser capturada.
                             * @param b um byte da imagem que está sendo capturada.
                             */
                            void write(int b);

                            /**
                             * Percorre o vetor de fluxos de saída e envia os bytes da imagem que está sendo capturada para cada fluxo (OutputStream).
                             * Este método somente é usado caso não seja especificado um destino para a imagem a ser capturada.
                             * @param b origem dos dados.
                             * @param len quantidade de bytes da origem.
                             * @return quantidade de bytes escritos ou -1 caso ocorra erro.
                             */
                            int write(const char* b, int len);
                            
                        };

            }
        }
    }
}

#endif //__IMAGE_CAPTURE_H__

