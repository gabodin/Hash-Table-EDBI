#ifndef HASHTBL_H
#define HASHTBL_H

#include <string>
#include <initializer_list>
#include <forward_list>
#include <cmath>
#include <iostream>

#include <stdexcept>
using std::out_of_range;

namespace ac {
    
    // template < typename KeyType >
    // struct Account
    // {   
    //     using AcctKey = KeyType;

    //     AcctKey m_key;
    //     std::string client_name;
    //     size_t bank_code;
    //     size_t agency_number;
    //     size_t account_number;
    //     float balance;
        
    //     Account(AcctKey mk_, std::string c_ = default, size_t b_ = default, size_t agen_ = default, size_t acc_ = default, float b_ = 0.0f) : m_key(ak_), client_name(c_), bank_code(b_), agency_number(agen_), account_number(acc_), balance(b_)
    //     { /* Empty */ };
        
        
        
        
    // };

    template < class KeyType, class DataType >
    class HashEntry
    {
        public :
            HashEntry ( KeyType k_, DataType d_) : m_key( k_ ), m_data ( d_ )
            { /* Empty */ };

            KeyType m_key; //!< Stores the key for an entry .
            DataType m_data; //!< Stores the data for an entry .

    };

    template <typename KeyType, typename DataType, typename KeyHash = std::hash< KeyType >, typename KeyEqual = std::equal_to< KeyType > >
    class HashTbl
    {
        public:



            using Entry = HashEntry<KeyType, DataType>;

            HashTbl(size_t tbl_size_ = DEFAULT_SIZE) : m_size(smallest_prime(tbl_size_)), m_count(0)
            //! Default constructor;
            {
                this->m_data_table = new std::forward_list<Entry>[m_size];
            }
            virtual ~HashTbl()
            {
                for(size_t i(0); i < m_size; i++)
                {
                    m_data_table[i].clear();
                }
                delete[] m_data_table;
            }
            //! Destructor;
            HashTbl(const HashTbl& other) : m_size(other.m_size), m_count(other.m_count)
            {
                m_data_table = new std::forward_list<Entry>[m_size];

                for(size_t i(0); i < m_size; i++)
                {
                    m_data_table[i] = other.m_data_table[i];
                }
            }
            //! Constructs a Hash Table from another Hash Table; 
            HashTbl(std::initializer_list< Entry > ilist) : m_size(DEFAULT_SIZE), m_count(0) 
            {
                m_data_table = new std::forward_list<Entry>[m_size];

                auto itr = ilist.begin();
                for(size_t i(0); i < ilist.size(); i++)
                {
                    insert(itr->m_key, itr->m_data);
                    itr++;
                }
            }
            //! Constructs a Hash Table from an initializer list;

            HashTbl& operator=(const HashTbl &other)
            {
                delete m_data_table;

                m_size = other.m_size;
                m_count = other.m_count;
                m_data_table = new std::forward_list<Entry>[m_size];

                for(size_t i(0); i < m_size; i++)
                {
                    m_data_table[i] = other.m_data_table[i];
                }
                return *this;
            }
            HashTbl& operator=(std::initializer_list<Entry> ilist)
            {
                clear();

                auto itr = ilist.begin();
                for(size_t i(0); i < ilist.size(); i++)
                {
                    insert(itr->m_key, itr->m_data);
                    itr++;
                }
                return *this;
            }

            bool insert (const KeyType & k_, const DataType & d_)
            //! Insert the content of 'd_' associated with the key 'k_'. It returns true if 'k_' does not exist, false otherwise;
            {
                KeyHash hashFunc;
                KeyEqual equalFunc;

                Entry new_entry{k_, d_};

                size_t index{ hashFunc(k_) % m_size };

                auto it = m_data_table[index].begin();
                
                if(m_count > m_size) rehash();
                while(it != m_data_table[index].end())
                {
                    if(true == equalFunc(it->m_key, new_entry.m_key))
                    {
                        it->m_data = d_;
                        return false;
                    }
                    ++it;
                }
                m_data_table[index].push_front(new_entry);
                m_count++;

                return true;  
            }
            
            bool erase (const KeyType & k_)
            //! Erase the element associated with the key 'k_'. It returns true if 'k_' exists, false otherwise;
            {
                KeyHash hashFunc;
                KeyEqual equalFunc;

                size_t index{hashFunc(k_)%m_size};

                auto it = m_data_table[index].begin();
                auto prev = m_data_table[index].begin();
                
                while(it != m_data_table[index].end()) 
                {
                    if(true == equalFunc(it->m_key, k_))
                    {
                        if(it == m_data_table[index].begin()) m_data_table[index].erase_after(m_data_table[index].before_begin());
                        else m_data_table[index].erase_after(prev);
                        m_count--;
                        return true;
                    }
                    if(it != prev) prev = it;
                    ++it;     
                }
                return false;
            }
            bool retrieve (const KeyType & k_, DataType & d_)
             //! If 'k_' exists, 'd_' stores the data associated to 'k_' and the method returns true. Otherwise, it returns false;
            {
                KeyHash hashFunc;
                KeyEqual equalFunc;

                size_t index{hashFunc(k_)%m_size};

                auto it = m_data_table[index].begin(); 

                while(it != m_data_table[index].end()) 
                {
                    if(true == equalFunc(it->m_key, k_))
                    {
                        d_ = it->m_data;
                        return true;
                    }
                    ++it;     
                }
                return false;


            }
            void clear (void)
            //! Remove all the elements from the hash table;
            {
                for(size_t i(0); i < m_size; i++)
                {
                    m_data_table[i].clear();
                }
                    
            }

            bool empty (void) const{return !(m_count);}
            //! Returns true if the hash table is empty, false otherwise;
            size_t size(void) const{return m_count;}
            //! Returns the size of the hash table;

            DataType & at(const KeyType& k_)
            //! Returns a reference to the content associated with the key 'k_'. If 'k_' isn't in the hash table, it throws std::out_of_range
            {
                KeyHash hashFunc;
                KeyEqual hashEqual;
                
                size_t index{ hashFunc (k_) % m_size };

                auto it = m_data_table[index].begin();
                
                while(it != m_data_table[index].end()) 
                {
                    if(true == hashEqual(it->m_key, k_))
                    {
                        return it->m_data;
                    }
                    ++it;
                }
                
                throw std::out_of_range("You tried to access a key that does not exist.");
            }
            DataType & operator[](const KeyType& k_)
            //! Returns a reference to the content associated with the key 'k_'. If 'k_' isn't in the hash table, it inserts the key and returns the new data;
            {
                KeyHash hashFunc;
                KeyEqual hashEqual;
                
                size_t index{ hashFunc (k_) % m_size };

                auto it = m_data_table[index].begin();
                
                while(it != m_data_table[index].end()) 
                {
                    if(true == hashEqual(it->m_key, k_))
                    {
                        return it->m_data;
                    }
                    ++it;
                }

                insert(k_, DataType());
                return at(k_);
            }
            size_t count(const KeyType& k_) const
            //! Returns how many elements from the hash table are in the collision list associated with the key 'k_';
            {
                KeyHash hashFunc;
                
                size_t index{hashFunc(k_)%m_size};

                size_t aux(0);
                auto it = m_data_table[index].begin();

                while(it != m_data_table[index].end())
                {
                    it++;
                    aux++;
                }

                return aux;
            }
            
            friend std::ostream& operator<< (std::ostream & out, const HashTbl&)
            {
                //out << ;
                //return out;
            }
            //! It's a debugging method used only to generate a textual representation of the hash table and its elements;

        private:
            void rehash()
            //! It's a private method. It creates a new hash table, which the size will be equal to the smallest prime number greater then or equal to the double of the old size;
            {
                std::cout << "teste1\n";
                HashTbl old_tbl = *this;
                delete[] m_data_table;

                m_count = 0;
                m_size = smallest_prime(2*old_tbl.size());
                m_data_table = new std::forward_list<Entry>[m_size];

                for(size_t i(0); i < old_tbl.m_size; i++) 
                {
                    auto it = old_tbl.m_data_table[i].begin();
                    while(it != old_tbl.m_data_table[i].end())
                    {
                        insert(it->m_key, it->m_data);
                        ++it;
                    }
                    std::cout << "teste2\n";
                }
            }
            unsigned int m_size;
            //! It stores the size of the hash table;
            unsigned int m_count;
            //! Current element count;

            std::forward_list<Entry> * m_data_table;

            static const short DEFAULT_SIZE = 11;

        //Extra members:
        
        private:
            size_t smallest_prime(size_t x)
            {   
                size_t number = x;
                while(true)
                {                    
                    size_t div_counter = 0;

                    for(size_t i(1); i < sqrt(number); i++)
                    {
                        if(number % i == 0)
                        {
                            ++div_counter;                            
                        }
                        if(div_counter > 1) break;                        
                    }

                    if(div_counter == 1) return number;
                     else 
                     {
                        ++number;
                     }                    
                }
            }

    };
}

#endif