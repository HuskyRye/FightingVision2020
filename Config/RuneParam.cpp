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

#include "RuneParam.h"

RuneParam runeParam;

void RuneParam::LoadParam()
{
    cv::FileStorage fs("RuneParam.yml", cv::FileStorage::READ);
    fs["brightness_thresh"] >> runeParam.brightness_thresh;
    fs["armor_min_area"] >> runeParam.armor_min_area;
    fs["big_armor_width"] >> runeParam.armor_width;
    fs["armor_height"] >> runeParam.armor_height;
    fs.release();
}
