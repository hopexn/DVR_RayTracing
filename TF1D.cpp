#include "TF1D.h"

void TF1D::loadTF(string filename) {
    FILE *fp = fopen(filename.c_str(), "r");
    if (fp == NULL) {
        cout << "Cannot open file: " << filename << endl;
        return;
    }
    int key_num;
    Key key;
    fscanf(fp, "%d", &key_num);
    for (int i = 0; i < key_num; i++) {
        fscanf(fp, "%d %f %f %f %f", &key.index, &key.color.a, &key.color.r, &key.color.g, &key.color.b);
        keys.push_back(key);
    }
}

glm::vec4 TF1D::trans_func(float index) {
    int i;
    for (i = 0; i < keys.size() - 1; i++) {
        if (keys[i].index <= index && keys[i + 1].index >= index) {
            break;
        }
    }
    glm::vec4 color1 = keys[i].color;
    glm::vec4 color2 = keys[i + 1].color;
    color1 *= 1.0f * (keys[i + 1].index - index) / (keys[i + 1].index - keys[i].index);
    color2 *= 1.0f * (index - keys[i].index) / (keys[i + 1].index - keys[i].index);
    color1 += color2;
    return color1;
}