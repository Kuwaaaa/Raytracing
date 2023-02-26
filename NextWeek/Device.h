#pragma once

enum class DEVICE_TYPE {
	DEVICE_CPI,
	DEVICE_GL
};

class RenderInfo {

};

class Device
{

public:
	virtual void render(int ssp) = 0;

private:
	DEVICE_TYPE type;
	RenderInfo info;
};

