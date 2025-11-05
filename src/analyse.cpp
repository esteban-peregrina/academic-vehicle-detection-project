#include "../inc/analyse.hpp"

#define TEST 0

void affiche_contours(const cv::Mat& frame) {
    // Lissage
    cv::Mat blurred;
    cv::GaussianBlur(frame, blurred, cv::Size(5,5), 1.5);

    // Conversion HSV
    cv::Mat hsvFrame;
    cv::cvtColor(blurred, hsvFrame, cv::COLOR_BGR2HSV);
    if (TEST) imshow("HSV", hsvFrame);

    // Extraction du canal de saturation
    std::vector<cv::Mat> channels;
    cv::split(hsvFrame, channels);
    cv::Mat sat = channels[1];
    if (TEST) imshow("Saturation", sat);

    // Seuillage
    cv::Mat mask;
    int sat_tresh = 50; // L'asphalte aura une saturation inférieure
    cv::threshold(sat, mask, sat_tresh, 255, cv::THRESH_BINARY_INV); //THRESH_BINARY_INV pour que ce qui est inférieur au seuil soit blanc
    if (TEST) imshow("Seuillage", mask);

    // Morphologies (amélioration de l'image)
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, element);
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, element);
    if (TEST) imshow("Morphologies", mask);

    // Détection des contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Isolation des voies (2 contours les plus grands)
    std::vector<std::vector<cv::Point>> largestContours;
    if(contours.size() > 0) {
        std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b){
            return cv::contourArea(a) > cv::contourArea(b);
        });

        for(size_t i = 0; i < std::min<size_t>(2, contours.size()); i++)
            largestContours.push_back(contours[i]);
    }    
    // Superposition des contours sur l'image originale
    cv::Mat frameWithContours = frame.clone();
    cv::drawContours(frameWithContours, largestContours, -1, cv::Scalar(0, 0, 255), 2); // rouge
    imshow("Contours superposes", frameWithContours);
}

std::vector<std::vector<cv::Point>> follow_vehicles(const cv::Mat& frame, cv::Mat& ref_courante, cv::Mat& ref_precedente) {
    // Construit l'image de référence courante
    float rate = 0.55f;
    ref_courante = rate * frame + (1.0f - rate) * ref_precedente;
    if (TEST) imshow("References", ref_courante);

    // Différence absolue avec la frame 
    cv::Mat abs_delta;
    abs_delta = abs(ref_courante - frame);
    if (TEST) imshow("Difference absolue", abs_delta);

    // NDG pour simplifier
    cv::Mat delta_NDG;
    cv::cvtColor(abs_delta, delta_NDG, COLOR_BGR2GRAY);
    if (TEST) imshow("Difference NDG", delta_NDG);

    // Seuillage
    int threshold = 5;
    cv::Mat delta_bin;
    cv::threshold(delta_NDG, delta_bin, threshold, 255, THRESH_BINARY);
    if (TEST) imshow("Seuillage", delta_bin);

    // Morphologies (amélioration de l'image)
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(15, 15));
    cv::morphologyEx(delta_bin, delta_bin, cv::MORPH_CLOSE, element);
    cv::morphologyEx(delta_bin, delta_bin, cv::MORPH_OPEN, element);
    if (TEST) imshow("Morphologies", delta_bin);
    
    // Detection des contours
    std::vector<std::vector<cv::Point>> vehicles_contours;
    findContours(delta_bin, vehicles_contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Superposition des contours sur l'image originale
    cv::Mat frame_with_vehicles = frame.clone();
    cv::drawContours(frame_with_vehicles, vehicles_contours, -1, cv::Scalar(0,255,0), 2); // vert
    imshow("Vehicules detectes", frame_with_vehicles);

    ref_precedente = ref_courante;

    return vehicles_contours;
}

int count_vehicles(const cv::Mat& frame,
                   const std::vector<std::vector<cv::Point>>& vehicles_contours,
                   int& left_vehicles_count,
                   int& right_vehicles_count,
                   int& last_left_instant_count,
                   int& last_right_instant_count) {
    int horizon = 90, threshold = 3;
    int left_instant_count = 0, right_instant_count = 0;

    cv::Mat frame_copy = frame.clone();

    for (auto &c : vehicles_contours) {
        // Calcul du centre du contour
        cv::Moments m = cv::moments(c);
        if (m.m00 == 0) continue; // Pas de division par 0
        cv::Point center(m.m10 / m.m00, m.m01 / m.m00);

        // Vérifie si le centre traverse la ligne horizontale gauche ou droite
        if (center.y > horizon - threshold && center.y < horizon + threshold && center.x < frame.cols / 2) { left_instant_count++; }
        if (center.y > horizon - threshold && center.y < horizon + threshold && center.x > frame.cols / 2) { right_instant_count++; }

        // Dessine le centre et le contour
        cv::circle(frame_copy, center, 5, cv::Scalar(255, 0, 125), -1);
        cv::Rect box = cv::boundingRect(c);
        cv::rectangle(frame_copy, box, cv::Scalar(0, 255, 0), 2);
    }

    // Mise à jour du compteur total
    if (left_instant_count > last_left_instant_count) { left_vehicles_count += left_instant_count - last_left_instant_count; }
    if (right_instant_count > last_right_instant_count) { right_vehicles_count += right_instant_count - last_right_instant_count; }

    // Affiche le compteur sur la frame
    cv::putText(frame_copy, "Left cars: " + std::to_string(left_vehicles_count),
                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 
                1, cv::Scalar(0, 125, 255), 2);
    cv::putText(frame_copy, "Right cars: " + std::to_string(right_vehicles_count),
                cv::Point(10, 70), cv::FONT_HERSHEY_SIMPLEX, 
                1, cv::Scalar(0, 0, 255), 2);

    cv::line(frame_copy, 
         cv::Point(0, horizon),                 // point à gauche
         cv::Point(frame.cols, horizon),        // point à droite
         cv::Scalar(255, 0, 0),                 // couleur 
         2);                                    // epaisseur
    
    cv::rectangle(frame_copy,
        cv::Point(0, horizon - threshold),
        cv::Point(frame.cols, horizon + threshold),
        cv::Scalar(255, 255, 0), // couleur
        1); // contour seulement

    cv::imshow("Vehicles counted", frame_copy);

    return left_vehicles_count + right_vehicles_count;
}