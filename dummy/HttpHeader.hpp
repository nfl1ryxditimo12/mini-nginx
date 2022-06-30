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
		엔터티헤더
		*/
		std::string::size_type _content_length;

		/*
		keep-alive: maintaining after connection to server (1.0)
		close: close after connection to server (1.1)
		헤더의 이름에 대응되는 토큰의 목록
		일반헤더
		*/
		std::string _connection;

		/*
		이 메세지에 담긴 객체의 미디어 타입
		엔터티헤더
		content-type: text/html;
		*/
		std::string	_content_type;

		/*
		chunked 인지 확인하는 필드
		chunked가 아니면 에러 ?
		*/
		std::string _transfer_encoding;

		/*
		ex) 127.0.0.1:8080
		포트 안들어오면 80포트고
		호스트 부분 server_name이 들어올 수도 있음
		클라이언트의 인터넷 호스트명과 포트번호를 서버에게 제공하기 위해 사용
		호스트 명에 근거한 상대 url을 서버가 구별할 수 있게 해줌 -> 같은 IP주소에서 여러 다른 호스트 명을 호스팅 할 수 있게됨
		host가 없는 요청 헤더가 들어오면 400error로 응답
		syntax:
			Host: host[":"port]
		*/
		std::string _host;

		// /*
		// Millisecond from 1970 to the present
		// 메세지가 언제 만들어졌는지에 대한 날짜와 시간을 제공
		// 일반헤더
		// RFC 850형식:
		// Date: Tuesday, 03-Oct-97 02:15:31 GMT
		// */
		// std::string _date;

		/* ================================= */
		/*               option              */
		/* ================================= */

		/*
		Client가 받을수 있는 mime type
		text/txt?
		*/
		std::string _accept;

		/*
		Client가 받을수 있는 encoding 타입
		syntax:
			Accept-Encoding: 1#((content-coding | "*")[";" "q" "=" qvalue])
		*/
		std::string _accept_encoding;

		/*
		Client가 받을수 있는 언어
		syntax:
			Accept-Language: 1# (language-range[][";" "q" "=" qvalue])
			language-range = ((1*8ALPHA * ("-" !*8ALPHA)) | "*")
		*/
		std::string _accept_language;

		/*
		프로토콜://호스트:포트
		사용자가 링크를 클릭해 이동했을 때 이전 사이트의 url을 기록
		서버에게 클라이언트가 그 url을 어디서 얻었는지 알려주기 위해 클라이언트 요청에 들어간다
		직접 타이핑한 url은 포함되지 않는다
		요청헤더
		syntax:
			Referer: (absolute URL | relative URL)
		*/
		std::string _referer;

		/*
		만료시간을 알려주는데 뭔지 모르겠다 잘 사용 안할듯?
		응답이 더이상 유효하지 않게 되는 일시를 알려줌
		클라이언트가 사본을 캐시할 수 있게 해주며 클라이언트는 이 일시가 만료되기 전까지 서버에게 캐시된 사본이 유효한지 물어보지 않아도 된다
		엔터티헤더
		*/
		std::string _expires;

		/*
		필요할거 같아서 넣음
		503error로 응답할 때 클라이언트에게 언제 리소스에 대한 요청을 다시 시도할 수 있는지 말해줌
		또한 클라이언트에게 리소스를 리다이렉트 해줄 때 기다려야하는 시간을 알려줌
		*/
		std::string	_retry_after;

		HttpHeader& operator=(const HttpHeader& other);
		HttpHeader(const HttpHeader& other);

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
