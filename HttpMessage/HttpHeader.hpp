#pragma once

#include <string>

namespace ws {
	class HttpHeader {

	private:
		/* ================================= */
		/*              required             */
		/* ================================= */

		/*
		body's exactly size
		if not exist body content, optional
		 */
		std::string::size_type _content_length;

		/*
		keep-alive: maintaining after connection to server
		close: close after connection to server
		*/
		std::string _connection;

		std::string	_content_type;

		/*
		chunked 인지 확인하는 필드
		*/
		std::string _transfer_encoding;

		/*
		ex) 127.0.0.1:8080
		포트 안들어오면 80포트고
		호스트 부분 server_name이 들어올 수도 있음
		*/
		std::string _host;

		/*
		Millisecond from 1970 to the present
		*/
		std::string _date;

		/* ================================= */
		/*               option              */
		/* ================================= */

		/*
		Client가 받을수 있는 mime type
		*/
		std::string _accept;

		/*
		Client가 받을수 있는 encoding 타입
		*/
		std::string _accept_encoding;

		/*
		Client가 받을수 있는 언어
		*/
		std::string _accept_language;

		/*
		프로토콜://호스트:포트
		*/
		std::string _referer;

		/*
		만료시간을 알려주는데 뭔지 모르겠다 잘 사용 안할듯?
		*/
		std::string _expires;

		/*
		필요할거 같아서 넣음
		*/
		std::string	_retry_after;

		HttpHeader(const HttpHeader& cls);
		HttpHeader& operator=(const HttpHeader& cls);

	public:
		HttpHeader();
		~HttpHeader();

		std::string::size_type get_content_length() const throw();
		std::string get_connection() const throw();
		std::string get_content_type() const throw();
		std::string get_transfer_encoding() const throw();
		std::string get_host() const throw();
		std::string get_date() const throw();
		std::string get_accept() const throw();
		std::string get_accept_encoding() const throw();
		std::string get_accept_language() const throw();
		std::string get_referer() const throw();
		std::string get_expires() const throw();
		std::string get_retry_after() const throw();

		void set_content_length(const std::string& var) throw();
		void set_connection(const std::string& var) throw();
		void set_content_type(const std::string& var) throw();
		void set_transfer_encoding(const std::string& var) throw();
		void set_host(const std::string& var) throw();
		void set_date(const std::string& var) throw();
		void set_accept(const std::string& var) throw();
		void set_accept_encoding(const std::string& var) throw();
		void set_accept_language(const std::string& var) throw();
		void set_referer(const std::string& var) throw();
		void set_expires(const std::string& var) throw();
		void set_retry_after(const std::string& var) throw();
	};
}