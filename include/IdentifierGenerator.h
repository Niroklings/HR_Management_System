#ifndef IDENTIFIERGENERATOR_H
#define IDENTIFIERGENERATOR_H

class IdentifierGenerator {
private:
    static int nextInNum; 

public:
    static int getNextId();
};

#endif // IDENTIFIERGENERATOR_H