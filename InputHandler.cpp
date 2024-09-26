#include "InputHandler.h"

#include <iostream>

#include "ProcessManager.h"

void InputHandler::printHeader() {

}

void InputHandler::initialize() {

}

void InputHandler::screen() {

}

void InputHandler::schedulerTest() {
  
}

void InputHandler::schedulerStop() {
    
}

void InputHandler::reportUtil() {
    
}

void InputHandler::clear() {
    
}

void InputHandler::newProcess(std::string name, int linesOfCode) {
    ProcessManager::getInstance()->createProcess(name, linesOfCode);
}