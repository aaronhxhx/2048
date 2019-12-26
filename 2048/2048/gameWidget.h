#pragma once

#include <QGLWidget>
#include <QMouseEvent>
#include <QEventLoop>
#include <QTimer>
#include <QPainter>
#include <QList>


//手势方向
enum GestureDirect
{
	//向上下左右
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3
};

//定义动画类型
enum AnimationType
{
	//方格移动动画
	MOVE = 0,
	//方格出现动画
	APPEARANCE  = 1
};

//定义动画结构体
struct Animation
{
	AnimationType type;				//动画类型
	GestureDirect direct;			//方向
	QPointF startPos;				//起始点坐标
	QPointF endPos;					//终止点坐标
	int digit1;						//数码
	int digit2;						//第二数码 数码可能被合并
};


//游戏部件类 继承QWidget
class GameWidget : public QGLWidget
{
	Q_OBJECT

public:
	explicit GameWidget(QWidget* parent = 0);
	~GameWidget();
private:
	int board[4][4];				//游戏面板 存储每个格子的数值
	int digitCount;					//数码的个数 存储当前面板上的数字的个数
	int score;						//分数 存储当前得分值
	QPoint startPos;				//起始点坐标
	QList<Animation> animationList;	//存储所有需要展现的动画
	qreal w, h;						//每个小格子的宽度和高度
	qreal ratioW, ratioH;			//宽度和高度的缩放比例
	qreal rX, rY;					//小圆角的x，y
	//QImage* createImg;			//缓存图像
	bool isAnimating;				//是否播放动画效果
	QTimer timer;					//计时器

	void init2Block();				//初始化两个方格
	bool checkGameOver();			//检测游戏是否结束
	bool checkGameWin();			//检测游戏是否获胜
	int getBitCount(int);			// 获取一个数字的二进制位数 当然这里获取的不完全是二进制位数 
									//而是对应颜色数组的下标比如 2 对应 0    8 对应 2 
	bool playAnimation(Animation&, QPainter&);	//播放一帧动画
	bool drawAnimation(QPainter&);				//绘制动画效果

	void mousePressEvent(QMouseEvent *event)override;		//重载鼠标事件
	void mouseReleaseEvent(QMouseEvent *event)override;		//鼠标释放事件
	void paintEvent(QPaintEvent*)override;					//重载绘制事件
	void resizeEvent(QResizeEvent*)override;				//重载窗口改变事件

	//信号
signals:
	void GestureMove(GestureDirect);						//手势移动信号
	void ScoreIncreasing(int);								//分数增加信号
	void GameOver();										//游戏结束信号
	void Win();												//游戏获胜信号

	//槽函数
public slots:
	void onGestureMove(GestureDirect);						//手势移动槽函数
	void reStart();											//重新开始槽函数

};
