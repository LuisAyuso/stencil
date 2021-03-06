#include <gtest/gtest.h>


#include <chrono>
#include <thread>

#include "timer.h" 


TEST(Timer, lambda_1){


	int val =1;
	auto f = [&] (int i){
		EXPECT_EQ(i, val);
	};

	time_call(f, val);
	val++;
	time_call(f, val);
}

TEST(Timer, lambda_2){


	int val =1;

	time_call([&] (int i){
		EXPECT_EQ(i, val);
		EXPECT_EQ(i, 1);
	}, val);

	val++;
	time_call([&] (int i){
		EXPECT_EQ(i, val);
		EXPECT_EQ(i, 2);
	}, val);

}

TEST(Timer, lambda_3){


	int val =1;
	int val2 = 2;


	auto f = [&] (int i, int j){
		EXPECT_EQ(i, val);
		EXPECT_EQ(j, val2);
	};

	time_call(f, val, val2);
	val++;
	time_call(f, val, val2);

}

TEST(Timer, precission){

	auto f = [&] (){
	    std::chrono::seconds two(2);
		std::this_thread::sleep_for(two);
	};

	auto duration = time_call(f);
	EXPECT_TRUE(duration >= 2000);

}
