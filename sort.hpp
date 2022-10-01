#ifndef SORT_HPP
#define SORT_HPP
/*
* 8大排序加内省排序的简单实现
* 需要C++标准 >= 14
*/
#include <iterator>
#include <vector>
#include <algorithm>
#include <list>

#define SortFunc(FUNC, RETURNTYPE)\
template <class RandomAccessIterator>\
RETURNTYPE FUNC(RandomAccessIterator first, RandomAccessIterator last)\
{\
	using value_type = typename std::iterator_traits<RandomAccessIterator>::value_type;\
	FUNC(first, last, std::less<value_type>());\
}\
template<class T, size_t n>\
RETURNTYPE FUNC(T(&container)[n])\
{\
	FUNC(&container[0], &container[n], std::less<T>());\
}\
template <class IterableContainer>\
RETURNTYPE FUNC(IterableContainer& container)\
{\
	using value_type = typename std::iterator_traits<decltype(container.begin())>::value_type;\
	FUNC(container.begin(), container.end(), std::less<value_type>());\
}\
template<class T, size_t n, class Compare>\
RETURNTYPE FUNC(T(&container)[n], Compare comp)\
{\
	FUNC(&container[0], &container[n], comp);\
}\
template <class IterableContainer, class Compare>\
RETURNTYPE FUNC(IterableContainer& container, Compare comp)\
{\
FUNC(container.begin(), container.end(), comp); \
}\
template <class RandomAccessIterator, class Compare>\
RETURNTYPE FUNC(RandomAccessIterator first, RandomAccessIterator last, Compare comp)

#define StaticSortFunc(FUNC) SortFunc(FUNC, static constexpr void)

#define SortWithoutCompareFunc(FUNC, RETURNTYPE)\
template<class T, size_t n>\
RETURNTYPE FUNC(T(&container)[n])\
{\
	FUNC(&container[0], &container[n]);\
}\
template <class IterableContainer>\
RETURNTYPE FUNC(IterableContainer& container)\
{\
	FUNC(container.begin(), container.end());\
}\
template <class RandomAccessIterator>\
RETURNTYPE FUNC(RandomAccessIterator first, RandomAccessIterator last)

#define StaticSortWithoutCompareFunc(FUNC) SortWithoutCompareFunc(FUNC, static constexpr void)


class Sort
{
public:
	/*冒泡排序*/
	StaticSortFunc(BubbleSort)
	{
		using iter = RandomAccessIterator;
		if (last - first <= 1)
			return;

		bool NeedSort = true;
		for (iter i = first; i != last && NeedSort; ++i)
		{
			bool NeedSort = false;
			for (iter j = last - 1; j != i; --j)
			{
				if (comp(*j, *(j - 1)))
				{
					std::iter_swap(j, j - 1);
					NeedSort = true;
				}
			}
		}
	}


	/*插入排序*/
	StaticSortFunc(InsertionSort)
	{
		using iter = RandomAccessIterator;
		if (last - first <= 1)
			return;

		for (iter i = first + 1; i != last; ++i)
		{
			auto value = *i;
			iter j = i - 1;
			while (j >= first && comp(value, *j))
			{
				*(j + 1) = *j;
				--j;
			}
			*(j + 1) = value;
		}
	}


	/*选择排序*/
	StaticSortFunc(SelectionSort)
	{
		using iter = RandomAccessIterator;
		if (last - first <= 1)
			return;

		for (iter i = first; i != last - 1; ++i)
		{
			iter minj = i;
			for (iter j = i + 1; j != last; ++j)
				if (comp(*j, *minj))
					minj = j;
			std::iter_swap(i, minj);
		}
	}


	/*快速排序 三数取中*/
	StaticSortFunc(QuickSort)
	{
		using iter = RandomAccessIterator;
		if (last - first <= 1)
			return;

		auto pivot = median(*first, *(last - 1), *(first + (last - first) / 2));
		iter mid = partation(first, last, pivot, comp);
		QuickSort(first, mid, comp);
		QuickSort(mid, last, comp);
	}


	/*内省排序*/
	StaticSortFunc(IntroSort)
	{
		if (last - first <= 1)
			return;
		intro_sort(first, last, comp, 2 * log(last - first));
	}


	/*堆排序*/
	StaticSortFunc(HeapSort)
	{
		if (last - first <= 1)
			return;

		using iter = RandomAccessIterator;
		Heap<iter, Compare> h(first, last, comp);
		h.sort();
	}


	/*希尔排序*/
	StaticSortFunc(ShellSort)
	{
		using iter = RandomAccessIterator;
		if (last - first <= 1)
			return;

		/*希尔排序 Sedgewick增量*/
		constexpr size_t span[] =
		{
			1, 5, 19, 41, 109,
			209, 505, 929, 2161, 3905,
			8929, 16001, 36289, 64769, 146305,
			260609,587521,1045505,2354689,4188161,
			9427969, 16764929, 37730305, 67084289, 150958081,
			268386305, 603906049, 1073643521, 2415771649, 4294770689,
			9663381505, 17179475969, 38654115841, 68718690305, 154617643009,
			274876334081, 618472931329, 1099508482049, 2473896443905, 4398040219649,
			9895595212801, 17592173461505, 39582399725569, 70368719011841, 158329636651009,
			281474926379009, 633318622101505, 1125899806179329, 2533274639400961,
			4503599426043905, 10133098859593729, 18014398106828801,
			40532396042354689, 72057593232621569, 162129585377378305,
			288230374541099009, 648518343925432321, 1152921501385621505,
			2594073380533567489, 4611685999100035073, 4611686011984936961
		};

		size_t pos = std::upper_bound(span, span + 61, last - first) - span - 1;

		for (; pos != -1; --pos)
		{
			for (iter i = first + span[pos]; i < last; ++i)
			{
				auto temp = *i;
				iter j = i;
				for (; j >= first + span[pos]; j -= span[pos])
				{
					if (comp(temp, *(j - span[pos])))
						*j = *(j - span[pos]);
					else
						break;
				}
				*j = temp;
			}
		}
	}


	/*归并排序*/
	StaticSortFunc(MergeSort)
	{
		if (last - first <= 1)
			return;

		if (last - first < 32)
		{
			InsertionSort(first, last, comp);
			return;
		}
		using iter = RandomAccessIterator;
		iter mid = first + (last - first) / 2;
		MergeSort(first, mid, comp);
		MergeSort(mid, last, comp);
		merge(first, mid, last, comp);
	}


	/*基数排序*/
	StaticSortWithoutCompareFunc(RadixSort)
	{
		using iter = RandomAccessIterator;
		using value_type = std::remove_reference_t<decltype(*first)>;
		using list = std::list<value_type>;

		static_assert(std::is_unsigned<value_type>::value, "RadixSort can only sort unsigned integers!");


		constexpr size_t pow10[] =
		{
			1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000,
			10000000000, 100000000000, 1000000000000, 10000000000000, 100000000000000,
			1000000000000000, 10000000000000000, 100000000000000000, 1000000000000000000,
		};

		size_t length = last - first;
		if (length <= 1)
			return;

		auto max = *first;
		for (iter i = first + 1; i != last; ++i)
			if (max < *i)
				max = *i;

		int digits = 1;
		while (max / 10 != 0)
		{
			++digits;
			max = max / 10;
		}

		list bucket_list[10]{};

		for (int i = 1; i <= digits; ++i)
		{
			for (iter j = first; j != last; ++j)
			{
				auto radix = (*j / pow10[i - 1]) % 10;
				bucket_list[radix].push_back(*j);
			}
			iter pos = first - 1;
			for (int j = 0; j < 10; j++)
			{
				for (auto& value : bucket_list[j])
					*++pos = value;
				bucket_list[j].clear();
			}
		}
	}
private:
	/*快速排序的一次划分*/
	template <class RandomAccessIterator, class T, class Compare>
	static constexpr RandomAccessIterator partation(RandomAccessIterator first, RandomAccessIterator last, T pivot, Compare comp)
	{
		while (true)
		{
			while (comp(*first, pivot))
				++first;
			--last;
			while (comp(pivot, *last))
				--last;
			if (first >= last)
				return first;
			std::iter_swap(first, last);
			++first;
		}
	}


	/*堆排序用的堆*/
	template<class Iter, class Compare>
	class Heap
	{
		using value_type = typename std::iterator_traits<Iter>::value_type;
	public:
		constexpr Heap(Iter first, Iter last, Compare comp) :_first(first), _size(last - first), _comp(comp)
		{
			for (size_t LastParentPos = (last - first) / 2 - 1; LastParentPos != -1; --LastParentPos)
				down(LastParentPos);
		}

		constexpr void sort()
		{
			size_t tail = size();
			while (size() >= 2)
				index(--tail) = pop();
		}
	private:
		Iter _first;
		size_t _size;
		Compare _comp;

		static constexpr size_t NaN = -1;

		constexpr value_type& index(size_t pos) const
		{
			return *(_first + pos);
		}


		constexpr const value_type& top() const
		{
			return index(0);
		}

		constexpr value_type pop()
		{
			value_type ReturnVal = top();
			index(0) = index(size() - 1);
			--_size;
			down(0);
			return ReturnVal;
		}

		constexpr void push(const value_type& val)
		{
			++_size;
			index(size() - 1) = val;
			up(size() - 1);
		}

		constexpr bool empty() const
		{
			return _size == 0;
		}

		constexpr size_t size() const
		{
			return _size;
		}

		constexpr void up(size_t pos)
		{
			while (parent(pos) != NaN && _comp(index(parent(pos)), index(pos)))
			{
				std::swap(index(pos), index(parent(pos)));
				pos = parent(pos);
			}
		}

		constexpr void down(size_t pos)
		{
			size_t MinChild = 0;
			while ((MinChild = MinInstance(lchild(pos), rchild(pos))) != NaN && _comp(index(pos), index(MinChild)))
			{
				std::swap(index(pos), index(MinChild));
				pos = MinChild;
			}
		}

		constexpr size_t MinInstance(size_t pos1, size_t pos2)
		{
			if (pos1 >= size() && pos2 >= size())
				return NaN;
			else if (pos1 >= size() && pos2 < size())
				return pos2;
			else if (pos2 >= size() && pos1 < size())
				return pos1;
			else
				return _comp(index(pos2), index(pos1)) ?
				pos1 : pos2;
		}

		constexpr size_t lchild(size_t pos)
		{
			if (2 * pos + 1 < size())
				return 2 * pos + 1;
			return NaN;
		}

		constexpr size_t rchild(size_t pos)
		{
			if (2 * pos + 2 < size())
				return 2 * pos + 2;
			return NaN;
		}

		constexpr size_t parent(size_t pos)
		{
			if (pos != 0)
				return (pos - 1) / 2;
			return NaN;
		}
	};

	/*三数取中*/
	template <class T>
	static constexpr const T& median(const T& a, const T& b, const T& c)
	{
		if (a < b)
			if (b < c)
				return b;
			else if (a < c)
				return c;
			else
				return a;
		else if (a < c)
			return a;
		else if (b < c)
			return c;
		else
			return b;
	}

	/*内省排序控制分割恶化*/
	static constexpr size_t log(size_t n)
	{
		size_t k = 0;
		for (; n > 1; n >>= 1)
			++k;
		return k;
	}

	/*内省排序*/
	template <class RandomAccessIterator, class Compare>
	static constexpr void intro_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp, size_t deepth)
	{
		using iter = RandomAccessIterator;
		if (isSorted(first, last, comp))
			return;
		if (last - first <= 16)
			InsertionSort(first, last, comp);
		else if (deepth == 0)
			HeapSort(first, last, comp);
		else
		{
			auto pivot = median(*first, *(last - 1), *(first + (last - first) / 2));
			iter mid = partation(first, last, pivot, comp);
			intro_sort(first, mid, comp, deepth - 1);
			intro_sort(mid, last, comp, deepth - 1);
		}
	}

	/*归并排序合并*/
	template <class RandomAccessIterator, class Compare>
	static constexpr void merge(RandomAccessIterator first1, RandomAccessIterator mid, RandomAccessIterator last2, Compare comp)
	{
		if (first1 == mid || mid == last2)
			return;

		using iter = RandomAccessIterator;
		using value_type = typename std::iterator_traits<iter>::value_type;

		iter last1 = mid;
		iter first2 = mid;
		std::vector<value_type> buf(first1, last1);
		size_t i = 0;
		for (; i != buf.size() && first2 != last2; ++first1)
		{
			if (comp(buf[i], *first2))
				*first1 = buf[i++];
			else
				*first1 = *first2++;
		}
		while (i != buf.size())
			*first1++ = buf[i++];
		while (first2 != last2)
			*first1++ = *first2++;
	}
public:
	/*判断是否排序完成*/
	template <class RandomAccessIterator, class Compare>
	static constexpr bool isSorted(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		using iter = RandomAccessIterator;
		if (last - first <= 1)
			return true;
		for (iter i = first + 1; i != last; ++i)
			if (comp(*i, *(i - 1)))
				return false;
		return true;
	}
};


#undef SortFunc
#undef StaticSortFunc
#undef SortWithoutCompareFunc
#undef StaticSortWithoutCompareFunc


#endif // SORT_HPP