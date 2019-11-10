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
    cv::Mat show_armors;
    cv::Mat show_final_armor;

    cv::Mat DistillationColor(const cv::Mat& src);
    inline void DrawRotatedRect(const cv::Mat& image, const cv::RotatedRect& rect, const cv::Scalar& color, int thickness);
    void DetectLights(const cv::Mat& src, std::vector<cv::RotatedRect>& lights);

    inline void DrawArmor(const cv::Mat& image, const std::vector<cv::Point2f>& armor, const cv::Scalar& color, int thickness);
    void PossibleArmors(const std::vector<cv::RotatedRect>& lights, std::vector<std::vector<cv::Point2f>>& armors);
    void CalcArmorInfo(std::vector<cv::Point2f>& armor, const cv::RotatedRect& left_light, const cv::RotatedRect& right_light);

    const std::vector<cv::Point2f> armor_points { cv::Point2f(0, 0), cv::Point2f(armorParam.small_armor_width, 0),
        cv::Point2f(armorParam.small_armor_width, armorParam.armor_height), cv::Point2f(0, armorParam.armor_height) };
    std::vector<cv::Mat> number_template;
    int DetectNumber(const cv::Mat& perspective);
    /* 基地8(level_5) > 哨兵7(level_4) > 英雄1(level_3) > 步兵345(level_2) > 工程2(level_1) > 无0(level_0)) */
    const int number_level[9] = { 0, 3, 1, 2, 2, 2, 0, 4, 5 };
    std::vector<cv::Point2f>& SelectFinalArmor(const cv::Mat& src, std::vector<std::vector<cv::Point2f>>& armors);
    void CalcControlInfo(const std::vector<cv::Point2f>& armor, cv::Point3f& target);
};
