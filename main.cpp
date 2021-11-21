#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::cout << "Hello, world!\n";

	std::vector<int> arr;
	int n;
	std::cout << "Enter the number of elements in the array : ";
	std::cin >> n;

	std::cout << "Enter the elements in the array: \n";
	for(int i = 0; i < n; i++) {
		int temp;
		std::cin >> temp;
		arr.push_back(temp);
	}

	std::cout << "The elements of the array 'arr' is : " << std::endl;
	for(int x : arr) {
		std::cout << x << ' ';
	}
	std::cout << std::endl;

	return 0;
}
