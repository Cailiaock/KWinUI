/*
 *	KDListT.h --- a double linked list container
 *	Guoqiang Chen,2008
 *	03/04/2008(dd/mm/yyyy)
 *
 */
#ifndef __KDLIST_H__
#define __KDLIST_H__

template <typename T>
class KDNodeT{
public:
	T Element;
	KDNodeT<T>* prev;
	KDNodeT<T>* next;
	KDNodeT():prev(0),next(0){}
	KDNodeT(const T& element, KDNodeT<T>* p=0, KDNodeT<T>* n=0):Element(element),prev(p),next(n){}
};

template<typename T>
class KDListT;

template<typename T>
class _iterator {
public:
	typedef _iterator<T> _self;
	_iterator(){_M_node=0;}
	_self& operator++(){ 	//front
		incr();
		return *this;
	}
	_self operator++(int){	//after
		_self __tmp=*this;
		incr();
		return __tmp;
	}
	_self& operator--(){
		decr();
		return *this;
	}
	_self operator--(int){
		_self __tmp=*this;
		decr();
		return __tmp;
	}
	bool operator==(const _iterator& RHS) const {
		return _M_node=RHS._M_node;
	}
	bool operator!=(const _iterator& RHS) const {
		return _M_node!=RHS._M_node;
	}
	T& operator*(){
		return _M_node->Element;
	}
  	T* operator->(){return &(operator*());}	

private:
	void incr(){_M_node=_M_node->next;}
  	void decr(){_M_node=_M_node->prev;}
	KDNodeT<T>* _M_node;

	_iterator(KDNodeT<T>* p) {
		_M_node=p;
	}
	friend class KDListT<T>; // to use private constructor
};

template <typename T>
class KDListT{
public:
	typedef _iterator<T> iterator;

	KDListT(){ //ctor,make empty list
		_M_node=new KDNodeT<T>();
		_M_node->prev=_M_node;
		_M_node->next=_M_node;
	}
	~KDListT(){ // deallocate all nodes
		clear();
		delete _M_node;
	}
	bool empty(){
		 return _M_node->next==_M_node;
	}
	void clear(){
		KDNodeT<T>* cur=_M_node->next;
		while(cur!=_M_node){
			KDNodeT<T>* tmp=cur;
			cur=cur->next;
			delete tmp;
		}
		_M_node->next = _M_node;
  		_M_node->prev = _M_node;
	}
	KDNodeT<T>& front(){
		return _M_node->next;
	}
	KDNodeT<T>& back(){
		return _M_node->prev;
	}
	iterator begin(){
		return _M_node->next;
	}
	iterator end(){ 
		return _M_node;
	}
	iterator rbegin(){
		return _M_node->prev;
	}
	iterator rend(){
		return _M_node;
	}
	iterator insert(iterator It, const T& elem){	//insert before
		KDNodeT<T>* __tmp=new KDNodeT<T>(elem);
		__tmp->next=It._M_node;
		__tmp->prev=It._M_node->prev;
		It._M_node->prev->next=__tmp;
		It._M_node->prev=__tmp;
		return __tmp;
	}
	iterator erase(iterator pos){
		KDNodeT<T>* next_node=pos._M_node->next;
		KDNodeT<T>* prev_node=pos._M_node->prev;

		prev_node->next=next_node;
		next_node->prev=prev_node;
		delete pos._M_node;
		return next_node;
	}
	void push_back(T& elem){insert(end(),elem);}
	void pop_back(){iterator tmp=end();erase(--tmp);}

	void push_front(T& elem){insert(begin(),elem);}
	void pop_front(){erase(begin());}

private:
	KDNodeT<T>* _M_node; //node for management 
};

#endif //__KDLIST_H__
