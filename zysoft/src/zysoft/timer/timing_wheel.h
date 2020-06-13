#pragma once

#include <algorithm>
#include <numeric>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <array>
#include <memory>
#include <map>
#include <unordered_set>
#include <vector>

namespace zysoft
{

template <typename T, std::size_t N>
class timing_wheel
{
public:
    static_assert(N != 0, "N != 0");

    using size_type = std::size_t;
    using duration_type = std::uint64_t;

    struct entry
    {
        entry(std::uint64_t tick, std::uint64_t d, T&& v)
            : tick_(tick), duration_(d), value_(std::move(v))
        {
        }
        std::uint64_t tick_;
        const std::uint64_t duration_;
        T value_;
    };
    using entry_ptr = std::shared_ptr<entry>;
    using timer_handler = std::weak_ptr<entry>;

    static constexpr size_type static_capacity = N;

private:
    struct slot
    {
        std::multimap<std::uint64_t, entry_ptr> entries_;
    };

public:
    timing_wheel()
        : wheel_()
        , immediate_entries_()
        , all_entries_()
        , current_tick()
    {
        for (std::size_t i = 0; i != wheel_.size(); ++i)
            wheel_[i] = std::make_unique<slot>();
    }

    ~timing_wheel()
    {
    }

    void tick()
    {
        ++current_tick;
    }

    void get_expired(std::vector<std::pair<T, std::uint64_t>>& out)
    {
        return get_expired(current_tick, out);
    }

    std::vector<std::pair<T, std::uint64_t>> get_expired()
    {
        std::vector<std::pair<T, std::uint64_t>> vec;
        get_expired(current_tick, vec);
        return vec;
    }

    void get_expired(std::uint64_t t, std::vector<std::pair<T, std::uint64_t>>& expired)
    {
        assert(all_entries_.size() == wheel_entries_count() + immediate_entries_.size());
        for (const auto& p : immediate_entries_) {
            all_entries_.erase(p);
            expired.emplace_back(std::move(p->value_), p->duration_);
        }
        immediate_entries_.clear();

        std::uint64_t idx = current_index();
        slot* s = wheel_[idx].get();

        // TODO lower_bound ??
        //auto it_end = s->entries_.lower_bound(t);
        auto it_end = s->entries_.upper_bound(t);
        for (auto it = s->entries_.begin(); it != it_end; ++it) {
            const auto& p = it->second;
            all_entries_.erase(p);
            expired.emplace_back(std::move(p->value_), p->duration_);
        }
        s->entries_.erase(s->entries_.begin(), it_end);
        assert(all_entries_.size() == wheel_entries_count() + immediate_entries_.size());
    }

    timer_handler add(T&& v, duration_type duration)
    {
        assert(all_entries_.size() == wheel_entries_count() + immediate_entries_.size());
        if (duration == 0) {
            auto e = std::make_shared<entry>(current_tick, duration, std::forward<T&&>(v));
            immediate_entries_.insert(e);
            all_entries_.insert(e);
            return timer_handler{e};
        }

        std::uint64_t new_tick = duration + current_tick;
        std::uint64_t index = (duration + current_tick) % static_capacity;
        slot* s = wheel_[index].get();
        auto e = std::make_shared<entry>(new_tick, duration, std::forward<T&&>(v));
        s->entries_.insert(std::pair(e->tick_, e));
        all_entries_.insert(e);
        assert(all_entries_.size() == wheel_entries_count() + immediate_entries_.size());
        return timer_handler{e};
    }

    void cancel(timer_handler hdl)
    {
        entry_ptr p = hdl.lock();
        if (!p)
            return;
        assert(all_entries_.size() == wheel_entries_count() + immediate_entries_.size());
        auto it = all_entries_.find(p);
        if (it != all_entries_.end()) {
            entry_ptr e = *it;
            slot* s = get_slot(e);
            const entry* ret = slot_remove_entry(s, e);
            assert(ret);
            all_entries_.erase(it);
        }
        assert(all_entries_.size() == wheel_entries_count() + immediate_entries_.size());
    }

    void reset(timer_handler hdl)
    {
        entry_ptr p = hdl.lock();
        if (!p || p->duration_ == 0)
            return;
        assert(all_entries_.size() == wheel_entries_count() + immediate_entries_.size());
        auto it = all_entries_.find(p);
        if (it != all_entries_.end()) {
            entry_ptr e = *it;
            if (e->tick_ == e->duration_ + current_tick)
                return;

            slot* s = get_slot(e);
            const entry* ret = slot_remove_entry(s, e);
            assert(ret);

            e->tick_ = e->duration_ + current_tick;
            s = get_slot(e);
            s->entries_.insert(std::pair(e->tick_, e));
        }
        assert(all_entries_.size() == wheel_entries_count() + immediate_entries_.size());
    }

    void clear()
    {
        assert(all_entries_.size() == wheel_entries_count() + immediate_entries_.size());
        for (auto& it : wheel_) {
            it->entries_.clear();
        }
        immediate_entries_.clear();
        all_entries_.clear();
        current_tick = 0;
        assert(all_entries_.size() == wheel_entries_count() + immediate_entries_.size());
    }

    size_type size() const
    {
        assert(all_entries_.size() == wheel_entries_count() + immediate_entries_.size());
        return all_entries_.size();
    }

private:
    std::size_t wheel_entries_count() const 
    {
        return std::accumulate(wheel_.begin(), wheel_.end(), std::size_t(0),
            [](std::size_t n, const std::unique_ptr<slot>& p) 
            {
                return p->entries_.size() + n;
            }
        );
    }

    std::uint64_t current_index() const
    {
        return current_tick % static_capacity;
    }

    slot* get_slot(const entry_ptr& p)
    {
        auto idx = p->tick_%static_capacity;
        return wheel_[idx].get();
    }

    static const entry* slot_remove_entry(slot* s, const entry_ptr& p)
    {
        std::multimap<std::uint64_t, entry_ptr>::iterator it = s->entries_.end();
        auto it_pair = s->entries_.equal_range(p->tick_);
        for (auto i = it_pair.first; i != it_pair.second; ++i) {
            if (i->second == p) {
                it = i;
                break;
            }
        }
        auto it2 = s->entries_.erase(it);
        return it2 != s->entries_.end() ? p.get() : nullptr;
    }

private:
    using wheel_type = std::array<std::unique_ptr<slot>, static_capacity>;
    wheel_type                      wheel_;
    std::unordered_set<entry_ptr>   immediate_entries_;
    std::unordered_set<entry_ptr>   all_entries_;
    std::uint64_t                   current_tick;
};

} // namespace zysoft

