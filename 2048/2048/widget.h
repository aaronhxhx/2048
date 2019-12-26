#pragma once

#include <QtWidgets/QWidget>
#include "ui_widget.h"
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include "gameWidget.h"


#define LBLSTYLESHEET "QLabel {color: orange;background: #FFFFCC;border: %1px solid orange;border-radius: %2px;}"
#define BTNSTYLESHEET "QPushButton {color: red;background: lightgray;border: %1px solid darkgray;border-radius: %2px;} QPushButton:pressed{color: white;background: orange;border: %1px solid darkgray;border-radius: %2px;}"


class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = Q_NULLPTR);
	~Widget();

private:
	Ui::WidgetClass ui;
	QPushButton* restartBtn;					//重新开始按钮
	QLabel* scoreLabel;							//"分数"标签
	QLabel* highScoreLabel;						//最高分标签
	GameWidget* gameWidget;						//游戏部件
	qreal ratioW, ratioH;						//宽度和高度的缩放比例，用来是窗口部件随主窗口的尺寸改变
												//改变位置和尺寸
	int highScore;								//最高分

protected:
	void resizeEvent(QResizeEvent*);			//窗口改变尺寸事件

public slots:
	void onScoreIncreasing(int);				//分数增加槽函数
	void onGameOver();							//处理游戏获胜信号的槽函数
	void onWin();								//游戏获胜信号槽函数
};
