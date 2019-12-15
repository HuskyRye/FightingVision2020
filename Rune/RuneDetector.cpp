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

#include "RuneDetector.h"

RuneDetector::RuneDetector()
{
    // TODO: load from Serial
    enable_debug = true;
}

RuneDetector::~RuneDetector()
{
}

bool RuneDetector::DetectRune(cv::Mat& src, cv::Point3f& target)
{
    if (enable_debug) {
        show_possible_armors = src.clone();
        show_final_armor = src.clone();
        auto c = cv::waitKey(1);
        if (c == 'a')
            cv::waitKey();
    }

    cv::Mat gray_image;
    cv::cvtColor(src, gray_image, cv::COLOR_BGR2GRAY);

    cv::Mat binary_brightness_image;
    cv::threshold(gray_image, binary_brightness_image, runeParam.brightness_thresh, 255, cv::THRESH_BINARY);
    if (enable_debug)
        cv::imshow("binary_brightness_image", binary_brightness_image);

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
    return true;
}

void RuneDetector::PossibleArmors(const std::vector<std::vector<cv::Point>>& contours, const std::vector<cv::Vec4i>& hierarchy, std::vector<cv::RotatedRect>& armors)
{
    int index = 0;
    do {
        int child_index = hierarchy[index][2];
        if (child_index != -1 && hierarchy[child_index][0] == -1) {
            cv::RotatedRect possible_armor = cv::minAreaRect(contours[child_index]);
            if (possible_armor.size.area() > runeParam.armor_min_area) {
                armors.emplace_back(possible_armor);
                if (enable_debug)
                    DrawRotatedRect(show_possible_armors, possible_armor, cv::Scalar(0, 255, 0), 2);
            }
        }
    } while (hierarchy[index][0] != -1 && (index = hierarchy[index][0]));
    if (enable_debug)
        cv::imshow("show_possible_armors", show_possible_armors);
}

cv::RotatedRect& RuneDetector::SelectFinalArmor(std::vector<cv::RotatedRect>& armors)
{
    std::sort(armors.begin(), armors.end(),
        [](const cv::RotatedRect& r1, const cv::RotatedRect& r2) { return r1.size.area() > r2.size.area(); });
    if (enable_debug) {
        DrawRotatedRect(show_final_armor, armors[0], cv::Scalar(0, 255, 0), 2);
        cv::imshow("show_final_armor", show_final_armor);
        std::cout << "\narea = " << armors[0].size.area();
    }
    return armors[0];
}

void RuneDetector::CalcControlInfo(const cv::RotatedRect& armor, cv::Point3f& target)
{
    static float half_camera_width = cameraParam.resolution_width / 2;
    static float half_camera_height = cameraParam.resolution_height / 2;
    static float camera_fx = cameraParam.fx;
    static float camera_fy = cameraParam.fy;
    constexpr double rad = 57.295779513082320876798154814105;
    float yaw_offset = atan2(half_camera_width - armor.center.x, camera_fx) * rad;
    float pitch_offset = atan2(half_camera_height - armor.center.y, camera_fy) * rad;
    float distance = camera_fx * runeParam.armor_height / armor.size.height;
    target = cv::Point3f(yaw_offset, pitch_offset, distance);
}

void RuneDetector::DrawRotatedRect(const cv::Mat& image, const cv::RotatedRect& rect, const cv::Scalar& color, int thickness)
{
    cv::Point2f vertices[4];
    rect.points(vertices);
    for (int i = 0; i < 4; i++)
        cv::line(image, vertices[i], vertices[(i + 1) % 4], color, thickness, cv::LINE_AA);
}
