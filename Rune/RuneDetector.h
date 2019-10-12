#pragma once

#include "opencv2/opencv.hpp"
#include "../Camera/FightingCameraParam.h"

class RuneDetector {
public:
    RuneDetector();
    ~RuneDetector();
    bool DetectRune(cv::Mat& src, cv::Point3f& target_3d);

private:
    bool enable_debug;
    cv::Mat show_rune_contours;
    cv::Mat show_possible_armors;
    cv::Mat show_final_armor;

    void PossibleArmors(const std::vector<std::vector<cv::Point>>& contours,
        const std::vector<cv::Vec4i>& hierarchy,
        std::vector<cv::RotatedRect>& armors);
    cv::RotatedRect& SelectFinalArmor(std::vector<cv::RotatedRect>& armors);
    void CalcControlInfo(const cv::RotatedRect& armor, cv::Point3f& target);

    void DrawRotatedRect(const cv::Mat& image, const cv::RotatedRect& rect, const cv::Scalar& color, int thickness);
};
