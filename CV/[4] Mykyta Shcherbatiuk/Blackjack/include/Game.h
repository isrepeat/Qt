#ifndef GAME_H 
#define GAME_H

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPointer>
#include <memory>

#include "GameSceneManager.h"
#include "Button.h"

#define GAME_NAME        "Blackjack"
#define RESOLUTION_X     1200
#define RESOLUTION_Y     675

#define BUTTON_PLAY_POSITION 475 
#define BUTTON_QUIT_POSITION 550 
#define BUTTON_MENU_POSITION 600 

#define RESOLUTION_X     1200
#define RESOLUTION_Y     675

class Game : public QGraphicsView {
    Q_OBJECT

public:
    Game(QWidget *parent = nullptr);

private:
    std::unique_ptr <GameSceneManager>  _game_scene_manager;
    QPointer <QGraphicsScene>           _scene;

    QPointer <Button> _button_play;
    QPointer <Button> _button_quit;
    QPointer <Button> _button_menu;

    Sound sound;

    void addMenuSceneItems();
    void addPlaySceneItems();

    void initMenuScene();
    void initMenuSceneButtons();
    void initPlayScene();
    void initPlaySceneButtons();


    void initButton(QPointer<Button> &button,
                    const char *button_text,
                    const int button_height);

private slots:
    void play();
    void menu();
};

#endif // GAME_H 
