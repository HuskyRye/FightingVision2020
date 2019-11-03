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
        show_possible_armors = src.clone();
        show_armors_after_filter = src.clone();
        auto c = cv::waitKey(1);
        if (c == 'a')
            cv::waitKey();
    }
    std::vector<cv::RotatedRect> lights;
    DetectLights(src, lights);

    std::vector<cv::RotatedRect> armors;
    PossibleArmors(lights, armors);

    if (armors.empty())
        return false;
    cv::RotatedRect& final_armor = SelectFinalArmor(armors);
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

void ArmorDetector::PossibleArmors(const std::vector<cv::RotatedRect>& lights, std::vector<cv::RotatedRect>& armors)
{
    for (unsigned int i = 0; i < lights.size(); ++i) {
        for (unsigned int j = i + 1; j < lights.size(); ++j) {
            float angle_i = (lights[i].angle <= -45.0) ? lights[i].angle + 90 : lights[i].angle;
            float angle_j = (lights[j].angle <= -45.0) ? lights[j].angle + 90 : lights[j].angle;
            float angle_diff = std::abs(angle_i - angle_j);
            if (angle_diff > armorParam.armor_max_angle_diff)
                continue;

            double center_angle = (double)std::atan((lights[i].center.y - lights[j].center.y) / (lights[i].center.x - lights[j].center.x)) * 180 / CV_PI;
            if (std::max(abs(center_angle - angle_i), abs(center_angle - angle_j)) > armorParam.armor_max_angle_diff)
                continue;

            float height_i = std::max(lights[i].size.width, lights[i].size.height);
            float height_j = std::max(lights[j].size.width, lights[j].size.height);
            auto height = std::minmax(height_i, height_j);
            if ((height.second / height.first) > armorParam.armor_max_height_ratio)
                continue;

            cv::RotatedRect rect;
            rect.center = (lights[i].center + lights[j].center) / 2;
            rect.angle = static_cast<float>(center_angle);
            rect.size.width = std::sqrt((lights[i].center.x - lights[j].center.x) * (lights[i].center.x - lights[j].center.x) + (lights[i].center.y - lights[j].center.y) * (lights[i].center.y - lights[j].center.y));
            rect.size.height = std::max<float>(height_i, height_j);
            if ((rect.size.width / rect.size.height) > armorParam.armor_max_aspect_ratio)
                continue;

            /*
            if (enable_debug) {
                std::cout << "\ncenter_angle = " << center_angle;
                std::cout << "\nlights1_angle = " << lights1_angle;
                std::cout << "\nlights2_angle = " << lights2_angle;
                std::cout << "\nangle_diff = " << angle_diff;
                std::cout << "\nrect_angle = " << rect.angle;
                std::cout << "\nrect_width = " << rect.size.width;
                std::cout << "\nrect_height = " << rect.size.height;
                std::cout << "\nwidth/height = " << rect.size.width / rect.size.height;
                std::cout << "\nedge1.first = " << edge1.first;
                std::cout << "\nedge2.first = " << edge2.first;
                std::cout << "\nedge1.second = " << edge1.second;
                std::cout << "\nedge2.second = " << edge2.second;
                std::cout << "\nheight_i/height_j = " << height.second / height.first << '\n';
                std::cout << "\nrect.area = " << rect.size.area();
            }
            */

            armors.emplace_back(rect);
            if (enable_debug)
                DrawRotatedRect(show_possible_armors, rect, cv::Scalar(0, 255, 0), 2);
        }
    }
    if (enable_debug)
        cv::imshow("show_possible_armors", show_possible_armors);
}

cv::RotatedRect& ArmorDetector::SelectFinalArmor(std::vector<cv::RotatedRect>& armors)
{
    cv::RotatedRect max = *std::max_element(armors.begin(), armors.end(),
        [](const cv::RotatedRect& r1, const cv::RotatedRect& r2) { return r1.size.area() > r2.size.area(); });
    if (enable_debug) {
        DrawRotatedRect(show_armors_after_filter, max, cv::Scalar(0, 255, 0), 2);
        cv::imshow("show_armors_after_filter", show_armors_after_filter);
    }
    return max;
    // TODO: 根据 classifier 的值进行选择
}

void ArmorDetector::CalcControlInfo(const cv::RotatedRect& armor, cv::Point3f& target)
{
    static float half_camera_width = cameraParam.resolution_width / 2;
    static float half_camera_height = cameraParam.resolution_height / 2;
    static float camera_fx = cameraParam.camera_matrix.at<double>(0, 0);
    static float camera_fy = cameraParam.camera_matrix.at<double>(1, 1);
    constexpr double rad = 57.295779513082320876798154814105;
    float yaw_offset = atan2(half_camera_width - armor.center.x, camera_fx) * rad;
    float pitch_offset = atan2(half_camera_height - armor.center.y, camera_fy) * rad;
    float distance = camera_fx * armorParam.armor_height / armor.size.height;
    target = cv::Point3f(yaw_offset, pitch_offset, distance);
}
