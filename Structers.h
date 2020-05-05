#pragma once
struct Vector
{
	float x, y, z;

};

struct Vehicle
{
	Vector position;
	Vector color;
	int height = 50;
	int width = 50;
	int scale_factor = 1;
	int start_speed = 0;
	int speed = start_speed;

	Vehicle(Vector position, Vector color, int scale_factor, int start_speed) : position(position), color(color), scale_factor(scale_factor), start_speed(start_speed) {}
};
