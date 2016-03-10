#include "allocator.h"

Allocator::Allocator(void *base, size_t size):
	buf(base),
	buf_size(size)
{
	auto new_id = get_new_chunk_id();
	chunk_list.push_back(Chunk(new_id, buf, buf_size, CHUNK_EMPTY));
	empty_chunks_map.insert({new_id, chunk_list.begin()});
}

void *Pointer::get() const {
	return alloc_obj->get_pointer_from_chunk();
}

chunk_id_type Allocator::get_new_chunk_id() {
	if (chunk_ids.empty()) {
		return next_not_assigned_id++;
	}
	else {
		auto returned_val = chunk_ids.front();
		chunk_ids.pop();
		return returned_val;
	}
}

void *Allocator::get_pointer_from_chunk() {
	return NULL;
}

Pointer Allocator::alloc(size_t N) {
	for (auto pair_iter = empty_chunks_map.begin(); pair_iter != empty_chunks_map.end(); pair_iter++) {
		auto empty_chunk_id = pair_iter->first;
		auto empty_chunk = pair_iter->second;
		if (N <= empty_chunk->size) {
			// we can process the memory request using this chunk
			chunk_id_type new_id = get_new_chunk_id();
			auto new_chunk = Chunk(new_id, empty_chunk->base, N, CHUNK_OCCUPIED);
			auto new_chunk_list_iterator = chunk_list.insert(empty_chunk, new_chunk);
			occupied_chunks_map.insert({new_id, new_chunk_list_iterator});
			auto empty_chunk_remaining_size = empty_chunk->size - N;
			if (empty_chunk_remaining_size > 0) {
				empty_chunk->base += N;
				empty_chunk->size -= N;
			}
			else {
				empty_chunks_map.erase(pair_iter);
				chunk_list.erase(empty_chunk);
				//remembering this id, so that we can use it later
				chunk_ids.push(empty_chunk_id);
			}
			return Pointer(new_id, this);
		}
	}
	throw AllocError(AllocErrorType::NoMemory, "no memory left");
}

void Allocator::realloc(Pointer &p, size_t N) {}

void Allocator::merge_chunks(chunk_list_iterator &left, chunk_list_iterator &right) {
	if (right->type == CHUNK_EMPTY) {
		//we merge
		left->size += right->size;
		chunk_ids.push(right->id);
		//erase right
		chunk_list.erase(right);
		empty_chunks_map.erase(right->id);
	}
}

void Allocator::free(Pointer &p) {
	auto chunk_id = p.chunk_id;
	auto chunk_list_iterator = occupied_chunks_map.find(chunk_id)->second;
	chunk_list_iterator->type = CHUNK_EMPTY;
	occupied_chunks_map.erase(chunk_id);
	empty_chunks_map.insert({chunk_id, chunk_list_iterator});
	// trying to merge our empty chunk with another one on the right side
	auto chunk_right = chunk_list_iterator;
	chunk_right++;
	if (chunk_right != chunk_list.end()) {
		if (chunk_right->type == CHUNK_EMPTY) {
			//we merge
			merge_chunks(chunk_list_iterator, chunk_right);
		}
	}
	// trying to unite our empty chunk with another one on the left side
	if (chunk_list_iterator != chunk_list.begin()) {
		auto chunk_left = chunk_list_iterator;
		chunk_left--;
		if (chunk_left->type == CHUNK_EMPTY) {
			merge_chunks(chunk_left, chunk_list_iterator);
		}
	}
}

void Allocator::defrag() {}


