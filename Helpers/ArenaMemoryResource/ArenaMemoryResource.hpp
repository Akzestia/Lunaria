#ifndef ARENA_MEMORY_RESOURCE
#define ARENA_MEMORY_RESOURCE

#include <google/protobuf/arena.h>
#include <map>
#include <memory_resource>
#include <set>
#include <vector>

class ArenaMemoryResource : public std::pmr::memory_resource {
  private:
    google::protobuf::Arena *arena_;

  public:
    explicit ArenaMemoryResource(google::protobuf::Arena *arena)
        : arena_(arena) {}

  protected:
    void *do_allocate(size_t bytes, size_t alignment) override {
        return arena_->AllocateAligned(bytes, alignment);
    }

    void do_deallocate(void *p, size_t bytes, size_t alignment) override {}

    bool do_is_equal(
        const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }
};
template <typename T> using ArenaVector = std::pmr::vector<T>;
template <typename T> using ArenaSet = std::pmr::set<T>;
template <typename K, typename V> using ArenaMap = std::pmr::map<K, V>;

#endif //ARENA_MEMORY_RESOURCE
