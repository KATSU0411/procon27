#pragma once

int put(const int,IplImage*,const int,const int range = 1);

struct mam {
	double rad;
	double line;
	double area;
};
extern int piece_allnum;
extern class CPiece *cpiece;
extern class CPiece *cframe;
extern class CPiece *cpiece_clear;
extern double ddef;
extern int ldef;
extern struct mam min,max;
extern CvPoint **polyp;
extern int use_max;
extern CvFont font;
