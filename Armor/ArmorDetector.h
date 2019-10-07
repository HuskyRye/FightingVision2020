#pragma once

#include "opencv2/opencv.hpp"

class ArmorInfo {
public:
    explicit ArmorInfo(cv::RotatedRect armor_rect, float armor_stddev = 0.0)
        : rect(armor_rect)
        , stddev(armor_stddev)
    {
    }

public:
    cv::RotatedRect rect;
    float stddev;
    // TODO: classifier ‰≥ˆ÷µ
};

class ArmorDetector {
public:
    ArmorDetector();
    ~ArmorDetector();
    bool DetectArmor(cv::Mat& src, cv::Point3f& target_3d);

private:
    bool enable_debug;
    enum class State { SEARCHING_STATE,
        TRACKING_STATE };
    State state;

    enum class EnemyColor { RED,
        BLUE };
    EnemyColor enemy_color;

    cv::Mat detect_lights;
    cv::Mat armors_before_filter;
    cv::Mat armors_after_filter;

    float color_thresh;
    float blue_thresh, red_thresh;
    float light_min_area;
    void DetectLights(const cv::Mat& src, std::vector<cv::RotatedRect>& lights);
    cv::Mat DistillationColor(const cv::Mat& src);
    void DrawRotatedRect(const cv::Mat& image, const cv::RotatedRect& rect, const cv::Scalar& color, int thickness);

    float armor_max_angle_diff;
    float armor_max_aspect_ratio;
    float armor_max_height_ratio;
    float armor_min_area;
    void PossibleArmors(const std::vector<cv::RotatedRect>& lights, std::vector<ArmorInfo>& armors);
    void FilterArmors(std::vector<ArmorInfo>& armors);
    ArmorInfo SelectFinalArmor(std::vector<ArmorInfo>& armors);
    
    cv::Mat camera_matrix, distortion_coeffs;
    float camera_width, camera_height;
    float small_armor_width, big_armor_width, armor_height;
    std::vector<cv::Point3f> small_armor_points, big_armor_points;
    void CalcControlInfo(const ArmorInfo& armor, cv::Point3f& target_3d);
};
