#include "allocator.h"

Allocator::Allocator(void *base, size_t size):
	buf(base),
	buf_size(size)
{
	chunk_list.push_back(Chunk(buf, buf_size, CHUNK_EMPTY));
	empty_chunks_map.insert({get_new_chunk_id(), chunk_list.begin()});
}

void *Pointer::get() const {
	return alloc_obj->get_pointer_from_chunk();
}

chunk_id_type Allocator::get_new_chunk_id() {
	return 0;
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
			auto new_chunk = Chunk(empty_chunk->base, N, CHUNK_OCCUPIED);
			chunk_list.insert(empty_chunk, new_chunk);
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
	return Pointer(1,this);
}

void Allocator::realloc(Pointer &p, size_t N) {}

void Allocator::free(Pointer &p) {}

void Allocator::defrag() {}


