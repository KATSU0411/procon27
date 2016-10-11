#pragma once

	


extern double ddef;
extern int ldef;

struct Board {
	CvPoint2D64f coord;
	double rad;
	double line;
	double vector;//辺のベクトルとしての角度

	struct Board *next = nullptr;
	struct Board *prev = nullptr;
	int p = 0;
};



class CPiece
{
private:
	
	Board *piece = NULL;
	Board *first = piece;//最初の頂点へのポインタ
	Board *last = piece;
	void add_copy(Board*);
public:
	CPiece();
	~CPiece();

	class CPiece *next = NULL;
	class Cpiece *prev = NULL;

	int used = 0;
	int total = 0;//総数
	int p = 0;//評価値

	double area;
	void add(CvPoint2D64f);//頂点の追加
	void del(int);//頂点の削除//int numを削除
	void insert(Board*,int);//int numの＜後＞に挿入

	void search();//角度、辺の長さの導出//ついでに判定
	void reverse();//裏向き生成

	Board pick(int);//取り出し

	void p_plus(int);
	void rote(double,int);//回転
	void shift(CvPoint2D64f);//平行移動
	int judge(Board*,int);//交差判定

	void copy(Board*,int);
	void draw(IplImage*, int num = 0);
	void clear();//初期化
	void point_rote(int);//0 時計回り  1 反時計周り;
	void change(int, int, double);//0 vector
	void rev_change(int, int, double);


};
