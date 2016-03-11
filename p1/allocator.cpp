#include "allocator.h"
#include <cstring>

Allocator::Allocator(void *base, size_t size):
	buf(base),
	buf_size(size)
{
	auto new_id = get_new_chunk_id();
	chunk_list.push_back(Chunk(new_id, buf, buf_size, CHUNK_EMPTY));
	empty_chunks_map.insert({new_id, chunk_list.begin()});
}

void *Pointer::get() const {
	if (alloc_obj == nullptr) {
		return nullptr;
	}
	return alloc_obj->get_pointer_from_chunk(chunk_id);
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

void *Allocator::get_pointer_from_chunk(chunk_id_type chunk_id) {
	return occupied_chunks_map.find(chunk_id)->second->base;
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
				empty_chunk->base = (char *)empty_chunk->base + N;
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

void Allocator::realloc(Pointer &p, size_t N) {
	Pointer new_p;
	auto pair_iter = occupied_chunks_map.find(p.chunk_id);
	if (pair_iter != occupied_chunks_map.end()) {
		// it is not empty
		auto p_list_iter = pair_iter->second;
		if (p_list_iter->size < N) {
			//we grow
			auto p_right = p_list_iter;
			p_right++;
			bool grow_in_place = false;
			if (p_right != chunk_list.end()) {
				if ((p_right->type == CHUNK_EMPTY) && (p_right->size + p_list_iter->size >= N)) {
					//grow in place
					grow_in_place = true;
					auto right_new_size = p_right->size + p_list_iter->size - N;
					p_list_iter->size = N;
					if (right_new_size > 0) {
						p_right->size = right_new_size;
						p_right->base = (char *)p_list_iter->base + N;
					}
					else {
						//we must remove this empty chunk
						chunk_ids.push(p_right->id);
						empty_chunks_map.erase(p_right->id);
						chunk_list.erase(p_right);
					}
				}
			}
			if (!grow_in_place) {
				new_p = alloc(N);
				auto pair_iter_new = occupied_chunks_map.find(new_p.chunk_id);
				auto p_new_list_iter = pair_iter_new->second;
				std::memcpy(p_new_list_iter->base, p_list_iter->base, p_list_iter->size);
				this->free(p);
				p = new_p;
			}
		}
		else {
			//we shrink or leave it the same
			if (p_list_iter->size > N) {
				//shrink
				auto new_empty_chunk_size = p_list_iter->size - N;
				auto new_empty_chunk_id = get_new_chunk_id();
				auto new_empty_chunk = Chunk(new_empty_chunk_id, (char *)p_list_iter->base + N, new_empty_chunk_size, CHUNK_EMPTY);
				p_list_iter->size = N;
				p_list_iter++;
				auto new_empty_chunk_iter = chunk_list.insert(p_list_iter, new_empty_chunk);
				empty_chunks_map.insert({new_empty_chunk_id, new_empty_chunk_iter});
			}
			else {
				//the same size
			}
		}
	}
	else {
		//simply alloc
		new_p = alloc(N);
		p = new_p;
	}
}

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
	auto pair_iter = occupied_chunks_map.find(chunk_id);
	if (pair_iter == occupied_chunks_map.end()) {
		throw AllocError(AllocErrorType::InvalidFree, "trying to free a wrong pointer");
	}
	auto chunk_list_iterator = pair_iter->second;
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
	p.chunk_id = 0;
	p.alloc_obj = nullptr;
}

void Allocator::defrag() {
	bool looking_for_occupied = false;
	void *occupied_end = nullptr;
	size_t empty_space = 0;
	auto chunk_iter = chunk_list.begin();
	while (chunk_iter != chunk_list.end()) {
		if (chunk_iter->type == CHUNK_OCCUPIED) {
			if (occupied_end != nullptr) {
				std::memcpy(occupied_end, chunk_iter->base, chunk_iter->size);
				chunk_iter->base = occupied_end;
				occupied_end = (char *)occupied_end + chunk_iter->size;
			}
			chunk_iter++;
		}
		else if (chunk_iter->type == CHUNK_EMPTY) {
			empty_space += chunk_iter->size;
			if (occupied_end == nullptr) {
				occupied_end = chunk_iter->base;
			}
			empty_chunks_map.erase(chunk_iter->id);
			chunk_ids.push(chunk_iter->id);
			chunk_iter = chunk_list.erase(chunk_iter);
		}
	}
	if (empty_space > 0) {
		auto new_id = get_new_chunk_id();
		auto new_chunk = Chunk(new_id, (char *)buf + buf_size - empty_space, empty_space, CHUNK_EMPTY);
		chunk_list.push_back(new_chunk);
		empty_chunks_map.insert({new_id, --chunk_list.end()});
	}
}


