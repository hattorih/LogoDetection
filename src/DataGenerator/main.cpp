#include "opencv2/opencv.hpp"

#include <string>
#include <vector>

#include <windows.h>


const int W = 640;
const int H = 360;


int find(const std::string &dir, std::vector<std::string> &filenames)
{
	WIN32_FIND_DATA fd;

	char temp[_MAX_PATH] = { 0 };
	if (dir.size() > 0 && dir[dir.size() - 1] == '\\')
		sprintf_s(temp, _MAX_PATH, "%s*.*", dir.c_str());
	else
		sprintf_s(temp, _MAX_PATH, "%s\\*.*", dir.c_str());

	// 検索
	HANDLE hFind = FindFirstFile(temp, &fd);

	// フォルダが存在するかどうかを確認
	if (INVALID_HANDLE_VALUE == hFind)
	{
		printf("Can't find %s.\n", dir.c_str());
	}
	else
	{
		do
		{
			// カレント「.」と親「..」は処理をスキップ
			if (0 != strcmp(fd.cFileName, ".") && 0 != strcmp(fd.cFileName, ".."))
			{
				// 末尾に \\ があるかどうかで場合分けする
				char path_name[_MAX_PATH];
				if (dir.size() > 0 && dir[dir.size() - 1] == '\\')
					sprintf_s(path_name, _MAX_PATH, "%s%s", dir.c_str(), fd.cFileName);
				else
					sprintf_s(path_name, _MAX_PATH, "%s\\%s", dir.c_str(), fd.cFileName);

				// ディレクトリなら
				if (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes)
				{
#if 0
					// 再帰処理
					// ただし隠しフォルダ以外に限る
					if (fd.cFileName[0] != '.')
						find(std::string(path_name), filenames);
#endif
				}
				// ファイルなら
				else
				{
					filenames.push_back(path_name);
				}
			}
		}
		while (FindNextFile(hFind, &fd));

		FindClose(hFind);

		// ファイル名順に並び替え
		std::sort(filenames.begin(), filenames.end());

		printf("Loaded file from directory, %d\n", filenames.size());
	}

	return 0;
}


void printCvMat(const CvMat *m)
{
	printf("---\n");

	float m00 = cvmGet(m, 0, 0); float m01 = cvmGet(m, 0, 1); float m02 = cvmGet(m, 0, 2);
	float m10 = cvmGet(m, 1, 0); float m11 = cvmGet(m, 1, 1); float m12 = cvmGet(m, 1, 2);
	float m20 = cvmGet(m, 2, 0); float m21 = cvmGet(m, 2, 1); float m22 = cvmGet(m, 2, 2);

	printf("%7.4f\t%7.4f\t%7.4f\n", m00, m01, m02);
	printf("%7.4f\t%7.4f\t%7.4f\n", m10, m11, m12);
	printf("%7.4f\t%7.4f\t%7.4f\n", m20, m21, m22);
}


int main(int argc, char **argv)
{
	const char *src1 = NULL;
	const char *src2 = NULL;
	const char *dst = NULL;

	for (int i = 1; i < argc - 1; i++)
	{
		if (strcmp(argv[i], "-src1") == 0) src1 = argv[++i];
		if (strcmp(argv[i], "-src2") == 0) src2 = argv[++i];
		if (strcmp(argv[i], "-dst") == 0) dst = argv[++i];
	}

	if (src1 == NULL || src2 == NULL)
	{
		printf("Usage: %s -src1 <src1_dir> -src2 <src2_dir> -dst <dst_dir>\n", argv[0]);
	}
	else
	{
		std::vector<std::string> src1_files;
		find(src1, src1_files);

		std::vector<std::string> src2_files;
		find(src2, src2_files);

		if (src1_files.size() == 0 || src2_files.size() == 0)
		{
		}
		else
		{
			bool halt = false;
			while (!halt)
			{
				const int i = rand() % src1_files.size();
				const int j = rand() % src2_files.size();

				// Load image
				IplImage *image1 = NULL;
				if ((image1 = cvLoadImage(src1_files[i].c_str())) == NULL)
				{
					printf("Error: cannot load image correctly: %s\n", src1_files[i].c_str());
				}
				else
				{
					// Load image
					IplImage *image2 = NULL;
					if ((image2 = cvLoadImage(src2_files[j].c_str())) == NULL)
					{
						printf("Error: cannot load image correctly: %s\n", src2_files[j].c_str());
					}
					else
					{
						static int index = 0;
						printf("%07d\n", index);

						IplImage *image = cvCreateImage(cvSize(W, H), IPL_DEPTH_8U, 3);

						const int w = image->width;
						const int h = image->height;

						const int w2 = w / 2;
						const int h2 = h / 2;

						const float theta_x = 0;
						const float theta_y = 0;
						const float theta_z = index;

						const float tx = 0;
						const float ty = 0;
						const float tz = image1->width;

#if 1
						// A
						CvMat *a = cvCreateMat(4, 4, CV_32F);
						cvmSet(a, 0, 0, w);   cvmSet(a, 0, 1, 0.0); cvmSet(a, 0, 2, w2);  cvmSet(a, 0, 3, 0.0);
						cvmSet(a, 1, 0, 0.0); cvmSet(a, 1, 1, w);   cvmSet(a, 1, 2, h2);  cvmSet(a, 1, 3, 0.0);
						cvmSet(a, 2, 0, 0.0); cvmSet(a, 2, 1, 0.0); cvmSet(a, 2, 2, 1.0); cvmSet(a, 2, 3, 0.0);
						cvmSet(a, 3, 0, 0.0); cvmSet(a, 3, 1, 0.0); cvmSet(a, 3, 2, 0.0); cvmSet(a, 3, 3, 1.0);

						// R
						CvMat *r = cvCreateMat(3, 3, CV_32F);
						cvmSet(r, 0, 0, 1.0); cvmSet(r, 0, 1, 0.0); cvmSet(r, 0, 2, 0.0);
						cvmSet(r, 1, 0, 0.0); cvmSet(r, 1, 1, 1.0); cvmSet(r, 1, 2, 0.0);
						cvmSet(r, 2, 0, 0.0); cvmSet(r, 2, 1, 0.0); cvmSet(r, 2, 2, 1.0);

						const float theta_x_rad = theta_x / 180 * 3.141592f;
						CvMat *rx = cvCreateMat(3, 3, CV_32F);
						cvmSet(rx, 0, 0, cos(theta_x_rad)); cvmSet(rx, 0, 1, -sin(theta_x_rad)); cvmSet(rx, 0, 2, 0.0);
						cvmSet(rx, 1, 0, sin(theta_x_rad)); cvmSet(rx, 1, 1,  cos(theta_x_rad)); cvmSet(rx, 1, 2, 0.0);
						cvmSet(rx, 2, 0, 0.0);              cvmSet(rx, 2, 1, 0.0);               cvmSet(rx, 2, 2, 1.0);
						printCvMat(rx);

						const float theta_y_rad = theta_y / 180 * 3.141592f;
						CvMat *ry = cvCreateMat(3, 3, CV_32F);
						cvmSet(ry, 0, 0, 1.0); cvmSet(ry, 0, 1, 0.0);              cvmSet(ry, 0, 2, 0.0);
						cvmSet(ry, 1, 0, 0.0); cvmSet(ry, 1, 1, cos(theta_y_rad)); cvmSet(ry, 1, 2, -sin(theta_y_rad));
						cvmSet(ry, 2, 0, 0.0); cvmSet(ry, 2, 1, sin(theta_y_rad)); cvmSet(ry, 2, 2,  cos(theta_y_rad));
						printCvMat(ry);

						const float theta_z_rad = theta_z / 180 * 3.141592f;
						CvMat *rz = cvCreateMat(3, 3, CV_32F);
						cvmSet(rz, 0, 0,  cos(theta_z_rad)); cvmSet(rz, 0, 1, 0.0); cvmSet(rz, 0, 2, sin(theta_z_rad));
						cvmSet(rz, 1, 0, 0);                 cvmSet(rz, 1, 1, 1.0); cvmSet(rz, 1, 2, 0.0);
						cvmSet(rz, 2, 0, -sin(theta_z_rad)); cvmSet(rz, 2, 1, 0.0); cvmSet(rz, 2, 2, cos(theta_z_rad));
						printCvMat(rz);

						cvMatMul(rx, r, r);
						cvMatMul(ry, r, r);
						cvMatMul(rz, r, r);

						printCvMat(r);

						// T
						CvMat *T0 = cvCreateMat(4, 4, CV_32F);
						cvmSet(T0, 0, 0, 1); cvmSet(T0, 0, 1, 0); cvmSet(T0, 0, 2, 0); cvmSet(T0, 0, 3, -image1->width / 2);
						cvmSet(T0, 1, 0, 0); cvmSet(T0, 1, 1, 1); cvmSet(T0, 1, 2, 0); cvmSet(T0, 1, 3, -image1->height / 2);
						cvmSet(T0, 2, 0, 0); cvmSet(T0, 2, 1, 0); cvmSet(T0, 2, 2, 1); cvmSet(T0, 2, 3, 0);
						cvmSet(T0, 3, 0, 0); cvmSet(T0, 3, 1, 0); cvmSet(T0, 3, 2, 0); cvmSet(T0, 3, 3, 1);

						CvMat *T = cvCreateMat(4, 4, CV_32F);
						cvmSet(T, 0, 0, 1); cvmSet(T, 0, 1, 0); cvmSet(T, 0, 2, 0); cvmSet(T, 0, 3, tx);
						cvmSet(T, 1, 0, 0); cvmSet(T, 1, 1, 1); cvmSet(T, 1, 2, 0); cvmSet(T, 1, 3, ty);
						cvmSet(T, 2, 0, 0); cvmSet(T, 2, 1, 0); cvmSet(T, 2, 2, 1); cvmSet(T, 2, 3, tz);
						cvmSet(T, 3, 0, 0); cvmSet(T, 3, 1, 0); cvmSet(T, 3, 2, 0); cvmSet(T, 3, 3, 1);

						// K = RI
						CvMat *k = cvCreateMat(4, 4, CV_32F);
						cvmSet(k, 0, 0, cvmGet(r, 0, 0)); cvmSet(k, 0, 1, cvmGet(r, 0, 1)); cvmSet(k, 0, 2, cvmGet(r, 0, 2)); cvmSet(k, 0, 3, 0);
						cvmSet(k, 1, 0, cvmGet(r, 1, 0)); cvmSet(k, 1, 1, cvmGet(r, 1, 1)); cvmSet(k, 1, 2, cvmGet(r, 1, 2)); cvmSet(k, 1, 3, 0);
						cvmSet(k, 2, 0, cvmGet(r, 2, 0)); cvmSet(k, 2, 1, cvmGet(r, 2, 1)); cvmSet(k, 2, 2, cvmGet(r, 2, 2)); cvmSet(k, 2, 3, 0);
						cvmSet(k, 3, 0, 0);               cvmSet(k, 3, 1, 0);               cvmSet(k, 3, 2, 1);               cvmSet(k, 3, 3, 1);

						CvMat *ak = cvCreateMat(4, 4, CV_32F);

						cvMatMul(k, T0, k);
						cvMatMul(T, k, k);

						cvMatMul(a, k, ak);

						CvMat *homography = cvCreateMat(3, 3, CV_32F);

						cvmSet(homography, 0, 0, cvmGet(ak, 0, 0)); cvmSet(homography, 0, 1, cvmGet(ak, 0, 1)); cvmSet(homography, 0, 2, cvmGet(ak, 0, 3));
						cvmSet(homography, 1, 0, cvmGet(ak, 1, 0)); cvmSet(homography, 1, 1, cvmGet(ak, 1, 1)); cvmSet(homography, 1, 2, cvmGet(ak, 1, 3));
						cvmSet(homography, 2, 0, cvmGet(ak, 2, 0)); cvmSet(homography, 2, 1, cvmGet(ak, 2, 1)); cvmSet(homography, 2, 2, cvmGet(ak, 2, 3));
						printCvMat(homography);

						cvWarpPerspective(image1, image, homography, 9, CV_RGB(128,128,255));

						const float h00 = cvmGet(homography, 0, 0);
						const float h01 = cvmGet(homography, 0, 1);
						const float h02 = cvmGet(homography, 0, 2);

						const float h10 = cvmGet(homography, 1, 0);
						const float h11 = cvmGet(homography, 1, 1);
						const float h12 = cvmGet(homography, 1, 2);

						const float h20 = cvmGet(homography, 2, 0);
						const float h21 = cvmGet(homography, 2, 1);
						const float h22 = cvmGet(homography, 2, 2);

						const float ltx = (0 * h00 + 0 * h01 + 1 * h02) / (0 * h20 + 0 * h21 * 1 + h22);
						const float lty = (0 * h10 + 0 * h11 + 1 * h12) / (0 * h20 + 0 * h21 * 1 + h22);
						printf("ltx: %7.4f, lty: %7.4f\n", ltx, lty);

						const float rtx = (image1->width * h00 + 0 * h01 + 1 * h02) / (image1->width * h20 + 0 * h21 + 1 * h22);
						const float rty = (image1->width * h10 + 0 * h11 + 1 * h12) / (image1->width * h20 + 0 * h21 + 1 * h22);
						printf("rtx: %7.4f, rty: %7.4f\n", rtx, rty);

						const float rbx = (image1->width * h00 + image1->height * h01 + 1 * h02) / (image1->width * h20 + image1->height * h21 + 1 * h22);
						const float rby = (image1->width * h10 + image1->height * h11 + 1 * h12) / (image1->width * h20 + image1->height * h21 + 1 * h22);
						printf("rbx: %7.4f, rby: %7.4f\n", rbx, rby);

						const float lbx = (0 * h00 + image1->height * h01 + 1 * h02) / (0 * h20 + image1->height * h21 + 1 * h22);
						const float lby = (0 * h10 + image1->height * h11 + 1 * h12) / (0 * h20 + image1->height * h21 + 1 * h22);
						printf("lbx: %7.4f, lby: %7.4f\n", lbx, lby);

						const float p0x = min(min(ltx, rtx), min(lbx, rbx));
						const float p0y = min(min(lty, rty), min(lby, rby));
						const float p1x = max(max(ltx, rtx), max(lbx, rbx));
						const float p1y = max(max(lty, rty), max(lby, rby));

						cvRectangle(image, cvPoint(p0x, p0y), cvPoint(p1x, p1y), CV_RGB(255, 0, 0));

						cvReleaseMat(&T);

						cvReleaseMat(&rz);
						cvReleaseMat(&ry);
						cvReleaseMat(&rx);
						cvReleaseMat(&r);

						cvReleaseMat(&homography);
						cvReleaseMat(&ak);
						cvReleaseMat(&k);
						cvReleaseMat(&a);
#endif

						// Save result image
						if (dst != NULL)
						{
							char temp[BUFSIZ] = { 0 };
							sprintf_s(temp, BUFSIZ, "%s\\%07d.jpg", dst, index);
							printf("Saving... %s\n", temp);

							cvSaveImage(temp, image);
						}

						cvReleaseImage(&image);

						index++;
						halt = (index == 100);

						// Release
						cvReleaseImage(&image2);
					}

					// Release
					cvReleaseImage(&image1);
				}
			}
		}
	}

	return 0;
}

