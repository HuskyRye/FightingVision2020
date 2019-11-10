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
#include "FightingParam.h"

struct ArmorParam : FightingParam{
    // DetectLights
    float brightness_thresh;
    float blue_thresh, red_thresh;
    float light_min_area;

    // DetectArmors
    float armor_max_angle_diff;
    float armor_max_aspect_ratio;
    float armor_max_height_ratio;

    // ArmorSize
    float small_armor_width;
    float big_armor_width;
    float armor_height;
    float shift_scale;

    // DetectNumber
    float number_thresh;

    void LoadParam() final;
};

extern ArmorParam armorParam;
