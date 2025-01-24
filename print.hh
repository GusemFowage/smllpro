#pragma once
#include <iostream>
#include <format>
#include <cstdio>

#define STD ::std::

namespace {

	using STD format;

	template <typename ... A>
	void print(STD FILE* p, STD string_view fmt, A&&...a) {
		fputs(STD vformat(fmt, STD make_format_args(a...)).c_str(), p);
	}

	template <typename ... A>
	void print(STD ostream& out, STD string_view fmt, A&&...a) {
		out << STD vformat(fmt, STD make_format_args(a...));
	}

	template<typename ... A>
	void print(STD format_string<A...> fmt, A&&...a) {
		print(STD cout, fmt.get(), a...);
	}

	template <typename ... A>
	void println(STD FILE* p, STD string_view fmt, A&&...a) {
		fputs(STD vformat(fmt, STD make_format_args(a...)).c_str(), p);
		fputc('\n', p);
	}

	template <typename ... A>
	void println(STD ostream& out, STD string_view fmt, A&&...a) {
		out << STD vformat(fmt, STD make_format_args(a...)) << STD endl;
	}

	template<typename ... A>
	void println(STD format_string<A...> fmt, A&&...a) {
		println(STD cout, fmt.get(), a...);
	}

	using STD getline;

}