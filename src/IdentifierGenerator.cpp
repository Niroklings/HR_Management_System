#include "IdentifierGenerator.h"

int IdentifierGenerator::nextInNum = 1;

int IdentifierGenerator::getNextId() {
    return nextInNum++; 
}