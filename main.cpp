#include<opencv\cv.h>
#include<opencv\highgui.h>
//#include<opencv\ctype.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include <omp.h>
#include"piece.h"
//#include"frame.h"
#include"put.h"

#define PAT_ROW    (7)          /* パターンの行数 */
#define PAT_COL    (10)         /* パターンの列数 */
#define PAT_SIZE   (PAT_ROW*PAT_COL)


#define CAMERA 0 //撮るかどうか
#define ROTE 0//初期値変更(0),時計周り






int piece_allnum;//ピースの数

double ddef = 0.01745 * 3;
int ldef= 5; ///////////////////////////////////////////////////////////////////////////////////////////角度誤差、長さ誤差
struct mam max, min;
class CPiece *cpiece;
class CPiece *cframe;
class CPiece *cpiece_clear;
CvPoint **polyp;
int use_max = 0;
CvFont font;



IplImage* rm_noise(IplImage *img) {

	const int Iterations = 0;
	int i;



	for (i = 0;i < Iterations;i++) {
		cvErode(img, img, NULL, 1);
	}
	for (i = 0;i < Iterations*2;i++) {
		cvDilate(img, img, NULL, 1);
	}
	for (i = 0;i < Iterations;i++) {
		cvErode(img, img, NULL, 1);
	}


	return img;
}







int main()
{

	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX,0.5,0.5,0,2);
	IplImage *src_img = 0;
	IplImage *tmp_img;
	IplImage *out_img;
	IplImage *p_img = 0;
	IplImage *cap_img;
	IplImage *f_img;
	CvTreeNodeIterator it;
	CvFileStorage *fs;
	int key=0;
	const int CV_WAITKEY_CURSORKEY_TOP = 2490368;
	const int CV_WAITKEY_CURSORKEY_BOTTOM = 2621440;
	const int CV_WAITKEY_CURSORKEY_RIGHT = 2555904;
	const int CV_WAITKEY_CURSORKEY_LEFT = 2424832;
	int found,corner_count;
	polyp = (CvPoint **)cvAlloc(sizeof(CvPoint *) * 1);
	polyp[0] = (CvPoint *)cvAlloc(sizeof(CvPoint) * 32);
	CvSize pattern_size = cvSize(PAT_COL, PAT_ROW);
	CvPoint2D32f homo_bef[4],homo_aft[4];//左上から
	CvPoint2D32f corner[PAT_SIZE];
	CvMat *map_matrix;

	float m[6];
	CvMat M;




	int appro_val_frame = 5;
	int appro_val = 5;

	CvMemStorage *storage = cvCreateMemStorage(0);
	CvMemStorage *ch_storage;
	CvSeq *approx;
	CvSeq *approx_frame;
	CvPoint2D64f ver;
	CvPoint *point, *tmp;
	CvPoint pt1, pt0;

	CvSeq *contour_frame;
	CvSeq *contours = 0;
	CvSeq *contour;
	int i, j, l, k;
	double area;

	CvCapture *capture;
	IplImage *img_gray;
	if (CAMERA) {
		capture = cvCaptureFromCAM(1);//カメラ番号指定
		cap_img = cvQueryFrame(capture);
		if (cap_img == NULL) {
			cvReleaseCapture(&capture);
			return 0;
		}
	}

	//cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 960);
	//cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 1280);


	
	cap_img = cvLoadImage("img\\frame.jpg", CV_LOAD_IMAGE_COLOR);//////////////////////////////
	//cap_img = cvLoadImage("img\\frame.png", CV_LOAD_IMAGE_COLOR);//////////////////////////////
	img_gray = cvCreateImage(cvGetSize(cap_img), IPL_DEPTH_8U, 1);
	tmp_img = cvCreateImage(cvGetSize(cap_img), IPL_DEPTH_8U, 1);
	src_img = cvCloneImage(cap_img);
	p_img = cvCloneImage(cap_img);
	f_img = cvCloneImage(cap_img);

	out_img = cvCloneImage(cap_img);
	cvNamedWindow("capture window", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Gray Schale", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("piece", CV_WINDOW_AUTOSIZE);

	m[0] = (float)(cos(-M_PI / 2));
	m[1] = (float)(-sin(-M_PI / 2));
	m[2] = src_img->width*0.5;
	m[3] = -m[1];
	m[4] = m[0];
	m[5] = src_img->height*0.5;

	cvInitMatHeader(&M, 2, 3, CV_32FC1, m, CV_AUTOSTEP);


	int gray_frame = 126;
	int gray = 126;
	//src_img = cvLoadImage("img\\2.jpg", CV_LOAD_IMAGE_COLOR);////////////////////////////


	////////////////////////////////射影補正/////////////////////////////////////////////////
	//////////////////////////////まだちゃんとした補正が行えていない/////////////////////

	/*
	while (key != 13) {


		src_img = cvQueryFrame(capture);
		

		cvShowImage("capture window", src_img);


		found = cvFindChessboardCorners(src_img, pattern_size, corner, &corner_count); 
		homo_bef[0]=corner[0];
		homo_bef[1]=corner[PAT_COL - 1];
		homo_bef[2]=corner[PAT_SIZE - PAT_COL];
		homo_bef[3]=corner[PAT_SIZE - 1];


		if (fabs(homo_bef[0].x - homo_bef[2].x) < 0.5) {
			if (fabs(homo_bef[0].y - homo_bef[1].y) < 0.5) {
				break;
			}
		}

		cvShowImage("piece", out_img);
		cvWaitKey(1);
		cvCopy(src_img, out_img);
	}

	cvCopy(src_img, out_img);


	homo_bef[0]=corner[0];
	homo_bef[1]=corner[PAT_COL - 1];
	homo_bef[2]=corner[PAT_SIZE - PAT_COL];
	homo_bef[3]=corner[PAT_SIZE - 1];



	homo_aft[0] = homo_bef[0];

	homo_aft[1].x = homo_bef[1].x;
	homo_aft[1].y = homo_aft[0].y;

	homo_aft[2].x = homo_aft[0].x;
	homo_aft[2].y = homo_bef[2].y;

	homo_aft[3].x = homo_aft[1].x;
	homo_aft[3].y = homo_aft[2].y;


	for (i = 0;i < 4;i++) {
		cvCircle(out_img,cvPointFrom32f(homo_bef[i]), 3, CV_RGB(255, 0, 0), -1);
		cvCircle(out_img,cvPointFrom32f(homo_aft[i]), 3, CV_RGB(0, 255, 0), -1);
	}





	map_matrix = cvCreateMat(3, 3, CV_32FC1);


	cvShowImage("piece", out_img);
	cvWaitKey(0);
	cvCopy(src_img, out_img);


	cvGetPerspectiveTransform(homo_bef, homo_aft, map_matrix);


	cvWarpPerspective(src_img, out_img, map_matrix, CV_WARP_FILL_OUTLIERS);



	cvShowImage("piece", out_img);
	cvWaitKey(0);
	cvCopy(src_img, out_img);

	*/














	////////////////////////////枠写真//////////////////////////////////////////
	key = 0;

	cap_img = cvLoadImage("img\\frame.jpg", CV_LOAD_IMAGE_COLOR);//////////////////////////////
	//cap_img = cvLoadImage("img\\frame.png", CV_LOAD_IMAGE_COLOR);//////////////////////////////

	while (key != 13) {
		ch_storage = cvCreateChildMemStorage(storage);

		if (CAMERA) {
			cap_img = cvQueryFrame(capture);
		}
		src_img = rm_noise(cap_img);
		//cvCopy(cap_img, src_img);


		if (src_img == NULL) {
			return 0;
		}




		switch (key) {
		case CV_WAITKEY_CURSORKEY_RIGHT:
			gray_frame++;
			break;

		case CV_WAITKEY_CURSORKEY_LEFT:
			gray_frame--;
			break;

		case CV_WAITKEY_CURSORKEY_TOP:
			appro_val_frame++;
			break;

		case CV_WAITKEY_CURSORKEY_BOTTOM:
			if (!appro_val_frame) appro_val_frame++;
			appro_val_frame--;
			break;

		}

		//cvGetQuadrangleSubPix(src_img, out_img, &M);
		cvCopy(src_img, out_img);

		cvCvtColor(src_img, img_gray, CV_BGR2GRAY);

		cvThreshold(img_gray, tmp_img, gray_frame, 255, CV_THRESH_BINARY);//////////////////////////////////////////////////////////////////////////////////////////////////////////





		cvNot(tmp_img, tmp_img);//黒白の反転


		cvShowImage("Gray Schale", tmp_img);

		cvFindContours(tmp_img, ch_storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_NONE);///////////////////////輪郭の点の出し方



		cvCopy(src_img, out_img);
		if ((contours = contours->v_next) != NULL) {
			contours = contours->v_next;

		}
		


		// (3)各ノード（輪郭）を走査
		if ((contours != NULL)) {
			approx = cvApproxPoly(contours, sizeof(CvContour), NULL, CV_POLY_APPROX_DP, appro_val_frame);////////////////////////////////////////////////////

			tmp = CV_GET_SEQ_ELEM(CvPoint, approx, approx->total - 1);//頂点座標を次々取得
			// (4)輪郭を構成する頂点座標を取得
			for (i = 0; i < approx->total; i++) {
				point = CV_GET_SEQ_ELEM(CvPoint, approx, i);//頂点座標を次々取得
				cvLine(out_img, *tmp, *point, CV_RGB(0, 0, 255), 3);
				cvCircle(out_img, *tmp, 3, CV_RGB(255, 0, 0), -1);//描画
				tmp = point;

			}
			contours = contours->h_next;

		}


		cvShowImage("piece", out_img);








		//cvShowImage("Gray Schale", img_gray);
		cvShowImage("capture window", src_img);


		key = cvWaitKey(100);


		cvReleaseMemStorage(&ch_storage);
	}

	cvSaveImage("img\\frame.jpg", cap_img);

	cvCopy(cap_img, f_img);


	////////////////////ここから枠読み込み////////////////////////////////////////
	
	

	cvFindContours(tmp_img, storage, &contour_frame, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_NONE);///////////////////////輪郭の点の出し方

	//contours = contours->h_next;
	if ((contour_frame = contour_frame->v_next) != NULL) {
		contour_frame = contour_frame->v_next;

	}






	////////////////////////////////////////ピース写真///////////////////////////////////////
	key = 0;
	cap_img = cvLoadImage("img\\piece.jpg", CV_LOAD_IMAGE_COLOR);//////////////////////////////
	//cap_img = cvLoadImage("img\\piece.png", CV_LOAD_IMAGE_COLOR);//////////////////////////////
	//p_img = cvLoadImage("img\\piece.jpg", CV_LOAD_IMAGE_COLOR);//////////////////////////////


	//while (key != 27) {




		while (key != 13 && key != 27) {
			ch_storage = cvCreateChildMemStorage(storage);
			if (CAMERA) {
				cap_img = cvQueryFrame(capture);
			}
			p_img = rm_noise(cap_img);

			//cvCopy(cap_img, p_img);

			if (p_img == NULL) {
				return 0;
			}


			//	cvUndistort2(*img, *img, intrinsic, distortion);

			// (2)歪み補正
			//	cvRemap (*img, *img, mapx, mapy);


			switch (key) {
			case CV_WAITKEY_CURSORKEY_RIGHT:
				gray++;
				break;

			case CV_WAITKEY_CURSORKEY_LEFT:
				gray--;
				break;

			case CV_WAITKEY_CURSORKEY_TOP:
				appro_val++;
				break;

			case CV_WAITKEY_CURSORKEY_BOTTOM:
				if (!appro_val) appro_val++;
				appro_val--;
				break;

			}


			//cvGetQuadrangleSubPix(p_img, out_img, &M);
			cvCopy(p_img, out_img);
			cvCvtColor(p_img, img_gray, CV_BGR2GRAY);

			cvThreshold(img_gray, tmp_img, gray, 255, CV_THRESH_BINARY);//////////////////////////////////////////////////////////////////////////////////////////////////////////
			//cvNot(tmp_img, tmp_img);//黒白の反転


			cvShowImage("Gray Schale", tmp_img);

			cvFindContours(tmp_img, ch_storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_NONE);///////////////////////輪郭の点の出し方
			if (contours != NULL) {
				cvInitTreeNodeIterator(&it, contours, 1);



				cvCopy(p_img, out_img);
				cvInitTreeNodeIterator(&it, contours, 1);
				// (3)各ノード（輪郭）を走査
				while ((contour = (CvSeq *)cvNextTreeNode(&it)) != NULL) {
					approx = cvApproxPoly(contour, sizeof(CvContour), NULL, CV_POLY_APPROX_DP, appro_val);////////////////////////////////////////////////////

					if (approx->total > 2) {
						tmp = CV_GET_SEQ_ELEM(CvPoint, approx, approx->total - 1);//頂点座標を次々取得
						// (4)輪郭を構成する頂点座標を取得
						for (i = 0; i < approx->total; i++) {
							point = CV_GET_SEQ_ELEM(CvPoint, approx, i);//頂点座標を次々取得
							cvLine(out_img, *tmp, *point, CV_RGB(0, 0, 255), 3);
							cvCircle(out_img, *tmp, 3, CV_RGB(255, 0, 0), -1);//描画
							tmp = point;

						}
					}
				}
			}



			cvShowImage("piece", out_img);








			//cvShowImage("Gray Schale", img_gray);
			cvShowImage("capture window", p_img);


			key = cvWaitKey(100);


			cvReleaseMemStorage(&ch_storage);
		}

		cvSaveImage("img\\piece.jpg", cap_img);
		/*

		cvFindContours(tmp_img, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_NONE);///////////////////////輪郭の点の出し方

		cvInitTreeNodeIterator(&it, contours, 1);
		// (3)各ノード（輪郭）を走査
		while ((contour = (CvSeq *)cvNextTreeNode(&it)) != NULL) {
			approx = cvApproxPoly(contour, sizeof(CvContour), NULL, CV_POLY_APPROX_DP, appro_val);////////////////////////////////////////////////////
			if (approx->total > 2) {
				if ((area = cvContourArea(approx)) < min.area) {
					min.area = area;
				}
				// (4)輪郭を構成する頂点座標を取得
				for (i = 0; i < approx->total; i++) {
					point = CV_GET_SEQ_ELEM(CvPoint, approx, i);//頂点座標を次々取得
					//cvLine(dst_img[2], *tmp, *point, CV_RGB(0, 0, 255), 1);
					ver.x = (double)point->x;
					ver.y = (double)point->y;

					cpiece[k].add(ver);
					ver.x *= -1;
					cpiece[k + piece_allnum].add(ver);
				}
				cpiece[k].search();
				cpiece[k + piece_allnum].reverse();
				cpiece[k + piece_allnum].search();


				k++;
				//top(x[j], y[j]);//頂点の探索
			}
		}


	}
	*/


	//cvSaveImage("img\\piece.jpg", cap_img);

	area = tmp_img->height * tmp_img->width;
	min.area = area;

	////////////////////////////////////////////ピース読み込み//////////////////////////////////////////	

	/*
		src_img_gray = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
		cvCvtColor(src_img, src_img_gray, CV_BGR2GRAY);
		tmp_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
		*/

	j = 0;
	//cvThreshold(src_img_gray, tmp_img, 128, 255, CV_THRESH_BINARY);//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//cvNot(tmp_img, tmp_img);//黒白の反転

	cvFindContours(tmp_img, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_NONE);///////////////////////輪郭の点の出し方
	cvInitTreeNodeIterator(&it, contours, 1);
	while ((contour = (CvSeq *)cvNextTreeNode(&it)) != NULL) {
		approx = cvApproxPoly(contour, sizeof(CvContour), NULL, CV_POLY_APPROX_DP, appro_val);////////////////////////////////////////////////////
		if (approx->total > 2) {
			j++;
		}
	}

	approx_frame = cvApproxPoly(contour_frame, sizeof(CvContour), NULL, CV_POLY_APPROX_DP, appro_val_frame);////////////////////////////////////////////////////
	cpiece = new CPiece[j * 2];
	cframe = new CPiece[j];
	cpiece_clear = new CPiece[j * 2];
	piece_allnum = j;
	k = 0;
///////////////////////////////////////////////枠/////////////////////////////


	for (i = 0; i < approx_frame->total; i++) {
		point = CV_GET_SEQ_ELEM(CvPoint, approx_frame, i);//頂点座標を次々取得

		ver.x = (double)point->x;
		ver.y = (double)point->y;				//x,yに値を渡す
//		cvEndWriteStruct(fs);
		cframe[0].add(ver);
	}
	cframe[0].search();
	/////////////////////////////////////////////////////////////////////////////



	cvCopy(p_img, out_img);



	cvCvtColor(p_img, img_gray, CV_BGR2GRAY);
	cvThreshold(img_gray, tmp_img, gray, 255, CV_THRESH_BINARY);//////////////////////////////////////////////////////////////////////////////////////////////////////////

	cvFindContours(tmp_img, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_NONE);///////////////////////輪郭の点の出し方

	cvInitTreeNodeIterator(&it, contours, 1);
	// (3)各ノード（輪郭）を走査
	while ((contour = (CvSeq *)cvNextTreeNode(&it)) != NULL) {
		approx = cvApproxPoly(contour, sizeof(CvContour), NULL, CV_POLY_APPROX_DP, appro_val);////////////////////////////////////////////////////
		if (approx->total > 2) {
			if ((area = cvContourArea(approx)) < min.area) {
				min.area = area;
			}
			cpiece[k].area = area;
			cpiece[k + piece_allnum].area = area;
			// (4)輪郭を構成する頂点座標を取得
			for (i = 0; i < approx->total; i++) {
				point = CV_GET_SEQ_ELEM(CvPoint, approx, i);//頂点座標を次々取得
				//cvLine(dst_img[2], *tmp, *point, CV_RGB(0, 0, 255), 1);
				ver.x = (double)point->x;
				ver.y = (double)point->y;

				cpiece[k].add(ver);
				ver.x *= -1;
				cpiece[k + piece_allnum].add(ver);
			}
			cpiece[k].search();
			cpiece[k + piece_allnum].reverse();
			cpiece[k + piece_allnum].search();



			k++;
			//top(x[j], y[j]);//頂点の探索
		}
	}


	for (i = 0;i < piece_allnum * 2;i++) {
		cpiece_clear[i].copy(&cpiece[i].pick(0),cpiece[i].total);
	}


//	piece[k].x[0] = -1;
	CvPoint2D32f sum, ave;
	char s[3];

	max.rad = min.rad = cpiece[0].pick(0).rad;
	max.line = min.line = cpiece[0].pick(0).line;
	for (i = 0;i < piece_allnum;i++) {
		printf("%d\n", i);
		sum.x = 0;
		sum.y = 0;
		pt1.x = (int)cpiece[i].pick(-1).coord.x ;
		pt1.y = (int)cpiece[i].pick(-1).coord.y ;
		
		for (j = 0;j < cpiece[i].total;j++) {

			sum.x +=(float)cpiece[i].pick(j).coord.x;
			sum.y +=(float)cpiece[i].pick(j).coord.y;
			////角度、長さの最大値最小値をとる
			if (max.rad < cpiece[i].pick(j).rad) {
				max.rad = cpiece[i].pick(j).rad;
			}
			else if (min.rad > cpiece[i].pick(j).rad) {
				min.rad = cpiece[i].pick(j).rad;
			}

			if (max.line < cpiece[i].pick(j).line) {
				max.line = cpiece[i].pick(j).line;
			}
			else if (min.line > cpiece[i].pick(j).line) {
				min.line = cpiece[i].pick(j).line;
			}
			pt0.x = (int)cpiece[i].pick(j).coord.x+0.5 ;
			pt0.y = (int)cpiece[i].pick(j).coord.y+0.5 ;
			polyp[0][j] = pt0;

			cvLine(out_img, pt0, pt1, CV_RGB(0, 255, 0), 2);
			pt1 = pt0;
			printf("deg = %lf :line = %lf  deg = %lf :line = %lf\n", cpiece[i].pick(j).rad * 180 / M_PI, cpiece[i].pick(j).line, cpiece[i + piece_allnum].pick(j).rad * 180 / M_PI, cpiece[i + piece_allnum].pick(j).line);

		}
		cvFillPoly(out_img, polyp, &cpiece[i].total, 1, CV_RGB((i * 13) % 200 + 50, (i * 19) % 200 + 50, (i * 23) % 200 + 50));//見やすいようにHSVで出したいところ
		ave.x = sum.x / j;
		ave.y = sum.y / j;

		//ave.x = 0;
		//ave.y = 0;
		sprintf(s, "%d", i);
		cvPutText(out_img, s, cvPointFrom32f(ave), &font, CV_RGB(255, 255, 255));



		printf("\n");
	}
	printf("Peaces = %d\n", piece_allnum);

	for (i = 0;i < piece_allnum;i++) {
		for (j = 0;j < cpiece[i].total;j++) {
			pt0.x = (int)cpiece[i].pick(j).coord.x;
			pt0.y = (int)cpiece[i].pick(j).coord.y;
			cvCircle(out_img, pt0, 4, CV_RGB(255, 0, 0), -1);//描画
		}
	}



	for (i = 0;i < cframe[0].total;i++) {
		
		printf("deg = %lf :line = %lf\n", cframe[0].pick(i).rad * 180 / M_PI, cframe[0].pick(i).line);
	}


	/*
	for (i = 1;i < cframe[0].total;i++) {

		cframe[i][0].copy(&cframe[0].pick(0), cframe[0].total);
		cframe[i][0].search();
		for (j = 0;j < i;j++) {
			cframe[i][0].point_rote(1);
		}
		for (j = 0;j < piece_allnum*2;j++) {
			cpiece[i][j].copy(&cpiece[j].pick(0), cpiece[j].total);
			cpiece[i][j].search();
		}
	}
	*/

	


	cvDestroyAllWindows();

	cvNamedWindow("piece");

	
	cvShowImage("piece", out_img);
	
	cvNamedWindow("Contours", CV_WINDOW_AUTOSIZE);



	cvCopy(f_img, src_img);
	cvCopy(src_img, f_img);

	for (i = 0;i < ROTE;i++) {
		cframe[0].point_rote(0);
	}
//#pragma omp parallel shared(use_max)
//	{
//#pragma omp for private(f_img)
		//for (i = 0;i < cframe[0].total;i++) {
			f_img = cvCloneImage(src_img);
			put(0, f_img, i);
		//}

//	}

	//put(0, f_img, 6);


	cvWaitKey(0);


	cvDestroyAllWindows();
	cvReleaseImage(&src_img);
		//cvReleaseCapture(&capture);
	cvReleaseImage(&out_img);
	cvReleaseImage(&tmp_img);
	cvReleaseMemStorage(&storage);

	delete[] cpiece;
	delete[] cframe;
	

	return 0;
}