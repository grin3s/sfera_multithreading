#include <stdexcept>
#include <iostream>
#include <string>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <queue>

enum class AllocErrorType {
    InvalidFree,
    NoMemory,
};

class AllocError: std::runtime_error {
private:
    AllocErrorType type;

public:
    AllocError(AllocErrorType _type, std::string message):
            runtime_error(message),
            type(_type)
    {}

    AllocErrorType getType() const { return type; }
};

typedef size_t chunk_id_type;

class Allocator;

class Pointer {
	friend class Allocator;
private:
	chunk_id_type chunk_id = 0;
	Allocator *alloc_obj = nullptr;
public:
	Pointer(){};
	Pointer(chunk_id_type chunk_id, Allocator *alloc_obj): chunk_id(chunk_id), alloc_obj(alloc_obj) {}

    void *get() const;
};

class Allocator {
	friend class Pointer;
private:
	typedef enum {
		CHUNK_OCCUPIED,
		CHUNK_EMPTY
	} ChunkType;

	struct Chunk {
		chunk_id_type id;
		void *base;
		size_t size;
		ChunkType type;
		Chunk(chunk_id_type id, void *base, size_t size, ChunkType type): id(id), base(base), size(size), type(type) {}
	};

	typedef std::list<Chunk>::iterator chunk_list_iterator;

	size_t buf_size;
	void *buf;

	std::list<Chunk> chunk_list;
	std::unordered_map<chunk_id_type, chunk_list_iterator> occupied_chunks_map;
	std::unordered_map<chunk_id_type, chunk_list_iterator> empty_chunks_map;
	std::queue<chunk_id_type> chunk_ids;
	chunk_id_type next_not_assigned_id = 0;

	void *get_pointer_from_chunk(chunk_id_type chunk_id);
	chunk_id_type get_new_chunk_id();

	//we merge right into left and remove right
	void merge_chunks(chunk_list_iterator &left, chunk_list_iterator &right);
public:
    Allocator(void *base, size_t size);
    
    Pointer alloc(size_t N);
    void realloc(Pointer &p, size_t N);
    void free(Pointer &p);

    void defrag();
    std::string dump() { return ""; }
};

