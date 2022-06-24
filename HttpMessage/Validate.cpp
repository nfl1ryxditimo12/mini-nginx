
	// if (!check_method(_method, ))
		// throw; //403 error : 금지된 메소드 사용


	// if (!check_uri(_request_uri))
	// 	throw; //403 error : 콘텐츠 접근 미승인


	// if (_http_version != "HTTP/1.1")
		// throw; 




	//transfer-encoding이 chunked -> chunked body
	// for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); it++) {
	//content-length
		// if (it->first == "Content-Length")
		// 	if (!check_content_length(it->second, _body.length()))
		// 		throw; //411error : (POST요청에) content-length가 없음
	// ...
