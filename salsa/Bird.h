#ifndef BIRD_H
#define BIRD_H

#include "ObjModel.h"
#include "texture.hpp"


class Bird {

public:
    /*
     * Load object files and store vertices, normals, uvs
     */
    Bird() : body("./models/bird/eagle/parts/body.obj"),
             head("./models/bird/eagle/parts/head.obj"),
             tail("./models/bird/eagle/parts/tail.obj"),
             left_wing_far("./models/bird/eagle/parts/left_wings_outer.obj"),
             left_wing_close("./models/bird/eagle/parts/left_wings_inner.obj"),
             right_wing_far("./models/bird/eagle/parts/right_wings_outer.obj"),
             right_wing_close("./models/bird/eagle/parts/right_wings_inner.obj"),
             texture("./models/bird/eagle/texture/full.png") {
        std::cout << "Loaded Bird parts" << std::endl;
    }


    void init();
    void draw();

    /* Getters
     */
    Texture& getTexture() { return this->texture; }

private:
    // whether to animate the bird (moving wings, tail, ...) or not
    bool animate;

    // whether to move the bird (on some path) or not
    bool move;

    // all parts of the bird (ObjModels, drawable independently)
    ObjModel body;
    ObjModel head;
    ObjModel tail;
    ObjModel left_wing_far;
    ObjModel left_wing_close;
    ObjModel right_wing_far;
    ObjModel right_wing_close;

    // texture
    Texture texture;
};

#endif // BIRD_H