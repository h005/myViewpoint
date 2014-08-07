#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

extern "C" {
#include <vl/generic.h>
#include <vl/covdet.h>
#include <vl/sift.h>
}

#include "def.h"

using namespace cv;

static bool checkDescSame(singleFeature* sf, const std::vector<singleFeature *> &featsList)
{
	/*
		判断在feat列表中是否已经存在相似特征
		*/
	for (int i = 0; i != featsList.size(); ++i)
	{
		if (abs(featsList.at(i)->c.x - sf->c.x) > 3
			|| abs(featsList.at(i)->c.y - sf->c.y) > 3)
			continue;
		else
		{
			int index = -1;
			for (int j = 0; j != ORIGINAL_DIM; ++j)
			{
				++index;
				if (sf->descriptor[j] != featsList.at(i)->descriptor[j])
					break;
			}

			if (index == ORIGINAL_DIM - 1)
				return true;
		}
	}

	return false;
}

static void flip_descriptor(float *dst, float const *src)
{
	int const BO = 8;  /* number of orientation bins */
	int const BP = 4;  /* number of spatial bins     */
	int i, j, t;

	for (j = 0; j < BP; ++j) //row
	{
		int jp = BP - 1 - j;
		for (i = 0; i < BP; ++i) //col
		{
			int o = BO * i + BP*BO * j;
			int op = BO * i + BP*BO * jp;

			// src[j][i][0] = dst[BP - 1 - j][i][0]
			dst[op] = src[o];
			for (t = 1; t < BO; ++t)
				//dst[BP - 1 - j][i][BO - t] = src[j][i][t]
				dst[BO - t + op] = src[t + o];
		}
	}
}

void generateFeats(const char* fileName, detectTypes detectTypeNum, std::vector<singleFeature *> &featsList)
{
	// 释放并清空列表中已有的元素
	if (featsList.size() != 0)
	{
		for (int i = 0; i != featsList.size(); ++i)
			delete featsList.at(i);
		featsList.clear();
	}

	// 读取图像，并将结果归一化放在浮点数组中
	IplImage *Image = cvLoadImage(fileName, 0);
	float *ImageData = new float[Image->height * Image->width];
	int k = 0;
	for (int i = 0; i < Image->height; i++)
	{
		for (int j = 0; j < Image->width; j++)
		{
			ImageData[j*Image->height + i] = (float)(((uchar*)(Image->imageData))[k++]) / 255.0;
		}
	}

	// 根据设定的类型生成并配置检测器
	VlCovDet * covd = NULL;
	if (detectTypeNum == DETECT_HARRIS_AFFINE)
		covd = vl_covdet_new(VL_COVDET_METHOD_HARRIS_LAPLACE);
	else if (detectTypeNum == DETECT_HESSIAN_AFFINE)
		covd = vl_covdet_new(VL_COVDET_METHOD_HESSIAN_LAPLACE);
	vl_covdet_set_transposed(covd, VL_TRUE);
	vl_covdet_set_first_octave(covd, -1);
	double peakThreshold = 1e-7;
	if (detectTypeNum == DETECT_HESSIAN_AFFINE)
		peakThreshold = 0.001;

	vl_covdet_set_peak_threshold(covd, peakThreshold);
	//vl_covdet_set_edge_threshold(covd, edgeThreshold) ;
	vl_covdet_put_image(covd, ImageData, Image->height, Image->width);
	vl_covdet_detect(covd);
	vl_covdet_drop_features_outside(covd, 2);
	vl_covdet_extract_orientations(covd);

	vl_size numFeatures = vl_covdet_get_num_features(covd);
	VlCovDetFeature * feature = (VlCovDetFeature *)vl_covdet_get_features(covd);


	//sift descripter
	vl_index patchResolution = -1;
	double patchRelativeExtent = -1;
	double patchRelativeSmoothing = -1;
	float *patch = NULL;
	float *patchXY = NULL;
	if (patchResolution < 0)  patchResolution = 15;
	if (patchRelativeExtent < 0) patchRelativeExtent = 7.5;
	if (patchRelativeSmoothing < 0) patchRelativeSmoothing = 1;
	if (patchResolution > 0)
	{
		vl_size w = 2 * patchResolution + 1;
		patch = (float*)malloc(sizeof(float)* w * w);
		patchXY = (float*)malloc(2 * sizeof(float)* w * w);
	}
	VlSiftFilt * sift = vl_sift_new(16, 16, 1, 3, 0);
	vl_size dimension = 128;
	vl_size patchSide = 2 * patchResolution + 1;
	double patchStep = (double)patchRelativeExtent / patchResolution;
	float tempDesc[128];
	float * desc = new float[128 * numFeatures];
	vl_sift_set_magnif(sift, 3.0);
	vl_size i;

	for (i = 0; i < (signed)numFeatures; ++i)
	{
		vl_covdet_extract_patch_for_frame(covd, patch, patchResolution, patchRelativeExtent, patchRelativeSmoothing, feature[i].frame);

		vl_imgradient_polar_f(patchXY, patchXY + 1, 2, 2 * patchSide, patch, patchSide, patchSide, patchSide);

		vl_sift_calc_raw_descriptor(sift, patchXY, tempDesc, (int)patchSide, (int)patchSide, (double)(patchSide - 1) / 2, (double)(patchSide - 1) / 2,
			(double)patchRelativeExtent / (3.0 * (4 + 1) / 2) / patchStep, VL_PI / 2);

		flip_descriptor(desc, tempDesc);
		desc += dimension;
	}
	vl_sift_delete(sift);

	desc -= dimension*numFeatures;
	for (i = 0; i < (signed)numFeatures; ++i)
	{
		singleFeature* sf = new singleFeature();
		sf->c.x = feature[i].frame.y;
		sf->c.y = feature[i].frame.x;
		sf->trackC = sf->c;
		for (int j = 0; j < 128; j++)
		{
			sf->descriptor[j] = (int)(512.0*desc[i * 128 + j] + 0.5);
			if (sf->descriptor[j] > 255)	sf->descriptor[j] = 255;
		}
		if (!checkDescSame(sf, featsList))
			featsList.push_back(sf);
		else
			delete sf;
	}
	cvReleaseImage(&Image);
	delete[]ImageData;
	delete[]desc;
	//delete feature ;
	vl_covdet_delete(covd);
	//feature = NULL ;
	if (patchXY) free(patchXY);
	if (patch) free(patch);
}

void generateSIFTFeats(const char* fileName, std::vector<singleFeature *> &featsList)
{

	// 释放并清空列表中已有的元素
	if (featsList.size() != 0)
	{
		for (int i = 0; i != featsList.size(); ++i)
			delete featsList.at(i);
		featsList.clear();
	}

	IplImage *img = cvLoadImage(fileName, 0);

	int i, j, k;
	int tmp = img->width;
	if (img->height < tmp)	tmp = img->height;
	double o = log((double)tmp) / log(2.0);

	VlSiftFilt* filter;
	filter = vl_sift_new(img->width, img->height, (int)(o + 0.5), LEVELS_PER_OCTIVE, MIN_OCTIVE_INDEX);

	vl_sift_pix* tmpData = new vl_sift_pix[(img->width) * (img->height)];
	for (i = 0; i < (img->width) * (img->height); i++)
		tmpData[i] = (vl_sift_pix)(((uchar*)(img->imageData))[i]);

	vl_sift_pix* descTmp;

	vl_sift_process_first_octave(filter, tmpData);

	delete[] tmpData;

	vl_sift_detect(filter);

	const VlSiftKeypoint* klist = vl_sift_get_keypoints(filter);

	int temInt = vl_sift_get_nkeypoints(filter);
	for (i = 0; i < temInt; ++i)
	{
		double angle[4];
		int t = vl_sift_calc_keypoint_orientations(filter, angle, &(klist[i]));
		if (t == 0)
			continue;
		for (j = 0; j < t; ++j)
		{
			singleFeature* sf = new singleFeature();
			sf->c.x = klist[i].x;
			sf->c.y = klist[i].y;
			sf->trackC.x = klist[i].x;
			sf->trackC.y = klist[i].y;
			sf->octaveIndex = klist[i].o;
			sf->scale = klist[i].sigma;
			sf->oritation = angle[j];

			descTmp = new vl_sift_pix[ORIGINAL_DIM];
			vl_sift_calc_keypoint_descriptor(filter, descTmp, &(klist[i]), angle[j]);

			for (k = 0; k < ORIGINAL_DIM; ++k)
			{
				sf->descriptor[k] = (int)(512.0 * descTmp[k] + 0.5);
				if (sf->descriptor[k] > 255)	sf->descriptor[k] = 255;
			}

			featsList.push_back(sf);
			delete[] descTmp;
			descTmp = NULL;
		}
	}

	while (vl_sift_process_next_octave(filter) != VL_ERR_EOF)
	{
		vl_sift_detect(filter);

		const VlSiftKeypoint* klist = vl_sift_get_keypoints(filter);

		for (i = 0; i < vl_sift_get_nkeypoints(filter); i++)
		{
			double angle[4];
			int t = vl_sift_calc_keypoint_orientations(filter, angle, &(klist[i]));
			if (t == 0)
				continue;
			for (j = 0; j < t; j++)
			{
				singleFeature* sf = new singleFeature();
				sf->c.x = klist[i].x;
				sf->c.y = klist[i].y;
				sf->trackC.x = klist[i].x;
				sf->trackC.y = klist[i].y;
				sf->octaveIndex = klist[i].o;
				sf->scale = klist[i].sigma;
				sf->oritation = angle[j];

				descTmp = new vl_sift_pix[ORIGINAL_DIM];
				vl_sift_calc_keypoint_descriptor(filter, descTmp, &(klist[i]), angle[j]);

				for (k = 0; k < ORIGINAL_DIM; k++)
				{
					sf->descriptor[k] = (int)(512.0 * descTmp[k] + 0.5);
					if (sf->descriptor[k] > 255)	sf->descriptor[k] = 255;
				}
				featsList.push_back(sf);
				delete[] descTmp;
				descTmp = NULL;
			}
		}
	}
	vl_sift_delete(filter);
}