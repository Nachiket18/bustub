//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_k_replacer.h"
#include "common/exception.h"

namespace bustub {

/**
 *
 * TODO(P1): Add implementation
 *
 * @brief a new LRUKReplacer.
 * @param num_frames the maximum number of frames the LRUReplacer will be required to store
 */
LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}

/**
 * TODO(P1): Add implementation
 *
 * @brief Find the frame with largest backward k-distance and evict that frame. Only frames
 * that are marked as 'evictable' are candidates for eviction.
 *
 * A frame with less than k historical references is given +inf as its backward k-distance.
 * If multiple frames have inf backward k-distance, then evict frame whose oldest timestamp
 * is furthest in the past.
 *
 * Successful eviction of a frame should decrement the size of replacer and remove the frame's
 * access history.
 *
 * @return the frame ID if a frame is successfully evicted, or `std::nullopt` if no frames can be evicted.
 */
auto LRUKReplacer::Evict() -> std::optional<frame_id_t> { 
    frame_id_t lowest_frame_id = 0;
    size_t oldest_timestamp = 10000;
    bool inf_flag = false;

    for (auto it = node_store_.begin(); it != node_store_.end(); ++it) {
        LRUKNode node = it->second;
        if (node.history_.size() < k_) {
            inf_flag = true;
        }
        
    }

    std::cout << std::endl;
    for (auto it = node_store_.begin(); it != node_store_.end(); ++it) {
        LRUKNode node = it->second;

        size_t node_oldest; 
        
        // [1,2,3,4]

        size_t leng = node.history_.size(); 
        if (leng > k_) {
            std::list<size_t>::iterator it_advance = node.history_.begin();
            std::advance(it_advance, leng-k_);
            node_oldest = *it_advance;
        }
        else {
            node_oldest = node.history_.front();
        }
        
        
        
        
        // std::cout << "Node: " << node.fid_ << std::endl;
        // std::cout << "Node Backward Distance: " << node.history_.front() << std::endl;
        // std::cout << "Node Backward size : " << node.history_.size() << std::endl;

        if (node.is_evictable_ == 0) { 
            continue; 
        }

        if (node.history_.size() < k_) {
            
            if (node_oldest < oldest_timestamp) {
                lowest_frame_id = node.fid_;
                oldest_timestamp = node_oldest;
            }
            
        }
        else {
            if (inf_flag == true) { 
                continue; 
            }

            if (node_oldest < oldest_timestamp) {
                lowest_frame_id = node.fid_;
                oldest_timestamp = node_oldest;
            }
        }
    }

    // history < k => inf
    // 

    // oldest-timestamp: 7:00pm
    // oldest-inf-node: D
    // A - inf
    // B - 2
    // F - 2
    // C - 6
    // D - inf
    

    if (lowest_frame_id == std::numeric_limits<size_t>::infinity()) { return std::nullopt; }
    else {
        curr_size_ -= 1; 
        node_store_.erase(lowest_frame_id);
        return lowest_frame_id;
    }
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Record the event that the given frame id is accessed at current timestamp.
 * Create a new entry for access history if frame id has not been seen before.
 *
 * If frame id is invalid (ie. larger than replacer_size_), throw an exception. You can
 * also use BUSTUB_ASSERT to abort the process if frame id is invalid.
 *
 * @param frame_id id of frame that received a new access.
 * @param access_type type of access that was received. This parameter is only needed for
 * leaderboard tests.
 */
void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {

    size_t _f = (size_t)frame_id ? frame_id > 0 : 0;
    if (_f > replacer_size_) {
        throw Exception("BUSTUB_ASSERT");
    }

    

    if ((bool)node_store_.count(frame_id)) {
        LRUKNode node = node_store_[frame_id];
        node.history_.insert(node.history_.end(), current_timestamp_);
        node_store_[frame_id] = node;
        
    }
    else {
        LRUKNode node = LRUKNode(frame_id, k_);
        
        node.history_.insert(node.history_.end(), current_timestamp_);
        node_store_.insert({frame_id, node});
    }
    this -> current_timestamp_ += 1;
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Toggle whether a frame is evictable or non-evictable. This function also
 * controls replacer's size. Note that size is equal to number of evictable entries.
 *
 * If a frame was previously evictable and is to be set to non-evictable, then size should
 * decrement. If a frame was previously non-evictable and is to be set to evictable,
 * then size should increment.
 *
 * If frame id is invalid, throw an exception or abort the process.
 *
 * For other scenarios, this function should terminate without modifying anything.
 *
 * @param frame_id id of frame whose 'evictable' status will be modified
 * @param set_evictable whether the given frame is evictable or not
 */
void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {

    size_t _f = (size_t)frame_id ? frame_id > 0 : 0;
    if (_f > replacer_size_) {
        throw Exception("BUSTUB_ASSERT");
    }

    if ((bool)node_store_.count(frame_id)) {
        LRUKNode node = node_store_[frame_id];
        bool previous_evictability = node.is_evictable_;

        if (previous_evictability == true && set_evictable == false) { // decrement size
            curr_size_ -= 1;
        }
        else if (previous_evictability == false && set_evictable == true) { // increment size
            curr_size_ += 1;
        }

        node.is_evictable_ = set_evictable;
        node_store_[frame_id] = node;
    }
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Remove an evictable frame from replacer, along with its access history.
 * This function should also decrement replacer's size if removal is successful.
 *
 * Note that this is different from evicting a frame, which always remove the frame
 * with largest backward k-distance. This function removes specified frame id,
 * no matter what its backward k-distance is.
 *
 * If Remove is called on a non-evictable frame, throw an exception or abort the
 * process.
 *
 * If specified frame is not found, directly return from this function.
 *
 * @param frame_id id of frame to be removed
 */
void LRUKReplacer::Remove(frame_id_t frame_id) {}

/**
 * TODO(P1): Add implementation
 *
 * @brief Return replacer's size, which tracks the number of evictable frames.
 *
 * @return size_t
 */
auto LRUKReplacer::Size() -> size_t { return curr_size_; }

}  // namespace bustub
