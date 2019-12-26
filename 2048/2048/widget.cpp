#include "widget.h"

Widget::Widget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	highScore = 0;								////��ʼ����߷�
	QFile file("score.j");						//��ȡ�洢��߷ֵ��ļ�
	if (file.open(QIODevice::ReadOnly))
	{
		file.read((char*)&highScore, sizeof(highScore));
		file.close();
	}

	//��Ϸ����
	gameWidget = new GameWidget(this);
	gameWidget->setGeometry(2, 200, 400, 400);
	connect(gameWidget, &GameWidget::ScoreIncreasing, this, &Widget::onScoreIncreasing);
	connect(gameWidget, &GameWidget::GameOver, this, &Widget::onGameOver);
	connect(gameWidget, &GameWidget::Win, this, &Widget::onWin);

	//����һ���������
	QFont font;
	font.setFamily("Consolas");
	font.setBold(true);
	font.setPixelSize(25);

	//����һ����ť����
	restartBtn = new QPushButton(QStringLiteral("���¿�ʼ"), this);
	restartBtn->setGeometry(100, 120, 200, 50);
	restartBtn->setFont(font);
	restartBtn->setStyleSheet(QString(BTNSTYLESHEET).arg(3).arg(15));
	connect(restartBtn, &QPushButton::clicked, gameWidget, &GameWidget::reStart);

	//�����ǩ����
	highScoreLabel = new QLabel(QStringLiteral("��߷֣�\n%1").arg(highScore), this);
	highScoreLabel->setGeometry(209, 20, 180, 70);
	highScoreLabel->setFont(font);
	highScoreLabel->setAlignment(Qt::AlignCenter);
	highScoreLabel->setStyleSheet(QString(LBLSTYLESHEET).arg(5).arg(20));

	//����һ��������ǩ
	scoreLabel = new QLabel(QStringLiteral("������\n0"), this);
	scoreLabel->setGeometry(15, 20, 180, 70);
	scoreLabel->setFont(font);
	scoreLabel->setAlignment(Qt::AlignCenter);
	scoreLabel->setStyleSheet(QString(LBLSTYLESHEET).arg(5).arg(20));

	//���ñ�����ɫ
	QPalette palette(this->palette());
	palette.setColor(QPalette::Background, QColor::fromRgb(0, 255, 255));
	this->setPalette(palette);

	//����ͼ��
	QIcon icon(":/Resources/2048-1.png");
	setWindowIcon(icon);

	setWindowTitle("2048");

}

Widget::~Widget()
{
	delete restartBtn;
	delete scoreLabel;
	delete highScoreLabel;
	delete gameWidget;
}

void Widget::onScoreIncreasing(int score)
{
	//���·�����ʾ
	scoreLabel->setText(QStringLiteral("����:\n%1").arg(score));
	//������߷�
	if (score > highScore)
	{
		highScore = score;
		highScoreLabel->setText(QStringLiteral("��߷�:\n%1").arg(highScore));

		//�洢��߷�
		QFile file("score.j");
		file.open(QIODevice::WriteOnly);
		file.write((char*)&highScore, sizeof(highScore));
		file.close();
	}
}

void Widget::onGameOver()
{
	QMessageBox::information(this, "Game Over", "You lost");
}

void Widget::onWin()
{
	QMessageBox::information(this, "Congratulation", "You Win!");
}

void Widget::resizeEvent(QResizeEvent* event)
{
	//������ű���
	ratioW = width() / 404.0;
	ratioH = height() / 606.0;

	//����һ���������
	QFont font;
	font.setFamily("Consolas");
	font.setBold(true);
	font.setPixelSize(25 * ratioH);
	restartBtn->setFont(font);
	highScoreLabel->setFont(font);
	scoreLabel->setFont(font);
	restartBtn->setStyleSheet(QString(BTNSTYLESHEET).arg(3 * ratioW).arg(15 * ratioW));
	highScoreLabel->setStyleSheet(QString(LBLSTYLESHEET).arg(5 * ratioW).arg(20 * ratioW));
	scoreLabel->setStyleSheet(QString(LBLSTYLESHEET).arg(5 * ratioW).arg(20 * ratioW));

	//��ֵ�Ӳ�����С��λ��
	gameWidget->setGeometry(2 * ratioW, 200 * ratioH, 400 * ratioW, 400 * ratioH);
	restartBtn->setGeometry(100 * ratioW, 120 * ratioH, 200 * ratioW, 50 * ratioH);
	highScoreLabel->setGeometry(209 * ratioW, 20 * ratioH, 180 * ratioW, 70 * ratioH);
	scoreLabel->setGeometry(15 * ratioW, 20 * ratioH, 180 * ratioW, 70 * ratioH);
}


