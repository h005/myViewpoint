#include <iostream>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

using namespace cv;

int main()
{
	
	FILE *fp;
	if (!(fp = fopen("kxm.data", "r"))) {
		printf("no file\n");
		system("pause");
		return -1;
	}

	int num, featureCount;
	fscanf(fp, "%d %d\n", &num, &featureCount);

	float *labels = new float[num];
	float *features = new float[num * featureCount];
	for (int t = 0; t < num; t++) {
		fscanf(fp, "%f", &labels[t]);
		printf("%f\n", labels[t]);
		for (int i = 0; i < featureCount; i++)
			fscanf(fp, "%f", &features[t * featureCount + i]);
	}
	Mat labelsMat(num, 1, CV_32FC1, labels);
	Mat trainingDataMat(num, featureCount, CV_32FC1, features);

	// Set up SVM's parameters
	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::LINEAR;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

	// Train the SVM
	CvSVM SVM;
	SVM.train(trainingDataMat, labelsMat, Mat(), Mat(), params);

	for (int t = 0; t < num; t++) {
		float response = SVM.predict(trainingDataMat.row(t));
		printf("%f\n", response);
	}
	return 0;
}