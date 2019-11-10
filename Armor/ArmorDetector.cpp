/*
 *  Copyright (C) 2019 刘臣轩
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ArmorDetector.h"

ArmorDetector::ArmorDetector()
{
    // TODO: 从串口读取数据
    enemy_color = EnemyColor::BLUE;
    enable_debug = true;
    number_template.emplace_back(cv::Mat());
    for (int i = 1; i <= 8; ++i)
        number_template.emplace_back(cv::imread(std::to_string(i) + ".png", cv::IMREAD_GRAYSCALE));

}

ArmorDetector::~ArmorDetector()
{
}

bool ArmorDetector::DetectArmor(cv::Mat& src, cv::Point3f& target)
{
    if (enable_debug) {
        show_lights = src.clone();
        show_armors = src.clone();
        show_final_armor = src.clone();
        auto c = cv::waitKey(1);
        if (c == 'a')
            cv::waitKey();
    }
    std::vector<cv::RotatedRect> lights;
    DetectLights(src, lights);

    std::vector<std::vector<cv::Point2f>> armors;
    DetectArmors(lights, armors);
    if (armors.empty())
        return false;

    std::vector<cv::Point2f>& final_armor = SelectFinalArmor(src, armors);
    CalcControlInfo(final_armor, target);
    return true;
}

cv::Mat ArmorDetector::DistillationColor(const cv::Mat& src)
{
    std::vector<cv::Mat> bgr;
    cv::split(src, bgr);
    cv::Mat result;
    if (EnemyColor::BLUE == enemy_color)
        cv::subtract(bgr[0], bgr[2], result);
    else
        cv::subtract(bgr[2], bgr[0], result);
    return result;
}

inline void ArmorDetector::DrawRotatedRect(const cv::Mat& image, const cv::RotatedRect& rect, const cv::Scalar& color, int thickness)
{
    cv::Point2f vertices[4];
    rect.points(vertices);
    for (int i = 0; i < 4; i++)
        cv::line(image, vertices[i], vertices[(i + 1) % 4], color, thickness, cv::LINE_AA);
}

void ArmorDetector::DetectLights(const cv::Mat& src, std::vector<cv::RotatedRect>& lights)
{
    cv::Mat gray_image;
    cv::cvtColor(src, gray_image, cv::COLOR_BGR2GRAY);
    cv::Mat binary_brightness_image;
    cv::threshold(gray_image, binary_brightness_image, armorParam.brightness_thresh, 255, cv::THRESH_BINARY);

    cv::Mat binary_color_image;
    cv::Mat distillation = DistillationColor(src);
    float thresh = (enemy_color == EnemyColor::BLUE) ? armorParam.blue_thresh : armorParam.red_thresh;
    cv::threshold(distillation, binary_color_image, thresh, 255, cv::THRESH_BINARY);
    cv::morphologyEx(binary_color_image, binary_color_image, cv::MORPH_CLOSE, cv::Mat());

    std::vector<std::vector<cv::Point>> contours_color;
    cv::findContours(binary_color_image, contours_color, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<std::vector<cv::Point>> contours_brightness;
    cv::findContours(binary_brightness_image, contours_brightness, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    lights.reserve(contours_color.size());
    for (unsigned int i = 0; i < contours_brightness.size(); ++i) {
        for (unsigned int j = 0; j < contours_color.size(); ++j) {
            if (cv::pointPolygonTest(contours_color[j], contours_brightness[i][0], false) >= 0.0) {
                cv::RotatedRect single_light = cv::minAreaRect(contours_brightness[i]);
                cv::Rect bounding_rect = single_light.boundingRect();
                if (bounding_rect.height < bounding_rect.width || single_light.size.area() < armorParam.light_min_area)
                    break;
                lights.emplace_back(single_light);
                break;
            }
        }
    }
    if (enable_debug) {
        for (auto light : lights)
            DrawRotatedRect(show_lights, light, cv::Scalar(0, 255, 0), 2);
        cv::imshow("binary_brightness_image", binary_brightness_image);
        cv::imshow("binary_color_img", binary_color_image);
        cv::imshow("show_lights", show_lights);
    }
}

inline void ArmorDetector::DrawArmor(const cv::Mat& image, const std::vector<cv::Point2f>& armor, const cv::Scalar& color, int thickness)
{
    for (int i = 0; i < 4; i++)
        cv::line(image, armor[i], armor[(i + 1) % 4], color, thickness, cv::LINE_AA);
}

void ArmorDetector::DetectArmors(const std::vector<cv::RotatedRect>& lights, std::vector<std::vector<cv::Point2f>>& armors)
{
    for (unsigned int i = 0; i < lights.size(); ++i) {
        for (unsigned int j = i + 1; j < lights.size(); ++j) {
            float angle_i = (lights[i].angle <= -45.0) ? lights[i].angle + 90 : lights[i].angle;
            float angle_j = (lights[j].angle <= -45.0) ? lights[j].angle + 90 : lights[j].angle;
            float angle_diff = std::abs(angle_i - angle_j);
            // std::cout << "\nangle_diff = " << angle_diff;
            if (angle_diff > armorParam.armor_max_angle_diff)
                continue;

            double armor_angle = (double)std::atan((lights[i].center.y - lights[j].center.y) / (lights[i].center.x - lights[j].center.x)) * 180 / CV_PI;
            // std::cout << "\nangle_i = " << angle_i;
            // std::cout << "\nangle_j = " << angle_j;
            // std::cout << "\narmor_angle = " << armor_angle;
            if (std::max(abs(armor_angle - angle_i), abs(armor_angle - angle_j)) > armorParam.armor_max_angle_diff)
                continue;

            float height_i = std::max(lights[i].size.width, lights[i].size.height);
            float height_j = std::max(lights[j].size.width, lights[j].size.height);
            auto height = std::minmax(height_i, height_j);
            // std::cout << "\nheight_i/height_j = " << height.second / height.first << '\n';
            if ((height.second / height.first) > armorParam.armor_max_height_ratio)
                continue;

            float armor_width = std::sqrt((lights[i].center.x - lights[j].center.x) * (lights[i].center.x - lights[j].center.x) + (lights[i].center.y - lights[j].center.y) * (lights[i].center.y - lights[j].center.y));
            float armor_height = (height_i + height_j) / 2;
            // std::cout << "\narmor_width = " << narmor_width;
            // std::cout << "\narmor_height = " << armor_height;
            // std::cout << "\nwidth/height = " << armor_width / armor_height;
            if ((armor_width / armor_height) > armorParam.armor_max_aspect_ratio)
                continue;

            std::vector<cv::Point2f> armor;
            if (lights[i].center.x < lights[j].center.x)
                CalcArmorInfo(armor, lights[i], lights[j]);
            else
                CalcArmorInfo(armor, lights[j], lights[i]);
            armors.emplace_back(armor);
        }
    }
    if (enable_debug) {
        for (auto armor : armors)
            DrawArmor(show_armors, armor, cv::Scalar(0, 255, 0), 2);
        cv::imshow("show_armors", show_armors);
    }
}

void ArmorDetector::CalcArmorInfo(std::vector<cv::Point2f>& armor, const cv::RotatedRect& left_light, const cv::RotatedRect& right_light)
{
    cv::Point2f left_points[4], right_points[4];
    left_light.points(left_points);
    right_light.points(right_points);

    std::sort(left_points, left_points + 4, [](const cv::Point2f& p1, const cv::Point2f& p2) { return p1.y < p2.y; });
    cv::Point2f left_up = (left_points[0].x > left_points[1].x) ? left_points[0] : left_points[1];
    cv::Point2f left_down = (left_points[2].x > left_points[3].x) ? left_points[2] : left_points[3];
    cv::Point2f left_shift = (left_down - left_up) * armorParam.shift_scale;

    std::sort(right_points, right_points + 4, [](const cv::Point2f& p1, const cv::Point2f& p2) { return p1.y < p2.y; });
    cv::Point2f right_up = (right_points[0].x < right_points[1].x) ? right_points[0] : right_points[1];
    cv::Point2f right_down = (right_points[2].x < right_points[3].x) ? right_points[2] : right_points[3];
    cv::Point2f right_shift = (right_down - right_up) * armorParam.shift_scale;

    armor.emplace_back(left_up - left_shift);
    armor.emplace_back(right_up - right_shift);
    armor.emplace_back(right_down + right_shift);
    armor.emplace_back(left_down + left_shift);
}

int ArmorDetector::DetectNumber(const cv::Mat& perspective)
{
    int result[9] = { INT_MAX };
    for (int i = 1; i <= 8; ++i) {
        cv::Mat dst;
        cv::absdiff(perspective, number_template[i], dst);
        result[i] = cv::countNonZero(dst);
        // cv::imshow("abs_diff" + std::to_string(i), dst);
    }
    return std::distance(result, std::min_element(result, result + 8));
}

std::vector<cv::Point2f>& ArmorDetector::SelectFinalArmor(const cv::Mat& src, std::vector<std::vector<cv::Point2f>>& armors)
{
    std::vector<int> armors_level(armors.size());
    for (int i = 0; i < armors.size(); ++i) {
        cv::Mat homography = cv::findHomography(armors[i], armor_points, cv::RANSAC);
        cv::Mat perspective;
        cv::warpPerspective(src, perspective, homography, cv::Size(armorParam.small_armor_width, armorParam.armor_height));
        cv::cvtColor(perspective, perspective, cv::COLOR_BGR2GRAY);
        cv::threshold(perspective, perspective, 100, 255, cv::THRESH_BINARY);
        armors_level[i] = number_level[DetectNumber(perspective)];
        if (enable_debug) {
            // std::cout << "\nnumber = " << min_result_index + 1;
            // std::cout << ", level = " << armors_level[i];
            cv::imshow("perspective", perspective);
        }
    }
    int max_level_index = std::distance(armors_level.begin(), std::max_element(armors_level.begin(), armors_level.end()));
    if (enable_debug) {
        DrawArmor(show_final_armor, armors[max_level_index], cv::Scalar(0, 255, 0), 2);
        cv::imshow("show_final_armor", show_final_armor);
    }
    return armors[max_level_index];
}

void ArmorDetector::CalcControlInfo(const std::vector<cv::Point2f>& armor, cv::Point3f& target)
{
    static float half_camera_width = cameraParam.resolution_width / 2;
    static float half_camera_height = cameraParam.resolution_height / 2;
    static float camera_fx = cameraParam.camera_matrix.at<double>(0, 0);
    static float camera_fy = cameraParam.camera_matrix.at<double>(1, 1);
    constexpr double rad = 57.295779513082320876798154814105;

    cv::Point2f armor_up = (armor[0] + armor[1]) / 2;
    cv::Point2f armor_down = (armor[2] + armor[3]) / 2;
    float armor_height = sqrt(powf(armor_down.x - armor_up.x, 2) + powf(armor_down.y - armor_up.y, 2));
    cv::Point2f armor_center = (armor_up + armor_down) / 2;

    float yaw_offset = atan2(half_camera_width - armor_center.x, camera_fx) * rad;
    float pitch_offset = atan2(half_camera_height - armor_center.y, camera_fy) * rad;
    float distance = camera_fx * armorParam.armor_height / armor_height / 1000;
    target = cv::Point3f(yaw_offset, pitch_offset, distance);
}
