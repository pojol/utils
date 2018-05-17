
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <random>
#include <iostream>

#include "aoi.h"

int main()
{
	AoiScene<int, int> scene;

	/*
	std::random_device _rd;
	std::uniform_int_distribution<int> uni_dist(0, 7);


	auto _begin = get_system_tick();
	for (int i = 0; i < 200; ++i)
	{
		scene.add(i, uni_dist(_rd), uni_dist(_rd), 5, [&](int id, int x, int y) {}
			, [&](int id, int x, int y) {}
		, [&](int id, int x, int y) {});
	}

	std::cout << "add complete " << get_system_tick() - _begin << std::endl;

	while (true)
	{
		_begin = get_system_tick();

		for (int i = 0; i < 2000; ++i)
		{
			scene.move(i, uni_dist(_rd), uni_dist(_rd));
		}

		std::cout << "tick cosume " << get_system_tick() - _begin << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	*/


	scene.add(1, 0, 0, 5, [&](int id, int x, int y) {
		std::cout << "object " << id << " enter " << 1 << " range" << std::endl;
	}, [&](int id, int x, int y) {
		std::cout << id << " send " << 1 << " move pos change " << x << "," << y << std::endl;
	}, [&](int id, int x, int y) {
		std::cout << "object " << id << " leave " << 1 << " range" << std::endl;
	});

	scene.add(2, 5, 5, 5, [&](int id, int x, int y) {
		std::cout << "object " << id << " enter " << 2 << " range" << std::endl;
	}, [&](int id, int x, int y) {
		std::cout << id << " send " << 2 << " move pos change " << x << "," << y << std::endl;
	}, [&](int id, int x, int y) {
		std::cout << "object " << id << " leave " << 2 << " range" << std::endl;
	});

	scene.move(2, 5, 6);
	scene.move(2, 4, 5);
	scene.move(2, 5, 4);
	scene.add(3, 4, 4, 5, [&](int id, int x, int y) {
		std::cout << "object " << id << " enter " << 3 << " range" << std::endl;
	}, [&](int id, int x, int y) {
		std::cout << id << " send " << 3 << " move pos change " << x << "," << y << std::endl;
	}, [&](int id, int x, int y) {
		std::cout << "object " << id << " leave " << 3 << " range" << std::endl;
	});


	system("pause");
	return 0;
}