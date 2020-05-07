#pragma once
struct Vector
{
	float x, y, z;

};

struct Vehicle
{
	int scale_factor ;
	Vector position;
	Vector color;
	int height = 50;
	int width = 50;
	
	int start_speed = 0;
	int speed = 0;

	Vehicle(Vector pposition, Vector color, int scale_factor, int pstart_speed) : position(pposition), color(color), scale_factor(scale_factor) {
		if (scale_factor == -1)
		{
			width *= -1;
			start_speed = -pstart_speed;
			position.x = 480 - pposition.x;

		}
		else {
			start_speed = pstart_speed;
		}
	}
};
