#pragma once

#include <fstream>

void test_vector(std::fstream& test_file);
void test_stack(std::fstream& test_file);
void test_map(std::fstream& test_file);

void benchmark_vector(std::fstream& benchmark_file);
void benchmark_stack(std::fstream& benchmark_file);
void benchmark_map(std::fstream& benchmark_file);
