#include "../include/ImageCapture.h"

namespace br {
    namespace ufscar {
        namespace lince {
            namespace util {

                ImageCapture* ImageCapture::instance = NULL;
                GingaFFMpeg ImageCapture::ffmpegCapture;

                /**
                 * Factory
                 * Devolve uma instância de ImageCapture.
                 * @return instância de ImageCapture.
                 */
                ImageCapture* ImageCapture::ImageCaptureFactory() {
                    if (instance == NULL) {
                        instance = new ImageCapture();
                    }
                    return instance;
                }

                /**
                 * Destrutor.
                 */
                ImageCapture::~ImageCapture() {
                    if (instance != NULL) {
                        delete instance;
                    }
                    instance = NULL;
                }

                /**
                 * Faz a captura de imagem do framebuffer conforme os parâmetros préviamente configurados.
                 * Caso não possa escrever no arquivo de destino, lança exception (FileWriteException)
                 * Caso não possa escrever no fluxo de saída (OutputStream) lança exception (StreamWriteException)
                 * Caso não possa escrever no formato informado (jpg, png, etc) lança exception (EncondingException)
                 * @return true, caso a ação ocorra com sucesso, ou false, caso contrário.
                 */
                bool ImageCapture::capture() throw () {
                    int i;
                    vector<char*> arguments;
                    char** argv;
                    stringstream sw, sh;
                    string resolution;

                    sw << instance->width;
                    sh << instance->height;
                    resolution = sw.str() + "x" + sh.str();

                    arguments.push_back("ffmpeg");
                    arguments.push_back("-f");
                    arguments.push_back("fbgrab");
                    arguments.push_back("-r");
                    arguments.push_back("1");
                    arguments.push_back("-i");
                    arguments.push_back(":");
                    arguments.push_back("-f");
                    arguments.push_back("rawvideo");
                    arguments.push_back("-s");
                    arguments.push_back((char*) resolution.c_str());
                    arguments.push_back("-vcodec");
                    arguments.push_back((char*) instance->imageFormat.c_str());
                    arguments.push_back("-vframes");
                    arguments.push_back("1");
                    arguments.push_back("-an");
                    arguments.push_back((char*) instance->destination.c_str());

                    argv = &arguments[0];
                    i = 17;

/*
                    ffmpegCapture.start_recording();
                    try {
                        ffmpegCapture.ffmpeg(i, argv);
                    } catch (...) {
                        throw;
                        return false;
                    }
                    ffmpegCapture.stop_recording();
*/
                    ffmpegCapture.set_recording(i, argv);
                    try {
                        ffmpegCapture.run();
                    } catch (...) {
                        throw;
                        return false;
                    }
                    ffmpegCapture.stop_recording();

                    return true;
                }

                /**
                 * Configura a largura (em pixels) da imagem a ser capturada.
                 * @param width largura da imagem destino.
                 */
                void ImageCapture::setWidth(int Width) {
                    if (Width > 0) {
                        instance->width = Width;
                    }
                }

                /**
                 * Retorna a largura (em pixels) da imagem a ser capturada.
                 * @return largura da imagem destino.
                 */
                int ImageCapture::getWidth() {
                    return instance->width;
                }

                /**
                 * Configura a altura (em pixels) da imagem a ser capturada.
                 * @param height altura da imagem destino.
                 */
                void ImageCapture::setHeight(int Height) {
                    if (Height > 0) {
                        instance->height = Height;
                    }
                }

                /**
                 * Retorna a altura (em pixels) da imagem a ser capturada.
                 * @return altura da imagem destino.
                 */
                int ImageCapture::getHeight() {
                    return instance->height;
                }

                /**
                 * Configura o formato da imagem a ser capturada.
                 * @param imageFormat formato da imagem destino (possíveis valores: png, jpeg).
                 */
                void ImageCapture::setImageFormat(const char* ImageFormat) {
                    if (ImageFormat != NULL) {
                        instance->imageFormat = ImageFormat;
                    }
                }

                /**
                 * Retorna o formato da imagem a ser capturada.
                 * @return formato da imagem destino.
                 */
                const char* ImageCapture::getImageFormat() {
                    return instance->imageFormat.c_str();
                }

                /**
                 * Configura o destino (arquivo local) da imagem a ser capturada.
                 * Caso não seja especificado um destino, o método write() deve ser usado.
                 * Caso não consiga reconhecer a URI lança exception (URIException)
                 * @param destination destino da imagem (possíveis valores: "/home/user/file.jpg", "").
                 */
                void ImageCapture::setDestination(const char* Destination) throw () {
                    if (Destination != NULL) {
                        instance->destination = Destination;
                    }
                }

                /**
                 * Retorna o destino da imagem a ser capturada.
                 * @return destino da imagem.
                 */
                const char* ImageCapture::getDestination() {
                    return instance->destination.c_str();
                }

#if OUTPUT_STREAM
                /**
                 * Adiciona um fluxo de saída (OutputStream) à captura.
                 * Quando adicionado algum fluxo de saída (OutputStream), o método write() deve ser usado.
                 * @param stream fluxo de saída.
                 * @return true, se a ação for executada com sucesso, ou false, caso contrário.
                 */
                bool ImageCapture::addOutputStream(OutputStream* Stream) {
                    if (Stream == NULL) {
                        return false;
                    }
                    //TODO
                    return true;
                }

                /**
                 * Remove um fluxo de saída (OutputStream) da captura.
                 * @param stream fluxo de saída.
                 * @return true, se a ação for executada com sucesso, ou false, caso contrário.
                 */
                bool ImageCapture::removeOutputStream(OutputStream* Stream) {

                }
#endif

                /**
                 * Construtor.
                 */
                ImageCapture::ImageCapture() {
                }

                /**
                 * Percorre o vetor de fluxos de saída e envia os bytes da imagem que está sendo capturada para cada fluxo (OutputStream).
                 * Este método somente é usado caso não seja especificado um destino para a imagem a ser capturada.
                 * @param b um byte da imagem que está sendo capturada.
                 */
                void ImageCapture::write(int b) {

                }

                /**
                 * Percorre o vetor de fluxos de saída e envia os bytes da imagem que está sendo capturada para cada fluxo (OutputStream).
                 * Este método somente é usado caso não seja especificado um destino para a imagem a ser capturada.
                 * @param b origem dos dados.
                 * @param len quantidade de bytes da origem.
                 * @return quantidade de bytes escritos ou -1 caso ocorra erro.
                 */
                int ImageCapture::write(const char* b, int len) {

                }

            }
        }
    }
}


