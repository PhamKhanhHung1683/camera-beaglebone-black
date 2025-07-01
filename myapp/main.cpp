#include <iostream>
#include <thread>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include <opencv2/opencv.hpp>
#include "ssd1306/ssd1306.h"
#include "utils/utils.h"

static void process_frames(int width, int height, cv::VideoCapture& cap, cv::CascadeClassifier& face_cascade, FILE* pipeout);
static void cleanup_records(const std::string& directory, int max_files);

int main() {
    int width = 320; int height = 240; int fps = 5;

    std::string ip = "239.255.0.1";
    int port = 1234;

    std::string output_dir = "/var/lib/myapp/records";
    const int max_files = 3;

    if (ssd1306_init() == -1) {
        std::cerr << "Failed to init ssd1306\n";
        return -1;
    }

    if (mkdir(output_dir.c_str(), 0755) == -1 && errno != EEXIST) {
        std::cerr << "Failed to make dir\n";
        return -1;
    }

    cv::CascadeClassifier face_cascade;
    if (!face_cascade.load("/usr/share/myapp/haarcascade_frontalface_default.xml")) {
        std::cerr << "Failed to load haarcascade\n";
        return -1;
    }

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Failed to open camera USB\n";
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    cap.set(cv::CAP_PROP_FPS, fps);

    std::string ffmpeg_cmd = std::string("ffmpeg -f rawvideo -pix_fmt bgr24 -s ")
        + std::to_string(width) + "x" + std::to_string(height)
        + " -r " + std::to_string(fps)
        + " -i pipe: "
        + "-map 0 -vcodec libx264 -preset ultrafast -tune zerolatency "
        + "-g " + std::to_string(fps * 10)
        + " -force_key_frames \"expr:gte(t,n_forced*10)\" "
        + "-f tee "
        + "\"[f=mpegts]udp://" + ip + ":" + std::to_string(port) + "?pkt_size=1316"
        + "|[f=segment:segment_time=10:strftime=1]" + output_dir + "/record_%Y-%m-%d_%H-%M-%S.mp4\"";

    FILE* pipeout = popen(ffmpeg_cmd.c_str(), "w");
    if (!pipeout) {
        std::cerr << "Failed to open pipe to FFmpeg\n";
        return -1;
    }

    std::thread process_frames_thread(process_frames, width, height, std::ref(cap), std::ref(face_cascade), pipeout);

    std::thread cleanup_records_thread(cleanup_records, output_dir, max_files);
    cleanup_records_thread.detach();

    while (true)
    {
        int cpu_usage;
        get_cpu_usage(&cpu_usage);

        int mem_used, mem_total;
        get_memory_usage(&mem_used, &mem_total);

        char display_text[128];
        snprintf(display_text, sizeof(display_text), 
            "CPU: %d%%\nRAM: %d/%d MB", 
            cpu_usage, mem_used, mem_total);

        if (ssd1306_write(display_text) != 0) {
            std::cerr << "Failed to write to SSD1306\n";
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    ssd1306_end();

    process_frames_thread.join();
    pclose(pipeout);
    cap.release();

    return 0;
}

static void process_frames(int width, int height, cv::VideoCapture& cap, cv::CascadeClassifier& face_cascade, FILE* pipeout) {
    cv::Mat frame, gray_frame;
    int frame_count = 0;
    const int detection_interval = 10;
    std::vector<cv::Rect> previous_faces;
    int face_count = 0;

    while (true)
    {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Empty frame, retrying...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        char time_str[100];
        std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
        cv::putText(frame, time_str, cv::Point(2, 12), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 255, 0), 1);

        std::vector<cv::Rect> faces;

        if (frame_count % detection_interval == 0) {
            cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
            cv::equalizeHist(gray_frame, gray_frame);
            
            face_cascade.detectMultiScale(
                gray_frame,
                faces,
                1.1,
                3,
                0,
                cv::Size(20, 20),  // minSize - face
                cv::Size(100, 100) // maxSize - face
            );
                
            face_count = faces.size();
            previous_faces = faces;
        } else {
            faces = previous_faces;
        }
        
        for (const auto& face : faces) {
            cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2);
        }

        std::string count_text = "Faces: " + std::to_string(face_count);
        cv::putText(frame, count_text, cv::Point(2, height - 8), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 255, 255), 1);

        size_t written = fwrite(frame.data, 1, width * height * 3, pipeout);
        fflush(pipeout);
        if (written != width * height * 3) {
            std::cerr << "Failed to write data to pipe\n";
            break;
        }

        frame_count++;
        if (frame_count > 10000) {
            frame_count = 0;
        }
    }
}

static void cleanup_records(const std::string& directory, int max_files) {
    while (true) {
        std::vector<std::string> file_list;
        DIR* dir = opendir(directory.c_str());
        if (!dir) {
            std::cerr << "Failed to open output dir\n";
            return;
        }

        struct dirent* entry;
        while ((entry = readdir(dir))) {
            if (entry->d_type != DT_REG) continue;
            std::string filename = entry->d_name;
            if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".mp4") {
                file_list.push_back(filename);
            }
        }
        closedir(dir);

        while (file_list.size() > static_cast<size_t>(max_files)) {
            auto min_it = std::min_element(file_list.begin(), file_list.end());
            if (min_it != file_list.end()) {
                std::string filepath = directory + "/" + *min_it;
                if (unlink(filepath.c_str()) != 0) {
                    std::cerr << "Failed to delete file\n";
                    return;
                } else {
                    std::cout << "Deleted file: " << filepath << "\n";
                    file_list.erase(min_it);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}
