#pragma once

#include <memory>

namespace knet {

namespace detail {

class EventCB;
class DeliveryReportCB;
class RebalanceCB;
class ConsumeCB;
class OffsetCommitCB;

using EventCBUPtr           = std::unique_ptr<EventCB>;
using DeliveryReportCBUPtr  = std::unique_ptr<DeliveryReportCB>;
using RebalanceCBUPtr       = std::unique_ptr<RebalanceCB>;
using ConsumeCBUPtr         = std::unique_ptr<ConsumeCB>;
using OffsetCommitCBUPtr    = std::unique_ptr<OffsetCommitCB>;

} // detail

} // knet
