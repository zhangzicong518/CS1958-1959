#ifndef SJTU_LRU_HPP
#define SJTU_LRU_HPP

#include "utility.hpp"
#include "exceptions.hpp"
#include "class-integer.hpp"
#include "class-matrix.hpp"

class Hash
{
public:
	unsigned int operator()(Integer lhs) const
	{
		int val = lhs.val;
		return std::hash<int>()(val);
	}
};
class Equal
{
public:
	bool operator()(const Integer &lhs, const Integer &rhs) const
	{
		return lhs.val == rhs.val;
	}
};

static int prime[25] = {23, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319};
namespace sjtu
{
	template <
		class Key,
		class T,
		class Hash = std::hash<Key>,
		class Equal = std::equal_to<Key>>
	class hashmap
	{
	public:
		using value_type = pair<const Key, T>;
		int size, capacity;
		double LoadingFactor;
		struct node
		{
			value_type *val;
			node *pre, *nxt;
			node(value_type *_val = nullptr, node *_pre = nullptr, node *_nxt = nullptr) : val(_val), pre(_pre), nxt(_nxt) {}
			~node()
			{
				if (val != nullptr)
				{
					delete val;
					val = nullptr;
				}
				if (pre != nullptr)
					pre = nullptr;
				if (nxt != nullptr)
					nxt = nullptr;
			}
		} **table;

		int GetMod(int x)
		{
			int pos = 0;
			while (pos <= 25)
			{
				if (prime[pos] > x)
					return prime[pos];
				pos++;
			}
			return prime[20];
		}
		hashmap()
		{
			LoadingFactor = 0.9;
			size = 0;
			capacity = GetMod(0);
			table = new node *[capacity];
			for (int i = 0; i < capacity; i++)
				table[i] = nullptr;
		}
		hashmap(int _cap)
		{
			LoadingFactor = 0.9;
			size = 0;
			capacity = _cap;
			table = new node *[capacity];
			for (int i = 0; i < capacity; i++)
				table[i] = nullptr;
		}
		hashmap(const hashmap &other)
		{
			LoadingFactor = 0.9;
			size = 0;
			capacity = other.capacity;
			table = new node *[capacity];
			for (int i = 0; i < capacity; i++)
				table[i] = nullptr;
			for (int i = 0; i < other.capacity; i++)
			{
				node *now = other.table[i];
				while (now != nullptr)
				{
					insert(*(now->val));
					now = now->nxt;
				}
			}
		}
		~hashmap()
		{
			if (table != nullptr)
			{
				for (int i = 0; i < capacity; i++)
				{
					if (table[i] == nullptr)
						continue;
					node *now = table[i];
					while (now != nullptr)
					{
						node *tmp = now;
						now = now->nxt;
						delete tmp;
					}
					table[i] = nullptr;
				}
				delete[] table;
			}
			table = nullptr;
			size = capacity = 0;
			LoadingFactor = 0.9;
		}
		hashmap &operator=(const hashmap &other)
		{
			this->~hashmap();
			size = 0;
			capacity = other.capacity;
			LoadingFactor = 0.9;
			table = new node *[capacity];
			for (int i = 0; i < capacity; i++)
				table[i] = nullptr;
			for (int i = 0; i < other.capacity; i++)
			{
				node *now = other.table[i];
				while (now != nullptr)
				{
					insert(*(now->val));
					now = now->nxt;
				}
			}
			return *this;
		}

		class iterator
		{
		public:
			node *iter;
			iterator(node *pos = nullptr)
			{
				iter = pos;
			}
			iterator(const iterator &t)
			{
				iter = t.iter;
			}
			~iterator()
			{
				iter = nullptr;
			}
			value_type &operator*() const
			{
				if (iter == nullptr)
					throw("invalid");
				return *(iter->val);
			}
			value_type *operator->() const noexcept
			{
				if (iter == nullptr)
					throw("invalid");
				return iter->val;
			}
			bool operator==(const iterator &rhs) const
			{
				return iter == rhs.iter;
			}
			bool operator!=(const iterator &rhs) const
			{
				return iter != rhs.iter;
			}
		};

		void clear()
		{
			this->~hashmap();
			LoadingFactor = 0.9;
			size = 0;
			capacity = GetMod(0);
			table = new node *[capacity];
			for (int i = 0; i < capacity; i++)
				table[i] = nullptr;
		}

		void expand()
		{
			hashmap new_table(GetMod(capacity));
			for (int i = 0; i < capacity; i++)
			{
				node *tmp = table[i];
				while (tmp != nullptr)
				{
					new_table.insert(*(tmp->val));
					tmp = tmp->nxt;
				}
			}
			(*this) = new_table;
		}
		iterator end() const
		{
			return iterator(nullptr);
		}
		iterator find(const Key &key) const
		{
			int index = Hash()(key) % capacity;
			node *tmp = table[index];
			while (tmp != nullptr)
			{
				if (Equal()((*(tmp->val)).first, key))
					break;
				tmp = tmp->nxt;
			}
			return iterator(tmp);
		}
		pair<iterator, bool> insert(const value_type &value_pair)
		{
			iterator tar = find(value_pair.first);
			if (tar.iter != nullptr)
			{
				if (tar.iter->val != nullptr)
					delete tar.iter->val;
				tar.iter->val = new value_type(value_pair);
				return pair<iterator, bool>(tar, false);
			}
			int index = Hash()(value_pair.first) % capacity;
			node *tmp = table[index];
			table[index] = new node();
			table[index]->val = new value_type(value_pair);
			table[index]->nxt = tmp;
			if (tmp != nullptr)
				tmp->pre = table[index];
			size++;
			if (1.0 * size >= LoadingFactor * capacity)
				expand();
			return pair<iterator, bool>(iterator(table[index]), true);
		}

		bool remove(const Key &key)
		{
			iterator tar = find(key);
			if (tar.iter == nullptr)
				return false;
			node *now = tar.iter, *_pre = now->pre, *_nxt = now->nxt;
			if (_pre == nullptr) // table first
			{
				int index = Hash()(key) % capacity;
				if (_nxt != nullptr)
					_nxt->pre = nullptr;
				table[index] = _nxt;
			}
			else if (_nxt == nullptr)
				_pre->nxt = _nxt;
			else
			{
				_pre->nxt = _nxt;
				_nxt->pre = _pre;
			}
			size--;
			delete now;
			return true;
		}
	};

	template <
		class Key,
		class T,
		class Hash = std::hash<Key>,
		class Equal = std::equal_to<Key>>
	class linked_hashmap
	{
	public:
		typedef pair<const Key, T> value_type;
		struct node
		{
			node *pre, *nxt;
			value_type *val;
			node(node *_pre = nullptr, node *_nxt = nullptr, value_type *_val = nullptr) : pre(_pre), nxt(_nxt), val(_val) {}
			~node()
			{
				if (val != nullptr)
				{
					delete val;
					val = nullptr;
				}
				if (pre != nullptr)
					pre = nullptr;
				if (nxt != nullptr)
					nxt = nullptr;
			}
		};

		class double_list
		{
		public:
			node *head, *tail;
			size_t len;
			double_list()
			{
				len = 0;
				head = tail = nullptr;
			}
			double_list(const double_list &other)
			{
				len = 0;
				head = tail = nullptr;
				node *now = other.head->nxt;
				while (now != other.tail)
				{
					insert_tail(*(now->val));
					now = now->nxt;
				}
			}
			~double_list()
			{
				node *now = head, *tmp;
				while (now != nullptr)
				{
					tmp = now->nxt;
					delete now;
					now = tmp;
				}
				len = 0;
				head = nullptr;
				tail = nullptr;
			}

			void init(const value_type &val)
			{
				// std::cerr<<val.first<<val.second<<std::endl;
				value_type *tmp = new value_type(val);
				head = new node(nullptr, nullptr, tmp);
				tail = new node();
				head->nxt = tail;
				tail->pre = head;
			}

			class iterator
			{
			public:
				node *iter;
				iterator(node *pos = nullptr)
				{
					iter = pos;
				}
				iterator(const iterator &t)
				{
					iter = t.iter;
				}
				~iterator()
				{
					iter = nullptr;
				}
				iterator &operator=(const iterator &t)
				{
					iter = t.iter;
					return *this;
				}
				iterator &operator++()
				{
					if (iter->nxt == nullptr)
						throw("index_out_of_bound");
					iter = iter->nxt;
					return *this;
				}
				iterator &operator--()
				{
					if (iter->pre == nullptr || iter->pre->pre == nullptr)
						throw("index_out_of_bound");
					iter = iter->pre;
					return *this;
				}
				iterator operator++(int)
				{
					iterator tmp = *this;
					++(*this);
					return tmp;
				}
				iterator operator--(int)
				{
					iterator tmp = *this;
					--(*this);
					return tmp;
				}
				value_type &operator*() const
				{
					if (iter->val == nullptr)
						throw("invalid");
					return *(iter->val);
				}
				value_type *operator->() const noexcept
				{
					if (iter->val == nullptr)
						throw("invalid");
					return iter->val;
				}
				bool operator==(const iterator &rhs) const
				{
					return iter == rhs.iter;
				}
				bool operator!=(const iterator &rhs) const
				{
					return iter != rhs.iter;
				}
			};
			class const_iterator
			{
			public:
				const node *const_iter;
				const_iterator(node *pos = nullptr)
				{
					const_iter = pos;
				}
				const_iterator(const iterator &other)
				{
					const_iter = other.const_iter;
				}
				const_iterator &operator=(const iterator &other)
				{
					const_iter = other.iter;
					return *this;
				}
				const_iterator &operator++()
				{
					if (const_iter->nxt == nullptr)
						throw("index_out_of_bound");
					const_iter = const_iter->nxt;
					return *this;
				}
				const_iterator &operator--()
				{
					if (const_iter->pre == nullptr || const_iter->pre->pre == nullptr)
						throw("index_out_of_bound");
					const_iter = const_iter->pre;
					return *this;
				}
				const_iterator operator++(int)
				{
					const_iterator tmp = *this;
					++(*this);
					return tmp;
				}
				const_iterator operator--(int)
				{
					const_iterator tmp = *this;
					--(*this);
					return tmp;
				}

				const value_type &operator*() const
				{
					if (const_iter->val == nullptr)
						throw("star invalid");
					return *(const_iter->val);
				}
				const value_type *operator->() const noexcept
				{
					if (const_iter->val == nullptr)
						throw("star invalid");
					return *(const_iter->val);
				}
				bool operator==(const iterator &rhs) const
				{
					return const_iter == rhs.iter;
				}
				bool operator!=(const iterator &rhs) const
				{
					return const_iter != rhs.iter;
				}
				bool operator==(const const_iterator &rhs) const
				{
					return const_iter == rhs.const_iter;
				}
				bool operator!=(const const_iterator &rhs) const
				{
					return const_iter != rhs.const_iter;
				}
			};

			iterator begin()
			{
				return iterator(head->nxt);
			}
			iterator end()
			{
				return iterator(tail);
			}
			const_iterator cbegin() const
			{
				return const_iterator(head->nxt);
			}
			const_iterator cend() const
			{
				return const_iterator(tail);
			}

			iterator erase(iterator pos)
			{
				if (len == 0 || pos == end())
					return end();
				node *now = pos.iter, *_pre = pos.iter->pre, *_nxt = pos.iter->nxt;
				len--;
				_pre->nxt = _nxt;
				_nxt->pre = _pre;
				delete now;
				if (len == 0)
					clear();
				return iterator(_nxt);
			}

			void insert(iterator pos, const value_type &val)
			{
				node *tmp = new node();
				tmp->val = new value_type(val);
				len++;
				node *now = pos.iter;
				tmp->pre = now->pre;
				tmp->nxt = now;
				now->pre->nxt = tmp;
				now->pre = tmp;
			}

			void insert_head(const value_type &val)
			{
				// std::cerr<<len<<std::endl;
				if (len == 0 || head == nullptr)
				{
					init(val);
				}
				insert(begin(), val);
			}
			void insert_tail(const value_type &val)
			{
				// std::cerr<<len<<std::endl;
				if (len == 0 || head == nullptr)
				{
					init(val);
				}
				insert(end(), val);
			}
			void delete_head()
			{
				erase(begin());
			}
			void delete_tail()
			{
				erase(iterator(tail->pre));
			}
			bool empty()
			{
				return (len == 0);
			}

			void clear()
			{
				this->~double_list();
				double_list();
			}

			void print() const
			{
				node *now = head->nxt;
				while (now != tail)
				{
					std::cerr << (*(now->val)).second << " ";
					now = now->nxt;
				}
				std::cerr << std::endl;
			}
		};
		typedef typename double_list::iterator iterator;
		typedef typename double_list::const_iterator const_iterator;
		typedef typename hashmap<Key, iterator, Hash, Equal>::iterator map_iterator;
		double_list *List;
		hashmap<Key, iterator, Hash, Equal> *HashMap;
		size_t _size;

		linked_hashmap()
		{
			_size = 0;
			List = new double_list();
			HashMap = new hashmap<Key, iterator, Hash, Equal>();
		}
		~linked_hashmap()
		{
			_size = 0;
			if (HashMap != nullptr)
				delete HashMap;
			if (List != nullptr)
				delete List;
			HashMap = nullptr;
			List = nullptr;
		}
		linked_hashmap(const linked_hashmap &other)
		{
			_size = other._size;
			HashMap = new hashmap<Key, iterator, Hash, Equal>(*other.HashMap);
			List = new double_list(*other.List);
		}
		linked_hashmap &operator=(const linked_hashmap &other)
		{
			if (HashMap != nullptr)
				delete HashMap;
			if (List != nullptr)
				delete List;
			_size = other._size;
			HashMap = new hashmap<Key, iterator, Hash, Equal>(*other.HashMap);
			List = new double_list(*other.List);
			return *this;
		}

		T &at(const Key &key)
		{
			map_iterator it = HashMap->find(key);
			if (it.iter == nullptr)
				throw("invalid");
			iterator lit((*it).second);
			return (*(lit.iter->val)).second;
		}
		const T &at(const Key &key) const
		{
			map_iterator it = HashMap->find(key);
			if (it.iter == nullptr)
				throw("invalid");
			const_iterator lit((*it).second);
			return (*(lit.iter->val)).second;
		}
		T &operator[](const Key &key)
		{
			map_iterator it = HashMap->find(key);
			if (it.iter == nullptr)
				throw("invalid");
			iterator lit((*it).second);
			return (*(lit.iter->val)).second;
		}
		const T &operator[](const Key &key) const
		{
			map_iterator it = HashMap->find(key);
			if (it.iter == nullptr)
				throw("invalid");
			const_iterator lit((*it).second);
			return (*(lit.iter->val)).second;
		}

		iterator begin()
		{
			return List->begin();
		}
		const_iterator cbegin() const
		{
			return List->cbegin();
		}
		iterator end()
		{
			return List->end();
		}
		const_iterator cend() const
		{
			return List->cend();
		}

		bool empty() const
		{
			return (_size == 0);
		}

		void clear()
		{
			this->~linked_hashmap();
			_size = 0;
			List = new double_list();
			HashMap = new hashmap<Key, iterator, Hash, Equal>();
		}

		size_t size() const
		{
			return _size;
		}

		iterator find(const Key &key)
		{
			if (_size == 0)
				return end();
			map_iterator mit = HashMap->find(key);
			if (mit == HashMap->end())
				return end();
			iterator it((*mit).second);
			return it;
		}

		pair<iterator, bool> insert(const value_type &value)
		{
			map_iterator mit = HashMap->find(value.first);
			pair<iterator, bool> res;
			if (mit == HashMap->end())
			{
				List->insert_tail(value);
				HashMap->insert(pair<Key, iterator>(value.first, --List->end()));
				++_size;
				res.first = --List->end();
				res.second = true;
			}
			else
			{
				iterator tmp_it((*mit).second);
				List->erase(tmp_it);
				List->insert_tail(value);
				(*mit).second = --List->end();
				res.first = --List->end();
				res.second = false;
			}
			return res;
		}
		void remove(iterator pos)
		{
			if (pos.iter == nullptr)
				return;
			value_type val = *pos;
			List->erase(pos);
			HashMap->remove(val.first);
			_size--;
		}

		size_t count(const Key &key) const
		{
			map_iterator mit = HashMap->find(key);
			return (mit == HashMap->end()) ? 0 : 1;
		}
	};

	class lru
	{
		using lmap = sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>;
		using value_type = sjtu::pair<const Integer, Matrix<int>>;
		typedef typename lmap::double_list::iterator iterator;
		typedef typename hashmap<Integer, iterator, Hash, Equal>::iterator map_iterator;

	public:
		int capacity;
		lmap *LinkedHashMap;
		lru(int size)
		{
			capacity = size;
			LinkedHashMap = new lmap();
		}
		~lru()
		{
			capacity = 0;
			delete LinkedHashMap;
		}

		void save(const value_type &v) const
		{
			if (LinkedHashMap->HashMap->find(v.first) == LinkedHashMap->HashMap->end())
			{
				if (LinkedHashMap->size() == capacity)
				{
					LinkedHashMap->HashMap->remove((*(LinkedHashMap->List->begin())).first);
					LinkedHashMap->List->delete_head();
					LinkedHashMap->_size--;
				}
			}
			else
			{
				LinkedHashMap->remove(LinkedHashMap->find(v.first));
			}
			LinkedHashMap->List->insert_tail(v);
			LinkedHashMap->HashMap->insert(sjtu::pair<const Integer, iterator>(v.first, --LinkedHashMap->List->end()));
			LinkedHashMap->_size++;
		}

		Matrix<int> *get(const Integer &v) const
		{
			map_iterator mit = LinkedHashMap->HashMap->find(v);
			if (mit == LinkedHashMap->HashMap->end())
			{
				Matrix<int> *res = nullptr;
				return res;
			}
			iterator it = (*mit).second;
			LinkedHashMap->List->insert_tail(*it);
			LinkedHashMap->List->erase(it);
			(*mit).second = --LinkedHashMap->List->end();
			return &(*(it)).second;
		}

		void print()
		{
			iterator it;
			for (it = LinkedHashMap->List->begin(); it != LinkedHashMap->List->end(); it++)
				std::cout << (*it).first.val << " " << (*it).second << std::endl;
		}
	};
}

#endif
