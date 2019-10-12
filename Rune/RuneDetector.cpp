
#include "RuneDetector.h"

RuneDetector::RuneDetector()
{
    enable_debug = true;
}

RuneDetector::~RuneDetector()
{
}

bool RuneDetector::DetectRune(cv::Mat& src, cv::Point3f& target)
{
    if (enable_debug) {
        auto c = cv::waitKey(1);
        if (c == 'a')
            cv::waitKey();
    }

    if (enable_debug) {
        show_rune_contours = src.clone();
        show_possible_armors = src.clone();
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
    int index = 0;
    do {
        int child_index = rune_hierarchy[index][2];
        if (child_index != -1
            && rune_hierarchy[child_index][0] == -1) {
            cv::RotatedRect possible_armor = cv::minAreaRect(rune_contours[child_index]);
            if (possible_armor.size.area() > 100)
                possible_armors.emplace_back(possible_armor);
        }
    } while (rune_hierarchy[index][0] != -1 && (index = rune_hierarchy[index][0]));
    if (possible_armors.empty())
        return false;

    if (enable_debug) {
        cv::drawContours(show_rune_contours, rune_contours, -1, cv::Scalar(0, 255, 0), 2);
        for (auto rect : possible_armors) {
            cv::Point2f vertices[4];
            rect.points(vertices);
            for (int i = 0; i < 4; i++)
                cv::line(show_possible_armors, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
        }
        cv::imshow("binary_brightness_image", binary_brightness_image);
        cv::imshow("armor_contours", show_rune_contours);
        cv::imshow("show_possible_armors", show_possible_armors);
    }

    std::sort(possible_armors.begin(), possible_armors.end(),
        [](const cv::RotatedRect& r1, const cv::RotatedRect& r2) { return r1.size.area() > r2.size.area(); });
    cv::RotatedRect final_armor = possible_armors[0];

    // count
    static float half_camera_width = camera_width / 2;
    static float half_camera_height = camera_height / 2;
    static float camera_fx = camera_matrix.at<double>(0, 0);
    static float camera_fy = camera_matrix.at<double>(1, 1);
    constexpr double rad = 57.295779513082320876798154814105;
    float yaw_offset = atan2(half_camera_width - final_armor.center.x, camera_fx) * rad;
    float pitch_offset = atan2(half_camera_height - final_armor.center.y, camera_fy) * rad;
    float distance = camera_fx * armor_height / final_armor.size.height;
    target = cv::Point3f(yaw_offset, pitch_offset, distance);

    return true;
}
