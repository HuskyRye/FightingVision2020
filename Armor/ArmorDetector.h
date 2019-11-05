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

#pragma once

#include "../Config/FightingParam.h"
#include "opencv2/opencv.hpp"

class ArmorDetector {
public:
    ArmorDetector();
    ~ArmorDetector();
    bool DetectArmor(cv::Mat& src, cv::Point3f& target);

private:
    bool enable_debug;
    enum class EnemyColor { RED,
        BLUE };
    EnemyColor enemy_color;

    cv::Mat show_lights;
    cv::Mat show_possible_armors;
    cv::Mat show_armors_after_filter;

    void DetectLights(const cv::Mat& src, std::vector<cv::RotatedRect>& lights);
    cv::Mat DistillationColor(const cv::Mat& src);
    void DrawRotatedRect(const cv::Mat& image, const cv::RotatedRect& rect, const cv::Scalar& color, int thickness);

    void DetectArmors(const std::vector<cv::RotatedRect>& lights, std::vector<std::vector<cv::Point2f>>& armors);
    void CalcArmorInfo(std::vector<cv::Point2f>& armor, cv::RotatedRect left_light, cv::RotatedRect right_light);
    std::vector<cv::Point2f>& SelectFinalArmor(std::vector<std::vector<cv::Point2f>>& armors);

    void CalcControlInfo(const std::vector<cv::Point2f>& armor, cv::Point3f& target);
};
