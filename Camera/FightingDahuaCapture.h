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

#include "FightingCapture.h"

#include "GenICam/Camera.h"
#include "GenICam/StreamSource.h"
#include "GenICam/System.h"
#include "ImageConvert.h"

#include "CircularQueue.h"
#include "../Config/FightingParam.h"

using namespace Dahua::GenICam;

class FightingDahuaCapture : public FightingCapture {
public:
    FightingDahuaCapture();
    ~FightingDahuaCapture();

    bool init() final;
    bool read(cv::Mat& image) final;

    friend void grabbingCallback(const CFrame& pFrame, const void* pUser);

private:
    ICameraPtr cameraSptr;
    IStreamSourcePtr streamPtr;

    CircularQueue<cv::Mat, 5> imageQueue;
};

void grabbingCallback(const CFrame& pFrame, const void* pUser);
