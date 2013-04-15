/*
    Author: Lisheng Guan (guanlisheng@gmail.com) 
*/

template <class T>
class Singleton
{
public:
    static inline T& instance()
    {   
        static T _instance;
        return _instance;
    }   

private:
    Singleton(void);
    ~Singleton(void);
    Singleton(const Singleton<T>&);
    Singleton<T>& operator= (const Singleton<T> &); 
};
