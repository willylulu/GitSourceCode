#include "wingjean.h"

float t_data[17 * 6] = {
	302.8664,-34.1103,443.5220,
	0.1876,0.1055,-0.9766,

	277.9408,30.5724,231.0771,
	-0.5608,0.0080,-0.8279,

	289.2813,35.6185,-42.3676,
	-0.9649,-0.2286,0.1290,

	230.4682,33.0213,-196.6580,
	0.7975,-0.0738,-0.5988,

	89.3463,36.4091,-263.5290,
	-0.2927,-0.2204,0.9305,

	38.2826,66.6921,-135.0522,
	-0.0346,-0.4096,0.9116,

	30.2549,38.5205,-58.9848,
	-0.2000,-0.5413,0.8167,

	-19.1705,18.8891,-43.1104,
	-0.9914,-0.0846,0.0997,

	-85.3560,19.5341,-32.9201,
	-0.8794,-0.4692,-0.0803,

	-154.7104,0.2994,-11.0514,
	-0.9555,0.0021,-0.2949,

	-161.4461,-0.5138,-48.1768,
	-0.0518,0.0403,-0.9978,

	-150.0522, 0.0980, -81.3971,
	0.7311, -0.2468, -0.6360,

	-116.4063, 0.0980, -112.5000,
	0.4707, 0.1357, -0.8718,

	-89.8438, -1.8627, -156.9445,
	0.9994, 0.0277, 0.0214,

	-17.4949,21.9232,-170.2035,
	0.8132,-0.3411,-0.4715,

	94.7979,34.2327,-204.5926,
	0.9297,0.2312,-0.2866,

	389.4624, 66.7647, -370.4186,
	-0.2052, 0.0756, -0.9758
};

WingJean::WingJean(FirstMan *firstMan, bool* flying, bool* dragLock)
{
	this->data = t_data;
	this->target = -1;
	this->eyePosition = firstMan->getPtrPos();
	this->eyeDirection = firstMan->getPtrFor();
	this->flying = flying;
	this->dragLock = dragLock;
}

float i = 0;

void WingJean::getWingJean()
{
	if (target == -1)return;
	vec3 targetPos = vec3(data[target * 6], data[target * 6 + 1], data[target * 6 + 2]);
	vec3 targetDir = vec3(data[target * 6 + 3], data[target * 6 + 4], data[target * 6 + 5]);
	if (target == 0)
	{
		*flying = true;
		vec3 posVector = targetPos - originPos;
		vec3 dirVector = targetDir - originDir;
		*eyePosition += posVector / 100.0f;
		*eyeDirection += dirVector / 100.0f;
	}
	else
	{
		vec3 lastPos = vec3(data[(target - 1) * 6], data[(target - 1) * 6 + 1], data[(target - 1) * 6 + 2]);
		vec3 lastDir = vec3(data[(target - 1) * 6 + 3], data[(target - 1) * 6 + 4], data[(target - 1) * 6 + 5]);
		vec3 posVector = targetPos - lastPos;
		vec3 dirVector = targetDir - lastDir;
		*eyePosition += posVector / 100.0f;
		*eyeDirection += dirVector / 100.0f;
	}
	vec3 judgePos = targetPos - *eyePosition;
	if (i == 100)
	{
		*eyePosition = targetPos;
		*eyeDirection = targetDir;
		target++;
		i = 0;
		if (target == 17)endWingJean();
	}
	else i++;
}


void WingJean::startWingJean()
{
	target = 0;
	originPos = *eyePosition;
	originDir = *eyeDirection;
	*dragLock = true;
}

void WingJean::endWingJean()
{
	*flying = false;
	*dragLock = false;
	target = -1;
	*eyeDirection = originDir;
}

