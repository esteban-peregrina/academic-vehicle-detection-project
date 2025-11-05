#pragma once

#include <opencv2/opencv.hpp>
#include <string.h>

using namespace std;
using namespace cv;

void affiche_contours(const cv::Mat& frame);
std::vector<std::vector<cv::Point>> follow_vehicles(const cv::Mat& frame, cv::Mat& ref_courante, cv::Mat& ref_precedente);
int count_vehicles(const cv::Mat& frame,
                   const std::vector<std::vector<cv::Point>>& vehicles_contours,
                   int& left_vehicles_count,
                   int& right_vehicles_count,
                   int& last_left_instant_count,
                   int& last_right_instant_count);