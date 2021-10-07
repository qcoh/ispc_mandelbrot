#pragma once

#include <string>
#include <chrono>
#include <iostream>

struct benchmark {
	std::string m_msg;
	std::chrono::high_resolution_clock::time_point start;

	benchmark(std::string const& msg) : m_msg{msg}, start{std::chrono::high_resolution_clock::now()} {}
	~benchmark() {
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
		std::cout << m_msg << ": " << duration.count() << '\n';
	}
};

