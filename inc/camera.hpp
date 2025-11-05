#pragma once

#include <opencv2/opencv.hpp>
#include <string.h>

using namespace std;
using namespace cv;

class Camera {
	public:
		Camera(); // en C++, les constructeurs (pour instancier la classe) sont les méthodes de même nom que la classe
		// ~Camera(); // pas de destructeur nécessaire, l'objet ne possède aucune ressource non libérée par close() (voir le principe RAII)
		
		bool open(std::string name);
		void play();
		bool close();
		
	private:
		std::string m_fileName; // m_ comme member
		VideoCapture m_cap;
		int m_fps;
			
		Mat m_frame; // buffer qui stocke la vidéo frame par frame	
};
