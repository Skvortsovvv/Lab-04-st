#pragma once

#include <math.h>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <utility>
#include <map>
#include <algorithm>
#include <fstream>


int GetRandomFromRange(int min, int max)
{
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	srand(std::chrono::steady_clock::now().time_since_epoch().count());
	return static_cast<int>(rand() * fraction * (max - min) + min);
}

double average_distance(const std::vector<int>& permutation) {
	double distance_sum = 0;
	for (int i = 0; i < 1000; i++)
		distance_sum += abs(permutation[i] - i);

	return distance_sum / 1000;
}


double NumberConstructor(int a, int b, int number) {
	double result = number;
	while (result >= 1) {
		result /= 10;
	}
	int temp = GetRandomFromRange(a, b);
	if (temp == b) {
		temp -= 1;
	}
	return temp + result;
}

double RandomNumberGenerator(double a, double b) {
	std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
	std::vector<int> permutation(1000);

	for (int i = 0; i < 1000; i++)
		permutation[i] = i;
	shuffle(permutation.begin(), permutation.end(), rng);
	double part1 = average_distance(permutation);
	shuffle(permutation.begin(), permutation.end(), rng);
	double part2 = average_distance(permutation);
	int index = permutation[static_cast<int>(abs(a) + abs(b) + 1) % 1000] + 1;
	double a_ = part1 * part2;
	int b_ = ceil(a_);
	int result = b_ % index;
	return NumberConstructor(a, b, result);
}


struct Chromosome {
	double x;
	double y;
	Chromosome() {
		y = x = 0;
	}
	Chromosome(double a, double b) {
		x = a;
		y = b;
	}
	Chromosome(int a,int b) {
		x = RandomNumberGenerator(a, b);
		y = RandomNumberGenerator(a, b);
	}
};


bool operator!=(const Chromosome& lhs, const Chromosome& rhs) {
	return (lhs.x != rhs.x && lhs.y != rhs.y);
}


bool operator!=(const std::pair < double, Chromosome>& lhs,const std::pair < double, Chromosome>& rhs) {
	return (lhs.first != rhs.first && lhs.second != rhs.second);
}


bool compare(const std::pair<double, Chromosome>& lhs, const std::pair<double, Chromosome>& rhs) {
	return lhs.first < rhs.first;
}



class GeneticAlgorithm {
private:
	std::vector<Chromosome> chromo;
	std::vector<std::pair<double,Chromosome>> values;
public:

	GeneticAlgorithm(int a, int b) {
		for (size_t i = 0; i < 4; ++i) {
			chromo.push_back({ a, b });
		}
	}

	void Algorithm() {
		std::ofstream ofs("output.txt");
		if (!ofs) {
			return;
		}
		int N = 0;
		while (N <= 10) {
			values = {};
			for (const auto& obj : chromo) {
				values.push_back({ FitFuncion(obj), obj });
			}
			sort(values.rbegin(), values.rend(), compare);
			double average_val = AverageResult();
			auto max_res = MaxResult();
			int counter = 0;
			writeIntoFile(ofs, N);
			Print();
			std::pair<Chromosome, Chromosome> temp_pair;
			for (size_t i = 0; i < 4; ++i) {
				if (values[i].first >= average_val) {
					++counter;
				}
			}
			if (counter == 1) {
				temp_pair = Cross(max_res->second, values[1].second);
				chromo[0] = temp_pair.first;
				chromo[1] = temp_pair.second;
				temp_pair = Cross(max_res->second, values[2].second);
				chromo[2] = temp_pair.first;
				chromo[3] = temp_pair.second;
			}
			else if (counter == 2) {
				if (values[0] != values[1]) {
					temp_pair = Cross(values[0].second, values[1].second);
					chromo[0] = temp_pair.first;
					chromo[1] = temp_pair.second;
					temp_pair = Cross(values[0].second, values[2].second);
					chromo[2] = temp_pair.first;
					chromo[3] = temp_pair.second;
				}
				else {
					temp_pair = Cross(values[0].second, values[2].second);
					chromo[0] = temp_pair.first;
					chromo[1] = temp_pair.second;
					temp_pair = Cross(values[0].second, values[3].second);
					chromo[2] = temp_pair.first;
					chromo[3] = temp_pair.second;
				}
			}
			else if (counter == 3) {
				if (values[0] != values[1]) {
					if (values[1] != values[2]) {
						temp_pair = Cross(values[1].second, values[2].second);
						chromo[2] = temp_pair.first;
						chromo[3] = temp_pair.second;
					}
					else {
						temp_pair = Cross(values[0].second, values[3].second);
						chromo[2] = temp_pair.first;
						chromo[3] = temp_pair.second;
					}
					temp_pair = Cross(values[0].second, values[1].second);
					chromo[0] = temp_pair.first;
					chromo[1] = temp_pair.second;
				}
				else {
					if (values[1] != values[2]) {
						temp_pair = Cross(values[1].second, values[2].second);
						chromo[0] = temp_pair.first;
						chromo[1] = temp_pair.second;
						temp_pair = Cross(values[1].second, values[3].second);
						chromo[2] = temp_pair.first;
						chromo[3] = temp_pair.second;

					}
					else {
						temp_pair = Cross(values[0].second, values[3].second);
						chromo[0] = temp_pair.second;
						chromo[1] = values[2].second;
						chromo[2] = values[1].second;
						chromo[3] = temp_pair.first;
					}
				}
			}
			/*if (N == 10) {
				std::cout << MaxResult()->first << ' ' << MaxResult()->second.x << ' ' << MaxResult()->second.x;
			}*/
			++N;
		}
		std::cout<<"X: " << MaxResult()->second.x <<" Y: "<< MaxResult()->second.y<< " Value: " << MaxResult()->first << std::endl;
	}


	void writeIntoFile(std::ofstream& fs, int N) {
		for (const auto& obj : values) {
			fs << "(" << N << ";" << AverageResult() << ")";
		}
	}


	void Print() {
		int i = 1;
		for (const auto& obj : values) {
			std::cout << "Dot" << i << ": x: " << obj.second.x << " y: " << obj.second.y << " value: " << obj.first << std::endl;
			++i;
		}
		std::cout << "Max value: " << MaxResult()->first << " Average: " << AverageResult() << std::endl << std::endl;
	}


	std::pair<Chromosome, Chromosome> Cross(const Chromosome& obj1, const Chromosome& obj2) {
		return { { obj1.x,obj2.y }, { obj2.x,obj1.y } };
	}

	//std::pair<Chromosome, Chromosome> Crossover(int index1, int index2) {
	//	return Cross(chromo[index1], chromo[index2]);
	//}


	void Mutation(std::pair<Chromosome, Chromosome>& crs_pair) {
		if (RandomNumberGenerator(0, 1) < 0.25) {
			while (true) {
				double delta_x = pow(-1, GetRandomFromRange(1, 3)) * (0.1 + RandomNumberGenerator(0, 1) / 10);
				double delta_y = pow(-1, GetRandomFromRange(1, 3)) * (0.1 + RandomNumberGenerator(0, 1) / 10);
				if (crs_pair.first.x+delta_x <= 2 && crs_pair.first.x + delta_x >= -2) {
					if (crs_pair.first.y + delta_y <= 2 && crs_pair.first.y + delta_y >= -2) {
						if (crs_pair.second.x + delta_x <= 2 && crs_pair.second.x + delta_x >= -2) {
							if (crs_pair.second.y + delta_y <= 2 && crs_pair.second.y + delta_y >= -2) {
								crs_pair.first.x += delta_x;
								crs_pair.first.y += delta_y;
								crs_pair.second.x += delta_x;
								crs_pair.second.y += delta_y;
								break;
							}
						}
					}
				}

			}

		}
	}


	double FitFuncion(const Chromosome& obj) {
		return sin(obj.x) / (1 + pow(obj.x, 2) + pow(obj.y, 2));
	}


	double AverageResult() {
		double Average = 0;
		for (const auto& y : values) {
			Average += y.first;
		}
		return (Average / values.size());
	}

	std::vector<std::pair<double, Chromosome>>::iterator  MaxResult() {
		return values.begin();
	}

};

