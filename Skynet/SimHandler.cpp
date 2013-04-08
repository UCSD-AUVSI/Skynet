#include "SimHandler.h"
#include "PlaneWatcher.h"

using namespace System;
using namespace Communications;

Simulator::SimHandler::SimHandler(PlaneWatcher ^ watcher): watcher(watcher){}

bool Simulator::SimHandler::load(String ^ const filename){
	// TODO: Implement
	return false;
}

bool Simulator::SimHandler::nextFrame(){
	// TODO: Implement
	return false;
}

bool Simulator::SimHandler::previousFrame(){
	// TODO: Implement
	return false;
}