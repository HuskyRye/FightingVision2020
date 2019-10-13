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

#include "FightingCameraCapture.h"

using namespace Dahua::GenICam;
using namespace Dahua::Infra;

FightingCameraCapture::FightingCameraCapture()
{
}

FightingCameraCapture::~FightingCameraCapture()
{
    if (streamPtr && streamPtr->isGrabbing() && !streamPtr->stopGrabbing())
        printf("StopGrabbing fail. \n");
    if (cameraSptr && cameraSptr->isConnected() && !cameraSptr->disConnect())
        printf("DisConnect camera fail.\n");
}

bool FightingCameraCapture::init()
{
    /* 发现设备 */
    CSystem& systemObj = CSystem::getInstance();
    TVector<ICameraPtr> vCameraPtrList;
    bool isDiscoverySuccess = systemObj.discovery(vCameraPtrList);
    if (!isDiscoverySuccess || vCameraPtrList.size() == 0) {
        printf("Discovery device fail or no device founded.\n");
        return false;
    }

    /* 连接相机 */
    cameraSptr = vCameraPtrList[0];
    if (!cameraSptr->connect()) {
        printf("Connect cameral fail.\n");
        return false;
    }

    /* 设置相机 */
    // 构造对象
    IImageFormatControlPtr sptrFormatControl = systemObj.createImageFormatControl(cameraSptr);
    IAcquisitionControlPtr sptrAcquisitionControl = systemObj.createAcquisitionControl(cameraSptr);
    IISPControlPtr sptrISPControl = systemObj.createISPControl(cameraSptr);
    IAnalogControlPtr sptrAnalogControl = systemObj.createAnalogControl(cameraSptr);

    // 获取属性节点
    CIntNode nodeWidth = sptrFormatControl->width();
    CIntNode nodeHeight = sptrFormatControl->height();
    CEnumNode nodeExposureAuto = sptrAcquisitionControl->exposureAuto();
    CDoubleNode nodeExposureTime = sptrAcquisitionControl->exposureTime();
    CDoubleNode nodeFrameRate = sptrAcquisitionControl->acquisitionFrameRate();
    CIntNode nodeBrightness = sptrISPControl->brightness();
    CEnumNode nodeBalanceRatioSection = sptrAnalogControl->balanceRatioSelector();
    CDoubleNode nodeBalanceRatio = sptrAnalogControl->balanceRatio();

    // 设置属性值
    nodeWidth.setValue(cameraParam.resolution_width);
    nodeHeight.setValue(cameraParam.resolution_height);
    nodeExposureAuto.setValue(cameraParam.auto_exposure);
    nodeExposureTime.setValue(cameraParam.exposure_time);
    nodeFrameRate.setValue(cameraParam.frame_rate);
    nodeBrightness.setValue(cameraParam.brightness);
    nodeBalanceRatioSection.setValue(0);
    nodeBalanceRatio.setValue(cameraParam.balance_ratio_red);
    nodeBalanceRatioSection.setValue(1);
    nodeBalanceRatio.setValue(cameraParam.balance_ratio_green);
    nodeBalanceRatioSection.setValue(2);
    nodeBalanceRatio.setValue(cameraParam.balance_ratio_blue);

    /* 创建流对象 */
    streamPtr = systemObj.createStreamSource(cameraSptr);
    if (!streamPtr) {
        printf("Create stream obj fail.\n");
        return false;
    }

    /* 注册回调函数 */
    streamPtr->attachGrabbingEx(grabbingCallback, this);

    /*开始采集图像 */
    if (!streamPtr->startGrabbing()) {
        printf("StartGrabbing fail.\n");
        return false;
    }
    return true;
}

bool FightingCameraCapture::read(cv::Mat& image)
{
    double start = static_cast<double>(cv::getTickCount());
    while (imageQueue.empty()) {
        double time = ((double)cv::getTickCount() - start) / cv::getTickFrequency();
        if (time > 0.5)
            return false;
    }
    return imageQueue.pop(image);
}

void grabbingCallback(const CFrame& pFrame, const void* pUser)
{
    int nBGRBufferSize = pFrame.getImageWidth() * pFrame.getImageHeight() * 3;
    uint8_t* pBGRbuffer = new uint8_t[nBGRBufferSize];

    // 设置转换配置参数
    IMGCNV_SOpenParam openParam;
    openParam.width = pFrame.getImageWidth();
    openParam.height = pFrame.getImageHeight();
    openParam.paddingX = pFrame.getImagePadddingX();
    openParam.paddingY = pFrame.getImagePadddingY();
    openParam.dataSize = pFrame.getImageSize();
    openParam.pixelForamt = pFrame.getImagePixelFormat();

    // 转换为 BGR24
    if (IMGCNV_ConvertToBGR24((unsigned char*)pFrame.getImage(), &openParam, pBGRbuffer, &nBGRBufferSize) != IMGCNV_SUCCESS) {
        delete[] pBGRbuffer;
        printf("Convert fail. \n");
        return;
    }

    cv::Mat image = cv::Mat(pFrame.getImageHeight(), pFrame.getImageWidth(), CV_8UC3, (uint8_t*)pBGRbuffer);
    FightingCameraCapture* capture = (FightingCameraCapture*)pUser;
    capture->imageQueue.push(image.clone());
    delete[] pBGRbuffer;
}
