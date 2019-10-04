#include "ArmorDetector.h"

ArmorDetector::ArmorDetector()
    : state(State::SEARCHING_STATE)
{
    // TODO: 从串口数据加载颜色
    enemy_color = EnemyColor::BLUE;

    // TODO: 从文件读取参数
    // DEBUG
    enable_debug = true;

    // DetectLights
    color_thresh = 230;
    blue_thresh = 50;
    red_thresh = 50;
    light_min_area = 50;

    // PossibleArmors
    armor_max_angle_diff = 15;
    armor_max_aspect_ratio = 6;
    armor_max_height_ratio = 2;
    armor_min_area = 200;

    // Camera
    // double camera_m[9] = { 1745.86173683508, 0, 662.18119817513, 0, 1747.84123649139, 453.89956107988, 0, 0, 1 };
    // double camera_m[9] = { 2666.7, 0, 310.7, 0, 2282.5, 284.1, 0, 0, 1 };
    double camera_m[9] = { 2092, 0, 117, 0, 1776, 213, 0, 0, 1 };
    camera_matrix = cv::Mat(3, 3, CV_64F, camera_m);
    // double camera_d[5] = { 0.00779612314, 0.08290683074, -0.00177829643, 0.00336206233, -1.69370174590 };
    // double camera_d[5] = { -1.29, 166.25, -0.04, -0.01, -3.37 };
    double camera_d[5] = { 1.42, 90.49, 0.04, -0.44, -147.77 };
    distortion_coeffs = cv::Mat(5, 1, CV_64F, camera_d);

    small_armor_width = 128;
    small_armor_height = 45;
    big_armor_width = 225;
    big_armor_height = 45;

    float half_small_armor_width = small_armor_width / 2;
    float half_small_armor_height = small_armor_height / 2;
    float half_big_armor_width = big_armor_width / 2;
    float half_big_armor_height = big_armor_height / 2;

    small_armor_points.emplace_back(cv::Point3f(-half_small_armor_width, half_small_armor_height, 0.0)); // left down
    small_armor_points.emplace_back(cv::Point3f(half_small_armor_width, half_small_armor_height, 0.0)); // right down
    small_armor_points.emplace_back(cv::Point3f(half_small_armor_width, -half_small_armor_height, 0.0)); // right up
    small_armor_points.emplace_back(cv::Point3f(-half_small_armor_width, -half_small_armor_height, 0.0)); // left up

    big_armor_points.emplace_back(cv::Point3f(-half_big_armor_width, half_big_armor_height, 0.0)); // left down
    big_armor_points.emplace_back(cv::Point3f(half_big_armor_width, half_big_armor_height, 0.0)); // right down
    big_armor_points.emplace_back(cv::Point3f(half_big_armor_width, -half_big_armor_height, 0.0)); // right up
    big_armor_points.emplace_back(cv::Point3f(-half_big_armor_width, -half_big_armor_height, 0.0)); // left up
    
}

ArmorDetector::~ArmorDetector()
{
}

void ArmorDetector::DetectArmor(cv::Mat& src, cv::Point3f& target_3d)
{
    switch (state) {
    case ArmorDetector::State::SEARCHING_STATE:
        SearchArmor(src, target_3d);
        break;
    case ArmorDetector::State::TRACKING_STATE:
        // 跟踪
        break;
    default:
        break;
    }
}

bool ArmorDetector::SearchArmor(const cv::Mat& src, cv::Point3f& target_3d)
{
    if (enable_debug) {
        detect_lights = src.clone();
        armors_before_filter = src.clone();
        armors_after_filter = src.clone();
    }
    double start = static_cast<double>(cv::getTickCount());

    std::vector<cv::RotatedRect> lights;
    DetectLights(src, lights);

    std::vector<ArmorInfo> armors;
    PossibleArmors(lights, armors);

    FilterArmors(armors);
    if (armors.empty())
        return false;
    ArmorInfo final_armor = SelectFinalArmor(armors);
    CalcControlInfo(final_armor, target_3d);
    // TODO: Create Tracker?
    double time = ((double)cv::getTickCount() - start) / cv::getTickFrequency();
    std::cout << "\ntime: " << time;
    return true;
}

void ArmorDetector::DetectLights(const cv::Mat& src, std::vector<cv::RotatedRect>& lights)
{
    cv::Mat light = DistillationColor(src);

    cv::Mat gray_image;
    cv::cvtColor(src, gray_image, cv::COLOR_BGR2GRAY);

    cv::Mat binary_brightness_image;
    cv::threshold(gray_image, binary_brightness_image, color_thresh, 255, cv::THRESH_BINARY);

    cv::Mat binary_color_image;
    float thresh = (enemy_color == EnemyColor::BLUE) ? blue_thresh : red_thresh;
    cv::threshold(light, binary_color_image, thresh, 255, cv::THRESH_BINARY);

    if (enable_debug) {
        cv::imshow("binary_brightness_image", binary_brightness_image);
        cv::imshow("binary_color_img", binary_color_image);
    }

    std::vector<std::vector<cv::Point>> contours_color;
    cv::findContours(binary_color_image, contours_color, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<std::vector<cv::Point>> contours_brightness;
    cv::findContours(binary_brightness_image, contours_brightness, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    lights.reserve(contours_color.size());
    for (unsigned int i = 0; i < contours_brightness.size(); ++i) {
        for (unsigned int j = 0; j < contours_color.size(); ++j) {
            if (cv::pointPolygonTest(contours_color[j], contours_brightness[i][0], false) >= 0.0) {
                cv::RotatedRect single_light = cv::minAreaRect(contours_brightness[i]);
                /* Filter Lights */
                cv::Rect bounding_rect = single_light.boundingRect();
                if (bounding_rect.height < bounding_rect.width || single_light.size.area() < light_min_area)
                    break;
                lights.push_back(single_light);
                if (enable_debug)
                    DrawRotatedRect(detect_lights, single_light, cv::Scalar(0, 255, 0), 2);
                break;
            }
        }
    }
    if (enable_debug) {
        cv::imshow("lights_before_filter", detect_lights);
        /*
        auto c = cv::waitKey(1);
        if (c == 'a')
            cv::waitKey(0);
        */
    }
}

cv::Mat ArmorDetector::DistillationColor(const cv::Mat& src)
{
    std::vector<cv::Mat> bgr;
    cv::split(src, bgr);
    cv::Mat result;
    if (EnemyColor::BLUE == enemy_color)
        cv::subtract(bgr[0], bgr[2], result);
    else
        cv::subtract(bgr[2], bgr[0], result);
    return result;
}

void ArmorDetector::DrawRotatedRect(const cv::Mat& image, const cv::RotatedRect& rect, const cv::Scalar& color, int thickness)
{
    cv::Point2f vertices[4];
    rect.points(vertices);
    for (int i = 0; i < 4; i++)
        cv::line(image, vertices[i], vertices[(i + 1) % 4], color, thickness, cv::LINE_AA);
}

void ArmorDetector::PossibleArmors(const std::vector<cv::RotatedRect>& lights, std::vector<ArmorInfo>& armors)
{
    for (unsigned int i = 0; i < lights.size(); ++i) {
        for (unsigned int j = i + 1; j < lights.size(); ++j) {
            cv::RotatedRect light1 = lights[i];
            cv::RotatedRect light2 = lights[j];
            auto edge1 = std::minmax(light1.size.width, light1.size.height);
            auto edge2 = std::minmax(light2.size.width, light2.size.height);
            float light1_angle = (light1.angle <= -45.0) ? light1.angle + 90 : light1.angle;
            float light2_angle = (light2.angle <= -45.0) ? light2.angle + 90 : light2.angle;
            float angle_diff = std::abs(light1_angle - light2_angle);
            double center_angle = (double)std::atan((lights[i].center.y - lights[j].center.y) / (lights[i].center.x - lights[j].center.x)) * 180 / CV_PI;
            cv::RotatedRect rect;
            rect.angle = static_cast<float>(center_angle);
            rect.center = (light1.center + light2.center) / 2;
            rect.size.width = std::sqrt((lights[i].center.x - lights[j].center.x) * (lights[i].center.x - lights[j].center.x) + (lights[i].center.y - lights[j].center.y) * (lights[i].center.y - lights[j].center.y));
            rect.size.height = std::max<float>(edge1.second, edge2.second);
            auto height = std::minmax(edge1.second, edge2.second);
            /*
            if (enable_debug) {
                std::cout << "\ncenter_angle = " << center_angle;
                std::cout << "\nlight1_angle = " << light1_angle;
                std::cout << "\nlight2_angle = " << light2_angle;
                std::cout << "\nangle_diff = " << angle_diff;
                std::cout << "\nrect_angle = " << rect.angle;
                std::cout << "\nrect_width = " << rect.size.width;
                std::cout << "\nrect_height = " << rect.size.height;
                std::cout << "\nwidth/height = " << rect.size.width / rect.size.height;
                std::cout << "\nedge1.first = " << edge1.first;
                std::cout << "\nedge2.first = " << edge2.first;
                std::cout << "\nedge1.second = " << edge1.second;
                std::cout << "\nedge2.second = " << edge2.second;
                std::cout << "\nheight1/height2 = " << height.second / height.first << '\n';
                std::cout << "\nrect.area = " << rect.size.area();
            }
            */
            if (angle_diff < armor_max_angle_diff
                && std::max(abs(center_angle - light1_angle), abs(center_angle - light2_angle)) < armor_max_angle_diff
                && (rect.size.width / rect.size.height) < armor_max_aspect_ratio
                && (height.second / height.first) < armor_max_height_ratio
                && rect.size.area() > armor_min_area) {
                if (enable_debug)
                    DrawRotatedRect(armors_before_filter, rect, cv::Scalar(0, 255, 0), 2);
                std::vector<cv::Point2f> armor_points;
                if (light1.center.x < light2.center.x)
                    CalcArmorInfo(armor_points, light1, light2);
                else
                    CalcArmorInfo(armor_points, light2, light1);
                armors.emplace_back(ArmorInfo(rect, armor_points));
            }
        }
    }
    if (enable_debug) {
        cv::imshow("armors_before_filter", armors_before_filter);
        /*
        auto c = cv::waitKey(1);
        if (c == 'a')
            cv::waitKey(0);
        */
    }
}

void ArmorDetector::CalcArmorInfo(std::vector<cv::Point2f>& armor_points, cv::RotatedRect left_light, cv::RotatedRect right_light)
{
    cv::Point2f left_points[4], right_points[4];
    left_light.points(left_points);
    right_light.points(right_points);

    cv::Point2f left_down, left_up, right_up, right_down;
    std::sort(left_points, left_points + 4, [](const cv::Point2f& p1, const cv::Point2f& p2) { return p1.y < p2.y; });
    std::sort(right_points, right_points + 4, [](const cv::Point2f& p1, const cv::Point2f& p2) { return p1.y < p2.y; });
    left_down = (left_points[2].x > left_points[3].x) ? left_points[2] : left_points[3];
    left_up = (left_points[0].x > left_points[1].x) ? left_points[0] : left_points[1];
    right_up = (right_points[0].x < right_points[1].x) ? right_points[0] : right_points[1];
    right_down = (right_points[2].x < right_points[3].x) ? right_points[2] : right_points[3];

    armor_points.push_back(right_up);
    armor_points.push_back(right_down);
    armor_points.push_back(left_down);
    armor_points.push_back(left_up);
}

void ArmorDetector::FilterArmors(std::vector<ArmorInfo>& armors)
{
    // TODO: Non-Maximum Suppression
    // TODO: Classifier
}

ArmorInfo ArmorDetector::SelectFinalArmor(std::vector<ArmorInfo>& armors)
{
    std::sort(armors.begin(), armors.end(),
        [](const ArmorInfo& p1, const ArmorInfo& p2) { return p1.rect.size.area() > p2.rect.size.area(); });
    return armors[0];

    // TODO: 根据 classifier 的值进行选择
}

void ArmorDetector::CalcControlInfo(const ArmorInfo& armor, cv::Point3f& target)
{
    cv::Mat rvec, tvec;
    // TODO: test the distortion and intrinsic_matrix of the camera
    if (armor.rect.size.width / armor.rect.size.height < 4)
        cv::solvePnP(small_armor_points, armor.vertex, camera_matrix, distortion_coeffs, rvec, tvec);
    else
        cv::solvePnP(big_armor_points, armor.vertex, camera_matrix, distortion_coeffs, rvec, tvec);
    cv::Mat rmat(3, 3, CV_64F);
    cv::Rodrigues(rvec, rmat);
    double x = tvec.at<double>(0);
    double y = tvec.at<double>(1);
    double z = tvec.at<double>(2);
    float distance = static_cast<float>(sqrt(x * x + y * y + z * z));
    // TODO: 测试小孔成像原理

    double r11 = rmat.at<double>(0, 0);
    double r21 = rmat.at<double>(1, 0);
    double r31 = rmat.at<double>(2, 0);
    double r32 = rmat.at<double>(2, 1);
    double r33 = rmat.at<double>(2, 2);

    float theta_z = atan2(r21, r11) * 180 / CV_PI;
    float theta_y = atan2(-r31, sqrt(r32 * r32 + r33 * r33)) * 180 / CV_PI;
    float theta_x = atan2(r32, r33) * 180 / CV_PI;

    float yaw_offset = 0;
    float pitch_offset = 0;
    target = cv::Point3f(yaw_offset, pitch_offset, distance);

    if (enable_debug) {
        std::cout << "\nx = " << x;
        std::cout << "\ny = " << y;
        std::cout << "\nz = " << z;
        std::cout << "\ndist = " << distance;
        std::cout << "\ntheta_z = " << theta_z;
        std::cout << "\ntheta_y =  " << theta_y;
        std::cout << "\ntheta_x = " << theta_x;
        auto c = cv::waitKey(1);
        if (c == 'a')
            cv::waitKey(0);
    }
}