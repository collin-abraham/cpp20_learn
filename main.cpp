/* main.cpp
* 2022-06-21
* Collin Abraham
* 
* This program explores my learning of the major features changes to the C++ standard that came in 2020
*/

#define _CRT_SECURE_NO_WARNINGS // allow for ctime 

#include <iostream>		// import <iostream>; will be supported but as of this Visual Studio version this isn't possible yet
#include <vector>
#include <algorithm>
#include <ranges>
#include <string>
#include <experimental/generator>
#include <ctime>
#include <chrono>
#include <coroutine>



using namespace std; 

/* cpp20 - modules
* Aim is to replace header files.. explicitly state what you want from it (classes/functions)
* Module interface files and implementation files is possible, but not necessary 
* Modules are built only once, faster build times
* Order of modules isn't important
* Preprocessor macors have no effect on and also do not leak from modules  
* Headers can be importable but depends on compiler 
* 
* This import statement includes a custom made module called cpp20learning (module1.cppm)
*/

import cpp20learning; // see module1.cppm in modules folder for details 
void use_module() {
	// call the module namespace and store as an auto
	auto moduleValue = cpp20learning::get_return_words();
}

/* cpp20 - ranges
* Ranges are objects referring to a sequence of elements; similar to a .begin() .end() iterator pair
* They are a nice clean syntax to use 
* Any container that has a .begin() .end() are valid to use with ranges
* Almost all standard library algorithms accept range 
* Projection: transform elements of a container before being used by an algorithm
* View: transform/filter a range without ownership 
* Range factories: produce views on demand
* Pipelining: ranges are able to be created or changed in a row using pipes | 
* 
* This function explores much of the power and usability with ranges and views within ranges.
* Code is commented throughout the show processing 
*/
void ranges_example() {
	// pre c++20 
	vector<int> somedata{ 45,7,2,22,100,64 };
	sort(begin(somedata), end(somedata));

	// c++20
	vector<int> somedata2{ 5,23,76,23,8,22 };
	ranges::sort(somedata2);

	// piping views 
	vector<int> somedata3{ 6,12,64,43,12,32,65,23 };
	auto viewsResult{ somedata3
		| views::transform([](const auto& x) { return x * 3; }) // multiply all vector elements by 3.0
		| views::drop(2) // delete the first 2 elements of the container 
		| views::reverse // reverse contents 
		| views::transform([](const auto& x) { return to_string(x); }) // return contents as a string per element
	};
	// all of the view is lazy executed, so nothing is done unless you iterate over viewsResult

	// show the view contents 
	cout << "Views executed on vector returned: ";
	auto jointView = ranges::join_view(viewsResult);
	for (auto x : viewsResult) { cout << x << " "; }

	//  values can also be filtered using a lambda func
	const auto useValues = { 0,1,2,3,4,5,6,7,8,9,10 }; 
	auto odd = [](const auto& x) { return x % 3 == 0; };  // lambda to determine odd numbers
	cout << "\n\nDisplaying only odd values from a 0-10 using views and lambdas: ";
	for (auto x : useValues | views::filter(odd)) { cout << x << " "; }

	// values can be transformed and worked into one composed statment too
	auto cubed = [](const auto& x) { return x * x * x; };	// returns a cubed version of the value 
	cout << "\n\nDisplaying values that are odd and cubed from 0-10 using filter views: ";
	for (auto x : views::transform(views::filter(useValues, odd), cubed)) { cout << x << " "; }

}

/* cpp20 - coroutines
* Function that contains:
*	co_await suspects while waiting for computation to finish
*	co_yield returns a value from a coroutine to caller - suspends coroutine
*	co_return returns from a co_routine, cannot just use 'return' 
* Coroutines simplify asynchronous input/output, even driven apps, generators, lazy computations 
* Visual C++ includes some helper clases std::experimental::generator<T>
* 
* This function loops returning the system time extracted from chrono::system_clock::now() .. 
* uses a coroutine to continually yield execution and wait for use input (cin.ignore())
*/
experimental::generator<int> return_seq_generator (const int& startVal, const size_t& numVals) {

	for (int i{ startVal }; i < startVal + numVals; ++i) {
		time_t systime { chrono::system_clock::to_time_t(chrono::system_clock::now()) };
		cout << ctime(&systime);
		co_yield i;
	}
}

int main() {

	ranges_example();

#define COROUTINE_EXAMPLE false

#if COROUTINE_EXAMPLE
	cout << "\n\nCoroutine example: \n";
	auto generator_time{ return_seq_generator(15,20) };
	for (const auto& x : generator_time) { 
		cout << x << " Press enter for next value " << endl; 
		cin.ignore();
	}
#endif
	
}