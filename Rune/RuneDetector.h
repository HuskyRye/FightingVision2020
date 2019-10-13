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

#include "opencv2/opencv.hpp"
#include "../Camera/FightingCameraParam.h"
#include "RuneParam.h"

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
