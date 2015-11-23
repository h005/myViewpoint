#ifndef LMDLT_H
#define LMDLT_H

#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>

class LMDLT
{
public:
    static void DLTwithPoints(int matchnum,
        float points2d[][2],
        float points3d[][3],
        int imgWidth,
        int imgHeight,
        const cv::Mat &initialCameraMatrix,
        const cv::Mat &initialExtrinsicMatrix,
        cv::Mat &modelView, cv::Mat &K);

    static void ModelRegistration(int matchnum,
                                  glm::vec3 ptCloudPoints[],
                                  glm::vec3 modelPoints[],
                                  glm::mat3 &R,
                                  glm::vec3 &t,
                                  float &c);
};

#endif // LMDLT_H
