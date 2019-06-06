#ifndef SINGLETON_H_
#define SINGLETON_H_

template < class type >
class Singleton
{
public:
	Singleton()
	{
		mSingleton = static_cast< type* >(this);
	}

	virtual ~Singleton()
	{
		mSingleton = 0;
	}

	__forceinline static type & getSingleton( ) { return *mSingleton; }
	__forceinline static type * getSingletonPtr( ) { return mSingleton; }

protected:
	static type* mSingleton;
};

template < class type >
type* Singleton< type >::mSingleton = 0;

#endif /* SINGLETON_H_ */