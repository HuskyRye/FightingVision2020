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
    : state(State::SEARCHING_STATE)
{
    // TODO: 从串口读取的数据
    enemy_color = EnemyColor::BLUE;
    enable_debug = true;

    // DetectLights
    brightness_thresh = 200;
    blue_thresh = 50;
    red_thresh = 50;
    light_min_area = 1;

    // PossibleArmors
    armor_max_angle_diff = 15;
    armor_max_aspect_ratio = 6;
    armor_max_height_ratio = 2;
    armor_min_area = 200;

    small_armor_width = 0.122;
    big_armor_width = 0.225;
    armor_height = 0.045;
}

ArmorDetector::~ArmorDetector()
{
}

bool ArmorDetector::DetectArmor(cv::Mat& src, cv::Point3f& target)
{
    if (enable_debug) {
        detect_lights = src.clone();
        armors_before_filter = src.clone();
        armors_after_filter = src.clone();
        auto c = cv::waitKey(1);
        if (c == 'a')
            cv::waitKey();
    }
    std::vector<cv::RotatedRect> lights;
    DetectLights(src, lights);

    std::vector<ArmorInfo> armors;
    PossibleArmors(lights, armors);

    FilterArmors(armors);
    if (armors.empty())
        return false;
    ArmorInfo& final_armor = SelectFinalArmor(armors);
    CalcControlInfo(final_armor, target);
    return true;
}

void ArmorDetector::DetectLights(const cv::Mat& src, std::vector<cv::RotatedRect>& lights)
{
    cv::Mat distillation = DistillationColor(src);

    cv::Mat gray_image;
    cv::cvtColor(src, gray_image, cv::COLOR_BGR2GRAY);

    cv::Mat binary_brightness_image;
    cv::threshold(gray_image, binary_brightness_image, brightness_thresh, 255, cv::THRESH_BINARY);

    cv::Mat binary_color_image;
    float thresh = (enemy_color == EnemyColor::BLUE) ? blue_thresh : red_thresh;
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
                if (bounding_rect.height < bounding_rect.width || single_light.size.area() < light_min_area)
                    break;
                lights.emplace_back(single_light);
                if (enable_debug)
                    DrawRotatedRect(detect_lights, single_light, cv::Scalar(0, 255, 0), 2);
                break;
            }
        }
    }
    if (enable_debug)
        cv::imshow("lights_before_filter", detect_lights);
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

void ArmorDetector::PossibleArmors(const std::vector<cv::RotatedRect>& lights, std::vector<ArmorInfo>& armors)
{
    for (unsigned int i = 0; i < lights.size(); ++i) {
        for (unsigned int j = i + 1; j < lights.size(); ++j) {
            cv::RotatedRect light1 = lights[i];
            cv::RotatedRect light2 = lights[j];
            auto edge1 = std::minmax(light1.size.width, light1.size.height);
            auto edge2 = std::minmax(light2.size.width, light2.size.height);
            float light1_angle = (light1.angle <= -45.0) ? light1.angle + 90 : light1.angle;
            float light2_angle = (light2.angle <= -45.0) ? light2.angle + 90 : light2.angle;
            float angle_diff = std::abs(light1_angle - light2_angle);
            double center_angle = (double)std::atan((lights[i].center.y - lights[j].center.y) / (lights[i].center.x - lights[j].center.x)) * 180 / CV_PI;
            cv::RotatedRect rect;
            rect.angle = static_cast<float>(center_angle);
            rect.center = (light1.center + light2.center) / 2;
            rect.size.width = std::sqrt((lights[i].center.x - lights[j].center.x) * (lights[i].center.x - lights[j].center.x) + (lights[i].center.y - lights[j].center.y) * (lights[i].center.y - lights[j].center.y));
            rect.size.height = std::max<float>(edge1.second, edge2.second);
            auto height = std::minmax(edge1.second, edge2.second);
            /*
            if (enable_debug) {
                std::cout << "\ncenter_angle = " << center_angle;
                std::cout << "\nlight1_angle = " << light1_angle;
                std::cout << "\nlight2_angle = " << light2_angle;
                std::cout << "\nangle_diff = " << angle_diff;
                std::cout << "\nrect_angle = " << rect.angle;
                std::cout << "\nrect_width = " << rect.size.width;
                std::cout << "\nrect_height = " << rect.size.height;
                std::cout << "\nwidth/height = " << rect.size.width / rect.size.height;
                std::cout << "\nedge1.first = " << edge1.first;
                std::cout << "\nedge2.first = " << edge2.first;
                std::cout << "\nedge1.second = " << edge1.second;
                std::cout << "\nedge2.second = " << edge2.second;
                std::cout << "\nheight1/height2 = " << height.second / height.first << '\n';
                std::cout << "\nrect.area = " << rect.size.area();
            }
            */
            if (angle_diff < armor_max_angle_diff
                && std::max(abs(center_angle - light1_angle), abs(center_angle - light2_angle)) < armor_max_angle_diff
                && (rect.size.width / rect.size.height) < armor_max_aspect_ratio
                && (height.second / height.first) < armor_max_height_ratio
                && rect.size.area() > armor_min_area) {
                if (enable_debug)
                    DrawRotatedRect(armors_before_filter, rect, cv::Scalar(0, 255, 0), 2);
                armors.emplace_back(ArmorInfo(rect));
            }
        }
    }
    if (enable_debug)
        cv::imshow("armors_before_filter", armors_before_filter);
}

void ArmorDetector::FilterArmors(std::vector<ArmorInfo>& armors)
{
    // TODO: Non-Maximum Suppression
    // TODO: Classifier
}

ArmorInfo& ArmorDetector::SelectFinalArmor(std::vector<ArmorInfo>& armors)
{
    std::sort(armors.begin(), armors.end(),
        [](const ArmorInfo& p1, const ArmorInfo& p2) { return p1.rect.size.area() > p2.rect.size.area(); });
    return armors[0];
    // TODO: 根据 classifier 的值进行选择
}

void ArmorDetector::CalcControlInfo(const ArmorInfo& armor, cv::Point3f& target)
{
    static float half_camera_width = cameraInfo.resolution_width / 2;
    static float half_camera_height = cameraInfo.resolution_height / 2;
    static float camera_fx = cameraInfo.camera_matrix.at<double>(0, 0);
    static float camera_fy = cameraInfo.camera_matrix.at<double>(1, 1);
    constexpr double rad = 57.295779513082320876798154814105;
    float yaw_offset = atan2(half_camera_width - armor.rect.center.x, camera_fx) * rad;
    float pitch_offset = atan2(half_camera_height - armor.rect.center.y, camera_fy) * rad;
    float distance = camera_fx * armor_height / armor.rect.size.height;
    target = cv::Point3f(yaw_offset, pitch_offset, distance);
}
