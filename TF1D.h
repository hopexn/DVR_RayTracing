#ifndef RAYTRACING_TF1D_H
#define RAYTRACING_TF1D_H

#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

using namespace std;

struct Key {
    int index;
    glm::vec4 color;
};

class TF1D {
public:
    TF1D(){};
    void loadTF(string filename);
    glm::vec4 trans_func(float index);
    vector<Key> keys;
};
#endif //RAYTRACING_TF1D_H
