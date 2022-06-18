#ifndef CONFIGUREOPTION_HPP
# define CONFIGUREOPTION_HPP

# include <iostream>
# include <vector>
# include <map>

namespace ws
{
	class ConfigureOption
	{
		private:
			std::map<int, std::string>	_error_page;
			std::string					_root;
			std::vector<std::string>	_index;
			int							_client_max_body_size;
			int							_directory_flag;

		public:
			ConfigureOption();
			~ConfigureOption();
	};
}

#endif