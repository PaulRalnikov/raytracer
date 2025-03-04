#include "scene.hpp"

void Scene::readTxt(std::string txt_path) {
    std::ifstream in;
    in.open(txt_path);

    std::vector<Primitive> primitives;

    std::string command;
    while (in >> command)
    {
        if (command == "DIMENSIONS")
        {
            in >> width >> height;
        }
        else if (command == "BG_COLOR")
        {
            in >> background_color.x >> background_color.y >> background_color.z;
        }
        else if (command == "CAMERA_POSITION")
        {
            in >> camera_position.x >> camera_position.y >> camera_position.z;
        }
        else if (command == "CAMERA_RIGHT")
        {
            in >> camera_right.x >> camera_right.y >> camera_right.z;
        }
        else if (command == "CAMERA_UP")
        {
            in >> camera_up.x >> camera_up.y >> camera_up.z;
        }
        else if (command == "CAMERA_FORWARD")
        {
            in >> camera_forward.x >> camera_forward.y >> camera_forward.z;
        }
        else if (command == "CAMERA_FOV_X")
        {
            in >> camera_fov;
        }
        else if (command == "NEW_PRIMITIVE")
        {
            primitives.push_back(Primitive());
        }
        else if (command == "PLANE")
        {
            primitives.back().type = PLANE;
            in >> primitives.back().geom;
        }
        else if (command == "ELLIPSOID")
        {
            primitives.back().type = ELLIPSOID;
            in >> primitives.back().geom;
        }
        else if (command == "BOX")
        {
            primitives.back().type = BOX;
            in >> primitives.back().geom;
        }
        else if (command == "POSITION")
        {
            in >> primitives.back().position;
        }
        else if (command == "ROTATION")
        {
            in >> primitives.back().rotation;
        }
        else if (command == "COLOR")
        {
            in >> primitives.back().color;
        }
    }
}
