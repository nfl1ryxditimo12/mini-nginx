#pragma once

namespace ws {

  /* =================================== */
  /*           HTTP Status Code          */
  /* =================================== */

  enum Code {
    /* 200 Status Code */
    OK                    = 200,
    CREATED               = 201,

    /* 300 Status Code */

    /* 400 Status Code */
    BAD_REQUEST           = 400,
    UNAUTHORIZED          = 401,
    FORBIDDEN             = 403,
    NOT_FOUND             = 404,

    /* 500 Status Code */
    INTERNAL_SERVER_ERROR = 500
  };
  
}
