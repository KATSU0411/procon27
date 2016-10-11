#include<opencv\cv.h>
#include<opencv\highgui.h>
//#include<opencv\ctype.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include"put.h"
#include"piece.h"
#define WHITE CV_RGB(0,0,0)
#define URA CV_RGB(255,0,0)


struct set {
	int f_courn;
	int p_courn;//評価値と設置する角と設置するピースの角
	int p_num;
	int r_val;//radvalue_時計回り
	int l_val;//line_value
	int rev_r_val;//rad_value_反時計周り
	int rev_l_val;//line_value
	int just=0;
	//int ljust = 0;
};



CvMemStorage *storage = cvCreateMemStorage(0);


/*

void piece_com()///ピースの組み合わせで360度になるやつを探す（とりあえずは2つの組み合わせ,唯一の組み合わせのみ
{
	int i, j, k, l;
	int count;
	int p_num,c_num;
	for (i = 0;i < piece_allnum;i++) {
		count = 0;
		for (j = 0;j < piece[i].count;j++) {
			for (k = 0;k < piece_allnum;k++) {
				if (i != k) {
					for (l = 0;l < piece[k].count;l++) {
						if (fabs(piece[i].rad[j] + piece[k].rad[l] - M_PI) < ddef) {
							count++;
							p_num = k;
							c_num = l;
						}
					}
				}
			}
		}
	}

}


*/

int comp( const void *c1, const void *c2 )
{
  set test1 = *(set *)c1;
  set test2 = *(set *)c2;

  int tmp1=0;   /* value を基準とする */
  int tmp2=0;
	//tmp1 = test1.f_courn;
	//tmp2 = test2.f_courn;
  if (!(tmp2 - tmp1)) {
	  tmp1 = test1.just;
	  tmp2 = test2.just;
	  if (!(tmp2 - tmp1)) {
		  tmp1 = test1.l_val + test1.rev_l_val;
		  tmp2 = test2.l_val + test2.rev_l_val;
		  if (!(tmp2 - tmp1)) {
			  tmp1 = test1.r_val + test1.rev_r_val;
			  tmp2 = test2.r_val + test2.rev_r_val;
			  if (!(tmp2 - tmp1)) {
				  tmp1 = cpiece[test1.p_num].area;
				  tmp2 = cpiece[test2.p_num].area;
			  }
		  }
	  }
  }
  return tmp2 - tmp1;
}


int put(const int use_piece, IplImage *src_img, const int num,const int range)
{
	if (use_piece == piece_allnum) {
		cvShowImage("Contours", src_img);
		cvWaitKey(1);
		return 1;
	}
	int k, i, l, j, n, m;
	const int next_use_piece = use_piece + 1;
	int put_piece = 0;//置いたピース数
	int possible;//設置可能かどうか
	//int obt[2] = { 0 }, line[2];//鈍角判定 [0]角より左側 [1]角より右側
	//double obt[2] = { 0 }, line[2];//鈍角判定 [0]角より左側 [1]角より右側
	CvPoint2D32f sum, ave;
	char s[5];

	CvMemStorage *ch_storage = cvCreateChildMemStorage(storage);
	CvMemStorage *ap_storage = cvCreateChildMemStorage(storage);


	set *set_piece = new set[65535];

	struct {
		double rad;
		double line;
		CvPoint2D64f p;
	}def;

	if (use_piece > use_max) {

		cframe[use_piece].draw(src_img);
		use_max = use_piece;
		printf("%d\n", use_piece);
	}
	//cframe[use_piece].draw(src_img);

	if (cvWaitKey(1) == 27) {
		cvWaitKey(0);
	}


	//	cvShowImage("Contours", src_img);
	//cvWaitKey(0);
	IplImage *src_def;//= cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
	//int posit[2];////////////////////////////ピースの角と枠の角との位置関係（伝われ）、-1なら枠側(?)、0なら一致、+1ならより枠の内側
				//これも誤差を考慮するものとする
	int count = 0;
	int p_count[126] = { 0 };
	/*
	if (use_piece) {
		return 0;
	}
	*/
	//cvCopyImage(src_img,src_def);
	src_def = cvCloneImage(src_img);
	int piece_num, courn_num;
	CvSeq *approx;


	int put_courn = 0;
	struct {
		int p_num;
		int c_num;//corner_num 
	} piece_put[1600];


	CvPoint points;

	//contour_bef = cvCloneSeq(contour_frame[use_piece], ch_storage);
	//while (1) {


//////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////



	////////////////////////////////////////////////評価値探索///////////////////////////////////////////////////

	int flag = 0;
	int pnum = 0, fnum = 0;
	int rval,lval;
	int rev_r, rev_l;
	int just;

	count = 0;

	//for (i = 0;i < cframe[use_piece].total;i++) {
	for (i = 0;i < range ;i++) {
		for (j = 0;j < piece_allnum * 2;j++) {
			if (!cpiece[j].used) {
				for (k = 0;k < cpiece[j].total;k++) {
					if ((cframe[use_piece].pick(i).rad - cpiece[j].pick(k).rad) > -ddef) {
						just = 0;
						def.p.x = cpiece[j].pick(k).coord.x - cframe[use_piece].pick(i).coord.x;
						def.p.y = cpiece[j].pick(k).coord.y - cframe[use_piece].pick(i).coord.y;

						cpiece[j].shift(def.p);

						//cpiece[j].draw(src_img);
						rval = 0;
						lval = 0;
						rev_r = 0;
						rev_l = 0;

						if ((fabs(cframe[use_piece].pick(i).rad - cpiece[j].pick(k).rad)) < ddef) {///////////////一致する箇所が見つかればそこから反時計回りに探索

							def.rad = cframe[use_piece].pick(i-1).vector - cpiece[j].pick(k-1).vector;
							just = 1;
						}
						else {
							def.rad = cframe[use_piece].pick(i).vector - cpiece[j].pick(k).vector;


						}

						if (((cframe[use_piece].pick(i).rad - cpiece[j].pick(k).rad - min.rad) > -ddef) || just) {

							//cpiece[piece_num].draw(src_img);

							//cpiece[piece_num].draw(src_img);

							//cpiece[j].draw(src_img);
							cpiece[j].rote(def.rad, k);
							if (just) {
								cpiece[j].change(0, k, cframe[use_piece].pick(i).vector);
							}

							//cpiece[j].draw(src_img);

							///////////////////////////////////判定/////////////////


							flag = 1;
							if ((fabs(cframe[use_piece].pick(i).line - cpiece[j].pick(k).line)) < ldef) {
								lval++;
								cpiece[j].change(1, k, cframe[use_piece].pick(i).line);
							}
							else {
								flag = 0;
							}


							for (n = 1;n < cpiece[j].total && flag;n++) {
								pnum = (k + n);
								fnum = (i + n);
								/*
								if ((fabs(cframe[use_piece].pick(fnum - 1).rad - cpiece[j].pick(num - 1).rad)) < ddef) {///////////////一致する箇所が見つかればそこから反時計回りに探索
									if ((fabs(cframe[use_piece].pick(fnum).rad - cpiece[j].pick(num).rad)) < ddef) {///////////////一致する箇所が見つかればそこから反時計回りに探索
										val += 10;
									}
								}
								*/
								if ((fabs(cframe[use_piece].pick(fnum).rad - cpiece[j].pick(pnum).rad)) < ddef) {
									rval++;
									p_count[fnum]++;
									cpiece[j].change(0, pnum, cframe[use_piece].pick(fnum).vector);
									if ((fabs(cframe[use_piece].pick(fnum).line - cpiece[j].pick(pnum).line)) < ldef) {
										cpiece[j].change(1, pnum, cframe[use_piece].pick(fnum).line);
										lval++;
									}
									else {
										flag = 0;
									}

								}
								else {
									flag = 0;
								}


							}






							flag = just;

							for (n = 1;n < cpiece[j].total && flag;n++) {
								pnum = (k - n);
								fnum = (i - n);
								if ((fabs(cframe[use_piece].pick(fnum).line - cpiece[j].pick(pnum).line)) < ldef) {
									rev_l++;
									cpiece[j].rev_change(1, pnum, cframe[use_piece].pick(fnum).line);
									if ((fabs(cframe[use_piece].pick(fnum).rad - cpiece[j].pick(pnum).rad)) < ddef) {
										p_count[fnum+cframe[use_piece].total]++;
										cpiece[j].rev_change(0, pnum, cframe[use_piece].pick(fnum).vector);
										rev_r++;
									}
									else {
										flag = 0;
									}

								}
								else {
									flag = 0;
								}


							}




							/*

							if (rev_l > 0) {
								cpiece[j].draw(src_img);
								printf("%d %d\n", rev_l,rev_r);
								cvWaitKey(0);
							}
							*/
							
							//k = num - 1;

							if (!cpiece[j].judge(&cframe[use_piece].pick(0), cframe[use_piece].total)) {
									p_count[i]++;
								//if (!i) {
									set_piece[count].f_courn = i;
									set_piece[count].p_courn = k;
									set_piece[count].p_num = j;
									set_piece[count].r_val = rval;
									set_piece[count].l_val = lval;
									set_piece[count].just = just;
									set_piece[count].rev_r_val = rev_r;
									set_piece[count].rev_l_val = rev_l;

									count++;
									//printf("OK%d\n", count);
								//}
							}
						}
						cpiece[j].copy(&cpiece_clear[j].pick(0),cpiece_clear[j].total);

					}
				}
			}
		}
		/*
		if (!p_count[i]) {
			//cframe[use_piece].draw(src_img, 0);
			//cframe[use_piece].draw(src_img, i);
			cvReleaseMemStorage(&ch_storage);
			cvReleaseMemStorage(&ap_storage);
			cvReleaseImage(&src_def);
			delete set_piece;
			return 0;
		}
		*/
	}


	//////////////////////評価値の高い順にソートする///////////////////////////////////////////////////////////////


	qsort((void *)set_piece, count, sizeof(struct set), comp);







	int draw_num;
	for (i = 0;i < count;i++) {


		flag = 0;

		possible = 1;
		piece_num = set_piece[i].p_num;
		courn_num = set_piece[i].p_courn;
		put_courn = set_piece[i].f_courn;
		rval = set_piece[i].r_val;
		lval = set_piece[i].l_val;
		just = set_piece[i].just;
		rev_l = set_piece[i].rev_l_val;




		def.p.x = cpiece[piece_num].pick(courn_num).coord.x - cframe[use_piece].pick(put_courn).coord.x;
		def.p.y = cpiece[piece_num].pick(courn_num).coord.y - cframe[use_piece].pick(put_courn).coord.y;
		if (just) {
			def.rad = cframe[use_piece].pick(put_courn-1).vector - cpiece[piece_num].pick(courn_num-1).vector;
		}
		else {
			def.rad = cframe[use_piece].pick(put_courn).vector - cpiece[piece_num].pick(courn_num).vector;
		}

		//cpiece[piece_num].draw(src_img);
		cpiece[piece_num].shift(def.p);

		//cpiece[piece_num].draw(src_img);

		cpiece[piece_num].rote(def.rad, courn_num);

		//cpiece[piece_num].draw(src_img);

		if (just) {
			cpiece[piece_num].change(0, courn_num, cframe[use_piece].pick(put_courn).vector);
		}

		//cpiece[piece_num].draw(src_img);

		for (j = 0;j<rval;j++) {

			cpiece[piece_num].change(1, courn_num + j, cframe[use_piece].pick(put_courn + j).line);
			cpiece[piece_num].change(0, courn_num + j + 1, cframe[use_piece].pick(put_courn + j + 1).vector);
		}
		if (rval < lval) {
			cpiece[piece_num].change(1, courn_num + j, cframe[use_piece].pick(put_courn + j).line);
		}

		for (j = 1;j <= rev_r;j++) {
			cpiece[piece_num].rev_change(1, courn_num - j, cframe[use_piece].pick(put_courn - j).line);
			cpiece[piece_num].rev_change(0, courn_num - j, cframe[use_piece].pick(put_courn - j).vector);

		}
		if (rev_r < rev_l) {
			cpiece[piece_num].rev_change(1, courn_num - j, cframe[use_piece].pick(put_courn - j).line);
		}


		//cpiece[piece_num].draw(src_img);







		//cvLineを使用しポリゴン周りを描画
		//if (possible) {
			//////////////設置
		cpiece[piece_num].used = 1;
		draw_num = piece_num%piece_allnum;



		sum.x = 0;
		sum.y = 0;
		for (j = 0;j < cpiece[piece_num].total;j++) {
			sum.x += points.x = (int)(cpiece[piece_num].pick(j).coord.x + 0.5);
			sum.y += points.y = (int)(cpiece[piece_num].pick(j).coord.y + 0.5);
			polyp[0][j] = points;
		}
		ave.x = sum.x / j;
		ave.y = sum.y / j;


		cvFillPoly(src_img, polyp, &cpiece[draw_num].total, 1, CV_RGB((draw_num * 13) % 200 + 50, (draw_num * 19) % 200 + 50, (draw_num * 23) % 200 + 50));//見やすいようにHSVで出したいところ
		if (piece_num < piece_allnum) {
			cpiece[piece_num + piece_allnum].used = 1;
			sprintf(s, "%d", draw_num);
			cvPutText(src_img, s, cvPointFrom32f(ave), &font, WHITE);
		}
		else {
			sprintf(s, "%d", draw_num);
			cpiece[piece_num - piece_allnum].used = 1;
			cvPutText(src_img, s, cvPointFrom32f(ave), &font, URA);
		}
		//cvFillPoly(src_img, polyp, &piece[piece_num].count, 1, CV_RGB(255, 255, 255));//見やすいようにHSVで出したいところ
		//cvFillPoly(src_img, polyp, &cpiece[piece_num].total, 1, CV_RGB((piece_num * 13) % 30 + 220, (piece_num * 19) % 30 + 220, (piece_num * 17) % 30 + 220));//見やすいようにHSVで出したいところ
		//cvFillPoly(src_img, polyp, &piece[piece_num].count, 1, CV_RGB((piece_num * 13) % 120, (piece_num * 19) % 120, (piece_num * 17) % 120));//見やすいようにHSVで出したいところ
		//cvFillConvexPoly(src_img, piece[piece_num].coord, piece[piece_num].count, CV_RGB(255, 255, 255));
		//cvFillConvexPoly(src_img, piece[piece_num].coord, piece[piece_num].count, CV_RGB((piece_num * 13) % 30 + 220, (piece_num * 19) % 30 + 220, (piece_num * 17) % 30 + 220));
		//cvFillConvexPoly(src_img, piece[piece_num].coord, c, CV_RGB((piece_num * 13) % 120, (piece_num * 19) % 120, (piece_num * 17) % 120));
		//描画時少々周りも描画するとよい？


	//src_img = cpiece[piece_num].draw(src_img);
		/*
	cvShowImage("Contours", src_img);
	cvWaitKey(0);
	*/












	//cframe[use_piece].draw(src_img);
		cframe[next_use_piece].copy(&cframe[use_piece].pick(0), cframe[use_piece].total);



		//cframe[next_use_piece].draw(src_img);


		//printf("l = %d  r = %d j = %d rl = %d rr = %d\n", lval, rval,just,rev_l,rev_r);

		//cframe[next_use_piece].draw(src_img);
		if (just) {
			for (j = 0;j < rev_l;j++) {
				put_courn--;
				cframe[next_use_piece].del(put_courn);
			}
		}

		if (put_courn < 0) {
			put_courn = 0;
		}
		/*
		//cframe[next_use_piece].draw(src_img);
		//if ((fabs(cframe[use_piece].pick(put_courn + rval).line - cpiece[piece_num].pick(courn_num + rval).line)) < ldef) {
		if (rval < lval) {
			cframe[next_use_piece].del(put_courn + rval);
		}
		//cframe[next_use_piece].draw(src_img);
		*/
		/*
		if (rval == 1) {

			cframe[next_use_piece].draw(src_img);
			cvWaitKey(0);
		}
		*/
		/*
		for (j = 0;j < rval;j++) {
			cframe[next_use_piece].del(put_courn);
		}




		//cframe[next_use_piece].draw(src_img);
		for (j = just;j <= cpiece[piece_num].total - 1 - rval;j++) {
			cframe[next_use_piece].insert(&cpiece[piece_num].pick(courn_num - j), put_courn - 1);
		}
		*/



		cframe[next_use_piece].del(put_courn);
		for (j = 0;j < lval;j++) {
			cframe[next_use_piece].del(put_courn);
		}

		if (put_courn > cframe[next_use_piece].total) {
			put_courn = 0;
		}


		//cframe[next_use_piece].draw(src_img);


		for (j=rval+1;j <= cpiece[piece_num].total-just-rev_l;j++) {
			cframe[next_use_piece].insert(&cpiece[piece_num].pick(courn_num + j), put_courn);
			//cframe[next_use_piece].draw(src_img);
		}
		//cframe[next_use_piece].draw(src_img);
		//cframe[next_use_piece].point_rote(1);


		//cframe[next_use_piece].draw(src_img);

		cframe[next_use_piece].search();







		//					printf("piece_num = %d\n", piece_num);
							//cvClearSeq(contour_frame[use_piece]);

		if (put(next_use_piece, src_img, num, j)) {
			cvReleaseMemStorage(&ch_storage);
			cvReleaseMemStorage(&ap_storage);
			cvReleaseImage(&src_def);
			delete set_piece;
			return 1;
		}



		cpiece[piece_num].copy(&cpiece_clear[piece_num].pick(0),cpiece[piece_num].total);
		cvCopy(src_def, src_img);
		cpiece[piece_num].used = 0;
		if (piece_num < piece_allnum) {
			cpiece[piece_num + piece_allnum].used = 0;
		}
		else {
			cpiece[piece_num - piece_allnum].used = 0;
		}
		cframe[next_use_piece].clear();
		//cframe[use_piece].draw(src_img);



		//cvFillPoly(src_img, polyp, &piece[piece_num].count, 1, CV_RGB(0, 0, 0));///（前回設置を取り除く）
		//cvShowImage("Contours", src_img);
		//cvWaitKey(0);



	//	piece[piece_num].used = 0;
		//piece[piece_num] = piece_clear[piece_num];
	}







	cvReleaseMemStorage(&ch_storage);
	cvReleaseMemStorage(&ap_storage);
	cvReleaseImage(&src_def);
	delete set_piece;
	return 0;

	//contour_bef = contour_frame[use_piece]->h_next;

//}
}
