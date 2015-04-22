#ifndef CVDLT_H
#define CVDLT_H

#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>

class CVDLT
{
public:
    static void DLTwithPoints(int matchnum,
            float points2d[][2],
            float points3d[][3],
            int imgWidth,
            int imgHeight,
            const cv::Mat &initialCameraMatrix,
            glm::mat4 &mvMatrix,
            glm::mat4 &projMatrix);
};

#endif // CVDLT_H
