
#include "RuneDetector.h"

RuneDetector::RuneDetector()
{
    enable_debug = true;
    // TODO: load from Serial
}

RuneDetector::~RuneDetector()
{
}

bool RuneDetector::DetectRune(cv::Mat& src, cv::Point3f& target)
{
    if (enable_debug) {
        show_rune_contours = src.clone();
        show_possible_armors = src.clone();
        show_final_armor = src.clone();
        auto c = cv::waitKey(1);
        if (c == 'a')
            cv::waitKey();
    }

    cv::Mat gray_image;
    cv::cvtColor(src, gray_image, cv::COLOR_BGR2GRAY);

    cv::Mat binary_brightness_image;
    cv::threshold(gray_image, binary_brightness_image, 200, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> rune_contours;
    std::vector<cv::Vec4i> rune_hierarchy;
    cv::findContours(binary_brightness_image, rune_contours, rune_hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    if (rune_contours.empty())
        return false;
    std::vector<cv::RotatedRect> possible_armors;
    PossibleArmors(rune_contours, rune_hierarchy, possible_armors);
    if (possible_armors.empty())
        return false;
    cv::RotatedRect& final_armor = SelectFinalArmor(possible_armors);
    CalcControlInfo(final_armor, target);

    if (enable_debug) {
        cv::drawContours(show_rune_contours, rune_contours, -1, cv::Scalar(0, 255, 0), 2);
        for (auto rect : possible_armors)
            DrawRotatedRect(show_possible_armors, rect, cv::Scalar(0, 255, 0), 2);
        DrawRotatedRect(show_final_armor, final_armor, cv::Scalar(0, 255, 0), 2);
        cv::imshow("binary_brightness_image", binary_brightness_image);
        cv::imshow("show_rune_contours", show_rune_contours);
        cv::imshow("show_possible_armors", show_possible_armors);
        cv::imshow("show_final_armor", show_final_armor);
    }

    return true;
}

void RuneDetector::PossibleArmors(const std::vector<std::vector<cv::Point>>& contours, const std::vector<cv::Vec4i>& hierarchy, std::vector<cv::RotatedRect>& armors)
{
    int index = 0;
    do {
        int child_index = hierarchy[index][2];
        if (child_index != -1
            && hierarchy[child_index][0] == -1) {
            cv::RotatedRect possible_armor = cv::minAreaRect(contours[child_index]);
            if (possible_armor.size.area() > 100)
                armors.emplace_back(possible_armor);
        }
    } while (hierarchy[index][0] != -1 && (index = hierarchy[index][0]));
}

cv::RotatedRect& RuneDetector::SelectFinalArmor(std::vector<cv::RotatedRect>& armors)
{
    std::sort(armors.begin(), armors.end(),
        [](const cv::RotatedRect& r1, const cv::RotatedRect& r2) { return r1.size.area() > r2.size.area(); });
    return armors[0];
}

void RuneDetector::CalcControlInfo(const cv::RotatedRect& armor, cv::Point3f& target)
{
    static float half_camera_width = cameraInfo.resolution_width / 2;
    static float half_camera_height = cameraInfo.resolution_height / 2;
    static float camera_fx = cameraInfo.camera_matrix.at<double>(0, 0);
    static float camera_fy = cameraInfo.camera_matrix.at<double>(1, 1);
    constexpr double rad = 57.295779513082320876798154814105;
    float yaw_offset = atan2(half_camera_width - armor.center.x, camera_fx) * rad;
    float pitch_offset = atan2(half_camera_height - armor.center.y, camera_fy) * rad;
    // float distance = camera_fx * armor_height / armor.size.height;
    // target = cv::Point3f(yaw_offset, pitch_offset, distance);
}

void RuneDetector::DrawRotatedRect(const cv::Mat& image, const cv::RotatedRect& rect, const cv::Scalar& color, int thickness)
{
    cv::Point2f vertices[4];
    rect.points(vertices);
    for (int i = 0; i < 4; i++)
        cv::line(show_final_armor, vertices[i], vertices[(i + 1) % 4], color, thickness, cv::LINE_AA);
}
