#pragma once
#include <string>
#include <GL/glew.h>
#include <iostream>

static const std::string MISSING_TEXTURE_PATH = "res/missing_texture";

class Texture {

public:
	static unsigned LoadImageToTexture(const std::string& filePath);
};
