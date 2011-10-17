#ifndef PLAYERSTATE_H_
#define PLAYERSTATE_H_

#include <vector>
#include <map>
#include <string>

using namespace std;

namespace br {
namespace ufscar {
namespace lince {
namespace ginga {
namespace core {
namespace player {
	
//! Representa o estado de um player.
/*!
 Um objeto da classe PlayerState representa o estado interno de um dado player em um determinado
 instante de tempo. 
 */

class PlayerState
{
protected:
	//! Armazena o estado do player em que o player se encontra.
	/*!
	  O estado de um player pode ser:
	 	0 - NONE: Estado inicial de um player;
	  	1 - PLAY: Estado em que um player se encontra quando esta em execução;
	    2 - PAUSE: Estado em que um player se encontra quanto esta pausado; e
	    3 - STOP: Estado em que um player se encontra após ter sido parado.
	 */
	short status;
	
	//! Denota se o player terminou sua execução.
	/*!
	  Se presented for true, o player já terminou sua execução. Caso seja false, o player não terminou sua execução.
	 */
	bool presented;
	
	//! Denota se o player está visivel.
	/*!
	  Caso seja true, o player está visivel. Caso seja false, o player não esta visivel.
	 */
	bool visible;
	
	//! A altura do player.
	/*!
	  Representa a altura do player em pixels.
	 */
	int hight;
	
	 //! A largura do player.
	 /*!
	   Representa a largura do player em pixels.
	  */
	int width;
	
	//! A posição horizontal do player.
	/*!
	  Denota a distância entre o ponto que representa o canto superior esquerdo do player
	  e o contorno superior da superfice em pixels.
	 */
	int x;
	
	//! A posição vertical do player.
	/*!
	  Denota a distância entre o ponto que representa o canto superior esquerdo do player
	  e o contorno esquerdo da superfice em pixels.
	 */
	int y;
	
	//! Tempo de execução da media.
	/*!
	  * Denota o tempo da mídia em execução no player segundos,
	  */
	double mediaTime;

	string mrl;


	map<string, string>* properties;
	
public:
	static const short NONE;
	static const short PLAY;
	static const short PAUSE;
	static const short STOP;

	//! Construtor padrão sem parametros.
	/*!
	  Inicializa a instancia de PlayerState com os valores padrão.
	 */
	PlayerState();

	~PlayerState();
	
	//! Seta a variavel presented da instancia.
	/*!
	 \sa presented
	 \param nPresented novo valor a ser atribuido a presented.
	 */
	void setPresented(bool nPresented);
    
    //! Seta a variavel visible da instancia.
    /*!
     *\sa visible
     * param nVisible novo valor a ser atribuido a visible.
     */
    void setVisible(bool nVisible);
    
    //! Seta o estado do player.
    /*!
     * \sa status
     * \param nStatus novo estado.
     */
    void setStatus(short nStatus);
    
    
    //! Seta a altura do player.
    /*!
     * \sa hight
     * \param nHight nova altura.
     */
    void setHight(int nHight);
   
    //! Seta o largura do player.
    /*!
     * \sa width
     * \param nWitdh novo largura.
     */
    void setWidth(int nWidth);
    
    //! Seta o tempo de execução da mídia do player.
    /*!
     * \sa mediaTime
     * \param time novo tempo de execução.
     */
    void setMediaTime(double time);
    
    //! Obtêm o estado.
    /*!
     * \sa status
     * \return o estado do player.
     */
    short getStatus();
    
    //! Obtêm o estado.
    /*!
     * \sa status
     * \return o estado do player.
     */
    double getMediaTime();
    
    //! Obtêm a altura.
    /*!
     * \sa hight
     * \return a altura do player.
     */
    int getHight();
    
    //! Obtêm a largura.
    /*!
     * \sa width
     * \return a lagura do player.
     */
    int getWidh();

    //! Verifica se o player terminou sua execução.
    /*!
     * \sa presented
     * \return true se já terminou a execução e false se não terminou a execução.
     */
    bool isPresented();

    //! Veritica se o player está visivel.
    /*!
     * \sa visible
     * \return true se está visibel, e false se não está visivel.
     */
    bool isVisible();

	void setProperties(map<string, string>* prop);

	string getPropertyValue(string name);

	vector<string>* getPropertiesNames();

   void setMrl(string mrl);

   string getMrl();
};


}
}
}
}
}
}

#endif /*PLAYERSTATE_H_*/
