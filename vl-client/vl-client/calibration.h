#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
* \brief Compute and draw the epipolar lines in two images
*      associated to each other by a fundamental matrix
*
* \param title     Title of the window to display
* \param F         Fundamental matrix
* \param img1      First image
* \param img2      Second image
* \param points1   Set of points in the first image
* \param points2   Set of points in the second image matching to the first set
* \param inlierDistance      Points with a high distance to the epipolar lines are
*                not displayed. If it is negative, all points are displayed
**/
template <typename T2>
static void drawEpipolarLines(const cv::Mat &F,
	const cv::Mat& img1, const cv::Mat& img2,
	const std::vector<cv::Point_<T2>> points1,
	const std::vector<cv::Point_<T2>> points2,
	const float inlierDistance = -1,
	const char * outputPath = NULL)
{
	Mat outImg(std::max<int>(img1.rows, img2.rows), img1.cols + img2.cols, img1.type());
	outImg.setTo(0);
	cv::Rect rect1(0, 0, img1.cols, img1.rows);
	cv::Rect rect2(img1.cols, 0, img2.cols, img2.rows);
	img1.copyTo(outImg(rect1));
	img2.copyTo(outImg(rect2));

	std::vector<cv::Vec<T2, 3>> epilines1, epilines2;
	cv::computeCorrespondEpilines(points1, 1, F, epilines1); //Index starts with 1
	cv::computeCorrespondEpilines(points2, 2, F, epilines2);

	CV_Assert(points1.size() == points2.size() &&
		points2.size() == epilines1.size() &&
		epilines1.size() == epilines2.size());

	cv::RNG rng(0);
	for (size_t i = 0; i<points1.size(); i++)
	{
		if (inlierDistance > 0)
		{
			if (distancePointLine(points1[i], epilines2[i]) > inlierDistance ||
				distancePointLine(points2[i], epilines1[i]) > inlierDistance)
			{
				//The point match is no inlier
				continue;
			}
		}
		/*
		* Epipolar lines of the 1st point set are drawn in the 2nd image and vice-versa
		*/
		cv::Scalar color(rng(256), rng(256), rng(256));

		cv::Point pa, pb;
		convertLineEquationToPoints(epilines1[i][0], epilines1[i][1], epilines1[i][2], rect2.size(), pa, pb);
		cv::line(outImg(rect2),
			pa,
			pb,
			color,
			1,
			CV_AA);
		cv::circle(outImg(rect1), points1[i], 3, color, -1, CV_AA);

		cv::Point pc, pd;
		convertLineEquationToPoints(epilines2[i][0], epilines2[i][1], epilines2[i][2], rect1.size(), pc, pd);
		cv::line(outImg(rect1),
			pc,
			pd,
			color,
			1,
			CV_AA);
		cv::circle(outImg(rect2), points2[i], 3, color, -1, CV_AA);
	}
	if (outputPath) {
		cv::imwrite(outputPath, outImg);
	}
	else {
		cv::imshow("epipolar.png", outImg);
	}
}

template <typename T>
static float distancePointLine(const cv::Point_<T> point, const cv::Vec<T, 3>& line)
{
	//Line is given as a*x + b*y + c = 0
	return std::fabsf(line(0)*point.x + line(1)*point.y + line(2))
		/ std::sqrt(line(0)*line(0) + line(1)*line(1));
}