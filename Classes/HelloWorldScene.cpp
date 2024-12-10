#include "HelloWorldScene.h"
#include "../proj.win32/MenuPrincipal.h"
#include <cocostudio/SimpleAudioEngine.h>
#include <ui/UISlider.h>

USING_NS_CC;


HelloWorld::HelloWorld() : sonidoVacasActivo(false) {}

HelloWorld::~HelloWorld() {}

Scene* HelloWorld::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vect(0, 0));

	// 'layer' is an autorelease object
	auto layer = HelloWorld::create();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

void HelloWorld::inicializarFisica(cocos2d::Sprite* sprite)
{
	auto cuerpo = PhysicsBody::createCircle(sprite->getContentSize().width / 2);
	cuerpo->setContactTestBitmask(true);
	cuerpo->setDynamic(true);
	sprite->setPhysicsBody(cuerpo);
}

void HelloWorld::moverJugador(Touch* toque, Event* evento)
{
	auto posicionToque = toque->getLocation();
	if (_humano->getBoundingBox().containsPoint(posicionToque)) {
		_humano->setPositionX(posicionToque.x);
	}

}

void HelloWorld::inicializarJugador()
{
	_humano = cocos2d::Sprite::create("ruta_a_humano.png");
	_humano->setPosition(cocos2d::Vec2(100, 100));

	// Crear un cuerpo físico para el humano
	auto cuerpoHumano = cocos2d::PhysicsBody::createBox(_humano->getContentSize());
	cuerpoHumano->setContactTestBitmask(true); // Permite detectar colisiones
	cuerpoHumano->setDynamic(true);           // El cuerpo puede moverse
	_humano->setPhysicsBody(cuerpoHumano);

	this->addChild(_humano);
}

void HelloWorld::inicializarColisiones()
{
	auto escuchadorColision = EventListenerPhysicsContact::create();

	escuchadorColision->onContactBegin = [=](cocos2d::PhysicsContact& contacto) {
		auto nodoA = contacto.getShapeA()->getBody()->getNode();
		auto nodoB = contacto.getShapeB()->getBody()->getNode();

		// Verificar si uno de los nodos es el humano y el otro una bomba
		if ((nodoA == _humano && nodoB->getName() == "bomba") ||
			(nodoB == _humano && nodoA->getName() == "bomba")) {
			mostrarGameOver();
			return true; // Finaliza la colisión
		}
		return false;
		};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(escuchadorColision, this);
}

void HelloWorld::mostrarGameOver()
{
	// Mensaje de Game Over
	auto gameOverLabel = Label::createWithTTF("Game Over", "fonts/Pixel.ttf", 48);
	gameOverLabel->setPosition(Director::getInstance()->getVisibleSize() / 2);
	this->addChild(gameOverLabel);

	// Mensaje de Felicidades
	auto felicidadesLabel = Label::createWithTTF("Felicidades por pasar el tutorial", "fonts/Pixel.ttf", 36);
	felicidadesLabel->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2,
		Director::getInstance()->getVisibleSize().height / 2 - 40));
	this->addChild(felicidadesLabel);

	// Mensaje de Historia
	auto historiaLabel = Label::createWithTTF("Ahora veras la historia de este mundo...", "fonts/Pixel.ttf", 36);
	historiaLabel->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2,
		Director::getInstance()->getVisibleSize().height / 2 - 80));
	this->addChild(historiaLabel);

	// Detener el juego
	this->unscheduleUpdate(); // Si usas update() en tu juego
	this->_eventDispatcher->removeAllEventListeners();

	// Crear un Label para el botón "Ir al Menu Principal" con fuente Pixel
	auto menuPrincipalLabel = Label::createWithTTF("Ir al Menu Principal", "fonts/Pixel.ttf", 36);
	auto menuPrincipalItem = MenuItemLabel::create(menuPrincipalLabel, [&](Ref* sender) {
		auto menuEscena = MenuPrincipal::createScene(); // Asegúrate de que MenuPrincipal esté correctamente definida
		Director::getInstance()->replaceScene(menuEscena);
		});

	// Posicionar el botón
	auto menu = Menu::create(menuPrincipalItem, nullptr);
	menu->setPosition(gameOverLabel->getPosition() - Vec2(0, 150)); // Asegúrate de que no se solapen
	this->addChild(menu);
}

void HelloWorld::agregarBomba()
{
	auto bomba = cocos2d::Sprite::create("ruta_a_bomba.png");
	bomba->setPosition(cocos2d::Vec2(300, 100));
	bomba->setName("bomba");  // Asigna un nombre a la bomba

	// Crear un cuerpo físico para la bomba
	auto cuerpoBomba = cocos2d::PhysicsBody::createCircle(bomba->getContentSize().width / 2);
	cuerpoBomba->setContactTestBitmask(true); // Permite detectar colisiones
	cuerpoBomba->setDynamic(false);          // La bomba no se mueve
	bomba->setPhysicsBody(cuerpoBomba);

	this->addChild(bomba);
}

void HelloWorld::agregarBombar(float dt)
{
	auto director = Director::getInstance();
	auto tamano = director->getWinSize();
	Sprite* bomba = nullptr;
	for (int i = 0; i < 5; i++)
	{
		bomba = Sprite::create("imagenes/bomba.png");
		bomba->setAnchorPoint(Vec2::ZERO);
		bomba->setPosition(CCRANDOM_0_1() * tamano.width, tamano.height);
		bomba->setName("bomba");  // Asigna un nombre a la bomba
		inicializarFisica(bomba);
		bomba->getPhysicsBody()->setVelocity(Vect(0, ((CCRANDOM_0_1() + 0.2f) * -250)));
		iniciarSonidoVacas();

		_bombas.pushBack(bomba);
		this->addChild(bomba, 1);
	}
}

void HelloWorld::inicializarTeclado()
{
	// Crear el escuchador para el teclado
	auto escuchadorTeclado = EventListenerKeyboard::create();

	// Definir qué ocurre al presionar una tecla
	escuchadorTeclado->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW: // Flecha izquierda
		case EventKeyboard::KeyCode::KEY_A:         // Tecla 'A'
			moverJugadorIzquierda();
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW: // Flecha derecha
		case EventKeyboard::KeyCode::KEY_D:           // Tecla 'D'
			moverJugadorDerecha();
			break;
		default:
			break;
		}
		};

	// Agregar el escuchador al despachador de eventos
	_eventDispatcher->addEventListenerWithSceneGraphPriority(escuchadorTeclado, this);
}

void HelloWorld::moverJugadorIzquierda()
{
	auto posicionActual = _humano->getPosition();
	float nuevoX = posicionActual.x - 10; // Ajusta el valor del desplazamiento

	// Verifica que no se salga del límite izquierdo de la pantalla
	if (nuevoX < _humano->getContentSize().width / 2)
		nuevoX = _humano->getContentSize().width / 2;

	_humano->setPosition(nuevoX, posicionActual.y);
}

void HelloWorld::moverJugadorDerecha()
{
	auto posicionActual = _humano->getPosition();
	float nuevoX = posicionActual.x + 10; // Ajusta el valor del desplazamiento

	// Verifica que no se salga del límite derecho de la pantalla
	auto pantallaAncho = cocos2d::Director::getInstance()->getVisibleSize().width;
	auto limiteDerecho = pantallaAncho - _humano->getContentSize().width / 2;

	if (nuevoX > limiteDerecho)
		nuevoX = limiteDerecho;

	_humano->setPosition(nuevoX, posicionActual.y);
}

void HelloWorld::actualizarTiempo(float dt)
{
	tiempoSobreviviendo += dt;

	// Mostrar el tiempo en el label
	char tiempoTexto[50];
	sprintf(tiempoTexto, "Tiempo: %.2f", tiempoSobreviviendo); // Muestra el tiempo con 2 decimales
	labelTiempo->setString(tiempoTexto);
}

void HelloWorld::reproducirSonidoVaca(float dt)
{
	// Reproducir el sonido de la vaca cada vez que se llame
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sonidos/vaca_muu.wav", false);
}

void HelloWorld::iniciarSonidoVacas()
{
	// Solo empezar a reproducir el sonido en escenas donde caen bombas
	sonidoVacasActivo = true;
	this->schedule([=](float dt) { reproducirSonidoVaca(dt); }, 10.0f, "reproducir_vacas_key");
}

void HelloWorld::detenerSonidoVacas()
{
	// Detener el sonido de las vacas cuando sea necesario
	sonidoVacasActivo = false;
	this->unschedule("reproducir_vacas_key");
}

bool HelloWorld::init()
{
	if (!Layer::init())
	{
		return false;
	}
	auto director = Director::getInstance();
	auto tamano = director->getWinSize();
	auto spriteFondo = Sprite::create("imagenes/fondo.png");
	spriteFondo->setAnchorPoint(Vec2::ZERO);
	spriteFondo->setPosition(0, 0);
	addChild(spriteFondo, 0);

	// Crear y mostrar el texto de tiempo
	labelTiempo = Label::createWithTTF("Tiempo: 0", "fonts/Pixel.ttf", 24);
	labelTiempo->setPosition(tamano.width - 100, tamano.height - 30); // Esquina superior derecha
	this->addChild(labelTiempo, 1);

	// Crear y mostrar las teclas de movimiento
	labelTeclas = Label::createWithTTF("Usa las teclas A/D o las flechas\npara moverte.", "fonts/Pixel.ttf", 24);
	labelTeclas->setPosition(tamano.width / 2, tamano.height - 50); // Centro superior
	this->addChild(labelTeclas, 1);

	_humano = Sprite::create("imagenes/_humano.png");
	_humano->setPosition(400, 55);
	inicializarFisica(_humano);
	addChild(_humano, 1);

	inicializarTeclado();
	inicializarColisiones();  // Llamada para inicializar las colisiones
	schedule([this](float dt) {
		this->agregarBombar(dt);
		}, 5.0f, "agregarBombarKey");

	// Programar la actualización del tiempo
	schedule([this](float dt) {
		this->actualizarTiempo(dt);
		}, 1.0f, "actualizarTiempoKey");

	// Reproducir música de fondo (en bucle)
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("sounds/musica_fondo.mp3", true);

	return true;
}
