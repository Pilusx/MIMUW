#ifndef JNP5_KEYED_QUEUE_H
#define JNP5_KEYED_QUEUE_H

#include <memory>
#include <list>
#include <map>
#include <vector>
#include <exception>

//[TODO] https://stackoverflow.com/questions/12833241/difference-between-c03-throw-specifier-c11-noexcept

// https://stackoverflow.com/questions/13842468/comma-in-c-c-macro
#define UNPACK(...) __VA_ARGS__

#define redeclare_pointed(return_type, func_name, other_func, invoke_params, decl_params, is_const_except, prefix, suffix, is_return) \
UNPACK return_type func_name decl_params  is_const_except { \
    prefix \
    is_return queuePtr-> other_func invoke_params; \
    suffix \
}

#define redeclare_pointed_copy_not_void(return_type, func_name, other_func, invoke_params, decl_params, is_const_except, prefix, suffix, is_return, set_enforce_copy) \
UNPACK return_type func_name decl_params  is_const_except { \
    copy_guard guard(&queuePtr, &enforceCopy, set_enforce_copy); \
    auto ret = queuePtr->other_func invoke_params; \
    guard.setSuccess(); \
    suffix \
    return ret; \
}

#define rd(return_type, func_name, invoke_params, decl_params, is_const_except, prefix, suffix) \
    redeclare_pointed(return_type, func_name, func_name, invoke_params, decl_params, is_const_except, prefix, , return)

#define rdvoidcopy(func_name, invoke_params, decl_params, is_const_except) \
    redeclare_pointed((void), func_name, func_name, invoke_params, decl_params, is_const_except, copy_guard guard(&queuePtr, &enforceCopy);, guard.setSuccess();, )

// Wersja noexcept
#define rdnevoid(func_name, invoke_params, decl_params, is_const, prefix, suffix) \
    redeclare_pointed((void), func_name, func_name, invoke_params, decl_params, is_const noexcept, prefix, suffix, )

#define rdcopy(return_type, func_name, invoke_params, decl_params, is_const_except, set_enforce_copy) \
    redeclare_pointed_copy_not_void(return_type, func_name, func_name, invoke_params, decl_params, is_const_except, , ,return, set_enforce_copy)

class lookup_error: public std::exception {
    const char* what() const throw() override {
        return "lookup_error";
    }
};

template <class K, class V>
class keyed_queue {
private:
    class keyed_queue_impl;
    class copy_guard;

    std::shared_ptr<keyed_queue_impl> queuePtr;
    bool enforceCopy = false;
    // https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Copy-on-write
    void makeCopyIfNotUnique() {
        if (!queuePtr.unique()) {
            keyed_queue_impl kqi(*queuePtr);
            queuePtr = std::make_shared<keyed_queue_impl>(kqi);
        }
    }

public:
    using k_iterator = typename keyed_queue_impl::iterator;

    keyed_queue() {
        keyed_queue_impl kqi;
        queuePtr = std::make_shared<keyed_queue_impl>(kqi);
    }

    keyed_queue(keyed_queue const &other) {
        if (other.enforceCopy) {
            keyed_queue_impl kqi(*other.queuePtr);
            queuePtr = std::make_shared<keyed_queue_impl>(kqi);
        } else {
            queuePtr = other.queuePtr;
        }
        enforceCopy = false;
    }

    keyed_queue(keyed_queue&& other) {
        queuePtr = std::move(other.queuePtr);
        enforceCopy = other.enforceCopy;
    }

    keyed_queue &operator=(keyed_queue other) {
        enforceCopy = other.enforceCopy;
        std::swap(queuePtr, other.queuePtr);
        return *this;
    }

    rdvoidcopy(push, (k, v), (K const &k, V const &v), )

    rdvoidcopy(pop, (), (), )

    rdvoidcopy(pop, (k), (K const &k), )

    rdvoidcopy(move_to_back, (k), (K const &k), )

    rdcopy((std::pair<K const &, V &>), front, (), (), ,  true)

    rdcopy((std::pair<K const &, V &>), back, (), (), , true)

    rd((std::pair<K const &, V const &>), front, (), (), const, ,)

    rd((std::pair<K const &, V const &>), back, (), (), const, ,)

    rdcopy((std::pair<K const &, V &>), first, (key), , (K const &key), true)

    rdcopy((std::pair<K const &, V &>), last, (key), , (K const &key), true)

    rd((std::pair<K const &, V const &>), first, (key), (K const &key), const, ,)

    rd((std::pair<K const &, V const &>), last, (key), (K const &key), const, ,)

    rd((k_iterator), k_begin, (), (), , ,)

    rd((k_iterator), k_end, (), (), , ,)

    rdnevoid(clear, (), (), , enforceCopy = false;, )

    size_t size() const noexcept {
        return queuePtr->elemList.size();
    }

    bool empty() const noexcept {
        return queuePtr->elemList.size() == 0;
    }

    size_t count(K const &k) const {
        return queuePtr->count(k);
    }
};

template<class K, class V>
class keyed_queue<K, V>::copy_guard {
private:
    bool success = false;
    bool oldEnforceCopy;
    bool* enforceCopyPtr;
    std::shared_ptr<keyed_queue_impl> old;
    std::shared_ptr<keyed_queue_impl>* current;

public:

    ~copy_guard() {
        if (!success && old) {
            *current = old;
        }
        if(!success) {
            *enforceCopyPtr = oldEnforceCopy;
        }
    }

    copy_guard(std::shared_ptr<keyed_queue_impl>* shr_ptr, bool* _enforceCopyPtr, bool setEnforceCopy = false) {
        current = shr_ptr;
        enforceCopyPtr = _enforceCopyPtr;
        // Remember old value of enforceCopy and cahnge the value of enforceCopy
        // if needed.
        oldEnforceCopy = *_enforceCopyPtr;
        *_enforceCopyPtr |= setEnforceCopy;
        if (!(current->unique())) {
            keyed_queue_impl kqi(**current);
            *current = std::make_shared<keyed_queue_impl>(kqi);
            // Set enforceCopy again, because after copying it is not dependent
            // on previous reference exposures.
            *enforceCopyPtr = setEnforceCopy;
        }
    }

    void setSuccess() {
        success = true;
    }
};

template<class K, class V>
class keyed_queue<K, V>::keyed_queue_impl {
      using elem_type = std::pair<K const*, V>;
      using list_iter = typename std::list<std::pair<K const*, V>>::iterator;
      using map_iter = typename std::map<K const, std::list<list_iter>>::iterator;

public:
    class iterator;
    // TODO zmienić camel case na snake case wszędzie (ku chwale spójności)
    // List containing ordered elements.
    std::list<elem_type> elemList;
    // Map key -> iterator lists, which allows finding key in log n.
    // Order of iterators on list is the same as on elemList.
    std::map<K const, std::list<list_iter>> keyMap;
        keyed_queue_impl() : elemList(), keyMap() {}

        keyed_queue_impl(keyed_queue_impl& other) : elemList(other.elemList){
            // Iterators in keyMap are pointing to wrong container.
            for (list_iter it = elemList.begin(); it != elemList.end(); ++it){
                auto keyIters = keyMap.find(*(it->first));
                if(keyIters == keyMap.end()) {
                    std::list<list_iter> newList{it};
                    keyMap.insert({*(it->first), newList});
                } else {
                    keyIters->second.push_back(it);
                }
            }
        }

        // Strong exception guarantee: uses only strong methods and reverses
        // changes in case of an exception.
        void push(K const &k, V const &v) {
            auto key_list_pair = keyMap.find(k);
            if (key_list_pair == keyMap.end()) {
                // map::insert has strong guarantee with a single element.
                // If it fails, there will be no changes in the queue.
                auto inserted = keyMap.insert({k, std::list<list_iter>{}});
                key_list_pair = inserted.first;
            }
            try {
                const K *ptr = &key_list_pair->first;
                std::pair<K const*, V> p(ptr, v);
                std::list<elem_type> l_pair = std::list<elem_type>();
                l_pair.push_back(p);
                elemList.splice(elemList.end(), l_pair); //no-throw
                std::list<list_iter> l_iter;
                try {
                    l_iter = std::list<list_iter>();
                    l_iter.push_back(--elemList.end());
                } catch (std::exception &e) {
                    // Undo the changes when l_iter.push_back() fails.
                    elemList.pop_back(); // no-throw
                    throw e;
                }
                key_list_pair->second.splice(key_list_pair->second.end(),
                                           l_iter); // no-throw
            } catch (std::exception& e) {
                // If entry for k in map was created and an except was thrown
                // delete the created entry.
                if(key_list_pair->second.empty()) {
                    keyMap.erase(key_list_pair);
                }
                throw e;
            }
        }

        // Strong except. guarantee: makes changes only at the end with no-throw
        // methods.
        void pop() {
            if(elemList.empty()) {
                throw lookup_error();
            }

            auto keyIters = keyMap.find(*elemList.front().first);

            // All methods below are no-throw.
            // Remove iterator from the set and remove the set if empty in result.
            keyIters->second.pop_front();
            if (keyIters->second.empty()) {
                keyMap.erase(keyIters);
            }
            elemList.pop_front();
        }

        // Strong execpt. guarantee: uses only no-throw methods at the end.
        void pop(K const &k) {
            auto keyIters = keyMap.find(k);
            if(keyIters == keyMap.end()) {
              throw lookup_error();
            }
            bool delete_map_entry = keyIters->second.size() == 1;

            // All methods below are no-throw.
            // Remove the element from elemList
            elemList.erase(keyIters->second.front());
            // Remove iterator from the set and remove the set if empty in result.
            keyIters->second.pop_front();
            if (delete_map_entry) {
                keyMap.erase(keyIters);
            }
        }

        // Strong except. guarantee: doesn't modify the container.
        std::pair<K const &, V &> front() {
            if(elemList.empty()) {
                throw lookup_error();
            }
            std::pair<K const*, V>& f = elemList.front();
            return std::pair<K const &, V &>{*f.first, f.second};
        };

        // Strong except. guarantee: doesn't modify the container.
        std::pair<K const &, V &> back() {
            if(elemList.empty()) {
                throw lookup_error();
            }
            std::pair<K const*, V>& f = elemList.back();
            return std::pair<K const &, V &>{*f.first, f.second};
        };

        // Strong except. guarantee: doesn't modify the container.
        std::pair<K const &, V const &> front() const {
            if(elemList.empty()) {
                throw lookup_error();
            }
            std::pair<K const*, V>& f = elemList.front();
            return std::pair<K const &, V const &>{*f.first, f.second};
        };

        // Strong except. guarantee: doesn't modify the container.
        std::pair<K const &, V const &> back() const {
            if(elemList.empty()) {
                throw lookup_error();
            }
            std::pair<K const*, V>& f = elemList.back();
            return std::pair<K const &, V const &>{*f.first, f.second};
        };

        // Strong except. guarantee: doesn't modify the container.
        size_t count(K const &k) const {
            auto keyIters = keyMap.find(k);
            if(keyIters == keyMap.end()) {
                return 0;
            } else {
                return keyIters->second.size();
            }
        }

        // No throw guarantee: both functions give nothrow guarantee.
        void clear() noexcept {
            elemList.clear();
            keyMap.clear();
        }

        // Strong except. guarantee: doesn't modify the container.
        std::pair<K const &, V &> first (K const &key) {
            map_iter m = keyMap.find(key);
            if(m == keyMap.end()) {
                throw lookup_error();
            }
            std::pair<K const*, V>& f = *m->second.front();
            return std::pair<K const &, V &>{*f.first, f.second};
        }

        // Strong except. guarantee: doesn't modify the container.
        std::pair<K const &, V &> last(K const &key) {
            map_iter m = keyMap.find(key);
            if(m == keyMap.end()) {
                throw lookup_error();
            }
            std::pair<K const*, V>& f = *m->second.back();
            return std::pair<K const &, V &>{*f.first, f.second};
        }

        // Strong except. guarantee: doesn't modify the container.
        std::pair<K const &, V const &> first(K const &key) const {
            map_iter m = keyMap.find(key);
            if(m == keyMap.end()) {
                throw lookup_error();
            }
            std::pair<K const*, V const>& f = *m->second.front();
            return std::pair<K const &, V const &>{*f.first, f.second};
        }

        // Strong except. guarantee: doesn't modify the container.
        std::pair<K const &, V const &> last(K const &key) const {
            map_iter m = keyMap.find(key);
            if(m == keyMap.end()) {
                throw lookup_error();
            }
            std::pair<K const*, V const>& f = *m->second.back();
            return std::pair<K const &, V const &>{*f.first, f.second};
        }

        // ? : begin guarantees no throw, and the constructor...
        iterator k_begin() noexcept {
            return iterator(keyMap.begin());
        }

        // ? : begin guarantees no throw, and the constructor...
        iterator k_end() noexcept {
            return iterator(keyMap.end());
        }

        // Strong except. guarantee: It modifies the container only if vector of iterators is correctly computed.
        void move_to_back(K const &k) {
            map_iter m = keyMap.find(k);
            if(m == keyMap.end()) {
                throw lookup_error();
            }

            std::vector<list_iter> temp;
            typename std::list<list_iter>::iterator llit = m->second.begin();
            size_t size = m->second.size();
            temp.resize(size);

            for(size_t current = 0; current < size; current++) {
                temp[current] = *llit;
                ++llit;
            }

            for(size_t current = 0; current < size; current++) {
                elemList.splice(elemList.end(), elemList, temp[current]);
            }

            temp.clear();

        }
};

template<class K, class V>
class keyed_queue<K, V>::keyed_queue_impl::iterator : public map_iter{
public:
    iterator() : map_iter() {};
    iterator(map_iter s) : map_iter(s) {};

    K* operator->() { return (K* const)&(map_iter::operator->()->first); }

    K operator*() { return map_iter::operator*().first; }
};

#endif //JNP5_KEYED_QUEUE_H
