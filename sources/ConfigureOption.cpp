#include "ConfigureOption.hpp"

ws::ConfigureOption::ConfigureOption()
{
	this->_root = "~/webserv/www/";
	this->_index.push_back("");
	this->_client_max_body_size = 1024 * 1024;
	this->_directory_flag = false;
}

ws::ConfigureOption::~ConfigureOption() {}
