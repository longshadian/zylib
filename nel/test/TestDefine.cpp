#include "TestDefine.h"

#include "Types.h"

namespace def {

const NLNET::ServiceID GS_SID = 11;
const std::string GS_NAME = "GS";
const NLNET::CInetAddress GS_ADDR = { "127.0.0.1", 22001 };

const NLNET::ServiceID DBS_SID = 12;
const std::string DBS_NAME = "DBS";
const NLNET::CInetAddress DBS_ADDR = { "127.0.0.1", 22002 };

const std::string _REQ_TEST = "_REQ_TEST";
const std::string _RSP_TEST = "_RSP_TEST";

const std::string _REQ_USER_DATA = "_REQ_USER_DATA";
const std::string _RSP_USER_DATA = "_RSP_USER_DATA";

} // def
