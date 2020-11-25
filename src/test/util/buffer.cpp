#include <catch2/catch.hpp>
using Catch::Matchers::Equals;

extern "C" {
	#include <calp/util/buffer.h>
}

SCENARIO("buffer can be constructed and destroyed", "[buffer][low-level]"){
	GIVEN("nothing"){
		Buffer buff;
		buff = buffer_new(0);
		THEN("the buffer is empty"){
			REQUIRE(buff->cap == 0);
			REQUIRE(buff->size == 0);
			REQUIRE_THAT(buff->data, Equals(""));
			buffer_destroy(buff);
		}
	}
	GIVEN("initial capacity"){
		Buffer buff;
		buff = buffer_new(256);
		THEN("buffer's capacity is initialized appropriately, but the buffer is empty"){
			REQUIRE(buff->cap == 256);
			REQUIRE(buff->size == 0);
			REQUIRE_THAT(buff->data, Equals(""));
		}
		buffer_destroy(buff);
	}
	GIVEN("string to copy from"){
		char original[] = "hello world!";
		const size_t l = strlen(original);
		string_mut str = strdup(original);
		Buffer buff;
		buff = buffer_new_from(str, -1);
		free(str);
		THEN("the string is copied from the original"){
			REQUIRE(buff->cap >= l);
			REQUIRE(buff->size == l);
			REQUIRE_THAT(buff->data, Equals(original));
			AND_THEN("the buffer can be simply destroyed"){
				buffer_destroy(buff);
			}
			AND_THEN("destring the buffer keeps the data alive"){
				string_mut data = buffer_destr(buff);
				REQUIRE_THAT(data, Equals(original));
				free(data);
			}
		}
	}
	GIVEN("substring to copy from"){
		char original[] = "hello world!";
		const size_t l = strlen(original);
		const size_t subl = 5;
		string_mut str = strdup(original);
		string_mut expected = strdup(original);
		expected[subl] = '\0';
		Buffer buff;
		buff = buffer_new_from(str, subl);
		free(str);
		THEN("the string is copied from the original"){
			REQUIRE(buff->cap >= subl);
			REQUIRE(buff->size == subl);
			REQUIRE_THAT(buff->data, Equals(expected));
			AND_THEN("the buffer can be simply destroyed"){
				buffer_destroy(buff);
			}
			AND_THEN("destring the buffer keeps the data alive"){
				string_mut data = buffer_destr(buff);
				REQUIRE_THAT(data, Equals(expected));
				free(data);
			}
		}
		free(expected);
	}
}

SCENARIO("buffer can be resized", "[buffer][low-level]"){
	GIVEN("a buffer of equal size and capacity"){
		const size_t l = 15;
		Buffer buff = buffer_new_from("hello world! :)", -1);
		CHECK(buff->cap >= l);
		CHECK(buff->size == l);
		THEN("increasing the capacity doesn't touch the size"){
			buffer_resize(buff, 20);
			REQUIRE(buff->cap >= 20);
			REQUIRE(buff->size == l);
			REQUIRE_THAT(buff->data, Equals("hello world! :)"));
		}
		THEN("decreasing the capacity cuts the data"){
			buffer_resize(buff, 10);
			REQUIRE(buff->cap >= 10);
			REQUIRE(buff->size == 10);
			REQUIRE_THAT(buff->data, Equals("hello worl"));
		}
		THEN("resizing to 0 deletes all"){
			buffer_resize(buff, 0);
			REQUIRE(buff->cap == 0);
			REQUIRE(buff->size == 0);
			REQUIRE_THAT(buff->data, Equals(""));
		}
		buffer_destroy(buff);
	}
}

SCENARIO("buffer can be modified in magical ways", "[buffer][high-level]"){
	GIVEN("an empty buffer"){
		Buffer buff = buffer_new(0);
		THEN("deleting anything is no-op"){
			REQUIRE(buffer_delete(buff, 0, 0) == Ok);
			REQUIRE_THAT(buff->data, Equals(""));
			REQUIRE(buffer_delete(buff, 0, 10) == Ok);
			REQUIRE_THAT(buff->data, Equals(""));
			REQUIRE(buffer_delete(buff, 5, 10) == Ok);
			REQUIRE_THAT(buff->data, Equals(""));
		}
		THEN("appending data works like a charm"){
			REQUIRE(buffer_append_str(buff, "hello ") == Ok);
			REQUIRE(buff->cap >= 6);
			REQUIRE(buff->cap == 6);
			REQUIRE_THAT(buff->data, Equals("hello "));
			THEN("appending data again works too"){
				REQUIRE(buffer_append_str(buff, "venus!") == Ok);
				REQUIRE(buff->cap >= 12);
				REQUIRE(buff->size == 12);
				REQUIRE_THAT(buff->data, Equals("hello venus!"));
				THEN("delete a portion and it is gone"){
					REQUIRE(buffer_delete(buff, 6, 9) == Ok);
					REQUIRE(buff->cap >= 9);
					REQUIRE(buff->size == 9);
					REQUIRE_THAT(buff->data, Equals("hello us!"));
					THEN("replacing a portion removes the old puts the new"){
						REQUIRE(buffer_splice_str(buff, 2, 5, "he") == Ok);
						REQUIRE(buff->cap >= 8);
						REQUIRE(buff->size == 8);
						REQUIRE_THAT(buff->data, Equals("hehe us!"));
					}
				}
			}
		}
		buffer_destroy(buff);
	}
}
