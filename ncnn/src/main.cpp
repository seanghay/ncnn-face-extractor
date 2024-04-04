#include "UltraFace.hpp"
#include "glob.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv)
{
    if (argc <= 4) {
        fprintf(stderr,
                "Usage: %s <ncnn bin> <ncnn param> <glob_pattern> <outdir>\n",
                argv[0]);
        return 1;
    }

    std::string bin_path = argv[1];
    std::string param_path = argv[2];
    std::filesystem::path outdir = argv[4];

    if (!std::filesystem::is_directory(outdir) ||
        std::filesystem::exists(outdir)) {
        std::filesystem::create_directory(outdir);
    }

    UltraFace ultraface(bin_path, param_path, 320, 240, 1,
                        0.7); // config model input

    for (auto &p : glob::rglob(argv[3])) {
        std::string image_file = p.string();
        std::string image_filename =
            std::filesystem::path(image_file).filename();

        std::cout << image_file << std::endl;

        cv::Mat frame = cv::imread(image_file);
        ncnn::Mat inmat = ncnn::Mat::from_pixels(
            frame.data, ncnn::Mat::PIXEL_BGR2RGB, frame.cols, frame.rows);

        std::vector<FaceInfo> face_info;
        ultraface.detect(inmat, face_info);

        std::string s = "[";
        int num_total_face = face_info.size();

        for (int i = 0; i < num_total_face; i++) {

            auto face = face_info[i];
            s.append("{");

            s.append("\"x1\":");
            s.append(std::to_string(face.x1));
            s.append(",");

            s.append("\"x2\":");
            s.append(std::to_string(face.x2));
            s.append(",");

            s.append("\"y1\":");
            s.append(std::to_string(face.y1));
            s.append(",");

            s.append("\"y2\":");
            s.append(std::to_string(face.y2));

            s.append("}");

            if (i < num_total_face - 1) {
                s.append(",");
            }

            const float threshold = 0.3;
            const float np = (face.x2 - face.x1) * threshold;

            float x1 = face.x1 - np;
            if (x1 < 0)
                x1 = 0.0;

            float y1 = face.y1 - np;
            if (y1 < 0)
                y1 = 0.0;

            float w = face.x2 - face.x1 + np * 2.0;
            if (x1 + w > frame.cols)
                w = frame.cols - x1;

            float h = face.y2 - face.y1 + np * 2.0;
            
            if (y1 + h > frame.rows)
                h = frame.rows - y1;

            cv::Rect roi(x1, y1, w, h);
            cv::Mat croppedFrame = frame(roi).clone();
            // cv::imshow("image", croppedFrame);
            // cv::waitKey();

            std::string filename = image_filename + std::string("_face-") +
                                   std::to_string(i) + std::string(".jpg");
            cv::imwrite((outdir / filename).string(), croppedFrame);
        }

        s.append("]");

        std::string json = "{";
        json.append("\"w\":");
        json.append(std::to_string(frame.cols));
        json.append(",");

        json.append("\"h\":");
        json.append(std::to_string(frame.rows));
        json.append(",");

        json.append("\"face\":");
        json.append(s);
        json.append("}");

        std::ofstream outfile;
        outfile.open((outdir / (image_filename + ".json")).string());
        if (outfile.is_open()) {
            outfile << json;
        }

        outfile.close();
    }
    return 0;
}
