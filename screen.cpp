/* screen.cpp */
#include <QtGui>
#include "screen.h"
#include <QTextCodec>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QApplication>

Screen::Screen(QWidget *parent)
	: QWidget(parent)
{
	setFocus();										/* 获取焦点，用于接收键盘事件 */
    setFixedSize(800, 480);							/* 设置固定游戏界面大小 */
	init_var();										/* 初始化变量 */
	timer = new QTimer(this);							/* 创建定时器 */

    scoreLabel = new QLabel(tr("Your Score: 0"), this);     /*new 一个 QLabel*/
    upButton = new QPushButton(tr("Up"), this);             /*new 一个 Button*/
    downButton = new QPushButton(tr("Down"), this);
    rightButton = new QPushButton(tr("Right"), this);
    leftButton = new QPushButton(tr("Left"), this);
    quit = new QPushButton(tr("Quit"),this);


    /*移动按钮 分数 位置* 用绝对坐标设置按钮的位置 用绝对坐标设置标签的位置 */
    /*格式：坐标X、坐标Y，按钮长、按钮宽 */

    scoreLabel->setGeometry(100, 380, 100, 50);                 /*分数标签大小 */

    leftButton->setGeometry(325, 380, 50, 40);                  /*左*/

    downButton->setGeometry(380, 380, 50, 40);                  /*下*/

    rightButton->setGeometry(435, 380, 50, 40);                 /*右*/

    upButton->setGeometry(380, 335, 50, 40);                    /*上*/

    quit->setGeometry(490,380,50,40);                           /*退出*/

    /* 设置标签的字体和颜色 */
    QFont font;
    font.setPointSize(10);
    scoreLabel->setFont(font);

	QPalette palette;
	palette.setColor(QPalette::WindowText, Qt::red);
	scoreLabel->setPalette(palette);

    /*信号接受槽*/
	connect(upButton, SIGNAL(clicked()), this, SLOT(upClicked()));
	connect(downButton, SIGNAL(clicked()), this, SLOT(downClicked()));
	connect(rightButton, SIGNAL(clicked()), this, SLOT(rightClicked()));
    connect(leftButton, SIGNAL(clicked()), this, SLOT(leftClicked()));
    connect(quit, SIGNAL(clicked()), this, SLOT(quitClicked()));


	connect(timer, SIGNAL(timeout()), this, SLOT(my_timeout()));
	times = 200;
	timer->start(times);							/* 每200毫秒发出一个信号 */
}

Screen::~Screen()
{
}

void Screen::init_var(void)
{
	IsRun = false;
	IsPause = false;
	IsDie = false;
	IsWin = false;
	score = 0;
}

void Screen::keyPressEvent(QKeyEvent *event)			/* 捕捉键盘事件 */
{
	switch(event->key())
	{
	case Qt::Key_Up:
		snake.ChangeDirection(1);					/* 改变Snake的移动方向 */
		break;
	case Qt::Key_Right:
		snake.ChangeDirection(2);
		break;
	case Qt::Key_Down:
		snake.ChangeDirection(3);
		break;
	case Qt::Key_Left:
		snake.ChangeDirection(4);
		break;
	default:
		break;
	}
	QWidget::keyPressEvent(event);					/* 传递父窗口其他键盘事件 */
}

void Screen::my_timeout(void)
{
	if(IsRun == false) {
		timer->stop();
		return;
	}

	snake.Move();
	IsDie = snake.IsDie();
	IsWin = snake.IsWin();
	if(IsDie) {
		timer->stop();
		IsRun = false;
        QMessageBox::information(this, tr("Game over"),tr("Game over !!!"));
		IsDie = false;
		snake.Clear();
		return;
	}

	if(IsWin) {
		timer->stop();
		IsRun = false;
		QMessageBox::information(this, tr("You win"),tr("Congratulation, You Win!"));
		IsDie = false;
		snake.Clear();
		return;
	}

	score = snake.getScore();
	QString str = QString("Your Score:\n   %1").arg(score);
	scoreLabel->setText(str);

	update();
}

void Screen::paintEvent(QPaintEvent *event)
{
	QPainter  painter(this);
	painter.setBrush(Qt::black);

    painter.drawRect(0, 0, 800, 300);                       /*游戏黑布大小*/

    if(IsDie || !IsRun)
		return;
	vector<Node> node;
	Node food;
	snake.getCoordinate(node, food);						/* 获取Snake和果实的坐标 */

    /* 画果实 */
	painter.setBrush(Qt::red);
    painter.drawEllipse(5 * food.x, 5 * food.y,5, 5);

    /* 画墙  要和 游戏画面大小 相同，不然会撞墙 */
    painter.setBrush(Qt::white);

    /*格式：起点X坐标、起点Y坐标、结束X坐标、结束Y坐标*/
    painter.drawRect(0, 0, 800, 5);
    painter.drawRect(0, 300, 800, 5);
    painter.drawRect(0, 0, 5, 300);
    painter.drawRect(795, 0, 800, 300);                     /*这个要X轴要减去5不然会和边边重复导致看不到*/

	/* 画蛇，蛇头与蛇身用不同的颜色区分*/
	painter.setBrush(Qt::yellow);
	painter.drawRect(5 * node[0].x, 5 * node[0].y,5, 5);

    /* 画蛇身 */
	painter.setBrush(Qt::blue);
	int n = node.size();
	for(int i = 1; i < n; i++)
    painter.drawRect(5 *node[i].x, 5 *node[i].y,5, 5);
	node.clear();
	QWidget::paintEvent(event);
}

void Screen::upClicked(void)
{
	snake.ChangeDirection(1);
	setFocus();
}

void Screen::rightClicked(void)
{
	snake.ChangeDirection(2);
	setFocus();
}

void Screen::downClicked(void)
{
	snake.ChangeDirection(3);
	setFocus();
}

void Screen::leftClicked(void)
{
	snake.ChangeDirection(4);
	setFocus();
}


//结束程序 退出窗口
void Screen::quitClicked(void)
{
    QApplication* app;
    app->quit();
}



void Screen::startGame(void)
{
	snake.Clear();									/* 清空snake的坐标 */
	snake.init_var();									/* 初始化snake变量 */
	IsRun   = true;
	times   = 200;
	timer->start(times);
}

void Screen::continueGame(void)
{
	if(!IsPause)
		return;
	timer->start(times);								/* 继续定时，即继续更新画面 */
	setFocus();
}

void Screen::pauseGame(void)
{
	IsPause = true;
	timer->stop();									/* 暂停定时，即暂停画面 */
	setFocus();
}

void Screen::speedUp(void)
{
	times -= 20;
	if(times <= 20)
		times = 20;
	timer->stop();
	timer->start(times);								/* 加快定时器的频率 */
	setFocus();
}

void Screen::speedDown(void)
{
	times += 20;
	if(times >= 500)
		times = 500;
	timer->stop();
	timer->start(times);
	setFocus();
}

