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
	QPushButton* restartBtn;					//���¿�ʼ��ť
	QLabel* scoreLabel;							//"����"��ǩ
	QLabel* highScoreLabel;						//��߷ֱ�ǩ
	GameWidget* gameWidget;						//��Ϸ����
	qreal ratioW, ratioH;						//��Ⱥ͸߶ȵ����ű����������Ǵ��ڲ����������ڵĳߴ�ı�
												//�ı�λ�úͳߴ�
	int highScore;								//��߷�

protected:
	void resizeEvent(QResizeEvent*);			//���ڸı�ߴ��¼�

public slots:
	void onScoreIncreasing(int);				//�������Ӳۺ���
	void onGameOver();							//������Ϸ��ʤ�źŵĲۺ���
	void onWin();								//��Ϸ��ʤ�źŲۺ���
};
