#pragma once

namespace ws {

  /* =================================== */
  /*           HTTP Status Code          */
  /* =================================== */

  enum Code {
    /* 200 Status Code */
    OK                      = 200,
    CREATED                 = 201,
    NO_CONTENT              = 204,

    /* 300 Status Code */
    NOT_MODIFIED            = 304,
    
    /* 400 Status Code */
    BAD_REQUEST             = 400,
    UNAUTHORIZED            = 401,
    FORBIDDEN               = 403,
    NOT_FOUND               = 404,
    METHOD_NOT_ALLOWED      = 405,
    PAYLOAD_TOO_LARGE        = 413,
    UNSUPPORTED_MEDIA_TYPE  = 415,

    /* 500 Status Code */
    INTERNAL_SERVER_ERROR   = 500,
    HTTP_VERSION_NOT_SUPPORTED = 505
  };
  
}
