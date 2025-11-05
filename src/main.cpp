#include "../inc/camera.hpp"

// NE JAMAIS NOMMER LES FENETRES imgshow() AVEC DES ACCENTS

int main(void) {
	Camera myCam;
	
	myCam.open("cctv.avi");
	
	myCam.play();
	
	myCam.close();
	
	return 0;
}
