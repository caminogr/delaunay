#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>

#include "FaceRecognitionPointGenerator.h"

Point convertToOpenGLCoord(int cvX, int cvY, int width, int height) {
  float x = (cvX / (float)width) * 2.0f - 1.0f;
  float y = 1.0f - (cvY / (float)height) * 2.0f;
  return Point(x, y);
}

std::vector<Point> FaceRecognitionPointGenerator::generatePoints() const {
  std::vector<Point> points;
  std::filesystem::path current_path = std::filesystem::current_path();

  cv::CascadeClassifier faceDetector;
  cv::Ptr<cv::face::Facemark> facemark = cv::face::createFacemarkLBF();

  std::filesystem::path cascade_path = current_path / "assets" / "haarcascade_frontalface_alt2.xml";
  std::filesystem::path model_path = current_path / "assets" / "lbfmodel.yaml";
  faceDetector.load(cascade_path);
  facemark->loadModel(model_path);

  std::filesystem::path img_path = current_path / "assets" / "van_gogh.jpg";
  cv::Mat img = cv::imread(img_path.string());

  if (img.empty()) {
    throw std::runtime_error("image is not found");
  }
  int img_width = img.cols;
  int img_height = img.rows;
  // 画像の横幅と高さを取得
  std::vector<cv::Rect> faces;
  cv::Mat grayImg;
  cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);
  faceDetector.detectMultiScale(grayImg, faces);

  std::vector<std::vector<cv::Point2f>> landmarks;
  bool success = facemark->fit(img, faces, landmarks);

  if (success) {
      for (const auto& faceLandmarks : landmarks) {
          for (const auto& pt : faceLandmarks) {
            std::cout << pt << std::endl;
            cv::circle(img, pt, 1, cv::Scalar(0, 0, 255), -1);
            Point point = convertToOpenGLCoord(pt.x, pt.y, img_width, img_height);
            points.push_back(point);
          }
      }
  }

  return points;
}

