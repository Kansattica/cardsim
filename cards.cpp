// (c) 2021 Grace Lovelace
// made available under the AGPL
// compile with g++ -fopenmp -std=c++17 cards.cpp -O3 -o cards   

#include <algorithm>
#include <iostream>
#include <random>
#include <array>

#ifndef NO_OMP
#include <omp.h>
#endif

using card = uint_fast8_t;
using cardval = uint_fast8_t;

constexpr bool aces_high = true;
constexpr size_t max_hand_size = 7;

using deck = std::array<card, 52>;
using hand = std::array<card, max_hand_size>;

constexpr std::array<const char*, 4> suitnames = {"\u2663", "\u2660", "\u2665", "\u2666"};

constexpr std::array<const char*, 14> cardnames = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

constexpr cardval get_value(card c)
{
	const auto val = c % 13;
	if constexpr (aces_high)
	{
		if (val == 0) { return 13; }
	}
	return val;
}

void print_card(card c)
{
	std::cout << cardnames[get_value(c)] << suitnames[c % 4];	
}

constexpr bool card_compare(const card& lhs, const card& rhs)
{
	const auto leftval = get_value(lhs);
	const auto rightval = get_value(rhs);

	if (leftval != rightval)
		return leftval < rightval;
	return (leftval % 4) < (rightval % 4);
}

template <typename it>
constexpr void sort_deck(it begin, it end)
{
	std::sort(begin, end, card_compare);
}

auto random_engine = [](){
	std::random_device r;
	return std::mt19937{ r() };
}();

template <typename it>
void shuffle_deck(it begin, it end)
{
	std::shuffle(begin, end, random_engine);
}

template <typename it>
void random_draw(it begin, it end, size_t hand_size, hand& hand)
{
	std::sample(begin, end, hand.begin(), hand_size, random_engine);
}

template <typename it>
void print_deck(it begin, it end)
{
	std::for_each(begin, end, [](const auto card)
	{
		print_card(card);
		std::cout << ' ';
	});
}

constexpr cardval target_number = 8;

template <typename it>
bool is_success(it begin, it end)
{
	return std::any_of(begin, end, [](const auto card) { return get_value(card) >= target_number; });
}

int main()
{
	deck deck;
	std::iota(deck.begin(), deck.end(), 0);

	std::cout << "Target number is " << (int)target_number << " and aces are " << (aces_high ? "" : "not ") << "high.\n";
	
	std::array<size_t, max_hand_size> results;
	constexpr size_t trials = 10000000;

#pragma omp parallel for
	for (size_t hand_size = 1; hand_size < max_hand_size; hand_size++)
	{
		hand hand;
		size_t succeeded = 0;
		for (size_t trial_n = 0; trial_n < trials; trial_n++)
		{
			random_draw(deck.begin(), deck.end(), hand_size, hand);
			succeeded += is_success(hand.begin(), hand.begin() + hand_size);
		}
#ifdef NO_OMP
		std::cout << "You have a " << (double(succeeded)/trials) * 100 << " percent chance of success when drawing " << hand_size << (hand_size == 1 ? " card" : " cards") << ".\n";
#else
		results[hand_size] = succeeded;
#endif
	}
		
#ifndef NO_OMP
	for (size_t i = 1; i < max_hand_size; i++)
		std::cout << "You have a " << (double(results[i])/trials) * 100 << " percent chance of success when drawing " << i << (i == 1 ? " card" : " cards") << ".\n";
#endif

}
