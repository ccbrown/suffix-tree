#include <cassert>
#include <unordered_map>
#include <vector>

template <class CharT = char>
class SuffixTree {
public:
    SuffixTree() {
        clear();
    }

    void begin_new_string() {
        assert(!_remainder);
        for (size_t i = _first_string_node; i < _nodes.size(); ++i) {
            _nodes[i].end = std::min(_text.size(), _nodes[i].end);
        }
        _active_node = 0;
        _active_edge = 0;
        _active_length = 0;
        _remainder = 0;
        _first_string_node = _nodes.size();
    }

    template <class InputIt>
    void add_string(InputIt first, InputIt last) {
        begin_new_string();
        append(first, last);
    }

    template <class InputIt>
    void append(InputIt first, InputIt last) {
        for (size_t i = 0; first != last; ++i, ++first) {
            append(*first);
        }
    }

    void append(CharT character) {
        if (!_active_length) {
            _active_edge = _text.size();
        }

        _text.emplace_back(character);
        ++_remainder;
        size_t suffix_link = 0;

        while (_remainder > 0) {
            auto it = _nodes[_active_node].children.find(_text[_active_edge]);
            if (it == _nodes[_active_node].children.end()) {
                auto leaf = _nodes.size();
                _nodes.emplace_back(_text.size() - 1);
                _nodes[_active_node].children[_text[_active_edge]] = leaf;
                if (suffix_link) {
                    _nodes[suffix_link].suffix_link = _active_node;
                }
                suffix_link = _active_node;
            } else {
                auto child = it->second;
                auto edge_length = std::min(_text.size(), _nodes[child].end) - _nodes[child].begin;
                if (_active_length >= edge_length) {
                    _active_edge += edge_length;
                    _active_length -= edge_length;
                    _active_node = child;
                    continue;
                }

                if (_text[_nodes[child].begin + _active_length] == character) {
                    ++_active_length;
                    if (suffix_link) {
                        _nodes[suffix_link].suffix_link = _active_node;
                    }
                    suffix_link = _active_node;
                    return;
                }

                auto head = _nodes.size();
                _nodes.emplace_back(_nodes[child].begin, _nodes[child].begin + _active_length);
                _nodes[_active_node].children[_text[_active_edge]] = head;
                auto leaf = _nodes.size();
                _nodes.emplace_back(_text.size() - 1);
                _nodes[head].children[character] = leaf;
                _nodes[child].begin += _active_length;
                _nodes[head].children[_text[_nodes[child].begin]] = child;
                if (suffix_link) {
                    _nodes[suffix_link].suffix_link = head;
                }
                suffix_link = head;
            }
            --_remainder;
            if (!_active_node && _active_length > 0) {
                --_active_length;
                _active_edge = _text.size() - _remainder;
            } else {
                _active_node = _nodes[_active_node].suffix_link;
            }
        }
    }

    template <class InputIt>
    bool is_substring(InputIt first, InputIt last) const {
        return _find(first, last).node != -1;
    }

    template <class InputIt>
    bool is_suffix(InputIt first, InputIt last) const {
        if (first == last) { return true; }
        auto f = _find(first, last);
        return f.node != -1 && f.edge_length == 0 && _nodes[f.node].children.empty();
    }

    template <class InputIt>
    size_t substring_count(InputIt first, InputIt last) const {
        auto f = _find(first, last);
        if (f.node == -1) { return 0; }
        size_t count = 0;
        std::vector<size_t> to_visit;
        to_visit.emplace_back(f.edge_length ? _nodes[f.node].children.at(f.edge) : f.node);
        while (!to_visit.empty()) {
            auto next = to_visit.back();
            to_visit.pop_back();
            if (_nodes[next].children.empty()) {
                ++count;
                continue;
            }
            for (auto c : _nodes[next].children) {
                to_visit.emplace_back(c.second);
            }
        }
        return count;
    }

    void clear() {
        _text.clear();
        _nodes.clear();
        _nodes.emplace_back(0, 0);
        begin_new_string();
    }

private:
    struct Node {
        Node(size_t begin, size_t end = -1)
            : begin(begin), end(end) {}

        size_t begin, end;
        std::unordered_map<CharT, size_t> children;
        size_t suffix_link = 0;
    };

    struct FindResult {
        FindResult(size_t node, CharT edge, size_t edge_length)
            : node(node), edge(edge), edge_length(edge_length) {}
        size_t node;
        CharT edge;
        size_t edge_length;
    };

    template <class InputIt>
    FindResult _find(InputIt first, InputIt last) const {
        auto node = 0;
        while (first != last) {
            auto edge = *first;
            auto it = _nodes[node].children.find(edge);
            if (it == _nodes[node].children.end()) {
                return FindResult(-1, 0, 0);
            }
            auto& child = _nodes[it->second];
            auto childEnd = std::min(_text.size(), child.end);
            for (size_t i = 0; child.begin + i < childEnd; ++i) {
                if (*first != _text[child.begin + i]) {
                    return FindResult(-1, 0, 0);
                }
                if (++first == last) {
                    if (child.begin + i + 1 == childEnd) {
                        return FindResult(it->second, 0, 0);
                    }
                    return FindResult(node, edge, i + 1);
                }
            }
            node = it->second;
        }
        return FindResult(node, 0, 0);
    }

    std::vector<CharT> _text;
    size_t _active_node = 0;
    size_t _active_edge = 0;
    size_t _active_length = 0;
    size_t _remainder = 0;
    size_t _first_string_node = 0;
    std::vector<Node> _nodes;
};
