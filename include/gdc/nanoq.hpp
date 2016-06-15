//
// Copyright (c) 2016 Dado Colussi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#ifndef __gdc__nanoq__
#define __gdc__nanoq__


#include <stdexcept>
#include <memory>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdlib>


#ifndef LEVEL1_DCACHE_LINESIZE
#define LEVEL1_DCACHE_LINESIZE 64
#endif


namespace gdc
{
	
	
	template<typename T>
	class nanoq
	{
	public:
		
		
		typedef T value_type;
		typedef std::size_t size_type;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T* pointer;
		typedef const T* const_pointer;
		
		
		nanoq() : _data(nullptr), _capacity(0), _rpos(0), _wpos(0)
		{
		}
		
		
		nanoq(T* data, size_type capacity) : _data(data), _capacity(capacity), _rpos(0), _wpos(0)
		{
		}
		
		
		nanoq(const nanoq&) = delete;
		nanoq(nanoq&&) = delete;
		nanoq& operator=(nanoq&) = delete;
		nanoq& operator=(nanoq&&) = delete;
		~nanoq() = default;
		
		
		void init(T* data, size_type capacity)
		{
			_data = data;
			_capacity = capacity;
		}
		
		
		size_type capacity() const
		{
			return _capacity;
		}
		
		
		size_type available() const noexcept
		{
			auto rp = rpos();
			auto wp = wpos();
			size_type c = capacity();
			size_type n;
			
			if (wp >= rp)
			{
				// _____xxxxx_____
				//      ^    ^
				//     rp    wp
				//
				// Scenario 2 (empty):
				// _______________
				//      ^
				//    wp==rp
				n = wp - rp;
			}
			else
			{
				// xxxxx_____xxxxx
				//      ^    ^
				//     wp    rp
				n = c + wp - rp;
			}
			
			assert(n < c);
			
			return n;
		}
		
		
		size_type space() const noexcept
		{
			auto rp = rpos();
			auto wp = wpos();
			size_type c = capacity();
			size_type n;
			
			if (wp >= rp)
			{
				// _____xxxxx_____
				//      ^    ^
				//     rp    wp
				//
				// Scenario 2 (empty):
				// _______________
				//      ^
				//    wp==rp
				n = c + rp - wp - 1;
			}
			else
			{
				// xxxxx_____xxxxx
				//      ^    ^
				//     wp    rp
				n = rp - wp - 1;
			}
			
			assert(n < c);
			
			return n;
		}
		
		
		bool empty() const
		{
			return rpos() == wpos();
		}
		
		
		bool full() const
		{
			return space() == 0;
		}
		
		
		void push_back(const_reference item)
		{
			if (full())
			{
				throw std::overflow_error("nanoq is full");
			}
			
			auto pos = wpos();
			new (&_data[pos]) value_type(item);
			wpos(pos + 1);
		}
		
		
		void push_back(T&& item)
		{
			if (full())
			{
				throw std::overflow_error("nanoq is full");
			}
			
			auto pos = wpos();
			new (&_data[pos]) value_type(std::move(item));
			wpos(pos + 1);
		}
		
		
		template<typename ... Args>
		void emplace_back(Args&& ... args)
		{
			if (full())
			{
				throw std::overflow_error("nanoq is full");
			}
			
			auto pos = wpos();
			void* p = &_data[pos];
			new (p) value_type(args ...);
			wpos(pos + 1);
		}
		
		
		const_reference front() const
		{
			if (empty())
			{
				throw std::underflow_error("nanoq is empty");
			}
			
			auto pos = rpos();
			return _data[pos];
		}
		
		
		void pop_front()
		{
			if (empty())
			{
				throw std::underflow_error("nanoq is empty");
			}
			
			auto pos = rpos();
			_data[pos].~T();
			rpos(pos + 1);
		}
		
		
	private:
		
		
		size_type rpos() const
		{
			return _rpos.load(std::memory_order_relaxed);
		}
		
		
		void rpos(size_type pos)
		{
			if (pos >= capacity())
			{
				pos = 0;
			}
			
			_rpos.store(pos, std::memory_order_relaxed);
		}
		
		
		size_type wpos() const
		{
			return _wpos.load(std::memory_order_relaxed);
		}
		
		
		void wpos(size_type pos)
		{
			if (pos >= capacity())
			{
				pos = 0;
			}
			
			_wpos.store(pos, std::memory_order_release);
		}
		
		
		T* _data;
		size_type _capacity;
		
		union
		{
			std::atomic<size_type> _rpos;
			char pad_rpos[LEVEL1_DCACHE_LINESIZE];
		};
		
		union
		{
			std::atomic<size_type> _wpos;
			char pad_wpos[LEVEL1_DCACHE_LINESIZE];
		};
		
	};
	
	
	template<typename T, typename nanoq<T>::size_type _capacity>
	class nanoq_stack_factory
	{
	public:
		
		
		typedef nanoq<T> queue_type;
		typedef typename queue_type::size_type size_type;
		
		
		nanoq_stack_factory() : _queue(_data, _capacity)
		{
		}
		
		
		nanoq_stack_factory(const nanoq_stack_factory&) = delete;
		nanoq_stack_factory(nanoq_stack_factory&&) = delete;
		nanoq_stack_factory& operator=(const nanoq_stack_factory&) = delete;
		nanoq_stack_factory&& operator=(const nanoq_stack_factory&&) = delete;
		
		
		queue_type& get()
		{
			return _queue;
		}
		
		
	private:
		
		
		queue_type _queue;
		T _data[_capacity];
		
		
	};
	
	
	template<typename T>
	class nanoq_heap_factory
	{
	public:
		
		
		typedef nanoq<T> queue_type;
		typedef typename queue_type::size_type size_type;
		
		
		nanoq_heap_factory() = delete;
		
		
		nanoq_heap_factory(size_type capacity) :
			_data(malloc(capacity * sizeof (T)), free),
			_queue(_data.get(), capacity)
		{
		}
		
		
		nanoq_heap_factory(const nanoq_heap_factory&) = delete;
		
		
		nanoq_heap_factory(nanoq_heap_factory&& f) :
			_data(std::move(f._data)),
			_queue(_data.get(), f._queue.capacity())
		{
		}
		
		
		nanoq_heap_factory& operator=(const nanoq_heap_factory&) = delete;
		nanoq_heap_factory& operator=(const nanoq_heap_factory&&) = delete;
		
		queue_type& get()
		{
			return _queue;
		}
		
		
	private:
		
		
		std::unique_ptr<T, void(*)(T*)> _data;
		queue_type _queue;
		
		
		static T* malloc(size_type nbytes)
		{
			return reinterpret_cast<T*>(std::malloc(nbytes));
		}
		
		
		static void free(T* p)
		{
			std::free(p);
		}
		
		
	};
	
	
}


#endif
