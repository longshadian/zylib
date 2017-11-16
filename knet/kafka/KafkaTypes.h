#pragma once

#include <memory>

namespace knet {

class ReplayEventCB;
class ProducerDeliveryReportCB;
class ConsumerRebalanceCB;
class ConsumerConsumeCB;
class ConsumerOffsetCommitCB;

using ReplayEventCBUPtr = std::unique_ptr<ReplayEventCB>;
using ProducerDeliveryReportCBUPtr = std::unique_ptr<ProducerDeliveryReportCB>;
using ConsumerRebalanceCBUPtr = std::unique_ptr<ConsumerRebalanceCB>;
using ConsumerConsumeCBUPtr = std::unique_ptr<ConsumerConsumeCB>;

} // knet
