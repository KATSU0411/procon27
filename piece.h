#pragma once

	


extern double ddef;
extern int ldef;

struct Board {
	CvPoint2D64f coord;
	double rad;
	double line;
	double vector;//�ӂ̃x�N�g���Ƃ��Ă̊p�x

	struct Board *next = nullptr;
	struct Board *prev = nullptr;
	int p = 0;
};



class CPiece
{
private:
	
	Board *piece = NULL;
	Board *first = piece;//�ŏ��̒��_�ւ̃|�C���^
	Board *last = piece;
	void add_copy(Board*);
public:
	CPiece();
	~CPiece();

	class CPiece *next = NULL;
	class Cpiece *prev = NULL;

	int used = 0;
	int total = 0;//����
	int p = 0;//�]���l

	double area;
	void add(CvPoint2D64f);//���_�̒ǉ�
	void del(int);//���_�̍폜//int num���폜
	void insert(Board*,int);//int num�́��い�ɑ}��

	void search();//�p�x�A�ӂ̒����̓��o//���łɔ���
	void reverse();//����������

	Board pick(int);//���o��

	void p_plus(int);
	void rote(double,int);//��]
	void shift(CvPoint2D64f);//���s�ړ�
	int judge(Board*,int);//��������

	void copy(Board*,int);
	void draw(IplImage*, int num = 0);
	void clear();//������
	void point_rote(int);//0 ���v���  1 �����v����;
	void change(int, int, double);//0 vector
	void rev_change(int, int, double);


};
