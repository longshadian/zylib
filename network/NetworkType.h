#pragma once

#include <memory>

class RWHandler;
class Message;

typedef std::shared_ptr<RWHandler>  RWHandlerPtr;
typedef std::shared_ptr<Message>    MessagePtr;
