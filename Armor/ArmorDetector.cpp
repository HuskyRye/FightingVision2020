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
    // TODO: 从串口读取的数据
    enemy_color = EnemyColor::BLUE;
    enable_debug = false;
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

    if (enable_debug) {
        cv::imshow("binary_brightness_image", binary_brightness_image);
        cv::imshow("binary_color_img", binary_color_image);
    }

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
                if (enable_debug)
                    DrawRotatedRect(show_lights, single_light, cv::Scalar(0, 255, 0), 2);
                break;
            }
        }
    }
    if (enable_debug)
        cv::imshow("show_lights", show_lights);
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

void ArmorDetector::DrawRotatedRect(const cv::Mat& image, const cv::RotatedRect& rect, const cv::Scalar& color, int thickness)
{
    cv::Point2f vertices[4];
    rect.points(vertices);
    for (int i = 0; i < 4; i++)
        cv::line(image, vertices[i], vertices[(i + 1) % 4], color, thickness, cv::LINE_AA);
}

void ArmorDetector::DetectArmors(const std::vector<cv::RotatedRect>& lights, std::vector<std::vector<cv::Point2f>>& armors)
{
    for (unsigned int i = 0; i < lights.size(); ++i) {
        for (unsigned int j = i + 1; j < lights.size(); ++j) {
            float angle_i = (lights[i].angle <= -45.0) ? lights[i].angle + 90 : lights[i].angle;
            float angle_j = (lights[j].angle <= -45.0) ? lights[j].angle + 90 : lights[j].angle;
            float angle_diff = std::abs(angle_i - angle_j);
            if (angle_diff > armorParam.armor_max_angle_diff)
                continue;

            double armor_angle = (double)std::atan((lights[i].center.y - lights[j].center.y) / (lights[i].center.x - lights[j].center.x)) * 180 / CV_PI;
            if (std::max(abs(armor_angle - angle_i), abs(armor_angle - angle_j)) > armorParam.armor_max_angle_diff)
                continue;

            float height_i = std::max(lights[i].size.width, lights[i].size.height);
            float height_j = std::max(lights[j].size.width, lights[j].size.height);
            auto height = std::minmax(height_i, height_j);
            if ((height.second / height.first) > armorParam.armor_max_height_ratio)
                continue;

            float armor_width = std::sqrt((lights[i].center.x - lights[j].center.x) * (lights[i].center.x - lights[j].center.x) + (lights[i].center.y - lights[j].center.y) * (lights[i].center.y - lights[j].center.y));
            float armor_height = (height_i + height_j) / 2;
            if ((armor_width / armor_height) > armorParam.armor_max_aspect_ratio)
                continue;

            std::vector<cv::Point2f> armor;
            if (lights[i].center.x < lights[j].center.x)
                CalcArmorInfo(armor, lights[i], lights[j]);
            else
                CalcArmorInfo(armor, lights[j], lights[i]);

            /*
            if (enable_debug) {
                std::cout << "\nangle_i = " << angle_i;
                std::cout << "\nangle_j = " << angle_j;
                std::cout << "\nangle_diff = " << angle_diff;
                std::cout << "\narmor_angle = " << armor_angle;

                std::cout << "\nheight_i/height_j = " << height.second / height.first << '\n';
                std::cout << "\narmor_width = " << narmor_width;
                std::cout << "\narmor_height = " << armor_height;
                std::cout << "\nwidth/height = " << armor_width / armor_height;
            }
            */

            armors.emplace_back(armor);
            if (enable_debug)
                cv::polylines(show_armors, armor, true, cv::Scalar(0, 255, 0), 2);
        }
    }
    if (enable_debug)
        cv::imshow("show_armors", show_armors);
}

void ArmorDetector::CalcArmorInfo(std::vector<cv::Point2f>& armor, cv::RotatedRect left_light, cv::RotatedRect right_light)
{
    cv::Point2f left_points[4], right_points[4];
    left_light.points(left_points);
    right_light.points(right_points);

    std::sort(left_points, left_points + 4, [](const cv::Point2f& p1, const cv::Point2f& p2) { return p1.y < p2.y; });
    cv::Point2f left_up = (left_points[0] + left_points[1]) / 2;
    cv::Point2f left_down = (left_points[2] + left_points[3]) / 2;
    cv::Point2f left_half_light = (left_down - left_up) / 2;

    std::sort(right_points, right_points + 4, [](const cv::Point2f& p1, const cv::Point2f& p2) { return p1.y < p2.y; });
    cv::Point2f right_up = (right_points[0] + right_points[1]) / 2;
    cv::Point2f right_down = (right_points[2] + right_points[3]) / 2;
    cv::Point2f right_half_light = (right_down - right_up) / 2;

    armor.emplace_back(left_up - left_half_light);
    armor.emplace_back(left_down + left_half_light);
    armor.emplace_back(right_up - right_half_light);
    armor.emplace_back(right_down + right_half_light);
}

std::vector<cv::Point2f>& ArmorDetector::SelectFinalArmor(const cv::Mat& src, std::vector<std::vector<cv::Point2f>>& armors)
{
    std::vector<cv::Point2f>& max = *std::max_element(armors.begin(), armors.end(),
        [](const std::vector<cv::Point2f>& armor1, const std::vector<cv::Point2f>& armor2) { return cv::contourArea(armor1) > cv::contourArea(armor2); });
    if (enable_debug) {
        cv::polylines(show_final_armor, max, true, cv::Scalar(0, 255, 0), 2);
        cv::imshow("show_final_armor", show_final_armor);
    }
    return max;
    // TODO: 根据 classifier 的值进行选择
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
    cv::Point2f armor_center = (armor_up + armor_down) / 2;
    float armor_height = sqrt(powf(armor_down.x - armor_up.x, 2) + powf(armor_down.y - armor_up.y, 2)) / 2;
    // TODO: armorParam中的armor_height 需要修改，这里删去/2

    float yaw_offset = atan2(half_camera_width - armor_center.x, camera_fx) * rad;
    float pitch_offset = atan2(half_camera_height - armor_center.y, camera_fy) * rad;
    float distance = camera_fx * armorParam.armor_height / armor_height;
    target = cv::Point3f(yaw_offset, pitch_offset, distance);
}
