#include "gameWidget.h"

//��ɫ���� �洢ÿ�����ֶ�Ӧ�ı���ɫ,2 4, 8...2048, 11�����ֶ�Ӧ��ͬ����ɫ
QColor ditiBkg[11] = { QColor::fromRgb(0xFF, 0xFF, 0xCC), QColor::fromRgb(0xFF, 0xFF, 0x99),
							QColor::fromRgb(0xFF, 0xCC, 0xCC), QColor::fromRgb(0xFF, 0xCC, 0x99),
							QColor::fromRgb(0xFF, 0x99, 0x99), QColor::fromRgb(0xFF, 0x99, 0x66),
							QColor::fromRgb(0xFF, 0x66, 0x66), QColor::fromRgb(0xCC, 0x99, 0x66),
							QColor::fromRgb(0xCC, 0x33, 0x33), QColor::fromRgb(0xCC, 0x00, 0x33),
							QColor::fromRgb(0xFF, 0x00, 0x00)
};

QPointF dPos[5];				//ÿ��λ�õķ�������




GameWidget::GameWidget(QWidget* parent):
	QGLWidget(QGLFormat(QGL::SampleBuffers), parent), timer(this)
{
	//���������ƶ��źź���Ӧ�Ĳۺ���
	connect(this, &GameWidget::GestureMove, this, &GameWidget::onGestureMove);
	//����ʱ���źźͻ�����µĲ�
	//connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
	connect(&timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
	//��ʼ��board����
	memset(board, 0, sizeof(board));
	//��ʼ������
	score = 0;
	//�����ĸ���Ϊ 2
	digitCount = 2;
	//���Ŷ���Ч����
	isAnimating = false;
	//��ʼ����������
	init2Block();
}

GameWidget::~GameWidget()
{
}

void GameWidget::init2Block()
{
	//���λ���ұ�֤����λ�ò���ͬ
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
	//����������
	startPos = event->pos();
}

void GameWidget::mouseReleaseEvent(QMouseEvent *event)
{
	//����ڲ��Ŷ���Ч����ֱ���˳���ֹ�ظ����������¼�
	if (isAnimating)
		return;
	//�����յ����������������XY���������
	float dX = (float)(event->pos().x() - startPos.x());
	float dY = (float)(event->pos().y() - startPos.y());

	//ȷ�����Ƶķ���
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
	//����һ��QPainter����
	QPainter painter(this);

	//���÷���ݻ�ͼ
	painter.setRenderHint(QPainter::Antialiasing);

	//
	if (isAnimating)
	{
		if (drawAnimation(painter))
		{
			isAnimating = false;
			timer.stop();
			//�������
			animationList.clear();
			if (digitCount == 16)
			{
				//����Ƿ���Ϸ����
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

	// ����һ����ˢ ��ɫΪR G B�����ֱ�Ϊ141 121 81����ɫ
	QBrush brush(QColor::fromRgb(141, 121, 81));
	// ʹpainterӦ�������ˢ
	painter.setBrush(brush);

	// ���û���Ϊ�ձ� Ŀ����ʹ���Ƶ�ͼ��û�����
	painter.setPen(Qt::NoPen);

	// ����һ������
	painter.drawRoundedRect(QRectF(2 * ratioW, 2 * ratioH, width() - 4 * ratioW, height() - 4 * ratioH), rX, rY);

	/* ����һ������
	 * ��������ΪConsolas
	 * ��������Ϊ����
	 * �����СΪ40����
	 * */
	QFont font;
	font.setFamily("Consolas");
	font.setBold(true);
	font.setPixelSize(40 * ratioH);
	// ʹpainterӦ���������
	painter.setFont(font);

	// ѭ��������Ϸ���
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			// ���������������
			if (board[i][j])
			{
				// ���û�ˢ��ɫΪ�����Ӧ����ɫ
				brush.setColor(ditiBkg[getBitCount(board[i][j])]);
				// Ӧ�������ˢ
				painter.setBrush(brush);
				// ����һ��С����
				painter.drawRoundedRect(QRectF(7 * ratioW + (w + 5 * ratioW) * j, 7 * ratioH + (h + 5 * ratioH) * i, w, h), rX, rY);
				// ���û���Ϊ��ɫ����
				painter.setPen(QColor::fromRgb(0, 0, 0));
				// ��������
				painter.drawText(QRectF(7 * ratioW + (w + 5 * ratioW) * j, 7 * ratioH + (h + 5 * ratioH) * i, w, h), Qt::AlignCenter,
					QString::number(board[i][j]));
				// ���û���Ϊ�ձ�
				painter.setPen(Qt::NoPen);
			}
			// ���������û������
			else
			{
				// ���û�ˢ��ɫΪ RGB����Ϊ171 165 141����ɫ
				brush.setColor(QColor::fromRgb(171, 165, 141));
				// Ӧ�������ˢ
				painter.setBrush(brush);
				// ����С����
				painter.drawRoundedRect(QRectF(7 * ratioW + (w + 5 * ratioW) * j, 7 * ratioH + (h + 5 * ratioH) * i, w, h), rX, rY);
			}
		}


}

void GameWidget::onGestureMove(GestureDirect direct)
{
	int i, j, k;
	Animation a;
	//��¼�Ƿ��ƶ��������Լ��Ƿ��з����ƶ�
	bool move = false, combine = false;
	//��¼ĳ�������Ƿ�μӹ��ϲ�
	bool isCombine[4][4];
	memset(isCombine, 0, sizeof(isCombine));
	
	//����ͬ����
	switch (direct)
	{
		//����
	case LEFT:
		//ѭ��ÿһ��
		for (int i = 0; i < 4; i++)
		{
			//��ʼ��j��kΪ0
			//�ҵ���Ϊ 0 �ĵ㣬j��k��ʾͬһ�е��н���λ��
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

				//��¼������Ϣ
				a.type = MOVE;
				a.startPos = QPointF(7 * ratioW + (w + 5 * ratioW) * j, 7 * ratioH + (h + 5 * ratioH) * i);
				a.endPos = QPointF(7 * ratioW + (w + 5 * ratioW) * k, 7 * ratioH + (h + 5 * ratioH) * i);
				a.digit1 = a.digit2 = board[i][k];
				a.direct = LEFT;

				//����ϲ����Ǯһ��������ͬ
				if (k > 0 && board[i][k] == board[i][k - 1] && !isCombine[i][k - 1])
				{
					//ǰһ�е�����*2����������Ϊ0���õ�ϲ�����Ϊ1����������,���ֿ��һ
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
				//��ӵ���������
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

	//����û������
	if ((move || combine) && digitCount != 16)
	{
		//���������������
		i = rand() % 4;
		j = rand() % 4;
		while (board[i][j]  != 0)
		{
			i = rand() % 4, j = rand() % 4;
		}

		//��������2����4
		board[i][j] = (rand() % 2 + 1) * 2;

		//��¼������Ϣ
		a.type = APPEARANCE;
		a.startPos = a.endPos = QPointF(7 * ratioW + (w + 5 * ratioW) * j, 7 * ratioH + (h + 5 * ratioH) * i);
		a.startPos += QPointF(w / 2, h / 2);
		a.digit1 = board[i][j];
		animationList.append(a);
		digitCount++;
	}

	//��ʼ���ƶ���Ч��
	isAnimating = true;
	//������ʱ��
	timer.start(10);
}

bool GameWidget::drawAnimation(QPainter& painter)
{
	//�����б������
	QList<Animation>::iterator it;

	//����
	QFont font;
	font.setFamily("Consolas");
	font.setBold(true);
	font.setPixelSize(40 * ratioH);
	painter.setFont(font);

	//��ʾ�����Ƿ񲥷����
	bool ok = true;

	//���컭ˢ ��ɫΪRGBΪ141 121 81
	QBrush brush(QColor::fromRgb(141, 121, 81));
	// ʹpainterӦ�������ˢ
	painter.setBrush(brush);

	// ���û���Ϊ�ձ� Ŀ����ʹ���Ƶ�ͼ��û�����
	painter.setPen(Qt::NoPen);

	// ����һ������
	painter.drawRoundedRect(QRectF(2 * ratioW, 2 * ratioH, width() - 4 * ratioW, height() - 4 * ratioH), rX, rY);

	// ���û�ˢ��ɫΪ RGB����Ϊ171 165 141����ɫ
	brush.setColor(QColor::fromRgb(171, 165, 141));
	// Ӧ�������ˢ
	painter.setBrush(brush);

	//ѭ��������Ϸ���
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			painter.drawRoundedRect(QRectF(7 * ratioW + (w + 5 * ratioW)*j, 7 * ratioH + (h + 5 * ratioH)*i, w, h), rX, rY);
		}
	}

	//ѭ������ÿ������Ķ���
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
	//����ÿ��С���ӵĿ�Ⱥ͸߶�,-4��Ϊ��ʼ��
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
	// ��ʼ����ر��� ͬ���캯��
	score = 0;
	digitCount = 2;
	memset(board, 0, sizeof(board));
	init2Block();
	emit ScoreIncreasing(score);
	update();
}

bool GameWidget::checkGameOver()
{
	// ѭ������Ƿ������ڵ���ͬ����
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
	// ѭ������Ƿ�ĳ�����������Ϊ2048
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (board[i][j] == 2048)
				return true;
	return false;
}

int GameWidget::getBitCount(int n)
{
	// ѭ����ȡ���ֶ�����λ��
	int c = 0;
	while (n >>= 1)
		c++;
	// ����λ��-1
	return c - 1;
}

bool GameWidget::playAnimation(Animation& a, QPainter& painter)
{
	bool rtn = false;
	QBrush brush(Qt::SolidPattern);

	//�ƶ�����λ��
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

		//����ƶ����յ�
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
		//������ֵĶ���Ч��
		// ������ֵĶ���Ч��
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