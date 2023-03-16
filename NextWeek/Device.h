#pragma once

#include "camera.h"
#include "scene/scene.h"


enum class DEVICE_TYPE {
	DEVICE_CPU,
	DEVICE_GL
};

class RenderInfo {

};

class Device
{
public:
	virtual void render(int ssp) = 0;
	bool saveImg(char* fileName);

protected:
	Device(Scene scene, Camera cam);

protected:
	DEVICE_TYPE m_type;
	RenderInfo info;
	Scene m_scene;
	Camera m_camera;
	double ssp_sum = 0;
};

