#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <thread>
#include <future>
#include <atomic>
#include <mutex>         // std::mutex, std::unique_lock
#include <cmath>

#include <fstream>
#include <sstream>

// It makes sense only for video-Camera (not for video-File)
// To use - uncomment the following line. Optical-flow is supported only by OpenCV 3.x - 4.x
//#define TRACK_OPTFLOW
//#define GPU

// To use 3D-stereo camera ZED - uncomment the following line. ZED_SDK should be installed.
//#define ZED_STEREO

#include "yolo_v2_class.hpp"    // imported functions from DLL

// Kuwaaa
// --------------------------------------------------------------------------------
#include <algorithm>
//#include "../../build/serial.h" // 串口
#include "KuwaaaCode/include/Tdet.h"
#include "KuwaaaCode/include/common.h"

                                                 
KillSb ks(960, 720);
//#define SERIALLL
#define VIDEO
#define MULTI__

int detBlue;                                                        // 当前比赛要检测的车的颜色 （true 为检测蓝色）
int showDet;                                                        // 是否显示检测框 (0 不显示）
int exposure = 100;                                                 // zed相机曝光值 [0, 100]
int use_kalman_filter = 0;                                          // true - for stationary camera
int outVideoIdx;
int isCalib = 0;
std::vector<int> cfgVec = { detBlue, showDet, exposure, use_kalman_filter, outVideoIdx };           // 简化代码统一处理配置
std::vector<bbox_t> resultSave;
std::string vecOut;
cv::Point calibCenter;
std::ofstream outvec;
int stopCalib = 1;                                                  // 停止标定模板
cv::Mat tem;                                                        // 标定物图片（使用模板检测识别标定物）
#ifdef SERIALLL
static WZSerialPort ser;                                            // 串口类
#endif // SERIALLL

void getCalib(int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)
        calibCenter = Point(x, y);
    std::cout << calibCenter.x << " " << calibCenter.y << std::endl;
}

// --------------------------------------------------------------------------------

#ifdef OPENCV
#ifdef ZED_STEREO
#include <sl/Camera.hpp>
#if ZED_SDK_MAJOR_VERSION == 2
#define ZED_STEREO_2_COMPAT_MODE
#endif

//#undef GPU // avoid conflict with sl::MEM::GPU

#ifdef ZED_STEREO_2_COMPAT_MODE
#pragma comment(lib, "sl_core64.lib")
#pragma comment(lib, "sl_input64.lib")
#endif
#pragma comment(lib, "sl_zed64.lib")


cv::Mat slMat2cvMat(sl::Mat &input) {
    int cv_type = -1; // Mapping between MAT_TYPE and CV_TYPE
    if(input.getDataType() ==
#ifdef ZED_STEREO_2_COMPAT_MODE
        sl::MAT_TYPE_32F_C4
#else
        sl::MAT_TYPE::F32_C4
#endif
        ) {
        cv_type = CV_32FC4;
    } else cv_type = CV_8UC4; // sl::Mat used are either RGBA images or XYZ (4C) point clouds
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(
#ifdef ZED_STEREO_2_COMPAT_MODE
        sl::MEM::MEM_CPU
#else
        sl::MEM::CPU
#endif
        ));
}

cv::Mat zed_capture_rgb(sl::Camera &zed) {
    sl::Mat left;
    zed.retrieveImage(left);
    cv::Mat left_rgb;
    cv::cvtColor(slMat2cvMat(left), left_rgb, CV_RGBA2RGB);
    return left_rgb;
}

cv::Mat zed_capture_3d(sl::Camera &zed) {
    sl::Mat cur_cloud;
    zed.retrieveMeasure(cur_cloud,
#ifdef ZED_STEREO_2_COMPAT_MODE
        sl::MEASURE_XYZ
#else
        sl::MEASURE::XYZ
#endif
        );
    return slMat2cvMat(cur_cloud).clone();
}

static sl::Camera zed; // ZED-camera

#else   // ZED_STEREO
std::vector<bbox_t> get_3d_coordinates(std::vector<bbox_t> bbox_vect, cv::Mat xyzrgba) {
    return bbox_vect;
}
#endif  // ZED_STEREO


#include <D:/Opencv/new/cuda110/install/include/opencv2/opencv.hpp>            // C++
#include <D:/Opencv/new/cuda110/install/include/opencv2/core/version.hpp>
#ifndef CV_VERSION_EPOCH     // OpenCV 3.x and 4.x
#include <D:/Opencv/new/cuda110/install/include/opencv2/videoio/videoio.hpp>
#define OPENCV_VERSION CVAUX_STR(CV_VERSION_MAJOR)"" CVAUX_STR(CV_VERSION_MINOR)"" CVAUX_STR(CV_VERSION_REVISION)
#ifndef USE_CMAKE_LIBS
#pragma comment(lib, "opencv_world" OPENCV_VERSION ".lib")
#ifdef TRACK_OPTFLOW
/*
#pragma comment(lib, "opencv_cudaoptflow" OPENCV_VERSION ".lib")
#pragma comment(lib, "opencv_cudaimgproc" OPENCV_VERSION ".lib")
#pragma comment(lib, "opencv_core" OPENCV_VERSION ".lib")
#pragma comment(lib, "opencv_imgproc" OPENCV_VERSION ".lib")
#pragma comment(lib, "opencv_highgui" OPENCV_VERSION ".lib")
*/
#endif    // TRACK_OPTFLOW
#endif    // USE_CMAKE_LIBS
#else     // OpenCV 2.x
#define OPENCV_VERSION CVAUX_STR(CV_VERSION_EPOCH)"" CVAUX_STR(CV_VERSION_MAJOR)"" CVAUX_STR(CV_VERSION_MINOR)
#ifndef USE_CMAKE_LIBS
#pragma comment(lib, "opencv_core" OPENCV_VERSION ".lib")
#pragma comment(lib, "opencv_imgproc" OPENCV_VERSION ".lib")
#pragma comment(lib, "opencv_highgui" OPENCV_VERSION ".lib")
#pragma comment(lib, "opencv_video" OPENCV_VERSION ".lib")
#endif    // USE_CMAKE_LIBS
#endif    // CV_VERSION_EPOCH


// 增加了参数 Mat xyzrbga, 用来标定赛场中心位置
void draw_boxes(cv::Mat mat_img, std::vector<bbox_t> result_vec, std::vector<std::string> obj_names, cv::Mat xyzrgba,
    int current_det_fps = -1, int current_cap_fps = -1)
{
    
    int const colors[6][3] = { { 1,0,1 },{ 0,0,1 },{ 0,1,1 },{ 0,1,0 },{ 1,1,0 },{ 1,0,0 } };
    // 使向量按识别ID升序排序， 为下方识别前哨站提供环境
    sort(result_vec.begin(), result_vec.end(), [](bbox_t b1, bbox_t b2)
                                                    {
                                                        return b1.obj_id < b2.obj_id;
                                                    });
    // 每帧更新标志位
    ks.setFrame();
    /*std::cout << "++++++++++++++++++++" << std::endl;
    std::cout << result_vec.size() << std::endl;*/
    static cv::Rect temRect;
    if (stopCalib)
        if (ks.frameNum % 5 == 0)
           cv::Rect temRect = ks.calibRect(mat_img, tem, xyzrgba, calibCenter, isCalib);
    static cv::Scalar temColor = detBlue ? cv::Scalar(255, 0, 0) : cv::Scalar(0, 0, 255);

    //ks.proData(result_vec, mat_img, detBlue);

    vector<sendData> resDatas;
    vector<bool> bools;
    // 遍历result向量
    if (showDet)
    {
        cv::rectangle(mat_img, temRect, temColor);
        for (auto& i : result_vec) {
            cv::Scalar color = obj_id_to_color(i.obj_id);

            outvec << i.x_3d << " " << i.y_3d << " " << i.z_3d << " " << i.obj_id << "/n";

            resultSave.push_back(i);
            resDatas.push_back(sendData(i.x_3d, i.y_3d, i.z_3d, i.obj_id));
            int weight = mat_img.cols;
            int height = mat_img.rows;
            cv::Point pt1(std::max(int(std::min(i.x - i.w / 2 - 1, weight - i.w)), 1), std::max(int(std::min(i.y - i.h / 2 - 1, height - i.w)), 1));
            cv::Point pt2(std::max(int(std::min(i.x + i.w / 2 - 1, weight - i.w)), 1), std::max(int(std::min(i.y + i.h / 2 - 1, height - i.w)), 1));
            cv::Rect r(std::max(int(std::min(i.x, weight - i.w - 1)), 1), std::max(int(std::min(i.y, height - i.h - 1)), 1), i.w, i.h);

            cv::rectangle(mat_img, cv::Rect(i.x, i.y, i.w, i.h), color, 2);
            if (true) {
                bools.push_back(ks.isBlue(mat_img(r)));
                std::string obj_name = obj_names[i.obj_id];
                if (i.track_id > 0) obj_name += " - " + std::to_string(i.track_id);
                // 有可能边界的框会出界，这里暂且忽略
                try
                {
                    if (ks.checkOutpost(mat_img(r), pt1, pt2, i.obj_id, 25))
                    {
                        obj_name = "######";
                    }
                    if (i.obj_id > 0)
                    {
                        int padding = 20;
                        if (r.x + (r.width + padding) / 2 < weight)
                            r.width += padding;
                        if (r.y + (r.height + padding) / 2 < height)
                            r.height += padding;
                    }
                    if (bools[bools.size() - 1])
                        color = cv::Scalar(255, 0, 0);
                    else
                        color = cv::Scalar(0, 0, 255);
                }
                catch (const std::exception& e)
                {

                }


                cv::Size const text_size = getTextSize(obj_name, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, 2, 0);
                int max_width = (text_size.width > i.w + 2) ? text_size.width : (i.w + 2);
                max_width = std::max(max_width, (int)i.w + 2);
                //max_width = std::max(max_width, 283);
                std::string coords_3d;
                if (!std::isnan(i.z_3d)) {
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(2) << "x:" << i.x_3d << "m y:" << i.y_3d << "m z:" << i.z_3d << "m ";
                    coords_3d = ss.str();
                    cv::Size const text_size_3d = getTextSize(ss.str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, 1, 0);
                    int const max_width_3d = (text_size_3d.width > i.w + 2) ? text_size_3d.width : (i.w + 2);
                    if (max_width_3d > max_width) max_width = max_width_3d;
                }

                cv::Point2f p1(std::max((int)i.x - 1, 0), std::max((int)i.y - 35, 0)), p2(std::min((int)i.x + max_width, mat_img.cols - 1),
                    std::min((int)i.y, mat_img.rows - 1));

                cv::rectangle(mat_img, p1, p2, color, CV_FILLED, 8, 0);
                putText(mat_img, obj_name, cv::Point2f(i.x, i.y - 16), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, cv::Scalar(0, 0, 0), 2);
                if (!coords_3d.empty()) putText(mat_img, coords_3d, cv::Point2f(i.x, i.y - 1), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(0, 0, 0), 1);


#ifndef MULTI__
#ifdef SERIALLL
                ser.send(ks.proData(result_vec, mat_img, detBlue));
                if (i.obj_id != 0)
                    ser.send(catData(i.x_3d, i.z_3d, i.obj_id, false));
#endif // SERIAL
#endif // !MULTi__
            }
        }

#ifdef SERIALLL
#ifdef MULTI__
        ser.send(ks.proData(result_vec, mat_img, detBlue));
        //ser.send(multiCar(resDatas, bools));
#endif // MULTI__
#endif // SERIALLL


        std::cout << std::endl;
        if (current_det_fps >= 0 && current_cap_fps >= 0) {
            std::string fps_str = "FPS detection: " + std::to_string(current_det_fps) + "   FPS capture: " + std::to_string(current_cap_fps);
            putText(mat_img, fps_str, cv::Point2f(10, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, cv::Scalar(50, 255, 0), 2);
        }
    }
}
#endif    // OPENCV


void show_console_result(std::vector<bbox_t> const result_vec, std::vector<std::string> const obj_names, int frame_id = -1) {
    if (frame_id >= 0) std::cout << " Frame: " << frame_id << std::endl;
    for (auto &i : result_vec) {
        if (obj_names.size() > i.obj_id) std::cout << obj_names[i.obj_id] << " - ";
        std::cout << "obj_id = " << i.obj_id << ",  x = " << i.x << ", y = " << i.y
            << ", w = " << i.w << ", h = " << i.h
            << std::setprecision(3) << ", prob = " << i.prob << std::endl;
    }
}

std::vector<std::string> objects_names_from_file(std::string const filename) {
    std::ifstream file(filename);
    std::vector<std::string> file_lines;
    if (!file.is_open()) return file_lines;
    for(std::string line; getline(file, line);) file_lines.push_back(line);
    std::cout << "object names loaded \n";
    return file_lines;
}

template<typename T>
class send_one_replaceable_object_t {
    const bool sync;
    std::atomic<T *> a_ptr;
public:

    void send(T const& _obj) {
        T *new_ptr = new T;
        *new_ptr = _obj;
        if (sync) {
            while (a_ptr.load()) std::this_thread::sleep_for(std::chrono::milliseconds(3));
        }
        std::unique_ptr<T> old_ptr(a_ptr.exchange(new_ptr));
    }

    T receive() {
        std::unique_ptr<T> ptr;
        do {
            while(!a_ptr.load()) std::this_thread::sleep_for(std::chrono::milliseconds(3));
            ptr.reset(a_ptr.exchange(NULL));
        } while (!ptr);
        T obj = *ptr;
        return obj;
    }

    bool is_object_present() {
        return (a_ptr.load() != NULL);
    }

    send_one_replaceable_object_t(bool _sync) : sync(_sync), a_ptr(NULL)
    {}
};

int main(int argc, char *argv[])
{
    
    std::string  names_file = "data/coco.names";
    std::string  cfg_file = "cfg/yolov3.cfg";
    std::string  weights_file = "yolov3.weights";
    std::string filename;

    if (argc > 4) {    //voc.names yolo-voc.cfg yolo-voc.weights test.mp4
        names_file = argv[1];
        cfg_file = argv[2];
        weights_file = argv[3];
        filename = argv[4];
    }
    else if (argc > 1) filename = argv[1];

    float const thresh = (argc > 5) ? std::stof(argv[5]) : 0.2;

    // **************************************************************************************
    // --------------------------------------------------------------------------------------
    names_file = "E:/download/yolo/yolov4/darknet-master/build/darknet/x64/cTrain/robo.names";
    cfg_file = "E:/download/yolo/yolov4/darknet-master/build/darknet/x64/cTrain/yolov4_custom_test.cfg";
    weights_file = "E:/download/yolo/yolov4/darknet-master/build/darknet/x64/roboBackup/yolov4_7800.weights";
    cv::Mat origin;
    cv::namedWindow("window name", 0);
    cv::namedWindow("QAQ", 0);
    cv::setMouseCallback("window name", getCalib);
    cv::Mat nullMat = Mat::zeros(Size(960, 720), CV_8UC1);
    cv::createTrackbar("showDet", "QAQ", &showDet, 1);                      // 一些东西的调试窗口
    cv::createTrackbar("useKalman", "QAQ", &use_kalman_filter, 1);          // 启用卡尔曼滤波
    cv::createTrackbar("stopCalib", "QAQ", &stopCalib, 1);                  // 停止标定场地
    cv::createTrackbar("isHandCalib", "QAQ", &isCalib, 1);                  // 手动标定
    

#ifndef VIDEO
    filename = "zed_camera";
    cv::createTrackbar("exposure", "QAQ", &exposure, 100);
#else
    filename = "E:/download/yolo/yolov5-master/w11cap1.mp4";
#endif // VIDEO
    std::string root = "E:/download/yolo/yolov4/";
    std::string outFileName = "result" + to_string(outVideoIdx);
    std::string rax = ".mp4";
    std::string out_videofile = root + outFileName + rax;

    vecOut = root + outFileName + "vec" + ".txt";
    outvec.open(vecOut);
    
#ifdef SERIALLL
    // 打开串口
    if (!ser.open("com3"))
        return 0;
#endif //

    string redTemPath = "./red.jpg";
    string blueTemPath = "./blue.jpg";
    string robocfg = "./robocfg.txt";
    
    ifstream input(robocfg);
    string line;
    string str;
    stringstream ss;
    // 这里文件打不开或者变量没对齐就寄了!!
    for (int& i : cfgVec)
        if (!input.eof())
        {
            getline(input, line);
            ss.str(line);

            ss >> str;
            i = stoi(str);
        }

    if (detBlue)
        tem = imread(blueTemPath);
    else
        tem = imread(redTemPath);

    // --------------------------------------------------------------------------------------
    // **************************************************************************************

    Detector detector(cfg_file, weights_file);

    auto obj_names = objects_names_from_file(names_file);
    bool const save_output_videofile = true;   // true - for history
    bool const send_network = false;        // true - for remote detection
    //bool const use_kalman_filter = false;   // true - for stationary camera

    bool detection_sync = true;             // true - for video-file
#ifdef TRACK_OPTFLOW    // for slow GPU
    detection_sync = false;
    Tracker_optflow tracker_flow;
    //detector.wait_stream = true;
#endif  // TRACK_OPTFLOW


    while (true)
    {
        std::cout << "input image or video filename: ";
        if(filename.size() == 0) std::cin >> filename;
        if (filename.size() == 0) break;

        try {
#ifdef OPENCV
            preview_boxes_t large_preview(100, 150, false), small_preview(50, 50, true);
            bool show_small_boxes = false;

            std::string const file_ext = filename.substr(filename.find_last_of(".") + 1);
            std::string const protocol = filename.substr(0, 7);
            if (file_ext == "avi" || file_ext == "mp4" || file_ext == "mjpg" || file_ext == "mov" ||     // video file
                protocol == "rtmp://" || protocol == "rtsp://" || protocol == "http://" || protocol == "https:/" ||    // video network stream
                filename == "zed_camera" || file_ext == "svo" || filename == "web_camera")   // ZED stereo camera

            {
                if (protocol == "rtsp://" || protocol == "http://" || protocol == "https:/" || filename == "zed_camera" || filename == "web_camera")
                    detection_sync = false;

                cv::Mat cur_frame;
                std::atomic<int> fps_cap_counter(0), fps_det_counter(0);
                std::atomic<int> current_fps_cap(0), current_fps_det(0);
                std::atomic<bool> exit_flag(false);
                std::chrono::steady_clock::time_point steady_start, steady_end;
                int video_fps = 25;
                bool use_zed_camera = false;

                track_kalman_t track_kalman;

#ifdef ZED_STEREO
                sl::InitParameters init_params;
                init_params.depth_minimum_distance = 0.5;
    #ifdef ZED_STEREO_2_COMPAT_MODE
                init_params.depth_mode = sl::DEPTH_MODE_ULTRA;
                init_params.camera_resolution = sl::RESOLUTION_HD720;// sl::RESOLUTION_HD1080, sl::RESOLUTION_HD720
                init_params.coordinate_units = sl::UNIT_METER;
                init_params.camera_buffer_count_linux = 2;
                if (file_ext == "svo") init_params.svo_input_filename.set(filename.c_str());
    #else
                init_params.depth_mode = sl::DEPTH_MODE::ULTRA;
                init_params.camera_resolution = sl::RESOLUTION::HD1080;// sl::RESOLUTION::HD1080, sl::RESOLUTION::HD720
                init_params.coordinate_units = sl::UNIT::METER;
                if (file_ext == "svo") init_params.input.setFromSVOFile(filename.c_str());
    #endif
                //init_params.sdk_cuda_ctx = (CUcontext)detector.get_cuda_context();
                init_params.sdk_gpu_id = detector.cur_gpu_id;

                if (filename == "zed_camera" || file_ext == "svo") {
                    std::cout << "ZED 3D Camera " << zed.open(init_params) << std::endl;
                    if (!zed.isOpened()) {
                        std::cout << " Error: ZED Camera should be connected to USB 3.0. And ZED_SDK should be installed. \n";
                        getchar();
                        return 0;
                    } 
                    cur_frame = zed_capture_rgb(zed);
                    use_zed_camera = true;
                }

#endif  // ZED_STEREO

                cv::VideoCapture cap;
                if (filename == "web_camera") {
                    cap.open(0);
                    cap >> cur_frame;
                } else if (!use_zed_camera) {
                    cap.open(filename);
                    cap >> cur_frame;
                }
#ifdef CV_VERSION_EPOCH // OpenCV 2.x
                video_fps = cap.get(CV_CAP_PROP_FPS);
#else
                video_fps = cap.get(cv::CAP_PROP_FPS);
#endif
                cv::Size const frame_size = cur_frame.size();
                //cv::Size const frame_size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
                std::cout << "\n Video size: " << frame_size << std::endl;

                cv::VideoWriter output_video;
                if (save_output_videofile)
#ifdef CV_VERSION_EPOCH // OpenCV 2.x
                    output_video.open(out_videofile, CV_FOURCC('D', 'I', 'V', 'X'), std::max(35, video_fps), frame_size, true);
#else
                    output_video.open(out_videofile, cv::VideoWriter::fourcc('M', 'P', '4', '2'), 20, frame_size, true);
#endif

                struct detection_data_t {
                    cv::Mat cap_frame;
                    std::shared_ptr<image_t> det_image;
                    std::vector<bbox_t> result_vec;
                    cv::Mat draw_frame;
                    bool new_detection;
                    uint64_t frame_id;
                    bool exit_flag;
                    cv::Mat zed_cloud;
                    std::queue<cv::Mat> track_optflow_queue;
                    detection_data_t() : new_detection(false), exit_flag(false) {}
                };

                const bool sync = detection_sync; // sync data exchange
                send_one_replaceable_object_t<detection_data_t> cap2prepare(sync), cap2draw(sync),
                    prepare2detect(sync), detect2draw(sync), draw2show(sync), draw2write(sync), draw2net(sync);

                std::thread t_cap, t_prepare, t_detect, t_post, t_draw, t_write, t_network;

                // capture new video-frame
                if (t_cap.joinable()) t_cap.join();
                t_cap = std::thread([&]()
                {
                    uint64_t frame_id = 0;
                    detection_data_t detection_data;
                    do {
                        detection_data = detection_data_t();
#ifdef ZED_STEREO
                        if (use_zed_camera) {
                            while (zed.grab() !=
        #ifdef ZED_STEREO_2_COMPAT_MODE
                                sl::SUCCESS
        #else
                                sl::ERROR_CODE::SUCCESS
        #endif
                                ) std::this_thread::sleep_for(std::chrono::milliseconds(2));
                            detection_data.cap_frame = zed_capture_rgb(zed);
                            detection_data.zed_cloud = zed_capture_3d(zed);
                        }
                        else
#endif   // ZED_STEREO
                        {
                            cap >> detection_data.cap_frame;
                        }
                        fps_cap_counter++;
                        detection_data.frame_id = frame_id++;
                        if (detection_data.cap_frame.empty() || exit_flag) {
                            std::cout << " exit_flag: detection_data.cap_frame.size = " << detection_data.cap_frame.size() << std::endl;
                            detection_data.exit_flag = true;
                            detection_data.cap_frame = cv::Mat(frame_size, CV_8UC3);
                        }

                        if (!detection_sync) {
                            cap2draw.send(detection_data);       // skip detection
                        }
                        cap2prepare.send(detection_data);
                    } while (!detection_data.exit_flag);
                    std::cout << " t_cap exit \n";
                });


                // pre-processing video frame (resize, convertion)
                t_prepare = std::thread([&]()
                {
                        
                    std::shared_ptr<image_t> det_image;
                    detection_data_t detection_data;
                    do {
                        detection_data = cap2prepare.receive();

                        det_image = detector.mat_to_image_resize(detection_data.cap_frame);
                        detection_data.det_image = det_image;
                        prepare2detect.send(detection_data);    // detection

                    } while (!detection_data.exit_flag);
                    std::cout << " t_prepare exit \n";
                });


                // detection by Yolo
                if (t_detect.joinable()) t_detect.join();
                t_detect = std::thread([&]()
                {

                        
                    std::shared_ptr<image_t> det_image;
                    detection_data_t detection_data;
                    do {
                        detection_data = prepare2detect.receive();
                        det_image = detection_data.det_image;
                        std::vector<bbox_t> result_vec;

                        if(det_image)
                            result_vec = detector.detect_resized(*det_image, frame_size.width, frame_size.height, thresh, true);  // true
                        fps_det_counter++;
                        //std::this_thread::sleep_for(std::chrono::milliseconds(150));

                        detection_data.new_detection = true;
                        detection_data.result_vec = result_vec;
                        detect2draw.send(detection_data);
                    } while (!detection_data.exit_flag);
                    std::cout << " t_detect exit \n";
                });

                // draw rectangles (and track objects)
                t_draw = std::thread([&]()
                {
                    
                    std::queue<cv::Mat> track_optflow_queue;
                    detection_data_t detection_data;
                    do {

                        // for Video-file
                        if (detection_sync) {
                            detection_data = detect2draw.receive();
                        }
                        // for Video-camera
                        else
                        {
                            // get new Detection result if present
                            if (detect2draw.is_object_present()) {
                                cv::Mat old_cap_frame = detection_data.cap_frame;   // use old captured frame
                                detection_data = detect2draw.receive();
                                if (!old_cap_frame.empty()) detection_data.cap_frame = old_cap_frame;
                            }
                            // get new Captured frame
                            else {
                                std::vector<bbox_t> old_result_vec = detection_data.result_vec; // use old detections
                                detection_data = cap2draw.receive();
                                detection_data.result_vec = old_result_vec;
                            }
                        }

                        cv::Mat cap_frame = detection_data.cap_frame;
                        cv::Mat draw_frame = detection_data.cap_frame.clone();
                        std::vector<bbox_t> result_vec = detection_data.result_vec;
#ifdef TRACK_OPTFLOW
                        //if (detection_data.new_detection) {
                        //    tracker_flow.update_tracking_flow(detection_data.cap_frame, detection_data.result_vec);
                        //    while (track_optflow_queue.size() > 0) {
                        //        draw_frame = track_optflow_queue.back();
                        //        result_vec = tracker_flow.tracking_flow(track_optflow_queue.front(), false);
                        //        track_optflow_queue.pop();
                        //    }
                        //}
                        //else {
                        //    /*cv::imshow("aaaa", cap_frame);
                        //    cv::waitKey();*/
                        //    track_optflow_queue.push(cap_frame);
                        //    result_vec = tracker_flow.tracking_flow(cap_frame, false);
                        //}
                        //detection_data.new_detection = true;    // to correct kalman filter
#endif //TRACK_OPTFLOW

                        // track ID by using kalman filter
                        if (use_kalman_filter) {
                            if (detection_data.new_detection) {
                                result_vec = track_kalman.correct(result_vec);
                            }
                            else {
                                result_vec = track_kalman.predict();
                            }
                        }
                        // track ID by using custom function
                        else {
                            int frame_story = std::max(5, current_fps_cap.load());
                            result_vec = detector.tracking_id(result_vec, true, frame_story, 40);
                        }

                        if (use_zed_camera && !detection_data.zed_cloud.empty()) {
                            result_vec = get_3d_coordinates(result_vec, detection_data.zed_cloud);
                        }
                        origin = draw_frame.clone();
                        //small_preview.set(draw_frame, result_vec);
                        //large_preview.set(draw_frame, result_vec);
                        draw_boxes(draw_frame, result_vec, obj_names, detection_data.zed_cloud, current_fps_det, current_fps_cap);
                        //show_console_result(result_vec, obj_names, detection_data.frame_id);
                        //large_preview.draw(draw_frame);
                        //small_preview.draw(draw_frame, true);
                        
                        /*for (const auto& i : result_vec)
                            ser.send(catData(i.x, i.y, i.obj_id), true);*/

                        detection_data.result_vec = result_vec;
                        detection_data.draw_frame = draw_frame;
                        draw2show.send(detection_data);
                        if (send_network) draw2net.send(detection_data);
                        if (output_video.isOpened()) draw2write.send(detection_data);
                    } while (!detection_data.exit_flag);
                    std::cout << " t_draw exit \n";
                });


                // write frame to videofile
                t_write = std::thread([&]()
                {
                        std::cout << "t_write" << std::endl;
                    if (output_video.isOpened()) {
                        std::cout << "isOpened" << std::endl;
                        detection_data_t detection_data;
                        cv::Mat output_frame;
                        do {
                            detection_data = draw2write.receive();
                            if(detection_data.draw_frame.channels() == 4) cv::cvtColor(detection_data.draw_frame, output_frame, CV_RGBA2RGB);
                            else output_frame = detection_data.draw_frame;
                            output_video << output_frame;
                        } while (!detection_data.exit_flag);
                        output_video.release();
                    }
                    std::cout << " t_write exit \n";
                });

                // send detection to the network
                t_network = std::thread([&]()
                {
                    if (send_network) {
                        detection_data_t detection_data;
                        do {
                            detection_data = draw2net.receive();
                                
                            detector.send_json_http(detection_data.result_vec, obj_names, detection_data.frame_id, filename);

                        } while (!detection_data.exit_flag);
                    }
                    std::cout << " t_network exit \n";
                });

                VideoWriter write("C:/Users/DAZE/Desktop/result222.mp4", VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, frame_size, true);
                // show detection
                detection_data_t detection_data;
                do {

                    steady_end = std::chrono::steady_clock::now();
                    float time_sec = std::chrono::duration<double>(steady_end - steady_start).count();
                    if (time_sec >= 1) {
                        current_fps_det = fps_det_counter.load() / time_sec;
                        current_fps_cap = fps_cap_counter.load() / time_sec;
                        steady_start = steady_end;
                        fps_det_counter = 0;
                        fps_cap_counter = 0;
                    }

                    detection_data = draw2show.receive();
                    cv::Mat draw_frame = detection_data.draw_frame;

                    //if (extrapolate_flag) {
                    //    cv::putText(draw_frame, "extrapolate", cv::Point2f(10, 40), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(50, 50, 0), 2);
                    //}
                    //cv::imshow("aaa", origin);
#ifndef VIDEO
                    zed.setCameraSettings(sl::VIDEO_SETTINGS::EXPOSURE, exposure);
                    zed.setCameraSettings(sl::VIDEO_SETTINGS::WHITEBALANCE_AUTO);

#endif // !VIDEO
                    cv::imshow("QAQ", nullMat);
                    cv::imshow("window name", draw_frame);
                    int key = cv::waitKey(3);    // 3 or 16ms
                    if (key == 'f') show_small_boxes = !show_small_boxes;
                    if (key == 'p') while (true) if (cv::waitKey(100) == 'p') break;
                    //if (key == 'e') extrapolate_flag = !extrapolate_flag;
                    if (key == 27) { exit_flag = true;}

                    //std::cout << " current_fps_det = " << current_fps_det << ", current_fps_cap = " << current_fps_cap << std::endl;
                } while (!detection_data.exit_flag);
                std::cout << " show detection exit \n";

                cv::destroyWindow("window name");
                // wait for all threads
                if (t_cap.joinable()) t_cap.join();
                if (t_prepare.joinable()) t_prepare.join();
                if (t_detect.joinable()) t_detect.join();
                //if (t_post.joinable()) t_post.join();
                if (t_draw.joinable()) t_draw.join();
                if (t_write.joinable()) t_write.join(); else std::cout << "write__joinable false" << std::endl;
                //if (t_network.joinable()) t_network.join();
                
                break;

            }
            else if (file_ext == "txt") {    // list of image files
                std::ifstream file(filename);
                if (!file.is_open()) std::cout << "File not found! \n";
                else
                    for (std::string line; file >> line;) {
                        std::cout << line << std::endl;
                        cv::Mat mat_img = cv::imread(line);
                        std::vector<bbox_t> result_vec = detector.detect(mat_img);
                        show_console_result(result_vec, obj_names);
                        //draw_boxes(mat_img, result_vec, obj_names);
                        //cv::imwrite("res_" + line, mat_img);
                    }
            }
            else {    // image file
                // to achive high performance for multiple images do these 2 lines in another thread
                cv::Mat mat_img = cv::imread(filename);
                auto det_image = detector.mat_to_image_resize(mat_img);

                auto start = std::chrono::steady_clock::now();
                std::vector<bbox_t> result_vec = detector.detect_resized(*det_image, mat_img.size().width, mat_img.size().height);
                auto end = std::chrono::steady_clock::now();
                std::chrono::duration<double> spent = end - start;
                std::cout << " Time: " << spent.count() << " sec \n";

                //result_vec = detector.tracking_id(result_vec);    // comment it - if track_id is not required
                //draw_boxes(mat_img, result_vec, obj_names);
                cv::imshow("window name", mat_img);
                show_console_result(result_vec, obj_names);
                cv::waitKey(0);
            }
#else   // OPENCV
            //std::vector<bbox_t> result_vec = detector.detect(filename);

            auto img = detector.load_image(filename);
            std::vector<bbox_t> result_vec = detector.detect(img);
            detector.free_image(img);
            show_console_result(result_vec, obj_names);
#endif  // OPENCV
        }
        catch (std::exception &e) { std::cerr << "exception: " << e.what() << "\n"; getchar(); }
        catch (...) { std::cerr << "unknown exception \n"; getchar(); }
        filename.clear();
    }
    return 0;

     }
     