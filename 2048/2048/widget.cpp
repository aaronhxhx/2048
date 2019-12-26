#include "widget.h"

Widget::Widget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	highScore = 0;								////初始化最高分
	QFile file("score.j");						//读取存储最高分的文件
	if (file.open(QIODevice::ReadOnly))
	{
		file.read((char*)&highScore, sizeof(highScore));
		file.close();
	}

	//游戏部件
	gameWidget = new GameWidget(this);
	gameWidget->setGeometry(2, 200, 400, 400);
	connect(gameWidget, &GameWidget::ScoreIncreasing, this, &Widget::onScoreIncreasing);
	connect(gameWidget, &GameWidget::GameOver, this, &Widget::onGameOver);
	connect(gameWidget, &GameWidget::Win, this, &Widget::onWin);

	//构造一个字体对象
	QFont font;
	font.setFamily("Consolas");
	font.setBold(true);
	font.setPixelSize(25);

	//构造一个按钮对象
	restartBtn = new QPushButton(QStringLiteral("重新开始"), this);
	restartBtn->setGeometry(100, 120, 200, 50);
	restartBtn->setFont(font);
	restartBtn->setStyleSheet(QString(BTNSTYLESHEET).arg(3).arg(15));
	connect(restartBtn, &QPushButton::clicked, gameWidget, &GameWidget::reStart);

	//构造标签对象
	highScoreLabel = new QLabel(QStringLiteral("最高分：\n%1").arg(highScore), this);
	highScoreLabel->setGeometry(209, 20, 180, 70);
	highScoreLabel->setFont(font);
	highScoreLabel->setAlignment(Qt::AlignCenter);
	highScoreLabel->setStyleSheet(QString(LBLSTYLESHEET).arg(5).arg(20));

	//构造一个分数标签
	scoreLabel = new QLabel(QStringLiteral("分数：\n0"), this);
	scoreLabel->setGeometry(15, 20, 180, 70);
	scoreLabel->setFont(font);
	scoreLabel->setAlignment(Qt::AlignCenter);
	scoreLabel->setStyleSheet(QString(LBLSTYLESHEET).arg(5).arg(20));

	//设置背景颜色
	QPalette palette(this->palette());
	palette.setColor(QPalette::Background, QColor::fromRgb(0, 255, 255));
	this->setPalette(palette);

	//设置图标
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
	//更新分数显示
	scoreLabel->setText(QStringLiteral("分数:\n%1").arg(score));
	//更新最高分
	if (score > highScore)
	{
		highScore = score;
		highScoreLabel->setText(QStringLiteral("最高分:\n%1").arg(highScore));

		//存储最高分
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
	//宽高缩放比例
	ratioW = width() / 404.0;
	ratioH = height() / 606.0;

	//构造一个字体对象
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

	//充值子部件大小和位置
	gameWidget->setGeometry(2 * ratioW, 200 * ratioH, 400 * ratioW, 400 * ratioH);
	restartBtn->setGeometry(100 * ratioW, 120 * ratioH, 200 * ratioW, 50 * ratioH);
	highScoreLabel->setGeometry(209 * ratioW, 20 * ratioH, 180 * ratioW, 70 * ratioH);
	scoreLabel->setGeometry(15 * ratioW, 20 * ratioH, 180 * ratioW, 70 * ratioH);
}


