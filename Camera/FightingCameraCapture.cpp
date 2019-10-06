
#include "FightingCameraCapture.h"

using namespace Dahua::GenICam;
using namespace Dahua::Infra;

FightingCameraCapture::FightingCameraCapture()
{
    // TODO: 从文件读取相机配置
    width = 1280;
    height = 1024;
    exposure_auto = false;
    exposure_time = 9000;
    frame_rate = 210;
    brightness = 50;
    gamma = 1;
    balanceRatio_Red = 1.5;
    balanceRatio_Green = 1;
    balanceRatio_Blue = 1.5;
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
    CDoubleNode nodeGamma = sptrAnalogControl->gamma();
    CEnumNode nodeBalanceRatioSection = sptrAnalogControl->balanceRatioSelector();
    CDoubleNode nodeBalanceRatio = sptrAnalogControl->balanceRatio();

    // 设置属性值
    nodeWidth.setValue(width);
    nodeHeight.setValue(height);
    nodeExposureAuto.setValue(exposure_auto);
    nodeExposureTime.setValue(exposure_time);
    nodeFrameRate.setValue(frame_rate);
    nodeBrightness.setValue(brightness);
    nodeGamma.setValue(gamma);
    nodeBalanceRatioSection.setValue(0);
    nodeBalanceRatio.setValue(balanceRatio_Red);
    nodeBalanceRatioSection.setValue(1);
    nodeBalanceRatio.setValue(balanceRatio_Green);
    nodeBalanceRatioSection.setValue(2);
    nodeBalanceRatio.setValue(balanceRatio_Blue);

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
    /* timing
    double start = static_cast<double>(cv::getTickCount());
    double time = ((double)cv::getTickCount() - start) / cv::getTickFrequency();
    std::cout << "time: " << time << "s\n" << std::endl;
    */

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
    IMGCNV_EErr status = IMGCNV_ConvertToBGR24((unsigned char*)pFrame.getImage(), &openParam, pBGRbuffer, &nBGRBufferSize);
    if (IMGCNV_SUCCESS != status) {
        delete[] pBGRbuffer;
        printf("Convert fail. \n");
        return;
    }

    cv::Mat image = cv::Mat(pFrame.getImageHeight(), pFrame.getImageWidth(), CV_8UC3, (uint8_t*)pBGRbuffer);
    FightingCameraCapture* capture = (FightingCameraCapture*)pUser;
    capture->imageQueue.push(image.clone());
    delete[] pBGRbuffer;
}
