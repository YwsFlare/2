#include "scene1.h"
#include "tower.h"
#include "math.h"
#include <QPainter>
#include <QPixmap>
#include <QPaintEvent>
#include <QDebug>
#include <QSplashScreen>
#include <QLabel>
#include <QMovie>
#include <windows.h>
#include<QMap>
#include <QMouseEvent>
#include <QTimer>
#include "win.h"
#include "lose.h"
#include "money.h"
#include "hp.h"
bool operator<(const QPoint& p1, const QPoint& p2) {//重写小于
    if (p1.x() == p2.x()) {
        return p1.y() < p2.y();
    } else {
        return p1.x() < p2.x();
    }
}
myobject* monsteroccur(int h1,int h2,int k);
void lockTowerAngles(QList<tower*>& tower_list);// 函数用于为每个塔计算旋转角度以锁定到最近的怪物
class point1
{
public:
    int x;
    int y;
    bool available;//是否可放置
    point1(){
        available=true;
    }
}p1[15];//记录可放置tower的点的位置,1~14编号对应14个tower点位

static void pointinitialize();
static float getdistance(QPoint pos1,QPoint pos2);
static int count=0;//记录怪物数
static int wave=1;//记录怪物波数
static hp1* carrot_hp1=nullptr;//血条
static money1*mymoney1=nullptr;//金钱

scene1::scene1(QWidget *parent)//构造函数
    : QMainWindow{parent}
{
    this->setFixedSize(800,500);
    pointinitialize();
    {
        count=0;
        wave=1;
        timer1=new QTimer;//界面更新
        timer2=new QTimer;//怪物生成
        timer3=new QTimer;//子弹生成
        //初始化button
        button1=new QPushButton;
        button2=new QPushButton;
        QPixmap pixmap1(":/tower/images/choose1.png");//瓶子
        button1->setFixedSize(pixmap1.size());
        button1->setStyleSheet("QPushButton{Border:Opx;}");
        button1->setIcon(pixmap1);
        button1->setIconSize(QSize(pixmap1.size()));

        QPixmap pixmap2(":/tower/images/choose2.png");//冰冻

        button2->setFixedSize(pixmap2.size());
        button2->setStyleSheet("QPushButton{Border:Opx;}");
        button2->setIcon(pixmap2);
        button2->setIconSize(QSize(pixmap2.size()));

       // 设置背景图片（包含自适应大小操作）
        QPalette background = this->palette();//获取当前背景
        QImage Img(":/stage/images/map1.png");
        QImage image=Img.scaled(this->width(),this->height(), Qt::IgnoreAspectRatio);//自适应的实现
        background.setBrush(QPalette::Window, QBrush(image));
        this->setPalette(background);

        //萝卜gif动图添加
        QMovie *movie=new QMovie(":/extra/images/carrot.gif");
        QLabel *label=new QLabel(this);
        label->setFixedSize(70,70);
        movie->setScaledSize(label->size());
        label->setMovie(movie);
        movie->setSpeed(300);
        label->move(668,90);
        movie->start();
        //血条初始化
        carrot_hp1=new hp1(this);
        //金币初始化
        mymoney1=new money1(this);
        //避免提前设置好，避免一直重新设置造成卡顿
        sound3=new QSoundEffect(this);
        sound3->setSource(QUrl::fromLocalFile("D:\\project1\\carrots\\monster_fat1.wav"));//必须绝对路径
        sound3->setVolume(0.3);
        sound4=new QSoundEffect(this);
        sound4->setSource(QUrl::fromLocalFile("D:\\project1\\carrots\\tower_attack.wav"));//必须绝对路径
        sound4->setVolume(0.7);
        sound5=new QSoundEffect(this);
        sound5->setSource(QUrl::fromLocalFile("D:\\project1\\carrots\\carrot.wav"));//必须绝对路径
        sound5->setVolume(0.3);

        //播放背景音乐
        sound1=new QSoundEffect(this);
        sound1->setSource(QUrl::fromLocalFile("D:\\project1\\carrots\\gamebk_music.wav"));//必须绝对路径
        sound1->setLoopCount(QSoundEffect::Infinite);
        sound1->setVolume(3.3);
        sound1->play();

        //开启屏幕不断更新
        connect(timer1,&QTimer::timeout,this,&scene1::updatescene);
        timer1->start(15);
        //go按钮，代表游戏开始
        //monster生成
        {
             QPushButton *button=new QPushButton(this);
            setupButton(button,":/extra/images/go.png",10,10);//go按钮
            connect(button,&QPushButton::clicked,this,[=](){
                //点击音效
                sound2=new QSoundEffect(this);
                sound2->setSource(QUrl::fromLocalFile("D:\\project1\\carrots\\button_press.wav"));//必须绝对路径
                sound2->setVolume(1.5);
                sound2->play();
                //计时器定时更新调用
                connect(timer2,&QTimer::timeout,this,&scene1::addmonster);//点击go后游戏开始，设置信号和槽，time2是怪物，time3是子弹
                connect(timer3,&QTimer::timeout,this,[=](){//到时间检查周围有没有monster，有的话就发射子弹
                    performTowerAttacks(tower_list,monster_list);
                });
                timer2->start(500);
                timer3->start(500);
                button->close();
            }
            );//lambda表达式
        }
    }
}

void scene1::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        button1->close();
        button2->close();
        button1->disconnect();
        button2->disconnect();
        int x=this->mapFromGlobal(QCursor().pos()).x();//
        int y=this->mapFromGlobal(QCursor().pos()).y();//获取鼠标在窗口的相对位置
        qDebug()<<x<<" "<<y;//鼠标点击相对位置的输出
        for(int i=1;i<=24;i++)
        {
            if(x>p1[i].x&&x<p1[i].x+40&&y>p1[i].y&&y<p1[i].y+40&&p1[i].available==true)//如果鼠标点击处位于tower放置点且当前位置可放置Tower
            {
                 playSoundEffect(sound2, "D:\\project1\\carrots\\button_press.wav", 1.5);
                handleTowerPlacement(i);
                 qDebug()<<QString("yy");
                break;
            }
            if(x>p1[i].x&&x<p1[i].x+40&&y>p1[i].y&&y<p1[i].y+40&&p1[i].available==false)//false代表可以升级
            {
                setupUpgradeOptions(i);
                qDebug()<<QString("xx");
                break;
            }
        }
    }

}

void scene1::paintEvent(QPaintEvent *)
{
    foreach(tower *new_tower,tower_list)
    {
        QPainter *painter1=new QPainter(this);
        new_tower->draw(painter1);//一直进行绘制，旋转
        painter1->end();
    }//给每个tower进行绘制
    QPainter *painter2=new QPainter(this);
    foreach(myobject *object,monster_list)
        object->draw(painter2);
    //给每个物体进行绘制
    foreach(myobject *object,bullet_list)
        object->draw(painter2);
    //给每个子弹进行绘制
    painter2->end();
}

void scene1::setTower(int x,int y,QString towerpixfileName,QString bulletpixfileName)
{
    tower *new_tower=new tower(QPoint(x,y),towerpixfileName,bulletpixfileName);
    tower_list.push_back(new_tower);
    update();

}

void scene1::updatescene()//怪物移动，判断是否到达终点，是否死亡，萝卜血量
{//bullet的追踪，移动，转动，判断是否击中
    updateMonsterMovements();
    //子弹移动
    foreach(myobject *monster,monster_list)
    {
        foreach(myobject *bullet,bullet_list)
        {
            if(bullet->lock==monster)
                bullet->targetpos=monster->currentpos;
            bullet->move();
        }
    }

    //判断子弹是否已击中怪物

    updateBulletCollisions();

    lockTowerAngles(tower_list);
    //tower对怪物的旋转角度锁定
    checkVictoryConditions();
}

void scene1::addmonster()//怪物出现
{
    count++;

    //设立各波次的怪物类型与数量
    if(count<=7)
    {
        myobject* monster01=monsteroccur(200,200,1);
        monster_list.push_back(monster01);
    }
    if(count>7&&count<=16)
    {
        myobject* monster02=monsteroccur(300,300,1);
        monster_list.push_back(monster02);
    }
    if(count>16&&count<=32)
    {
        myobject* monster03=monsteroccur(500,500,1);
        monster_list.push_back(monster03);
    }
    if(count==33)
    {
        myobject* monster04=monsteroccur(2000,2000,1);
        monster_list.push_back(monster04);
    }
    //第一波怪物
    if(count>=3&&wave==1)
    {
        wave=2;
        timer2->stop();
        QTimer::singleShot(7000,this,[=](){
            timer2->start();
        });
    }

    //第二波怪物
    if(count>=10&&wave==2)
    {
        wave=3;
        timer2->stop();
        QTimer::singleShot(7000,this,[=](){
            timer2->start();
        });
    }

    //第三波怪物
    if(count>=20&&wave==3)
    {
        wave=4;
        timer2->stop();
        QTimer::singleShot(7000,this,[=](){
            timer2->start();
        });
    }

    //第四波怪物
    if(count>=27&&wave==4)
    {
        wave=5;
        timer2->stop();
        QTimer::singleShot(7000,this,[=](){
           timer2->start();
       });
    }

    //第五波怪物
    if(count>=33&&wave==5)
    {
        timer2->stop();
    }
}

void scene1::addbullet(QPoint pos1,QPoint pos2,QString bulletpixfileName,myobject *lockedmonster,int _size)//size为攻击力
{
    sound4->play();
    myobject* bullet=new myobject(pos1,pos2,bulletpixfileName);
    if(bulletpixfileName.compare(":/extra/images/bullet2.png")==0)
        bullet->bullet_effect=1;
    bullet->lock=lockedmonster;//子弹锁定的monster
    bullet->speed=7.0;
    bullet->size=_size;
    bullet_list.push_back(bullet);
}
// 执行塔的攻击逻辑
void scene1:: performTowerAttacks(QList<tower*>& towerList, QList<myobject*>& monsterList) {
    for (int i = 0; i < towerList.size(); i++) {
        int check = 0;  // 检查是否范围内已没有怪物
        for (int j = 0; j < monsterList.size(); j++) {
            if (getdistance(towerList[i]->pos, monsterList[j]->currentpos) < 300) {//寻找最近的
                int size = towerList[i]->level == 1 ? 17 : 30;
                towerList[i]->target = monsterList[j];
                addbullet(towerList[i]->pos, monsterList[j]->currentpos, towerList[i]->bulletpixfileName, monsterList[j], size);
                check = 1;
                break;
            }
        }
        if (check == 0) {
            towerList[i]->target = NULL;
        }
    }
}

void scene1::playSoundEffect(QSoundEffect* &sound, const QString &filePath, double volume) {
    //if (!sound) {
        sound = new QSoundEffect(this);
        sound->setSource(QUrl::fromLocalFile(filePath));
        sound->setVolume(volume);

    sound->play();
}

void scene1::setupButton(QPushButton* button22, const QString &iconPath, int posX, int posY) {
    QPixmap pixmap(iconPath);
   // pixmap.scaled(40,40);
    button22->setParent(this);
    button22->setFixedSize(pixmap.size());
    button22->setStyleSheet("QPushButton{Border:0px;}");
    button22->setIcon(pixmap);
    button22->setIconSize(pixmap.size());
    button22->move(posX, posY);
    button22->show();
}

void scene1::handleTowerPlacement(int i) {
    setupButton(button1, ":/tower/images/choose1.png", p1[i].x-3, p1[i].y+40);
    setupButton(button2, ":/tower/images/choose2.png", p1[i].x-3, p1[i].y-52);
    if(mymoney1->nowmoney>=100)
    {
        connect(button1, &QPushButton::clicked, this, [=]() {
            sound2->setVolume(1.5);
            sound2->play();
            setTower(p1[i].x+5, p1[i].y+5, ":/tower/images/tower1.png", ":/extra/images/bullet1.png");
            p1[i].available = false;
            mymoney1->buytower100();
            button1->close();
            button2->close();
            button1->disconnect();
            button2->disconnect();
        });
    }


    if(mymoney1->nowmoney >= 180) {
        connect(button2, &QPushButton::clicked, this, [=]() {
            sound2->setVolume(1.5);
            sound2->play();
            setTower(p1[i].x+5, p1[i].y+5, ":/tower/images/tower2.png", ":/extra/images/bullet2.png");
            p1[i].available = false;
            mymoney1->buytower180();
            button1->close();
            button2->close();
            button1->disconnect();
            button2->disconnect();
        });
    }
}
void scene1::setupUpgradeOptions(int index) {
    playSoundEffect(sound2, "D:\\project1\\carrots\\button_press.wav", 1.5);
    setupButton(button1, ":/tower/images/remove.png", p1[index].x - 3, p1[index].y + 40);
    setupButton(button2, ":/tower/images/level_up.png", p1[index].x - 3, p1[index].y - 52);

    connectUpgradeButtons(index);
}

void scene1::connectUpgradeButtons(int index) {
    connect(button1, &QPushButton::clicked, this, [=]() {
        removeTower(index);
        cleanupButtons();
    });

    if (mymoney1->nowmoney >= 180) {
        connect(button2, &QPushButton::clicked, this, [=]() {
            upgradeTower(index);
            cleanupButtons();
        });
    }
}

void scene1::removeTower(int index) {
    sound2->setVolume(1.5);
    sound2->play();
    for (int j = 0; j < tower_list.size(); j++) {
        if ((tower_list[j]->level == 1 && tower_list[j]->pos == QPoint(p1[index].x + 5, p1[index].y + 5)) ||
            (tower_list[j]->level == 2 && tower_list[j]->pos == QPoint(p1[index].x, p1[index].y))) {
            delete tower_list[j];
            tower_list.removeAt(j);
            p1[index].available = true;
            break;
        }
    }
    mymoney1->destroytower();
}

void scene1::upgradeTower(int index) {
    sound2->setVolume(1.5);
    sound2->play();
    foreach (tower* _tower, tower_list) {
        if (_tower->pos == QPoint(p1[index].x + 5, p1[index].y + 5) && _tower->level == 1) {
            mymoney1->buytower180();
            _tower->pos=QPoint(p1[index].x,p1[index].y);
            _tower->size=40;
            _tower->level=2;
            p1[index].available = false;
            break;
        }
    }
}

void scene1::cleanupButtons() {
    button1->close();
    button2->close();
    button1->disconnect();
    button2->disconnect();
}

void scene1::updateMonsterMovements() {
    for (int i = 0; i < monster_list.size(); i++)
        {
        updateMonsterDirection(monster_list[i]);

        // 检查怪物是否抵达终点
        if (isMonsterAtEndpoint(monster_list[i])) {
            handleMonsterArrivalAtEndpoint(i);
            i--; // 修正索引以应对移除操作
            continue;
        }
        if(monster_list[i]->now_hp<=0)
        {
            sound3->play();
            mymoney1->killenemy();
            for(int j=0;j<bullet_list.size();)
            {
                if(bullet_list[j]->lock==monster_list[i])
                {
                    delete bullet_list[j];
                    bullet_list.removeAt(j);
                }
            }
            delete monster_list[i];
            monster_list.removeAt(i);
            i--;
            continue;
        }
        //怪物移动
        monster_list[i]->move();
    }
}

void scene1::updateMonsterDirection(myobject* monster) {
    static const QList<QPoint> pathPoints = {
        {80, 320}, {277, 320}, {277, 260}, {480, 260}, {480, 320}, {675, 320}, {675, 165}
    };
    static const QMap <QPoint, QPoint> nextPoints = {
        {{80, 320}, {277, 320}}, {{277, 320}, {277, 260}}, {{277, 260}, {480, 260}},
        {{480, 260}, {480, 320}}, {{480, 320}, {675, 320}}, {{675, 320}, {675, 165}}
    };
    QPoint currentPos = monster->currentpos;
    if (nextPoints.contains(currentPos)) {
        monster->targetpos = nextPoints[currentPos];//改变目标位置。如果到达拐角
    }
}

bool scene1::isMonsterAtEndpoint(myobject* monster) {
    return monster->currentpos == QPoint(675, 165);
}

void scene1::handleMonsterArrivalAtEndpoint(int monsterIndex) {
    myobject* monster = monster_list[monsterIndex];
    sound5->play();
    // 移除锁定此怪物的所有子弹
    for (int j = 0; j < bullet_list.size();) {
        if (bullet_list[j]->lock == monster) {
            delete bullet_list[j];
            bullet_list.removeAt(j);
        }
    }

    // 移除怪物并处理生命值
    delete monster_list[monsterIndex];
    monster_list.removeAt(monsterIndex);
    carrot_hp1->hurt();
    carrot_hp1->setpic();

    // 检查游戏是否结束
    if (carrot_hp1->ifdead()) {
        endGame();
    }
}

void scene1::endGame() {
    lose* los = new lose(1);
    los->show();
    this->sound1->stop();
    timer3->stop();
    timer2->stop();
    timer1->stop();
    for (int i = 1; i < 15; i++)
        p1[i].available = true;

    QTimer::singleShot(1000, [=]() {
        this->close();
    });
}
void scene1::checkVictoryConditions() {
    if (wave == 5 && monster_list.isEmpty() && !carrot_hp1->ifdead()) {
        sound1->stop();
        timer3->stop();
        timer2->stop();
        timer1->stop();

        for (int i = 1; i < 15; i++) {
            p1[i].available = true;
        }

        win* w = new win();
        QTimer::singleShot(1000, [=]() {
            this->close();
            w->show();
        });
    }

    mymoney1->updatemoney();
    update();
}
// 函数用于为每个塔计算旋转角度以锁定到最近的怪物
void lockTowerAngles(QList<tower*>& tower_list) {

    foreach(tower *_tower,tower_list)
    {
        if(_tower->target!=NULL)
        {
            float _angle=0;
            float deltax=_tower->pos.x()-_tower->target->currentpos.x();
            float deltay=_tower->pos.y()-_tower->target->currentpos.y();

            if(deltax>0&&deltay!=0)
            {
                _angle=(atan(deltay/deltax)*180)/3.1416;
                _tower->angle=270+_angle;//270
                //qDebug()<<_tower->angle;
            }
            else if(deltax<0&&deltay!=0)
            {
                _angle=(atan(deltay/deltax)*180)/3.1416;
                _tower->angle=90+_angle;
                //qDebug()<<_tower->angle;
            }
            else if(deltax==0&&deltay<0)
            {
                _tower->angle=180;
            }
        }
        else _tower->angle=0;
    }
}
void scene1::updateBulletCollisions() {//如果打到monster就删除子弹
    bool playedSound = false;  // 防止多次播放声音
    for (int i = 0; i < bullet_list.size(); i++) {
        if (getdistance(bullet_list[i]->currentpos, bullet_list[i]->lock->currentpos) < 10) {
            if (!playedSound) {
                playSoundEffect(sound3, "path/to/soundfile.wav", 1.0);  // 封装好的播放声音方法
                playedSound = true;
            }
            applyBulletDamage(bullet_list[i]);
            delete bullet_list[i];
            bullet_list.removeAt(i);
            i--;  // 确保不跳过下一个元素
        }
    }
}
void scene1::applyBulletDamage(myobject* bullet) {//子弹的效果和伤害
    myobject* monster = bullet->lock;
    int damage = (bullet->size == 17) ? 50 : 100;  // 根据子弹类型决定伤害值
    monster->now_hp -= damage;
    if (bullet->bullet_effect == 1) {
        monster->monster_effect = 1;  // 应用冰冻效果
    }
}
myobject* monsteroccur(int h1,int h2,int k)
{
    myobject* monster03=new myobject(QPoint(80,140),QPoint(80,320),QString(":/monster/images/monster%1.png").arg(k));
    monster03->hp=h1;
    monster03->now_hp=h2;
    monster03->ifmonster=1;
    return monster03;
}
static void pointinitialize()
{
    {
        p1[1].x=140;
        p1[1].y=200;
        p1[2].x=140;
        p1[2].y=265;
        p1[3].x=210;
        p1[3].y=265;
        p1[4].x=140;
        p1[4].y=385;
        p1[5].x=210;
        p1[5].y=385;
        p1[6].x=345;
        p1[6].y=200;
        p1[7].x=345;
        p1[7].y=325;
        p1[8].x=410;
        p1[8].y=200;
        p1[9].x=410;
        p1[9].y=325;
        p1[10].x=543;
        p1[10].y=265;
        p1[11].x=543;
        p1[11].y=385;
        p1[12].x=610;
        p1[12].y=265;
        p1[13].x=610;
        p1[13].y=385;
        p1[14].x=610;
        p1[14].y=200;
    }//给每个塔防点坐标赋值,x、y为正方形左上角坐标，宽度30
}
static float getdistance(QPoint pos1,QPoint pos2)
{
    int deltax=pos1.x()-pos2.x();
    int deltay=pos1.y()-pos2.y();
    float result=sqrt(pow(deltax,2)+pow(deltay,2));
    return result;
}
