/* main.cpp
* 2022-06-21
* Collin Abraham
* 
* This program explores my learning of the major features changes to the C++ standard that came in 2020
*/

#define _CRT_SECURE_NO_WARNINGS // allow for ctime 

#include <iostream>		// import <iostream>; will be supported but as of this Visual Studio version this isn't possible yet
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <ranges>
#include <string>
#include <experimental/generator>
#include <ctime>
#include <chrono>
#include <coroutine>
#include <type_traits>
#include <compare>
#include <numeric>
#include <version>
#include <numbers> 
#include <source_location>
#include <bit>
#include <bitset>
#include <cmath>

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
		| views::transform([](const auto& x) { return x * 3; }) // multiply all vector elements by 3
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
* current i returned to main and waits
*/
experimental::generator<int> return_seq_generator (const int& startVal, const size_t& numVals) {

	for (int i{ startVal }; i < startVal + numVals; ++i) {
		time_t systime { chrono::system_clock::to_time_t(chrono::system_clock::now()) };
		cout << ctime(&systime);
		co_yield i;
	}
}

/* cpp20 - concepts
* Predicates evaluated at compile time that constrain template parameters  
* Several ways to implement a concept demonstrated below
* This concept ensures that value can be decremented 
* There are many standard concepts built into the language, such as same, derived_from, convertable_to, constructible, sortable, mergable etc...
* Concepts Help the compiler produce nicer error messages .. templated error messages can be nightmare inducing 
*/

/* concept 1 */
template<typename T>
concept can_decrement = requires(T value) { 
	value--; 
	--value; 
};

/* implementation methods of a concept */
template<can_decrement T>
void concept_ex1(T type);

template<typename T> requires can_decrement<T>
void concept_ex2(T type);

template<typename T>
void concept_ex3(T type) requires can_decrement<T>;

void concept_ex4(can_decrement auto type);

/* concept 2 - size must be convertable to a size_t and cannot exceed size_t(1000) in size */
template<typename T>
concept size_check = requires(T & a) {
	{ a.size() } -> std::convertible_to<std::size_t>;
	{ a.size() <= size_t(1000) };
};

/* concepts can be combined together */
template <typename T> requires can_decrement<T> && size_check<T>
void concept_ex5(T type);

template <typename T> 
concept combined_concept = can_decrement<T> && size_check<T>;
void concept_ex6(combined_concept auto type);

/* error message example */
void concept_ex7(can_decrement auto type) { cout << "Do something"; };

struct Bar {};



/* cpp20 - lambda changes
* Pre cpp20 if you captured by value, the lambda would implicitly capture 'this'
* you must be explicit, so lambda will look like [=, this] in captures
* Support for template lambda expressions
*/
void lambda_changes() {
	/* several options to use templates with lambdas */
	auto a = [] <typename T> (T val) { return val + 1; };
	auto b = [] <typename T> (T * val) { return *val * 5; };
	auto c = [] <typename T, int intVal> (T(&a)[intVal]) { return a + intVal; };

	/* generic lambdas can accept a vector<T> and can deduce T easier than pre cpp20 */
	auto d = [] <typename T>(const vector<T>&vec) {
		T x{ }; // do whatever processing you want
		T::static_function();
	};

	/* variadic set of parameters to std::forward */
	auto e = [](auto&& ...args) { return foo(forward<decltype(args)>(args)...); };		// perfect forwarding pre cpp20

	auto f = []<typename ...T>(T&& ... args) { return foo(forward<T>(args)...); };		// simplified forwarding in cpp20


}

/* lambda pack expansion
* 1st auto g [](){} is a simple capture followed by ellipsis: pack expansion
* Pre cpp20 this was legal as there was no init within the capture
*/
template <class A, class... Args>
auto g = [](A a, Args... args) {
	return[a, args...]{
		return invoke(a, args...);
	};
};

/* init capture is now supported .. call move now, for instance  */
//template <class A, class... Args>
//auto h = [](A a, Args... args) {
//	return[a = std::move(a), args = std::move(args)...]{
//		return invoke(a, args...);
//	};
//};


/* cpp 20 constexpr changes 
* Virtual polymorphic functions can be constexpr
* can use dynamic_cast() and typeid
* do dynamic memory allocations, must remember to explicitly use new/delete (be careful)
* can now contain try/catch - but doesn't throw exceptions 
* std::string and std::vector are now constexpr support 
*
*/

constexpr size_t constexpr_example() {
	const string theStrings[] = { "Billy", "Jimmy" };
	vector<string> vec;
	for (const auto& str : theStrings) { vec.push_back(str); };
	return vec.size();
}

/* cpp20 concurrency changes
* shared_ptr is thread safe.. guarantees obj is deallocated exactly once
* BUT accessing the pointer is not thread safe.. one pointer might be reading ptr and the other may be storing a new ptr
* can manually use a mutex to protect access to smart ptr 
*	can also use global non-member atomic operations .. atomic_load()
*	error prone -> very easy to accidentally not use global non-member atomic operations 
* atomic<shared_ptr<T>>
* 
* My experience and knowledge with threads is lacking, so i'm not going to try to tackle understanding the new features
* until I get some more experience with concurrency period (soon) 
*/

/* cpp20 synchronization library
* <semaphore>
*	lightweight synchronization primtiives.. can implement any other sync concept (mutex, latches, barriers)
*	couting semaphore models non-negative resource count
*	binary semaphore only has 1 slot -> free slow or no free slot which is ideal for a mutex
* 
* <latch>
*	coordination point within a thread 
*	thread block at a latch point until a iven number of threads reach the latch point
*	at which point all threads are allowed to continue 
*	a std::latch is a single use object
* 
* <barrier>
*	a sequence of phases
*	number of threads block until req number of threads arrive at a barrier 
*		a phase completion callback is executed
*		thread counter is reset
*		next phase starts
*		threads can continue
* 
* <atomic>
*	atomic reference
*	operates almost the same as std::atomic but works with references 
*	whereas std::atomic always copies the value it is provided
* 
*/

void concurrency_examples() {
	/* TO-DO:
	* learn threading and get updated on threading changes
	*/
}

/* cpp20 designated initializers
* aggregates can be designated init 
* 
* The following code declares a simple class with a string member, then defines a function
* which uses designated init to set the member as "bob" right away 
*/
struct Something {
	string member;
};

void constructSomething() {
	Something a { .member = "bob" };		
}

/* cpp20 three way comparison operator (also known as Spaceship operator)
* can compare two objects and then compare the result with 0
* similar to the old C-style strcmp() 
*	returning -n, 0 or +n 
* 
* This function creates an int i = 4000, then uses std::strong_ordering::less/greater/equal 
* to determine if it's state when compared against the number 0, stored in theResult 
*/

void spaceship_operator_example() {

	cout << "\n\nSpaceship operator example: ";

	const int i = 4000;
	strong_ordering theResult{ i <=> 0 };

	if (theResult == strong_ordering::less) { cout << "it's less"; }
	if (theResult == strong_ordering::greater) { cout << "it's greater"; }
	if (theResult == strong_ordering::equal) { cout << "it's equal"; }
	
	cout << endl;
}

/* <compare> using cpp20 
* library heavily simplifies writing operator overloads for classes 
* compiler auto generates all six standard comparisons == != > < >= <= 
* all standard library types include support: vector, string, map, set etc. 
* 
* Declare a simple class, auto generate comparison operators <=>
* call a function to see if operators work 
*/
class CompareClass {
	int x;

public:
	CompareClass(const auto& x) { this->x = x; };

public:
	auto operator<=>(const CompareClass&) const = default; 
};

void compare_class_example() {
	CompareClass foo(11);
	CompareClass bar(10);
	
	// lets try out some of the comparison operators (foo > bar)
	if (foo == bar)
		cout << "\n\nfoo == bar!\n";
	else if (foo > bar)
			cout << "\n\n foo > bar!\n";
	else if (foo < bar)
			cout << "\n\n foo < bar!\n";
	else
		cout << "\n\n foo != bar!\n";
}

/* cpp20 range-based for loop init
* 
* Declare a simple class with vector<int> as a member, create a function that returns a new obj of that class
* call a function that initializes the object within a ranged for loop 
*/

struct RangedBasedLoopClass { vector<int> member; };

RangedBasedLoopClass return_data() { return RangedBasedLoopClass(); }

void loop_init_ex() {
	int i = 10;

	for (auto data{ return_data() }; auto & val : data.member) {
		/* do something */
	}
}

/* cpp20 - non-type template parameters 
* in the past non-type template param had limitations
* could not use string literals...
* float is allowed
* some class types are allowed 
* 
* 'auto usecase' gives an example usecase where you can create a compile time regex match with the
* CTRE library .. rather than a run-time regex match 
*/

// auto usecase{ ctre::match<"[a-z].+([0-9]+)%">(str) };


/* cpp20 [[likely]] and [[unlikely]]
* gives hints to compiler to aid in optimization of conditional branches
* 
* This simple function declare an int i = 100, passes it into a switch and we tell the compiler that it's likely that it is greater
* than or equal to 50 and unlikely to not be >= 50.. optimization
*/
void likely_unlikely_example() {
	const int i = 100;

	switch (i) {
		[[likely]] case (i >= 50):
			break;
		[[unlikey]] case (i < 50):
			break;
	}
}

/* cpp20 calendars & timezones 
* <chrono> extended to support calendars
* Gregorian is now supported
* 
* The following code explores the various new functionalities of the <chrono> library, placing their data into lambdas
* and printing the results at the end (overloaded << operator allows stream insertion) 
*/

void chrono_examples() {
	// construct a year, 2 ways
	auto a { chrono::year { 2022 } };
	auto b { 2022y };

	// construct a month, 2 ways
	auto c { chrono::month { 06 } };
	auto d { chrono::June };

	// construct a full date as a std::chrono::year_month_day 
	auto e { 2022y / chrono::June / 27d };

	// construct a full day using the 2nd Tuesday of July 2022, for ex
	auto f { chrono::Tuesday[2] / chrono::July / 2022 };

	// <chrono> added support for days, weeks, months, years type alias.. pre-cpp20 seconds, minutes, hours 
	using days = chrono::duration<int, ratio_multiply<ratio<24>, chrono::hours::period >> ;
	using weeks = chrono::duration<int, ratio_multiply<ratio<7>, chrono::days::period>>; 
	using months = chrono::duration<int, ratio_multiply<ratio<4>, chrono::weeks::period>>; // assuming a month is always 4 weeks 
	using years = chrono::duration<int, ratio_multiply<ratio<12>, chrono::months::period>>;

	weeks w{ 1 }; // constructs 1 week
	days d1 { w }; // convert one week into days
	months m1 { 2 }; // constructs 2 months 
	days d2{ m1 }; // convert 2 months into days (will return 8 weeks, which translates to 7 days/week * 4 weeks/month * 2 = 56 

	// 4 new clocks that can be used 
	auto g = chrono::utc_clock(); // coordinated universal time, measures time since jan 1st 1970, including leap seconds
	auto h = chrono::tai_clock(); // international atomic time (TAI) measures time since jan 1st 1958 .. offset 10 seconds ahead of UTC without leap seconds
	auto i = chrono::gps_clock(); // global positioning system time, measure time since jan 1st 1980, without leap seconds
	auto j = chrono::file_clock(); // alias for std::filesystem::file_time_type, unspecific epoch 

	// new type alias sys_time which is a chrono::time_point of a chrono::system_clock within a chorno::duration 
	using sys_sec = chrono::sys_time<chrono::seconds>;
	using sys_days = chrono::sys_time<chrono::days>;

	chrono::system_clock::time_point t { sys_days { 2022y / chrono::June / 27d} }; // returns a conversion from date -> time_point
	auto k { chrono::year_month_day { floor<chrono::days>(t) } }; // returns a converson from time_point -> date

	// date and time can be combined painlessly
	auto l { sys_days{2022y / chrono::June / 27d} + 10h + 50min + 15s }; // 2022-06-27 10:50:15 UTC time 

	// timezone conversions 
	chrono::zoned_time local_denver { "America/Denver", l }; // conv the previous sys_days lambda to Miami timezone 

	// what is my current local time?
	auto m{ chrono::zoned_time { chrono::current_zone(), chrono::system_clock::now() } };

	// Ok.. let's output some of these times, they are designed to be able to use the regular stream insertion operator 
	cout << "\n\n<chrono> changes and timezones: \n";
	cout << "------------------------------\n";
	cout << a << endl;
	cout << b << endl;
	cout << c << endl;
	cout << d << endl;
	cout << e << endl;
	cout << f << endl;
	cout << k << endl;
	cout << l << endl;
	cout << m << endl;
	cout << "------------------------------\n";
}

/* cpp20 <span>
* view over some contiguous data, does not own the data
* no allocations/deallocations
* can be either read/write
* can be dynamic sized (runtime) or fixed sized (compile time)
* very cheap to copy, suggested to pass by value (as is the case with string_view)
* does not support strides 
* 
* The following code declares different ways of using spans then
* prints them to the screen in an interesting way using ranges 
*/

void span_example() {
	char datum[50];

	// following spans are read and write capable 
	span <char, 50> theSpan{ datum }; // fixed size of 100 chars 
	span <char> theSpan2{ datum }; // dynamic size of 100 chars 

	// read only span
	span<const char> theSpanReadonly{ datum }; // declare the <T> as const, don't declare the entire span as const 

	// spans support most of the std::algorithm uses
	auto spanBeg = theSpan.begin();
	auto spanfront = theSpan.front();
	auto spanEmpty = theSpan.empty();
	auto spanSize = theSpan.size();

	// fill the array with increasing chars starting at 'a' using writeable span
	iota(begin(theSpan), end(theSpan), 'a'); 

	// read the array using read-only span.. both are accessing the same data stored 
	// output results with one increasing char per line at a max column width of 30
	const auto colsize { 30 };
	const auto rowsize { theSpanReadonly.size() - colsize + 1};

	cout << endl << "Span output: " << endl;
	for (auto offset{ 0 }; offset < rowsize; ++offset) {
		ranges::for_each(theSpanReadonly.subspan(offset, colsize), putchar);
		putchar('\n');
	}	
}

/* cpp20 feature testing macros
* does the compiler support language or library features?
* 
* <version>
* implementation dependent information about whatever library you're using 
* version number, release date, copywrite notice, implementation-defined info
* includes library feature test macros:
*	__cpp_lib_filesystem   etc...
* 
* This function shows some feature testing macros to see how our compiler is handling headers and libraries from the standard 
*/
void test_macros() {
	__cpp_binary_literals;	// support literals?
	__cpp_char8_t; // support for utf-8 chars?
	__cpp_lib_coroutine; // support for coroutine library features?
	__cpp_lib_ranges; // support for ranges? 

// for example, lets use the new features to see if the <optional> library is available : 

#if __has_include(<optional>)					// does the compiler actually include <optional?>
	#include <optional>							// if so lets include it 
	#if __cpp_lib_optional;						// check if we are actually defining the optional library 
		#define has_optional 1					// if we are, then we do have optional
	#endif
#elif __has_include(<experimental/optional>)	// maybe we have the experimental/optional header? 
	#include<experimental/optional>				// if so, add the experimental version and follow same steps as above 
	#if __cpp_lib_experimental_optional
		#define has_optional 1
		#define optional_is_experimental 1
	#endif
#endif
}

/* cpp20 immediate functions 
* constexpr may be called at compile time, but that still isn't necessarily a hard requirement 
* 
* following code demonstrates the difference between constexpr and consteval 
*/

// --- use constexpr 
constexpr auto yardToCm(double yard) { return yard * 91.44; };

const double const_yard{ 2 };
const auto a{ yardToCm(const_yard) };	// evaluated at compile time

double dyn_yard{ 5 };
const auto b{ yardToCm(dyn_yard) }; // evaluated at run time, but what if that's not what you want?

// --- use const eval
consteval auto yardToCmEval (double yard) { return yard * 91.44; };	// guarantees that calls to yardToCmEval are executed at compile-time

constexpr double constexpr_yard{ 2 };
const auto c{ yardToCmEval(constexpr_yard) }; // evaluated at compile, everything is constant 

//const auto d{ yardToCmEval(dyn_yard) };	// impossible sice dyn_yard isn't const, can't be guaranteed at compile time 

/* cpp20 constinit
* allows for guaranteed const initialization values.. 
* this helps to avoid bugs as a result of undefined order of dynamic initializations 
* can create static variables with const initializers
* 
* following code demonstrates how constinit checks that the initializaton only occurs
* when it is used after calling a constexpr function 
*/

const char* foo() { return "this has dynamic init"; }
constexpr const char* bar(bool statement) { return statement ? "const init" : foo(); }
constinit const char* foobar = bar(true);
//constinit const char* fuubar = bar(false); // impossible because it tries to call the non constexpr foo()


/* cpp20 class enums & using directive 
* cpp11 class enums were strongly typed.. annoying 
* simplifies using enums 
*/
enum class Seasons {Spring, Summer, Fall, Winter};

// cpp11 had to explicitly declare the parent scope
string_view returnString(const Seasons currentSeason) {
	switch (currentSeason) {
		case Seasons::Spring: return "It's Spring!";
		case Seasons::Summer: return "It's Summer!";
		case Seasons::Fall: return "It's Fall!";
		case Seasons::Winter: return "It's Winter!";
	}
}

// cpp20 using directive.. be careful, use a small scope or you end up
// exporting everything again and run into C-style enum issues
string_view returnStringCpp20(const Seasons currentSeason) {
	switch (currentSeason) {
		using enum Seasons;
		case Spring: return "It's Spring!";
		case Summer: return "It's Summer!";
		case Fall: return "It's Fall!";
		case Winter: return "It's Winter!";
	}

}

/* cpp20 text formatting (std::format)
* I/O streams 
*	safe + extensible
*	hard to read and localize
*	no separation of formatting string and args
* printf()
*	not safe or extensible
*	easy to read, no << insertion operators
*	separates formating string and args
*	easy to localize
* 
* std::format in cpp20 combines i/o streams and printf()
*	easy to read, no need for << 
*	safe and extensible
*	positional args
*	easy to localize
*	performs better than sprintf(), ostringstream, to_string()
*	no reason to not use it, honestly
*/

template<typename... Args>
string print_dynamically(string_view returnedFrom, Args&&... args) {
	return std::vformat(returnedFrom, std::make_format_args(args...));
}

void formatting_example() {
	cout << format("\n{:=^20}", "A line of text");	// fills empty space with a total of 20 =
	cout << format("\nRead {0} bytes from {1}\n", 100, "file1.txt"); // if you are reading in file1.txt through a stream

	//also convenient to use with variadic params
	string formatting = "";
	int reps = 0;
	while (reps != 3) {
		formatting += "{} "; // the formatting string to be used 
		cout << formatting << " : ";
		cout << print_dynamically(formatting, "bob", 's', 42, "not used");
		cout << '\n';
		++reps;
	}
}


/* cpp20 <numbers> 
* defines many mathematical constants clearly
* 
* many to choose from, here are a few examples
*/
void numbers_example() {
	
	const auto a = numbers::e;
	const auto b = numbers::log2e;
	const auto c = numbers::pi;
	const auto d = numbers::sqrt2;
	const auto e = numbers::phi; 

	// and many more in the std::numbers namespace 
}


/* cpp20 <source_location>
* represents info about a location within source code.. 
*	line, column, file_name, function_name
* don't have to use any preproccesor macros anymore
* 
* The following code returns the file path we're working with and what line we're at for that source_location call
*/
void sourcelocation_example() {
	const source_location& myLocation = source_location::current();
	cout << "\nCurrent file: " << myLocation.file_name() << " at line#: " << myLocation.line() << endl;

}


/* cpp20 nodiscard(reason)
* was added in cpp11 but now you can give an arbirary reason why the return value
* of the function should not be discarded
* 
* This code returns a raw pointer that needs to be dealt with, else it causes a memory leak
* and the nodiscard expains what is going on
*/

[[nodiscard("Ignoring return value may cause a memory leak")]]
int* nodiscard_memoryleak() {
	int* value = new int(50);
	return value;
}


/* cpp20 <bit>
* global non-member functions that operate on bits
* to rotate bits, use: rotl(), rotr()   
* to count bits, use: 
*	countl_zero()		number of consecutive 0 bits starting at most significant 
*	countl_one()		number of consecutive 1 bits starting at most significant
*	countr_zero()		number of consecutive 0 bits starting at least significant
*	countr_one()		number of consecutive 1 bits starting at least significant
*	popcount()			number of 1 bits
*/

void bit_example() {
	const uint8_t num = 0b00111010;

	cout << "\nRotate " << bitset<8>(num) << " left by 2 " << bitset<8>(rotl(num,2)) << '\n';
	cout << "Rotate " << bitset<8>(num) << " left by 3 " << bitset<8>(rotl(num, 3)) << '\n';
	cout << "Rotate " << bitset<8>(num) << " left by 4 " << bitset<8>(rotl(num, 2)) << '\n';
	cout << "Rotate " << bitset<8>(num) << " left by -1 " << bitset<8>(rotl(num, -1)) << '\n';
	cout << "Num of 0 bits after most significant: " << countl_zero(num) << '\n';
	cout << "Num of 1 bits after most significant: " << countl_one(num) << '\n';
	cout << "Num of 1 bits: " << popcount(num) << '\n';
}


/* cpp20 soe extra std libary additions
* starts_with() and ends_with() for string/string_view
* contains() for associative containers 
* remove(), remove_if() and unique() for list/forward_list return size_type instead of void
* shift_left() and shift_right() added to <algorithm>
* erase() and erase_if() added for all containers
* midpoint() calculate midpoint of two numbers
* lerp() linear interpolation
* unsequenced_policy(execution::unseq) algorithm is allowed to be vectorized, but not paralellised 
* 
* Following code explores each of these features except unsequenced_policy()
*/

/* helper func to print a generic container */
template<typename CONTAINER_TYPE>
void printContainer(const CONTAINER_TYPE& cont) {
	for (const auto& x : cont) { cout << x << " "; };
}

void new_std_features() {
	// starts_with()
	const string str{ "Great balls of fire!" };
	bool check{ str.starts_with("Gre") };
	cout << "\n Great balls of fire starts with Gre? " << (check ? "true" : "false") << endl; 

	// contains() 
	const map newMap{ pair {1,"bobs"}, {2,"sallys"},{3,"jimmys"} };
	cout << "\nMap contents:\n";
	for_each(begin(newMap), end(newMap), [](const auto& x) { cout << x.first << " " << x.second << endl; } );
	bool mapCheck{ newMap.contains(2)};
	cout << "\nDoes the map contain 2? " << (mapCheck ? "true" : "false") << endl;

	// remove()
	list<int> newList{ 5,17,54,30,100,7,92 };
	cout << "\nList contents:\n";
	printContainer(newList);
	const auto removeReturn = remove(newList.begin(), newList.end(), 54);
	cout << "\nList contents after removing:\n";
	printContainer(newList);
	cout << "\nremoveReturn contains: " << *removeReturn << endl;	
	// the docs said this returns a size_type of removed elements.. 
	// but appears to still return an interator to where the element was removed from 

	// shift_left()
	vector<int> aVec{ 5,43,8,23,30,101,44,32 };
	cout << "\nVector contents: \n";
	printContainer(aVec);
	shift_left(begin(aVec), end(aVec), 1);
	cout << "\nVector contents after shift_left(): \n";
	printContainer(aVec);

	// erase() 
	aVec.erase(find(begin(aVec), end(aVec), 101));
	cout << "\n\nVector contents after calling erase(101): \n";
	printContainer(aVec);

	//midpoint()
	const int a = 4324324;
	const int b = 9829342;
	const auto c = midpoint(a, b);
	cout << "\n\nMidpoint() between a: " << a << " and b: " << b << " is " << c << endl; 

	//lerp()
	// from wikipedia: In mathematics, linear interpolation is a method of curve fitting using linear polynomials to construct new data points 
	// within the range of a discrete set of known data points.
	// so if a == (1,1) and b == (3,3) ... the linear interpolation between them is the line connecting 1,1 to 3,3 
	cout << "Linear Interpolation between 5.0 and 10.0 using jumps of 1 : " << endl; 
	for (auto i{ -5.0 }; i <= 5.0; i += 1)
		cout << endl << lerp(5.0, 10.0, i);
}



int main() {


/* the preproccesor enabled examples take up a lot of console space, 
* declare them true/false to see their results after running the functions 
*/
#define COROUTINE_EXAMPLE false
#define CONCEPT_ERROR_EXAMPLE false
#define CHRONO_EXAMPLE false
#define SPAN_EXAMPLE false

#if COROUTINE_EXAMPLE
	cout << "\n\nCoroutine example: \n";
	auto generator_time{ return_seq_generator(15,20) };
	for (const auto& x : generator_time) { 
		cout << x << " Press enter for next value " << endl; 
		cin.ignore();
	}
#endif

#if CONCEPT_ERROR_EXAMPLE
	Bar a;
	concept_ex7(a); // produces error c7602 associated constraints are not satisfied
#endif

#if CHRONO_EXAMPLE
	chrono_examples();
#endif 

#if SPAN_EXAMPLE
	span_example();
#endif
	
	ranges_example();

	lambda_changes();

	auto foo = constexpr_example();

	spaceship_operator_example();

	compare_class_example();

	loop_init_ex();

	likely_unlikely_example();

	test_macros();

	formatting_example();

	numbers_example();

	sourcelocation_example();

	auto toDelete = nodiscard_memoryleak();
	delete toDelete; 

	bit_example();

	new_std_features();
}