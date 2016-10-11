#include<math.h>
#define _USE_MATH_DEFINES
#pragma warning( disable: 4996 )
#include<opencv\cv.h>
#include<opencv\highgui.h>
#include"piece.h"

CPiece::CPiece()
{

}

CPiece::~CPiece()
{

}


void CPiece::add(CvPoint2D64f point)//末尾に追加
{
	Board *p;
	p = new Board;

	if (piece == NULL) {
		piece = new Board;
		//base = point;
//		point.x = point.y = 0;
		piece->coord = point;
		piece->next = piece->prev = piece;
		first = last = piece;
	}
	else {
//		point.x -= base.x ;
//		point.y -= base.y ;

		p->coord = point;
		p->next = first;
		p->prev = last;
		first->prev = p;
		last->next = p;
		last = p;
	}

	total++;

}

void CPiece::insert(Board *ins, int num)///numのまえにそうにゅう
{

	Board *p = new Board;
	*p = *ins;
	piece = first;
	if (num > 0) {
		for (int i = 0;i < num;i++) {
			piece = piece->next;
		}
	}
	else {
		for (int i = 0;i > num;i--) {
			piece = piece->prev;
		}
	}
	

	p->prev = piece->prev;
	piece->prev->next = p;
	piece->prev = p;
	p->next = piece;;
	if (num == 0) {
		first = p;
	}


	last = first->prev;

	total++;







}


void CPiece::del(int delnum)
{
	piece = first;
	if (total == 1) {
		delete piece;
		piece = NULL;
		total--;
		return;
	}

	if (delnum > 0) {
		for (int i = 0;i < delnum;i++) {
			piece = piece->next;
		}
	}
	else {
		for (int i = 0;i > delnum;i--) {
			piece = piece->prev;
		}
	}


	piece->next->prev = piece->prev;
	piece->prev->next = piece->next;

	
	if (delnum%total == 0) {
		first = last->next;
	}
	/*
	if (delnum+1%total == 0) {
		first = first->prev;
	}
	*/
	last = first->prev;
	delete piece;



	total--;

}


void CPiece::search()
{
	piece = first;
	Board *d;
	for (int i = 0;i < total;i++) {

		double rada, radb, rad;
		double ax, ay, bx, by;
		CvPoint2D64f p[3];
		p[0] = piece->prev->coord;
		p[1] = piece->coord;
		p[2] = piece->next->coord;




		ax = p[0].x - p[1].x;
		ay = -(p[0].y - p[1].y);
		bx = p[2].x - p[1].x;
		by = -(p[2].y - p[1].y);


		//rad = atan2(ay, ax) - atan2(by, bx);
		if ((rada = atan2(ay, ax)) < 0) {
			rada += 2 * M_PI;
		}

		if ((radb = atan2(by, bx)) < 0) {
			radb += 2 * M_PI;
		}

		piece->vector = atan2(-by, bx);

		//rad = adjust_rad(ax, ay, bx, by, rad);

		if ((rad = rada - radb) < 0) {
			rad += 2 * M_PI;
		}
		piece->rad = rad;
		piece->line = sqrt(bx*bx + by*by);

		if (fabs(piece->rad - M_PI) < 0.017*10) {//+-10度
			d = piece->prev->prev;
			del(i);
			i-=2;
			piece = d;
		}



		piece = piece->next;

	}

}


Board CPiece::pick(int num) 
{
	piece = first;
	if (num > 0) {
		for (int i = 0;i < num;i++) {
			piece = piece->next;
		}
	}
	else {
		for (int i = 0;i > num;i--) {
			piece = piece->prev;
		}
	}
	return *piece;
}



void CPiece::reverse(void)
{
	piece = first;
	Board *next;
	
	for (int i = 0;i < total;i++) {
		next = piece->next;
		piece->next = piece->prev;
		piece->prev = next;
		piece = next;
	}


	
	
}





void CPiece::p_plus(int num)
{
	piece = first;
	if (num > 0) {
		for (int i = 0;i < num;i++) {
			piece = piece->next;
		}
	}
	else {
		for (int i = 0;i > num;i--) {
			piece = piece->prev;
		}
	}


	piece->p++;
}


void CPiece::rote(double rad,int num)
{
	piece = first;
	CvPoint2D64f p,d;

	for (int i = 0;i < num;i++) {
		piece = piece->next;
	}
	d = piece->coord;//基準点

	piece = first;

	for (int i = 0;i < total;i++) {
		p.x = piece->coord.x - d.x;
		p.y = piece->coord.y - d.y;

		piece->coord.x = p.x*cos(rad) - p.y*sin(rad) + d.x;
		piece->coord.y = p.x*sin(rad) + p.y*cos(rad) + d.y;



		piece->vector += rad;



		piece = piece->next;

	}


	
}





int CPiece::judge(Board *frame, int ftotal)
{
	piece = first;

	CvPoint2D64f p, f, p_bef, f_bef;
	double ta, tb, tc, td;
	double d,min_d,l_bef;
	double tmp;
	double angle;

	p_bef = piece->prev->coord;
	f_bef = frame->prev->coord;
	l_bef = frame->prev->line;
	

	for (int i = 0;i < total;i++) {
		p = piece->coord;
		min_d = 114514;
		for (int j = 0;j < ftotal;j++) {
			f = frame->coord;

			ta =(int)((p_bef.x - p.x) *(f_bef.y - p_bef.y)	+(p_bef.y - p.y) *(p_bef.x - f_bef.x)	);
			tb =(int)((p_bef.x - p.x) *(f.y - p_bef.y)		+(p_bef.y - p.y) *(p_bef.x - f.x)		);
			tc =(int)((f_bef.x - f.x) *(p_bef.y - f_bef.y)	+(f_bef.y - f.y) *(f_bef.x - p_bef.x)	);
			td =(int)((f_bef.x - f.x) *(p.y - f_bef.y)		+(f_bef.y - f.y) *(f_bef.x - p.x)		);


			


			if (tc*td < 0 && ta*tb < 0) {
				return 1;
			}

			///辺(線分)との距離を出す




			d = ((f.x - f_bef.x)*(p.y - f_bef.y) - (f.y - f_bef.y)*(p.x - f_bef.x));//外積を使い、辺との距離を出す
			tmp = sqrt(pow(f.x - p.x, 2) + pow(f.y - p.y, 2));
			if (fabs(asin(l_bef*tmp)/d) > M_PI / 2){
				d = tmp;
			}
			else {
				tmp = sqrt(pow(f_bef.x - p.x, 2) + pow(f_bef.y - p.y, 2));
				if (fabs(asin(l_bef*tmp) / d) > M_PI / 2) {
					d = tmp;
				}
				else {
					d = d / l_bef;
				}
			}

			if (fabs(d) < fabs(min_d)) {
				min_d = d;
			}



			f_bef = f;
			l_bef = frame->line;
			frame = frame->next;
		}
		if (min_d > ldef) {
			return 1;
		}


		p_bef = p;
		piece = piece->next;
	}


	return 0;

}


void CPiece::copy(Board *frame, int ftotal) 
{
	clear();
	for (int i = 0;i < ftotal;i++) {
		add_copy(frame);
		frame = frame->next;
	}

}

void CPiece::add_copy(Board *point)//末尾に追加
{

	if (piece == NULL) {
		piece = new Board;
		*piece = *point;
		piece->next = piece->prev = piece;
		first = last = piece;
	}
	else {
		Board *p;
		p = new Board;

		*p = *point;
		p->next = first;
		p->prev = last;
		first->prev = p;
		last->next = p;
		last = p;
	}

	total++;

}
void CPiece::shift(CvPoint2D64f d)
{
	piece = first;
	for (int i = 0;i < total;i++) {
		piece->coord.x -= d.x;
		piece->coord.y -= d.y;

		piece = piece->next;
	}


}


void CPiece::draw(IplImage *img,int num)
{
	IplImage *bef_img = cvCloneImage(img);
	
	piece = first;
	while (num < 0) {
		num += total;
	}
	CvPoint pt0, pt1;
	pt0.x = (int)piece->prev->coord.x;
	pt0.y = (int)piece->prev->coord.y;

	for (int i = 0;i < total;i++) {
		pt1.x = (int)piece->coord.x;
		pt1.y = (int)piece->coord.y;

		if (i == num%total) {
			cvLine(img, pt1, pt0, CV_RGB(0, 255, 0), 2);
			cvCircle(img, pt1, 4, CV_RGB(255, 0, 0), -1);
		}
		else {

			cvLine(img, pt1, pt0, CV_RGB(0, 255, 0), 2);
			cvCircle(img, pt1, 4, CV_RGB(0, 0, 255), -1);
		}


		pt0			= pt1;
		piece = piece->next;

	}

	cvNamedWindow("Contours");
	cvShowImage("Contours", img);

	cvCopy(bef_img, img);
	cvReleaseImage(&bef_img);

//	return img;

}




void CPiece::clear()
{

	while(total){
		del(0);
	}
	first = last = NULL;
	total = 0;

}


void CPiece::point_rote(int way)//0 時計回り 1 反時計回り
{
	if (way) {
		first = first->next;
		last = first->prev;
	}
	else {
		first = first->prev;
		last = first->prev;

	}


}


void CPiece::change(int flag, int num, double data)
{
	piece = first;
	double deff;


	if (num > 0) {
		for (int i = 0;i < num;i++) {
			piece = piece->next;
		}
	}
	else {
		for (int i = 0;i > num;i--) {
			piece = piece->prev;
		}
	}



	switch (flag) {
	case 0:
		piece->vector = data;
		break;
	case 1:
		piece->line = data;
		break;
	}

	piece->next->coord.x = piece->coord.x + piece->line*cos(piece->vector);
	piece->next->coord.y = piece->coord.y + piece->line*sin(piece->vector);
	search();


}


void CPiece::rev_change(int flag, int num, double data)
{

	/*
	piece = first;
	double deff;


	if (num > 0) {
		for (int i = 0;i < num;i++) {
			piece = piece->next;
		}
	}
	else {
		for (int i = 0;i > num;i--) {
			piece = piece->prev;
		}
	}



	switch (flag) {
	case 0:
		piece->prev->vector = data;
		piece->prev->coord.x = piece->coord.x - piece->prev->line*cos(piece->prev->vector);
		piece->prev->coord.y = piece->coord.y - piece->prev->line*sin(piece->prev->vector);
		break;
	case 1:
		piece->line = data;
		piece->coord.x = piece->next->coord.x - piece->line*cos(piece->vector);
		piece->coord.y = piece->next->coord.y - piece->line*sin(piece->vector);
		break;
	}

	search();
	*/

}
