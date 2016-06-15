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


// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN


#include <gdc/nanoq.hpp>

#include "catch.hpp"


SCENARIO("nanoq", "[nanoq]")
{
	
	
	GIVEN("an empty stack-allocated nanoq")
	{
		gdc::nanoq_stack_factory<int, 8> factory;
		gdc::nanoq<int>& that = factory.get();
		
		THEN("empty() returns true")
		{
			CHECK(that.empty());
		}
		
		THEN("full() returns false")
		{
			CHECK_FALSE(that.full());
		}
		
		THEN("space() returns capacity() - 1")
		{
			CHECK(that.space() == that.capacity() - 1);
		}
		
		THEN("available() returns 0")
		{
			CHECK(that.available() == 0);
		}
		
		THEN("popping an item throws std::underflow_error")
		{
			CHECK_THROWS_AS(that.pop_front(), std::underflow_error);
		}
	
		WHEN("pushing an item")
		{
			that.push_back(42);
			
			THEN("empty() returns false")
			{
				CHECK_FALSE(that.empty());
			}
			
			THEN("full() returns false")
			{
				CHECK_FALSE(that.full());
			}
			
			THEN("space() returns capacity() - 2")
			{
				CHECK(that.space() == that.capacity() - 2);
			}
			
			THEN("available() returns 1")
			{
				CHECK(that.available() == 1);
			}
			
			THEN("front() returns the item")
			{
				CHECK(that.front() == 42);
			}
		}
		
		WHEN("pushing and popping an item")
		{
			that.push_back(42);
			that.pop_front();
			
			THEN("empty() returns true")
			{
				CHECK(that.empty());
			}
			
			THEN("full() returns false")
			{
				CHECK_FALSE(that.full());
			}
			
			THEN("space() returns capacity() - 1")
			{
				CHECK(that.space() == that.capacity() - 1);
			}
			
			THEN("available() returns 0")
			{
				CHECK(that.available() == 0);
			}
		}
		
		WHEN("pushing and popping many items")
		{
			int n = 11 * static_cast<int>(that.capacity()) / 7;
			
			for (int i = 0; i < n; ++i)
			{
				that.push_back(i);
				that.pop_front();
			}
			
			that.push_back(static_cast<int>(n));
			CHECK(that.front() == n);
		}
	}
	
	
	GIVEN("a full heap-allocated nanoq")
	{
		gdc::nanoq_heap_factory<int> factory(8);
		gdc::nanoq<int>& that = factory.get();
		
		for (gdc::nanoq<int>::size_type i = 0; i < that.capacity() - 1; ++i)
		{
			that.push_back(static_cast<int>(i));
		}
		
		THEN("push_back() throws std::overflow_error")
		{
			CHECK_THROWS_AS(that.push_back(42), std::overflow_error);
		}
		
		THEN("empty() returns false")
		{
			CHECK_FALSE(that.empty());
		}
		
		THEN("full() returns true")
		{
			CHECK(that.full());
		}
		
		THEN("space() returns 0")
		{
			CHECK(that.space() == 0);
		}
		
		THEN("available() returns capacity() - 1")
		{
			CHECK(that.available() == that.capacity() - 1);
		}
	}
	
	
}
