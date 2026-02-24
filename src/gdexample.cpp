#include "gdexample.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void GDExample::_bind_methods() {
}

GDExample::GDExample() {
	// Initialize any variables here.
	time_passed = 0.0;
}

GDExample::~GDExample() {
	// Add your cleanup here.
}

void godot::GDExample::_ready()
{
	UtilityFunctions::print("C++ Node is officially initialized and ready!");
}

void GDExample::_process(double delta) {
	return; 
}