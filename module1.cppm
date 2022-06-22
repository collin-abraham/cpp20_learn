/* module1.cppm
* 2022-06-21
* Collin Abraham
*
* This is an example module, a new feature included in the cpp20 standard
* Only get_return_words() is accessible due to export keyword
*/

export module cpp20learning;

namespace cpp20learning {
	auto return_words() { return "This string came from a module!"; }
	export auto get_return_words() { return return_words; }
}