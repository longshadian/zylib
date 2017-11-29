#pragma once

#include <memory>

namespace knet {

class EventCB;
class ProducerDeliveryReportCB;
class ConsumerRebalanceCB;
class ConsumerConsumeCB;
class ConsumerOffsetCommitCB;

using EventCBUPtr                   = std::unique_ptr<EventCB>;
using ProducerDeliveryReportCBUPtr  = std::unique_ptr<ProducerDeliveryReportCB>;
using ConsumerRebalanceCBUPtr       = std::unique_ptr<ConsumerRebalanceCB>;
using ConsumerConsumeCBUPtr         = std::unique_ptr<ConsumerConsumeCB>;
using ConsumerOffsetCommitCBUPtr    = std::unique_ptr<ConsumerOffsetCommitCB>;

} // knet
