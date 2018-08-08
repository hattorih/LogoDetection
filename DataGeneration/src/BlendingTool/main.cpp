#include "opencv2/opencv.hpp"

#include <string>
#include <vector>

#include <windows.h>


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


void prtCvMat(const CvMat *m, const std::string &name)
{
	float m00 = cvmGet(m, 0, 0); float m01 = cvmGet(m, 0, 1); float m02 = cvmGet(m, 0, 2);
	float m10 = cvmGet(m, 1, 0); float m11 = cvmGet(m, 1, 1); float m12 = cvmGet(m, 1, 2);
	float m20 = cvmGet(m, 2, 0); float m21 = cvmGet(m, 2, 1); float m22 = cvmGet(m, 2, 2);

	printf("[%11.4f\t%11.4f\t%11.4f\n",  m00, m01, m02);
	printf(" %11.4f\t%11.4f\t%11.4f\n",  m10, m11, m12);
	printf(" %11.4f\t%11.4f\t%11.4f] --- %s\n", m20, m21, m22, name.c_str());
}


float rand(const float n) // return [-n:+n]
{
	const float temp = (rand() % 201 - 100) * 0.01f; // -1.00f to +1.00f
	return n * temp;
}


int main(int argc, char **argv)
{
	const char *src = NULL;
	const char *src_mask = NULL;
	const char *src_background = NULL;

	const char *dst = NULL;

	bool no_display = false;

	for (int i = 1; i < argc - 1; i++)
	{
		if (strcmp(argv[i], "-src") == 0) src = argv[++i];
		if (strcmp(argv[i], "-src_mask") == 0) src_mask = argv[++i];
		if (strcmp(argv[i], "-src_background") == 0) src_background = argv[++i];

		if (strcmp(argv[i], "-dst") == 0) dst = argv[++i];

		if (strcmp(argv[i], "-no_display") == 0) no_display = (atoi(argv[++i]) == 1);
	}

	if (src == NULL || src_mask == NULL || src_background == NULL)
	{
		printf("Usage: %s -src <src_dir> -src_mask <src_mask_dir> -dst <dst_dir>\n", argv[0]);
	}
	else
	{
		std::vector<std::string> src_files;
		find(src, src_files);

		std::vector<std::string> src_mask_files;
		find(src_mask, src_mask_files);

		std::vector<std::string> src_background_files;
		find(src_background, src_background_files);

		if (src_files.size() == 0 || src_mask_files.size() == 0 || src_files.size() != src_mask_files.size() || src_background_files.size() == 0)
		{
			;
		}
		else
		{
			bool halt = false;
			while (!halt)
			{
				const int i = rand() % src_files.size();
				const int j = rand() % src_background_files.size();

				// Load image
				IplImage *image = NULL;
				if ((image = cvLoadImage(src_files[i].c_str())) == NULL)
				{
					printf("Error: cannot load image correctly: %s\n", src_files[i].c_str());
				}
				else
				{
					// Load mask image
					IplImage *image_mask = NULL;
					if ((image_mask = cvLoadImage(src_mask_files[i].c_str())) == NULL)
					{
						printf("Error: cannot load image correctly: %s\n", src_mask_files[i].c_str());
					}
					else
					{
						if (image->width != image_mask->width || image->height != image_mask->height)
						{
							printf("Error: the size %dx%d and %dx%d\n", image->width, image->height, image_mask->width, image_mask->height);
						}
						else
						{
							// Add smoothing
							cvSmooth(image_mask, image_mask);

							// Load background image
							IplImage *image_background = NULL;
							if ((image_background = cvLoadImage(src_background_files[j].c_str())) == NULL)
							{
								printf("Error: cannot load image correctly: %s\n", src_background_files[j].c_str());
							}
							else
							{
								static int index = 0;
								printf("%07d\n", index);

								const int w = image->width;
								const int h = image->height;

								IplImage *image_result = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);

								int w_offset = image_background->width - w;
								int h_offset = image_background->height - h;

								if (0 < w_offset && 0 < h_offset)
								{
									cvSetImageROI(image_background, cvRect(rand() % w_offset, rand() % h_offset, w, h));
									cvCopy(image_background, image_result);

									printf("Copy background\n");
								}

								for (int y = 0; y < h; y++)
								{
									for (int x = 0; x < w; x++)
									{
										unsigned char b1 = ((unsigned char *)(image->imageData))[image->widthStep * y + x * 3 + 0];
										unsigned char g1 = ((unsigned char *)(image->imageData))[image->widthStep * y + x * 3 + 1];
										unsigned char r1 = ((unsigned char *)(image->imageData))[image->widthStep * y + x * 3 + 2];

										unsigned char b2 = ((unsigned char *)(image_result->imageData))[image_result->widthStep * y + x * 3 + 0];
										unsigned char g2 = ((unsigned char *)(image_result->imageData))[image_result->widthStep * y + x * 3 + 1];
										unsigned char r2 = ((unsigned char *)(image_result->imageData))[image_result->widthStep * y + x * 3 + 2];

										unsigned char coef = ((unsigned char *)(image_mask->imageData))[image_mask->widthStep * y + x * 3 + 0];

										((unsigned char *)(image_result->imageData))[image_result->widthStep * y + x * 3 + 0] = (b1 * coef + b2 * (255 - coef)) / 255;
										((unsigned char *)(image_result->imageData))[image_result->widthStep * y + x * 3 + 1] = (g1 * coef + g2 * (255 - coef)) / 255;
										((unsigned char *)(image_result->imageData))[image_result->widthStep * y + x * 3 + 2] = (r1 * coef + r2 * (255 - coef)) / 255;
									}
								}

								// Save result image
								if (dst != NULL)
								{
									char temp[BUFSIZ] = { 0 };
									sprintf_s(temp, BUFSIZ, "%s\\%07d.jpg", dst, index);
									printf("Saving... %s\n", temp);

									cvSaveImage(temp, image_result);
								}

								cvReleaseImage(&image_result);

								cvReleaseImage(&image_background);

								index++;
								halt = (index == 100);
							}
						}

						// Release
						cvReleaseImage(&image_mask);
					}

					// Release
					cvReleaseImage(&image);
				}
			}
		}
	}

	return 0;
}

