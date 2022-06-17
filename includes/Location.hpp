#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "ConfigureOption.hpp"

namespace ws
{
	enum Method
	{
		GET,
		POST,
		DELETE
	};

	class Location
	{
		private:
			std::vector<ws::Method>				_limit_except;
			std::pair<int, std::string>			_return;
			std::pair<std::string, std::string>	_cgi;
			ws::ConfigureOption					_option;

		public:
			Location();
			~Location();
	};
}

#endif