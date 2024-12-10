#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Layer
{
public:
    HelloWorld();
    ~HelloWorld();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
    // Referencia al humano controlado por el jugador
    cocos2d::Sprite* _humano;
    void inicializarJugador();
    float tiempoSobreviviendo; // Para llevar el tiempo transcurrido
    cocos2d::Label* labelTiempo; // Para mostrar el tiempo
    cocos2d::Label* labelTeclas; // Para mostrar las teclas del tutorial

    //Manejar el Tiempo
    void actualizarTiempo(float dt);

    // Métodos para manejar el sonido de las vacas
    void reproducirSonidoVaca(float dt);
    void iniciarSonidoVacas();
    void detenerSonidoVacas();
    bool sonidoVacasActivo; // Variable para saber si el sonido está activo
    // Vector para las bombas
    cocos2d::Vector<cocos2d::Sprite*> _bombas;

    // Funciones para el movimiento del jugador
    void inicializarTeclado();
    void moverJugadorIzquierda();
    void moverJugadorDerecha();
    void inicializarFisica(cocos2d::Sprite* sprite);
    void moverJugador(cocos2d::Touch* toque, cocos2d::Event* event);

    

    // Funciones relacionadas con la bomba
    void agregarBomba();
    void agregarBombar(float dt);

    // Funciones para manejar la colisión y Game Over
    void inicializarColisiones();
    void mostrarGameOver();
};

#endif // __HELLOWORLD_SCENE_H__
