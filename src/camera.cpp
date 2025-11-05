#include "../inc/camera.hpp"
#include "../inc/analyse.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

Camera::Camera() {
	m_fps = 30; // valeur par défaut
}

// Camera::~Camera() {

// }

bool Camera::open(std::string filename) {
	m_fileName = filename;
	
	// Ouvre le flux vidéo (caméra ou fichier)
	std::istringstream iss(filename.c_str()); // .c_str() reconvertit std:string en char* pour iss() (input string stream)
	int devid;
	bool isOpen;
	if (!(iss >> devid) || !iss.eof()) { // on essaie d'extraire un entier du istringstream (en le mettant dans devid)
		isOpen = m_cap.open(filename.c_str()); // on ouvre un flux vidéo issu d'un fichier vidéo
	} else { isOpen = m_cap.open(devid); } // on ouvre un flux vidéo issu d'une camera 
	// Remarque : OpenCV a deux surcharges de VideoCapture::open() qui renvoit un VideoCapture, d'où la possibilité d'utiliser un nom de fichier ou un indice caméra
 
	if(!isOpen) {
		std::cerr << "Unable to open video file." << std::endl; // cerr comme "character error"
		return false;
	}
	
	// Récupère le framerate 
	m_fps = m_cap.get(cv::CAP_PROP_FPS); // retourne 0 si introuvable
	if(m_fps == 0) { m_fps = 30; } // si introuvable, met à 30 fps par défaut

	return true; 
}

void Camera::play() {
	// Create main window
	cv::namedWindow("Video", cv::WINDOW_AUTOSIZE);
	
	// Compute time to wait to obtain wanted framerate
	int timeToWait = 1000/m_fps;

	// Variables pour le suivi des véhicules
    cv::Mat ref_courante, ref_precedente;
	std::vector<std::vector<cv::Point>> vehicles_contours;

	// Variable pour le comptage de vehicules
	int left_vehicles_count = 0;
	int right_vehicles_count = 0;
	int last_left_instant_count = 0;
	int last_right_instant_count = 0;
	
	// Lit chaque frame une par une
	bool isReading = true;
	while(isReading) {
		// Get frame from stream
		isReading = m_cap.read(m_frame); // stocke l'image extraite de m_cap dans m_frame (allocation dynamique lors du premier appel seulement) 
		// Remarque : m_cap est un objet VideoCapture qui contient entre-autre un pointeur interne vers la dernière frame lue (pas besoin de stocker)
		
		if(!isReading) { std::cerr << "Unable to read device or file." << std::endl; }
		else { // Traitement
			// Affiche la frame dans la fenêtre principale
			imshow("Video", m_frame);

			// Affiche les contours
			affiche_contours(m_frame);

			// Suivi des véhicules
            if (ref_precedente.empty()) {
                // Si c'est la première frame, initialise la référence précédente
                m_frame.copyTo(ref_precedente);
            }
            vehicles_contours = follow_vehicles(m_frame, ref_courante, ref_precedente);
			count_vehicles(m_frame, vehicles_contours,
               left_vehicles_count, right_vehicles_count,
               last_left_instant_count, last_right_instant_count)
			;

		}

		// If escape key is pressed, quit the program
		if(waitKey(timeToWait)%256 == 27) {
			std::cout << "Stopped by user" << std::endl;
			isReading = false;
		}	
	}	
}

bool Camera::close() {
	// Close the stream
	m_cap.release();
	
	// Close all the windows
	destroyAllWindows();
	usleep(100000);

	return true;
}

