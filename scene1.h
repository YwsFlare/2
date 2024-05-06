#ifndef SCENE1_H
#define SCENE1_H

#include "tower.h"
#include "myobject.h"
#include <QMainWindow>
#include <QSoundEffect>
#include <QPushButton>
#include<QTimer>
#include<QLabel>

class scene1 : public QMainWindow
{
    Q_OBJECT
public:
    QTimer *timer1;
    QTimer *timer2;
    QTimer *timer3;
    QSoundEffect *sound1;
    QSoundEffect *sound2;
    QSoundEffect *sound3;
    QSoundEffect *sound4;
    QSoundEffect *sound5;
    QPushButton *button1;
    QPushButton* pause;
    QPushButton *button2;
    QLabel* upbackground;
    QLabel *moneynumber;
    QLabel* wavenumber;
    QList<tower*> tower_list;//tower列表,便于管理tower
    QList<myobject*> monster_list;//怪物列表
    QList<myobject*> bullet_list;//子弹列表
    explicit scene1(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

    void setTower(int x,int y,QString towerpixmfileName,QString bulletpixfileName);
    void addmonster();
    void addbullet(QPoint pos1,QPoint pos2,QString bulletpixfileName,myobject *lockedmonster,int size);//发射子弹

    void updatescene();

    void performTowerAttacks(QList<tower*>& towerList, QList<myobject*>& monsterList);//判断周围是否有怪物，target，然后发射子弹
    void playSoundEffect(QSoundEffect* &sound, const QString &filePath, double volume);//播放音乐
    void setupButton(QPushButton* button, const QString &iconPath, int posX, int posY);//设置按钮
    void handleTowerPlacement(int index);//防止tower
   // void handleTowerUpgrade(int index);//升级tower
    void setupUpgradeOptions(int index);//设置升级
    void connectUpgradeButtons(int index);
    void removeTower(int index);
    void upgradeTower(int index);//升级操作
    void cleanupButtons();

    void updateMonsterMovements();//怪物移动函数
    void updateMonsterDirection(myobject* monster) ;//monster换方向
    bool isMonsterAtEndpoint(myobject* monster);//是否到达终点
    void handleMonsterArrivalAtEndpoint(int monsterIndex);//是否血量为0
    void endGame() ;

    void updateBulletCollisions();//子弹是否接触到monster
    void applyBulletDamage(myobject* bullet);//子弹造成伤害

      void checkVictoryConditions();  // 检查胜利条件的方法
private:


signals:

};


#endif // SCENE1_H
