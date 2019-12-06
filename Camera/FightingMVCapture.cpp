#include "FightingMVCapture.h"
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

#include "FightingMVCapture.h"

FightingMVCapture::FightingMVCapture()
{
}

FightingMVCapture::~FightingMVCapture()
{
    CameraUnInit(m_hCamera);
}

bool FightingMVCapture::init()
{
    // 初始化SDK
    CameraSdkInit(1);

    // 枚举设备
    tSdkCameraDevInfo sCameraList[1];
    int iCameraNums = 1;
    if (CameraEnumerateDevice(sCameraList, &iCameraNums) != CAMERA_STATUS_SUCCESS || iCameraNums == 0) {
        printf("No camera was found!");
        return false;
    }

    // 初始化设备
    if (CameraSdkStatus status = CameraInit(&sCameraList[0], -1, -1, &m_hCamera) != CAMERA_STATUS_SUCCESS) {
        printf("Failed to init the camera! Error code is %d", status);
        printf(CameraGetErrorString(status));
        return false;
    }

    // 注册回调函数
    CameraSetCallbackFunction(m_hCamera, grabbingCallback, this, nullptr);

    // 开始采集图像
    CameraPlay(m_hCamera);
    return true;
}

bool FightingMVCapture::read(cv::Mat& image)
{
    double start = static_cast<double>(cv::getTickCount());
    while (circular_queue.empty()) {
        double time = ((double)cv::getTickCount() - start) / cv::getTickFrequency();
        if (time > 0.5)
            return false;
    }
    return circular_queue.pop(image);
}

void grabbingCallback(CameraHandle hCamera, BYTE* pFrameBuffer, tSdkFrameHead* pFrameHead, PVOID pContext)
{
    FightingMVCapture* capture = (FightingMVCapture*)pContext;
    CameraImageProcess(hCamera, pFrameBuffer, capture->m_pFrameBuffer, pFrameHead);
    cv::Mat matImage(cv::Size(pFrameHead->iWidth, pFrameHead->iHeight), CV_8UC3, capture->m_pFrameBuffer);
    capture->circular_queue.push(matImage.clone());
}