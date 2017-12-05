// EventEmitter.h
#ifndef EVENTEMITTER_H
#define EVENTEMITTER_H

#include <functional>
#include <vector>
#include <map>

template <typename E, typename F>
struct EventEmitter;

template <typename E, typename R, typename ...Args>
struct EventEmitter<E, R(Args...)> {
public:
    using Handler = std::function<R(Args...)>;
    using AllHandler = std::function<R(E, Args...)>;
    typedef size_t ID;

private:
    std::map<E, std::map<size_t, Handler>> _events;
    std::map<size_t, AllHandler> _all;
    std::map<size_t, E> _index;
    size_t _next_id_ = 1;

public:
    ID on(E event, Handler handler) {
        auto id = _next_id_++;
        this->_events[event][id] = handler;
        this->_index[id] = event;
        return id;
    }

    ID once(E event, Handler handler) {
        auto id = _next_id_++;
        this->_events[event][id] = [this, id, handler](Args... args){
            this->off(id);
            return handler(args...);
        };
        this->_index[id] = event;
        return id;
    }

    ID onAll(AllHandler handler) {
        auto id = _next_id_++;
        this->_all[id] = handler;
        return id;
    }

    ID onceAll(AllHandler handler) {
        auto id = _next_id_++;
        this->_all[id] = [this, id, handler](E event, Args... args){
            this->off(id);
            return handler(event, args...);
        };
        return id;
    }

    void off() {
        this->_events.clear();
        this->_all.clear();
    }

    void off(E event) {
        this->_events.erase(event);
    }

    void offAll() {
        this->_all.clear();
    }

    void off(ID id) {
        if (this->_index.find(id) != this->_index.end()) {
            this->_events[this->_index[id]].erase(id);
        } else {
            this->_all.erase(id);
        }
    }

    void emit(E event, Args... args){
        for (auto& it : this->_events[event]) {
            it.second(args...);
        }
        for (auto& it : this->_all) {
            it.second(event, args...);
        }
    }
};

#endif // EVENTEMITTER_H
