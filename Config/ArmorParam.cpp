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

#include "ArmorParam.h"

ArmorParam armorParam;

void ArmorParam::LoadParam()
{
    cv::FileStorage fs("ArmorParam.yml", cv::FileStorage::READ);
    fs["brightness_thresh"] >> armorParam.brightness_thresh;
    fs["blue_thresh"] >> armorParam.blue_thresh;
    fs["red_thresh"] >> armorParam.red_thresh;
    fs["light_min_area"] >> armorParam.light_min_area;
    fs["armor_max_angle_diff"] >> armorParam.armor_max_angle_diff;
    fs["armor_max_aspect_ratio"] >> armorParam.armor_max_aspect_ratio;
    fs["armor_max_height_ratio"] >> armorParam.armor_max_height_ratio;
    fs["small_armor_width"] >> armorParam.small_armor_width;
    fs["big_armor_width"] >> armorParam.big_armor_width;
    fs["armor_height"] >> armorParam.armor_height;
    fs["shift_scale"] >> armorParam.shift_scale;
    fs["number_thresh"] >> armorParam.number_thresh;
    fs.release();
}
