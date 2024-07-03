#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace sjtu
{

	class exception
	{
	protected:
		const std::string variant = "";
		std::string detail = "";

	public:
		exception() {}
		exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
		virtual std::string what()
		{
			return variant + " " + detail;
		}
	};

	class index_out_of_bound : public exception
	{
		index_out_of_bound()
		{
			exception::detail = "index out of bound";
		}
	};

	class runtime_error : public exception
	{
		runtime_error()
		{
			exception::detail = "run time error";
		}
	};

	class invalid_iterator : public exception
	{
		invalid_iterator()
		{
			exception::detail = "invalid iterator";
		}
	};

	class container_is_empty : public exception
	{
		container_is_empty()
		{
			exception::detail = "container is empty";
		}
	};
}

#endif
