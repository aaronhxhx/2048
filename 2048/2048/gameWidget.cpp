#include "gameWidget.h"

//颜色数组 存储每个数字对应的背景色,2 4, 8...2048, 11个数字对应不同的颜色
QColor ditiBkg[11] = { QColor::fromRgb(0xFF, 0xFF, 0xCC), QColor::fromRgb(0xFF, 0xFF, 0x99),
							QColor::fromRgb(0xFF, 0xCC, 0xCC), QColor::fromRgb(0xFF, 0xCC, 0x99),
							QColor::fromRgb(0xFF, 0x99, 0x99), QColor::fromRgb(0xFF, 0x99, 0x66),
							QColor::fromRgb(0xFF, 0x66, 0x66), QColor::fromRgb(0xCC, 0x99, 0x66),
							QColor::fromRgb(0xCC, 0x33, 0x33), QColor::fromRgb(0xCC, 0x00, 0x33),
							QColor::fromRgb(0xFF, 0x00, 0x00)
};

QPointF dPos[5];				//每个位置的方向增量




GameWidget::GameWidget(QWidget* parent):
	QGLWidget(QGLFormat(QGL::SampleBuffers), parent), timer(this)
{
	//连接手势移动信号和相应的槽函数
	connect(this, &GameWidget::GestureMove, this, &GameWidget::onGestureMove);
	//连接时钟信号和画板更新的槽
	//connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
	connect(&timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
	//初始化board数组
	memset(board, 0, sizeof(board));
	//初始化分数
	score = 0;
	//数码块的个数为 2
	digitCount = 2;
	//播放动画效果无
	isAnimating = false;
	//初始化两个方格
	init2Block();
}

GameWidget::~GameWidget()
{
}

void GameWidget::init2Block()
{
	//随机位置且保证两次位置不相同
	board[rand() % 4][rand() % 4] = 2;
	int i = rand() % 4;
	int j = rand() % 4;
	while (board[i][j]  != 0)
	{
		i = rand() % 4;
		j = rand() % 4;
	}
	board[i][j] = 2;
	update();
}

void GameWidget::mousePressEvent(QMouseEvent *event)
{
	//获得起点坐标
	startPos = event->pos();
}

void GameWidget::mouseReleaseEvent(QMouseEvent *event)
{
	//如果在播放动画效果则直接退出防止重复产生手势事件
	if (isAnimating)
		return;
	//根据终点坐标和起点坐标计算XY坐标的增量
	float dX = (float)(event->pos().x() - startPos.x());
	float dY = (float)(event->pos().y() - startPos.y());

	//确定手势的方向
	if (abs(dX) > abs(dY))
	{
		if (dX < 0)
		{
			emit GestureMove(LEFT);
		}
		else
		{
			emit GestureMove(RIGHT);
		}
	}
	else
	{
		if (dY < 0)
		{
			emit GestureMove(UP);
		}
		else
		{
			emit GestureMove(DOWN);
		}
	}
}

void GameWidget::paintEvent(QPaintEvent *)
{
	//构造一个QPainter对象
	QPainter painter(this);

	//设置反锯齿绘图
	painter.setRenderHint(QPainter::Antialiasing);

	//
	if (isAnimating)
	{
		if (drawAnimation(painter))
		{
			isAnimating = false;
			timer.stop();
			//清除动画
			animationList.clear();
			if (digitCount == 16)
			{
				//检查是否游戏结束
				if (checkGameOver())
				{
					emit GameOver();
				}
			}

			if (checkGameWin())
			{
				emit Win();
			}
		}
		return;
	}

	// 构造一个画刷 颜色为R G B分量分别为141 121 81的颜色
	QBrush brush(QColor::fromRgb(141, 121, 81));
	// 使painter应用这个画刷
	painter.setBrush(brush);

	// 设置画笔为空笔 目的是使绘制的图形没有描边
	painter.setPen(Qt::NoPen);

	// 绘制一个矩形
	painter.drawRoundedRect(QRectF(2 * ratioW, 2 * ratioH, width() - 4 * ratioW, height() - 4 * ratioH), rX, rY);

	/* 构造一个字体
	 * 字体名字为Consolas
	 * 字体设置为粗体
	 * 字体大小为40像素
	 * */
	QFont font;
	font.setFamily("Consolas");
	font.setBold(true);
	font.setPixelSize(40 * ratioH);
	// 使painter应用这个字体
	painter.setFont(font);

	// 循环绘制游戏面板
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			// 如果方格中有数字
			if (board[i][j])
			{
				// 设置画刷颜色为数码对应的颜色
				brush.setColor(ditiBkg[getBitCount(board[i][j])]);
				// 应用这个画刷
				painter.setBrush(brush);
				// 绘制一个小方格
				painter.drawRoundedRect(QRectF(7 * ratioW + (w + 5 * ratioW) * j, 7 * ratioH + (h + 5 * ratioH) * i, w, h), rX, rY);
				// 设置画笔为黑色画笔
				painter.setPen(QColor::fromRgb(0, 0, 0));
				// 绘制数码
				painter.drawText(QRectF(7 * ratioW + (w + 5 * ratioW) * j, 7 * ratioH + (h + 5 * ratioH) * i, w, h), Qt::AlignCenter,
					QString::number(board[i][j]));
				// 设置画笔为空笔
				painter.setPen(Qt::NoPen);
			}
			// 如果方格中没有数字
			else
			{
				// 设置画刷颜色为 RGB分量为171 165 141的颜色
				brush.setColor(QColor::fromRgb(171, 165, 141));
				// 应用这个画刷
				painter.setBrush(brush);
				// 绘制小方格
				painter.drawRoundedRect(QRectF(7 * ratioW + (w + 5 * ratioW) * j, 7 * ratioH + (h + 5 * ratioH) * i, w, h), rX, rY);
			}
		}


}

void GameWidget::onGestureMove(GestureDirect direct)
{
	int i, j, k;
	Animation a;
	//记录是否移动过方格以及是否有方格移动
	bool move = false, combine = false;
	//记录某个格子是否参加过合并
	bool isCombine[4][4];
	memset(isCombine, 0, sizeof(isCombine));
	
	//处理不同方向
	switch (direct)
	{
		//向左
	case LEFT:
		//循环每一行
		for (int i = 0; i < 4; i++)
		{
			//初始化j，k为0
			//找到不为 0 的点，j，k表示同一行的列交换位置
			j = 0, k = 0;
			while (true)
			{
				while (j < 4 && board[i][j] == 0)
					j++;
				if (j > 3)
					break;
				qSwap(board[i][k], board[i][j]);
				if (j != k)
					move = true;

				//记录动画信息
				a.type = MOVE;
				a.startPos = QPointF(7 * ratioW + (w + 5 * ratioW) * j, 7 * ratioH + (h + 5 * ratioH) * i);
				a.endPos = QPointF(7 * ratioW + (w + 5 * ratioW) * k, 7 * ratioH + (h + 5 * ratioH) * i);
				a.digit1 = a.digit2 = board[i][k];
				a.direct = LEFT;

				//如果合并后和钱一列数字相同
				if (k > 0 && board[i][k] == board[i][k - 1] && !isCombine[i][k - 1])
				{
					//前一列的数字*2，本列数字为0，该点合并矩阵为1，分数增加,数字块减一
					board[i][k - 1] <<= 1;
					isCombine[i][k - 1] = true;
					board[i][k] = 0;
					combine = true;
					a.digit2 = board[i][k - 1];
					a.endPos = QPointF(7 * ratioW + (w + 5 * ratioW) * (k - 1), 7 * ratioH + (h + 5 * ratioH) * i);
					score += board[i][k - 1];
					emit ScoreIncreasing(score);
					digitCount--;
				}
				else
					k++;
				j++;
				//添加到动画链表
				animationList.append(a);
			}
		}
		break;
	case RIGHT:
		for (i = 0; i < 4; i++)
		{
			j = 3, k = 3;
			while (true)
			{
				while (j > -1 && board[i][j] == 0)
					j--;
				if (j < 0)
					break;
				qSwap(board[i][k], board[i][j]);
				if (j != k)
					move = true;
				a.type = MOVE;
				a.startPos = QPointF(7 * ratioW + (w + 5 * ratioW) * j, 7 * ratioH + (h + 5 * ratioH) * i);
				a.endPos = QPointF(7 * ratioW + (w + 5 * ratioW) * k, 7 * ratioH + (h + 5 * ratioH) * i);
				a.digit1 = a.digit2 = board[i][k];
				a.direct = RIGHT;
				if (k < 3 && board[i][k] == board[i][k + 1] && !isCombine[i][k + 1])
				{
					board[i][k + 1] <<= 1;
					isCombine[i][k + 1] = true;
					board[i][k] = 0;
					combine = true;
					a.digit2 = board[i][k + 1];
					a.endPos = QPointF(7 * ratioW + (w + 5 * ratioW) * (k + 1), 7 * ratioH + (h + 5 * ratioH) * i);
					score += board[i][k + 1];
					emit ScoreIncreasing(score);
					digitCount--;
				}
				else
					k--;
				j--;
				animationList.append(a);
			}
		}
		break;
	case UP:
		for (i = 0; i < 4; i++)
		{
			j = 0, k = 0;
			while (true)
			{
				while (j < 4 && board[j][i] == 0)
					j++;
				if (j > 3)
					break;
				qSwap(board[k][i], board[j][i]);
				if (j != k)
					move = true;
				a.type = MOVE;
				a.startPos = QPointF(7 * ratioW + (w + 5 * ratioW) * i, 7 * ratioH + (h + 5 * ratioH) * j);
				a.endPos = QPointF(7 * ratioW + (w + 5 * ratioW) * i, 7 * ratioH + (h + 5 * ratioH) * k);
				a.digit1 = a.digit2 = board[k][i];
				a.direct = UP;
				if (k > 0 && board[k][i] == board[k - 1][i] && !isCombine[k - 1][i])
				{
					board[k - 1][i] <<= 1;
					isCombine[k - 1][i] = true;
					board[k][i] = 0;
					combine = true;
					a.digit2 = board[k - 1][i];
					a.endPos = QPointF(7 * ratioW + (w + 5 * ratioW) * i, 7 * ratioH + (h + 5 * ratioH) * (k - 1));
					score += board[k - 1][i];
					emit ScoreIncreasing(score);
					digitCount--;
				}
				else
					k++;
				j++;
				animationList.append(a);
			}
		}
		break;
	case DOWN:
		for (i = 0; i < 4; i++)
		{
			j = 3, k = 3;
			while (true)
			{
				while (j > -1 && board[j][i] == 0)
					j--;
				if (j < 0)
					break;
				qSwap(board[k][i], board[j][i]);
				if (j != k)
					move = true;
				a.type = MOVE;
				a.startPos = QPointF(7 * ratioW + (w + 5 * ratioW) * i, 7 * ratioH + (h + 5 * ratioH) * j);
				a.endPos = QPointF(7 * ratioW + (w + 5 * ratioW) * i, 7 * ratioH + (h + 5 * ratioH) * k);
				a.digit1 = a.digit2 = board[k][i];
				a.direct = DOWN;
				if (k < 3 && board[k][i] == board[k + 1][i] && !isCombine[k + 1][i])
				{
					board[k + 1][i] <<= 1;
					isCombine[k + 1][i] = true;
					board[k][i] = 0;
					combine = true;
					a.digit2 = board[k + 1][i];
					a.endPos = QPointF(7 * ratioW + (w + 5 * ratioW) * i, 7 * ratioH + (h + 5 * ratioH) * (k + 1));
					score += board[k + 1][i];
					emit ScoreIncreasing(score);
					digitCount--;
				}
				else
					k--;
				j--;
				animationList.append(a);
			}
		}
		break;
	}

	//数字没有填满
	if ((move || combine) && digitCount != 16)
	{
		//继续随机产生数字
		i = rand() % 4;
		j = rand() % 4;
		while (board[i][j]  != 0)
		{
			i = rand() % 4, j = rand() % 4;
		}

		//填入数字2或者4
		board[i][j] = (rand() % 2 + 1) * 2;

		//记录动画信息
		a.type = APPEARANCE;
		a.startPos = a.endPos = QPointF(7 * ratioW + (w + 5 * ratioW) * j, 7 * ratioH + (h + 5 * ratioH) * i);
		a.startPos += QPointF(w / 2, h / 2);
		a.digit1 = board[i][j];
		animationList.append(a);
		digitCount++;
	}

	//开始绘制动画效果
	isAnimating = true;
	//启动计时器
	timer.start(10);
}

bool GameWidget::drawAnimation(QPainter& painter)
{
	//动画列表迭代器
	QList<Animation>::iterator it;

	//字体
	QFont font;
	font.setFamily("Consolas");
	font.setBold(true);
	font.setPixelSize(40 * ratioH);
	painter.setFont(font);

	//表示动画是否播放完毕
	bool ok = true;

	//构造画刷 颜色为RGB为141 121 81
	QBrush brush(QColor::fromRgb(141, 121, 81));
	// 使painter应用这个画刷
	painter.setBrush(brush);

	// 设置画笔为空笔 目的是使绘制的图形没有描边
	painter.setPen(Qt::NoPen);

	// 绘制一个矩形
	painter.drawRoundedRect(QRectF(2 * ratioW, 2 * ratioH, width() - 4 * ratioW, height() - 4 * ratioH), rX, rY);

	// 设置画刷颜色为 RGB分量为171 165 141的颜色
	brush.setColor(QColor::fromRgb(171, 165, 141));
	// 应用这个画刷
	painter.setBrush(brush);

	//循环绘制游戏面板
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			painter.drawRoundedRect(QRectF(7 * ratioW + (w + 5 * ratioW)*j, 7 * ratioH + (h + 5 * ratioH)*i, w, h), rX, rY);
		}
	}

	//循环播放每个方格的动画
	for (it = animationList.begin(); it != animationList.end(); it++)
	{
		if (!playAnimation(*it, painter))
			ok = false;
	}
	return ok;
}

void GameWidget::resizeEvent(QResizeEvent* event)
{
	ratioW = width() / 400.0;
	ratioH = height() / 400.0;
	//计算每个小格子的宽度和高度,-4因为起始点
	w = width() - 4 * ratioW;
	h = height() - 4 * ratioH;
	w = (width() - 29 * ratioW) / 4;
	h = (height() - 29 * ratioH) / 4;
	rX = 15 * ratioW;
	rY = 15 * ratioH;
	dPos[0] = QPointF(-20 * ratioW, 0);
	dPos[1] = QPointF(20 * ratioW, 0);
	dPos[2] = QPointF(0, -20 * ratioH);
	dPos[3] = QPointF(0, 20 * ratioH);
	dPos[4] = QPointF(-4 * ratioW, -4 * ratioH);
}

void GameWidget::reStart()
{
	// 初始化相关变量 同构造函数
	score = 0;
	digitCount = 2;
	memset(board, 0, sizeof(board));
	init2Block();
	emit ScoreIncreasing(score);
	update();
}

bool GameWidget::checkGameOver()
{
	// 循环检测是否含有相邻的相同数码
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			if (j != 3 && board[i][j] == board[i][j + 1])
				return false;
			if (i != 3 && board[i][j] == board[i + 1][j])
				return false;
		}
	return true;
}

bool GameWidget::checkGameWin()
{
	// 循环检测是否某个方格的数字为2048
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (board[i][j] == 2048)
				return true;
	return false;
}

int GameWidget::getBitCount(int n)
{
	// 循环获取数字二进制位数
	int c = 0;
	while (n >>= 1)
		c++;
	// 返回位数-1
	return c - 1;
}

bool GameWidget::playAnimation(Animation& a, QPainter& painter)
{
	bool rtn = false;
	QBrush brush(Qt::SolidPattern);

	//移动方格位置
	if (a.type == MOVE)
	{
		switch (a.direct)
		{
		case LEFT:
			if (a.startPos.x() > a.endPos.x())
				a.startPos += dPos[LEFT];
			else
			{
				a.startPos = a.endPos;
				rtn = true;
			}
			break;
		case RIGHT:
			if (a.startPos.x() < a.endPos.x())
				a.startPos += dPos[RIGHT];
			else
			{
				a.startPos = a.endPos;
				rtn = true;
			}
			break;
		case UP:
			if (a.startPos.y() > a.endPos.y())
				a.startPos += dPos[UP];
			else
			{
				a.startPos = a.endPos;
				rtn = true;
			}
			break;
		case DOWN:
			if (a.startPos.y() < a.endPos.y())
				a.startPos += dPos[DOWN];
			else
			{
				a.startPos = a.endPos;
				rtn = true;
			}

		}

		//如果移动到终点
		if (!rtn)
		{
			brush.setColor(ditiBkg[getBitCount(a.digit1)]);
			painter.setBrush(brush);
			painter.drawRoundedRect(QRectF(a.startPos.x(), a.startPos.y(), w, h), rX, rY);
			painter.setPen(QColor::fromRgb(0, 0, 0));
			painter.drawText(QRectF(a.startPos.x(), a.startPos.y(), w, h), Qt::AlignCenter, QString::number(a.digit1));
		}
		else
		{
			brush.setColor(ditiBkg[getBitCount(a.digit2)]);
			painter.setBrush(brush);
			painter.drawRoundedRect(QRectF(a.startPos.x(), a.startPos.y(), w, h), rX, rY);
			painter.setPen(QColor::fromRgb(0, 0, 0));
			painter.drawText(QRectF(a.startPos.x(), a.startPos.y(), w, h), Qt::AlignCenter, QString::number(a.digit2));
		}
		painter.setPen(Qt::NoPen);
	}
	else
	{
		//方格出现的动画效果
		// 方格出现的动画效果
		if (a.startPos.x() > a.endPos.x())
			a.startPos += dPos[4];
		else
			a.startPos = a.endPos, rtn = true;
		brush.setColor(ditiBkg[getBitCount(a.digit1)]);
		painter.setBrush(brush);
		painter.drawRoundedRect(QRectF(a.startPos.x(), a.startPos.y(),
			w - 2 * (a.startPos.x() - a.endPos.x()),
			h - 2 * (a.startPos.y() - a.endPos.y())), rX, rY);
		painter.setPen(QColor::fromRgb(0, 0, 0));
		painter.drawText(QRectF(a.endPos.x(), a.endPos.y(), w, h),
			Qt::AlignCenter, QString::number(a.digit1));
		painter.setPen(Qt::NoPen);
	}
	return rtn;
}