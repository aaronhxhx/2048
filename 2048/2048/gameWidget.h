#pragma once

#include <QGLWidget>
#include <QMouseEvent>
#include <QEventLoop>
#include <QTimer>
#include <QPainter>
#include <QList>


//���Ʒ���
enum GestureDirect
{
	//����������
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3
};

//���嶯������
enum AnimationType
{
	//�����ƶ�����
	MOVE = 0,
	//������ֶ���
	APPEARANCE  = 1
};

//���嶯���ṹ��
struct Animation
{
	AnimationType type;				//��������
	GestureDirect direct;			//����
	QPointF startPos;				//��ʼ������
	QPointF endPos;					//��ֹ������
	int digit1;						//����
	int digit2;						//�ڶ����� ������ܱ��ϲ�
};


//��Ϸ������ �̳�QWidget
class GameWidget : public QGLWidget
{
	Q_OBJECT

public:
	explicit GameWidget(QWidget* parent = 0);
	~GameWidget();
private:
	int board[4][4];				//��Ϸ��� �洢ÿ�����ӵ���ֵ
	int digitCount;					//����ĸ��� �洢��ǰ����ϵ����ֵĸ���
	int score;						//���� �洢��ǰ�÷�ֵ
	QPoint startPos;				//��ʼ������
	QList<Animation> animationList;	//�洢������Ҫչ�ֵĶ���
	qreal w, h;						//ÿ��С���ӵĿ�Ⱥ͸߶�
	qreal ratioW, ratioH;			//��Ⱥ͸߶ȵ����ű���
	qreal rX, rY;					//СԲ�ǵ�x��y
	//QImage* createImg;			//����ͼ��
	bool isAnimating;				//�Ƿ񲥷Ŷ���Ч��
	QTimer timer;					//��ʱ��

	void init2Block();				//��ʼ����������
	bool checkGameOver();			//�����Ϸ�Ƿ����
	bool checkGameWin();			//�����Ϸ�Ƿ��ʤ
	int getBitCount(int);			// ��ȡһ�����ֵĶ�����λ�� ��Ȼ�����ȡ�Ĳ���ȫ�Ƕ�����λ�� 
									//���Ƕ�Ӧ��ɫ������±���� 2 ��Ӧ 0    8 ��Ӧ 2 
	bool playAnimation(Animation&, QPainter&);	//����һ֡����
	bool drawAnimation(QPainter&);				//���ƶ���Ч��

	void mousePressEvent(QMouseEvent *event)override;		//��������¼�
	void mouseReleaseEvent(QMouseEvent *event)override;		//����ͷ��¼�
	void paintEvent(QPaintEvent*)override;					//���ػ����¼�
	void resizeEvent(QResizeEvent*)override;				//���ش��ڸı��¼�

	//�ź�
signals:
	void GestureMove(GestureDirect);						//�����ƶ��ź�
	void ScoreIncreasing(int);								//���������ź�
	void GameOver();										//��Ϸ�����ź�
	void Win();												//��Ϸ��ʤ�ź�

	//�ۺ���
public slots:
	void onGestureMove(GestureDirect);						//�����ƶ��ۺ���
	void reStart();											//���¿�ʼ�ۺ���

};
